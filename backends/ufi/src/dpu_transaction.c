/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include "dpu_attributes.h"
#include "dpu_transaction.h"

/**
 * dpu_transaction_new - Create a new transaction
 *
 * Instantiate a new transaction and init the list
 * of queries composing the transaction.
 * Note: dpu_transaction_delete must be called on
 * the returned object.
 * Returns the new allocated dpu_transaction_t or
 * NULL on error.
 */
__API_SYMBOL__ dpu_transaction_t
dpu_transaction_new(uint8_t rank_nr_of_ci)
{
    dpu_transaction_t new_transaction;
    uint8_t slice;

    new_transaction = malloc(sizeof(*new_transaction));
    if (!new_transaction)
        return NULL;

    new_transaction->list = malloc(rank_nr_of_ci * sizeof(struct list_head));
    if (!new_transaction->list)
        goto err_transaction;

    for (slice = 0; slice < rank_nr_of_ci; ++slice)
        INIT_LIST_HEAD(&new_transaction->list[slice]);

    new_transaction->rank_nr_of_ci = rank_nr_of_ci;

    return new_transaction;

err_transaction:
    free(new_transaction);

    return NULL;
}

/**
 * dpu_transaction_free_queries - Delete all queries and free them from the transaction
 *
 */
__API_SYMBOL__ void
dpu_transaction_free_queries_for_slice(dpu_transaction_t transaction, dpu_slice_id_t slice)
{
    dpu_query_t query, tmp;

    list_for_each_entry_safe(query, tmp, &transaction->list[slice], list) { dpu_query_free(query); }
}

/**
 * dpu_transaction_free_queries - Delete all queries of all slices and free them from the transaction
 *
 */
__API_SYMBOL__ void
dpu_transaction_clean(dpu_transaction_t transaction)
{
    for (dpu_slice_id_t each_slice = 0; each_slice < transaction->rank_nr_of_ci; ++each_slice) {
        dpu_transaction_free_queries_for_slice(transaction, each_slice);
    }
}

/**
 * dpu_transaction_free - Free a transaction
 *
 */
__API_SYMBOL__ void
dpu_transaction_free(dpu_transaction_t transaction)
{
    uint8_t rank_nr_of_ci = transaction->rank_nr_of_ci;

    for (dpu_slice_id_t each_slice = 0; each_slice < rank_nr_of_ci; ++each_slice) {
        dpu_transaction_free_queries_for_slice(transaction, each_slice);
    }

    free(transaction->list);
    free(transaction);
}

/**
 * dpu_transaction_add_query_head - Add a query to the transaction
 *
 * Note: Add the query to the head of the list
 */
__API_SYMBOL__ void
dpu_transaction_add_query_head(dpu_transaction_t transaction, dpu_query_t query)
{
    list_add(&query->list, &transaction->list[query->target.slice_id]);
}

/**
 * dpu_transaction_add_query_tail - Add a query to the transaction
 *
 * Note: Add the query to the tail of the list
 */
__API_SYMBOL__ void
dpu_transaction_add_query_tail(dpu_transaction_t transaction, dpu_query_t query)
{
    list_add_tail(&query->list, &transaction->list[query->target.slice_id]);
}

/**
 * dpu_transaction_remove_query - Delete a query from the transaction
 *
 */
__API_SYMBOL__ void
dpu_transaction_remove_query(dpu_query_t query)
{
    list_del(&query->list);
}

/**
 * dpu_transaction_dump - Dump the queries of the transaction
 *
 */
__API_SYMBOL__ void
dpu_transaction_dump(FILE *output, uint32_t indent, dpu_transaction_t transaction)
{
    dpu_query_t query;
    uint8_t slice;

    printf("transaction @%p {\n", transaction);

    for (slice = 0; slice < transaction->rank_nr_of_ci; ++slice) {
        printf("slice %u\n", slice);
        list_for_each_entry(query, &transaction->list[slice], list) { dpu_query_dump(output, indent + 4, query); }
    }

    printf("}\n");
}