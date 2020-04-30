/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <dpu.h>

#include <dpu_attributes.h>
#include <dpu_config.h>
#include <dpu_description.h>
#include <dpu_loader.h>
#include <dpu_management.h>
#include <dpu_mask.h>
#include <dpu_memory.h>
#include <dpu_rank.h>
#include <dpu_ufi_types.h>
#include <static_verbose.h>
#include <dpu_internals.h>
#include <dpu_log_utils.h>
#include <dpu_program.h>

static dpu_error_t
dpu_load_rank(struct dpu_rank_t *rank, struct dpu_program_t *program, dpu_elf_file_t elf_info);
static dpu_error_t
dpu_load_dpu(struct dpu_t *dpu, struct dpu_program_t *program, dpu_elf_file_t elf_info);

static dpu_error_t
dpu_boot_rank(struct dpu_rank_t *rank, dpu_launch_policy_t policy);
static dpu_error_t
dpu_boot_dpu(struct dpu_t *dpu, dpu_launch_policy_t policy);
static dpu_error_t
run_all_one_loop_iteration(struct dpu_rank_t *rank,
    bool *must_stop,
    dpu_bitfield_t *dpu_is_running,
    dpu_bitfield_t *dpu_is_in_fault);
static dpu_error_t
run_dpu_one_loop_iteration(struct dpu_t *dpu, bool *must_stop);
static dpu_error_t
do_generic_run_all(struct dpu_rank_t *rank);
static dpu_error_t
do_simple_run_dpu(struct dpu_t *dpu);
static dpu_error_t
do_run_all(struct dpu_rank_t *rank);
static dpu_error_t
do_run_dpu(struct dpu_t *dpu);
static void *
do_run_all_thread(struct dpu_rank_t *rank);
static void *
do_run_dpu_thread(struct dpu_t *dpu);
static dpu_error_t
dpu_status_rank(struct dpu_rank_t *rank, bool *done, bool *fault);
static dpu_error_t
dpu_status_dpu(struct dpu_t *dpu, bool *done, bool *fault);

static dpu_error_t
dpu_copy_from_dpu(struct dpu_t *dpu, const char *symbol_name, uint32_t symbol_offset, void *dst, size_t length);

static dpu_error_t
dpu_set_transfer_buffer_safe(struct dpu_t *dpu, void *buffer);

static dpu_error_t
dpu_get_common_program(struct dpu_set_t *set, struct dpu_program_t **program);

static struct verbose_control *this_vc;
static inline struct verbose_control *
__vc()
{
    if (this_vc == NULL) {
        this_vc = get_verbose_control_for("api");
    }
    return this_vc;
}

static const char *
dpu_launch_policy_to_string(dpu_launch_policy_t policy)
{
    switch (policy) {
        case DPU_ASYNCHRONOUS:
            return "ASYNCHRONOUS";
        case DPU_SYNCHRONOUS:
            return "SYNCHRONOUS";
        default:
            return "UNKNOWN";
    }
}

static const char *
dpu_transfer_to_string(dpu_xfer_t transfer)
{
    switch (transfer) {
        case DPU_XFER_TO_DPU:
            return "HOST_TO_DPU";
        case DPU_XFER_FROM_DPU:
            return "DPU_TO_HOST";
        default:
            return "UNKNOWN";
    }
}

static const char *
dpu_transfer_flags_to_string(dpu_xfer_flags_t flags)
{
    switch (flags) {
        case DPU_XFER_DEFAULT:
            return "DEFAULT";
        case DPU_XFER_NO_RESET:
            return "NO_RESET";
        default:
            return "UNKNOWN";
    }
}

static pthread_mutex_t set_allocator_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct dpu_set_t *set_allocator_sets = NULL;
static uint32_t set_allocator_size = 0;
static uint32_t set_allocator_capacity = 0;

static void __attribute__((destructor, used)) set_allocator_destructor() { free(set_allocator_sets); }

static dpu_error_t
set_allocator_register(struct dpu_set_t *set)
{
    dpu_error_t status = DPU_OK;

    pthread_mutex_lock(&set_allocator_mutex);

    if (set_allocator_size == set_allocator_capacity) {
        set_allocator_capacity = 2 * set_allocator_capacity + 2;
        struct dpu_set_t *new_sets;
        if ((new_sets = realloc(set_allocator_sets, set_allocator_capacity * sizeof(*set_allocator_sets))) == NULL) {
            status = DPU_ERR_SYSTEM;
            goto unlock_mutex;
        }
        set_allocator_sets = new_sets;
    }

    memcpy(set_allocator_sets + set_allocator_size, set, sizeof(*set));

    set_allocator_size++;
unlock_mutex:
    pthread_mutex_unlock(&set_allocator_mutex);
    return status;
}

static struct dpu_set_t *
set_allocator_find(struct dpu_set_t *set)
{
    for (uint32_t each_set = 0; each_set < set_allocator_size; ++each_set) {
        struct dpu_set_t *allocated_set = set_allocator_sets + each_set;
        if (memcmp(set, allocated_set, sizeof(*set)) == 0) {
            return allocated_set;
        }
    }

    return NULL;
}

static dpu_error_t
set_allocator_unregister(struct dpu_set_t *set)
{
    dpu_error_t status = DPU_OK;
    struct dpu_set_t *allocated_set;

    pthread_mutex_lock(&set_allocator_mutex);

    if ((allocated_set = set_allocator_find(set)) == NULL) {
        status = DPU_ERR_INVALID_DPU_SET;
        goto unlock_mutex;
    }

    // todo: this is probably not the more efficient method
    if (allocated_set != (set_allocator_sets + set_allocator_size - 1)) {
        memmove(allocated_set, allocated_set + 1, set_allocator_sets + set_allocator_size - (allocated_set + 1));
    }
    set_allocator_size--;
unlock_mutex:
    pthread_mutex_unlock(&set_allocator_mutex);
    return status;
}

