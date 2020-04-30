/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "dpu_query.h"
#include "dpu_operation.h"
#ifdef DEBUG_TRACE
#include "dpu_commands_trace.h"
#else
#include "dpu_commands.h"
#endif
#include "dpu_query_parameters.h"

uint64_t
dpu_operation_to_command_build(dpu_operation_type_e type, dpu_query_t query)
{
    switch (type) {
        case DPU_OPERATION_EMPTY:
            return CI_FRAME_DPU_OPERATION_EMPTY;
        case DPU_OPERATION_BIT_ORDER:
            return CI_FRAME_DPU_OPERATION_BIT_ORDER(((dpu_query_bit_order_parameters_t)(query->parameters))->ne,
                ((dpu_query_bit_order_parameters_t)(query->parameters))->stutter,
                ((dpu_query_bit_order_parameters_t)(query->parameters))->c2d,
                ((dpu_query_bit_order_parameters_t)(query->parameters))->d2c);
        case DPU_OPERATION_IDENTITY:
            return CI_FRAME_DPU_OPERATION_IDENTITY;
        case DPU_OPERATION_SOFTWARE_RESET:
            return CI_FRAME_DPU_OPERATION_SOFTWARE_RESET(((dpu_query_software_reset_parameters_t)(query->parameters))->factor,
                ((dpu_query_software_reset_parameters_t)(query->parameters))->cycle_accurate);
        case DPU_OPERATION_THERMAL_CONFIG:
            return CI_FRAME_DPU_OPERATION_THERMAL_CONFIG(
                ((dpu_query_thermal_configuration_parameters_t)(query->parameters))->threshold);
        case DPU_OPERATION_CMD_DURATION_FUTUR:
            return CI_FRAME_DPU_OPERATION_CMD_DURATION_FUTUR(
                ((dpu_query_command_duration_configuration_parameters_t)(query->parameters))->duration);
        case DPU_OPERATION_RES_DURATION_FUTUR:
            return CI_FRAME_DPU_OPERATION_RES_DURATION_FUTUR(
                ((dpu_query_result_duration_configuration_parameters_t)(query->parameters))->duration);
        case DPU_OPERATION_RES_SAMPLING_FUTUR:
            return CI_FRAME_DPU_OPERATION_RES_SAMPLING_FUTUR(
                ((dpu_query_result_sampling_configuration_parameters_t)(query->parameters))->sampling);
        case DPU_OPERATION_BYTE_ORDER:
            return CI_FRAME_DPU_OPERATION_BYTE_ORDER;
        case DPU_OPERATION_NOP:
            return CI_FRAME_DPU_OPERATION_NOP;
        case DPU_OPERATION_SELECT_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_SELECT_DPU_STRUCTURE;
        case DPU_OPERATION_SELECT_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_SELECT_DPU_FRAME(((dpu_query_select_dpu_parameters_t)(query->parameters))->dpu_id);
        case DPU_OPERATION_SELECT_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_SELECT_GROUP_STRUCTURE;
        case DPU_OPERATION_SELECT_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_SELECT_GROUP_FRAME(
                ((dpu_query_select_group_parameters_t)(query->parameters))->group_id);
        case DPU_OPERATION_SELECT_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_SELECT_ALL_STRUCTURE;
        case DPU_OPERATION_SELECT_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_SELECT_ALL_FRAME;
        case DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_write_group_parameters_t)(query->parameters))->group_id);
        case DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME(
                ((dpu_query_write_group_parameters_t)(query->parameters))->group_id);
        case DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME(
                ((dpu_query_write_group_parameters_t)(query->parameters))->group_id);
        case DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE(
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5);
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4);
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE(
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5);
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME(query->target.slice_target.group_id,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4);
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME(
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5);
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME(
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4,
                ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5);
        case DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode);
        case DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode);
        case DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME(
                ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode);
        case DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME(
                ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode);
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME(query->target.slice_target.group_id);
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME;
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME;
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME(query->target.slice_target.group_id);
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME(query->target.slice_target.group_id);
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME;
        case DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME;
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME;
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME;
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME;
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME(query->target.slice_target.group_id);
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME(query->target.slice_target.group_id);
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE(
                ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE(
                ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME(
                ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME(
                ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE(
                ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE(
                ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME(
                ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME(
                ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE(
                ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE(
                ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME(
                ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME(
                ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE(
                ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE(
                ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME(
                ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME(
                ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE(
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index),
                (((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE(
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index),
                (((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME(query->target.slice_target.group_id,
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE(
                (((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME(
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE(
                (((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME(
                *((((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data) + query->_index),
                (((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address + (query->_index >> 3)));
        case DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE(
                (((dpu_query_write_wram_word_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME(
                *((((dpu_query_write_wram_word_parameters_t)(query->parameters))->data) + query->_index),
                (((dpu_query_write_wram_word_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE(
                (((dpu_query_read_wram_word_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                (((dpu_query_read_wram_word_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME(
                (((dpu_query_read_wram_word_parameters_t)(query->parameters))->address + query->_index));
        case DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->msb,
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_A,
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_B);
        case DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME(
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->msb,
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_A,
                ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_B);
        case DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->msb,
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_C,
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_D);
        case DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME(
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->msb,
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_C,
                ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_D);
        case DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->odd_index,
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->even_index,
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->timing);
        case DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME(
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->odd_index,
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->even_index,
                ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->timing);
        case DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME(
                ((dpu_query_register_file_timing_configuration_parameters_t)(query->parameters))->rf_timing);
        case DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME(
                ((dpu_query_command_bus_duration_configuration_parameters_t)(query->parameters))->duration);
        case DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME(
                ((dpu_query_command_bus_sampling_configuration_parameters_t)(query->parameters))->sampling);
        case DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME(
                ((dpu_query_result_bus_duration_configuration_parameters_t)(query->parameters))->duration);
        case DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE(
                ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME(
                query->target.slice_target.dpu_id, ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE(
                ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME(
                query->target.slice_target.group_id, ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME(
                ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME(
                ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE(
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE(
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME(query->target.slice_target.group_id,
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME(
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME(
                ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE(
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME(query->target.slice_target.dpu_id,
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE(
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME(query->target.slice_target.group_id,
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE;
        case DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME(
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME(
                ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id);
        case DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE;
        case DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME(query->target.slice_target.dpu_id);
        case DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME;
        case DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE;
        case DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME:
            return CI_FRAME_DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME;
        default:
            return CI_FRAME_DPU_OPERATION_EMPTY;
    }
}
