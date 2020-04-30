/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <string.h>
#include <sys/ptrace.h>
#include <sys/param.h>
#include <errno.h>

#include <dpu_debug.h>
#include <dpu_management.h>

#include <dpu_api_log.h>
#include <dpu_attributes.h>
#include <dpu_commands.h>
#include <dpu_config.h>
#include <dpu_instruction_encoder.h>
#include <dpu_internals.h>
#include <dpu_mask.h>
#include <dpu_predef_programs.h>
#include <dpu_rank.h>
#include <ufi_utils.h>
#include <verbose_control.h>
#include <dpu_memory.h>
#include <dpu_elf.h>

#define DPU_FAULT_UNKNOWN_ID (0xffffff)

static dpu_error_t
extract_bkp_fault_id(struct dpu_t *dpu, iram_addr_t pc, uint32_t *bkp_fault_id);
static dpu_error_t
decrement_thread_pc(struct dpu_t *dpu, dpu_thread_t thread, iram_addr_t *pc);

__API_SYMBOL__ dpu_error_t
dpu_trigger_fault_on_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

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
        safe_add_query(query, dpu_query_build_set_and_step_dpu_fault_state_for_previous(each_slice), transaction, status, err);
        safe_add_query(query, dpu_query_build_set_bkp_fault_for_previous(each_slice), transaction, status, err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

err:
    dpu_transaction_free(transaction);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_trigger_fault_on_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    dpu_query_t query = dpu_query_build_set_and_step_dpu_fault_state_for_dpu(slice_id, member_id);

    if (query == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    dpu_transaction_add_query_tail(transaction, query);

    query = dpu_query_build_set_bkp_fault_for_dpu(slice_id, member_id);

    if (query == NULL) {
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
dpu_clear_fault_on_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    dpu_error_t status = DPU_OK;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        uint32_t ignored;

        if (rank->runtime.control_interface.slice_info[each_slice].enabled_dpus == dpu_mask_empty()) {
            continue;
        }

        build_select_query_for_all_enabled_dpus(rank, each_slice, query, transaction, status, err);
        /* Reading BKP fault clears potential INTERCEPT fault */
        safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(each_slice, &ignored), transaction, status, err);
        safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(each_slice), transaction, status, err);
    }

    dpu_lock_rank(rank);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_unlock_rank(rank);

err:
    dpu_transaction_free(transaction);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_clear_fault_on_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_transaction_t transaction;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        return DPU_ERR_SYSTEM;
    }

    uint32_t ignored;
    /* Reading BKP fault clears potential INTERCEPT fault */
    dpu_query_t query = dpu_query_build_read_bkp_fault_for_dpu(slice_id, member_id, &ignored);

    if (query == NULL) {
        dpu_transaction_free(transaction);
        return DPU_ERR_SYSTEM;
    }

    query = dpu_query_build_clear_dpu_fault_state_for_dpu(slice_id, member_id);

    if (query == NULL) {
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
dpu_extract_pcs_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    dpu_pc_mode_e pc_mode = DPU_PC_12;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;

    dpu_lock_rank(rank);
    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 1. Fetch PC mode
    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto free_transaction;
    }

    // 2. Set fault
    safe_add_query(
        query, dpu_query_build_set_and_step_dpu_fault_state_for_dpu(slice_id, member_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_set_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);

    // 3. Loop on each thread
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        // 3.1 Resume thread
        safe_add_query(query,
            dpu_query_build_resume_thread_for_previous(slice_id, each_thread, &ignored),
            transaction,
            status,
            free_transaction);
    }
    // Interception Fault Clear
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free(transaction);

    // 4. Drain Pipeline
    drain_pipeline(dpu, context, pc_mode, false);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 5. Clear fault
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

free_transaction:
    dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

typedef dpuinstruction_t *(*fetch_program_t)(iram_size_t *);
typedef dpu_error_t (*routine_t)(dpu_query_t,
    dpu_slice_id_t,
    dpu_member_id_t,
    dpu_transaction_t,
    struct dpu_rank_t *,
    dpuword_t *,
    dpu_context_t,
    wram_size_t,
    uint32_t,
    uint8_t,
    uint8_t,
    wram_size_t);

static dpu_error_t
dpu_boot_and_wait_for_dpu(dpu_query_t query,
    dpu_slice_id_t slice_id,
    dpu_member_id_t member_id,
    dpu_transaction_t transaction,
    struct dpu_rank_t *rank)
{
    uint32_t ignored;
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    uint32_t run_state_result;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    // 1. Boot thread 0
    safe_add_query(query,
        dpu_query_build_boot_thread_for_previous(slice_id, 0, &ignored),
        transaction,
        status,
        dpu_boot_and_wait_for_dpu_exit);

    safe_execute_transaction(transaction, rank, planner_status, status, dpu_boot_and_wait_for_dpu_exit);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 2. Wait for end of program
    safe_add_query(query,
        dpu_query_build_read_dpu_run_state_for_previous(slice_id, &run_state_result),
        transaction,
        status,
        dpu_boot_and_wait_for_dpu_exit);

    do {
        run_state_result = 0;
        safe_execute_transaction(transaction, rank, planner_status, status, dpu_boot_and_wait_for_dpu_exit);
    } while ((run_state_result & mask_one) != 0);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

dpu_boot_and_wait_for_dpu_exit:
    return status;
}

static dpu_error_t
dpu_execute_routine_for_dpu(struct dpu_t *dpu,
    dpu_context_t context,
    fetch_program_t fetch_program,
    routine_t routine,
    dpu_event_kind_t custom_event)
{
    LOG_DPU(VERBOSE, dpu, "");

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    dpu_selected_mask_t selected_mask = dpu_mask_one(member_id);
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    iram_size_t program_size_in_instructions;
    wram_size_t context_size_in_words;
    dpuinstruction_t *iram_backup;
    dpuword_t *wram_backup;
    dpuword_t *raw_context;

    dpu_lock_rank(rank);

    if ((status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)custom_event)) != DPU_OK) {
        goto end;
    }

    // 1. Save IRAM
    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    dpuinstruction_t *program = fetch_program(&program_size_in_instructions);

    if (program == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_transaction;
    }

    if ((iram_backup = malloc(program_size_in_instructions * sizeof(*iram_backup))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_program;
    }

    safe_add_query(query,
        dpu_query_build_read_iram_instruction_for_dpu(slice_id, member_id, 0, program_size_in_instructions, iram_backup),
        transaction,
        status,
        free_iram_backup);

    // 2. Save WRAM
    uint32_t nr_of_atomic_bits_per_dpu = rank->description->dpu.nr_of_atomic_bits;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    uint8_t nr_of_work_registers_per_thread = rank->description->dpu.nr_of_work_registers_per_thread;
    wram_size_t atomic_register_size_in_words = nr_of_atomic_bits_per_dpu / sizeof(dpuword_t);
    context_size_in_words = atomic_register_size_in_words + (nr_of_threads_per_dpu * (nr_of_work_registers_per_thread + 1));

    if ((wram_backup = malloc(context_size_in_words * sizeof(*wram_backup))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_iram_backup;
    }

    safe_add_query(query,
        dpu_query_build_read_wram_word_for_previous(slice_id, 0, context_size_in_words, wram_backup),
        transaction,
        status,
        free_wram_backup);

    // 3. Insert PCs in core dump program
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        set_pc_in_core_dump_or_restore_registers(
            each_thread, context->pcs[each_thread], program, program_size_in_instructions, nr_of_threads_per_dpu);
    }

    // 4. Load IRAM with core dump program
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(slice_id, selected_mask, 0, program, program_size_in_instructions),
        transaction,
        status,
        free_wram_backup);

    safe_execute_transaction(transaction, rank, planner_status, status, free_wram_backup);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 5. Execute routine
    if ((raw_context = malloc(context_size_in_words * sizeof(*raw_context))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto free_wram_backup;
    }

    status = routine(query,
        slice_id,
        member_id,
        transaction,
        rank,
        raw_context,
        context,
        context_size_in_words,
        nr_of_atomic_bits_per_dpu,
        nr_of_threads_per_dpu,
        nr_of_work_registers_per_thread,
        atomic_register_size_in_words);

    free(raw_context);
    if (status != DPU_OK)
        goto free_wram_backup;

    // 6. Restore WRAM
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(slice_id, selected_mask, 0, wram_backup, context_size_in_words),
        transaction,
        status,
        free_wram_backup);

    // 7. Restore IRAM
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(
            slice_id, selected_mask, 0, iram_backup, program_size_in_instructions),
        transaction,
        status,
        free_wram_backup);

    safe_execute_transaction(transaction, rank, planner_status, status, free_wram_backup);

    status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)custom_event);

