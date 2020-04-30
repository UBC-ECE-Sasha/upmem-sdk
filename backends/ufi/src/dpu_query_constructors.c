/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include "dpu_query.h"
#include "dpu_query_parameters.h"
#include "dpu_attributes.h"

dpu_query_t __API_SYMBOL__
dpu_query_build_ignored_for_control(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IGNORED;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_bit_order_for_control(dpu_slice_id_t slice_id,
    uint16_t c2d,
    uint16_t d2c,
    uint8_t ne,
    uint8_t stutter,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BIT_ORDER;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_bit_order_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_bit_order_parameters_t)(query->parameters))->c2d = c2d;
    ((dpu_query_bit_order_parameters_t)(query->parameters))->d2c = d2c;
    ((dpu_query_bit_order_parameters_t)(query->parameters))->ne = ne;
    ((dpu_query_bit_order_parameters_t)(query->parameters))->stutter = stutter;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_identity_for_control(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IDENTITY;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_software_reset_for_control(dpu_slice_id_t slice_id, uint8_t cycle_accurate, dpu_clock_division_t factor)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SOFTWARE_RESET;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_software_reset_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_software_reset_parameters_t)(query->parameters))->cycle_accurate = cycle_accurate;
    ((dpu_query_software_reset_parameters_t)(query->parameters))->factor = factor;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_thermal_configuration_for_control(dpu_slice_id_t slice_id, dpu_temperature_e threshold)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_THERMAL_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_thermal_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_thermal_configuration_parameters_t)(query->parameters))->threshold = threshold;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_command_duration_configuration_for_control(dpu_slice_id_t slice_id, uint8_t duration)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_COMMAND_DURATION_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_command_duration_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_command_duration_configuration_parameters_t)(query->parameters))->duration = duration;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_result_duration_configuration_for_control(dpu_slice_id_t slice_id, uint8_t duration)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESULT_DURATION_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_result_duration_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_result_duration_configuration_parameters_t)(query->parameters))->duration = duration;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_result_sampling_configuration_for_control(dpu_slice_id_t slice_id, uint8_t sampling)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESULT_SAMPLING_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_result_sampling_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_result_sampling_configuration_parameters_t)(query->parameters))->sampling = sampling;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_byte_order_for_control(dpu_slice_id_t slice_id, uint64_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BYTE_ORDER;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_nop_for_control(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_NOP;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_select_dpu_for_control(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SELECT_DPU;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_select_dpu_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_select_dpu_parameters_t)(query->parameters))->dpu_id = dpu_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_select_group_for_control(dpu_slice_id_t slice_id, dpu_group_id_t group_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SELECT_GROUP;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_select_group_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_select_group_parameters_t)(query->parameters))->group_id = group_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_select_all_for_control(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SELECT_ALL;
    query->target.slice_target.type = DPU_SLICE_TARGET_CONTROL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_group_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_group_id_t group_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_GROUP;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_group_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_group_parameters_t)(query->parameters))->group_id = group_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_group_for_all(dpu_slice_id_t slice_id, dpu_group_id_t group_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_GROUP;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_group_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_group_parameters_t)(query->parameters))->group_id = group_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_group_for_previous(dpu_slice_id_t slice_id, dpu_group_id_t group_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_GROUP;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_group_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_group_parameters_t)(query->parameters))->group_id = group_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dma_control_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_dma_ctrl_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DMA_CONTROL;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dma_control_for_previous(dpu_slice_id_t slice_id, dpu_dma_ctrl_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DMA_CONTROL;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_dma_control_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_DMA_CONTROL;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_dma_control_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0 = byte0;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1 = byte1;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2 = byte2;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3 = byte3;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4 = byte4;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5 = byte5;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_dma_control_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_DMA_CONTROL;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_dma_control_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0 = byte0;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1 = byte1;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2 = byte2;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3 = byte3;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4 = byte4;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5 = byte5;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_dma_control_for_all(dpu_slice_id_t slice_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_DMA_CONTROL;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_dma_control_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0 = byte0;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1 = byte1;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2 = byte2;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3 = byte3;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4 = byte4;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5 = byte5;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_dma_control_for_previous(dpu_slice_id_t slice_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_DMA_CONTROL;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_dma_control_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte0 = byte0;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte1 = byte1;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte2 = byte2;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte3 = byte3;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte4 = byte4;
    ((dpu_query_write_dma_control_parameters_t)(query->parameters))->byte5 = byte5;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_dma_control_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_DMA_CONTROL;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dma_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DMA_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dma_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DMA_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_bkp_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_BKP_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_bkp_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_BKP_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_mem_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_MEM_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_mem_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_MEM_FAULT_THREAD_INDEX;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_lsb_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint8_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_LSB;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_lsb_for_previous(dpu_slice_id_t slice_id, uint8_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_LSB;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_msb_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint8_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_MSB;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_msb_for_previous(dpu_slice_id_t slice_id, uint8_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_MSB;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_mode_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_pc_mode_e *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_MODE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_pc_mode_for_previous(dpu_slice_id_t slice_id, dpu_pc_mode_e *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_PC_MODE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_pc_mode_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_pc_mode_e pc_mode)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_PC_MODE;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_pc_mode_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode = pc_mode;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_pc_mode_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, dpu_pc_mode_e pc_mode)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_PC_MODE;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_pc_mode_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode = pc_mode;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_pc_mode_for_all(dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_PC_MODE;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_pc_mode_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode = pc_mode;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_pc_mode_for_previous(dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_PC_MODE;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_pc_mode_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_pc_mode_parameters_t)(query->parameters))->pc_mode = pc_mode;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_FAULT_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_fault_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_FAULT_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_fault_state_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_FAULT_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_fault_state_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_FAULT_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_dpu_fault_state_for_all(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_dpu_fault_state_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_and_step_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_AND_STEP_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_and_step_dpu_fault_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_AND_STEP_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_and_step_dpu_fault_state_for_all(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_AND_STEP_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_and_step_dpu_fault_state_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_AND_STEP_DPU_FAULT_STATE;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_run_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_RUN_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_run_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_RUN_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_run_state_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_RUN_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_dpu_run_state_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_DPU_RUN_STATE;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_BKP_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_bkp_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_BKP_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_bkp_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_BKP_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_BKP_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_bkp_fault_for_all(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_BKP_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_bkp_fault_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_BKP_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_BKP_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_bkp_fault_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_BKP_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_POISON_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_poison_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_POISON_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_POISON_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_poison_fault_for_all(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_POISON_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_poison_fault_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_POISON_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_POISON_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_set_poison_fault_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_SET_POISON_FAULT;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_dma_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_DMA_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_dma_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_DMA_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_dma_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_DMA_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_mem_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_MEM_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_mem_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_MEM_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_mem_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_MEM_FAULT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_stack_up_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_stack_up_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_stack_up_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_stack_up_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_stack_up_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_stack_up_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_stack_up_for_all(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_stack_up_for_previous(dpu_slice_id_t slice_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_STACK_UP;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_boot_thread_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BOOT_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_boot_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_boot_thread_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BOOT_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_boot_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_boot_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BOOT_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_boot_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_boot_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_BOOT_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_boot_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_boot_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_resume_thread_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESUME_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_resume_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_resume_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESUME_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_resume_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_resume_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESUME_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_resume_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_resume_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESUME_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_resume_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_resume_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_run_thread_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_thread_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_clear_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_run_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_clear_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_run_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_clear_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_clear_run_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_CLEAR_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_clear_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_clear_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_run_thread_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_run_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_run_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_run_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_RUN_THREAD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_run_thread_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_run_thread_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_iram_instruction_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_IRAM_INSTRUCTION;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->selected_dpus = selected_dpus;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data = data;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_iram_instruction_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_IRAM_INSTRUCTION;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->selected_dpus = selected_dpus;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data = data;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_iram_instruction_for_all(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_IRAM_INSTRUCTION;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->selected_dpus = selected_dpus;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data = data;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_iram_instruction_for_previous(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_IRAM_INSTRUCTION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->selected_dpus = selected_dpus;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->data = data;
    ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_iram_instruction_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    iram_addr_t address,
    iram_size_t size,
    dpuinstruction_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_IRAM_INSTRUCTION;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_iram_instruction_for_previous(dpu_slice_id_t slice_id,
    iram_addr_t address,
    iram_size_t size,
    dpuinstruction_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_IRAM_INSTRUCTION;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_iram_instruction_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->address = address;
    ((dpu_query_read_iram_instruction_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_write_wram_word_for_previous(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    wram_addr_t address,
    dpuword_t *data,
    wram_size_t size)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_WRITE_WRAM_WORD;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_write_wram_word_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_write_wram_word_parameters_t)(query->parameters))->selected_dpus = selected_dpus;
    ((dpu_query_write_wram_word_parameters_t)(query->parameters))->address = address;
    ((dpu_query_write_wram_word_parameters_t)(query->parameters))->data = data;
    ((dpu_query_write_wram_word_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_wram_word_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    wram_addr_t address,
    wram_size_t size,
    dpuword_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_WRAM_WORD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_wram_word_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_wram_word_parameters_t)(query->parameters))->address = address;
    ((dpu_query_read_wram_word_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_wram_word_for_previous(dpu_slice_id_t slice_id, wram_addr_t address, wram_size_t size, dpuword_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_WRAM_WORD;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_wram_word_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_wram_word_parameters_t)(query->parameters))->address = address;
    ((dpu_query_read_wram_word_parameters_t)(query->parameters))->size = size;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_AB_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t lsb_A,
    uint8_t lsb_B,
    uint8_t msb)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_AB_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_AB_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_A = lsb_A;
    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_B = lsb_B;
    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->msb = msb;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_AB_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t lsb_A, uint8_t lsb_B, uint8_t msb)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_AB_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_AB_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_A = lsb_A;
    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->lsb_B = lsb_B;
    ((dpu_query_irepair_AB_configuration_parameters_t)(query->parameters))->msb = msb;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_CD_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t lsb_C,
    uint8_t lsb_D,
    uint8_t msb)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_CD_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_CD_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_C = lsb_C;
    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_D = lsb_D;
    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->msb = msb;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_CD_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t lsb_C, uint8_t lsb_D, uint8_t msb)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_CD_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_CD_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_C = lsb_C;
    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->lsb_D = lsb_D;
    ((dpu_query_irepair_CD_configuration_parameters_t)(query->parameters))->msb = msb;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_OE_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t even_index,
    uint8_t odd_index,
    uint8_t timing)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_OE_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_OE_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->even_index = even_index;
    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->odd_index = odd_index;
    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->timing = timing;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_irepair_OE_configuration_for_previous(dpu_slice_id_t slice_id,
    uint8_t even_index,
    uint8_t odd_index,
    uint8_t timing)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_IREPAIR_OE_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_irepair_OE_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->even_index = even_index;
    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->odd_index = odd_index;
    ((dpu_query_irepair_OE_configuration_parameters_t)(query->parameters))->timing = timing;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_register_file_timing_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t rf_timing)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_REGISTER_FILE_TIMING_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_register_file_timing_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_register_file_timing_configuration_parameters_t)(query->parameters))->rf_timing = rf_timing;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_command_bus_duration_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t duration)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_COMMAND_BUS_DURATION_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_command_bus_duration_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_command_bus_duration_configuration_parameters_t)(query->parameters))->duration = duration;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_command_bus_sampling_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t sampling)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_COMMAND_BUS_SAMPLING_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_command_bus_sampling_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_command_bus_sampling_configuration_parameters_t)(query->parameters))->sampling = sampling;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_command_bus_synchronization_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_COMMAND_BUS_SYNCHRONIZATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_result_bus_duration_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t duration)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESULT_BUS_DURATION_CONFIGURATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_result_bus_duration_configuration_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_result_bus_duration_configuration_parameters_t)(query->parameters))->duration = duration;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_result_bus_synchronization_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_RESULT_BUS_SYNCHRONIZATION;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_clear_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_clear_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_clear_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_clear_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_clear_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_clear_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_set_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_GROUP;
    query->target.slice_id = slice_id;
    query->target.slice_target.group_id = group_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_set_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_ALL;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_set_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_read_and_set_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_READ_AND_SET_NOTIFY_BIT;
    query->result = result;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    if ((query->parameters = malloc(sizeof(struct _dpu_query_read_and_set_notify_bit_parameters_t))) == NULL)
        goto free_query;

    ((dpu_query_read_and_set_notify_bit_parameters_t)(query->parameters))->thread_id = thread_id;

    goto end;

free_query:
    free(query);
    query = NULL;
end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_debug_std_replace_stop_enabled_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_DEBUG_STD_REPLACE_STOP_ENABLED;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_debug_std_replace_stop_enabled_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_DEBUG_STD_REPLACE_STOP_ENABLED;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_debug_std_replace_clear_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_DEBUG_STD_REPLACE_CLEAR;
    query->target.slice_target.type = DPU_SLICE_TARGET_DPU;
    query->target.slice_id = slice_id;
    query->target.slice_target.dpu_id = dpu_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_debug_std_replace_clear_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_DEBUG_STD_REPLACE_CLEAR;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}

dpu_query_t __API_SYMBOL__
dpu_query_build_debug_std_sample_pc_for_previous(dpu_slice_id_t slice_id)
{
    dpu_query_t query;

    if ((query = malloc(sizeof(*query))) == NULL)
        goto end;

    query->type = DPU_QUERY_DEBUG_STD_SAMPLE_PC;
    query->target.slice_target.type = DPU_SLICE_TARGET_PREVIOUS;
    query->target.slice_id = slice_id;

    query->parameters = NULL;

end:
    return query;
}
