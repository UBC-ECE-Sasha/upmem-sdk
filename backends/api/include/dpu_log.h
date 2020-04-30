/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_LOG_H
#define DPU_LOG_H

#include <stdio.h>
#include <stdbool.h>

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_log.h
 * @brief C API to read and display log produced by DPUs.
 */

dpu_error_t
dpu_log_read(struct dpu_set_t set, FILE *stream);

/**
 * @fn dpulog_read_for_dpu
 * @brief reads and displays the contents of the log of a DPU
 * @param dpu the DPU producing the log (MUST NOT BE FREED)
 * @param stream output stream where messages should be sent
 * @return whether the log was successfully read
 */
dpu_error_t
dpulog_read_for_dpu(struct dpu_t *dpu, FILE *stream);

dpu_error_t
dpulog_read_and_display_contents_of(void *input, size_t input_size, FILE *output);

#endif /* DPU_LOG_H */
