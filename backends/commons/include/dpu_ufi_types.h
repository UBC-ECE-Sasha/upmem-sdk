/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_UFI_TYPES_H
#define DPU_UFI_TYPES_H

#include <stdint.h>

typedef uint32_t dpu_selected_mask_t;

// todo
typedef uint8_t dpu_dma_ctrl_t;

typedef enum _dpu_transfer_type_e {
    DPU_TRANSFER_FROM_MRAM,
    DPU_TRANSFER_TO_MRAM,
} dpu_transfer_type_e;

// todo
typedef enum _dpu_planner_status_e {
    DPU_PLANNER_SUCCESS = 0,
    DPU_PLANNER_INVALID_NR_OF_CI_ERROR,
    DPU_PLANNER_INVALID_TRANSFER_TYPE_ERROR,
    DPU_PLANNER_RANK_ERROR,
    DPU_PLANNER_SYSTEM_ERROR,
    DPU_PLANNER_TIMEOUT_WAITING_FOR_VALID_RESULTS,
} dpu_planner_status_e;

#endif // DPU_UFI_TYPES_H
