/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>

#include "list.h"
#include "dpu_operation.h"

/**
 * dpu_operation_new - Allocate a new operation
 * @type: Type of the operation
 *
 * Returns NULL in case of error, the new operation
 * allocated otherwise.
 */
dpu_operation_t
dpu_operation_new(dpu_operation_type_e type)
{
    dpu_operation_t operation;

    if ((operation = malloc(sizeof(*operation))) == NULL)
        return NULL;

    operation->type = type;

    return operation;
}

/**
 * dpu_operation_free - Free the operation
 * @operation: The operation to free
 *
 * This function frees the operation. Moreover, it takes care of removing
 * the operation for the list it is in.
 */
void
dpu_operation_free(dpu_operation_t operation)
{
    list_del(&operation->list);
    free(operation);
}

/**
 * dpu_operation_step_new - Allocate new operation step structure
 *
 * Returns NULL in case of error, the new operation otherwise
 */
dpu_operation_step_t
dpu_operation_step_new(uint8_t rank_nr_of_ci)
{
    dpu_operation_step_t operation_step;

    operation_step = malloc(sizeof(*operation_step));
    if (!operation_step)
        return NULL;

    operation_step->rank_nr_of_ci = rank_nr_of_ci;

    operation_step->list = malloc(rank_nr_of_ci * sizeof(struct list_head));
    if (!operation_step->list)
        goto err_operation_step;

    for (int each_slice = 0; each_slice < rank_nr_of_ci; ++each_slice) {
        INIT_LIST_HEAD(&operation_step->list[each_slice]);
    }

    return operation_step;

err_operation_step:
    free(operation_step);

    return NULL;
}

/**
 * dpu_operation_step_free - Free the operation step structure
 *
 * Note: Also frees operations in the list !
 */
void
dpu_operation_step_free(dpu_operation_step_t operation_step)
{
    unsigned int slice;

    for (slice = 0; slice < operation_step->rank_nr_of_ci; ++slice) {
        dpu_operation_t operation, tmp;

        list_for_each_entry_safe(operation, tmp, &operation_step->list[slice], list) { dpu_operation_free(operation); }
    }

    free(operation_step->list);
    free(operation_step);
}

/**
 * dpu_operation_dump - Print human readable operation
 * @operation: The operation to dump
 *
 */
void
dpu_operation_dump(FILE *output, uint32_t indent, dpu_operation_t operation)
{
    fprintf(output, "%*soperation @%p {\n", indent, "", operation);

    fprintf(output, "%*stype = %s (%d)\n", indent + 4, "", DPU_OPERATION_TYPE_NAME(operation->type), operation->type);

    fprintf(output, "%*s}\n", indent, "");
}