static dpu_description_t
get_set_description(struct dpu_set_t *set)
{
    struct dpu_rank_t *rank = NULL;

    switch (set->kind) {
        case DPU_SET_RANKS:
            rank = set->list.ranks[0];
            break;
        case DPU_SET_DPU:
            rank = set->dpu->rank;
            break;
        default:
            return NULL;
    }

    return dpu_get_description(rank);
}

static uint32_t
get_nr_of_dpus_in_rank(struct dpu_rank_t *rank)
{
    dpu_description_t description = dpu_get_description(rank);

    uint8_t nr_cis = description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;

    uint32_t count = 0;

    for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        if (rank->runtime.control_interface.slice_info[each_ci].all_dpus_are_enabled) {
            count += nr_dpus_per_ci;
        } else {
            count += dpu_mask_count(rank->runtime.control_interface.slice_info[each_ci].enabled_dpus);
        }
    }

    return count;
}

__API_SYMBOL__ struct dpu_set_rank_iterator_t
dpu_set_rank_iterator_from(struct dpu_set_t *set)
{
    struct dpu_set_t first;
    bool has_next;

    switch (set->kind) {
        case DPU_SET_RANKS:
            has_next = set->list.nr_ranks != 0;
            first = *set;
            first.list.nr_ranks = 1;
            break;
        case DPU_SET_DPU:
            has_next = true;
            first = *set;
            break;
        default:
            has_next = false;
            first = *set;
            break;
    }

    struct dpu_set_rank_iterator_t iterator = { .set = set, .count = 0, .next_idx = 1, .has_next = has_next, .next = first };

    return iterator;
}

__API_SYMBOL__ void
dpu_set_rank_iterator_next(struct dpu_set_rank_iterator_t *iterator)
{
    iterator->count++;

    if (!iterator->has_next) {
        return;
    }

    switch (iterator->set->kind) {
        case DPU_SET_RANKS:
            iterator->has_next = iterator->next_idx < iterator->set->list.nr_ranks;

            if (iterator->has_next) {
                iterator->next.list.ranks = iterator->set->list.ranks + iterator->next_idx;
                iterator->next_idx++;
            }
            break;
        case DPU_SET_DPU:
            iterator->has_next = false;
            break;
        default:
            iterator->has_next = false;
            break;
    }
}

static void
advance_to_next_dpu_in_rank_list(struct dpu_set_dpu_iterator_t *iterator)
{
    struct dpu_rank_t *rank = *iterator->rank_iterator.next.list.ranks;
    uint32_t dpu_idx = iterator->next_idx;

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint32_t nr_dpus = nr_cis * nr_dpus_per_ci;

    do {
        for (; dpu_idx < nr_dpus; ++dpu_idx) {
            struct dpu_t *dpu = rank->dpus + dpu_idx;

            if (dpu->enabled) {
                iterator->has_next = true;
                iterator->next_idx = dpu_idx + 1;
                iterator->next.dpu = dpu;
                return;
            }
        }

        dpu_set_rank_iterator_next(&iterator->rank_iterator);
        rank = *iterator->rank_iterator.next.list.ranks;
        dpu_idx = 0;
    } while (iterator->rank_iterator.has_next);

    iterator->has_next = false;
}

__API_SYMBOL__ struct dpu_set_dpu_iterator_t
dpu_set_dpu_iterator_from(struct dpu_set_t *set)
{
    struct dpu_set_dpu_iterator_t iterator;
    iterator.rank_iterator = dpu_set_rank_iterator_from(set);
    iterator.count = 0;

    if (!iterator.rank_iterator.has_next) {
        iterator.has_next = false;
    } else {
        switch (set->kind) {
            case DPU_SET_RANKS:
                iterator.next.kind = DPU_SET_DPU;
                iterator.next_idx = 0;
                advance_to_next_dpu_in_rank_list(&iterator);
                break;
            case DPU_SET_DPU:
                iterator.has_next = true;
                iterator.next = *set;
                break;
            default:
                iterator.has_next = false;
                break;
        }
    }

    return iterator;
}

