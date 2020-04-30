/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_CONFIG_H
#define DPU_CONFIG_H

#include <stdint.h>

#include <dpu_types.h>
#include <dpu_error.h>
#include <dpu_pcb_transformation.h>

/**
 * @file dpu_config.h
 * @brief C API to configure DPUs.
 */

typedef struct _dpu_carousel_configuration_t {
    uint8_t cmd_duration;
    uint8_t cmd_sampling;
    uint8_t res_duration;
    uint8_t res_sampling;
} * dpu_carousel_configuration_t;

typedef struct _dpu_dma_engine_configuration_t {
    uint8_t refresh_access_number;
    uint8_t column_read_latency;
    uint8_t minimal_access_number;
    uint8_t default_time_origin;
    uint8_t ldma_to_sdma_time_origin;
    uint8_t xdma_time_start_activate;
    uint8_t xdma_time_start_access;
    uint8_t sdma_time_start_wb_f1;
} * dpu_dma_engine_configuration_t;

struct dpu_waveform_generator_register_configuration_t {
    uint8_t rise;
    uint8_t fall;
    uint8_t counter_enable;
    uint8_t counter_disable;
};

struct dpu_waveform_generator_timing_configuration_t {
    uint8_t activate;
    uint8_t read;
    uint8_t write;
    uint8_t precharge;
    uint8_t refresh_start;
    uint8_t refresh_activ;
    uint8_t refresh_prech;
    uint8_t refresh_end;
};

struct dpu_waveform_generator_vector_sampling_configuration_t {
    uint8_t rab_gross;
    uint8_t cat_gross;
    uint8_t dwbsb_gross;
    uint8_t drbsb_gross;
    uint8_t drbsb_fine;
};

typedef struct _dpu_waveform_generator_configuration_t {
    struct dpu_waveform_generator_register_configuration_t MCBAB;
    struct dpu_waveform_generator_register_configuration_t RCYCLKB;
    struct dpu_waveform_generator_register_configuration_t WCYCLKB;
    struct dpu_waveform_generator_register_configuration_t DWCLKB;
    struct dpu_waveform_generator_register_configuration_t DWAEB;
    struct dpu_waveform_generator_register_configuration_t DRAPB;
    struct dpu_waveform_generator_register_configuration_t DRAOB;
    struct dpu_waveform_generator_register_configuration_t DWBSB_EN;
    struct dpu_waveform_generator_timing_configuration_t timing_completion;
    struct dpu_waveform_generator_vector_sampling_configuration_t vector_sampling;
    uint16_t refresh_and_row_hammer_info; // {...[8], 4'h0 rah_auto, ref_mode[1:0]} pmcrft counter[7:0]
    uint8_t row_hammer_config;
} * dpu_waveform_generator_configuration_t;

extern const struct _dpu_dma_engine_configuration_t dma_engine_clock_div2_config;
extern const struct _dpu_dma_engine_configuration_t dma_engine_clock_div3_config;
extern const struct _dpu_dma_engine_configuration_t dma_engine_clock_div4_config;
extern const struct _dpu_dma_engine_configuration_t dma_engine_cas_config;
extern const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div2_config;
extern const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div3_config;
extern const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div4_config;

dpu_error_t
dpu_reset_rank(struct dpu_rank_t *rank);
dpu_error_t
dpu_soft_reset_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_configure_carousel(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_carousel_configuration_t *config);
dpu_error_t
dpu_configure_ci_shuffling_box(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_pcb_transformation_t *config);
dpu_error_t
dpu_configure_dma_engine(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_dma_engine_configuration_t *config);
dpu_error_t
dpu_configure_dma_shuffling_box(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_pcb_transformation_t *config);
dpu_error_t
dpu_configure_waveform_generator(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    const struct _dpu_waveform_generator_configuration_t *config);
dpu_error_t
dpu_waveform_generator_read_status(struct dpu_t *dpu, uint8_t read_addr, uint8_t *read_value);
dpu_error_t
dpu_configure_iram_repair_for_previous(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    uint8_t AB_msbs,
    uint8_t A_lsbs,
    uint8_t B_lsbs,
    uint8_t CD_msbs,
    uint8_t C_lsbs,
    uint8_t D_lsbs,
    uint8_t even_index,
    uint8_t odd_index);
dpu_error_t
dpu_configure_wram_repair_for_previous(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    uint8_t wram_bank_index,
    uint8_t AB_msbs,
    uint8_t A_lsbs,
    uint8_t B_lsbs,
    uint8_t CD_msbs,
    uint8_t C_lsbs,
    uint8_t D_lsbs,
    uint8_t even_index,
    uint8_t odd_index);

dpu_error_t
dpu_host_release_access_for_dpu(struct dpu_t *dpu);
dpu_error_t
dpu_host_get_access_for_dpu(struct dpu_t *dpu);

#endif // DPU_CONFIG_H
