/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <stdint.h>
#include "list.h"
#include "dpu_query.h"
#include "dpu_query_parameters.h"

int
dpu_query_fill_operations_list(dpu_query_t query, struct list_head *list)
{
    dpu_operation_t operation, tmp;

    switch (query->type) {
        case DPU_QUERY_IGNORED: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_BIT_ORDER: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BIT_ORDER)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_IDENTITY: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IDENTITY)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SOFTWARE_RESET: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_SOFTWARE_RESET)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_THERMAL_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THERMAL_CONFIG)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_COMMAND_DURATION_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_DURATION_FUTUR)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_RESULT_DURATION_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_DURATION_FUTUR)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_RESULT_SAMPLING_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_SAMPLING_FUTUR)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_BYTE_ORDER: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BYTE_ORDER)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_NOP: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOP)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SELECT_DPU: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SELECT_GROUP: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SELECT_ALL: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_CONTROL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_SELECT_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_WRITE_GROUP: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_DMA_CONTROL: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_WRITE_DMA_CONTROL: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_CLEAR_DMA_CONTROL: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_DMA_FAULT_THREAD_INDEX: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_BKP_FAULT_THREAD_INDEX: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_MEM_FAULT_THREAD_INDEX: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_PC_LSB: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_PC_MSB: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_PC_MODE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_WRITE_PC_MODE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_DPU_FAULT_STATE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_CLEAR_DPU_FAULT_STATE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SET_AND_STEP_DPU_FAULT_STATE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE))
                        == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_DPU_RUN_STATE: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_BKP_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_CLEAR_BKP_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SET_BKP_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_POISON_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_CLEAR_POISON_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_SET_POISON_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_CLEAR_DMA_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_CLEAR_MEM_FAULT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_CLEAR_STACK_UP: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_SET_STACK_UP: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_BOOT_THREAD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_RESUME_THREAD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_CLEAR_RUN_THREAD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_RUN_THREAD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_WRITE_IRAM_INSTRUCTION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    uint32_t nb_of_iterations = ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    uint32_t nb_of_iterations = ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    uint32_t nb_of_iterations = ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    uint32_t nb_of_iterations = ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE))
                                == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_IRAM_INSTRUCTION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    uint32_t nb_of_iterations = ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    uint32_t nb_of_iterations = ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_WRITE_WRAM_WORD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    uint32_t nb_of_iterations = ((dpu_query_write_wram_word_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_WRAM_WORD: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    uint32_t nb_of_iterations = ((dpu_query_read_wram_word_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    uint32_t nb_of_iterations = ((dpu_query_read_wram_word_parameters_t)(query->parameters))->size;
                    if (nb_of_iterations == 0) {
                        if ((operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL)
                            goto error;
                        list_add_tail(&operation->list, list);
                    } else {
                        for (uint32_t each_iteration = 0; each_iteration < nb_of_iterations; ++each_iteration) {
                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);

                            if ((operation = dpu_operation_new(DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME)) == NULL)
                                goto error;
                            list_add_tail(&operation->list, list);
                        }
                    }
                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_IREPAIR_AB_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_IREPAIR_CD_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_IREPAIR_OE_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_REGISTER_FILE_TIMING_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_COMMAND_BUS_DURATION_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_COMMAND_BUS_SAMPLING_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_COMMAND_BUS_SYNCHRONIZATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_RESULT_BUS_DURATION_CONFIGURATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_RESULT_BUS_SYNCHRONIZATION: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_NOTIFY_BIT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_READ_AND_SET_NOTIFY_BIT: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_GROUP: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_ALL: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_DEBUG_STD_REPLACE_STOP_ENABLED: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE))
                        == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_DEBUG_STD_REPLACE_CLEAR: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_DPU: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        case DPU_QUERY_DEBUG_STD_SAMPLE_PC: {
            switch (query->target.slice_target.type) {
                case DPU_SLICE_TARGET_PREVIOUS: {
                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    if ((operation = dpu_operation_new(DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME)) == NULL)
                        goto error;
                    list_add_tail(&operation->list, list);

                    break;
                }
                default: {
                    goto error;
                }
            }
            break;
        }
        default: {
            goto error;
        }
    }
    return 0;

error:
    list_for_each_entry_safe(operation, tmp, list, list) { dpu_operation_free(operation); }
    return -1;
}
