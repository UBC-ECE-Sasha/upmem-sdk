/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <dpu_error.h>
#include <dpu_debug.h>
#include <dpu_management.h>

#include <dpu_api_log.h>
#include <dpu_rank.h>
#include <dpu_mask.h>
#include <ufi_utils.h>
#include <verbose_control.h>

dpu_error_t
drain_pipeline(struct dpu_t *dpu, dpu_context_t context, dpu_pc_mode_e pc_mode, bool should_add_to_schedule)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status = DPU_OK;
    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    bool still_draining;
    uint8_t lsb_pc;
    uint8_t msb_pc;
    uint8_t nr_of_bits_in_lsb_pc;
    uint32_t *previous_run_register;
    uint32_t *run_register;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if ((previous_run_register = malloc(nr_of_threads_per_dpu * sizeof(*previous_run_register))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }
    if ((run_register = malloc(nr_of_threads_per_dpu * sizeof(*run_register))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_previous_run_register;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_run_register;
    }

    // 1. Fetching initial RUN register
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        safe_add_query(query,
            dpu_query_build_read_run_thread_for_dpu(slice_id, member_id, each_thread, previous_run_register + each_thread),
            transaction,
            status,
            free_transaction);
    }
    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free(transaction);

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

    // 2. Initial RUN register can be null: draining is done
    still_draining = false;
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        if ((previous_run_register[each_thread] & mask_one) != 0) {
            still_draining = true;
            break;
        }
    }

    if (still_draining) {
        // 3. Looping until there is no more running thread
        if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
            status = DPU_ERR_SYSTEM;
            goto free_run_register;
        }
        // 3.1 Set replacing instruction to a STOP
        safe_add_query(
            query, dpu_query_build_debug_std_replace_stop_enabled_for_previous(slice_id), transaction, status, free_transaction);
        // 3.2 Popping out a thread from the FIFO
        safe_add_query(
            query, dpu_query_build_set_and_step_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);
        // 3.3 Fetching potential PC (if a thread has been popped out)
        safe_add_query(query, dpu_query_build_read_pc_lsb_for_previous(slice_id, &lsb_pc), transaction, status, free_transaction);
        safe_add_query(query, dpu_query_build_read_pc_msb_for_previous(slice_id, &msb_pc), transaction, status, free_transaction);
        // 3.4 Fetching new RUN register
        for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
            safe_add_query(query,
                dpu_query_build_read_run_thread_for_previous(slice_id, each_thread, run_register + each_thread),
                transaction,
                status,
                free_transaction);
        }

        do {
            safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

            still_draining = false;
            for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
                if ((run_register[each_thread] & mask_one) != 0) {
                    still_draining = true;
                } else if (context && (previous_run_register[each_thread] & mask_one) != 0) {
                    context->pcs[each_thread] = lsb_pc | (msb_pc << nr_of_bits_in_lsb_pc);
                    if (should_add_to_schedule) {
                        context->scheduling[each_thread] = context->nr_of_running_threads++;
                    }
                    previous_run_register[each_thread] = 0;
                }
            }
        } while (still_draining);

        dpu_transaction_free(transaction);

        // 4. Clear dbg_replace_en
        if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
            status = DPU_ERR_SYSTEM;
            goto free_run_register;
        }

        safe_add_query(
            query, dpu_query_build_debug_std_replace_clear_for_previous(slice_id), transaction, status, free_transaction);

        safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

        dpu_transaction_free(transaction);
    }

    goto free_run_register;

free_transaction:
    dpu_transaction_free(transaction);
free_run_register:
    free(run_register);
free_previous_run_register:
    free(previous_run_register);
end:
    return status;
}

bool
fetch_natural_pc_mode(struct dpu_rank_t *rank, dpu_pc_mode_e *pc_mode)
{
    switch (rank->description->memories.iram_size) {
        case 1 << 12:
            *pc_mode = DPU_PC_12;
            break;
        case 1 << 13:
            *pc_mode = DPU_PC_13;
            break;
        case 1 << 14:
            *pc_mode = DPU_PC_14;
            break;
        case 1 << 15:
            *pc_mode = DPU_PC_15;
            break;
        default:
            *pc_mode = DPU_PC_16;
            break;
    }

    return true;
}

void
set_pc_in_core_dump_or_restore_registers(dpu_thread_t thread,
    iram_addr_t pc,
    dpuinstruction_t *program,
    iram_size_t program_size,
    uint8_t nr_of_threads)
{
    uint32_t upper_index = (uint32_t)(program_size - (12 * (nr_of_threads - thread - 1)));

    program[upper_index - 1] |= pc;
    program[upper_index - 4] |= pc;
    program[upper_index - 7] |= pc;
    program[upper_index - 10] |= pc;
}

dpu_clock_division_t
from_division_factor_to_dpu_enum(uint8_t factor)
{
    switch (factor) {
        default:
            return DPU_CLOCK_DIV2;
        case 3:
            return DPU_CLOCK_DIV3;
        case 4:
            return DPU_CLOCK_DIV4;
        case 8:
            return DPU_CLOCK_DIV8;
    }
}
