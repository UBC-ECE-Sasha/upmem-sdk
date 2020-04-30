/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_UFI_H
#define DPU_UFI_H

#include <stdio.h>
#include <stdint.h>

#include "dpu_query_constructors.h"

#include "dpu_rank.h"
#include "dpu_rank_handler.h"

typedef void *dpu_transaction_t;

dpu_planner_status_e
dpu_planner_execute_transaction(dpu_transaction_t transaction, dpu_rank_handler_t rank_handler, struct dpu_rank_t *rank);
dpu_planner_status_e
dpu_planner_execute_transfer(dpu_rank_handler_t rank_handler,
    struct dpu_rank_t *rank,
    dpu_transfer_type_e transfer_type,
    struct dpu_transfer_mram *transfer_matrix);

dpu_transaction_t
dpu_transaction_new(uint8_t rank_nr_of_ci);
void
dpu_transaction_free(dpu_transaction_t transaction);

void
dpu_transaction_add_query_head(dpu_transaction_t transaction, dpu_query_t query);
void
dpu_transaction_add_query_tail(dpu_transaction_t transaction, dpu_query_t query);
void
dpu_transaction_remove_query(dpu_query_t query);
void
dpu_transaction_free_queries_for_slice(dpu_transaction_t transaction, dpu_slice_id_t slice);
void
dpu_transaction_clean(dpu_transaction_t transaction);

void
dpu_transaction_dump(FILE *output, uint32_t indent, dpu_transaction_t transaction);

#endif // DPU_UFI_H
