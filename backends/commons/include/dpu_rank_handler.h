/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_RANK_HANDLER_H
#define DPU_RANK_HANDLER_H

#include <stdint.h>
#include <dpu_transfer_mram.h>
#include <dpu_target.h>
#include <dpu_custom.h>
#include <dpu_properties.h>

typedef enum _dpu_rank_status_e {
    DPU_RANK_SUCCESS = 0,
    DPU_RANK_COMMUNICATION_ERROR,
    DPU_RANK_BACKEND_ERROR,
    DPU_RANK_SYSTEM_ERROR,
    DPU_RANK_INVALID_PROPERTY_ERROR,
    DPU_RANK_ENODEV,
} dpu_rank_status_e;

typedef uint64_t *dpu_rank_buffer_t;

typedef struct _dpu_rank_handler_t {
    dpu_rank_status_e (*allocate)(struct dpu_rank_t *rank, dpu_description_t description);
    dpu_rank_status_e (*free)(struct dpu_rank_t *rank);

    dpu_rank_status_e (*commit_commands)(struct dpu_rank_t *rank, dpu_rank_buffer_t *buffer);
    dpu_rank_status_e (*update_commands)(struct dpu_rank_t *rank, dpu_rank_buffer_t *buffer);

    dpu_rank_status_e (*copy_to_rank)(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);
    dpu_rank_status_e (*copy_from_rank)(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);

    void (*print_lldb_message_on_fault)(struct dpu_t *dpu, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

    dpu_rank_status_e (*custom_operation)(struct dpu_rank_t *rank,
        dpu_slice_id_t slice_id,
        dpu_member_id_t member_id,
        dpu_custom_command_t command,
        dpu_custom_command_args_t args);
    dpu_rank_status_e (*fill_description_from_profile)(dpu_properties_t properties, dpu_description_t description);
} * dpu_rank_handler_t;

/* We need to keep a global handler for further rank allocation: the first 'allocates' it, the others get it. */
typedef struct _dpu_rank_handler_context_t {
    dpu_rank_handler_t handler;
    int handler_refcount;
    void *library;
} * dpu_rank_handler_context_t;

bool
dpu_rank_handler_instantiate(dpu_type_t type, dpu_rank_handler_context_t *ret_handler_context, bool verbose);
void
dpu_rank_handler_release(dpu_rank_handler_context_t handler_context);

bool
dpu_rank_handler_get_rank(struct dpu_rank_t *rank, dpu_rank_handler_context_t handler_context, dpu_properties_t properties);
void
dpu_rank_handler_free_rank(struct dpu_rank_t *rank, dpu_rank_handler_context_t handler_context);

static inline void
print_lldb_message_on_fault_do_nothing(__attribute__((unused)) struct dpu_t *dpu,
    __attribute__((unused)) dpu_slice_id_t slice_id,
    __attribute__((unused)) dpu_member_id_t dpu_id)
{
    return;
}

#endif // DPU_RANK_HANDLER_H
