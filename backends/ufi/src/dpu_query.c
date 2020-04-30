/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dpu_query.h"
#include "dpu_attributes.h"

extern void
dpu_query_target_dump(FILE *output, uint32_t indent, dpu_query_target_t target);
extern void
dpu_query_parameters_dump(FILE *output, uint32_t indent, dpu_query_parameters_t parameters, dpu_query_type_e type);
extern void
dpu_query_result_dump(FILE *output, uint32_t indent, dpu_query_result_t result, dpu_query_type_e type);

/**
 * dpu_query_free - Free the query
 * @query: The query to free
 *
 * This function frees EXACTLY what is allocated in
 * dpu_query_new. Moreover, it takes care of removing
 * the operation for the list it is in.
 */
__API_SYMBOL__ void
dpu_query_free(dpu_query_t query)
{
    /* Do not forget to delete the query from the list it is in ! */
    list_del(&query->list);
    if (query->parameters)
        free(query->parameters);
    free(query);
}

/**
 * dpu_query_dump - Print human readable query
 * @query: The query to dump
 *
 */
void
dpu_query_dump(FILE *output, uint32_t indent, dpu_query_t query)
{
    fprintf(output, "%*squery @%p {\n", indent, "", query);

    fprintf(output, "%*stype = %s (%d)\n", indent + 4, "", DPU_QUERY_TYPE_NAME(query->type), query->type);

    dpu_query_target_dump(output, indent + 4, &query->target);
    dpu_query_parameters_dump(output, indent + 4, query->parameters, query->type);
    dpu_query_result_dump(output, indent + 4, query->result, query->type);

    fprintf(output, "%*s}\n", indent, "");
}