free_wram_backup:
    free(wram_backup);
free_iram_backup:
    free(iram_backup);
free_program:
    free(program);
free_transaction:
    if (transaction != NULL)
        dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

dpu_error_t
dpu_extract_context_for_dpu_routine(dpu_query_t query,
    dpu_slice_id_t slice_id,
    dpu_member_id_t member_id,
    dpu_transaction_t transaction,
    struct dpu_rank_t *rank,
    dpuword_t *raw_context,
    dpu_context_t context,
    wram_size_t context_size_in_words,
    uint32_t nr_of_atomic_bits_per_dpu,
    uint8_t nr_of_threads_per_dpu,
    uint8_t nr_of_work_registers_per_thread,
    wram_size_t atomic_register_size_in_words)
{
    dpu_error_t status;
    dpu_planner_status_e planner_status;

    // 1. Boot and wait
    status = dpu_boot_and_wait_for_dpu(query, slice_id, member_id, transaction, rank);
    if (status != DPU_OK)
        goto dpu_extract_context_for_dpu_routine_exit;

    // 2. Fetch context from WRAM
    safe_add_query(query,
        dpu_query_build_read_wram_word_for_previous(slice_id, 0, context_size_in_words, raw_context),
        transaction,
        status,
        dpu_extract_context_for_dpu_routine_exit);

    safe_execute_transaction(transaction, rank, planner_status, status, dpu_extract_context_for_dpu_routine_exit);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 3. Format context
    for (uint32_t each_atomic_bit = 0; each_atomic_bit < nr_of_atomic_bits_per_dpu; ++each_atomic_bit) {
        context->atomic_register[each_atomic_bit] = ((uint8_t *)raw_context)[each_atomic_bit] != 0;
    }

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        for (uint32_t each_register_index = 0; each_register_index < nr_of_work_registers_per_thread; each_register_index += 2) {
            uint32_t even_register_index = each_thread * nr_of_work_registers_per_thread + each_register_index;
            wram_size_t odd_register_context_index
                = atomic_register_size_in_words + (each_register_index * nr_of_threads_per_dpu) + (2 * each_thread);

            context->registers[even_register_index] = raw_context[odd_register_context_index + 1];
            context->registers[even_register_index + 1] = raw_context[odd_register_context_index];
        }

        uint32_t flags = raw_context[atomic_register_size_in_words + (nr_of_work_registers_per_thread * nr_of_threads_per_dpu)
            + each_thread];

        context->carry_flags[each_thread] = (flags & 1) != 0;
        context->zero_flags[each_thread] = (flags & 2) != 0;
    }

dpu_extract_context_for_dpu_routine_exit:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_extract_context_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    return dpu_execute_routine_for_dpu(
        dpu, context, fetch_core_dump_program, dpu_extract_context_for_dpu_routine, DPU_EVENT_EXTRACT_CONTEXT);
}

dpu_error_t
dpu_restore_context_for_dpu_routine(dpu_query_t query,
    dpu_slice_id_t slice_id,
    dpu_member_id_t member_id,
    dpu_transaction_t transaction,
    struct dpu_rank_t *rank,
    dpuword_t *raw_context,
    dpu_context_t context,
    wram_size_t context_size_in_words,
    uint32_t nr_of_atomic_bits_per_dpu,
    uint8_t nr_of_threads_per_dpu,
    uint8_t nr_of_work_registers_per_thread,
    wram_size_t atomic_register_size_in_words)
{
    dpu_error_t status;
    dpu_selected_mask_t selected_mask = dpu_mask_one(member_id);
    dpu_planner_status_e planner_status;

    // 1. Format raw context
    for (uint32_t each_atomic_bit = 0; each_atomic_bit < nr_of_atomic_bits_per_dpu; ++each_atomic_bit) {
        ((uint8_t *)raw_context)[each_atomic_bit] = context->atomic_register[each_atomic_bit] ? 0xFF : 0x00;
    }
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        for (uint32_t each_register_index = 0; each_register_index < nr_of_work_registers_per_thread; each_register_index += 2) {
            uint32_t even_regsiter_index = each_thread * nr_of_work_registers_per_thread + each_register_index;
            wram_size_t odd_register_context_index
                = atomic_register_size_in_words + (each_register_index * nr_of_threads_per_dpu) + (2 * each_thread);

            raw_context[odd_register_context_index] = context->registers[even_regsiter_index + 1];
            raw_context[odd_register_context_index + 1] = context->registers[even_regsiter_index];
        }
        uint32_t flags = (context->carry_flags[each_thread] ? 1 : 0) + (context->zero_flags[each_thread] ? 2 : 0);
        raw_context[atomic_register_size_in_words + (nr_of_work_registers_per_thread * nr_of_threads_per_dpu) + each_thread]
            = flags;
    }

    // 2. Load WRAM with raw context
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(slice_id, selected_mask, 0, raw_context, context_size_in_words),
        transaction,
        status,
        dpu_restore_context_for_dpu_routine_exit);

    safe_execute_transaction(transaction, rank, planner_status, status, dpu_restore_context_for_dpu_routine_exit);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 3. Boot thread 0
    status = dpu_boot_and_wait_for_dpu(query, slice_id, member_id, transaction, rank);

dpu_restore_context_for_dpu_routine_exit:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_restore_context_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    return dpu_execute_routine_for_dpu(
        dpu, context, fetch_restore_registers_program, dpu_restore_context_for_dpu_routine, DPU_EVENT_RESTORE_CONTEXT);
}

