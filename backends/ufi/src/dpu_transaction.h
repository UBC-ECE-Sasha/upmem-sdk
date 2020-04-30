/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_TRANSACTION_H
#define UFI_DPU_TRANSACTION_H

#include "dpu_query.h"

#include "list.h"

/* dpu_transaction_t contains an array (of size rank_nr_of_ci, ie number of slices)
 * of lists of queries:each list comprises queries that are in the same slice.
 */
typedef struct {
    struct list_head *list;
    uint8_t rank_nr_of_ci; // ie: number of slices in the rank
} * dpu_transaction_t;

#endif // UFI_DPU_TRANSACTION_H
