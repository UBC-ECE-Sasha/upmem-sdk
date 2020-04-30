/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_TRANSFER_MATRIX_H
#define DPU_TRANSFER_MATRIX_H

#include <stdbool.h>
#include <stdint.h>

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_transfer_mrams.h
 * @brief C API for DPU transfer matrix operations.
 */

/**
 * @struct dpu_transfer_mram
 * @brief Context of a DPU MRAM transfer.
 */
struct dpu_transfer_mram;

dpu_error_t
dpu_transfer_matrix_allocate(struct dpu_rank_t *rank, struct dpu_transfer_mram **transfer_matrix);
void
dpu_transfer_matrix_free(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);
dpu_error_t
dpu_transfer_matrix_add_dpu(struct dpu_t *dpu,
    struct dpu_transfer_mram *transfer_matrix,
    void *buffer,
    mram_size_t size,
    mram_addr_t offset_in_mram,
    uint32_t mram_number);
void
dpu_transfer_matrix_set_all(struct dpu_rank_t *rank,
    struct dpu_transfer_mram *transfer_matrix,
    void *buffer,
    mram_size_t size,
    mram_addr_t offset_in_mram,
    uint32_t mram_number);
dpu_error_t
dpu_transfer_matrix_clear_dpu(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix);
void
dpu_transfer_matrix_clear_all(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);

void *
dpu_transfer_matrix_get_ptr(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix);
uint32_t
dpu_transfer_matrix_get_size(struct dpu_t *dpu, struct dpu_transfer_mram *transfer_matrix);

#endif // DPU_TRANSFER_MATRIX_H