__API_SYMBOL__ dpu_error_t
dpu_initialize_fault_process_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    uint32_t bkp_fault;
    uint32_t dma_fault;
    uint32_t mem_fault;
    dpu_thread_t bkp_fault_thread_index;
    dpu_thread_t dma_fault_thread_index;
    dpu_thread_t mem_fault_thread_index;
    dpu_pc_mode_e pc_mode;
    uint32_t ignored;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    dpu_lock_rank(rank);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 1. Draining the pipeline
    // 1.0 Fetching PC mode
    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto free_transaction;
    }
    // 1.1 Read and set BKP fault
    safe_add_query(query,
        dpu_query_build_read_bkp_fault_thread_index_for_dpu(slice_id, member_id, &bkp_fault_thread_index),
        transaction,
        status,
        free_transaction);
    // !!! Read BKP fault + Interception Fault Clear
    safe_add_query(
        query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &bkp_fault), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_set_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free(transaction);

    // 1.2 Pipeline drain
    drain_pipeline(dpu, context, pc_mode, true);

    // 2. Fetching PCs for non-running threads
    uint8_t nr_of_running_threads = context->nr_of_running_threads;
    if (nr_of_running_threads != nr_of_threads_per_dpu) {
        // 2.1 Resuming all non-running threads
        if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
            status = DPU_ERR_SYSTEM;
            goto end;
        }

        // Interception Fault Set
        safe_add_query(
            query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);
        safe_add_query(
            query, dpu_query_build_set_and_step_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

        for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
            if (context->scheduling[each_thread] == 0xFF) {
                safe_add_query(query,
                    dpu_query_build_resume_thread_for_previous(slice_id, each_thread, &ignored),
                    transaction,
                    status,
                    free_transaction);
            }
        }

        // Interception Fault Clear
        safe_add_query(
            query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);

        safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

        dpu_transaction_free(transaction);

        // 2.2 Draining the pipeline, again
        drain_pipeline(dpu, context, pc_mode, false);
    }

    // 3. Fault identification
    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 3.1 Read and clear for dma & mem fault thread index
    safe_add_query(query,
        dpu_query_build_read_dma_fault_thread_index_for_previous(slice_id, &dma_fault_thread_index),
        transaction,
        status,
        free_transaction);
    safe_add_query(query,
        dpu_query_build_read_mem_fault_thread_index_for_previous(slice_id, &mem_fault_thread_index),
        transaction,
        status,
        free_transaction);
    // 3.2 Read and clear for dma & mem faults
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query,
        dpu_query_build_read_and_clear_dma_fault_for_previous(slice_id, &dma_fault),
        transaction,
        status,
        free_transaction);
    safe_add_query(query,
        dpu_query_build_read_and_clear_mem_fault_for_previous(slice_id, &mem_fault),
        transaction,
        status,
        free_transaction);
    // 3.3 Clear fault
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    if ((bkp_fault & mask_one) != 0) {
        context->bkp_fault = true;
        context->bkp_fault_thread_index = bkp_fault_thread_index;

        iram_addr_t previous_pc;
        iram_addr_t current_pc = context->pcs[context->bkp_fault_thread_index];
        if (current_pc == 0) {
            previous_pc = rank->description->memories.iram_size - 1;
        } else {
            previous_pc = current_pc - 1;
        }
        if ((status = extract_bkp_fault_id(dpu, previous_pc, &(context->bkp_fault_id))) != DPU_OK) {
            goto free_transaction;
        }

        /* If bkp_fault_id is unknown, it means that the fault has not been generated by the DPU program, so it came from a host
         * and we can clear the bkp_fault.
         * If the bkp_fault_id is not unknown, we need to decrement the pc as the bkp instruction should have incremented it. */
        if (context->bkp_fault_id == DPU_FAULT_UNKNOWN_ID) {
            context->bkp_fault = false;
        } else {
            if ((status = decrement_thread_pc(dpu, bkp_fault_thread_index, context->pcs + bkp_fault_thread_index)) != DPU_OK) {
                goto free_transaction;
            }
        }
    }
    if ((dma_fault & mask_one) != 0) {
        context->dma_fault = true;
        context->dma_fault_thread_index = dma_fault_thread_index;
        if ((status = decrement_thread_pc(dpu, dma_fault_thread_index, context->pcs + dma_fault_thread_index)) != DPU_OK) {
            goto free_transaction;
        }
    }
    if ((mem_fault & mask_one) != 0) {
        context->mem_fault = true;
        context->mem_fault_thread_index = mem_fault_thread_index;
        if ((status = decrement_thread_pc(dpu, mem_fault_thread_index, context->pcs + mem_fault_thread_index)) != DPU_OK) {
            goto free_transaction;
        }
    }

    if ((rank->runtime.run_context.dpu_running[slice_id] & mask_one) != 0) {
        rank->runtime.run_context.dpu_running[slice_id] ^= mask_one;
        rank->runtime.run_context.nb_dpu_running--;
    }

free_transaction:
    dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_finalize_fault_process_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    uint8_t nr_of_running_threads = context->nr_of_running_threads;
    dpu_thread_t scheduling_order[nr_of_threads_per_dpu];
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    uint32_t ignored;

    dpu_lock_rank(rank);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 1. Set fault & bkp_fault
    safe_add_query(
        query, dpu_query_build_set_and_step_dpu_fault_state_for_dpu(slice_id, member_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_set_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);

    // 2. Resume running threads
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        uint8_t scheduling_position = context->scheduling[each_thread];
        if (scheduling_position != 0xFF) {
            scheduling_order[scheduling_position] = each_thread;
        }
    }

    for (dpu_thread_t each_running_thread = 0; each_running_thread < nr_of_running_threads; ++each_running_thread) {
        safe_add_query(query,
            dpu_query_build_resume_thread_for_previous(slice_id, scheduling_order[each_running_thread], &ignored),
            transaction,
            status,
            free_transaction);
    }
    // Interception Fault Clear
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);

    // 3. Clear bkp_fault & fault
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    if (nr_of_running_threads != 0) {
        rank->runtime.run_context.dpu_running[slice_id] |= dpu_mask_one(member_id);
        rank->runtime.run_context.nb_dpu_running++;
    }