__API_SYMBOL__ void
dpu_set_dpu_iterator_next(struct dpu_set_dpu_iterator_t *iterator)
{
    iterator->count++;

    if (!iterator->has_next) {
        return;
    }

    switch (iterator->rank_iterator.set->kind) {
        case DPU_SET_RANKS: {
            advance_to_next_dpu_in_rank_list(iterator);
            break;
        }
        case DPU_SET_DPU:
            iterator->has_next = false;
            break;
        default:
            iterator->has_next = false;
            break;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_alloc(uint32_t nr_dpus, const char *profile, struct dpu_set_t *dpu_set)
{

    LOG_FN(VERBOSE, "%d, \"%s\"", nr_dpus, profile);

    struct dpu_rank_t **current_ranks = NULL;
    uint32_t capacity = 0;
    uint32_t current_nr_of_dpus = 0;
    uint32_t current_nr_of_ranks = 0;
    dpu_error_t status;

    while (current_nr_of_dpus < nr_dpus) {
        if (current_nr_of_ranks == capacity) {
            capacity = 2 * capacity + 2;

            struct dpu_rank_t **current_ranks_tmp;
            if ((current_ranks_tmp = realloc(current_ranks, capacity * sizeof(*current_ranks))) == NULL) {
                status = DPU_ERR_SYSTEM;
                goto free_ranks;
            }
            current_ranks = current_ranks_tmp;
        }

        struct dpu_rank_t **next_rank = current_ranks + current_nr_of_ranks;
        if ((status = dpu_get_rank_of_type(profile, next_rank)) != DPU_OK) {
            if ((status == DPU_ERR_ALLOCATION) && (nr_dpus == DPU_ALLOCATE_ALL) && (current_nr_of_ranks != 0)) {
                goto reset_ranks;
            }

            goto free_ranks;
        }

        current_nr_of_dpus += get_nr_of_dpus_in_rank(*next_rank);
        current_nr_of_ranks++;
    }

    if (current_nr_of_dpus != nr_dpus) {
        // todo #110
        struct dpu_rank_t *last_rank = current_ranks[current_nr_of_ranks - 1];

        for (int8_t each_dpu = last_rank->description->topology.nr_of_dpus_per_control_interface - 1; each_dpu >= 0; --each_dpu) {
            for (int8_t each_ci = last_rank->description->topology.nr_of_control_interfaces - 1; each_ci >= 0; --each_ci) {
                dpu_selected_mask_t enabled_dpus = last_rank->runtime.control_interface.slice_info[each_ci].enabled_dpus;

                if (dpu_mask_is_selected(enabled_dpus, each_dpu)) {
                    last_rank->runtime.control_interface.slice_info[each_ci].enabled_dpus
                        = dpu_mask_unselect(enabled_dpus, each_dpu);
                    last_rank->runtime.control_interface.slice_info[each_ci].all_dpus_are_enabled = false;
                    dpu_get(last_rank, each_ci, each_dpu)->enabled = false;

                    current_nr_of_dpus--;

                    if (current_nr_of_dpus == nr_dpus) {
                        goto reset_ranks;
                    }
                }
            }
        }
    }

reset_ranks:
    for (uint32_t each_rank = 0; each_rank < current_nr_of_ranks; ++each_rank) {
        if ((status = dpu_reset_rank(current_ranks[each_rank])) != DPU_OK) {
            goto free_ranks;
        }
    }

    // Making sure that the whole structure is initialized
    // (in particular, we are using memcmp in set_allocator_find)
    memset(dpu_set, 0, sizeof(*dpu_set));
    dpu_set->kind = DPU_SET_RANKS;
    dpu_set->list.nr_ranks = current_nr_of_ranks;
    dpu_set->list.ranks = current_ranks;

    if ((status = set_allocator_register(dpu_set)) != DPU_OK) {
        goto free_ranks;
    }

    return DPU_OK;

free_ranks:
    for (unsigned int each_allocated_rank = 0; each_allocated_rank < current_nr_of_ranks; ++each_allocated_rank) {
        dpu_free_rank(current_ranks[each_allocated_rank]);
    }
    if (current_ranks != NULL) {
        free(current_ranks);
    }
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_free(struct dpu_set_t dpu_set)
{
    LOG_FN(VERBOSE, "");

    dpu_error_t status, ret;

    if ((status = set_allocator_unregister(&dpu_set)) != DPU_OK) {
        return status;
    }

    // Allocated set are always a DPU_SET_RANKS
    for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
        if ((ret = dpu_free_rank(dpu_set.list.ranks[each_rank])) != DPU_OK) {
            status = ret;
        }
    }

    free(dpu_set.list.ranks);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_nr_ranks(struct dpu_set_t dpu_set, uint32_t *nr_ranks)
{
    LOG_FN(VERBOSE, "");

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            *nr_ranks = dpu_set.list.nr_ranks;
            break;
        case DPU_SET_DPU:
            *nr_ranks = 1;
            break;
        default:
            return DPU_ERR_INTERNAL;
    }

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_get_nr_dpus(struct dpu_set_t dpu_set, uint32_t *nr_dpus)
{
    LOG_FN(VERBOSE, "");

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            *nr_dpus = 0;

            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                *nr_dpus += get_nr_of_dpus_in_rank(dpu_set.list.ranks[each_rank]);
            }

            break;
        case DPU_SET_DPU:
            *nr_dpus = 1;
            break;
        default:
            return DPU_ERR_INTERNAL;
    }

    return DPU_OK;
}

typedef dpu_error_t (*load_elf_program_fct_t)(dpu_elf_file_t *elf_info,
    const char *path,
    uint8_t *buffer,
    size_t buffer_size,
    struct dpu_program_t *program,
    mram_size_t mram_size_hint);

static dpu_error_t
dpu_load_generic(struct dpu_set_t dpu_set,
    const char *path,
    uint8_t *buffer,
    size_t buffer_size,
    struct dpu_program_t **program,
    load_elf_program_fct_t load_elf_program)
{
    dpu_error_t status;
    dpu_elf_file_t elf_info;
    struct dpu_program_t *runtime;

    if ((runtime = malloc(sizeof(*runtime))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }
    dpu_init_program_ref(runtime);

    dpu_description_t description = get_set_description(&dpu_set);

    if ((status = load_elf_program(&elf_info, path, buffer, buffer_size, runtime, description->memories.mram_size)) != DPU_OK) {
        free(runtime);
        goto end;
    }

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                if ((status = dpu_load_rank(dpu_set.list.ranks[each_rank], runtime, elf_info)) != DPU_OK) {
                    goto free_runtime;
                }
            }
            break;
        case DPU_SET_DPU:
            if ((status = dpu_load_dpu(dpu_set.dpu, runtime, elf_info)) != DPU_OK) {
                goto free_runtime;
            }
            break;
        default:
            status = DPU_ERR_INTERNAL;
            goto free_runtime;
    }

    if (program != NULL) {
        *program = runtime;
    }
    goto close_elf;

free_runtime:
    runtime->reference_count = 1;
    dpu_free_program(runtime);
close_elf:
    dpu_elf_close(elf_info);
end:
    return status;
}

static dpu_error_t
__dpu_load_elf_program_from_incbin(dpu_elf_file_t *elf_info,
    __attribute__((unused)) const char *path,
    uint8_t *buffer,
    size_t buffer_size,
    struct dpu_program_t *program,
    mram_size_t mram_size_hint)
{
    return dpu_load_elf_program_from_memory(elf_info, buffer, buffer_size, program, mram_size_hint);
}

__API_SYMBOL__ dpu_error_t
dpu_load_from_memory(struct dpu_set_t dpu_set, uint8_t *buffer, size_t buffer_size, struct dpu_program_t **program)
{
    LOG_FN(VERBOSE, "%p %lu", buffer, buffer_size);

    return dpu_load_generic(dpu_set, NULL, buffer, buffer_size, program, __dpu_load_elf_program_from_incbin);
}

