/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_OPERATION_H
#define UFI_DPU_OPERATION_H

#include <stdint.h>

#include "list.h"
#include "dpu_operation_type.h"

typedef struct _dpu_operation_t {
    struct list_head list;
    dpu_operation_type_e type;
} * dpu_operation_t;

/* This structure is a 'container' for an array of list of operations,
 * in the same way as dpu_transaction_t.
 */
typedef struct {
    struct list_head *list;
    uint8_t rank_nr_of_ci; // ie: number of slices in the rank
} * dpu_operation_step_t;

dpu_operation_t
dpu_operation_new(dpu_operation_type_e type);
void
dpu_operation_free(dpu_operation_t operation);

dpu_operation_step_t
dpu_operation_step_new(uint8_t rank_nr_of_ci);
void
dpu_operation_step_free(dpu_operation_step_t operation_step);

void
dpu_operation_dump(FILE *output, uint32_t indent, dpu_operation_t operation);

#endif // UFI_DPU_OPERATION_H
