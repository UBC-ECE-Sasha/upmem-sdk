/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <string.h>

#include <dpu_error.h>
#include <verbose_control.h>
#include <dpu_types.h>
#include <dpu_attributes.h>
#include <dpu_instruction_encoder.h>
#include <dpu_predef_programs.h>
#include <dpu_management.h>
#include <dpu_memory.h>
#include <dpu_config.h>

#include "dpu_rank.h"
#include "include/dpu_internals.h"
#include "include/dpu_api_log.h"
#include "dpu_mask.h"
#include "include/ufi_utils.h"

static bool
is_transfer_matrix_for_debug_mram(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
copy_from_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
copy_to_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
access_mram_using_dpu_program_individual(struct dpu_t *dpu,
    const struct dpu_transfer_mram *transfer,
    dpu_transfer_type_e transfer_type,
    dpuinstruction_t *program,
    dpuinstruction_t *iram_save,
    iram_size_t nr_instructions,
    dpuword_t *wram_save,
    wram_size_t wram_size);
static dpu_error_t
copy_mram_for_dpus(struct dpu_rank_t *rank,
    dpu_transfer_type_e type,
    const struct dpu_transfer_mram *transfer_matrix,
    bool is_mram_debug_transfer);
static bool
duplicate_transfer_matrix(struct dpu_rank_t *rank,
    const struct dpu_transfer_mram *transfer_matrix,
    struct dpu_transfer_mram **even_transfer_matrix,
    struct dpu_transfer_mram **odd_transfer_matrix);
static dpu_error_t
host_get_access_for_transfer_matrix(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
host_release_access_for_transfer_matrix(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);

dpu_error_t __API_SYMBOL__
dpu_copy_to_symbol_dpu(struct dpu_t *dpu, struct dpu_symbol_t symbol, uint32_t symbol_offset, const void *src, size_t length)
{
    dpu_error_t status;

    if (!dpu->enabled) {
        status = DPU_ERR_DPU_DISABLED;
        goto end;
    }

    if ((symbol_offset + length) > symbol.size) {
        status = DPU_ERR_INVALID_SYMBOL_ACCESS;
        goto end;
    }

    if ((status = dpu_copy_to_address_dpu(dpu, symbol.address + symbol_offset, src, length)) != DPU_OK) {
        goto end;
    }

end:
    return status;
}

dpu_error_t __API_SYMBOL__
dpu_copy_to_symbol_rank(struct dpu_rank_t *rank,
    struct dpu_symbol_t symbol,
    uint32_t symbol_offset,
    const void *src,
    size_t length)
{
    dpu_error_t status;

    if ((symbol_offset + length) > symbol.size) {
        status = DPU_ERR_INVALID_SYMBOL_ACCESS;
        goto end;
    }

    if ((status = dpu_copy_to_address_rank(rank, symbol.address + symbol_offset, src, length)) != DPU_OK) {
        goto end;
    }

end:
    return status;
}

dpu_error_t __API_SYMBOL__
dpu_copy_to_symbol_matrix(struct dpu_rank_t *rank, struct dpu_symbol_t symbol, uint32_t symbol_offset, size_t length)
{
    dpu_error_t status;

    if ((symbol_offset + length) > symbol.size) {
        status = DPU_ERR_INVALID_SYMBOL_ACCESS;
        goto end;
    }

    if ((status = dpu_copy_to_address_matrix(rank, symbol.address + symbol_offset, length)) != DPU_OK) {
        goto end;
    }

end:
    return status;
}

dpu_error_t __API_SYMBOL__
dpu_copy_from_symbol_dpu(struct dpu_t *dpu, struct dpu_symbol_t symbol, uint32_t symbol_offset, void *dst, size_t length)
{
    dpu_error_t status;

    if (!dpu->enabled) {
        status = DPU_ERR_DPU_DISABLED;
        goto end;
    }

    if ((symbol_offset + length) > symbol.size) {
        status = DPU_ERR_INVALID_SYMBOL_ACCESS;
        goto end;
    }

    if ((status = dpu_copy_from_address_dpu(dpu, symbol.address + symbol_offset, dst, length)) != DPU_OK) {
        goto end;
    }

end:
    return status;
}

dpu_error_t __API_SYMBOL__
dpu_copy_from_symbol_matrix(struct dpu_rank_t *rank, struct dpu_symbol_t symbol, uint32_t symbol_offset, size_t length)
{
    dpu_error_t status;

    if ((symbol_offset + length) > symbol.size) {
        status = DPU_ERR_INVALID_SYMBOL_ACCESS;
        goto end;
    }

    if ((status = dpu_copy_from_address_matrix(rank, symbol.address + symbol_offset, length)) != DPU_OK) {
        goto end;
    }

end:
    return status;
}

#define IRAM_MASK (0x80000000u)
#define MRAM_MASK (0x08000000u)

#define IRAM_ALIGN (3u)
#define WRAM_ALIGN (2u)

#define ALIGN_MASK(align) (~((1u << (align)) - 1u))
#define IRAM_ALIGN_MASK ALIGN_MASK(IRAM_ALIGN)
#define WRAM_ALIGN_MASK ALIGN_MASK(WRAM_ALIGN)

dpu_error_t __API_SYMBOL__
dpu_copy_to_address_dpu(struct dpu_t *dpu, dpu_mem_max_addr_t address, const void *src, dpu_mem_max_size_t length)
{
    if ((address & IRAM_MASK) == IRAM_MASK) {
        if ((address & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((length & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((((uintptr_t)src) & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }

        return dpu_copy_to_iram_for_dpu(dpu, (address & ~IRAM_MASK) >> IRAM_ALIGN, src, length >> IRAM_ALIGN);
    } else if ((address & MRAM_MASK) == MRAM_MASK) {
        return dpu_copy_to_mram(dpu, address & ~MRAM_MASK, src, length, DPU_PRIMARY_MRAM);
    } else {
        if ((address & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((length & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((((uintptr_t)src) & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }

        return dpu_copy_to_wram_for_dpu(dpu, address >> WRAM_ALIGN, src, length >> WRAM_ALIGN);
    }
}

dpu_error_t __API_SYMBOL__
dpu_copy_to_address_rank(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, const void *src, dpu_mem_max_size_t length)
{
    if ((address & IRAM_MASK) == IRAM_MASK) {
        if ((address & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((length & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((((uintptr_t)src) & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }

        return dpu_copy_to_iram_for_rank(rank, (address & ~IRAM_MASK) >> IRAM_ALIGN, src, length >> IRAM_ALIGN);
    } else if ((address & MRAM_MASK) == MRAM_MASK) {
        dpu_error_t status;
        struct dpu_transfer_mram *matrix;
        if ((status = dpu_transfer_matrix_allocate(rank, &matrix)) != DPU_OK) {
            return status;
        }
        dpu_transfer_matrix_set_all(rank, matrix, (void *)src, length, address & ~MRAM_MASK, DPU_PRIMARY_MRAM);
        status = dpu_copy_to_mrams(rank, matrix);
        dpu_transfer_matrix_free(rank, matrix);
        return status;
    } else {
        if ((address & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((length & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((((uintptr_t)src) & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }

        return dpu_copy_to_wram_for_rank(rank, address >> WRAM_ALIGN, src, length >> WRAM_ALIGN);
    }
}

dpu_error_t __API_SYMBOL__
dpu_copy_to_address_matrix(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, dpu_mem_max_size_t length)
{
    dpu_error_t status;
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

    if ((address & IRAM_MASK) == IRAM_MASK) {
        if ((address & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((length & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((((uintptr_t)buffer) & ~IRAM_ALIGN_MASK) != 0) {
                    return DPU_ERR_INVALID_IRAM_ACCESS;
                }

                if ((status = dpu_copy_to_iram_for_dpu(dpu, (address & ~IRAM_MASK) >> IRAM_ALIGN, buffer, length >> IRAM_ALIGN))
                    != DPU_OK) {
                    return status;
                }
            }
        }
    } else if ((address & MRAM_MASK) == MRAM_MASK) {
        struct dpu_transfer_mram *matrix;
        if ((status = dpu_transfer_matrix_allocate(rank, &matrix)) != DPU_OK) {
            return status;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((status = dpu_transfer_matrix_add_dpu(dpu, matrix, buffer, length, address & ~MRAM_MASK, DPU_PRIMARY_MRAM))
                    != DPU_OK) {
                    dpu_transfer_matrix_free(rank, matrix);
                    return status;
                }
            }
        }

        status = dpu_copy_to_mrams(rank, matrix);
        dpu_transfer_matrix_free(rank, matrix);
        return status;
    } else {
        if ((address & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((length & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((((uintptr_t)buffer) & ~WRAM_ALIGN_MASK) != 0) {
                    return DPU_ERR_INVALID_WRAM_ACCESS;
                }

                if ((status = dpu_copy_to_wram_for_dpu(dpu, address >> WRAM_ALIGN, buffer, length >> WRAM_ALIGN)) != DPU_OK) {
                    return status;
                }
            }
        }
    }

    return DPU_OK;
}

dpu_error_t __API_SYMBOL__
dpu_copy_from_address_dpu(struct dpu_t *dpu, dpu_mem_max_addr_t address, void *dst, dpu_mem_max_size_t length)
{
    if ((address & IRAM_MASK) == IRAM_MASK) {
        if ((address & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((length & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((((uintptr_t)dst) & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }

        return dpu_copy_from_iram_for_dpu(dpu, dst, (address & ~IRAM_MASK) >> IRAM_ALIGN, length >> IRAM_ALIGN);
    } else if ((address & MRAM_MASK) == MRAM_MASK) {
        return dpu_copy_from_mram(dpu, dst, address & ~MRAM_MASK, length, DPU_PRIMARY_MRAM);
    } else {
        if ((address & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((length & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((((uintptr_t)dst) & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }

        return dpu_copy_from_wram_for_dpu(dpu, dst, address >> WRAM_ALIGN, length >> WRAM_ALIGN);
    }
}

dpu_error_t __API_SYMBOL__
dpu_copy_from_address_matrix(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, dpu_mem_max_size_t length)
{
    dpu_error_t status;
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

    if ((address & IRAM_MASK) == IRAM_MASK) {
        if ((address & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }
        if ((length & ~IRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_IRAM_ACCESS;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((((uintptr_t)buffer) & ~IRAM_ALIGN_MASK) != 0) {
                    return DPU_ERR_INVALID_IRAM_ACCESS;
                }

                if ((status = dpu_copy_from_iram_for_dpu(dpu, buffer, (address & ~IRAM_MASK) >> IRAM_ALIGN, length >> IRAM_ALIGN))
                    != DPU_OK) {
                    return status;
                }
            }
        }
    } else if ((address & MRAM_MASK) == MRAM_MASK) {
        struct dpu_transfer_mram *matrix;
        if ((status = dpu_transfer_matrix_allocate(rank, &matrix)) != DPU_OK) {
            return status;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((status = dpu_transfer_matrix_add_dpu(dpu, matrix, buffer, length, address & ~MRAM_MASK, DPU_PRIMARY_MRAM))
                    != DPU_OK) {
                    dpu_transfer_matrix_free(rank, matrix);
                    return status;
                }
            }
        }

        status = dpu_copy_from_mrams(rank, matrix);
        dpu_transfer_matrix_free(rank, matrix);
        return status;
    } else {
        if ((address & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }
        if ((length & ~WRAM_ALIGN_MASK) != 0) {
            return DPU_ERR_INVALID_WRAM_ACCESS;
        }

        for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            for (uint8_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

                if (!dpu_is_enabled(dpu)) {
                    continue;
                }

                void *buffer = dpu->transfer_buffer;

                if (buffer == NULL) {
                    continue;
                }

                if ((((uintptr_t)buffer) & ~WRAM_ALIGN_MASK) != 0) {
                    return DPU_ERR_INVALID_WRAM_ACCESS;
                }

                if ((status = dpu_copy_from_wram_for_dpu(dpu, buffer, address >> WRAM_ALIGN, length >> WRAM_ALIGN)) != DPU_OK) {
                    return status;
                }
            }
        }
    }

    return DPU_OK;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_iram_for_rank(struct dpu_rank_t *rank,
    iram_addr_t iram_instruction_index,
    const dpuinstruction_t *source,
    iram_size_t nb_of_instructions)
{
    LOG_RANK(VERBOSE, rank, "%u, %u", iram_instruction_index, nb_of_instructions);

    verify_iram_access(iram_instruction_index, nb_of_instructions, rank);

    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    dpu_error_t status = DPU_OK;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus == dpu_mask_empty()) {
            continue;
        }

        build_select_query_for_all_enabled_dpus(rank, each_slice, query, transaction, status, err);
        safe_add_query(query,
            dpu_query_build_write_iram_instruction_for_previous(each_slice,
                rank->runtime.control_interface.slice_info[each_slice].enabled_dpus,
                iram_instruction_index,
                (dpuinstruction_t *)source,
                nb_of_instructions),
            transaction,
            status,
            err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

err:
    dpu_transaction_free(transaction);
    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_iram_for_dpu(struct dpu_t *dpu,
    iram_addr_t iram_instruction_index,
    const dpuinstruction_t *source,
    iram_size_t nb_of_instructions)
{
    LOG_DPU(VERBOSE, dpu, "%u, %u", iram_instruction_index, nb_of_instructions);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    dpu_error_t api_status;

    verify_iram_access(iram_instruction_index, nb_of_instructions, rank);

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL)
        return DPU_ERR_SYSTEM;

    dpu_query_t query;

    if ((query = dpu_query_build_select_dpu_for_control(slice_id, member_id)) == NULL) {
        api_status = DPU_ERR_SYSTEM;
        goto err_free;
    }

    dpu_transaction_add_query_tail(transaction, query);

    if ((query = dpu_query_build_write_iram_instruction_for_previous(
             slice_id, dpu_mask_one(member_id), iram_instruction_index, (dpuinstruction_t *)source, nb_of_instructions))
        == NULL) {
        api_status = DPU_ERR_SYSTEM;
        goto err_free;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    api_status = map_planner_status_to_api_status(status);

err_free:
    dpu_transaction_free(transaction);

    return api_status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_from_iram_for_dpu(struct dpu_t *dpu,
    dpuinstruction_t *destination,
    iram_addr_t iram_instruction_index,
    iram_size_t nb_of_instructions)
{
    LOG_DPU(VERBOSE, dpu, "%u, %u", iram_instruction_index, nb_of_instructions);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    verify_iram_access(iram_instruction_index, nb_of_instructions, rank);

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query;

    if ((query = dpu_query_build_select_dpu_for_control(slice_id, member_id)) == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    if ((query = dpu_query_build_read_iram_instruction_for_previous(
             slice_id, iram_instruction_index, nb_of_instructions, destination))
        == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    return map_planner_status_to_api_status(status);
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_wram_for_rank(struct dpu_rank_t *rank, wram_addr_t wram_word_offset, const dpuword_t *source, wram_size_t nb_of_words)
{
    LOG_RANK(VERBOSE, rank, "%u, %u", wram_word_offset, nb_of_words);

    verify_wram_access(wram_word_offset, nb_of_words, rank);

    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    dpu_error_t status = DPU_OK;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus == dpu_mask_empty()) {
            continue;
        }

        build_select_query_for_all_enabled_dpus(rank, each_slice, query, transaction, status, err);
        safe_add_query(query,
            dpu_query_build_write_wram_word_for_previous(each_slice,
                rank->runtime.control_interface.slice_info[each_slice].enabled_dpus,
                wram_word_offset,
                (dpuword_t *)source,
                nb_of_words),
            transaction,
            status,
            err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

err:
    dpu_transaction_free(transaction);
    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_wram_for_dpu(struct dpu_t *dpu, wram_addr_t wram_word_offset, const dpuword_t *source, wram_size_t nb_of_words)
{
    LOG_DPU(VERBOSE, dpu, "%u, %u", wram_word_offset, nb_of_words);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    verify_wram_access(wram_word_offset, nb_of_words, rank);

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query;

    if ((query = dpu_query_build_select_dpu_for_control(slice_id, member_id)) == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    if ((query = dpu_query_build_write_wram_word_for_previous(
             slice_id, dpu_mask_one(member_id), wram_word_offset, (dpuword_t *)source, nb_of_words))
        == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    return map_planner_status_to_api_status(status);
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_from_wram_for_dpu(struct dpu_t *dpu, dpuword_t *destination, wram_addr_t wram_word_offset, wram_size_t nb_of_words)
{
    LOG_DPU(VERBOSE, dpu, "%u, %u", wram_word_offset, nb_of_words);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    verify_wram_access(wram_word_offset, nb_of_words, rank);

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query;

    if ((query = dpu_query_build_select_dpu_for_control(slice_id, member_id)) == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    if ((query = dpu_query_build_read_wram_word_for_previous(slice_id, wram_word_offset, nb_of_words, destination)) == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    return map_planner_status_to_api_status(status);
}

__API_SYMBOL__ dpu_error_t
dpu_copy_to_mram(struct dpu_t *dpu,
    mram_addr_t mram_byte_offset,
    const uint8_t *source,
    mram_size_t nb_of_bytes,
    uint8_t mram_number)
{
    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    if (!nb_of_bytes) {
        return DPU_OK;
    }

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    struct dpu_transfer_mram *transfer_matrix;

    if ((status = dpu_transfer_matrix_allocate(rank, &transfer_matrix)) != DPU_OK) {
        goto end;
    }

    dpu_transfer_matrix_add_dpu(dpu, transfer_matrix, (void *)source, nb_of_bytes, mram_byte_offset, mram_number);

    if ((status = dpu_copy_to_mrams(rank, transfer_matrix)) != DPU_OK) {
        goto free_matrix;
    }

free_matrix:
    dpu_transfer_matrix_free(rank, transfer_matrix);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_copy_from_mram(struct dpu_t *dpu,
    uint8_t *destination,
    mram_addr_t mram_byte_offset,
    mram_size_t nb_of_bytes,
    uint8_t mram_number)
{
    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    if (!nb_of_bytes) {
        return DPU_OK;
    }

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    struct dpu_transfer_mram *transfer_matrix;

    if ((status = dpu_transfer_matrix_allocate(rank, &transfer_matrix)) != DPU_OK) {
        goto end;
    }

    dpu_transfer_matrix_add_dpu(dpu, transfer_matrix, (void *)destination, nb_of_bytes, mram_byte_offset, mram_number);

    if ((status = dpu_copy_from_mrams(rank, transfer_matrix)) != DPU_OK) {
        goto free_matrix;
    }

free_matrix:
    dpu_transfer_matrix_free(rank, transfer_matrix);
end:
    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_mrams(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);
    dpu_error_t status;

    dpu_lock_rank(rank);

    if (rank->description->configuration.mram_access_by_dpu_only) {
        if (rank->runtime.run_context.nb_dpu_running > 0) {
            LOG_RANK(WARNING,
                rank,
                "Host does not have access to the MRAM because %u DPU%s running.",
                rank->runtime.run_context.nb_dpu_running,
                rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
            dpu_unlock_rank(rank);
            return DPU_ERR_MRAM_BUSY;
        }

        status = copy_to_mrams_using_dpu_program(rank, transfer_matrix);
    } else {
        bool is_mram_debug_transfer = is_transfer_matrix_for_debug_mram(rank, transfer_matrix);

        if (!is_mram_debug_transfer && rank->runtime.run_context.nb_dpu_running > 0) {
            LOG_RANK(WARNING,
                rank,
                "Host does not have access to the MRAM because %u DPU%s running.",
                rank->runtime.run_context.nb_dpu_running,
                rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
            dpu_unlock_rank(rank);
            return DPU_ERR_MRAM_BUSY;
        }
        status = copy_mram_for_dpus(rank, DPU_TRANSFER_TO_MRAM, transfer_matrix, is_mram_debug_transfer);
    }

    dpu_unlock_rank(rank);

    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_from_mrams(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);
    dpu_error_t status;

    dpu_lock_rank(rank);

    if (rank->description->configuration.mram_access_by_dpu_only) {
        if (rank->runtime.run_context.nb_dpu_running > 0) {
            LOG_RANK(WARNING,
                rank,
                "Host does not have access to the MRAM because %u DPU%s running.",
                rank->runtime.run_context.nb_dpu_running,
                rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
            dpu_unlock_rank(rank);
            return DPU_ERR_MRAM_BUSY;
        }

        status = copy_from_mrams_using_dpu_program(rank, transfer_matrix);
    } else {
        bool is_mram_debug_transfer = is_transfer_matrix_for_debug_mram(rank, (const struct dpu_transfer_mram *)transfer_matrix);

        if (!is_mram_debug_transfer && rank->runtime.run_context.nb_dpu_running > 0) {
            LOG_RANK(WARNING,
                rank,
                "Host does not have access to the MRAM because %u DPU%s running.",
                rank->runtime.run_context.nb_dpu_running,
                rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
            dpu_unlock_rank(rank);
            return DPU_ERR_MRAM_BUSY;
        }

        status = copy_mram_for_dpus(rank, DPU_TRANSFER_FROM_MRAM, transfer_matrix, is_mram_debug_transfer);
    }

    dpu_unlock_rank(rank);

    return status;
}

static inline uint32_t
_transfer_matrix_index(struct dpu_t *dpu)
{
    return dpu->dpu_id * dpu->rank->description->topology.nr_of_control_interfaces + dpu->slice_id;
}

__API_SYMBOL__ dpu_error_t
dpu_transfer_matrix_allocate(struct dpu_rank_t *rank, struct dpu_transfer_mram **transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "");
    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    *transfer_matrix = calloc(nr_of_dpus_per_ci * nr_of_cis, sizeof(struct dpu_transfer_mram));
    if (!*transfer_matrix) {
        return DPU_ERR_SYSTEM;
    }

    return DPU_OK;
}

__API_SYMBOL__ void
dpu_transfer_matrix_free(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);
    free(transfer_matrix);
}

__API_SYMBOL__ dpu_error_t
dpu_transfer_matrix_add_dpu(struct dpu_t *dpu,
    struct dpu_transfer_mram *transfer_matrix,
    void *buffer,
    mram_size_t size,
    mram_addr_t offset_in_mram,
    uint32_t mram_number)
{
    LOG_DPU(VERBOSE, dpu, "%p, %p, %d, %d, %d", transfer_matrix, buffer, size, offset_in_mram, mram_number);
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    uint32_t offset = offset_in_mram & ~MRAM_MASK;

    verify_mram_access(offset, size, rank);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    uint32_t dpu_index = _transfer_matrix_index(dpu);

    transfer_matrix[dpu_index].size = size;
    transfer_matrix[dpu_index].ptr = buffer;
    transfer_matrix[dpu_index].mram_number = mram_number;
    transfer_matrix[dpu_index].offset_in_mram = offset;

    return DPU_OK;
}

__API_SYMBOL__ void
dpu_transfer_matrix_set_all(struct dpu_rank_t *rank,
    struct dpu_transfer_mram *transfer_matrix,
    void *buffer,
    mram_size_t size,
    mram_addr_t offset_in_mram,
    uint32_t mram_number)
{
    LOG_RANK(VERBOSE, rank, "%p, %p, %d, %d, %d", transfer_matrix, buffer, size, offset_in_mram, mram_number);
    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_of_cis; ++each_ci) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_of_dpus_per_ci; ++each_dpu) {
            struct dpu_t *dpu = dpu_get(rank, each_ci, each_dpu);

            if (dpu->enabled) {
                dpu_transfer_matrix_add_dpu(dpu, transfer_matrix, buffer, size, offset_in_mram, mram_number);
            }
        }
    }
}

__API_SYMBOL__ dpu_error_t
dpu_transfer_matrix_clear_dpu(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_DPU(VERBOSE, dpu, "%p", __func__, dpu_get_id(dpu), transfer_matrix);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    uint32_t dpu_index = _transfer_matrix_index(dpu);

    transfer_matrix[dpu_index].size = 0;
    transfer_matrix[dpu_index].ptr = NULL;
    transfer_matrix[dpu_index].mram_number = 0;
    transfer_matrix[dpu_index].offset_in_mram = 0;

    return DPU_OK;
}

__API_SYMBOL__ void
dpu_transfer_matrix_clear_all(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);
    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    memset(transfer_matrix, 0, nr_of_dpus_per_ci * nr_of_cis * sizeof(struct dpu_transfer_mram));
}

__API_SYMBOL__ void *
dpu_transfer_matrix_get_ptr(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_DPU(VERBOSE, dpu, "%p", transfer_matrix);

    if (!dpu->enabled) {
        LOG_DPU(WARNING, dpu, "dpu is disabled");

        return NULL;
    }

    uint32_t dpu_index = _transfer_matrix_index(dpu);

    return transfer_matrix[dpu_index].ptr;
}

__API_SYMBOL__ uint32_t
dpu_transfer_matrix_get_size(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix)
{
    LOG_DPU(VERBOSE, dpu, "%p", transfer_matrix);

    if (!dpu->enabled) {
        LOG_DPU(WARNING, dpu, "dpu is disabled");

        return 0;
    }

    uint32_t dpu_index = _transfer_matrix_index(dpu);

    return transfer_matrix[dpu_index].size;
}

static dpu_error_t
copy_from_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    int idx = 0;
    dpu_error_t status = DPU_OK;
    bool arrays_initialized = false;
    dpuinstruction_t *program = NULL;
    dpuinstruction_t *iram_save = NULL;
    dpuword_t *wram_save = NULL;
    iram_size_t nr_saved_instructions = 0;
    wram_size_t wram_size_in_words = 0;
    dpuword_t *mram_buffer = NULL;

    for (dpu_member_id_t dpu_id = 0; dpu_id < nr_dpus_per_ci; ++dpu_id) {
        for (dpu_slice_id_t slice_id = 0; slice_id < nr_cis; ++slice_id, ++idx) {
            const struct dpu_transfer_mram *transfer = &transfer_matrix[idx];
            const struct dpu_transfer_mram *actual_transfer;
            struct dpu_transfer_mram aligned_transfer;
            bool copy_needed;

            if (transfer->ptr == NULL) {
                continue;
            }

            struct dpu_t *dpu = dpu_get(rank, slice_id, dpu_id);

            if (!dpu->enabled) {
                continue;
            }

            if (!arrays_initialized) {
                program = fetch_mram_access_program(&nr_saved_instructions);

                if (program == NULL) {
                    status = DPU_ERR_SYSTEM;
                    goto free_buffer;
                }

                uint32_t nr_saved_iram_bytes = nr_saved_instructions * sizeof(dpuinstruction_t);

                if ((iram_save = malloc(nr_saved_iram_bytes)) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    free(program);
                    goto free_buffer;
                }

                wram_size_in_words = rank->description->memories.wram_size;

                if ((wram_save = malloc(wram_size_in_words * sizeof(*wram_save))) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    free(iram_save);
                    free(program);
                    goto free_buffer;
                }

                arrays_initialized = true;
            }

            if (((((unsigned long)transfer->ptr) & 3l) != 0) || ((transfer->offset_in_mram & 7) != 0)
                || ((transfer->size & 7) != 0)) {
                uint32_t aligned_size = (transfer->size + (transfer->offset_in_mram & 7) + 7) & ~7;

                if ((mram_buffer = realloc(mram_buffer, aligned_size)) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    goto free_arrays;
                }

                aligned_transfer.offset_in_mram = transfer->offset_in_mram & ~7;
                aligned_transfer.size = aligned_size;
                aligned_transfer.mram_number = transfer->mram_number;
                aligned_transfer.ptr = mram_buffer;
                actual_transfer = &aligned_transfer;
                copy_needed = true;
            } else {
                actual_transfer = transfer;
                copy_needed = false;
            }

            if ((status = access_mram_using_dpu_program_individual(dpu,
                     actual_transfer,
                     DPU_TRANSFER_FROM_MRAM,
                     program,
                     iram_save,
                     nr_saved_instructions,
                     wram_save,
                     wram_size_in_words))
                != DPU_OK) {
                goto free_arrays;
            }

            if (copy_needed) {
                memcpy(transfer->ptr, ((uint8_t *)mram_buffer) + (transfer->offset_in_mram & 7), transfer->size);
            }
        }
    }

free_arrays:
    if (arrays_initialized) {
        free(program);
        free(iram_save);
        free(wram_save);
    }
free_buffer:
    if (mram_buffer != NULL) {
        free(mram_buffer);
    }
    return status;
}

static dpu_error_t
copy_to_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    int idx = 0;
    dpu_error_t status = DPU_OK;
    bool arrays_initialized = false;
    dpuinstruction_t *program = NULL;
    dpuinstruction_t *iram_save = NULL;
    dpuword_t *wram_save = NULL;
    iram_size_t nr_saved_instructions = 0;
    wram_size_t wram_size_in_words = 0;
    dpuword_t *mram_buffer = NULL;

    for (dpu_member_id_t dpu_id = 0; dpu_id < nr_dpus_per_ci; ++dpu_id) {
        for (dpu_slice_id_t slice_id = 0; slice_id < nr_cis; ++slice_id, ++idx) {
            const struct dpu_transfer_mram *transfer = &transfer_matrix[idx];
            const struct dpu_transfer_mram *actual_transfer;
            struct dpu_transfer_mram aligned_transfer;

            if (transfer->ptr == NULL) {
                continue;
            }

            struct dpu_t *dpu = dpu_get(rank, slice_id, dpu_id);

            if (!dpu->enabled) {
                continue;
            }

            if (!arrays_initialized) {
                program = fetch_mram_access_program(&nr_saved_instructions);

                if (program == NULL) {
                    status = DPU_ERR_SYSTEM;
                    goto free_buffer;
                }

                uint32_t nr_saved_iram_bytes = nr_saved_instructions * sizeof(dpuinstruction_t);

                if ((iram_save = malloc(nr_saved_iram_bytes)) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    free(program);
                    goto free_buffer;
                }

                wram_size_in_words = rank->description->memories.wram_size;

                if ((wram_save = malloc(wram_size_in_words * sizeof(*wram_save))) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    free(iram_save);
                    free(program);
                    goto free_buffer;
                }

                arrays_initialized = true;
            }

            if (((((unsigned long)transfer->ptr) & 3l) != 0) || ((transfer->offset_in_mram & 7) != 0)
                || ((transfer->size & 7) != 0)) {
                uint32_t aligned_size = (transfer->size + (transfer->offset_in_mram & 7) + 7) & ~7;

                if ((mram_buffer = realloc(mram_buffer, aligned_size)) == NULL) {
                    status = DPU_ERR_SYSTEM;
                    goto free_arrays;
                }

                if ((transfer->offset_in_mram & 7) != 0) {
                    aligned_transfer.offset_in_mram = transfer->offset_in_mram & ~7;
                    aligned_transfer.size = 8;
                    aligned_transfer.mram_number = transfer->mram_number;
                    aligned_transfer.ptr = mram_buffer;

                    if ((status = access_mram_using_dpu_program_individual(dpu,
                             &aligned_transfer,
                             DPU_TRANSFER_FROM_MRAM,
                             program,
                             iram_save,
                             nr_saved_instructions,
                             wram_save,
                             wram_size_in_words))
                        != DPU_OK) {
                        goto free_arrays;
                    }
                }

                if (((transfer->size + (transfer->offset_in_mram & 7)) & 7) != 0) {
                    aligned_transfer.offset_in_mram = transfer->offset_in_mram + aligned_size - 8;
                    aligned_transfer.size = 8;
                    aligned_transfer.mram_number = transfer->mram_number;
                    aligned_transfer.ptr = ((uint8_t *)mram_buffer) + aligned_size - 8;

                    if ((status = access_mram_using_dpu_program_individual(dpu,
                             &aligned_transfer,
                             DPU_TRANSFER_FROM_MRAM,
                             program,
                             iram_save,
                             nr_saved_instructions,
                             wram_save,
                             wram_size_in_words))
                        != DPU_OK) {
                        goto free_arrays;
                    }
                }

                memcpy(((uint8_t *)mram_buffer) + (transfer->offset_in_mram & 7), transfer->ptr, transfer->size);

                aligned_transfer.offset_in_mram = transfer->offset_in_mram & ~7;
                aligned_transfer.size = aligned_size;
                aligned_transfer.mram_number = transfer->mram_number;
                aligned_transfer.ptr = mram_buffer;
                actual_transfer = &aligned_transfer;
            } else {
                actual_transfer = transfer;
            }

            if ((status = access_mram_using_dpu_program_individual(dpu,
                     actual_transfer,
                     DPU_TRANSFER_TO_MRAM,
                     program,
                     iram_save,
                     nr_saved_instructions,
                     wram_save,
                     wram_size_in_words))
                != DPU_OK) {
                goto free_arrays;
            }
        }
    }

free_arrays:
    if (arrays_initialized) {
        free(program);
        free(iram_save);
        free(wram_save);
    }
free_buffer:
    if (mram_buffer != NULL) {
        free(mram_buffer);
    }

    return status;
}

static dpu_error_t
copy_mram_for_dpus(struct dpu_rank_t *rank,
    dpu_transfer_type_e type,
    const struct dpu_transfer_mram *transfer_matrix,
    bool is_mram_debug_transfer)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_planner_status_e planner_status;
    dpu_error_t status = DPU_OK;
    struct dpu_transfer_mram *even_transfer_matrix = NULL, *odd_transfer_matrix = NULL;
    bool is_duplication_needed = false;

    if (!is_mram_debug_transfer)
        is_duplication_needed = duplicate_transfer_matrix(rank, transfer_matrix, &even_transfer_matrix, &odd_transfer_matrix);

    if (is_duplication_needed == false) {
        if (!is_mram_debug_transfer) {
            status = host_get_access_for_transfer_matrix(rank, transfer_matrix);
            if (status != DPU_OK)
                return status;
        }

        planner_status = dpu_planner_execute_transfer(
            rank->handler_context->handler, rank, type, (struct dpu_transfer_mram *)transfer_matrix);
        if (planner_status != DPU_PLANNER_SUCCESS) {
            status = map_planner_status_to_api_status(planner_status);
            return status;
        }
    } else {
        status = host_get_access_for_transfer_matrix(rank, even_transfer_matrix);
        if (status != DPU_OK)
            goto err_even_odd;

        planner_status = dpu_planner_execute_transfer(rank->handler_context->handler, rank, type, even_transfer_matrix);
        if (planner_status != DPU_PLANNER_SUCCESS) {

            status = map_planner_status_to_api_status(planner_status);
            goto err_even_odd;
        }

        status = host_get_access_for_transfer_matrix(rank, odd_transfer_matrix);
        if (status != DPU_OK)
            goto err_even_odd;

        planner_status = dpu_planner_execute_transfer(rank->handler_context->handler, rank, type, odd_transfer_matrix);
        if (planner_status != DPU_PLANNER_SUCCESS) {
            status = map_planner_status_to_api_status(planner_status);
            goto err_even_odd;
        }

        free(even_transfer_matrix);
        free(odd_transfer_matrix);
    }

    if (!is_mram_debug_transfer) {
        status = host_release_access_for_transfer_matrix(rank, transfer_matrix);
        if (status != DPU_OK)
            return status;
    }

    return status;

err_even_odd:
    free(even_transfer_matrix);
    free(odd_transfer_matrix);

    return status;
}

static dpu_error_t
access_mram_using_dpu_program_individual(struct dpu_t *dpu,
    const struct dpu_transfer_mram *transfer,
    dpu_transfer_type_e transfer_type,
    dpuinstruction_t *program,
    dpuinstruction_t *iram_save,
    iram_size_t nr_instructions,
    dpuword_t *wram_save,
    wram_size_t wram_size)
{
    /*
     * Preconditions:
     *  - transfer->ptr             must be aligned on 4 bytes
     *  - transfer->size            must be aligned on 8 bytes
     *  - transfer->offset_in_mram  must be aligned on 8 bytes
     */

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

#define PROGRAM_CONTEXT_WRAM_SIZE 8
    dpu_error_t status = DPU_OK;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    uint8_t nr_mrams = (rank->description->memories.dbg_mram_size == 0) ? 1 : 2;
    dpu_selected_mask_t mask_one = dpu_mask_one(dpu_id);
    uint32_t bitfield_result;
    dpu_planner_status_e planner_status;
    uint32_t ptr_offset;

    if (transfer->size == 0) {
        goto end;
    }

    if ((status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_MRAM_ACCESS_PROGRAM))
        != DPU_OK) {
        goto end;
    }

    if (transfer->mram_number >= nr_mrams) {
        status = DPU_ERR_INVALID_MRAM_ACCESS;
        goto end;
    }

#define DMA_ACCESS_IDX_1 11
#define DMA_ACCESS_IDX_2 19
#define DMA_ACCESS_WRAM_REG REG_R2
#define DMA_ACCESS_MRAM_REG REG_R0
#define DMA_ACCESS_LENGTH_1 255
#define DMA_ACCESS_LENGTH_2 0

    // Patch DMA access instructions
    switch (transfer->mram_number) {
        case 0:
            if (transfer_type == DPU_TRANSFER_TO_MRAM) {
                program[DMA_ACCESS_IDX_1] = SDMArri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_1);
                program[DMA_ACCESS_IDX_2] = SDMArri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_2);
            } else {
                program[DMA_ACCESS_IDX_1] = LDMArri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_1);
                program[DMA_ACCESS_IDX_2] = LDMArri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_2);
            }
            break;
        case 1:
            if (transfer_type == DPU_TRANSFER_TO_MRAM) {
                program[DMA_ACCESS_IDX_1] = SDMA_DBGrri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_1);
                program[DMA_ACCESS_IDX_2] = SDMA_DBGrri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_2);
            } else {
                program[DMA_ACCESS_IDX_1] = LDMA_DBGrri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_1);
                program[DMA_ACCESS_IDX_2] = LDMA_DBGrri(DMA_ACCESS_WRAM_REG, DMA_ACCESS_MRAM_REG, DMA_ACCESS_LENGTH_2);
            }
            break;
        default:
            status = DPU_ERR_INVALID_MRAM_ACCESS;
            goto end;
    }

    wram_size_t wram_save_size;

    if (((transfer->size / sizeof(dpuword_t)) + PROGRAM_CONTEXT_WRAM_SIZE) >= wram_size) {
        wram_save_size = wram_size;
    } else {
        wram_save_size = (transfer->size / sizeof(dpuword_t)) + PROGRAM_CONTEXT_WRAM_SIZE;
    }

    dpu_transaction_t transaction;
    dpu_transaction_t polling_transaction;
    dpu_transaction_t wram_access_transaction;
    dpu_query_t query;

    if ((wram_access_transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    if ((polling_transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_wram_transaction;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_polling_transaction;
    }

    // Fetching PC for thread 0 and patching program

    dpu_pc_mode_e pc_mode;

    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto free_transaction;
    }
    safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, dpu_id), transaction, status, free_transaction);
    // Interception Fault Set
    safe_add_query(
        query, dpu_query_build_set_and_step_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_set_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);

    safe_add_query(
        query, dpu_query_build_resume_thread_for_previous(slice_id, 0, &bitfield_result), transaction, status, free_transaction);
    // Interception Fault Clear
    safe_add_query(
        query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &bitfield_result), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    iram_addr_t thread_0_pc;
    struct _dpu_context_t context;
    context.pcs = &thread_0_pc;

    // Draining the pipeline
    drain_pipeline(dpu, &context, pc_mode, false);

    // Clear fault
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    set_pc_in_core_dump_or_restore_registers(0, thread_0_pc, program, nr_instructions, 1);

    // Saving IRAM
    safe_add_query(query,
        dpu_query_build_read_iram_instruction_for_previous(slice_id, 0, nr_instructions, iram_save),
        transaction,
        status,
        free_transaction);
    // Saving WRAM
    safe_add_query(query,
        dpu_query_build_read_wram_word_for_previous(slice_id, 0, wram_save_size, wram_save),
        transaction,
        status,
        free_transaction);
    // Loading DPU program in IRAM
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(slice_id, mask_one, 0, program, nr_instructions),
        transaction,
        status,
        free_transaction);

    wram_size_t transfer_size = wram_save_size - PROGRAM_CONTEXT_WRAM_SIZE;
    wram_addr_t transfer_start = PROGRAM_CONTEXT_WRAM_SIZE;

    uint32_t transfer_size_in_bytes = transfer_size * sizeof(dpuword_t);
    uint32_t nr_complete_iterations = transfer->size / transfer_size_in_bytes;
    uint32_t remaining = transfer->size % transfer_size_in_bytes;

    // Write context
    dpuword_t program_context[3] = {
        transfer->offset_in_mram, // MRAM offset
        transfer_size * sizeof(dpuword_t), // Buffer size
        ((nr_complete_iterations == 0) || ((nr_complete_iterations == 1) && (remaining == 0))) ? 1 : 0 // Last transfer marker
    };
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(slice_id, mask_one, 5, program_context, 3),
        transaction,
        status,
        free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(
        query, dpu_query_build_boot_thread_for_previous(slice_id, 0, &bitfield_result), transaction, status, free_transaction);

    if (transfer_type == DPU_TRANSFER_TO_MRAM) {
        // Loading WRAM
        safe_add_query(query,
            dpu_query_build_write_wram_word_for_previous(slice_id, mask_one, transfer_start, transfer->ptr, transfer_size),
            wram_access_transaction,
            status,
            free_transaction);
        safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    safe_add_query(query,
        dpu_query_build_read_dpu_run_state_for_previous(slice_id, &bitfield_result),
        polling_transaction,
        status,
        free_transaction);

    // Polling
    do {
        safe_execute_transaction(polling_transaction, rank, planner_status, status, free_transaction);
    } while ((bitfield_result & mask_one) != 0);

    if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
        // Fetching WRAM
        safe_add_query(query,
            dpu_query_build_read_wram_word_for_previous(slice_id, transfer_start, transfer_size, transfer->ptr),
            wram_access_transaction,
            status,
            free_transaction);
        safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
    }
    ptr_offset = transfer_size * sizeof(dpuword_t);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(
        query, dpu_query_build_resume_thread_for_previous(slice_id, 0, &bitfield_result), transaction, status, free_transaction);

    for (uint32_t each_iteration = 1; each_iteration < nr_complete_iterations; ++each_iteration) {
        if (transfer_type == DPU_TRANSFER_TO_MRAM) {
            // Loading WRAM
            dpu_transaction_free_queries_for_slice(wram_access_transaction, slice_id);
            safe_add_query(query,
                dpu_query_build_write_wram_word_for_previous(
                    slice_id, mask_one, transfer_start, transfer->ptr + ptr_offset, transfer_size),
                wram_access_transaction,
                status,
                free_transaction);
            safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
        }

        if ((each_iteration == (nr_complete_iterations - 1)) && (remaining == 0)) {
            dpuword_t last_transfer_marker = 1;

            dpu_transaction_free_queries_for_slice(wram_access_transaction, slice_id);
            safe_add_query(query,
                dpu_query_build_write_wram_word_for_previous(slice_id, mask_one, 7, &last_transfer_marker, 1),
                wram_access_transaction,
                status,
                free_transaction);
            safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
        }

        safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

        // Polling
        do {
            safe_execute_transaction(polling_transaction, rank, planner_status, status, free_transaction);
        } while ((bitfield_result & mask_one) != 0);

        if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
            // Fetching WRAM
            dpu_transaction_free_queries_for_slice(wram_access_transaction, slice_id);
            safe_add_query(query,
                dpu_query_build_read_wram_word_for_previous(slice_id, transfer_start, transfer_size, transfer->ptr + ptr_offset),
                wram_access_transaction,
                status,
                free_transaction);
            safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
        }
        ptr_offset += transfer_size * sizeof(dpuword_t);
    }

    if (remaining > 0) {
        if (nr_complete_iterations > 0) {
            // Changing buffer size in the DPU program
            transfer_size = remaining / sizeof(dpuword_t);
            program_context[1] = remaining;
            program_context[2] = 1;

            dpu_transaction_free_queries_for_slice(transaction, slice_id);

            safe_add_query(query,
                dpu_query_build_write_wram_word_for_previous(slice_id, mask_one, 6, program_context + 1, 2),
                transaction,
                status,
                free_transaction);
            safe_add_query(query,
                dpu_query_build_resume_thread_for_previous(slice_id, 0, &bitfield_result),
                transaction,
                status,
                free_transaction);

            if (transfer_type == DPU_TRANSFER_TO_MRAM) {
                // Loading WRAM
                dpu_transaction_free_queries_for_slice(wram_access_transaction, slice_id);
                safe_add_query(query,
                    dpu_query_build_write_wram_word_for_previous(
                        slice_id, mask_one, transfer_start, transfer->ptr + ptr_offset, transfer_size),
                    wram_access_transaction,
                    status,
                    free_transaction);
                safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
            }

            safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

            dpu_transaction_free_queries_for_slice(wram_access_transaction, slice_id);

            // Polling
            do {
                safe_execute_transaction(polling_transaction, rank, planner_status, status, free_transaction);
            } while ((bitfield_result & mask_one) != 0);

            if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
                // Fetching WRAM
                safe_add_query(query,
                    dpu_query_build_read_wram_word_for_previous(
                        slice_id, transfer_start, transfer_size, transfer->ptr + ptr_offset),
                    wram_access_transaction,
                    status,
                    free_transaction);
                safe_execute_transaction(wram_access_transaction, rank, planner_status, status, free_transaction);
            }
        }
    }

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // Restoring IRAM
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(slice_id, mask_one, 0, iram_save, nr_instructions),
        transaction,
        status,
        free_transaction);
    // Restoring WRAM
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(slice_id, mask_one, 0, wram_save, wram_save_size),
        transaction,
        status,
        free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_MRAM_ACCESS_PROGRAM);

free_transaction:
    dpu_transaction_free(transaction);
free_polling_transaction:
    dpu_transaction_free(polling_transaction);
free_wram_transaction:
    dpu_transaction_free(wram_access_transaction);
end:
    return status;
}

static bool
is_transfer_matrix_for_debug_mram(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);

    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    for (uint8_t each_slice = 0; each_slice < nr_of_cis; ++each_slice) {
        for (uint8_t each_dpu = 0; each_dpu < nr_of_dpus_per_ci; ++each_dpu) {
            struct dpu_t *dpu = dpu_get(rank, each_slice, each_dpu);
            int idx = _transfer_matrix_index(dpu);

            if (transfer_matrix[idx].ptr && transfer_matrix[idx].mram_number != DPU_DEBUG_MRAM)
                return false;
        }
    }

    return true;
}

static bool
duplicate_transfer_matrix(struct dpu_rank_t *rank,
    const struct dpu_transfer_mram *transfer_matrix,
    struct dpu_transfer_mram **even_transfer_matrix,
    struct dpu_transfer_mram **odd_transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);

    dpu_error_t status;
    bool is_duplication_needed = false;

    if (rank->description->topology.nr_of_dpus_per_control_interface <= 1)
        return false;

    /* MRAM mux is set by pair of DPUs: DPU0-DPU1, DPU2-DPU3, DPU4-DPU5, DPU6-DPU7 have the same mux state.
     * In the case all DPUs are stopped and the transfer is authorized, we must take care not overriding MRAMs
     * whose transfer in the matrix is not defined. But with the pair of DPUs as explained above, we must
     * duplicate the transfer if one DPU of a pair has a defined transfer and not the other. Let's take an example
     * (where '1' means there is a defined transfer and '0' no transfer for this DPU should be done):
     *
     *       CI    0     1     2     3     4     5     6     7
     *          -------------------------------------------------
     *  DPU0    |  1  |  1  |  1  |  1  |  1  |  0  |  1  |  1  |
     *  DPU1    |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |
     *                              ....
     *
     *  In this case, we must not override the MRAM of CI5:DPU0, so we must switch the mux DPU-side. But doing so, we
     *  also switch the mux for CI5:DPU1. But CI5:DPU1 has a defined transfer, then we cannot do this at the
     *  same time and hence the duplication of the matrix.
     *  This applies only if it is the job of the API to do that and in that case matrix duplication is MANDATORY since
     *  we don't know how the backend goes through the matrix, so we must prepare all the muxes so that one transfer
     *  matrix is correct.
     *
     *  So the initial transfer matrix must be duplicated at most 2 times, one for even DPUs, one for odd DPUs:
     *
     *       CI    0     1     2     3     4     5     6     7
     *          -------------------------------------------------
     *  DPU0    |  1  |  1  |  1  |  1  |  1  |  0  |  1  |  1  |
     *  DPU1    |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |
     *
     *  For the matrix above, for transfers to be correct, we must duplicate it this way:
     *
     *       CI    0     1     2     3     4     5     6     7
     *          -------------------------------------------------
     *  DPU0    |  1  |  1  |  1  |  1  |  1  |  0  |  1  |  1  |
     *  DPU1    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *
     *                              +
     *
     *       CI    0     1     2     3     4     5     6     7
     *          -------------------------------------------------
     *  DPU0    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
     *  DPU1    |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |
     *
     *  Which amounts to, once such a duplication is detected, to split the initial transfer matrix into 2 matrix,
     *  one containing the odd line and the other the even.
     */
    if (rank->description->configuration.api_must_switch_mram_mux) {
        /* Let's go through the matrix and search for pairs of DPUs whose one DPU has a defined transfer and the other one
         * has no transfer. If we find one such pair, let's duplicate the matrix.
         */
        for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface;
             each_dpu += 2) {
            for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
                int idx_dpu_first, idx_dpu_second;
                struct dpu_t *first_dpu = dpu_get(rank, each_slice, each_dpu);
                struct dpu_t *second_dpu = dpu_get(rank, each_slice, each_dpu + 1);

                idx_dpu_first = _transfer_matrix_index(first_dpu);
                idx_dpu_second = _transfer_matrix_index(second_dpu);

                if ((!transfer_matrix[idx_dpu_first].ptr || !transfer_matrix[idx_dpu_second].ptr)
                    && (transfer_matrix[idx_dpu_first].ptr || transfer_matrix[idx_dpu_second].ptr)) {
                    dpu_member_id_t dpu_id_notnull
                        = transfer_matrix[idx_dpu_first].ptr ? each_dpu : (dpu_member_id_t)(each_dpu + 1);

                    LOG_RANK(VERBOSE,
                        rank,
                        "Duplicating transfer matrix since DPU %d of the pair (%d, %d) has a "
                        "defined transfer whereas the other does not.",
                        dpu_id_notnull,
                        each_dpu,
                        each_dpu + 1);

                    status = dpu_transfer_matrix_allocate(rank, even_transfer_matrix);
                    if (status != DPU_OK) {
                        return status;
                    }

                    status = dpu_transfer_matrix_allocate(rank, odd_transfer_matrix);
                    if (status != DPU_OK) {
                        return status;
                    }

                    is_duplication_needed = true;
                    break;
                }
            }

            if (is_duplication_needed)
                break;
        }

        if (is_duplication_needed) {
            for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface;
                 each_dpu += 2) {
                for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces;
                     ++each_slice) {
                    struct dpu_t *first_dpu = dpu_get(rank, each_slice, each_dpu);
                    struct dpu_t *second_dpu = dpu_get(rank, each_slice, each_dpu + 1);

                    int first_dpu_idx = _transfer_matrix_index(first_dpu);
                    int second_dpu_idx = _transfer_matrix_index(second_dpu);

                    dpu_transfer_matrix_add_dpu(first_dpu,
                        *even_transfer_matrix,
                        transfer_matrix[first_dpu_idx].ptr,
                        transfer_matrix[first_dpu_idx].size,
                        transfer_matrix[first_dpu_idx].offset_in_mram,
                        (uint8_t)transfer_matrix[first_dpu_idx].mram_number);

                    dpu_transfer_matrix_add_dpu(second_dpu,
                        *odd_transfer_matrix,
                        transfer_matrix[second_dpu_idx].ptr,
                        transfer_matrix[second_dpu_idx].size,
                        transfer_matrix[second_dpu_idx].offset_in_mram,
                        (uint8_t)transfer_matrix[second_dpu_idx].mram_number);
                }
            }
        }
    }

    return is_duplication_needed;
}

static dpu_error_t
host_get_access_for_transfer_matrix(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    dpu_error_t status;

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface;
             each_dpu += 2) {
            struct dpu_t *first_dpu = dpu_get(rank, each_slice, each_dpu);
            int idx_dpu_first = _transfer_matrix_index(first_dpu);

            bool transfer_present = transfer_matrix[idx_dpu_first].ptr != NULL;

            if (each_dpu + 1 < rank->description->topology.nr_of_dpus_per_control_interface) {
                struct dpu_t *second_dpu = dpu_get(rank, each_slice, each_dpu + 1);
                int idx_dpu_second = _transfer_matrix_index(second_dpu);
                transfer_present = transfer_present || transfer_matrix[idx_dpu_second].ptr != NULL;
            }

            if (transfer_present) {
                status = dpu_host_get_access_for_dpu(first_dpu);
                if (status != DPU_OK)
                    return status;
            }
        }
    }

    return DPU_OK;
}

static dpu_error_t
host_release_access_for_transfer_matrix(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    dpu_error_t status;

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface;
             each_dpu += 2) {
            struct dpu_t *first_dpu = dpu_get(rank, each_slice, each_dpu);
            int idx_dpu_first = _transfer_matrix_index(first_dpu);

            bool transfer_present = transfer_matrix[idx_dpu_first].ptr != NULL;

            if (each_dpu + 1 < rank->description->topology.nr_of_dpus_per_control_interface) {
                struct dpu_t *second_dpu = dpu_get(rank, each_slice, each_dpu + 1);
                int idx_dpu_second = _transfer_matrix_index(second_dpu);
                transfer_present = transfer_present || transfer_matrix[idx_dpu_second].ptr != NULL;
            }

            if (transfer_present) {
                status = dpu_host_release_access_for_dpu(first_dpu);
                if (status != DPU_OK)
                    return status;
            }
        }
    }

    return DPU_OK;
}