static dpu_error_t
__dpu_load_elf_program(dpu_elf_file_t *elf_info,
    const char *path,
    __attribute__((unused)) uint8_t *buffer,
    __attribute__((unused)) size_t buffer_size,
    struct dpu_program_t *program,
    mram_size_t mram_size_hint)
{
    return dpu_load_elf_program(elf_info, path, program, mram_size_hint);
}

__API_SYMBOL__ dpu_error_t
dpu_load(struct dpu_set_t dpu_set, const char *binary_path, struct dpu_program_t **program)
{
    LOG_FN(VERBOSE, "\"%s\"", binary_path);

    return dpu_load_generic(dpu_set, binary_path, NULL, 0, program, __dpu_load_elf_program);
}

__API_SYMBOL__ dpu_error_t
dpu_get_symbol(struct dpu_program_t *program, const char *symbol_name, struct dpu_symbol_t *symbol)
{
    LOG_FN(VERBOSE, "\"%s\"", symbol_name);

    dpu_error_t status = DPU_OK;

    uint32_t nr_symbols = program->symbols->nr_symbols;

    for (uint32_t each_symbol = 0; each_symbol < nr_symbols; ++each_symbol) {
        dpu_elf_symbol_t *elf_symbol = program->symbols->map + each_symbol;
        if (strcmp(symbol_name, elf_symbol->name) == 0) {
            symbol->address = elf_symbol->value;
            symbol->size = elf_symbol->size;
            goto end;
        }
    }

    status = DPU_ERR_UNKNOWN_SYMBOL;

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_launch(struct dpu_set_t dpu_set, dpu_launch_policy_t policy)
{
    LOG_FN(VERBOSE, "%s", dpu_launch_policy_to_string(policy));

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                dpu_error_t status;

                if ((status = dpu_boot_rank(dpu_set.list.ranks[each_rank], DPU_ASYNCHRONOUS)) != DPU_OK) {
                    return status;
                }
            }
            if (policy == DPU_SYNCHRONOUS) {
                return dpu_sync(dpu_set);
            }
            return DPU_OK;
        case DPU_SET_DPU:
            return dpu_boot_dpu(dpu_set.dpu, policy);
        default:
            return DPU_ERR_INTERNAL;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_status(struct dpu_set_t dpu_set, bool *done, bool *fault)
{
    LOG_FN(VERBOSE, "");

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            *done = true;
            *fault = false;

            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                dpu_error_t status;
                bool rank_done;
                bool rank_fault;

                if ((status = dpu_status_rank(dpu_set.list.ranks[each_rank], &rank_done, &rank_fault)) != DPU_OK) {
                    return status;
                }

                *done = *done && rank_done;
                *fault = *fault || rank_fault;
            }

            return DPU_OK;
        case DPU_SET_DPU:
            return dpu_status_dpu(dpu_set.dpu, done, fault);
        default:
            return DPU_ERR_INTERNAL;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_sync(struct dpu_set_t dpu_set)
{
    LOG_FN(VERBOSE, "");

    dpu_error_t status;
    bool fault;
    bool done;

    do {
        if ((status = dpu_status(dpu_set, &done, &fault)) != DPU_OK) {
            return status;
        }

        // todo: add a sleep here to reduce the CPU load?
    } while (!done);

    return fault ? DPU_ERR_DPU_FAULT : DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_copy_to(struct dpu_set_t dpu_set, const char *symbol_name, uint32_t symbol_offset, const void *src, size_t length)
{
    LOG_FN(VERBOSE, "\"%s\", %d, %p, %zd)", symbol_name, symbol_offset, src, length);

    dpu_error_t status;
    struct dpu_program_t *program;
    struct dpu_symbol_t symbol;

    if ((status = dpu_get_common_program(&dpu_set, &program)) != DPU_OK) {
        return status;
    }

    if ((status = dpu_get_symbol(program, symbol_name, &symbol)) != DPU_OK) {
        return status;
    }

    return dpu_copy_to_symbol(dpu_set, symbol, symbol_offset, src, length);
}

__API_SYMBOL__ dpu_error_t
dpu_copy_from(struct dpu_set_t dpu_set, const char *symbol_name, uint32_t symbol_offset, void *dst, size_t length)
{
    LOG_FN(VERBOSE, "\"%s\", %d, %p, %zd)", symbol_name, symbol_offset, dst, length);

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            return DPU_ERR_INVALID_DPU_SET;
        case DPU_SET_DPU:
            return dpu_copy_from_dpu(dpu_set.dpu, symbol_name, symbol_offset, dst, length);
        default:
            return DPU_ERR_INTERNAL;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_copy_to_symbol(struct dpu_set_t dpu_set, struct dpu_symbol_t symbol, uint32_t symbol_offset, const void *src, size_t length)
{
    LOG_FN(VERBOSE, "0x%08x, %d, %d, %p, %zd)", symbol.address, symbol.size, symbol_offset, src, length);

    dpu_error_t status;

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                if ((status = dpu_copy_to_symbol_rank(dpu_set.list.ranks[each_rank], symbol, symbol_offset, src, length))
                    != DPU_OK) {
                    return status;
                }
            }

            return DPU_OK;
        case DPU_SET_DPU:
            return dpu_copy_to_symbol_dpu(dpu_set.dpu, symbol, symbol_offset, src, length);
        default:
            return DPU_ERR_INTERNAL;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_copy_from_symbol(struct dpu_set_t dpu_set, struct dpu_symbol_t symbol, uint32_t symbol_offset, void *dst, size_t length)
{
    LOG_FN(VERBOSE, "0x%08x, %d, %d, %p, %zd)", symbol.address, symbol.size, symbol_offset, dst, length);

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            return DPU_ERR_INVALID_DPU_SET;
        case DPU_SET_DPU:
            return dpu_copy_from_symbol_dpu(dpu_set.dpu, symbol, symbol_offset, dst, length);
        default:
            return DPU_ERR_INTERNAL;
    }
}

__API_SYMBOL__ dpu_error_t
dpu_prepare_xfer(struct dpu_set_t dpu_set, void *buffer)
{
    LOG_FN(VERBOSE, "%p", buffer);

    dpu_error_t status = DPU_OK;

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                struct dpu_rank_t *rank = dpu_set.list.ranks[each_rank];
                uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
                uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

                for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
                    for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                        struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                        if (!dpu_is_enabled(dpu)) {
                            continue;
                        }

                        dpu_error_t buffer_status;
                        if ((buffer_status = dpu_set_transfer_buffer_safe(dpu, buffer)) != DPU_OK) {
                            status = buffer_status;
                        }
                    }
                }
            }

            break;
        case DPU_SET_DPU: {
            struct dpu_t *dpu = dpu_set.dpu;

            if (!dpu_is_enabled(dpu)) {
                return DPU_ERR_DPU_DISABLED;
            }

            if ((status = dpu_set_transfer_buffer_safe(dpu, buffer)) != DPU_OK) {
                return status;
            }

            break;
        }
        default:
            return DPU_ERR_INTERNAL;
    }

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_push_xfer(struct dpu_set_t dpu_set,
    dpu_xfer_t xfer,
    const char *symbol_name,
    uint32_t symbol_offset,
    size_t length,
    dpu_xfer_flags_t flags)
{
    LOG_FN(VERBOSE,
        "%s, %s, %d, %zd, %s",
        dpu_transfer_to_string(xfer),
        symbol_name,
        symbol_offset,
        length,
        dpu_transfer_flags_to_string(flags));

    dpu_error_t status;
    struct dpu_program_t *program;
    struct dpu_symbol_t symbol;

    if ((status = dpu_get_common_program(&dpu_set, &program)) != DPU_OK) {
        return status;
    }

    if ((status = dpu_get_symbol(program, symbol_name, &symbol)) != DPU_OK) {
        return status;
    }

    return dpu_push_xfer_symbol(dpu_set, xfer, symbol, symbol_offset, length, flags);
}

