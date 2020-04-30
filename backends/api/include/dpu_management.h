/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_MANAGEMENT_H
#define DPU_MANAGEMENT_H

#include <dpu_error.h>
#include <dpu_description.h>
#include <dpu_types.h>
#include <dpu_macro_utils.h>

/**
 * @file dpu_management.h
 * @brief C API to allocate, free and access DPU ranks & DPUs.
 */

/**
 * @fn dpu_get_profile_description
 * @brief Fetch the description of the given profile.
 * @param profile the profile precising configuration
 * @param description storage for the description
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_profile_description(const char *profile, dpu_description_t *description);

/**
 * @fn dpu_get_rank_of_type
 * @brief Allocates a rank of DPUs.
 * @param profile the profile precising configuration for the requested DPUs
 * @param rank storage for the newly allocated rank of DPUs
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_rank_of_type(const char *profile, struct dpu_rank_t **rank);

/**
 * @fn dpu_free_rank
 * @brief Free all the DPUs of a rank.
 *
 * Note that this function waits for all DPUs booted in asynchronous mode.
 *
 * @param rank the unique identifier of the freed rank
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_free_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_lock_rank
 * @brief Takes the lock on the rank.
 * @param rank the unique identifier of the rank
 */
void
dpu_lock_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_unlock_rank
 * @brief Releases the lock on the rank.
 * @param rank the unique identifier of the rank
 */
void
dpu_unlock_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_get
 * @brief Fetches the pointer on the specified DPU structure.
 * @param rank the unique identifier of the rank
 * @param slice_id the slice id of the wanted DPU
 * @param dpu_id the member id of the wanted DPU
 * @return ``NULL`` if the specified DPU is invalid, the DPU structure pointer otherwise.
 */
struct dpu_t *
dpu_get(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

/**
 * @fn dpu_get_id
 * @brief Fetches the unique ID of the specified DPU
 * @param dpu the unique identifier of the DPU
 * @return The unique ID of the DPU.
 */
dpu_id_t
dpu_get_id(struct dpu_t *dpu);

/**
 * @fn dpu_get_rank
 * @brief Fetches the pointer on the rank structure of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @return The rank structure pointer.
 */
struct dpu_rank_t *
dpu_get_rank(struct dpu_t *dpu);

/**
 * @fn dpu_get_slice_id
 * @brief Fetches the slice id of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @return The slice id of the DPU.
 */
dpu_slice_id_t
dpu_get_slice_id(struct dpu_t *dpu);

/**
 * @fn dpu_get_member_id
 * @brief Fetches the member id of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @return The member id of the DPU.
 */
dpu_member_id_t
dpu_get_member_id(struct dpu_t *dpu);

/**
 * @fn dpu_is_enabled
 * @brief Fetch the access state of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @return Whether the DPU is enabled.
 */
bool
dpu_is_enabled(struct dpu_t *dpu);

/**
 * @fn dpu_get_description
 * @brief Fetches the rank description.
 * @param rank the unique identifier of the rank
 * @return The pointer on the rank description.
 */
dpu_description_t
dpu_get_description(struct dpu_rank_t *rank);

/**
 * @fn dpu_set_from_rank
 * @brief Creates a DPU set from a single DPU rank
 * @param rank the rank in the set
 * @return The DPU set.
 */
struct dpu_set_t
dpu_set_from_rank(struct dpu_rank_t **rank);

/**
 * @fn dpu_set_from_dpu
 * @brief Creates a DPU set from a single DPU
 * @param dpu the DPU in the set
 * @return The DPU set.
 */
struct dpu_set_t
dpu_set_from_dpu(struct dpu_t *dpu);

/**
 * @fn dpu_rank_from_set
 * @brief Get the rank structure from a set
 * @param set the set from which to extract the rank
 * @return the rank structure or NULL if the set is not a single rank
 */
struct dpu_rank_t *
dpu_rank_from_set(struct dpu_set_t set);

/**
 * @fn dpu_from_set
 * @brief Get the dpu structure from a set
 * @param set the set from which to extract the dpu
 * @return the dpu structure or NULL if the set is not a single dpu
 */
struct dpu_t *
dpu_from_set(struct dpu_set_t set);

/**
 * @def STRUCT_DPU_FOREACH
 * @brief Iterator over all DPUs of a rank structure.
 * @param rank the targeted DPU rank
 * @param dpu a pointer to a dpu structure, which will store the dpu context for the current iteration
 */
#define STRUCT_DPU_FOREACH(rank, dpu, ...)                                                                                       \
    _CONCAT(_STRUCT_DPU_FOREACH_, _DPU_FOREACH_VARIANT(rank, dpu, ##__VA_ARGS__))(rank, dpu, ##__VA_ARGS__)

#define _STRUCT_DPU_FOREACH_X(rank, dpu)                                                                                         \
    for (struct dpu_iterator_t __dpu_it = dpu_iterator_from(rank); dpu = __dpu_it.next, __dpu_it.has_next;                       \
         dpu_iterator_next(&__dpu_it))

#define _STRUCT_DPU_FOREACH_I(rank, dpu, i)                                                                                      \
    for (struct dpu_iterator_t __dpu_it = dpu_iterator_from(rank); i = __dpu_it.count, dpu = __dpu_it.next, __dpu_it.has_next;   \
         dpu_iterator_next(&__dpu_it))

struct dpu_iterator_t {
    struct dpu_rank_t *rank;
    uint32_t count;
    uint32_t next_idx;
    bool has_next;
    struct dpu_t *next;
};

struct dpu_iterator_t
dpu_iterator_from(struct dpu_rank_t *rank);

void
dpu_iterator_next(struct dpu_iterator_t *iterator);

#endif // DPU_MANAGEMENT_H
