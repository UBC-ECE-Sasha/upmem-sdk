/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_DEBUG_H
#define DPU_DEBUG_H

#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_debug.h
 * @brief C API for debug operations on DPUs.
 */

/**
 * @struct dpu_context_t
 * @brief Debugging information.
 * @var registers Storage for the working register values of each DPU thread
 * @var pcs Storage for the PC value of each DPU thread
 * @var atomic_register Storage for the atomic register value
 * @var zero_flags Storage for the zero flag value of each DPU thread
 * @var carry_flags Storage for the carry flag value of each DPU thread
 * @var nr_of_running_threads Number of running threads in the current debugging session
 * @var scheduling Scheduling of the DPU thread in the current debugging session
 * @var bkp_fault Whether a BKP fault was triggered
 * @var dma_fault Whether a DMA fault was triggered
 * @var mem_fault Whether a MEM fault was triggered
 * @var bkp_fault_thread_index The DPU thread index that triggered the BKP fault (if any)
 * @var dma_fault_thread_index The DPU thread index that triggered the DMA fault (if any)
 * @var mem_fault_thread_index The DPU thread index that triggered the MEM fault (if any)
 * @var bkp_fault_id The BKP fault ID associated to the current BKP fault (if any)
 */
typedef struct _dpu_context_t {
    uint32_t *registers;
    iram_addr_t *pcs;
    bool *atomic_register;
    bool *zero_flags;
    bool *carry_flags;
    uint8_t nr_of_running_threads;
    uint8_t *scheduling;
    bool bkp_fault;
    bool dma_fault;
    bool mem_fault;
    dpu_thread_t bkp_fault_thread_index;
    dpu_thread_t dma_fault_thread_index;
    dpu_thread_t mem_fault_thread_index;
    uint32_t bkp_fault_id;
} * dpu_context_t;

