/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_PROFILER_H
#define DPU_PROFILER_H

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_profiler.h
 * @brief C API for DPU profiling operations.
 */

enum dpu_profiling_type_e {
    DPU_PROFILING_NOP,
    DPU_PROFILING_MCOUNT,
    DPU_PROFILING_STATS,
    DPU_PROFILING_SAMPLES,
};

typedef struct _dpu_profiling_context_t {
    enum dpu_profiling_type_e enable_profiling;
    struct dpu_t *dpu;

    /* In sample mode, pcs are randomly sampled without knowing the current thread */
    uint64_t *sample_stats;

    // In stat mode:
    // - we patched text at "address" with sw "idx_value", @wram (idx_value is the index in this array)
    //   and when reading at @wram from host, we increment count of idx_value
    // - we can profile only one DPU (for the moment)
    uint16_t nr_of_mcount_stats;

    struct {
        iram_addr_t address;
        uint64_t count;
    } * *mcount_stats;

    iram_addr_t mcount_address;
    iram_addr_t ret_mcount_address;
    wram_addr_t thread_profiling_address;
} * dpu_profiling_context_t;

dpu_error_t
dpu_fill_profiling_info(struct dpu_rank_t *rank,
    iram_addr_t mcount_address,
    iram_addr_t ret_mcount_address,
    wram_addr_t thread_profiling_address);
dpu_error_t
dpu_patch_profiling_for_dpu(struct dpu_t *dpu, dpuinstruction_t *source, uint32_t address, uint32_t size, bool init);
void
dpu_collect_statistics_profiling(struct dpu_t *dpu, uint8_t nr_threads, const uint32_t *profiled_address);
void
dpu_collect_samples_profiling(struct dpu_t *dpu, iram_addr_t profiled_address);
void
dpu_dump_statistics_profiling(struct dpu_t *dpu, uint8_t nr_threads);
void
dpu_dump_samples_profiling(struct dpu_t *dpu);
dpu_error_t
dpu_set_magic_profiling_for_dpu(struct dpu_t *dpu);

/**
 * @fn dpu_get_profiling_context
 * @brief Fetches the profiling context of the specified rank.
 * @param rank the unique identifier of the rank
 * @return The pointer on the profiling context of the rank.
 */
dpu_profiling_context_t
dpu_get_profiling_context(struct dpu_rank_t *rank);

#endif // DPU_PROFILER_H
