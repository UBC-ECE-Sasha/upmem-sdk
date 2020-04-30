/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <stdint.h>
#include "dpu_query.h"

void
dpu_query_result_dump(FILE *output, uint32_t indent, dpu_query_result_t result, dpu_query_type_e type)
{
    fprintf(output, "%*sresult @%p {\n", indent, "", result);

    switch (type) {
        case DPU_QUERY_READ_PC_LSB:
        case DPU_QUERY_READ_PC_MSB:
            fprintf(output, "%*svalue = 0x%02x\n", indent + 4, "", *((uint8_t *)result));
            break;
        case DPU_QUERY_BYTE_ORDER:
            fprintf(output, "%*svalue = 0x%016lx\n", indent + 4, "", *((uint64_t *)result));
            break;
        case DPU_QUERY_BIT_ORDER:
        case DPU_QUERY_IDENTITY:
        case DPU_QUERY_READ_DPU_FAULT_STATE:
        case DPU_QUERY_READ_DPU_RUN_STATE:
        case DPU_QUERY_READ_BKP_FAULT:
        case DPU_QUERY_READ_POISON_FAULT:
        case DPU_QUERY_READ_AND_CLEAR_DMA_FAULT:
        case DPU_QUERY_READ_AND_CLEAR_MEM_FAULT:
        case DPU_QUERY_READ_AND_CLEAR_STACK_UP:
        case DPU_QUERY_READ_AND_SET_STACK_UP:
        case DPU_QUERY_BOOT_THREAD:
        case DPU_QUERY_RESUME_THREAD:
        case DPU_QUERY_CLEAR_RUN_THREAD:
        case DPU_QUERY_READ_RUN_THREAD:
        case DPU_QUERY_READ_NOTIFY_BIT:
        case DPU_QUERY_READ_AND_CLEAR_NOTIFY_BIT:
        case DPU_QUERY_READ_AND_SET_NOTIFY_BIT:
            fprintf(output, "%*svalue = 0x%08x\n", indent + 4, "", *((uint32_t *)result));
            break;
        case DPU_QUERY_READ_DMA_CONTROL:
            fprintf(output, "%*svalue = 0x%x\n", indent + 4, "", *((dpu_dma_ctrl_t *)result));
            break;
        case DPU_QUERY_READ_PC_MODE:
            fprintf(output, "%*svalue = 0x%x\n", indent + 4, "", *((dpu_pc_mode_e *)result));
            break;
        case DPU_QUERY_READ_DMA_FAULT_THREAD_INDEX:
        case DPU_QUERY_READ_BKP_FAULT_THREAD_INDEX:
        case DPU_QUERY_READ_MEM_FAULT_THREAD_INDEX:
            fprintf(output, "%*svalue = 0x%x\n", indent + 4, "", *((dpu_thread_t *)result));
            break;
        default:
            break;
    }

    fprintf(output, "%*s}\n", indent, "");
}