/**
 * @fn dpu_trigger_fault_on_rank
 * @brief Triggers a fault on the DPU rank, stopping the execution of all DPUs.
 * @param rank the unique identifier of the rank
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_trigger_fault_on_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_trigger_fault_on_dpu
 * @brief Triggers a fault on the DPU, stopping the execution of the DPU.
 * @param rank the unique identifier of the dpu
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_trigger_fault_on_dpu(struct dpu_t *dpu);

/**
 * @fn dpu_clear_fault_on_rank
 * @brief Clears the fault on the DPU rank, restarting the execution of all DPUs.
 * @param rank the unique identifier of the rank
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_clear_fault_on_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_clear_fault_on_dpu
 * @brief Clears the fault on the DPU, restarting the execution of the DPU.
 * @param rank the unique identifier of the dpu
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_clear_fault_on_dpu(struct dpu_t *dpu);

/**
 * @fn dpu_extract_pcs_for_dpu
 * @brief Fetches the PC for each thread of the DPU.
 *
 * The DPU should be stopped before calling this function.
 * The different fields of the debug context must be correctly allocated by the function caller.
 *
 * @param dpu the unique identifier of the dpu
 * @param context the debug context which will store the PC of all threads
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_extract_pcs_for_dpu(struct dpu_t *dpu, dpu_context_t context);

/**
 * @fn dpu_extract_context_for_dpu
 * @brief Fetches the internal context for each thread of the DPU.
 *
 * dpu_extract_pcs_for_dpu or dpu_initialize_fault_process_for_dpu must have been called before calling this function.
 * This function extracts the working register values, the carry and zero flags for each thread, and also the
 * atomic register values for the DPU.
 *
 * @param dpu the unique identifier of the dpu
 * @param context the debug context which will store the internal context of all threads
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_extract_context_for_dpu(struct dpu_t *dpu, dpu_context_t context);

dpu_error_t
dpu_restore_context_for_dpu(struct dpu_t *dpu, dpu_context_t context);

/**
 * @fn dpu_initialize_fault_process_for_dpu
 * @brief Prepares the DPU to be debugged after an execution fault.
 *
 * The DPU should be in fault before calling this function.
 * The different fields of the debug context must be correctly allocated by the function caller.
 *
 * @param dpu the unique identifier of the dpu
 * @param context the debug context which will store the thread scheduling and the different fault states
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_initialize_fault_process_for_dpu(struct dpu_t *dpu, dpu_context_t context);

/**
 * @fn dpu_finalize_fault_process_for_dpu
 * @brief Restarts the DPU execution after a fault and a potential debugging procedure.
 *
 * dpu_initialize_fault_process_for_dpu must have been called before calling this function.
 *
 * @param dpu the unique identifier of the dpu
 * @param context the debug context containing the thread scheduling information
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_finalize_fault_process_for_dpu(struct dpu_t *dpu, dpu_context_t context);

/**
 * @fn dpu_execute_thread_step_in_fault_for_dpu
 * @brief When debugging, makes the specified DPU thread execute one instruction.
 *
 * dpu_initialize_fault_process_for_dpu must have been called before calling this function.
 * The specified thread must have been booted, and still running.
 *
 * @param dpu the unique identifier of the dpu
 * @param thread the DPU thread which will execute a step
 * @param context the debug context containing the thread scheduling information
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_execute_thread_step_in_fault_for_dpu(struct dpu_t *dpu, dpu_thread_t thread, dpu_context_t context);

dpu_error_t
dpu_resume_dpus_for_rank(struct dpu_rank_t *rank, dpu_context_t context);
dpu_error_t
dpu_resume_threads_for_dpu(struct dpu_t *dpu, dpu_context_t context);
dpu_error_t
dpu_stop_dpus_for_rank(struct dpu_rank_t *rank, dpu_context_t context);
dpu_error_t
dpu_stop_threads_for_dpu(struct dpu_t *dpu, dpu_context_t context);
dpu_error_t
dpu_set_debug_slice_info(struct dpu_rank_t *rank,
    uint32_t slice_id,
    uint64_t structure_value,
    uint64_t slice_target,
    dpu_bitfield_t host_mux_mram_state);
dpu_error_t
dpu_save_slice_context_for_dpu(struct dpu_t *dpu);
DPU_API_DEPRECATED dpu_error_t
dpu_save_slice_context_and_extract_debug_info_from_pid_for_dpu(struct dpu_t *dpu, pid_t pid);
dpu_error_t
dpu_restore_slice_context_for_dpu(struct dpu_t *dpu);

dpu_error_t
dpu_set_debug_info_host_address_for_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_set_debug_info_host_address_for_rank(struct dpu_rank_t *rank);

void
dpu_print_lldb_message_on_fault(struct dpu_t *dpu, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_error_t
dpu_serialize_context(struct dpu_rank_t *rank,
    dpu_context_t context,
    uint8_t **serialized_context,
    uint32_t *serialized_context_size);
dpu_error_t
dpu_deserialize_context(struct dpu_rank_t *rank,
    dpu_context_t context,
    uint8_t *serialized_context,
    uint32_t serialized_context_size);

dpu_error_t
dpu_create_core_dump(struct dpu_rank_t *rank,
    const char *exe_path,
    const char *core_file_path,
    dpu_context_t context,
    uint8_t *wram,
    uint8_t *mram,
    uint8_t *iram,
    uint32_t wram_size,
    uint32_t mram_size,
    uint32_t iram_size);

void
dpu_free_dpu_context(dpu_context_t context);
dpu_context_t
dpu_alloc_dpu_context(struct dpu_rank_t *rank);

/**
 * @fn dpu_pop_debug_context
 * @brief This function removes, if existing, the debug context from a DPU structure
 * @param dpu the unique identifier of the DPU
 * @param debug_context The debug context, if existing, associated to the DPU
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_pop_debug_context(struct dpu_t *dpu, dpu_context_t *debug_context);

#endif // DPU_DEBUG_H
