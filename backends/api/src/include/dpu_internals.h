/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_INTERNALS_H
#define DPU_INTERNALS_H

#include <stdbool.h>
#include <stdint.h>

#include "dpu_error.h"
#include "dpu_types.h"
#include "dpu_debug.h"

#include "dpu_rank.h"

#define verify_thread_id(t, r)                                                                                                   \
    do {                                                                                                                         \
        if ((t) >= (r)->description->dpu.nr_of_threads) {                                                                        \
            LOG_RANK(WARNING, r, "ERROR: invalid thread id (%d >= %d)", t, (r)->description->dpu.nr_of_threads);                 \
            return DPU_ERR_INVALID_THREAD_ID;                                                                                    \
        }                                                                                                                        \
    } while (0)

#define verify_notify_id(t, r)                                                                                                   \
    do {                                                                                                                         \
        if ((t) >= (r)->description->dpu.nr_of_notify_bits) {                                                                    \
            LOG_RANK(WARNING, r, "ERROR: invalid notify id (%d >= %d)", t, (r)->description->dpu.nr_of_notify_bits);             \
            return DPU_ERR_INVALID_NOTIFY_ID;                                                                                    \
        }                                                                                                                        \
    } while (0)

#define verify_wram_access(o, s, r)                                                                                              \
    do {                                                                                                                         \
        if (!(s)) {                                                                                                              \
            LOG_RANK(WARNING, r, "WARNING: wram access of size 0 at offset %d", o);                                              \
            return DPU_OK;                                                                                                       \
        }                                                                                                                        \
        if (((o) >= (r)->description->memories.wram_size) || (((o) + (s)) > (r)->description->memories.wram_size)) {             \
            LOG_RANK(WARNING,                                                                                                    \
                r,                                                                                                               \
                "ERROR: invalid wram access ((%d >= %d) || (%d > %d))",                                                          \
                o,                                                                                                               \
                (r)->description->memories.wram_size,                                                                            \
                (o) + (s),                                                                                                       \
                (r)->description->memories.wram_size);                                                                           \
            return DPU_ERR_INVALID_WRAM_ACCESS;                                                                                  \
        }                                                                                                                        \
    } while (0)

#define verify_iram_access(o, s, r)                                                                                              \
    do {                                                                                                                         \
        if (!(s)) {                                                                                                              \
            LOG_RANK(WARNING, r, "WARNING: iram access of size 0 at offset %d", o);                                              \
            return DPU_OK;                                                                                                       \
        }                                                                                                                        \
        if (((o) >= (r)->description->memories.iram_size) || (((o) + (s)) > (r)->description->memories.iram_size)) {             \
            LOG_RANK(WARNING,                                                                                                    \
                r,                                                                                                               \
                "ERROR: invalid iram access ((%d >= %d) || (%d > %d))",                                                          \
                o,                                                                                                               \
                (r)->description->memories.iram_size,                                                                            \
                (o) + (s),                                                                                                       \
                (r)->description->memories.iram_size);                                                                           \
            return DPU_ERR_INVALID_IRAM_ACCESS;                                                                                  \
        }                                                                                                                        \
    } while (0)

#define verify_mram_access(o, s, r)                                                                                              \
    do {                                                                                                                         \
        if (!(s)) {                                                                                                              \
            LOG_RANK(WARNING, r, "WARNING: mram access of size 0 at offset %d", o);                                              \
            return DPU_OK;                                                                                                       \
        }                                                                                                                        \
        if (((o) >= (r)->description->memories.mram_size) || (((o) + (s)) > (r)->description->memories.mram_size)) {             \
            LOG_RANK(WARNING,                                                                                                    \
                r,                                                                                                               \
                "ERROR: invalid mram access ((%d >= %d) || (%d > %d))",                                                          \
                o,                                                                                                               \
                (r)->description->memories.mram_size,                                                                            \
                (o) + (s),                                                                                                       \
                (r)->description->memories.mram_size);                                                                           \
            return DPU_ERR_INVALID_MRAM_ACCESS;                                                                                  \
        }                                                                                                                        \
    } while (0)

dpu_error_t
drain_pipeline(struct dpu_t *dpu, dpu_context_t context, dpu_pc_mode_e pc_mode, bool should_add_to_schedule);

bool
fetch_natural_pc_mode(struct dpu_rank_t *rank, dpu_pc_mode_e *pc_mode);

void
set_pc_in_core_dump_or_restore_registers(dpu_thread_t thread,
    iram_addr_t pc,
    dpuinstruction_t *program,
    iram_size_t program_size,
    uint8_t nr_of_threads);

dpu_clock_division_t
from_division_factor_to_dpu_enum(uint8_t factor);

/**
 * @fn dpu_get_run_context
 * @brief Fetches the rank run context.
 * @param rank the unique identifier of the rank
 * @return The pointer on the rank run context.
 */
dpu_run_context_t
dpu_get_run_context(struct dpu_rank_t *rank);

#endif /* DPU_INTERNALS_H */