free_transaction:
    dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_execute_thread_step_in_fault_for_dpu(struct dpu_t *dpu, dpu_thread_t thread, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "%d", thread);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    verify_thread_id(thread, rank);

    dpu_error_t status = DPU_OK;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    uint32_t ignored;
    uint32_t poison_fault;
    uint32_t dma_fault;
    uint32_t mem_fault;
    uint32_t nr_of_waiting_threads;
    uint32_t step_run_state;
    uint32_t step_fault_state;
    dpu_pc_mode_e pc_mode;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);
    uint8_t nr_of_running_threads = context->nr_of_running_threads;
    dpu_thread_t scheduling_order[nr_of_threads_per_dpu];

    dpu_lock_rank(rank);

    /* Important note: we are assuming that the thread is running/in fault. Let's just verify that it is true */
    if (context->scheduling[thread] == 0xFF) {
        LOG_DPU(WARNING, dpu, "ERROR: thread %d is not running before executing debug step", thread);
        status = DPU_ERR_INTERNAL;
        goto end;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // 1. Fetch PC mode
    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto free_transaction;
    }

    // 2. Set fault & fault_poison
    safe_add_query(
        query, dpu_query_build_set_and_step_dpu_fault_state_for_dpu(slice_id, member_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_set_poison_fault_for_previous(slice_id), transaction, status, free_transaction);

    // 3. Resume thread
    safe_add_query(
        query, dpu_query_build_resume_thread_for_previous(slice_id, thread, &ignored), transaction, status, free_transaction);

    // 4. Resume other running threads
    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        uint8_t scheduling_position = context->scheduling[each_thread];
        if (scheduling_position != 0xFF) {
            scheduling_order[scheduling_position] = each_thread;
        }
    }

    for (dpu_thread_t each_running_thread = 0; each_running_thread < nr_of_running_threads; ++each_running_thread) {
        dpu_thread_t thread_to_be_resumed = scheduling_order[each_running_thread];

        if (thread_to_be_resumed != thread) {
            safe_add_query(query,
                dpu_query_build_resume_thread_for_previous(slice_id, thread_to_be_resumed, &ignored),
                transaction,
                status,
                free_transaction);
        }
    }

    // Interception Fault Clear
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);

    // 5. Execute step (by clearing fault)
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 5 Bis. Wait for end of step
    safe_add_query(
        query, dpu_query_build_read_dpu_run_state_for_previous(slice_id, &step_run_state), transaction, status, free_transaction);
    safe_add_query(query,
        dpu_query_build_read_dpu_fault_state_for_previous(slice_id, &step_fault_state),
        transaction,
        status,
        free_transaction);

    do {
        safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);
    } while (((step_run_state & ~step_fault_state) & mask_one) != 0);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 6. Read & clear faults
    safe_add_query(
        query, dpu_query_build_read_poison_fault_for_previous(slice_id, &poison_fault), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_poison_fault_for_previous(slice_id), transaction, status, free_transaction);
    // Interception Fault Clear
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query,
        dpu_query_build_read_and_clear_dma_fault_for_previous(slice_id, &dma_fault),
        transaction,
        status,
        free_transaction);
    safe_add_query(query,
        dpu_query_build_read_and_clear_mem_fault_for_previous(slice_id, &mem_fault),
        transaction,
        status,
        free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    // 7. Resetting scheduling structure
    nr_of_waiting_threads = (uint32_t)(context->nr_of_running_threads - 1);

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        context->scheduling[each_thread] = 0xFF;
    }

    context->nr_of_running_threads = 0;

    // 8. Drain pipeline
    drain_pipeline(dpu, context, pc_mode, true);

    // 9. Clear fault
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    if ((context->nr_of_running_threads - nr_of_waiting_threads) == 1) {
        // Only one more thread running (the one we stepped on). It may have provoked a fault (if 0 or 2 more threads, the thread
        // has executed a stop or a boot/resume and not fault can happen).
        if ((poison_fault & mask_one) == 0) {
            // If poison_fault has been cleared, the stepped instruction was a bkp.
            context->bkp_fault = true;
            context->bkp_fault_thread_index = thread;
            if ((status = decrement_thread_pc(dpu, thread, context->pcs + thread)) != DPU_OK) {
                goto free_transaction;
            }

            if ((status = extract_bkp_fault_id(dpu, context->pcs[context->bkp_fault_thread_index], &(context->bkp_fault_id)))
                != DPU_OK) {
                goto free_transaction;
            }
        }
        if ((dma_fault & mask_one) != 0) {
            context->dma_fault = true;
            context->dma_fault_thread_index = thread;
            if ((status = decrement_thread_pc(dpu, thread, context->pcs + thread)) != DPU_OK) {
                goto free_transaction;
            }
        }
        if ((mem_fault & mask_one) != 0) {
            context->mem_fault = true;
            context->mem_fault_thread_index = thread;
            if ((status = decrement_thread_pc(dpu, thread, context->pcs + thread)) != DPU_OK) {
                goto free_transaction;
            }
        }
    }

free_transaction:
    dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_resume_dpus_for_rank(struct dpu_rank_t *rank, dpu_context_t context)
{
    dpu_error_t status;

    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        for (dpu_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
            uint32_t idx_dpu_context = each_dpu * rank->description->topology.nr_of_control_interfaces + each_slice;

            struct dpu_t *dpu = dpu_get(rank, each_slice, each_dpu);

            if (dpu->enabled) {
                status = dpu_resume_threads_for_dpu(dpu, context + idx_dpu_context);
                if (status != DPU_OK)
                    return status;
            }
        }
    }

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_resume_threads_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    uint8_t nr_of_threads_per_dpu = rank->description->dpu.nr_of_threads;
    uint8_t nr_of_running_threads = context->nr_of_running_threads;
    dpu_thread_t scheduling_order[nr_of_threads_per_dpu];
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    uint32_t ignored;

    dpu_lock_rank(rank);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_threads_per_dpu; ++each_thread) {
        uint8_t scheduling_position = context->scheduling[each_thread];
        if (scheduling_position != 0xFF) {
            scheduling_order[scheduling_position] = each_thread;
        }
    }

    /* 1/ Resume running threads */
    for (dpu_thread_t each_running_thread = 0; each_running_thread < nr_of_running_threads; ++each_running_thread) {
        safe_add_query(query,
            dpu_query_build_resume_thread_for_dpu(slice_id, member_id, scheduling_order[each_running_thread], &ignored),
            transaction,
            status,
            free_transaction);
    }
    /* 2/ Interception Fault Clear */
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, free_transaction);

    /* 3/ Clear bkp_fault & fault */
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, free_transaction);
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    /* From here we are sure the DPU has resumed. */
    rank->runtime.run_context.dpu_running[slice_id] |= dpu_mask_one(member_id);
    rank->runtime.run_context.nb_dpu_running++;

free_transaction:
    free(transaction);
end:
    dpu_unlock_rank(rank);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_stop_dpus_for_rank(struct dpu_rank_t *rank, dpu_context_t context)
{
    dpu_error_t cni_status;

    /* The whole rank stopping is not locked, that should not raise problems since the debugger won't do anything in parallel. */
    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
            uint32_t idx_dpu_context = each_dpu * rank->description->topology.nr_of_control_interfaces + each_slice;
            struct dpu_t *dpu = dpu_get(rank, each_slice, each_dpu);

            if (dpu->enabled) {
                cni_status = dpu_stop_threads_for_dpu(dpu, context + idx_dpu_context);
                if (cni_status != DPU_OK)
                    return cni_status;
            }
        }
    }

    /*
     * Normally, the user attaches a DPU when it is running and then we know that the MRAM mux is at DPU-side.
     * But if the user attaches a DPU while it is not running and right after the beginning of a MRAM transfer:
     *
     * 1/ The mux is at host-side.
     * 2/ The debugger does a MRAM transfer: it switches the mux to host-side, and then switches it back to DPU-side
     *    (because the mux is DPU-side by default).
     * 3/ The debugger hands control back to the host application that initiates the transfer believing the mux is
     *    at host-side.
     * 4/ Disaster.
     *
     * Conclusion; Maybe we should keep MRAM mux status...
     *
     */

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_stop_threads_for_dpu(struct dpu_t *dpu, dpu_context_t context)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status;
    dpu_transaction_t transaction;
    dpu_pc_mode_e pc_mode;

    dpu_lock_rank(rank);

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    /* Drain the pipeline */
    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto free_transaction;
    }

    /* drain_pipeline stores infos about running threads and their place in the FIFO, which is needed for resuming. */
    status = drain_pipeline(dpu, context, pc_mode, true);
    if (status != DPU_OK)
        goto free_transaction;

    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if (rank->runtime.run_context.dpu_running[slice_id] & mask_one) {
        rank->runtime.run_context.dpu_running[slice_id] &= ~mask_one;
        rank->runtime.run_context.nb_dpu_running--;
    }

