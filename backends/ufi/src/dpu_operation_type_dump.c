/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "dpu_operation_type.h"

const char *dpu_operation_type_names[NR_OF_DPU_OPERATION_TYPES] = {
    [DPU_OPERATION_EMPTY] = "DPU_OPERATION_EMPTY",
    [DPU_OPERATION_BIT_ORDER] = "DPU_OPERATION_BIT_ORDER",
    [DPU_OPERATION_IDENTITY] = "DPU_OPERATION_IDENTITY",
    [DPU_OPERATION_SOFTWARE_RESET] = "DPU_OPERATION_SOFTWARE_RESET",
    [DPU_OPERATION_THERMAL_CONFIG] = "DPU_OPERATION_THERMAL_CONFIG",
    [DPU_OPERATION_CMD_DURATION_FUTUR] = "DPU_OPERATION_CMD_DURATION_FUTUR",
    [DPU_OPERATION_RES_DURATION_FUTUR] = "DPU_OPERATION_RES_DURATION_FUTUR",
    [DPU_OPERATION_RES_SAMPLING_FUTUR] = "DPU_OPERATION_RES_SAMPLING_FUTUR",
    [DPU_OPERATION_BYTE_ORDER] = "DPU_OPERATION_BYTE_ORDER",
    [DPU_OPERATION_NOP] = "DPU_OPERATION_NOP",
    [DPU_OPERATION_SELECT_DPU_STRUCTURE] = "DPU_OPERATION_SELECT_DPU_STRUCTURE",
    [DPU_OPERATION_SELECT_DPU_FRAME] = "DPU_OPERATION_SELECT_DPU_FRAME",
    [DPU_OPERATION_SELECT_GROUP_STRUCTURE] = "DPU_OPERATION_SELECT_GROUP_STRUCTURE",
    [DPU_OPERATION_SELECT_GROUP_FRAME] = "DPU_OPERATION_SELECT_GROUP_FRAME",
    [DPU_OPERATION_SELECT_ALL_STRUCTURE] = "DPU_OPERATION_SELECT_ALL_STRUCTURE",
    [DPU_OPERATION_SELECT_ALL_FRAME] = "DPU_OPERATION_SELECT_ALL_FRAME",
    [DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE] = "DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE] = "DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME] = "DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME",
    [DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME] = "DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME",
    [DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME] = "DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME",
    [DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME] = "DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME] = "DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME] = "DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME] = "DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME] = "DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME] = "DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE] = "DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE] = "DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME] = "DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME] = "DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME] = "DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME",
    [DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE]
    = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE]
    = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE]
    = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME] = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME",
    [DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME",
    [DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME] = "DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME] = "DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME",
    [DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME",
    [DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME] = "DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME",
    [DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME] = "DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME",
    [DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME] = "DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME",
    [DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME",
    [DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME",
    [DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME",
    [DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE] = "DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE] = "DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME] = "DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME",
    [DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME] = "DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME",
    [DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME] = "DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME",
    [DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE] = "DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE] = "DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME] = "DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME",
    [DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME] = "DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME",
    [DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME] = "DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME",
    [DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME",
    [DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE] = "DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE] = "DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME] = "DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME] = "DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME] = "DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME",
    [DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME",
    [DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME",
    [DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE] = "DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME] = "DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME",
    [DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME",
    [DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE] = "DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE] = "DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME] = "DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME",
    [DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME] = "DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME",
    [DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME] = "DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME",
    [DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE] = "DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME] = "DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME",
    [DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME] = "DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME",
    [DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME] = "DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME",
    [DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE] = "DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE] = "DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE",
    [DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE] = "DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE",
    [DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME] = "DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME",
    [DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME] = "DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME",
    [DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME] = "DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME",
    [DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE]
    = "DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME] = "DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME",
    [DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME]
    = "DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE] = "DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE",
    [DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE]
    = "DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME] = "DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME",
    [DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME",
    [DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE] = "DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE",
    [DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME] = "DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME",
};
