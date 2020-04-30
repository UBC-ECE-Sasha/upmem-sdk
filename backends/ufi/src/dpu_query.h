/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_QUERY_H
#define UFI_DPU_QUERY_H

#include "dpu_types.h"
#include "dpu_ufi_types.h"
#include "list.h"

#include "dpu_query_type.h"
#include "dpu_operation.h"

typedef struct _dpu_query_target_t {
    dpu_slice_id_t slice_id;
    struct _dpu_slice_target_t slice_target;
} * dpu_query_target_t;

typedef void *dpu_query_parameters_t;
typedef void *dpu_query_result_t;

typedef struct _dpu_query_t {
    struct list_head list;
    dpu_query_type_e type;
    struct _dpu_query_target_t target;
    dpu_query_parameters_t parameters;
    dpu_query_result_t result;
    uint32_t _index;
} * dpu_query_t;

void
dpu_query_free(dpu_query_t query);
void
dpu_query_dump(FILE *output, uint32_t indent, dpu_query_t query);

/**
 * dpu_query_fill_operations_list - Fill the list with query's operation
 * @query: The query to turn into a list of operations
 * @list: The list to fill with the query's operations*
 *
 * Returns 0 in case of success, positive value otherwise
 */
int
dpu_query_fill_operations_list(dpu_query_t query, struct list_head *list);

void
dpu_query_update_result(dpu_query_t query, dpu_operation_t operation, uint8_t result_8, uint32_t result_32, uint64_t result_64);

#endif // UFI_DPU_QUERY_H