free_transaction:
    dpu_transaction_free(transaction);
end:
    dpu_unlock_rank(rank);
    return status;
}

#define TIMEOUT_COLOR (100000)
__API_SYMBOL__ dpu_error_t
dpu_save_slice_context_for_dpu(struct dpu_t *dpu)
{
    /* This function is used by a process that attaches a DPU that is running, ie
     * the DPU has been initialized.
     */
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;

    uint64_t *data;
    uint32_t timeout = TIMEOUT_COLOR;
    dpu_error_t status = DPU_OK;
    dpu_rank_status_e rank_status;
    uint8_t cmd_type, valid;

    data = malloc(rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    if (!data) {
        LOG_DPU(WARNING, dpu, "Error while allocating memory");
        return DPU_ERR_SYSTEM;
    }

    /* This is needed for some driver mappings that avoid to read control interface if no commands were
     * sent before: by memsetting at a value != 0, we force the control interface reading.
     */
    memset(data, 0xFF, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));

    /* Read the control interface as long as [63: 56] != 0: this loop is necessary in case of FPGA where
     * a latency due to implementation makes the initial result not appear right away in case of valid command (ie
     * not 0x00 and not 0xFF (NOP)).
     * A NOP command does not change the color and [63: 56],
     * A valid command changes the color and clears [63: 56],
     * An invalid command (ie something the CI does not understand) changes the color since it sets CMD_FAULT_DECODE and clears
     * [63: 56],
     *
     * In addition here, we must wait for the result to be valid because we need to restore it before handing control
     * back to the host application: since we don't know the last command, we cannot be sure of the type of response
     * the host application expects (cf WRAM/IRAM write). According to Fabrice, there might be at most 28 DPU cycles
     * between the moment we see [39: 32] == 0xFF and particular commands get their whole result in [31: 0]
     * => so once [39: 32] == 0xFF, we re-read the result to make sure it is ok.
     */
    do {
        rank_status = rank->handler_context->handler->update_commands(rank, &data);
        if (rank_status != DPU_RANK_SUCCESS) {
            LOG_DPU(WARNING, dpu, "Failed to read from rank");
            status = map_rank_status_to_api_status(rank_status);
            goto free_data;
        }

        timeout--;

        cmd_type = (uint8_t)(((data[slice_id] & 0xFF00000000000000ULL) >> 56) & 0xFF);
        if (cmd_type == 0xFF)
            break;
        valid = (uint8_t)(((data[slice_id] & 0xFF00000000ULL) >> 32) & 0xFF);
    } while (timeout && (cmd_type != 0x00 || valid != 0xFF));

    if (!timeout) {
        LOG_DPU(WARNING, dpu, "Timeout waiting for result to be correct");
        status = DPU_ERR_SYSTEM;
        goto free_data;
    }

    rank_status = rank->handler_context->handler->update_commands(rank, &data);
    if (rank_status != DPU_RANK_SUCCESS) {
        LOG_DPU(WARNING, dpu, "Failed to read from rank");
        status = map_rank_status_to_api_status(rank_status);
        goto free_data;
    }

    rank->debug.debug_result[slice_id] = (uint32_t)(data[slice_id] & 0xFFFFFFFF);

    if (cmd_type == 0xFF) {
        LOG_DPU(WARNING, dpu, "Nop command, must execute dummy command to get the right color...");
        /* To avoid stuttering here, just send twice a byte order command with [63: 56] shuffled
         * and send an identity command and wait for the result to get the color.
         */
        // TODO

        /* And simply commit an identity command to get the right color: we must
         * bypass UFI that requires the color. The timeout is needed for the same
         * reason as above.
         */
        timeout = TIMEOUT_COLOR;
        memset(data, 0, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
        data[slice_id] = CI_FRAME_DPU_OPERATION_IDENTITY;
        rank->handler_context->handler->commit_commands(rank, &data);

        do {
            rank_status = rank->handler_context->handler->update_commands(rank, &data);
            if (rank_status != DPU_RANK_SUCCESS) {
                LOG_DPU(WARNING, dpu, "Failed to read from rank");
                status = map_rank_status_to_api_status(rank_status);
                goto free_data;
            }

            timeout--;

            cmd_type = (uint8_t)(((data[slice_id] & 0xFF00000000000000ULL) >> 56) & 0xFF);
        } while (timeout && cmd_type != 0x00);

        if (!timeout) {
            LOG_DPU(WARNING, dpu, "Timeout waiting for result to be correct");
            status = DPU_ERR_TIMEOUT;
            goto free_data;
        }
    }

    /* Here, we have the color */
    uint8_t nb_bits_set = __builtin_popcount(((data[slice_id] & 0x00FF000000000000ULL) >> 48) & 0xFF);
    uint8_t expected_color = (uint8_t)((nb_bits_set <= 3) ? 1 : 0);

    if (!expected_color) {
        rank->runtime.control_interface.color &= ~(1UL << slice_id);
        rank->debug.debug_color &= ~(1UL << slice_id);
    } else {
        rank->runtime.control_interface.color |= (1UL << slice_id);
        rank->debug.debug_color |= (1UL << slice_id);
    }

    rank->debug.is_rank_for_debugger = true;

free_data:
    free(data);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_set_debug_slice_info(struct dpu_rank_t *rank,
    uint32_t slice_id,
    uint64_t structure_value,
    uint64_t slice_target,
    dpu_bitfield_t host_mux_mram_state)
{
    struct dpu_configuration_slice_info_t *slice_info = &rank->debug.debug_slice_info[slice_id];

    slice_info->structure_value = structure_value;
    slice_info->host_mux_mram_state = host_mux_mram_state;
    memcpy(&slice_info->slice_target, &slice_target, sizeof(slice_target));

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_save_slice_context_and_extract_debug_info_from_pid_for_dpu(__attribute__((unused)) struct dpu_t *dpu,
    __attribute__((unused)) pid_t pid)
{
    return DPU_ERR_SYSTEM;
}

__API_SYMBOL__ dpu_error_t
dpu_restore_slice_context_for_dpu(struct dpu_t *dpu)
{
    /* Restore as left by the debuggee:
     * - the last target
     * - the color
     * - the structure register
     * - the result
     */
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_pair_base_id = (dpu_member_id_t)(dpu->dpu_id & ~1);

    dpu_planner_status_e planner_status;
    dpu_query_t query;
    dpu_transaction_t transaction;
    dpu_rank_status_e rank_status;
    dpu_error_t status = DPU_OK;
    uint64_t *data;
    uint32_t timeout = TIMEOUT_COLOR;
    uint8_t cmd_type, valid;

    dpu_lock_rank(rank);

    /* 0/ Restores the mux of the current dpu and of the other dpu in the pair */
    if (rank->debug.debug_slice_info[slice_id].host_mux_mram_state & (1 << dpu_pair_base_id))
        dpu_host_get_access_for_dpu(dpu_get(rank, slice_id, dpu_pair_base_id));

    if (rank->debug.debug_slice_info[slice_id].host_mux_mram_state & (1 << (dpu_pair_base_id + 1)))
        dpu_host_get_access_for_dpu(dpu_get(rank, slice_id, dpu_pair_base_id + 1));

    /* 1/ Sets the color as it was before the debugger intervention */
    data = malloc(rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    if (!data) {
        LOG_DPU(WARNING, dpu, "Error while allocating memory");
        goto end;
    }

    /* This is needed for some driver mappings that avoid to read control interface if no commands were
     * sent before: by memsetting at a value != 0, we force the control interface reading.
     */
    memset(data, 0xFF, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));

    /* Read the control interface as long as [63: 56] != 0: this loop is necessary in case of FPGA where
     * a latency due to implementation makes the initial result not appear right away in case of valid command (ie
     * not 0x00 and not 0xFF (NOP)).
     * A NOP command does not change the color and [63: 56],
     * A valid command changes the color and clears [63: 56],
     * An invalid command (ie something the CI does not understand) changes the color since it sets CMD_FAULT_DECODE and clears
     * [63: 56],
     */
    do {
        rank_status = rank->handler_context->handler->update_commands(rank, &data);
        if (rank_status != DPU_RANK_SUCCESS) {
            LOG_DPU(WARNING, dpu, "Failed to read from rank");
            status = map_rank_status_to_api_status(rank_status);
            goto free_data;
        }

        timeout--;

        cmd_type = (uint8_t)(((data[slice_id] & 0xFF00000000000000ULL) >> 56) & 0xFF);
    } while (timeout && cmd_type != 0x00);

    if (!timeout) {
        LOG_DPU(WARNING, dpu, "Timeout waiting for result to be correct");
        status = DPU_ERR_TIMEOUT;
        goto free_data;
    }

    transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces);
    if (!transaction) {
        status = DPU_ERR_SYSTEM;
        goto free_data;
    }

    /* 2/ Restore the last target */
    if (rank->debug.debug_slice_info[slice_id].slice_target.type != DPU_SLICE_TARGET_CONTROL) {
        switch (rank->debug.debug_slice_info[slice_id].slice_target.type) {
            case DPU_SLICE_TARGET_DPU:
                safe_add_query(query,
                    dpu_query_build_select_dpu_for_control(slice_id, rank->debug.debug_slice_info[slice_id].slice_target.dpu_id),
                    transaction,
                    status,
                    free_data);
                break;
            case DPU_SLICE_TARGET_GROUP:
                safe_add_query(query,
                    dpu_query_build_select_group_for_control(
                        slice_id, rank->debug.debug_slice_info[slice_id].slice_target.group_id),
                    transaction,
                    status,
                    free_data);
                break;
            case DPU_SLICE_TARGET_ALL:
                safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, free_data);
            default:
                break;
        }

        planner_status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
        if (planner_status != DPU_PLANNER_SUCCESS) {
            status = map_planner_status_to_api_status(planner_status);
            goto free_transaction;
        }

        dpu_transaction_free_queries_for_slice(transaction, slice_id);
    }

    /* Here we have two important things to take care of:
     *  - Set the color to the value the application expects,
     *  - Replay the last write structure
     *
     * 2 cases:
     *  - Current color is not the one expected: simply replay the last write structure
     *  - Current color is     the one expected: send an identity command and replay the last write structure.
     *
     *  Note that replaying a write structure has no effect; structure register value is only
     *  used when a send frame is sent, on its own, it does nothing.
     */
    if ((rank->runtime.control_interface.color & (1 << slice_id)) == (rank->debug.debug_color & (1 << slice_id))) {
        uint32_t identity_result;

        safe_add_query(query, dpu_query_build_identity_for_control(slice_id, &identity_result), transaction, status, free_data);
        planner_status = dpu_planner_execute_transaction(transaction, rank->handler_context->handler, rank);
        if (planner_status != DPU_PLANNER_SUCCESS) {
            status = map_planner_status_to_api_status(planner_status);
            goto free_transaction;
        }
    }

    /* 3/ Restore the structure value */
    timeout = TIMEOUT_COLOR;
    memset(data, 0, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    data[slice_id] = rank->debug.debug_slice_info[slice_id].structure_value;
    /* Update debugger last structure value written */
    rank->runtime.control_interface.slice_info[slice_id].structure_value = rank->debug.debug_slice_info[slice_id].structure_value;
    rank->handler_context->handler->commit_commands(rank, &data);

    do {
        rank_status = rank->handler_context->handler->update_commands(rank, &data);
        if (rank_status != DPU_RANK_SUCCESS) {
            LOG_DPU(WARNING, dpu, "Failed to read from rank");
            status = map_rank_status_to_api_status(rank_status);
            goto free_transaction;
        }

        timeout--;

        cmd_type = (uint8_t)(((data[slice_id] & 0xFF00000000000000ULL) >> 56) & 0xFF);
        valid = (uint8_t)(((data[slice_id] & 0xFF00000000ULL) >> 32) & 0xFF);
    } while (timeout && (cmd_type != 0x00 || valid != 0xFF));

    if (!timeout) {
        LOG_DPU(WARNING, dpu, "Timeout waiting for result to be correct");
        status = DPU_ERR_TIMEOUT;
        goto free_transaction;
    }

    /* The above command toggled the color... */
    rank->runtime.control_interface.color ^= (1UL << slice_id);

    /* 4/ Restore the result expected by the host application */
    memset(data, 0, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    data[slice_id] = 0xFF00000000000000ULL | rank->debug.debug_result[slice_id];
    rank->handler_context->handler->commit_commands(rank, &data);

free_transaction:
    dpu_transaction_free(transaction);
free_data:
    free(data);
end:
    dpu_unlock_rank(rank);

    return status;
}

#define SERIALIZED_CONTEXT_MAGIC (0xfabddbaf)
#define SERIALIZED_CONTEXT_CHIP_VERSION 18
#define SERIALIZED_CONTEXT_VERSION 2

#define DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, curr_elem_size)                                       \
    do {                                                                                                                         \
        (serialized_context) += (curr_elem_size);                                                                                \
        if (serialized_context_end < serialized_context)                                                                         \
            return DPU_ERR_ELF_INVALID_FILE;                                                                                     \
    } while (0);

#define DESERIALIZED_ELEM(elem, serialized_context, serialized_context_end, curr_elem_size)                                      \
    do {                                                                                                                         \
        memcpy((elem), (serialized_context), (curr_elem_size));                                                                  \
        DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, curr_elem_size);                                      \
    } while (0);

__API_SYMBOL__ dpu_error_t
dpu_deserialize_context(struct dpu_rank_t *rank,
    dpu_context_t context,
    uint8_t *serialized_context,
    uint32_t serialized_context_size)
{
    uint32_t nr_of_threads = rank->description->dpu.nr_of_threads;
    uint32_t nr_of_work_registers_per_thread = rank->description->dpu.nr_of_work_registers_per_thread;
    uint32_t nr_of_atomic_bits = rank->description->dpu.nr_of_atomic_bits;

    uint32_t magic_size = sizeof(uint32_t);
    uint32_t chip_version_size = sizeof(uint32_t);
    uint32_t context_version_size = sizeof(uint32_t);
    uint32_t reserved_size = sizeof(uint32_t);
    uint32_t registers_size = sizeof(uint32_t) * nr_of_threads * nr_of_work_registers_per_thread;
    uint32_t pcs_size = sizeof(iram_addr_t) * nr_of_threads;
    uint32_t atomic_register_size = sizeof(bool) * nr_of_atomic_bits;
    uint32_t zero_flags_size = sizeof(bool) * nr_of_threads;
    uint32_t carry_flags_size = sizeof(bool) * nr_of_threads;
    uint32_t nr_of_running_threads_size = sizeof(uint8_t);
    uint32_t scheduling_size = sizeof(uint8_t) * nr_of_threads;
    uint32_t bkp_fault_size = sizeof(bool);
    uint32_t dma_fault_size = sizeof(bool);
    uint32_t mem_fault_size = sizeof(bool);
    uint32_t bkp_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t dma_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t mem_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t bkp_fault_id_size = sizeof(uint32_t);

    uint8_t *serialized_context_end = serialized_context + serialized_context_size;

    if (*((uint32_t *)serialized_context) != SERIALIZED_CONTEXT_MAGIC)
        return DPU_ERR_ELF_INVALID_FILE;
    DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, magic_size);

    if (*((uint32_t *)serialized_context) != SERIALIZED_CONTEXT_CHIP_VERSION)
        return DPU_ERR_ELF_INVALID_FILE;
    DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, chip_version_size);

    if (*((uint32_t *)serialized_context) != SERIALIZED_CONTEXT_VERSION)
        return DPU_ERR_ELF_INVALID_FILE;
    DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, context_version_size);

    if (*((uint32_t *)serialized_context) != SERIALIZED_CONTEXT_MAGIC)
        return DPU_ERR_ELF_INVALID_FILE;
    DESERIALIZED_NEXT_ELEM(serialized_context, serialized_context_end, reserved_size);

    DESERIALIZED_ELEM(context->registers, serialized_context, serialized_context_end, registers_size);
    DESERIALIZED_ELEM(context->pcs, serialized_context, serialized_context_end, pcs_size);
    DESERIALIZED_ELEM(context->atomic_register, serialized_context, serialized_context_end, atomic_register_size);
    DESERIALIZED_ELEM(context->zero_flags, serialized_context, serialized_context_end, zero_flags_size);
    DESERIALIZED_ELEM(context->carry_flags, serialized_context, serialized_context_end, carry_flags_size);
    DESERIALIZED_ELEM(&context->nr_of_running_threads, serialized_context, serialized_context_end, nr_of_running_threads_size);
    DESERIALIZED_ELEM(context->scheduling, serialized_context, serialized_context_end, scheduling_size);
    DESERIALIZED_ELEM(&context->bkp_fault, serialized_context, serialized_context_end, bkp_fault_size);
    DESERIALIZED_ELEM(&context->dma_fault, serialized_context, serialized_context_end, dma_fault_size);
    DESERIALIZED_ELEM(&context->mem_fault, serialized_context, serialized_context_end, mem_fault_size);
    DESERIALIZED_ELEM(&context->bkp_fault_thread_index, serialized_context, serialized_context_end, bkp_fault_thread_index_size);
    DESERIALIZED_ELEM(&context->dma_fault_thread_index, serialized_context, serialized_context_end, dma_fault_thread_index_size);
    DESERIALIZED_ELEM(&context->mem_fault_thread_index, serialized_context, serialized_context_end, mem_fault_thread_index_size);
    DESERIALIZED_ELEM(&context->bkp_fault_id, serialized_context, serialized_context_end, bkp_fault_id_size);

    return DPU_OK;
}

