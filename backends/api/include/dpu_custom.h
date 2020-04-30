/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_CUSTOM_H
#define DPU_CUSTOM_H

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_custom.h
 * @brief C API for custom operations on DPUs.
 */

/**
 * @enum dpu_custom_command_t
 * @brief Custom DPU operations that may have different behaviors on different targets.
 * @var DPU_COMMAND_DPU_SOFT_RESET      Reset for a single DPU
 * @var DPU_COMMAND_ALL_SOFT_RESET      Reset for a slice
 * @var DPU_COMMAND_DPU_PREEXECUTION    Allows configuration right before booting DPU
 * @var DPU_COMMAND_ALL_PREEXECUTION    Allows configuration right before booting DPUs
 * @var DPU_COMMAND_DPU_POSTEXECUTION   Allows configuration right after the end of DPU execution
 * @var DPU_COMMAND_ALL_POSTEXECUTION   Allows configuration right after the end of DPUs execution
 * @var DPU_COMMAND_SET_COMMAND_TRACE   Tells driver to start dumping CI commands
 * @var DPU_COMMAND_CUSTOM_LABEL        Add custom label for the current timestamp
 * @var DPU_COMMAND_POSTMORTEM          Detailed backend postmortem
 * @var DPU_COMMAND_SYSTEM_REPORT       Report additional backend status
 * @var DPU_COMMAND_BINARY_PATH         Provides the DPU binary path to the backend
 * @var DPU_COMMAND_EVENT_START         Notifies that an event has started
 * @var DPU_COMMAND_EVENT_END           Notifies that an event has ended
 */
typedef enum _dpu_custom_command_t {
    DPU_COMMAND_DPU_SOFT_RESET,
    DPU_COMMAND_ALL_SOFT_RESET,
    DPU_COMMAND_DPU_PREEXECUTION,
    DPU_COMMAND_ALL_PREEXECUTION,
    DPU_COMMAND_DPU_POSTEXECUTION,
    DPU_COMMAND_ALL_POSTEXECUTION,
    DPU_COMMAND_SET_COMMAND_TRACE,
    DPU_COMMAND_CUSTOM_LABEL,
    DPU_COMMAND_POSTMORTEM,
    DPU_COMMAND_SYSTEM_REPORT,
    DPU_COMMAND_BINARY_PATH,
    DPU_COMMAND_EVENT_START,
    DPU_COMMAND_EVENT_END,
} dpu_custom_command_t;

/**
 * @typedef dpu_custom_command_args_t
 * @brief Generic argument type for a DPU custom operation.
 */
typedef void *dpu_custom_command_args_t;

/**
 * @fn dpu_custom_for_rank
 * @param rank the unique identifier of the rank
 * @param command Custom command identifier to send
 * @param args Pointer to arguments for the command
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_custom_for_rank(struct dpu_rank_t *rank, dpu_custom_command_t command, dpu_custom_command_args_t args);

/**
 * @fn dpu_custom_for_dpu
 * @param dpu the unique identifier of the DPU
 * @param command Custom command identifier to send
 * @param args Pointer to arguments for the command
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_custom_for_dpu(struct dpu_t *dpu, dpu_custom_command_t command, dpu_custom_command_args_t args);

#endif // DPU_CUSTOM_H
