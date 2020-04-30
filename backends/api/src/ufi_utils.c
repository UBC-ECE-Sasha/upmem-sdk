/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/ufi_utils.h"

dpu_error_t
map_planner_status_to_api_status(dpu_planner_status_e planner_status)
{
    switch (planner_status) {
        case DPU_PLANNER_SUCCESS:
            return DPU_OK;
        case DPU_PLANNER_INVALID_NR_OF_CI_ERROR:
            return DPU_ERR_INTERNAL;
        case DPU_PLANNER_INVALID_TRANSFER_TYPE_ERROR:
            return DPU_ERR_INTERNAL;
        case DPU_PLANNER_RANK_ERROR:
            return DPU_ERR_DRIVER;
        case DPU_PLANNER_SYSTEM_ERROR:
            return DPU_ERR_SYSTEM;
        case DPU_PLANNER_TIMEOUT_WAITING_FOR_VALID_RESULTS:
            return DPU_ERR_TIMEOUT;
        default:
            return DPU_ERR_INTERNAL;
    }
}

dpu_error_t
map_rank_status_to_api_status(dpu_rank_status_e rank_status)
{
    switch (rank_status) {
        case DPU_RANK_SUCCESS:
            return DPU_OK;
        case DPU_RANK_COMMUNICATION_ERROR:
            return DPU_ERR_DRIVER;
        case DPU_RANK_BACKEND_ERROR:
            return DPU_ERR_DRIVER;
        case DPU_RANK_SYSTEM_ERROR:
            return DPU_ERR_SYSTEM;
        case DPU_RANK_INVALID_PROPERTY_ERROR:
            return DPU_ERR_INVALID_PROFILE;
        default:
            return DPU_ERR_INTERNAL;
    }
}