#define SERIALIZED_ELEM(serialized_context, elem, elem_size)                                                                     \
    do {                                                                                                                         \
        memcpy((serialized_context), (elem), (elem_size));                                                                       \
        (serialized_context) += (elem_size);                                                                                     \
    } while (0);

__API_SYMBOL__ dpu_error_t
dpu_serialize_context(struct dpu_rank_t *rank,
    dpu_context_t context,
    uint8_t **serialized_context,
    uint32_t *serialized_context_size)
{
    uint32_t nr_of_threads = rank->description->dpu.nr_of_threads;
    uint32_t nr_of_work_registers_per_thread = rank->description->dpu.nr_of_work_registers_per_thread;
    uint32_t nr_of_atomic_bits = rank->description->dpu.nr_of_atomic_bits;

    uint32_t magic_size = sizeof(uint32_t);
    uint32_t chip_version_size = sizeof(uint32_t);
    uint32_t context_version_size = sizeof(uint32_t);
    uint32_t reserved_size = sizeof(uint32_t);
    uint32_t registers_size = sizeof(uint32_t) * nr_of_threads * nr_of_work_registers_per_thread;
    uint32_t pcs_size = sizeof(iram_addr_t) * nr_of_threads;
    uint32_t atomic_register_size = sizeof(bool) * nr_of_atomic_bits;
    uint32_t zero_flags_size = sizeof(bool) * nr_of_threads;
    uint32_t carry_flags_size = sizeof(bool) * nr_of_threads;
    uint32_t nr_of_running_threads_size = sizeof(uint8_t);
    uint32_t scheduling_size = sizeof(uint8_t) * nr_of_threads;
    uint32_t bkp_fault_size = sizeof(bool);
    uint32_t dma_fault_size = sizeof(bool);
    uint32_t mem_fault_size = sizeof(bool);
    uint32_t bkp_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t dma_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t mem_fault_thread_index_size = sizeof(dpu_thread_t);
    uint32_t bkp_fault_id_size = sizeof(uint32_t);
    uint32_t serialized_size = magic_size + chip_version_size + context_version_size + reserved_size + registers_size + pcs_size
        + atomic_register_size + zero_flags_size + carry_flags_size + nr_of_running_threads_size + scheduling_size
        + bkp_fault_size + dma_fault_size + mem_fault_size + bkp_fault_thread_index_size + dma_fault_thread_index_size
        + mem_fault_thread_index_size + bkp_fault_id_size;

    *serialized_context = malloc(serialized_size);
    *serialized_context_size = serialized_size;
    uint8_t *curr = *serialized_context;

    if (curr == NULL)
        return DPU_ERR_SYSTEM;

    const uint32_t serialized_context_magic = SERIALIZED_CONTEXT_MAGIC;
    const uint32_t serialized_context_chip_version = SERIALIZED_CONTEXT_CHIP_VERSION;
    const uint32_t serialized_context_version = SERIALIZED_CONTEXT_VERSION;

    SERIALIZED_ELEM(curr, &serialized_context_magic, magic_size);
    SERIALIZED_ELEM(curr, &serialized_context_chip_version, chip_version_size);
    SERIALIZED_ELEM(curr, &serialized_context_version, context_version_size);
    SERIALIZED_ELEM(curr, &serialized_context_magic, reserved_size);
    SERIALIZED_ELEM(curr, context->registers, registers_size);
    SERIALIZED_ELEM(curr, context->pcs, pcs_size);
    SERIALIZED_ELEM(curr, context->atomic_register, atomic_register_size);
    SERIALIZED_ELEM(curr, context->zero_flags, zero_flags_size);
    SERIALIZED_ELEM(curr, context->carry_flags, carry_flags_size);
    SERIALIZED_ELEM(curr, &context->nr_of_running_threads, nr_of_running_threads_size);
    SERIALIZED_ELEM(curr, context->scheduling, scheduling_size);
    SERIALIZED_ELEM(curr, &context->bkp_fault, bkp_fault_size);
    SERIALIZED_ELEM(curr, &context->dma_fault, dma_fault_size);
    SERIALIZED_ELEM(curr, &context->mem_fault, mem_fault_size);
    SERIALIZED_ELEM(curr, &context->bkp_fault_thread_index, bkp_fault_thread_index_size);
    SERIALIZED_ELEM(curr, &context->dma_fault_thread_index, dma_fault_thread_index_size);
    SERIALIZED_ELEM(curr, &context->mem_fault_thread_index, mem_fault_thread_index_size);
    SERIALIZED_ELEM(curr, &context->bkp_fault_id, bkp_fault_id_size);

    return DPU_OK;
}

