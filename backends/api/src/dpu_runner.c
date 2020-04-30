/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdbool.h>
#include <string.h>

#include <dpu_error.h>
#include <dpu_types.h>
#include <verbose_control.h>
#include <dpu_attributes.h>
#include <dpu_management.h>
#include <dpu_internals.h>
#include <dpu_mask.h>
#include <dpu_rank.h>
#include <dpu_api_log.h>
#include <dpu/ufi.h>

dpu_run_context_t
dpu_get_run_context(struct dpu_rank_t *rank)
{
    return &rank->runtime.run_context;
}

__API_SYMBOL__ dpu_error_t
dpu_launch_thread_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, bool should_resume, dpu_bitfield_t *thread_was_running)
{
    LOG_RANK(VERBOSE, rank, "%d, %d", thread, should_resume);

    dpu_error_t status;
    dpu_bitfield_t run_bitfield[DPU_MAX_NR_CIS];
    dpu_bitfield_t fault_bitfield[DPU_MAX_NR_CIS];

    uint8_t mask = ALL_CIS;

    verify_thread_id(thread, rank);

    dpu_lock_rank(rank);

    FF(dpu_poll_rank(rank, run_bitfield, fault_bitfield));

    if (!should_resume) {
        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS:
                status = dpu_set_magic_profiling_for_dpu(rank->profiling_context.dpu);
                if (status != DPU_OK) {
                    dpu_unlock_rank(rank);
                    return status;
                }
                break;
            case DPU_PROFILING_SAMPLES:
                memset(rank->profiling_context.sample_stats,
                    0,
                    rank->description->memories.iram_size * sizeof(*(rank->profiling_context.sample_stats)));
                break;
        }
    }

    FF(ufi_select_all(rank, &mask));

    if (should_resume) {
        FF(ufi_thread_resume(rank, mask, thread, thread_was_running));
    } else {
        FF(ufi_thread_boot(rank, mask, thread, thread_was_running));
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;

        thread_was_running[each_slice] &= mask_all;
        run_bitfield[each_slice] &= mask_all;

        rank->runtime.run_context.dpu_running[each_slice] = mask_all;
        rank->runtime.run_context.nb_dpu_running += dpu_mask_count(mask_all) - __builtin_popcount(run_bitfield[each_slice]);
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_launch_thread_on_dpu(struct dpu_t *dpu, dpu_thread_t thread, bool should_resume, bool *thread_was_running)
{
    LOG_DPU(VERBOSE, dpu, "%d, %d", thread, should_resume);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    verify_thread_id(thread, rank);

    dpu_error_t status;

    dpu_lock_rank(rank);

    bool dpu_is_running;
    bool dpu_is_in_fault;

    FF(dpu_poll_dpu(dpu, &dpu_is_running, &dpu_is_in_fault));

    if (!should_resume && (rank->profiling_context.dpu == dpu)) {
        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS:
                FF(dpu_set_magic_profiling_for_dpu(dpu));
                break;
            case DPU_PROFILING_SAMPLES:
                memset(rank->profiling_context.sample_stats,
                    0,
                    rank->description->memories.iram_size * sizeof(*(rank->profiling_context.sample_stats)));
                break;
        }
    }

    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);
    uint8_t mask = CI_MASK_ONE(slice_id);
    uint8_t result_array[DPU_MAX_NR_CIS];

    FF(ufi_select_dpu(rank, &mask, member_id));

    if (should_resume) {
        FF(ufi_thread_resume(rank, mask, thread, result_array));
    } else {
        FF(ufi_thread_boot(rank, mask, thread, result_array));
    }

    uint8_t result = result_array[slice_id];

    *thread_was_running = (result & mask_one) != 0;

    if (!dpu_is_running) {
        rank->runtime.run_context.dpu_running[slice_id] |= mask_one;
        rank->runtime.run_context.nb_dpu_running++;
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_poll_rank(struct dpu_rank_t *rank, dpu_bitfield_t *dpu_is_running, dpu_bitfield_t *dpu_is_in_fault)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status;
    dpu_slice_id_t slice_id_profiling = dpu_get_slice_id(rank->profiling_context.dpu);
    dpu_member_id_t dpu_id_profiling = dpu_get_member_id(rank->profiling_context.dpu);
    uint8_t nr_threads = rank->description->dpu.nr_of_threads;
    uint32_t profiled_address[nr_threads];

    uint8_t mask = ALL_CIS;

    dpu_lock_rank(rank);
    FF(ufi_select_all(rank, &mask));
    FF(ufi_read_dpu_run(rank, mask, dpu_is_running));
    FF(ufi_read_dpu_fault(rank, mask, dpu_is_in_fault));

    switch (rank->profiling_context.enable_profiling) {
        default:
            break;
        case DPU_PROFILING_STATS: {
            memset(profiled_address, 0, nr_threads * sizeof(uint32_t));
            dpuword_t *wram_array[DPU_MAX_NR_CIS];
            wram_array[slice_id_profiling] = profiled_address;

            uint8_t ci_mask = CI_MASK_ONE(slice_id_profiling);
            FF(ufi_select_dpu(rank, &ci_mask, dpu_id_profiling));
            FF(ufi_wram_read(
                rank, ci_mask, wram_array, (wram_addr_t)(rank->profiling_context.thread_profiling_address / 4), nr_threads));

            dpu_selected_mask_t mask_one = dpu_mask_one(dpu_id_profiling);
            bool dpu_profiling_is_running, dpu_profiling_is_in_fault;

            dpu_collect_statistics_profiling(rank->profiling_context.dpu, nr_threads, profiled_address);

            dpu_profiling_is_running = (dpu_is_running[slice_id_profiling] & mask_one) != 0;
            dpu_profiling_is_in_fault = (dpu_is_in_fault[slice_id_profiling] & mask_one) != 0;

            if ((!dpu_profiling_is_running || dpu_profiling_is_in_fault)
                && (rank->runtime.run_context.dpu_running[slice_id_profiling] & mask_one)) {
                dpu_dump_statistics_profiling(rank->profiling_context.dpu, nr_threads);
            }

            break;
        }
        case DPU_PROFILING_SAMPLES: {
            uint8_t ci_mask = CI_MASK_ONE(slice_id_profiling);
            iram_addr_t pc_array[DPU_MAX_NR_CIS];

            FF(ufi_select_dpu(rank, &ci_mask, dpu_id_profiling));
            FF(ufi_debug_pc_sample(rank, ci_mask));
            FF(ufi_debug_pc_read(rank, ci_mask, pc_array));

            dpu_selected_mask_t mask_one = dpu_mask_one(dpu_id_profiling);
            bool dpu_profiling_is_running, dpu_profiling_is_in_fault;

            iram_addr_t sampled_address = pc_array[slice_id_profiling];

            dpu_collect_samples_profiling(rank->profiling_context.dpu, sampled_address);

            dpu_profiling_is_running = (dpu_is_running[slice_id_profiling] & mask_one) != 0;
            dpu_profiling_is_in_fault = (dpu_is_in_fault[slice_id_profiling] & mask_one) != 0;

            if ((!dpu_profiling_is_running || dpu_profiling_is_in_fault)
                && (rank->runtime.run_context.dpu_running[slice_id_profiling] & mask_one)) {
                dpu_dump_samples_profiling(rank->profiling_context.dpu);
            }

            break;
        }
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;

        dpu_is_running[each_slice] &= mask_all;
        dpu_is_in_fault[each_slice] &= mask_all;

        dpu_bitfield_t dpu_not_running = ~dpu_is_running[each_slice] | dpu_is_in_fault[each_slice];
        dpu_bitfield_t dpu_running = ~dpu_not_running & mask_all;
        dpu_bitfield_t dpu_was_running = rank->runtime.run_context.dpu_running[each_slice];

        rank->runtime.run_context.nb_dpu_running -= (__builtin_popcount(dpu_was_running) - __builtin_popcount(dpu_running));
        rank->runtime.run_context.dpu_running[each_slice] = dpu_running;
        rank->runtime.run_context.dpu_in_fault[each_slice] = dpu_is_in_fault[each_slice] & mask_all;
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_poll_dpu(struct dpu_t *dpu, bool *dpu_is_running, bool *dpu_is_in_fault)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_bitfield_t is_running_result[DPU_MAX_NR_CIS];
    dpu_bitfield_t is_in_fault_result[DPU_MAX_NR_CIS];
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);
    uint8_t nr_threads = rank->description->dpu.nr_of_threads;
    uint32_t profiled_address[nr_threads];

    uint8_t mask = CI_MASK_ONE(slice_id);

    dpu_lock_rank(rank);
    FF(ufi_select_dpu(rank, &mask, member_id));
    FF(ufi_read_dpu_run(rank, mask, is_running_result));
    FF(ufi_read_dpu_fault(rank, mask, is_in_fault_result));

    if (rank->profiling_context.dpu == dpu) {
        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS: {
                memset(profiled_address, 0, nr_threads * sizeof(uint32_t));
                dpuword_t *wram_array[DPU_MAX_NR_CIS];
                wram_array[slice_id] = profiled_address;

                FF(ufi_wram_read(
                    rank, mask, wram_array, (wram_addr_t)(rank->profiling_context.thread_profiling_address / 4), nr_threads));
                dpu_collect_statistics_profiling(dpu, nr_threads, profiled_address);

                break;
            }
            case DPU_PROFILING_SAMPLES: {
                iram_addr_t pc_array[DPU_MAX_NR_CIS];

                FF(ufi_debug_pc_sample(rank, mask));
                FF(ufi_debug_pc_read(rank, mask, pc_array));

                iram_addr_t sampled_address = pc_array[slice_id];
                dpu_collect_samples_profiling(dpu, sampled_address);
                break;
            }
        }
    }

    *dpu_is_running = (is_running_result[slice_id] & mask_one) != 0;
    *dpu_is_in_fault = (is_in_fault_result[slice_id] & mask_one) != 0;

    if ((!*dpu_is_running || *dpu_is_in_fault) && (rank->runtime.run_context.dpu_running[slice_id] & mask_one)) {
        rank->runtime.run_context.dpu_running[slice_id] &= ~mask_one;
        rank->runtime.run_context.nb_dpu_running--;

        if (*dpu_is_in_fault) {
            rank->runtime.run_context.dpu_in_fault[slice_id] |= mask_one;
        }

        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS:
                dpu_dump_statistics_profiling(dpu, nr_threads);
                break;
            case DPU_PROFILING_SAMPLES:
                dpu_dump_samples_profiling(dpu);
                break;
        }
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_thread_status_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, dpu_bitfield_t *thread_is_running)
{
    LOG_RANK(VERBOSE, rank, "%d", thread);

    verify_thread_id(thread, rank);

    dpu_error_t status;

    uint8_t mask = ALL_CIS;
    dpu_lock_rank(rank);
    FF(ufi_select_all(rank, &mask));
    FF(ufi_read_run_bit(rank, mask, thread, thread_is_running));

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        thread_is_running[each_slice] &= rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_thread_status_on_dpu(struct dpu_t *dpu, dpu_thread_t thread, bool *thread_is_running)
{
    LOG_DPU(VERBOSE, dpu, "%d", thread);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    verify_thread_id(thread, rank);

    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    dpu_bitfield_t result[DPU_MAX_NR_CIS];

    dpu_lock_rank(rank);
    uint8_t mask = CI_MASK_ONE(slice_id);
    FF(ufi_select_dpu(rank, &mask, member_id));
    FF(ufi_read_run_bit(rank, mask, thread, result));

    *thread_is_running = (result[slice_id] & mask_one) != 0;

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_and_update_notify_status_on_rank(struct dpu_rank_t *rank,
    dpu_notify_bit_id_t notify_bit,
    bool value,
    dpu_bitfield_t *notify_bit_was_set)
{
    LOG_RANK(VERBOSE, rank, "%d", notify_bit);

    verify_notify_id(notify_bit, rank);

    dpu_notify_bit_id_t run_bit = rank->description->dpu.nr_of_threads + notify_bit;
    dpu_error_t status;

    dpu_lock_rank(rank);
    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    if (value) {
        FF(ufi_thread_boot(rank, mask, run_bit, notify_bit_was_set));
    } else {
        FF(ufi_clear_run_bit(rank, mask, run_bit, notify_bit_was_set));
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;
        notify_bit_was_set[each_slice] &= mask_all;
    }

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_and_update_notify_status_on_dpu(struct dpu_t *dpu, dpu_notify_bit_id_t notify_bit, bool value, bool *notify_bit_was_set)
{
    LOG_DPU(VERBOSE, dpu, "%d", notify_bit);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    verify_notify_id(notify_bit, rank);

    dpu_notify_bit_id_t run_bit = rank->description->dpu.nr_of_threads + notify_bit;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    dpu_lock_rank(rank);

    dpu_bitfield_t result[DPU_MAX_NR_CIS];
    uint8_t mask = CI_MASK_ONE(slice_id);
    FF(ufi_select_dpu(rank, &mask, member_id));

    if (value) {
        FF(ufi_thread_boot(rank, mask, run_bit, result));
    } else {
        FF(ufi_clear_run_bit(rank, mask, run_bit, result));
    }

    *notify_bit_was_set = (result[slice_id] & mask_one) != 0;

end:
    dpu_unlock_rank(rank);
    return status;
}
