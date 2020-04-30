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

#include <dpu_rank.h>
#include <dpu_internals.h>
#include <dpu_api_log.h>
#include <dpu_mask.h>
#include <dpu/ufi.h>

static bool
is_transfer_matrix_full(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static bool
is_transfer_matrix_for_debug_mram(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
copy_from_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
copy_to_mrams_using_dpu_program(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);
static dpu_error_t
access_mram_using_dpu_program_individual(struct dpu_t *dpu,
    const struct dpu_transfer_mram *transfer,
    dpu_transfer_type_t transfer_type,
    dpuinstruction_t *program,
    dpuinstruction_t *iram_save,
    iram_size_t nr_instructions,
    dpuword_t *wram_save,
    wram_size_t wram_size);
static dpu_error_t
copy_mram_for_dpus(struct dpu_rank_t *rank,
    dpu_transfer_type_t type,
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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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

    dpu_error_t status;
    uint8_t mask = ALL_CIS;
    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = (dpuinstruction_t *)source };

    dpu_lock_rank(rank);
    FF(ufi_select_all(rank, &mask));
    FF(ufi_iram_write(rank, mask, iram_array, iram_instruction_index, nb_of_instructions));

end:
    dpu_unlock_rank(rank);
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
    verify_iram_access(iram_instruction_index, nb_of_instructions, rank);

    dpu_error_t status;
    dpu_slice_id_t slice_id = dpu->slice_id;
    uint8_t mask = CI_MASK_ONE(slice_id);
    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS];
    iram_array[slice_id] = (dpuinstruction_t *)source;

    dpu_lock_rank(rank);
    FF(ufi_select_dpu(rank, &mask, dpu->dpu_id));
    FF(ufi_iram_write(rank, mask, iram_array, iram_instruction_index, nb_of_instructions));

end:
    dpu_unlock_rank(rank);
    return status;
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
    verify_iram_access(iram_instruction_index, nb_of_instructions, rank);

    dpu_error_t status;
    dpu_slice_id_t slice_id = dpu->slice_id;
    uint8_t mask = CI_MASK_ONE(slice_id);
    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS];
    iram_array[slice_id] = destination;

    dpu_lock_rank(rank);
    FF(ufi_select_dpu(rank, &mask, dpu->dpu_id));
    FF(ufi_iram_read(rank, mask, iram_array, iram_instruction_index, nb_of_instructions));

end:
    dpu_unlock_rank(rank);
    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_to_wram_for_rank(struct dpu_rank_t *rank, wram_addr_t wram_word_offset, const dpuword_t *source, wram_size_t nb_of_words)
{
    LOG_RANK(VERBOSE, rank, "%u, %u", wram_word_offset, nb_of_words);

    verify_wram_access(wram_word_offset, nb_of_words, rank);
    dpu_error_t status;
    uint8_t mask = ALL_CIS;
    dpuword_t *wram_array[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = (dpuword_t *)source };

    dpu_lock_rank(rank);
    FF(ufi_select_all(rank, &mask));
    FF(ufi_wram_write(rank, mask, wram_array, wram_word_offset, nb_of_words));

end:
    dpu_unlock_rank(rank);
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
    verify_wram_access(wram_word_offset, nb_of_words, rank);

    dpu_error_t status;
    dpu_slice_id_t slice_id = dpu->slice_id;
    uint8_t mask = CI_MASK_ONE(slice_id);
    dpuword_t *wram_array[DPU_MAX_NR_CIS];
    wram_array[slice_id] = (dpuword_t *)source;

    dpu_lock_rank(rank);
    FF(ufi_select_dpu(rank, &mask, dpu->dpu_id));
    FF(ufi_wram_write(rank, mask, wram_array, wram_word_offset, nb_of_words));

end:
    dpu_unlock_rank(rank);
    return status;
}

__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ dpu_error_t
dpu_copy_from_wram_for_dpu(struct dpu_t *dpu, dpuword_t *destination, wram_addr_t wram_word_offset, wram_size_t nb_of_words)
{
    LOG_DPU(VERBOSE, dpu, "%u, %u", wram_word_offset, nb_of_words);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    verify_wram_access(wram_word_offset, nb_of_words, rank);

    dpu_error_t status;
    dpu_slice_id_t slice_id = dpu->slice_id;
    uint8_t mask = CI_MASK_ONE(slice_id);
    dpuword_t *wram_array[DPU_MAX_NR_CIS];
    wram_array[slice_id] = destination;

    dpu_lock_rank(rank);
    FF(ufi_select_dpu(rank, &mask, dpu->dpu_id));
    FF(ufi_wram_read(rank, mask, wram_array, wram_word_offset, nb_of_words));

end:
    dpu_unlock_rank(rank);
    return status;
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

    verify_mram_access(buffer, offset, size, rank);

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
            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

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

            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_id);

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

            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_id);

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
do_mram_transfer(struct dpu_rank_t *rank, dpu_transfer_type_t type, struct dpu_transfer_mram *matrix)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status = DPU_OK;
    dpu_rank_handler_t handler = rank->handler_context->handler;

    switch (type) {
        case DPU_TRANSFER_FROM_MRAM:
            if (handler->copy_from_rank(rank, matrix) != DPU_RANK_SUCCESS) {
                status = DPU_ERR_DRIVER;
            }
            break;
        case DPU_TRANSFER_TO_MRAM:
            if (handler->copy_to_rank(rank, matrix) != DPU_RANK_SUCCESS) {
                status = DPU_ERR_DRIVER;
            }
            break;

        default:
            status = DPU_ERR_INTERNAL;
            break;
    }

    return status;
}