__API_SYMBOL__ dpu_error_t
dpu_create_core_dump(struct dpu_rank_t *rank,
    const char *exe_path,
    const char *core_file_path,
    dpu_context_t context,
    uint8_t *wram,
    uint8_t *mram,
    uint8_t *iram,
    uint32_t wram_size,
    uint32_t mram_size,
    uint32_t iram_size)
{
    uint8_t *context_serialized;
    uint32_t context_serialized_size;
    dpu_error_t status;
    status = dpu_serialize_context(rank, context, &context_serialized, &context_serialized_size);
    if (status != DPU_OK)
        return status;

    status = dpu_elf_create_core_dump(
        exe_path, core_file_path, wram, mram, iram, context_serialized, wram_size, mram_size, iram_size, context_serialized_size);
    if (status != DPU_OK)
        free(context_serialized);

    return status;
}

__API_SYMBOL__ void
dpu_free_dpu_context(dpu_context_t context)
{
    if (context == NULL)
        return;

    free(context->registers);
    free(context->pcs);
    free(context->atomic_register);
    free(context->zero_flags);
    free(context->carry_flags);
    free(context->scheduling);

    free(context);
}

__API_SYMBOL__ dpu_context_t
dpu_alloc_dpu_context(struct dpu_rank_t *rank)
{
    dpu_context_t context = (dpu_context_t)calloc(1, sizeof(*context));
    if (context == NULL)
        return NULL;

    dpu_description_t description = dpu_get_description(rank);
    uint32_t nr_work_registers = description->dpu.nr_of_work_registers_per_thread;
    uint32_t nr_threads = description->dpu.nr_of_threads;
    uint32_t nr_atomic_bits = description->dpu.nr_of_atomic_bits;

    context->registers = (uint32_t *)calloc(nr_threads * nr_work_registers, sizeof(uint32_t));
    context->pcs = (iram_addr_t *)calloc(nr_threads, sizeof(iram_addr_t));
    context->atomic_register = (bool *)calloc(nr_atomic_bits, sizeof(bool));
    context->zero_flags = (bool *)calloc(nr_threads, sizeof(bool));
    context->carry_flags = (bool *)calloc(nr_threads, sizeof(bool));
    context->scheduling = (uint8_t *)calloc(nr_threads, sizeof(uint8_t));
    if (context->registers == NULL || context->pcs == NULL || context->atomic_register == NULL || context->zero_flags == NULL
        || context->carry_flags == NULL || context->scheduling == NULL) {
        dpu_free_dpu_context(context);
        return NULL;
    }

    return context;
}

