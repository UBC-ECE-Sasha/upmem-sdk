/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_PCB_TRANSFORMATION_H
#define DPU_PCB_TRANSFORMATION_H

#include <stdint.h>

/**
 * @file dpu_pcb_transformation.h
 * @brief C API for DPU PCB transformation.
 */

typedef struct _dpu_pcb_transformation_t {
    // TODO byte_order in which form ?
    uint16_t cpu_to_dpu;
    uint16_t dpu_to_cpu;
    uint8_t nibble_swap;
} dpu_pcb_transformation_t;

void
dpu_pcb_transformation_fill(uint32_t bit_order_result, dpu_pcb_transformation_t *pcb_transformation);

uint8_t
dpu_pcb_transformation_get_reciprocal(uint8_t code);
uint32_t
dpu_pcb_transformation_dpu_to_cpu(dpu_pcb_transformation_t *pcb_transformation, uint32_t dpu_value);
uint32_t
dpu_pcb_transformation_cpu_to_dpu(dpu_pcb_transformation_t *pcb_transformation, uint32_t cpu_value);

#endif // DPU_PCB_TRANSFORMATION_H