static dpu_error_t
copy_mram_for_dpus(struct dpu_rank_t *rank,
    dpu_transfer_type_t type,
    const struct dpu_transfer_mram *transfer_matrix,
    bool is_mram_debug_transfer)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;
    struct dpu_transfer_mram *even_transfer_matrix = NULL, *odd_transfer_matrix = NULL;
    bool is_duplication_needed = false;

    if (!is_mram_debug_transfer) {
        is_duplication_needed = duplicate_transfer_matrix(rank, transfer_matrix, &even_transfer_matrix, &odd_transfer_matrix);
    }

    if (!is_duplication_needed) {
        bool is_full_matrix = is_transfer_matrix_full(rank, transfer_matrix);

        if (!is_mram_debug_transfer) {
            if (is_full_matrix)
                FF(dpu_host_get_access_for_rank(rank));
            else
                FF(host_get_access_for_transfer_matrix(rank, transfer_matrix));
        }

        FF(do_mram_transfer(rank, type, (struct dpu_transfer_mram *)transfer_matrix));

        if (!is_mram_debug_transfer) {
            if (is_full_matrix)
                FF(dpu_host_release_access_for_rank(rank));
            else
                FF(host_release_access_for_transfer_matrix(rank, transfer_matrix));
        }
    } else {
        FF(host_get_access_for_transfer_matrix(rank, even_transfer_matrix));
        FF(do_mram_transfer(rank, type, even_transfer_matrix));

        FF(host_get_access_for_transfer_matrix(rank, odd_transfer_matrix));
        FF(do_mram_transfer(rank, type, odd_transfer_matrix));

        FF(host_release_access_for_transfer_matrix(rank, transfer_matrix));
    }

end:
    free(even_transfer_matrix);
    free(odd_transfer_matrix);

    return status;
}