__API_SYMBOL__ dpu_error_t
dpu_push_xfer_symbol(struct dpu_set_t dpu_set,
    dpu_xfer_t xfer,
    struct dpu_symbol_t symbol,
    uint32_t symbol_offset,
    size_t length,
    dpu_xfer_flags_t flags)
{
    LOG_FN(VERBOSE,
        "%s, 0x%08x, %d, %d, %zd, %s",
        dpu_transfer_to_string(xfer),
        symbol.address,
        symbol.size,
        symbol_offset,
        length,
        dpu_transfer_flags_to_string(flags));

    dpu_error_t status;

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                switch (xfer) {
                    case DPU_XFER_TO_DPU:
                        if ((status = dpu_copy_to_symbol_matrix(dpu_set.list.ranks[each_rank], symbol, symbol_offset, length))) {
                            return status;
                        }
                        break;
                    case DPU_XFER_FROM_DPU:
                        if ((status
                                = dpu_copy_from_symbol_matrix(dpu_set.list.ranks[each_rank], symbol, symbol_offset, length))) {
                            return status;
                        }
                        break;
                    default:
                        return DPU_ERR_INVALID_MEMORY_TRANSFER;
                }
            }
            break;
        case DPU_SET_DPU: {
            struct dpu_t *dpu = dpu_set.dpu;
            void *buffer = dpu->transfer_buffer;

            switch (xfer) {
                case DPU_XFER_TO_DPU:
                    if ((status = dpu_copy_to_symbol_dpu(dpu, symbol, symbol_offset, buffer, length)) != DPU_OK) {
                        return status;
                    }
                    break;
                case DPU_XFER_FROM_DPU:
                    if ((status = dpu_copy_from_symbol_dpu(dpu, symbol, symbol_offset, buffer, length)) != DPU_OK) {
                        return status;
                    }
                    break;
                default:
                    return DPU_ERR_INVALID_MEMORY_TRANSFER;
            }
            break;
        }
        default:
            return DPU_ERR_INTERNAL;
    }

    if (flags != DPU_XFER_NO_RESET) {
        if ((status = dpu_prepare_xfer(dpu_set, NULL)) != DPU_OK) {
            return status;
        }
    }

    return DPU_OK;
}

static dpu_error_t
dpu_load_rank(struct dpu_rank_t *rank, struct dpu_program_t *program, dpu_elf_file_t elf_info)
{
    dpu_error_t status;
    dpu_description_t description = dpu_get_description(rank);
    uint8_t nr_of_control_interfaces = description->topology.nr_of_control_interfaces;
    uint8_t nr_of_dpus_per_control_interface = description->topology.nr_of_dpus_per_control_interface;

    if ((status = dpu_fill_profiling_info(rank,
             (iram_addr_t)program->mcount_address,
             (iram_addr_t)program->ret_mcount_address,
             program->thread_profiling_address))
        != DPU_OK) {
        goto end;
    }

    dpu_lock_rank(rank);

    struct _dpu_loader_context_t loader_context;
    dpu_loader_fill_rank_context(&loader_context, rank);

    if ((status = dpu_elf_load(elf_info, &loader_context)) != DPU_OK) {
        goto unlock_rank;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < nr_of_control_interfaces; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_of_dpus_per_control_interface; ++each_dpu) {
            struct dpu_t *dpu = dpu_get(rank, each_slice, each_dpu);

            if (!dpu->enabled)
                continue;

            struct dpu_program_t *previous_program = dpu_get_program(dpu);
            dpu_free_program(previous_program);
            dpu_take_program_ref(program);
            dpu_set_program(dpu, program);
        }
    }

unlock_rank:
    dpu_unlock_rank(rank);
end:
    return status;
}

