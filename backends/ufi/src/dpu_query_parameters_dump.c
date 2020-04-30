/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <stdint.h>
#include "dpu_query.h"
#include "dpu_query_parameters.h"

void
dpu_query_parameters_dump(FILE *output, uint32_t indent, dpu_query_parameters_t parameters, dpu_query_type_e type)
{
    fprintf(output, "%*sparameters @%p {\n", indent, "", parameters);

    switch (type) {
        case DPU_QUERY_BIT_ORDER: {
            dpu_query_bit_order_parameters_t typed_parameters = (dpu_query_bit_order_parameters_t)parameters;

            fprintf(output, "%*sc2d = 0x%x\n", indent + 4, "", typed_parameters->c2d);
            fprintf(output, "%*sd2c = 0x%x\n", indent + 4, "", typed_parameters->d2c);
            fprintf(output, "%*sne = 0x%x\n", indent + 4, "", typed_parameters->ne);
            fprintf(output, "%*sstutter = 0x%x\n", indent + 4, "", typed_parameters->stutter);

            break;
        }
        case DPU_QUERY_SOFTWARE_RESET: {
            dpu_query_software_reset_parameters_t typed_parameters = (dpu_query_software_reset_parameters_t)parameters;

            fprintf(output, "%*scycle_accurate = 0x%x\n", indent + 4, "", typed_parameters->cycle_accurate);
            fprintf(output, "%*sfactor = 0x%x\n", indent + 4, "", typed_parameters->factor);

            break;
        }
        case DPU_QUERY_THERMAL_CONFIGURATION: {
            dpu_query_thermal_configuration_parameters_t typed_parameters
                = (dpu_query_thermal_configuration_parameters_t)parameters;

            fprintf(output, "%*sthreshold = 0x%x\n", indent + 4, "", typed_parameters->threshold);

            break;
        }
        case DPU_QUERY_COMMAND_DURATION_CONFIGURATION: {
            dpu_query_command_duration_configuration_parameters_t typed_parameters
                = (dpu_query_command_duration_configuration_parameters_t)parameters;

            fprintf(output, "%*sduration = 0x%x\n", indent + 4, "", typed_parameters->duration);

            break;
        }
        case DPU_QUERY_RESULT_DURATION_CONFIGURATION: {
            dpu_query_result_duration_configuration_parameters_t typed_parameters
                = (dpu_query_result_duration_configuration_parameters_t)parameters;

            fprintf(output, "%*sduration = 0x%x\n", indent + 4, "", typed_parameters->duration);

            break;
        }
        case DPU_QUERY_RESULT_SAMPLING_CONFIGURATION: {
            dpu_query_result_sampling_configuration_parameters_t typed_parameters
                = (dpu_query_result_sampling_configuration_parameters_t)parameters;

            fprintf(output, "%*ssampling = 0x%x\n", indent + 4, "", typed_parameters->sampling);

            break;
        }
        case DPU_QUERY_SELECT_DPU: {
            dpu_query_select_dpu_parameters_t typed_parameters = (dpu_query_select_dpu_parameters_t)parameters;

            fprintf(output, "%*sdpu_id = 0x%x\n", indent + 4, "", typed_parameters->dpu_id);

            break;
        }
        case DPU_QUERY_SELECT_GROUP: {
            dpu_query_select_group_parameters_t typed_parameters = (dpu_query_select_group_parameters_t)parameters;

            fprintf(output, "%*sgroup_id = 0x%x\n", indent + 4, "", typed_parameters->group_id);

            break;
        }
        case DPU_QUERY_WRITE_GROUP: {
            dpu_query_write_group_parameters_t typed_parameters = (dpu_query_write_group_parameters_t)parameters;

            fprintf(output, "%*sgroup_id = 0x%x\n", indent + 4, "", typed_parameters->group_id);

            break;
        }
        case DPU_QUERY_WRITE_DMA_CONTROL: {
            dpu_query_write_dma_control_parameters_t typed_parameters = (dpu_query_write_dma_control_parameters_t)parameters;

            fprintf(output, "%*sbyte0 = 0x%x\n", indent + 4, "", typed_parameters->byte0);
            fprintf(output, "%*sbyte1 = 0x%x\n", indent + 4, "", typed_parameters->byte1);
            fprintf(output, "%*sbyte2 = 0x%x\n", indent + 4, "", typed_parameters->byte2);
            fprintf(output, "%*sbyte3 = 0x%x\n", indent + 4, "", typed_parameters->byte3);
            fprintf(output, "%*sbyte4 = 0x%x\n", indent + 4, "", typed_parameters->byte4);
            fprintf(output, "%*sbyte5 = 0x%x\n", indent + 4, "", typed_parameters->byte5);

            break;
        }
        case DPU_QUERY_WRITE_PC_MODE: {
            dpu_query_write_pc_mode_parameters_t typed_parameters = (dpu_query_write_pc_mode_parameters_t)parameters;

            fprintf(output, "%*spc_mode = 0x%x\n", indent + 4, "", typed_parameters->pc_mode);

            break;
        }
        case DPU_QUERY_BOOT_THREAD: {
            dpu_query_boot_thread_parameters_t typed_parameters = (dpu_query_boot_thread_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_RESUME_THREAD: {
            dpu_query_resume_thread_parameters_t typed_parameters = (dpu_query_resume_thread_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_CLEAR_RUN_THREAD: {
            dpu_query_clear_run_thread_parameters_t typed_parameters = (dpu_query_clear_run_thread_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_READ_RUN_THREAD: {
            dpu_query_read_run_thread_parameters_t typed_parameters = (dpu_query_read_run_thread_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_WRITE_IRAM_INSTRUCTION: {
            dpu_query_write_iram_instruction_parameters_t typed_parameters
                = (dpu_query_write_iram_instruction_parameters_t)parameters;

            fprintf(output, "%*saddress = 0x%x\n", indent + 4, "", typed_parameters->address);
            fprintf(output, "%*sdata = %p\n", indent + 4, "", typed_parameters->data);
            fprintf(output, "%*sselected_dpus = 0x%x\n", indent + 4, "", typed_parameters->selected_dpus);
            fprintf(output, "%*ssize = 0x%x\n", indent + 4, "", typed_parameters->size);

            break;
        }
        case DPU_QUERY_READ_IRAM_INSTRUCTION: {
            dpu_query_read_iram_instruction_parameters_t typed_parameters
                = (dpu_query_read_iram_instruction_parameters_t)parameters;

            fprintf(output, "%*saddress = 0x%x\n", indent + 4, "", typed_parameters->address);
            fprintf(output, "%*ssize = 0x%x\n", indent + 4, "", typed_parameters->size);

            break;
        }
        case DPU_QUERY_WRITE_WRAM_WORD: {
            dpu_query_write_wram_word_parameters_t typed_parameters = (dpu_query_write_wram_word_parameters_t)parameters;

            fprintf(output, "%*saddress = 0x%x\n", indent + 4, "", typed_parameters->address);
            fprintf(output, "%*sdata = %p\n", indent + 4, "", typed_parameters->data);
            fprintf(output, "%*sselected_dpus = 0x%x\n", indent + 4, "", typed_parameters->selected_dpus);
            fprintf(output, "%*ssize = 0x%x\n", indent + 4, "", typed_parameters->size);

            break;
        }
        case DPU_QUERY_READ_WRAM_WORD: {
            dpu_query_read_wram_word_parameters_t typed_parameters = (dpu_query_read_wram_word_parameters_t)parameters;

            fprintf(output, "%*saddress = 0x%x\n", indent + 4, "", typed_parameters->address);
            fprintf(output, "%*ssize = 0x%x\n", indent + 4, "", typed_parameters->size);

            break;
        }
        case DPU_QUERY_IREPAIR_AB_CONFIGURATION: {
            dpu_query_irepair_AB_configuration_parameters_t typed_parameters
                = (dpu_query_irepair_AB_configuration_parameters_t)parameters;

            fprintf(output, "%*slsb_A = 0x%x\n", indent + 4, "", typed_parameters->lsb_A);
            fprintf(output, "%*slsb_B = 0x%x\n", indent + 4, "", typed_parameters->lsb_B);
            fprintf(output, "%*smsb = 0x%x\n", indent + 4, "", typed_parameters->msb);

            break;
        }
        case DPU_QUERY_IREPAIR_CD_CONFIGURATION: {
            dpu_query_irepair_CD_configuration_parameters_t typed_parameters
                = (dpu_query_irepair_CD_configuration_parameters_t)parameters;

            fprintf(output, "%*slsb_C = 0x%x\n", indent + 4, "", typed_parameters->lsb_C);
            fprintf(output, "%*slsb_D = 0x%x\n", indent + 4, "", typed_parameters->lsb_D);
            fprintf(output, "%*smsb = 0x%x\n", indent + 4, "", typed_parameters->msb);

            break;
        }
        case DPU_QUERY_IREPAIR_OE_CONFIGURATION: {
            dpu_query_irepair_OE_configuration_parameters_t typed_parameters
                = (dpu_query_irepair_OE_configuration_parameters_t)parameters;

            fprintf(output, "%*seven_index = 0x%x\n", indent + 4, "", typed_parameters->even_index);
            fprintf(output, "%*sodd_index = 0x%x\n", indent + 4, "", typed_parameters->odd_index);
            fprintf(output, "%*stiming = 0x%x\n", indent + 4, "", typed_parameters->timing);

            break;
        }
        case DPU_QUERY_REGISTER_FILE_TIMING_CONFIGURATION: {
            dpu_query_register_file_timing_configuration_parameters_t typed_parameters
                = (dpu_query_register_file_timing_configuration_parameters_t)parameters;

            fprintf(output, "%*srf_timing = 0x%x\n", indent + 4, "", typed_parameters->rf_timing);

            break;
        }
        case DPU_QUERY_COMMAND_BUS_DURATION_CONFIGURATION: {
            dpu_query_command_bus_duration_configuration_parameters_t typed_parameters
                = (dpu_query_command_bus_duration_configuration_parameters_t)parameters;

            fprintf(output, "%*sduration = 0x%x\n", indent + 4, "", typed_parameters->duration);

            break;
        }
        case DPU_QUERY_COMMAND_BUS_SAMPLING_CONFIGURATION: {
            dpu_query_command_bus_sampling_configuration_parameters_t typed_parameters
                = (dpu_query_command_bus_sampling_configuration_parameters_t)parameters;

            fprintf(output, "%*ssampling = 0x%x\n", indent + 4, "", typed_parameters->sampling);

            break;
        }
        case DPU_QUERY_RESULT_BUS_DURATION_CONFIGURATION: {
            dpu_query_result_bus_duration_configuration_parameters_t typed_parameters
                = (dpu_query_result_bus_duration_configuration_parameters_t)parameters;

            fprintf(output, "%*sduration = 0x%x\n", indent + 4, "", typed_parameters->duration);

            break;
        }
        case DPU_QUERY_READ_NOTIFY_BIT: {
            dpu_query_read_notify_bit_parameters_t typed_parameters = (dpu_query_read_notify_bit_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT: {
            dpu_query_read_and_clear_notify_bit_parameters_t typed_parameters
                = (dpu_query_read_and_clear_notify_bit_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        case DPU_QUERY_READ_AND_SET_NOTIFY_BIT: {
            dpu_query_read_and_set_notify_bit_parameters_t typed_parameters
                = (dpu_query_read_and_set_notify_bit_parameters_t)parameters;

            fprintf(output, "%*sthread_id = 0x%x\n", indent + 4, "", typed_parameters->thread_id);

            break;
        }
        default:
            break;
    }

    fprintf(output, "%*s}\n", indent, "");
}
