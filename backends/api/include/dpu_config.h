/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_CONFIG_H
#define DPU_CONFIG_H

#include <stdint.h>

#include <dpu_types.h>
#include <dpu_error.h>

/**
 * @file dpu_config.h
 * @brief C API to configure DPUs.
 */

struct dpu_dma_config {
    uint8_t refresh_access_number;
    uint8_t column_read_latency;
    uint8_t minimal_access_number;
    uint8_t default_time_origin;
    uint8_t ldma_to_sdma_time_origin;
    uint8_t xdma_time_start_activate;
    uint8_t xdma_time_start_access;
    uint8_t sdma_time_start_wb_f1;
};

struct dpu_wavegen_reg_config {
    uint8_t rise;
    uint8_t fall;
    uint8_t counter_enable;
    uint8_t counter_disable;
};

struct dpu_wavegen_timing_config {
    uint8_t activate;
    uint8_t read;
    uint8_t write;
    uint8_t precharge;
    uint8_t refresh_start;
    uint8_t refresh_activ;
    uint8_t refresh_prech;
    uint8_t refresh_end;
};

struct dpu_wavegen_vector_sampling_config {
    uint8_t rab_gross;
    uint8_t cat_gross;
    uint8_t dwbsb_gross;
    uint8_t drbsb_gross;
    uint8_t drbsb_fine;
};

struct dpu_wavegen_config {
    struct dpu_wavegen_reg_config MCBAB;
    struct dpu_wavegen_reg_config RCYCLKB;
    struct dpu_wavegen_reg_config WCYCLKB;
    struct dpu_wavegen_reg_config DWCLKB;
    struct dpu_wavegen_reg_config DWAEB;
    struct dpu_wavegen_reg_config DRAPB;
    struct dpu_wavegen_reg_config DRAOB;
    struct dpu_wavegen_reg_config DWBSB_EN;
    struct dpu_wavegen_timing_config timing_completion;
    struct dpu_wavegen_vector_sampling_config vector_sampling;
    uint16_t refresh_and_row_hammer_info; // {...[8], 4'h0 rah_auto, ref_mode[1:0]} pmcrft counter[7:0]
    uint8_t row_hammer_config;
};

extern const struct dpu_dma_config dma_engine_clock_div2_config;
extern const struct dpu_dma_config dma_engine_clock_div3_config;
extern const struct dpu_dma_config dma_engine_clock_div4_config;
extern const struct dpu_dma_config dma_engine_cas_config;
extern const struct dpu_wavegen_config waveform_generator_clock_div2_config;
extern const struct dpu_wavegen_config waveform_generator_clock_div3_config;
extern const struct dpu_wavegen_config waveform_generator_clock_div4_config;

dpu_error_t
dpu_reset_rank(struct dpu_rank_t *rank);
dpu_error_t
dpu_reset_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_dma_config(struct dpu_rank_t *rank, const struct dpu_dma_config *config);
dpu_error_t
dpu_dma_shuffling_box_config(struct dpu_rank_t *rank, const struct dpu_bit_config *config);
dpu_error_t
dpu_wavegen_config(struct dpu_rank_t *rank, const struct dpu_wavegen_config *config);
dpu_error_t
dpu_wavegen_read_status(struct dpu_t *dpu, uint8_t address, uint8_t *value);

dpu_error_t
dpu_host_release_access_for_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_host_get_access_for_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_host_release_access_for_rank(struct dpu_rank_t *rank);
dpu_error_t
dpu_host_get_access_for_rank(struct dpu_rank_t *rank);

#endif // DPU_CONFIG_H
