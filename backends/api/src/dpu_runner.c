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
#include <dpu_description.h>
#include <dpu_management.h>
#include "include/dpu_internals.h"
#include "dpu_mask.h"
#include "dpu_rank.h"
#include "include/dpu_api_log.h"
#include "include/ufi_utils.h"

dpu_run_context_t
dpu_get_run_context(struct dpu_rank_t *rank)
{
    return &rank->runtime.run_context;
}

__API_SYMBOL__ dpu_error_t
dpu_launch_thread_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, bool should_resume, dpu_bitfield_t *thread_was_running)
{
    LOG_RANK(VERBOSE, rank, "%d, %d", thread, should_resume);

    verify_thread_id(thread, rank);
    dpu_error_t status;

    dpu_lock_rank(rank);

    dpu_bitfield_t *run_bitfield;
    dpu_bitfield_t *fault_bitfield;

    if ((run_bitfield = calloc(rank->description->topology.nr_of_control_interfaces, sizeof(*run_bitfield))) == NULL) {
        dpu_unlock_rank(rank);
        return DPU_ERR_SYSTEM;
    }

    if ((fault_bitfield = calloc(rank->description->topology.nr_of_control_interfaces, sizeof(*fault_bitfield))) == NULL) {
        free(run_bitfield);
        dpu_unlock_rank(rank);
        return DPU_ERR_SYSTEM;
    }

    if ((status = dpu_poll_rank(rank, run_bitfield, fault_bitfield)) != DPU_OK) {
        free(run_bitfield);
        free(fault_bitfield);
        dpu_unlock_rank(rank);
        return status;
    }
    free(fault_bitfield);

    dpu_transaction_t transaction;

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

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        free(run_bitfield);
        dpu_unlock_rank(rank);
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_query_t query;

        if (rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled) {
            query = dpu_query_build_select_all_for_control(each_slice);
        } else if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus != dpu_mask_empty()) {
            query = dpu_query_build_select_group_for_control(each_slice, DPU_ENABLED_GROUP);
        } else {
            continue;
        }

        if (query == NULL) {
            dpu_transaction_free(transaction);
            free(run_bitfield);
            dpu_unlock_rank(rank);
            return DPU_ERR_SYSTEM;
        }

        dpu_transaction_add_query_tail(transaction, query);

        if (should_resume) {
            query = dpu_query_build_resume_thread_for_previous(each_slice, thread, thread_was_running + each_slice);
        } else {
            query = dpu_query_build_boot_thread_for_previous(each_slice, thread, thread_was_running + each_slice);
        }

        if (query == NULL) {
            dpu_transaction_free(transaction);
            free(run_bitfield);
            dpu_unlock_rank(rank);
            return DPU_ERR_SYSTEM;
        }

        dpu_transaction_add_query_tail(transaction, query);
    }

    dpu_planner_status_e planner_status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    if (planner_status != DPU_PLANNER_SUCCESS) {
        dpu_transaction_free(transaction);
        free(run_bitfield);
        dpu_unlock_rank(rank);

        return map_planner_status_to_api_status(planner_status);
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;

        thread_was_running[each_slice] &= mask_all;
        run_bitfield[each_slice] &= mask_all;

        rank->runtime.run_context.dpu_running[each_slice] = mask_all;
        rank->runtime.run_context.nb_dpu_running += dpu_mask_count(mask_all) - __builtin_popcount(run_bitfield[each_slice]);
    }

    free(run_bitfield);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);
    return DPU_OK;
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

    dpu_transaction_t transaction;
    dpu_error_t status;

    dpu_lock_rank(rank);

    bool dpu_is_running;
    bool dpu_is_in_fault;

    if ((status = dpu_poll_dpu(dpu, &dpu_is_running, &dpu_is_in_fault)) != DPU_OK) {
        dpu_unlock_rank(rank);
        return status;
    }

    if (!should_resume && (rank->profiling_context.dpu == dpu)) {
        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS:
                status = dpu_set_magic_profiling_for_dpu(dpu);
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

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        dpu_unlock_rank(rank);
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query;
    uint32_t result;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if (should_resume) {
        query = dpu_query_build_resume_thread_for_dpu(slice_id, member_id, thread, &result);
    } else {
        query = dpu_query_build_boot_thread_for_dpu(slice_id, member_id, thread, &result);
    }

    if (query == NULL) {
        dpu_transaction_free(transaction);
        dpu_unlock_rank(rank);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_planner_status_e planner_status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    if (planner_status != DPU_PLANNER_SUCCESS) {
        dpu_transaction_free(transaction);
        dpu_unlock_rank(rank);
        return map_planner_status_to_api_status(planner_status);
    }

    *thread_was_running = (result & mask_one) != 0;

    if (!dpu_is_running) {
        rank->runtime.run_context.dpu_running[slice_id] |= mask_one;
        rank->runtime.run_context.nb_dpu_running++;
    }
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_poll_rank(struct dpu_rank_t *rank, dpu_bitfield_t *dpu_is_running, dpu_bitfield_t *dpu_is_in_fault)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status = DPU_OK;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_slice_id_t slice_id_profiling = dpu_get_slice_id(rank->profiling_context.dpu);
    dpu_member_id_t dpu_id_profiling = dpu_get_member_id(rank->profiling_context.dpu);
    uint8_t nr_threads = rank->description->dpu.nr_of_threads;
    uint32_t profiled_address[nr_threads];
    uint8_t profiled_address_lsb = 0, profiled_address_msb = 0;

    dpu_pc_mode_e pc_mode;

    fetch_natural_pc_mode(rank, &pc_mode);

    uint8_t nr_of_bits_in_lsb_pc;
    switch (pc_mode) {
        case DPU_PC_12:
            nr_of_bits_in_lsb_pc = 6;
            break;
        case DPU_PC_13:
            nr_of_bits_in_lsb_pc = 7;
            break;
        case DPU_PC_14:
            nr_of_bits_in_lsb_pc = 7;
            break;
        case DPU_PC_15:
            nr_of_bits_in_lsb_pc = 8;
            break;
        case DPU_PC_16:
            nr_of_bits_in_lsb_pc = 8;
            break;
        default:
            nr_of_bits_in_lsb_pc = 8;
            break;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        if (rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled) {
            safe_add_query(query, dpu_query_build_select_all_for_control(each_slice), transaction, status, free_transaction);
        } else if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus != dpu_mask_empty()) {
            safe_add_query(query,
                dpu_query_build_select_group_for_control(each_slice, DPU_ENABLED_GROUP),
                transaction,
                status,
                free_transaction);
        } else {
            continue;
        }

        safe_add_query(query,
            dpu_query_build_read_dpu_run_state_for_previous(each_slice, dpu_is_running + each_slice),
            transaction,
            status,
            free_transaction);
        safe_add_query(query,
            dpu_query_build_read_dpu_fault_state_for_previous(each_slice, dpu_is_in_fault + each_slice),
            transaction,
            status,
            free_transaction);
    }

    switch (rank->profiling_context.enable_profiling) {
        default:
            break;
        case DPU_PROFILING_STATS:
            memset(profiled_address, 0, nr_threads * sizeof(uint32_t));

            safe_add_query(query,
                dpu_query_build_read_wram_word_for_dpu(slice_id_profiling,
                    dpu_id_profiling,
                    (wram_addr_t)(rank->profiling_context.thread_profiling_address / 4),
                    nr_threads,
                    profiled_address),
                transaction,
                status,
                free_transaction);
            break;
        case DPU_PROFILING_SAMPLES:
            safe_add_query(query,
                dpu_query_build_select_dpu_for_control(slice_id_profiling, dpu_id_profiling),
                transaction,
                status,
                free_transaction);
            safe_add_query(query,
                dpu_query_build_debug_std_sample_pc_for_previous(slice_id_profiling),
                transaction,
                status,
                free_transaction);
            safe_add_query(query,
                dpu_query_build_read_pc_lsb_for_previous(slice_id_profiling, &profiled_address_lsb),
                transaction,
                status,
                free_transaction);
            safe_add_query(query,
                dpu_query_build_read_pc_msb_for_previous(slice_id_profiling, &profiled_address_msb),
                transaction,
                status,
                free_transaction);
            break;
    }

    dpu_lock_rank(rank);
    dpu_planner_status_e planner_status;
    safe_execute_transaction(transaction, rank, planner_status, status, unlock_rank);

    switch (rank->profiling_context.enable_profiling) {
        default:
            break;
        case DPU_PROFILING_STATS: {
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
            dpu_selected_mask_t mask_one = dpu_mask_one(dpu_id_profiling);
            bool dpu_profiling_is_running, dpu_profiling_is_in_fault;

            iram_addr_t sampled_address = profiled_address_lsb | (profiled_address_msb << nr_of_bits_in_lsb_pc);

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

unlock_rank:
    dpu_unlock_rank(rank);

free_transaction:
    dpu_transaction_free(transaction);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_poll_dpu(struct dpu_t *dpu, bool *dpu_is_running, bool *dpu_is_in_fault)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status = DPU_OK;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_pc_mode_e pc_mode;

    fetch_natural_pc_mode(rank, &pc_mode);

    uint8_t nr_of_bits_in_lsb_pc;
    switch (pc_mode) {
        case DPU_PC_12:
            nr_of_bits_in_lsb_pc = 6;
            break;
        case DPU_PC_13:
            nr_of_bits_in_lsb_pc = 7;
            break;
        case DPU_PC_14:
            nr_of_bits_in_lsb_pc = 7;
            break;
        case DPU_PC_15:
            nr_of_bits_in_lsb_pc = 8;
            break;
        case DPU_PC_16:
            nr_of_bits_in_lsb_pc = 8;
            break;
        default:
            nr_of_bits_in_lsb_pc = 8;
            break;
    }

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query;
    uint32_t is_running_result;
    uint32_t is_in_fault_result;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);
    uint8_t nr_threads = rank->description->dpu.nr_of_threads;
    uint32_t profiled_address[nr_threads];
    uint8_t profiled_address_lsb = 0, profiled_address_msb = 0;

    safe_add_query(query,
        dpu_query_build_read_dpu_run_state_for_dpu(slice_id, member_id, &is_running_result),
        transaction,
        status,
        free_transaction);
    safe_add_query(query,
        dpu_query_build_read_dpu_fault_state_for_previous(slice_id, &is_in_fault_result),
        transaction,
        status,
        free_transaction);

    if (rank->profiling_context.dpu == dpu) {
        switch (rank->profiling_context.enable_profiling) {
            default:
                break;
            case DPU_PROFILING_STATS:
                memset(profiled_address, 0, nr_threads * sizeof(uint32_t));

                safe_add_query(query,
                    dpu_query_build_read_wram_word_for_previous(slice_id,
                        (wram_addr_t)(rank->profiling_context.thread_profiling_address / 4),
                        nr_threads,
                        profiled_address),
                    transaction,
                    status,
                    free_transaction);
                break;
            case DPU_PROFILING_SAMPLES:
                safe_add_query(
                    query, dpu_query_build_debug_std_sample_pc_for_previous(slice_id), transaction, status, free_transaction);
                safe_add_query(query,
                    dpu_query_build_read_pc_lsb_for_previous(slice_id, &profiled_address_lsb),
                    transaction,
                    status,
                    free_transaction);
                safe_add_query(query,
                    dpu_query_build_read_pc_msb_for_previous(slice_id, &profiled_address_msb),
                    transaction,
                    status,
                    free_transaction);
                break;
        }
    }

    dpu_lock_rank(rank);
    dpu_planner_status_e planner_status;
    safe_execute_transaction(transaction, rank, planner_status, status, unlock_rank);

    switch (rank->profiling_context.enable_profiling) {
        default:
            break;
        case DPU_PROFILING_STATS:
            dpu_collect_statistics_profiling(dpu, nr_threads, profiled_address);
            break;
        case DPU_PROFILING_SAMPLES: {
            iram_addr_t sampled_address = profiled_address_lsb | (profiled_address_msb << nr_of_bits_in_lsb_pc);
            dpu_collect_samples_profiling(dpu, sampled_address);
            break;
        }
    }

    *dpu_is_running = (is_running_result & mask_one) != 0;
    *dpu_is_in_fault = (is_in_fault_result & mask_one) != 0;

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

unlock_rank:
    dpu_unlock_rank(rank);
free_transaction:
    dpu_transaction_free(transaction);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_thread_status_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, dpu_bitfield_t *thread_is_running)
{
    LOG_RANK(VERBOSE, rank, "%d", thread);

    verify_thread_id(thread, rank);

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
            dpu_query_build_read_run_thread_for_previous(each_slice, thread, thread_is_running + each_slice),
            transaction,
            status,
            err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        thread_is_running[each_slice] &= rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;
    }

err:
    dpu_transaction_free(transaction);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_thread_status_on_dpu(struct dpu_t *dpu, dpu_thread_t thread, bool *thread_is_running)
{
    LOG_DPU(VERBOSE, dpu, "%d", thread);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    verify_thread_id(thread, rank);

    dpu_transaction_t transaction;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    uint32_t result;
    dpu_query_t query = dpu_query_build_read_run_thread_for_dpu(slice_id, member_id, thread, &result);

    if (query == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    *thread_is_running = (result & mask_one) != 0;

    return map_planner_status_to_api_status(status);
}

__API_SYMBOL__ dpu_error_t
dpu_get_and_update_notify_status_on_rank(struct dpu_rank_t *rank,
    dpu_notify_bit_id_t notify_bit,
    bool value,
    dpu_bitfield_t *notify_bit_was_set)
{
    LOGV(__vc(), "%s (%04x, %d)", __func__, rank->rank_id, notify_bit);

    verify_notify_id(notify_bit, rank);

    dpu_planner_status_e planner_status;
    dpu_error_t status = DPU_OK;
    dpu_query_t (*notify_query_builder)(dpu_slice_id_t, dpu_notify_bit_id_t, uint32_t *)
        = value ? dpu_query_build_read_and_set_notify_bit_for_previous : dpu_query_build_read_and_clear_notify_bit_for_previous;
    dpu_notify_bit_id_t real_notify_bit = rank->description->dpu.nr_of_threads + notify_bit;
    dpu_transaction_t transaction;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus == dpu_mask_empty()) {
            continue;
        }

        build_select_query_for_all_enabled_dpus(rank, each_slice, query, transaction, status, err);
        safe_add_query(
            query, notify_query_builder(each_slice, real_notify_bit, notify_bit_was_set + each_slice), transaction, status, err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;
        notify_bit_was_set[each_slice] &= mask_all;
    }

err:
    dpu_transaction_free(transaction);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_get_and_update_notify_status_on_dpu(struct dpu_t *dpu, dpu_notify_bit_id_t notify_bit, bool value, bool *notify_bit_was_set)
{
    LOGV(__vc(), "%s (%08x, %d)", __func__, dpu_get_id(dpu), notify_bit);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    verify_notify_id(notify_bit, rank);

    dpu_notify_bit_id_t real_notify_bit = rank->description->dpu.nr_of_threads + notify_bit;
    dpu_transaction_t transaction;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    uint32_t result;
    dpu_query_t query = value ? dpu_query_build_read_and_set_notify_bit_for_dpu(slice_id, member_id, real_notify_bit, &result)
                              : dpu_query_build_read_and_clear_notify_bit_for_dpu(slice_id, member_id, real_notify_bit, &result);

    if (query == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    dpu_lock_rank(rank);
    dpu_planner_status_e status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
    dpu_unlock_rank(rank);

    dpu_transaction_free(transaction);

    *notify_bit_was_set = (result & mask_one) != 0;

    return map_planner_status_to_api_status(status);
}
