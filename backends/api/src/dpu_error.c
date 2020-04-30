/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <dpu_error.h>
#include <dpu_attributes.h>

__API_SYMBOL__ const char *
dpu_error_to_string(dpu_error_t status)
{
    switch (status) {
        case DPU_OK:
            return "success";
        case DPU_ERR_SYSTEM:
            return "system error";
        case DPU_ERR_DPU_ALREADY_RUNNING:
            return "dpu is already running";
        case DPU_ERR_INVALID_LAUNCH_POLICY:
            return "invalid launch policy";
        case DPU_ERR_DPU_FAULT:
            return "dpu is in fault";
        case DPU_ERR_INTERNAL:
            return "internal error";
        case DPU_ERR_DRIVER:
            return "driver error";
        case DPU_ERR_INVALID_THREAD_ID:
            return "invalid thread id";
        case DPU_ERR_INVALID_NOTIFY_ID:
            return "invalid notify id";
        case DPU_ERR_INVALID_WRAM_ACCESS:
            return "invalid wram access";
        case DPU_ERR_INVALID_IRAM_ACCESS:
            return "invalid iram access";
        case DPU_ERR_MRAM_BUSY:
            return "mram is busy";
        case DPU_ERR_INVALID_MRAM_ACCESS:
            return "invalid mram access";
        case DPU_ERR_CORRUPTED_MEMORY:
            return "corrupted memory";
        case DPU_ERR_DPU_DISABLED:
            return "dpu is disabled";
        case DPU_ERR_TIMEOUT:
            return "result timeout";
        case DPU_ERR_INVALID_PROFILE:
            return "invalid profile";
        case DPU_ERR_ALLOCATION:
            return "dpu allocation error";
        case DPU_ERR_UNKNOWN_SYMBOL:
            return "undefined symbol";
        case DPU_ERR_LOG_FORMAT:
            return "logging format error";
        case DPU_ERR_LOG_CONTEXT_MISSING:
            return "logging context missing";
        case DPU_ERR_LOG_BUFFER_TOO_SMALL:
            return "logging buffer too small";
        case DPU_ERR_INVALID_DPU_SET:
            return "invalid action for this dpu set";
        case DPU_ERR_INVALID_SYMBOL_ACCESS:
            return "invalid memory symbol access";
        case DPU_ERR_TRANSFER_ALREADY_SET:
            return "dpu transfer is already set";
        case DPU_ERR_DIFFERENT_DPU_PROGRAMS:
            return "cannot access symbol on dpus with different programs";
        case DPU_ERR_INVALID_MEMORY_TRANSFER:
            return "invalid memory transfer";
        case DPU_ERR_ELF_INVALID_FILE:
            return "invalid ELF file";
        case DPU_ERR_ELF_NO_SUCH_FILE:
            return "no such ELF file";
        case DPU_ERR_ELF_NO_SUCH_SECTION:
            return "no such ELF section";
        case DPU_ERR_VPD_INVALID_FILE:
            return "invalid VPD file";
        default:
            return "unknown error";
    }
}