__API_SYMBOL__ dpu_error_t
dpu_pop_debug_context(struct dpu_t *dpu, dpu_context_t *debug_context)
{
    LOGV(__vc(), "%s (%08x)", __func__, dpu_get_id(dpu));

    *debug_context = dpu->debug_context;
    dpu->debug_context = NULL;

    return DPU_OK;
}

void
dpu_print_lldb_message_on_fault(struct dpu_t *dpu, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu->rank->handler_context->handler->print_lldb_message_on_fault(dpu, slice_id, dpu_id);
}

static dpu_error_t
extract_bkp_fault_id(struct dpu_t *dpu, iram_addr_t pc, uint32_t *bkp_fault_id)
{
    LOG_DPU(VERBOSE, dpu, "");

    dpu_error_t status;
    dpuinstruction_t instruction;

    if ((status = dpu_copy_from_iram_for_dpu(dpu, &instruction, pc, 1)) != DPU_OK) {
        goto end;
    }

    dpuinstruction_t fault_instruction_mask = FAULTi(0);

    if ((instruction & fault_instruction_mask) != fault_instruction_mask) {
        LOG_DPU(WARNING, dpu, "unexpected instruction when looking for FAULT (found: 0x%012lx)", instruction);
        *bkp_fault_id = DPU_FAULT_UNKNOWN_ID;
        goto end;
    }

    // We need to reconstruct the immediate
    uint32_t immediate = 0;
    for (int each_bit = 0; each_bit < 32; ++each_bit) {
        uint32_t bit_mask = 1 << each_bit;
        dpuinstruction_t bit_location = FAULTi(bit_mask) ^ fault_instruction_mask;
        dpuinstruction_t bit_in_instruction = instruction & bit_location;
        if (bit_in_instruction != 0) {
            immediate |= 1 << each_bit;
        }
    }
    *bkp_fault_id = immediate;

end:
    return status;
}

static dpu_error_t
decrement_thread_pc(struct dpu_t *dpu, dpu_thread_t thread, iram_addr_t *pc)
{
    LOG_DPU(VERBOSE, dpu, "");

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpuinstruction_t instruction;
    dpuinstruction_t modified_stop_j_instruction;
    uint32_t ignored;
    uint32_t dpu_is_running;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    if ((status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_DEBUG_ACTION))
        != DPU_OK) {
        goto end;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    if (*pc == 0) {
        *pc = rank->description->memories.iram_size - 1;
    } else {
        (*pc)--;
    }

    modified_stop_j_instruction = STOPci(BOOT_CC_TRUE, *pc);

    safe_add_query(query,
        dpu_query_build_read_iram_instruction_for_dpu(slice_id, member_id, 0, 1, &instruction),
        transaction,
        status,
        free_transaction);
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(
            slice_id, dpu_mask_one(member_id), 0, &modified_stop_j_instruction, 1),
        transaction,
        status,
        free_transaction);
    safe_add_query(
        query, dpu_query_build_boot_thread_for_previous(slice_id, thread, &ignored), transaction, status, free_transaction);

    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(
        query, dpu_query_build_read_dpu_run_state_for_previous(slice_id, &dpu_is_running), transaction, status, free_transaction);

    do {
        safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);
    } while ((dpu_is_running & mask_one) != 0);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_dpu(slice_id, member_id, dpu_mask_one(member_id), 0, &instruction, 1),
        transaction,
        status,
        free_transaction);
    safe_execute_transaction(transaction, rank, planner_status, status, free_transaction);

free_transaction:
    dpu_transaction_free(transaction);
end:
    status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_DEBUG_ACTION);

    return status;
}