static dpu_error_t
dpu_load_dpu(struct dpu_t *dpu, struct dpu_program_t *program, dpu_elf_file_t elf_info)
{
    dpu_error_t status;

    if (!dpu->enabled) {
        status = DPU_ERR_DPU_DISABLED;
        goto end;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);

    if ((status = dpu_fill_profiling_info(rank,
             (iram_addr_t)program->mcount_address,
             (iram_addr_t)program->ret_mcount_address,
             program->thread_profiling_address))
        != DPU_OK) {
        free(program);
        goto end;
    }

    dpu_lock_rank(rank);

    struct _dpu_loader_context_t loader_context;
    dpu_loader_fill_dpu_context(&loader_context, dpu);

    if ((status = dpu_elf_load(elf_info, &loader_context)) != DPU_OK) {
        goto unlock_rank;
    }

    struct dpu_program_t *previous_program = dpu_get_program(dpu);
    dpu_free_program(previous_program);
    dpu_take_program_ref(program);
    dpu_set_program(dpu, program);

unlock_rank:
    dpu_unlock_rank(rank);
end:
    return status;
}

static dpu_error_t
dpu_boot_rank(struct dpu_rank_t *rank, dpu_launch_policy_t policy)
{
    LOG_RANK(VERBOSE, rank, "%s", dpu_launch_policy_to_string(policy));

    dpu_error_t status;
    dpu_lock_rank(rank);
    dpu_run_context_t run_context = dpu_get_run_context(rank);

    if (run_context->nb_dpu_running != 0) {
        dpu_unlock_rank(rank);
        status = DPU_ERR_DPU_ALREADY_RUNNING;
        goto end;
    }

    dpu_description_t description = dpu_get_description(rank);
    uint8_t nr_cis = description->topology.nr_of_control_interfaces;

    dpu_bitfield_t *thread_was_running;

    if ((thread_was_running = malloc(nr_cis * sizeof(*thread_was_running))) == NULL) {
        dpu_unlock_rank(rank);
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    if ((status = dpu_custom_for_rank(rank, DPU_COMMAND_ALL_PREEXECUTION, NULL)) != DPU_OK) {
        dpu_unlock_rank(rank);
        goto free_bitfield;
    }

    if ((status = dpu_launch_thread_on_rank(rank, DPU_BOOT_THREAD, false, thread_was_running)) != DPU_OK) {
        dpu_unlock_rank(rank);
        goto free_bitfield;
    }

    dpu_unlock_rank(rank);

    switch (policy) {
        case DPU_ASYNCHRONOUS: {
            int res;

            if (!run_context->poll_thread.thr_exists) {
                res = pthread_cond_init(&(run_context->poll_thread.thr_cond), NULL);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                res = pthread_mutex_init(&(run_context->poll_thread.thr_mutex), NULL);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                res = pthread_create(&(run_context->poll_thread.thr_id), NULL, (void *(*)(void *))do_run_all_thread, rank);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                run_context->poll_thread.thr_exists = true;
            }

            /*
             * We need to have a global variable that says if thread has work or not, because a race could arise
             * if we signal the condition and the thread has not yet reached the pthread_cond_wait.
             * And we need to protect the global variable with a mutex: if the thread enters the pthread_cond_wait
             * thinking that it does not have work, and that in addition the pthread_cond_signal is missed, the thread
             * would starve.
             *          T1                                               T2
             * thr_has_work = true                              if (thr_has_work == false) // thr_has_work = false
             *                                                  SCHEDULED
             * pthread_cond_signal
             *                                                  pthread_cond_wait (Comes back from being scheduled)
             *                                                  T2 is never waken up
             */
            pthread_mutex_lock(&(run_context->poll_thread.thr_mutex));
            run_context->poll_thread.thr_has_work = 1;
            pthread_mutex_unlock(&(run_context->poll_thread.thr_mutex));

            // Not sure if the signal should be in mutex or not.
            res = pthread_cond_signal(&run_context->poll_thread.thr_cond);
            if (res) {
                status = DPU_ERR_SYSTEM;
            }

            break;
        }
        case DPU_SYNCHRONOUS: {
            status = do_run_all(rank);
            break;
        }
        default:
            status = DPU_ERR_INVALID_LAUNCH_POLICY;
            break;
    }

free_bitfield:
    free(thread_was_running);
end:
    return status;
}

static dpu_error_t
dpu_boot_dpu(struct dpu_t *dpu, dpu_launch_policy_t policy)
{
    LOG_DPU(VERBOSE, dpu, "%s", dpu_launch_policy_to_string(policy));

    dpu_error_t status;

    if (!dpu->enabled) {
        status = DPU_ERR_DPU_DISABLED;
        goto end;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_lock_rank(rank);
    dpu_run_context_t run_context = dpu_get_run_context(rank);
    struct dpu_poll_thread_context_t *poll_thread = &dpu->poll_thread;
    dpu_slice_id_t slice_id = dpu_get_slice_id(dpu);
    dpu_member_id_t member_id = dpu_get_member_id(dpu);

    if (dpu_mask_intersection(run_context->dpu_running[slice_id], dpu_mask_one(member_id)) != dpu_mask_empty()) {
        dpu_unlock_rank(rank);
        status = DPU_ERR_DPU_ALREADY_RUNNING;
        goto end;
    }

    if ((status = dpu_custom_for_dpu(dpu, DPU_COMMAND_DPU_PREEXECUTION, NULL)) != DPU_OK) {
        dpu_unlock_rank(rank);
        goto end;
    }

    bool thread_was_running;
    if ((status = dpu_launch_thread_on_dpu(dpu, DPU_BOOT_THREAD, false, &thread_was_running)) != DPU_OK) {
        dpu_unlock_rank(rank);
        goto end;
    }

    dpu_unlock_rank(rank);

    switch (policy) {
        case DPU_ASYNCHRONOUS: {
            int res;

            /* thr_exists is needed since pthread_create returned thread_id can not 'say' if thread exists or not. */
            if (!poll_thread->thr_exists) {
                res = pthread_cond_init(&(poll_thread->thr_cond), NULL);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                res = pthread_mutex_init(&(poll_thread->thr_mutex), NULL);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                res = pthread_create(&(poll_thread->thr_id), NULL, (void *(*)(void *))do_run_dpu_thread, dpu);
                if (res) {
                    status = DPU_ERR_SYSTEM;
                    break;
                }

                poll_thread->thr_exists = true;
            }

            /*
             * We need to have a global variable that says if thread has work or not, because a race could arise
             * if we signal the condition and the thread has not yet reached the pthread_cond_wait.
             * And we need to protect the global variable with a mutex: if the thread enters the pthread_cond_wait
             * thinking that it does not have work, and that in addition the pthread_cond_signal is missed, the thread
             * would starve.
             *          T1                                               T2
             * thr_has_work = true                              if (thr_has_work == false) // thr_has_work = false
             *                                                  SCHEDULED
             * pthread_cond_signal
             *                                                  pthread_cond_wait (Comes back from being scheduled)
             *                                                  T2 is never waken up
             */
            pthread_mutex_lock(&(poll_thread->thr_mutex));
            poll_thread->thr_has_work = 1;
            pthread_mutex_unlock(&(poll_thread->thr_mutex));

            // Not sure if the signal should be in mutex or not.
            res = pthread_cond_signal(&poll_thread->thr_cond);
            if (res) {
                /* signal failed, let the error path get rid of the thread created above. */
                status = DPU_ERR_SYSTEM;
                break;
            }

            break;
        }
        case DPU_SYNCHRONOUS: {
            status = do_run_dpu(dpu);
            break;
        }
        default:
            status = DPU_ERR_INVALID_LAUNCH_POLICY;
            break;
    }

end:
    return status;
}

static dpu_error_t
run_all_one_loop_iteration(struct dpu_rank_t *rank,
    bool *must_stop,
    dpu_bitfield_t *dpu_is_running,
    dpu_bitfield_t *dpu_is_in_fault)
{
    dpu_error_t status;

    if ((status = dpu_poll_rank(rank, dpu_is_running, dpu_is_in_fault)) != DPU_OK) {
        *must_stop = true;
    } else {
        dpu_description_t description = dpu_get_description(rank);
        uint8_t nr_of_control_interfaces = description->topology.nr_of_control_interfaces;

        *must_stop = true;

        for (dpu_slice_id_t each_ci = 0; each_ci < nr_of_control_interfaces; ++each_ci) {
            dpu_bitfield_t run_bitfield = dpu_is_running[each_ci];
            dpu_bitfield_t fault_bitfield = dpu_is_in_fault[each_ci];
            dpu_bitfield_t stopped_bitfield = ~run_bitfield | fault_bitfield;
            *must_stop = *must_stop && (stopped_bitfield == 0xFFFFFFFF);
        }
    }

    return status;
}

static dpu_error_t
run_dpu_one_loop_iteration(struct dpu_t *dpu, bool *must_stop)
{
    dpu_error_t status;
    bool dpu_is_running;
    bool dpu_is_in_fault;

    if ((status = dpu_poll_dpu(dpu, &dpu_is_running, &dpu_is_in_fault)) != DPU_OK) {
        *must_stop = true;
    } else {
        if (dpu_is_in_fault) {
            status = DPU_ERR_DPU_FAULT;
            dpu_custom_for_dpu(dpu, DPU_COMMAND_POSTMORTEM, NULL);
            *must_stop = true;
            dpu_print_lldb_message_on_fault(dpu, dpu_get_slice_id(dpu), dpu_get_member_id(dpu));
        } else {
            *must_stop = !dpu_is_running;
        }
    }

    return status;
}

static dpu_error_t
do_generic_run_all(struct dpu_rank_t *rank)
{
    dpu_error_t status = DPU_OK;
    dpu_description_t description = dpu_get_description(rank);

    uint8_t nr_of_control_interfaces = description->topology.nr_of_control_interfaces;
    unsigned long each_step = 0;
    bool must_stop = false;
    dpu_bitfield_t *dpu_is_running;
    dpu_bitfield_t *dpu_is_in_fault;

    if ((dpu_is_running = malloc(nr_of_control_interfaces * sizeof(*dpu_is_running))) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    if ((dpu_is_in_fault = malloc(nr_of_control_interfaces * sizeof(*dpu_is_in_fault))) == NULL) {
        free(dpu_is_running);
        return DPU_ERR_SYSTEM;
    }

    while (!must_stop) {
        dpu_lock_rank(rank);

        each_step++;
        status = run_all_one_loop_iteration(rank, &must_stop, dpu_is_running, dpu_is_in_fault);

        dpu_unlock_rank(rank);
    }

    if (status == DPU_OK) {
        bool dpu_fault = false;
        uint8_t nr_of_dpus_per_control_interface = description->topology.nr_of_dpus_per_control_interface;

        for (uint8_t each_ci = 0; each_ci < nr_of_control_interfaces; ++each_ci) {
            dpu_bitfield_t dpu_faults = dpu_is_in_fault[each_ci];
            bool at_least_one_dpu_in_fault_here = dpu_faults != 0;
            dpu_fault = dpu_fault || at_least_one_dpu_in_fault_here;

            if (at_least_one_dpu_in_fault_here) {
                for (dpu_member_id_t each_dpu = 0; each_dpu < nr_of_dpus_per_control_interface; ++each_dpu) {
                    if (dpu_mask_is_selected(dpu_faults, each_dpu)) {
                        dpu_print_lldb_message_on_fault(dpu_get(rank, each_ci, each_dpu), each_ci, each_dpu);
                    }
                }
            }
        }

        if (dpu_fault) {
            status = DPU_ERR_DPU_FAULT;
        }
    }

    free(dpu_is_in_fault);
    free(dpu_is_running);

    return status;
}

static dpu_error_t
do_simple_run_dpu(struct dpu_t *dpu)
{
    dpu_error_t status = DPU_OK;
    bool must_stop = false;

    while (!must_stop) {
        status = run_dpu_one_loop_iteration(dpu, &must_stop);
    }

    return status;
}

static dpu_error_t
do_run_all(struct dpu_rank_t *rank)
{
    dpu_error_t status;

    status = do_generic_run_all(rank);

    if (status != DPU_OK) {
        goto end;
    }

    status = dpu_custom_for_rank(rank, DPU_COMMAND_ALL_POSTEXECUTION, NULL);

end:
    return status;
}

static dpu_error_t
do_run_dpu(struct dpu_t *dpu)
{
    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);

    status = do_simple_run_dpu(dpu);

    if (status != DPU_OK) {
        goto end;
    }

    dpu_lock_rank(rank);

    status = dpu_custom_for_dpu(dpu, DPU_COMMAND_DPU_POSTEXECUTION, NULL);

    dpu_unlock_rank(rank);

end:
    return status;
}

static void *
do_run_all_thread(struct dpu_rank_t *rank)
{
    dpu_run_context_t run_context = dpu_get_run_context(rank);

    while (1) {
        pthread_mutex_lock(&(run_context->poll_thread.thr_mutex));
        while (!*((volatile int *)&(run_context->poll_thread.thr_has_work)))
            pthread_cond_wait(&(run_context->poll_thread.thr_cond), &(run_context->poll_thread.thr_mutex));

        if (run_context->poll_thread.thr_has_work == -1)
            goto end;

        run_context->poll_thread.thr_has_work = 0;
        pthread_mutex_unlock(&(run_context->poll_thread.thr_mutex));

        if (do_run_all(rank) != DPU_OK)
            goto end;
    }

end:
    return NULL;
}

static void *
do_run_dpu_thread(struct dpu_t *dpu)
{
    struct dpu_poll_thread_context_t *poll_thread = &dpu->poll_thread;

    while (1) {
        pthread_mutex_lock(&(poll_thread->thr_mutex));
        while (!*((volatile int *)&(poll_thread->thr_has_work)))
            pthread_cond_wait(&(poll_thread->thr_cond), &(poll_thread->thr_mutex));

        if (poll_thread->thr_has_work == -1)
            goto end;

        poll_thread->thr_has_work = 0;
        pthread_mutex_unlock(&(poll_thread->thr_mutex));

        if (do_run_dpu(dpu) != DPU_OK)
            goto end;
    }

end:
    return NULL;
}

static dpu_error_t
dpu_status_rank(struct dpu_rank_t *rank, bool *done, bool *fault)
{
    dpu_lock_rank(rank);
    dpu_description_t description = dpu_get_description(rank);
    dpu_run_context_t run_context = dpu_get_run_context(rank);

    uint8_t nr_cis = description->topology.nr_of_control_interfaces;
    dpu_bitfield_t empty_mask = dpu_mask_empty();

    *done = true;
    *fault = false;

    for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
        *done = *done && (run_context->dpu_running[each_slice] == empty_mask);
        *fault = *fault || (run_context->dpu_in_fault[each_slice] != empty_mask);
    }

    dpu_unlock_rank(rank);
    return DPU_OK;
}

static dpu_error_t
dpu_status_dpu(struct dpu_t *dpu, bool *done, bool *fault)
{
    struct dpu_rank_t *rank = dpu_get_rank(dpu);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_lock_rank(rank);

    dpu_run_context_t run_context = dpu_get_run_context(rank);
    dpu_slice_id_t slice_id = dpu_get_slice_id(dpu);
    dpu_member_id_t member_id = dpu_get_member_id(dpu);

    *done = !dpu_mask_is_selected(run_context->dpu_running[slice_id], member_id);
    *fault = dpu_mask_is_selected(run_context->dpu_in_fault[slice_id], member_id);

    dpu_unlock_rank(rank);

    return DPU_OK;
}

static dpu_error_t
dpu_copy_from_dpu(struct dpu_t *dpu, const char *symbol_name, uint32_t symbol_offset, void *dst, size_t length)
{
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_error_t status = DPU_OK;

    struct dpu_symbol_t symbol;
    struct dpu_program_t *program;

    if (!dpu->enabled) {
        status = DPU_ERR_DPU_DISABLED;
        goto end;
    }

    dpu_lock_rank(rank);

    if ((program = dpu_get_program(dpu)) == NULL) {
        goto unlock_rank;
    }

    if ((status = dpu_get_symbol(program, symbol_name, &symbol)) != DPU_OK) {
        goto unlock_rank;
    }

    if ((status = dpu_copy_from_symbol_dpu(dpu, symbol, symbol_offset, dst, length)) != DPU_OK) {
        goto unlock_rank;
    }

unlock_rank:
    dpu_unlock_rank(rank);
end:
    return status;
}

static dpu_error_t
dpu_set_transfer_buffer_safe(struct dpu_t *dpu, void *buffer)
{
    void *previous = dpu->transfer_buffer;
    dpu->transfer_buffer = buffer;

    return ((buffer != NULL) && (previous != NULL)) ? DPU_ERR_TRANSFER_ALREADY_SET : DPU_OK;
}

static dpu_error_t
dpu_get_common_program(struct dpu_set_t *dpu_set, struct dpu_program_t **program)
{
    struct dpu_program_t *the_program = NULL;

    switch (dpu_set->kind) {
        case DPU_SET_RANKS:
            for (uint32_t each_rank = 0; each_rank < dpu_set->list.nr_ranks; ++each_rank) {
                struct dpu_rank_t *rank = dpu_set->list.ranks[each_rank];
                uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
                uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

                for (int each_ci = 0; each_ci < nr_cis; ++each_ci) {
                    for (int each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                        struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                        if (!dpu_is_enabled(dpu)) {
                            continue;
                        }

                        struct dpu_program_t *dpu_program = dpu_get_program(dpu);

                        if (the_program == NULL) {
                            the_program = dpu_program;
                        }

                        if (the_program != dpu_program) {
                            return DPU_ERR_DIFFERENT_DPU_PROGRAMS;
                        }
                    }
                }
            }
            break;
        case DPU_SET_DPU:
            the_program = dpu_get_program(dpu_set->dpu);
            break;
        default:
            return DPU_ERR_INTERNAL;
    }

    *program = the_program;
    return DPU_OK;
}