static dpu_error_t
access_mram_using_dpu_program_individual(struct dpu_t *dpu,
    const struct dpu_transfer_mram *transfer,
    dpu_transfer_type_t transfer_type,
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
    uint32_t ptr_offset;

    dpu_lock_rank(rank);

    if (transfer->size == 0) {
        goto end;
    }

    if (transfer->mram_number >= nr_mrams) {
        status = DPU_ERR_INVALID_MRAM_ACCESS;
        goto end;
    }

    FF(dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_MRAM_ACCESS_PROGRAM));

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

    uint8_t mask = CI_MASK_ONE(slice_id);
    FF(ufi_select_dpu(rank, &mask, dpu_id));

    // Interception Fault Set
    FF(ufi_set_dpu_fault_and_step(rank, mask));
    FF(ufi_set_bkp_fault(rank, mask));
    FF(ufi_thread_resume(rank, mask, 0, NULL));
    // Interception Fault Clear
    FF(ufi_read_bkp_fault(rank, mask, NULL));

    iram_addr_t thread_0_pc;
    struct _dpu_context_t context;
    context.pcs = &thread_0_pc;

    // Draining the pipeline
    drain_pipeline(dpu, &context, false);

    // Clear fault
    FF(ufi_select_dpu(rank, &mask, dpu_id));
    FF(ufi_clear_fault_bkp(rank, mask));
    FF(ufi_clear_fault_dpu(rank, mask));

    set_pc_in_core_dump_or_restore_registers(0, thread_0_pc, program, nr_instructions, 1);

    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS];
    dpuword_t *wram_array[DPU_MAX_NR_CIS];

    // Saving IRAM
    iram_array[slice_id] = iram_save;
    FF(ufi_iram_read(rank, mask, iram_array, 0, nr_instructions));
    // Saving WRAM
    wram_array[slice_id] = wram_save;
    FF(ufi_wram_read(rank, mask, wram_array, 0, wram_save_size));
    // Loading DPU program in IRAM
    iram_array[slice_id] = program;
    FF(ufi_iram_write(rank, mask, iram_array, 0, nr_instructions));

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
    wram_array[slice_id] = program_context;
    FF(ufi_wram_write(rank, mask, wram_array, 5, 3));
    if (transfer_type == DPU_TRANSFER_TO_MRAM) {
        // Loading WRAM
        wram_array[slice_id] = transfer->ptr;
        FF(ufi_wram_write(rank, mask, wram_array, transfer_start, transfer_size));
    }
    FF(ufi_thread_boot(rank, mask, 0, NULL));

    // Polling
    dpu_bitfield_t running[DPU_MAX_NR_CIS];
    do {
        FF(ufi_read_dpu_run(rank, mask, running));
    } while ((running[slice_id] & mask_one) != 0);

    if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
        // Fetching WRAM
        wram_array[slice_id] = transfer->ptr;
        FF(ufi_wram_read(rank, mask, wram_array, transfer_start, transfer_size));
    }
    ptr_offset = transfer_size * sizeof(dpuword_t);

    for (uint32_t each_iteration = 1; each_iteration < nr_complete_iterations; ++each_iteration) {
        if (transfer_type == DPU_TRANSFER_TO_MRAM) {
            // Loading WRAM
            wram_array[slice_id] = transfer->ptr + ptr_offset;
            FF(ufi_wram_write(rank, mask, wram_array, transfer_start, transfer_size));
        }

        if ((each_iteration == (nr_complete_iterations - 1)) && (remaining == 0)) {
            dpuword_t last_transfer_marker = 1;
            wram_array[slice_id] = &last_transfer_marker;
            FF(ufi_wram_write(rank, mask, wram_array, 7, 1));
        }

        FF(ufi_thread_resume(rank, mask, 0, NULL));

        // Polling
        do {
            FF(ufi_read_dpu_run(rank, mask, running));
        } while ((running[slice_id] & mask_one) != 0);

        if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
            // Fetching WRAM
            wram_array[slice_id] = transfer->ptr + ptr_offset;
            FF(ufi_wram_read(rank, mask, wram_array, transfer_start, transfer_size));
        }
        ptr_offset += transfer_size * sizeof(dpuword_t);
    }

    if (remaining > 0) {
        if (nr_complete_iterations > 0) {
            // Changing buffer size in the DPU program
            transfer_size = remaining / sizeof(dpuword_t);
            program_context[1] = remaining;
            program_context[2] = 1;

            wram_array[slice_id] = program_context + 1;
            FF(ufi_wram_write(rank, mask, wram_array, 6, 2));

            if (transfer_type == DPU_TRANSFER_TO_MRAM) {
                // Loading WRAM
                wram_array[slice_id] = transfer->ptr + ptr_offset;
                FF(ufi_wram_write(rank, mask, wram_array, transfer_start, transfer_size));
            }

            FF(ufi_thread_resume(rank, mask, 0, NULL));

            // Polling
            do {
                FF(ufi_read_dpu_run(rank, mask, running));
            } while ((running[slice_id] & mask_one) != 0);

            if (transfer_type == DPU_TRANSFER_FROM_MRAM) {
                // Fetching WRAM
                wram_array[slice_id] = transfer->ptr + ptr_offset;
                FF(ufi_wram_read(rank, mask, wram_array, transfer_start, transfer_size));
            }
        }
    }

    // Restoring IRAM
    iram_array[slice_id] = iram_save;
    FF(ufi_iram_write(rank, mask, iram_array, 0, nr_instructions));
    // Restoring WRAM
    wram_array[slice_id] = wram_save;
    FF(ufi_wram_write(rank, mask, wram_array, 0, wram_save_size));

    FF(dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_MRAM_ACCESS_PROGRAM));

end:
    dpu_unlock_rank(rank);
    return status;
}

static bool
is_transfer_matrix_full(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);

    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    for (uint8_t each_slice = 0; each_slice < nr_of_cis; ++each_slice) {
        dpu_selected_mask_t enabled_dpus = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;

        for (uint8_t each_dpu = 0; each_dpu < nr_of_dpus_per_ci; ++each_dpu) {
            if (!dpu_mask_is_selected(enabled_dpus, each_dpu))
                continue;

            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
            int idx = _transfer_matrix_index(dpu);

            if (!transfer_matrix[idx].ptr)
                return false;
        }
    }

    return true;
}

static bool
is_transfer_matrix_for_debug_mram(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "%p", transfer_matrix);

    uint8_t nr_of_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_of_cis = rank->description->topology.nr_of_control_interfaces;

    for (uint8_t each_slice = 0; each_slice < nr_of_cis; ++each_slice) {
        for (uint8_t each_dpu = 0; each_dpu < nr_of_dpus_per_ci; ++each_dpu) {
            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
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
                struct dpu_t *first_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
                struct dpu_t *second_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu + 1);

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
                    struct dpu_t *first_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
                    struct dpu_t *second_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu + 1);

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
            struct dpu_t *first_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
            int idx_dpu_first = _transfer_matrix_index(first_dpu);

            bool transfer_present = transfer_matrix[idx_dpu_first].ptr != NULL;

            if (each_dpu + 1 < rank->description->topology.nr_of_dpus_per_control_interface) {
                struct dpu_t *second_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu + 1);
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
            struct dpu_t *first_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu);
            int idx_dpu_first = _transfer_matrix_index(first_dpu);

            bool transfer_present = transfer_matrix[idx_dpu_first].ptr != NULL;

            if (each_dpu + 1 < rank->description->topology.nr_of_dpus_per_control_interface) {
                struct dpu_t *second_dpu = DPU_GET_UNSAFE(rank, each_slice, each_dpu + 1);
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
