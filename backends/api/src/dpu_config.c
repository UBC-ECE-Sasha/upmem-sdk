/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <math.h>
#include <string.h>

#include <dpu_config.h>
#include <dpu_description.h>
#include <dpu_management.h>

#include <dpu_api_log.h>
#include <dpu_attributes.h>
#include <dpu_internals.h>
#include <dpu_mask.h>
#include <dpu_predef_programs.h>
#include <dpu_rank.h>
#include <ufi_utils.h>
#include <verbose_control.h>

static dpu_error_t
dpu_byte_order(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, uint64_t *byte_order_result);
static dpu_error_t
dpu_software_reset(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, bool param_is_null);
static dpu_error_t
dpu_bit_order(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, uint64_t byte_order_result);
static dpu_error_t
dpu_identity(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_thermal_config(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_set_group(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_check_for_iram_repair(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_check_for_wram_repair(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_clear_debug_replace(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_clear_pc_mode(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode);
static dpu_error_t
dpu_clear_faults_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_clear_faults_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
dpu_clear_thread_run_reg_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_clear_notify_bit_reg_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_clear_notify_bit_reg_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
dpu_set_stack_up(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_reset_internal_state(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id);
static dpu_error_t
dpu_reset_internal_state_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu);

static dpu_error_t
try_to_repair_iram(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
find_faulty_iram_bits_blocked_at(dpu_transaction_t transaction,
    struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    bool bit_is_set);
static dpu_error_t
try_to_repair_wram(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
find_faulty_wram_bits_blocked_at(dpu_transaction_t transaction,
    struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    bool bit_is_set);

static bool
byte_order_values_are_compatible(uint64_t reference, uint64_t found);
static dpu_temperature_e
from_celsius_to_dpu_enum(uint8_t temperature);

static dpu_error_t
host_get_access(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
wavegen_mux_status(dpu_transaction_t transaction,
    struct dpu_t *dpu,
    uint8_t expected,
    uint8_t cmd,
    dpu_dma_ctrl_t *ret_dpu_dma_ctrl);
static dpu_error_t
host_release_access(dpu_transaction_t transaction, struct dpu_t *dpu);
static dpu_error_t
dpu_check_wavegen_mux_status_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu, uint8_t expected);

static void
fetch_dma_and_wavegen_configs(uint32_t fck_frequency,
    uint8_t clock_division,
    uint8_t refresh_mode,
    struct _dpu_dma_engine_configuration_t *dma_config,
    struct _dpu_waveform_generator_configuration_t *wavegen_config);

#define DMA_CTRL_WRITE(slice, addr, data)                                                                                        \
    safe_add_query(query,                                                                                                        \
        dpu_query_build_write_dma_control_for_previous(slice,                                                                    \
            0x60 | (((addr) >> 4) & 0x0F),                                                                                       \
            0x60 | (((addr) >> 0) & 0x0F),                                                                                       \
            0x60 | (((data) >> 4) & 0x0F),                                                                                       \
            0x60 | (((data) >> 0) & 0x0F),                                                                                       \
            0x60,                                                                                                                \
            0x20),                                                                                                               \
        transaction,                                                                                                             \
        status,                                                                                                                  \
        err)

#define WAVEGEN_REG_CONFIG(slice, reg, addr)                                                                                     \
    do {                                                                                                                         \
        DMA_CTRL_WRITE(slice, addr + 0, (reg).rise);                                                                             \
        DMA_CTRL_WRITE(slice, addr + 1, (reg).fall);                                                                             \
        DMA_CTRL_WRITE(slice, addr + 2, (reg).counter_enable);                                                                   \
        DMA_CTRL_WRITE(slice, addr + 3, (reg).counter_disable);                                                                  \
    } while (0)

#define WAVEGEN_TIMING_CONFIG(slice, reg, addr)                                                                                  \
    do {                                                                                                                         \
        DMA_CTRL_WRITE(slice, addr + 0, (reg).activate);                                                                         \
        DMA_CTRL_WRITE(slice, addr + 1, (reg).read);                                                                             \
        DMA_CTRL_WRITE(slice, addr + 2, (reg).write);                                                                            \
        DMA_CTRL_WRITE(slice, addr + 3, (reg).precharge);                                                                        \
        DMA_CTRL_WRITE(slice, addr + 4, (reg).refresh_start);                                                                    \
        DMA_CTRL_WRITE(slice, addr + 5, (reg).refresh_activ);                                                                    \
        DMA_CTRL_WRITE(slice, addr + 6, (reg).refresh_prech);                                                                    \
        DMA_CTRL_WRITE(slice, addr + 7, (reg).refresh_end);                                                                      \
    } while (0)

#define WAVEGEN_VECTOR_SAMPLING_CONFIG(slice, reg, addr)                                                                         \
    do {                                                                                                                         \
        DMA_CTRL_WRITE(slice, addr + 0, (reg).rab_gross);                                                                        \
        DMA_CTRL_WRITE(slice, addr + 1, (reg).cat_gross);                                                                        \
        DMA_CTRL_WRITE(slice, addr + 2, (reg).dwbsb_gross);                                                                      \
        DMA_CTRL_WRITE(slice, addr + 3, (reg).drbsb_gross);                                                                      \
        DMA_CTRL_WRITE(slice, addr + 4, (reg).drbsb_fine);                                                                       \
    } while (0)

#define WAVEGEN_ROWHAMMER_AND_REFRESH_CONFIG(slice, reg, addr)                                                                   \
    do {                                                                                                                         \
        DMA_CTRL_WRITE(slice, addr + 0, ((reg) >> 0) & 0xFF);                                                                    \
        DMA_CTRL_WRITE(slice, addr + 1, ((reg) >> 8) & 0xFF);                                                                    \
    } while (0)

#define WAVEGEN_ROWHAMMER_CONFIG(slice, reg, addr)                                                                               \
    do {                                                                                                                         \
        DMA_CTRL_WRITE(slice, addr + 0, ((reg) >> 0) & 0xFF);                                                                    \
        DMA_CTRL_WRITE(slice, addr + 1, ((reg) >> 8) & 0xFF);                                                                    \
    } while (0)

#define EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION 0x000103FF0F8FCFEFULL

#define IRAM_TIMING_SAFE_VALUE 0x04
#define IRAM_TIMING_NORMAL_VALUE 0x04
#define IRAM_TIMING_AGGRESSIVE_VALUE 0x05

#define RFRAM_TIMING_SAFE_VALUE 0x01
#define RFRAM_TIMING_NORMAL_VALUE 0x04
#define RFRAM_TIMING_AGGRESSIVE_VALUE 0x05

#define WRAM_TIMING_SAFE_VALUE 0x05
#define WRAM_TIMING_NORMAL_VALUE 0x05
#define WRAM_TIMING_AGGRESSIVE_VALUE 0x06

#define REFRESH_MODE_VALUE 4

/* clang-format off */

__API_SYMBOL__ const struct _dpu_dma_engine_configuration_t dma_engine_cas_config = {
    .refresh_access_number = 53,
    .column_read_latency = 7,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

/* Configuration for DPU_CLOCK_DIV2 */
__API_SYMBOL__ const struct _dpu_dma_engine_configuration_t dma_engine_clock_div2_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 9,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div2_config = {
    .MCBAB    = { .rise = 0x00, .fall = 0x39, .counter_enable = 0x01, .counter_disable = 0x01 },
    .RCYCLKB  = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x01, .counter_disable = 0x03 },
    .WCYCLKB  = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x02, .counter_disable = 0x03 },
    .DWCLKB   = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x01, .counter_disable = 0x02 },
    .DWAEB    = { .rise = 0x31, .fall = 0x08, .counter_enable = 0x02, .counter_disable = 0x04 },
    .DRAPB    = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x01, .counter_disable = 0x03 },
    .DRAOB    = { .rise = 0x31, .fall = 0x39, .counter_enable = 0x03, .counter_disable = 0x04 },
    .DWBSB_EN = { .rise = 0x3C, .fall = 0x3E, .counter_enable = 0x01, .counter_disable = 0x03 },
    .vector_sampling = { .rab_gross = 0x01, .cat_gross = 0x01, .dwbsb_gross = 0x01, .drbsb_gross = 0x02, .drbsb_fine = 0x01 },
    .timing_completion = {.activate = 0x0A, .read = 0x03, .write = 0x03, .precharge = 0x0B,
        /* refresh_start, refresh_activ, refresh_prech, refresh_end computed later depending on frequency */
        },
    /* refresh_and_row_hammer_info computed later depending on frequency */
    .row_hammer_config = 0x06 };

/* Configuration for DPU_CLOCK_DIV3 */
__API_SYMBOL__ const struct _dpu_dma_engine_configuration_t dma_engine_clock_div3_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 8,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div3_config = {
    .MCBAB    = { .rise = 0x00, .fall = 0x39, .counter_enable = 0x02, .counter_disable = 0x02 },
    .RCYCLKB  = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x02, .counter_disable = 0x04 },
    .WCYCLKB  = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x03, .counter_disable = 0x04 },
    .DWCLKB   = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x02, .counter_disable = 0x03 },
    .DWAEB    = { .rise = 0x31, .fall = 0x08, .counter_enable = 0x03, .counter_disable = 0x05 },
    .DRAPB    = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x02, .counter_disable = 0x04 },
    .DRAOB    = { .rise = 0x31, .fall = 0x39, .counter_enable = 0x04, .counter_disable = 0x05 },
    .DWBSB_EN = { .rise = 0x3C, .fall = 0x3E, .counter_enable = 0x02, .counter_disable = 0x04 },
    .vector_sampling = { .rab_gross = 0x01, .cat_gross = 0x01, .dwbsb_gross = 0x01, .drbsb_gross = 0x01, .drbsb_fine = 0x02 },
    .timing_completion = { .activate = 0x0F, .read = 0x05, .write = 0x05, .precharge = 0x0B,
        /* refresh_start, refresh_activ, refresh_prech, refresh_end computed later depending on frequency */
        },
    /* refresh_and_row_hammer_info computed later depending on frequency */
    .row_hammer_config = 0x06 };

/* Configuration for DPU_CLOCK_DIV4 */
__API_SYMBOL__ const struct _dpu_dma_engine_configuration_t dma_engine_clock_div4_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 7,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct _dpu_waveform_generator_configuration_t waveform_generator_clock_div4_config = {
    .MCBAB    = { .rise = 0x00, .fall = 0x39, .counter_enable = 0x03, .counter_disable = 0x03 },
    .RCYCLKB  = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x03, .counter_disable = 0x05 },
    .WCYCLKB  = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x04, .counter_disable = 0x05 },
    .DWCLKB   = { .rise = 0x31, .fall = 0x00, .counter_enable = 0x03, .counter_disable = 0x04 },
    .DWAEB    = { .rise = 0x31, .fall = 0x08, .counter_enable = 0x04, .counter_disable = 0x06 },
    .DRAPB    = { .rise = 0x33, .fall = 0x39, .counter_enable = 0x03, .counter_disable = 0x05 },
    .DRAOB    = { .rise = 0x31, .fall = 0x39, .counter_enable = 0x05, .counter_disable = 0x06 },
    .DWBSB_EN = { .rise = 0x3C, .fall = 0x3E, .counter_enable = 0x03, .counter_disable = 0x05 },
    .vector_sampling = { .rab_gross = 0x02, .cat_gross = 0x02, .dwbsb_gross = 0x01, .drbsb_gross = 0x01, .drbsb_fine = 0x02 },
    .timing_completion = { .activate = 0x14, .read = 0x07, .write = 0x07, .precharge = 0x0B,
        /* refresh_start, refresh_activ, refresh_prech, refresh_end computed later depending on frequency */
        },
    /* refresh_and_row_hammer_info computed later depending on frequency */
    .row_hammer_config = 0x06 };

/* clang-format on */

__API_SYMBOL__ dpu_error_t
dpu_reset_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_transaction_t transaction;
    dpu_error_t status;

    if ((status = dpu_custom_for_rank(rank, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_RESET)) != DPU_OK) {
        goto end;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // todo inject the for loop in each reset subfunction?
    for (dpu_slice_id_t each_slice = 0; each_slice < rank->description->topology.nr_of_control_interfaces; ++each_slice) {
        /* 1/ [CI] Byte order */
        uint64_t byte_order_result;

        uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;
        bool saved_dpus_are_enabled = rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled;
        dpu_selected_mask_t saved_enabled_dpus = rank->runtime.control_interface.slice_info[each_slice].enabled_dpus;
        dpu_selected_mask_t mask_all_dpus = dpu_mask_all(nr_dpus_per_ci);

        /* Enabling ALL DPUs (regardless of disabled DPUs) for the reset process */
        rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled = true;
        rank->runtime.control_interface.slice_info[each_slice].enabled_dpus = mask_all_dpus;
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            dpu_get(rank, each_slice, each_dpu)->enabled = true;
        }

        status = dpu_byte_order(transaction, rank, each_slice, &byte_order_result);
        if (status != DPU_OK)
            goto err_query;

        /* 1 Bis/ [CI] Software reset */
        /* This soft reset is needed to reset the color and start from a known state. */
        status = dpu_software_reset(transaction, rank, each_slice, true);
        if (status != DPU_OK)
            goto err_query;

        /* 2/ [CI] Bit order */
        status = dpu_bit_order(transaction, rank, each_slice, byte_order_result);
        if (status != DPU_OK)
            goto err_query;

        /* 3/ [CI] Software reset */
        status = dpu_software_reset(transaction, rank, each_slice, false);
        if (status != DPU_OK)
            goto err_query;

        /* 4/ [CI] Bit order, again */
        status = dpu_bit_order(transaction, rank, each_slice, byte_order_result);
        if (status != DPU_OK)
            goto err_query;

        /* 5/ [CI] Identity */
        status = dpu_identity(transaction, rank, each_slice);

        if (status != DPU_OK)
            goto err_query;

        /* 6/ [CI] Thermal config */
        status = dpu_thermal_config(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 7/ [CI] [DPU] Carousel configuration */
        struct _dpu_carousel_configuration_t carousel_config = {
            .cmd_duration = rank->description->timings.cmd_duration,
            .cmd_sampling = rank->description->timings.cmd_sampling,
            .res_duration = rank->description->timings.res_duration,
            .res_sampling = rank->description->timings.res_sampling,
        };
        status = dpu_configure_carousel(rank, each_slice, &carousel_config);
        if (status != DPU_OK)
            goto err_query;

        /* Reverting disabled DPUs before setting groups */

        rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled = saved_dpus_are_enabled;
        rank->runtime.control_interface.slice_info[each_slice].enabled_dpus = saved_enabled_dpus;
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            dpu_get(rank, each_slice, each_dpu)->enabled = (saved_enabled_dpus & (1 << each_dpu)) != 0;
        }

        /* 8/ [DPU] Set group */
        status = dpu_set_group(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* Back to ALL DPUs enabled for the rest of the reset */
        rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled = true;
        rank->runtime.control_interface.slice_info[each_slice].enabled_dpus = mask_all_dpus;
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            dpu_get(rank, each_slice, each_dpu)->enabled = true;
        }

        /* 9/ [DPU] Iram repair */
        status = dpu_check_for_iram_repair(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 9 Bis/ [DPU] Wram repair */
        status = dpu_check_for_wram_repair(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        struct _dpu_dma_engine_configuration_t dma_engine_config;
        struct _dpu_waveform_generator_configuration_t waveform_generator_config;

        fetch_dma_and_wavegen_configs(rank->description->configuration.fck_frequency_in_mhz,
            rank->description->timings.clock_division,
            REFRESH_MODE_VALUE,
            &dma_engine_config,
            &waveform_generator_config);

        /* 9 Ter/ [DPU] DMA Engine Configuration */
        status = dpu_configure_dma_engine(
            rank, each_slice, (rank->type == CYCLE_ACCURATE_SIMULATOR) ? &dma_engine_cas_config : &dma_engine_config);
        if (status != DPU_OK)
            goto err_query;
        status = dpu_configure_dma_shuffling_box(rank, each_slice, &rank->description->configuration.pcb_transformation);
        if (status != DPU_OK)
            goto err_query;

        /* 9 Quater/ [DPU] Waveform Generator Configuration */
        status = dpu_configure_waveform_generator(rank, each_slice, &waveform_generator_config);
        if (status != DPU_OK)
            goto err_query;

        /* 10/ [DPU] Clear Debug Replace */
        status = dpu_clear_debug_replace(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 11/ [DPU] Clear PC mode (ie set it to DPU_PC_16) */
        status = dpu_clear_pc_mode(transaction, rank, each_slice, DPU_PC_16);
        if (status != DPU_OK)
            goto err_query;

        /* 12/ [DPU] Clear faults */
        status = dpu_clear_faults_for_rank(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 13/ [DPU] Clear thread run reg */
        status = dpu_clear_thread_run_reg_for_rank(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 14/ [DPU] Clear notify bit reg */
        status = dpu_clear_notify_bit_reg_for_rank(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 15/ [DPU] Set Stack_Up */
        status = dpu_set_stack_up(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 16/ [DPU] Reset time configuration, zero and carry flags, pcs */
        status = dpu_reset_internal_state(transaction, rank, each_slice);
        if (status != DPU_OK)
            goto err_query;

        /* 17/ [DPU] Give MRAM control to the DPUs */
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; each_dpu += 2) {
            struct dpu_t *dpu = dpu_get(rank, each_slice, each_dpu);
            status = dpu_host_release_access_for_dpu(dpu);
            if (status != DPU_OK)
                goto err_query;
        }

        /* Setting enabled DPUs information for the rest of the execution */
        rank->runtime.control_interface.slice_info[each_slice].all_dpus_are_enabled = saved_dpus_are_enabled;
        rank->runtime.control_interface.slice_info[each_slice].enabled_dpus = saved_enabled_dpus;
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            dpu_get(rank, each_slice, each_dpu)->enabled = (saved_enabled_dpus & (1 << each_dpu)) != 0;
        }

        /* 18/ Backend custom operation */
        status = map_rank_status_to_api_status(
            rank->handler_context->handler->custom_operation(rank, each_slice, 0, DPU_COMMAND_ALL_SOFT_RESET, NULL));
        if (status != DPU_OK)
            goto err_query;
    }

    status = dpu_custom_for_rank(rank, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_RESET);

err_query:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_soft_reset_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_pc_mode_e pc_mode;
    dpu_planner_status_e planner_status;
    dpu_rank_status_e rank_status;
    uint32_t ignored;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    if ((status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_RESET)) != DPU_OK) {
        goto end;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    /* 1/ Stop running thread by injecting a bkp fault */

    safe_add_query(
        query, dpu_query_build_set_and_step_dpu_fault_state_for_dpu(slice_id, member_id), transaction, status, err_query);
    safe_add_query(query, dpu_query_build_set_bkp_fault_for_dpu(slice_id, member_id), transaction, status, err_query);
    /* Interception fault clear */
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_dpu(slice_id, member_id, &ignored), transaction, status, err_query);

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    /* 2/ Stop all threads */
    if (!fetch_natural_pc_mode(rank, &pc_mode)) {
        status = DPU_ERR_INTERNAL;
        goto err_query;
    }

    status = drain_pipeline(dpu, NULL, pc_mode, false);
    if (status != DPU_OK)
        goto err_query;

    /* 3/ Clear all faults: dma, mem, bkp, poison and global */
    status = dpu_clear_faults_for_dpu(transaction, dpu);
    if (status != DPU_OK)
        goto err_query;

    /* 4/ Reset time configuration, zero and carry flags, pcs */
    status = dpu_reset_internal_state_for_dpu(transaction, dpu);
    if (status != DPU_OK)
        goto err_query;

    /* 5/ Clear Notify bits */
    status = dpu_clear_notify_bit_reg_for_dpu(transaction, dpu);
    if (status != DPU_OK)
        goto err_query;

    /* 6/ Backend custom operation */
    rank_status = rank->handler_context->handler->custom_operation(rank, slice_id, member_id, DPU_COMMAND_DPU_SOFT_RESET, NULL);
    if (rank_status != DPU_RANK_SUCCESS) {
        status = map_rank_status_to_api_status(rank_status);
        goto err_query;
    }

    status = dpu_custom_for_dpu(dpu, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_RESET);

err_query:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_carousel(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_carousel_configuration_t *config)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    uint8_t cmd_duration = config->cmd_duration;
    uint8_t cmd_sampling = config->cmd_sampling;
    uint8_t res_duration = config->res_duration;
    uint8_t res_sampling = config->res_sampling;

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "cmd_duration: %d cmd_sampling: %d res_duration: %d res_sampling: %d",
        cmd_duration,
        cmd_sampling,
        res_duration,
        res_sampling);

    // We suppose that we can configure the caroussel for ALL dpus (even disabled DPUs)
    safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, err_query);

    /* a/ [CI]  Cmd futur lllll */
    safe_add_query(
        query, dpu_query_build_command_duration_configuration_for_control(slice_id, cmd_duration), transaction, status, end);
    /* b/ [DPU] Cmd futur lllll */
    safe_add_query(
        query, dpu_query_build_command_bus_duration_configuration_for_previous(slice_id, cmd_duration), transaction, status, end);
    /* c/ [DPU] Cmd futur vvvvv */
    safe_add_query(
        query, dpu_query_build_command_bus_sampling_configuration_for_previous(slice_id, cmd_sampling), transaction, status, end);
    /* d/ [DPU] Cmd sync */
    safe_add_query(query, dpu_query_build_command_bus_synchronization_for_previous(slice_id), transaction, status, end);
    /* e/ [CI]  Res futur lllll */
    safe_add_query(
        query, dpu_query_build_result_duration_configuration_for_control(slice_id, res_duration), transaction, status, end);
    /* f/ [CI]  Res futur vvvvv */
    safe_add_query(
        query, dpu_query_build_result_sampling_configuration_for_control(slice_id, res_sampling), transaction, status, end);
    /* g/ [DPU] Res futur lllll */
    safe_add_query(
        query, dpu_query_build_result_bus_duration_configuration_for_previous(slice_id, res_duration), transaction, status, end);
    /* h/ [DPU] Res sync */
    safe_add_query(query, dpu_query_build_result_bus_synchronization_for_previous(slice_id), transaction, status, end);

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_ci_shuffling_box(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_pcb_transformation_t *config)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_transaction_t transaction;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t bit_order_result;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "CI shuffling box config: cpu_to_dpu: 0x%04x dpu_to_cpu: 0x%04x nibble_swap: 0x%02x",
        config->cpu_to_dpu,
        config->dpu_to_cpu,
        config->nibble_swap);

    /* Important: no shuffling boxes, so we have to software-shuffle correctly for the parameters of BIT_ORDER
     * to be understood...We want DPU to read a, so we send m(a):
     * CPU: a       -> mm(a)        DPU
     * CPU: m(a)    -> mm(m(a)) = a DPU
     * But no need to shuffle nibble swap since it is either 0 or 0xFFFFFFFF and nothing to do with bit_order_result.
     */
    safe_add_query(query,
        dpu_query_build_bit_order_for_control(slice_id,
            (uint16_t)dpu_pcb_transformation_dpu_to_cpu(&rank->description->configuration.pcb_transformation, config->cpu_to_dpu),
            (uint16_t)dpu_pcb_transformation_dpu_to_cpu(&rank->description->configuration.pcb_transformation, config->dpu_to_cpu),
            config->nibble_swap,
            0x00,
            &bit_order_result),
        transaction,
        status,
        end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_dma_engine(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_dma_engine_configuration_t *config)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "DMA engine config: refresh_access_number: %d column_read_latency: %d minimal_access_number: %d "
        "default_time_origin: %d ldma_to_sdma_time_origin: %d xdma_time_start_activate: %d "
        "xdma_time_start_access: %d sdma_time_start_wb_f1: %d",
        config->refresh_access_number,
        config->column_read_latency,
        config->minimal_access_number,
        config->default_time_origin,
        config->ldma_to_sdma_time_origin,
        config->xdma_time_start_activate,
        config->xdma_time_start_access,
        config->sdma_time_start_wb_f1);

    uint64_t dma_engine_timing = 0L;
    dma_engine_timing |= (((uint64_t)config->refresh_access_number) & 0x7FL) << 36;
    dma_engine_timing |= (((uint64_t)config->column_read_latency) & 0x0FL) << 32;
    dma_engine_timing |= (((uint64_t)config->minimal_access_number) & 0x07L) << 29;
    dma_engine_timing |= (((uint64_t)config->default_time_origin) & 0x7FL) << 22;
    dma_engine_timing |= (((uint64_t)config->ldma_to_sdma_time_origin) & 0x7FL) << 15;
    dma_engine_timing |= (((uint64_t)config->xdma_time_start_activate) & 0x1FL) << 10;
    dma_engine_timing |= (((uint64_t)config->xdma_time_start_access) & 0x1FL) << 5;
    dma_engine_timing |= (((uint64_t)config->sdma_time_start_wb_f1) & 0x1FL) << 0;

    // We suppose that we can configure the DMA engine for ALL dpus (even disabled DPUs)
    safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, err);
    // Configure DMA Engine Timing
    DMA_CTRL_WRITE(slice_id, 0x20, (dma_engine_timing >> 0) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x21, (dma_engine_timing >> 8) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x22, (dma_engine_timing >> 16) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x23, (dma_engine_timing >> 24) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x24, (dma_engine_timing >> 32) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x25, (dma_engine_timing >> 40) & 0xFF);

    // Clear DMA Engine Configuration Path and flush reg_replace_instr of readop2
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_dma_shuffling_box(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, const struct _dpu_pcb_transformation_t *config)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "DMA shuffling box config: cpu_to_dpu: 0x%04x dpu_to_cpu: 0x%04x nibble_swap: 0x%02x",
        config->cpu_to_dpu,
        config->dpu_to_cpu,
        config->nibble_swap);

    // We suppose that we can configure the DMA shuffling box for ALL dpus (even disabled DPUs)
    safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, err);
    // Configure Jedec Shuffling box
    DMA_CTRL_WRITE(slice_id, 0x10, (config->dpu_to_cpu >> 8) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x11, (config->dpu_to_cpu >> 0) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x12, (config->cpu_to_dpu >> 8) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x13, (config->cpu_to_dpu >> 0) & 0xFF);
    DMA_CTRL_WRITE(slice_id, 0x14, config->nibble_swap);
    // Clear DMA Engine Configuration Path and flush reg_replace_instr of readop2
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_waveform_generator(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    const struct _dpu_waveform_generator_configuration_t *config)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_transaction_t transaction;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    // We suppose that we can configure the waveform generator for ALL dpus (even disabled DPUs)
    safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, err);

    WAVEGEN_REG_CONFIG(slice_id, config->MCBAB, 0xC0);
    WAVEGEN_REG_CONFIG(slice_id, config->RCYCLKB, 0xC4);
    WAVEGEN_REG_CONFIG(slice_id, config->WCYCLKB, 0xC8);
    WAVEGEN_REG_CONFIG(slice_id, config->DWCLKB, 0xCC);
    WAVEGEN_REG_CONFIG(slice_id, config->DWAEB, 0xD0);
    WAVEGEN_REG_CONFIG(slice_id, config->DRAPB, 0xD4);
    WAVEGEN_REG_CONFIG(slice_id, config->DRAOB, 0xD8);
    WAVEGEN_REG_CONFIG(slice_id, config->DWBSB_EN, 0xDC);
    WAVEGEN_TIMING_CONFIG(slice_id, config->timing_completion, 0xE0);
    WAVEGEN_VECTOR_SAMPLING_CONFIG(slice_id, config->vector_sampling, 0xE8);
    WAVEGEN_ROWHAMMER_AND_REFRESH_CONFIG(slice_id, config->refresh_and_row_hammer_info, 0xFC); // Row Hammer Info
    WAVEGEN_ROWHAMMER_CONFIG(slice_id, config->row_hammer_config, 0xFE);

    for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
        // Configuration MUST start with dpu_id = 4 because of refresh
        dpu_member_id_t target_dpu;

        if (rank->description->topology.nr_of_dpus_per_control_interface <= 4)
            target_dpu = each_dpu;
        else
            target_dpu = (each_dpu >= 4) ? (each_dpu - 4) : (each_dpu + 4);

        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, target_dpu), transaction, status, err);

        // DPU Takes control of MRAM REFRESH (Note: in ASIC only, waveFormGen config shall start with dpu_id = 4)
        DMA_CTRL_WRITE(slice_id, 0x83, 0x01); // dpu_ref_ctrl

        // Clear WaveForm Generator Configuration Path and flush reg_replace_instr of readop2
        safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_waveform_generator_read_status(struct dpu_t *dpu, uint8_t read_addr, uint8_t *read_value)
{
    LOG_DPU(VERBOSE, dpu, "%d", read_addr);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_transaction_t transaction;
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, member_id), transaction, status, err);

    DMA_CTRL_WRITE(slice_id, 0xFF, read_addr & 0x3);
    // Clear WaveForm Generator Configuration Path and flush reg_replace_instr of readop2
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);
    safe_add_query(query, dpu_query_build_read_dma_control_for_previous(slice_id, read_value), transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_configure_iram_repair_for_previous(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    uint8_t AB_msbs,
    uint8_t A_lsbs,
    uint8_t B_lsbs,
    uint8_t CD_msbs,
    uint8_t C_lsbs,
    uint8_t D_lsbs,
    uint8_t even_index,
    uint8_t odd_index)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_transaction_t transaction;
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    safe_add_query(query,
        dpu_query_build_irepair_AB_configuration_for_previous(slice_id, A_lsbs, B_lsbs, AB_msbs),
        transaction,
        status,
        err);
    safe_add_query(query,
        dpu_query_build_irepair_CD_configuration_for_previous(slice_id, C_lsbs, D_lsbs, CD_msbs),
        transaction,
        status,
        err);
    safe_add_query(query,
        dpu_query_build_irepair_OE_configuration_for_previous(slice_id, even_index, odd_index, IRAM_TIMING_SAFE_VALUE),
        transaction,
        status,
        err);
    safe_add_query(query,
        dpu_query_build_register_file_timing_configuration_for_previous(slice_id, RFRAM_TIMING_SAFE_VALUE),
        transaction,
        status,
        err);

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

__API_SYMBOL__ dpu_error_t
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
    uint8_t odd_index)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_transaction_t transaction;
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_query_t query;

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    dpu_dma_ctrl_t wram_bank_addr = (dpu_dma_ctrl_t)(0x60 | (wram_bank_index << 2));

    // Configure WRAM Bank Even & Odd bits
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(
            slice_id, wram_bank_addr, 0x60, 0x60 | (even_index >> 1), 0x60 | (odd_index >> 1), 0x60, 0x40),
        transaction,
        status,
        err);
    // Configure WRAM Bank AB Address[11:4]
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(
            slice_id, wram_bank_addr, 0x61, 0x60 | (AB_msbs >> 4), 0x60 | (AB_msbs & 0xF), 0x60, 0x40),
        transaction,
        status,
        err);
    // Configure WRAM Bank A Address[3:0] and B Address[3:0]
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(slice_id, wram_bank_addr, 0x62, 0x60 | A_lsbs, 0x60 | B_lsbs, 0x60, 0x40),
        transaction,
        status,
        err);
    // Configure WRAM Bank CD Address[11:4]
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(
            slice_id, wram_bank_addr, 0x63, 0x60 | (CD_msbs >> 4), 0x60 | (CD_msbs & 0xF), 0x60, 0x40),
        transaction,
        status,
        err);
    // Configure WRAM Bank C Address[3:0] and D Address[3:0]
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(slice_id, wram_bank_addr, 0x64, 0x60 | C_lsbs, 0x60 | D_lsbs, 0x60, 0x40),
        transaction,
        status,
        err);
    // Configure WRAM Bank Timing to safe value
    safe_add_query(query,
        dpu_query_build_write_dma_control_for_previous(
            slice_id, wram_bank_addr, 0x65, 0x60, 0x60 | WRAM_TIMING_SAFE_VALUE, 0x60, 0x40),
        transaction,
        status,
        err);
    // Clear WRAM Bank Configuration Path and flush reg_replace_instr of readop2
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free(transaction);
end:
    return status;
}

static void
__dpu_set_host_mux_mram_state(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    rank->runtime.control_interface.slice_info[slice_id].host_mux_mram_state |= (1 << dpu_id);
}

static void
__dpu_clear_host_mux_mram_state(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    rank->runtime.control_interface.slice_info[slice_id].host_mux_mram_state &= ~(1 << dpu_id);
}

__API_SYMBOL__ dpu_error_t
dpu_host_release_access_for_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    dpu_error_t status = DPU_OK;
    dpu_transaction_t transaction;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    dpu_member_id_t dpu_pair_base_id = (dpu_member_id_t)(dpu_id & ~1);
    struct dpu_t *pair_base_dpu = dpu_get(rank, slice_id, dpu_pair_base_id);

    /* Stores the state of the mux in case the debugger intervenes: we record the state before
     * actually placing the mux in this state. If we did record the state after setting the mux,
     * we could be interrupted between the setting of the mux and the recording of the state, and
     * then the debugger would miss a mux state.
     */
    __dpu_clear_host_mux_mram_state(rank, slice_id, dpu_pair_base_id);
    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface)
        __dpu_clear_host_mux_mram_state(rank, slice_id, dpu_pair_base_id + 1);

    if (!rank->description->configuration.api_must_switch_mram_mux && !rank->description->configuration.init_mram_mux)
        /*
         * Yes, this is not an error, that just means that the following configuration should not be done since
         * it is useless and incorrect.
         */
        return DPU_OK;

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        return DPU_ERR_MRAM_BUSY;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL)
        return DPU_ERR_SYSTEM;

    status = host_release_access(transaction, pair_base_dpu);
    if (status != DPU_OK)
        goto err;

    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface) {
        struct dpu_t *paired_dpu = dpu_get(rank, slice_id, (dpu_member_id_t)(dpu_pair_base_id + 1));
        status = host_release_access(transaction, paired_dpu);
        if (status != DPU_OK)
            goto err;
    }

err:
    dpu_transaction_free(transaction);
    return status;
}

/* Must be called with rank locked */
__API_SYMBOL__ dpu_error_t
dpu_host_get_access_for_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    dpu_error_t status = DPU_OK;
    dpu_transaction_t transaction;
    dpu_member_id_t dpu_pair_base_id = (dpu_member_id_t)(dpu_id & ~1);
    struct dpu_t *pair_base_dpu = dpu_get(rank, slice_id, dpu_pair_base_id);

    /* Stores the state of the mux in case the debugger intervenes: we record the state before
     * actually placing the mux in this state. If we did record the state after setting the mux,
     * we could be interrupted between the setting of the mux and the recording of the state, and
     * then the debugger would miss a mux state.
     */
    __dpu_set_host_mux_mram_state(rank, slice_id, dpu_pair_base_id);
    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface)
        __dpu_set_host_mux_mram_state(rank, slice_id, dpu_pair_base_id + 1);

    if (!rank->description->configuration.api_must_switch_mram_mux)
        /*
         * Yes, this is not an error, that just means that the following configuration should not be done since
         * it is useless and incorrect.
         */
        return DPU_OK;

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        return DPU_ERR_MRAM_BUSY;
    }

    if ((transaction = dpu_transaction_new(rank->description->topology.nr_of_control_interfaces)) == NULL)
        return DPU_ERR_SYSTEM;

    status = host_get_access(transaction, pair_base_dpu);
    if (status != DPU_OK)
        goto err;

    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface) {
        struct dpu_t *paired_dpu = dpu_get(rank, slice_id, (dpu_member_id_t)(dpu_pair_base_id + 1));
        status = host_get_access(transaction, paired_dpu);
        if (status != DPU_OK)
            goto err;
    }

err:
    dpu_transaction_free(transaction);
    return status;
}

static bool
byte_order_values_are_compatible(uint64_t reference, uint64_t found)
{
    for (unsigned int each_byte = 0; each_byte < sizeof(uint64_t); ++each_byte) {
        if (__builtin_popcount((uint8_t)((reference >> (8 * each_byte)) & 0xFFl))
            != __builtin_popcount((uint8_t)((found >> (8 * each_byte)) & 0xFFl))) {
            return false;
        }
    }

    return true;
}

static dpu_error_t
dpu_byte_order(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, uint64_t *byte_order_result)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    safe_add_query(query, dpu_query_build_byte_order_for_control(slice_id, byte_order_result), transaction, status, end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    if (!byte_order_values_are_compatible(*byte_order_result, EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION)) {
        LOG_CI(WARNING,
            rank,
            slice_id,
            "ERROR: invalid byte order (reference: 0x%016lx; found: 0x%016lx)",
            EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION,
            *byte_order_result);
        status = DPU_ERR_DRIVER;
        goto err_query;
    }

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

static dpu_error_t
dpu_software_reset(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, bool param_is_null)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    dpu_rank_status_e rank_status;
    dpu_clock_division_t clock_division
        = param_is_null ? 0 : from_division_factor_to_dpu_enum(rank->description->timings.clock_division);
    uint8_t cycle_accurate = (uint8_t)((rank->description->configuration.enable_cycle_accurate_behavior == true) ? 1 : 0);

    safe_add_query(
        query, dpu_query_build_software_reset_for_control(slice_id, cycle_accurate, clock_division), transaction, status, end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    // Software Reset has been sent. We need to wait "some time" before we can continue (~ 20 clock cycles)
    // For simulator backends, we need to run the simulation. We suppose that these function calls will be long enough
    // to put a real hardware backend in a stable state.
    uint64_t *buffer;
    uint32_t reset_duration = rank->description->timings.reset_wait_duration;

    buffer = malloc(rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    if (!buffer) {
        status = DPU_ERR_SYSTEM;
        goto err_query;
    }

    /* This is needed for some driver backends that avoid to read control interface if no commands were
     * sent before: by memsetting at a value != 0, we force the control interface reading.
     */
    memset(buffer, 0xFF, rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));

    switch (clock_division) {
        case DPU_CLOCK_DIV8:
            reset_duration *= 4;
            break;
        case DPU_CLOCK_DIV4:
            reset_duration *= 2;
            break;
        case DPU_CLOCK_DIV3:
            reset_duration = reset_duration * 3 / 2;
            break;
        default /*DPU_CLOCK_DIV2*/:
            break;
    }

    for (uint32_t reset_duration_iteration = 0; reset_duration_iteration < reset_duration; ++reset_duration_iteration) {
        if ((rank_status = rank->handler_context->handler->update_commands(rank, &buffer)) != DPU_RANK_SUCCESS) {
            status = map_rank_status_to_api_status(rank_status);
            goto err_query;
        }
    }

    /* Reset the color of the slice */
    rank->runtime.control_interface.color &= ~(1 << slice_id);
    rank->runtime.control_interface.fault_collide &= ~(1 << slice_id);
    rank->runtime.control_interface.fault_decode &= ~(1 << slice_id);

    free(buffer);
err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

static dpu_error_t
dpu_bit_order(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, uint64_t byte_order_result)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t bit_order_result;

    /* 1/ Send a "NULL" bit order command */
    safe_add_query(
        query, dpu_query_build_bit_order_for_control(slice_id, 0, 0, 0, 0, &bit_order_result), transaction, status, end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    /* 2/ Then, from byte_order_result and bit_order_result, compute the necessary "transformation" from
     * dpu to cpu and cpu to dpu and insert the result into a new bit order command.
     */
    dpu_pcb_transformation_fill(bit_order_result, &rank->description->configuration.pcb_transformation);

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "byte_order: 0x%016lx bit_order: 0x%08x nibble_swap: 0x%02x cpu_to_dpu: 0x%04x dpu_to_cpu: 0x%04x",
        byte_order_result,
        bit_order_result,
        rank->description->configuration.pcb_transformation.nibble_swap,
        rank->description->configuration.pcb_transformation.cpu_to_dpu,
        rank->description->configuration.pcb_transformation.dpu_to_cpu);

    if ((status = dpu_configure_ci_shuffling_box(rank, slice_id, &rank->description->configuration.pcb_transformation))
        != DPU_OK) {
        goto err_query;
    }

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

static dpu_error_t
dpu_identity(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t identity_result;

    safe_add_query(query, dpu_query_build_identity_for_control(slice_id, &identity_result), transaction, status, end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    /* No shuffling boxes, so the result must be software-shuffled */
    dpu_chip_id_e identity_found
        = (dpu_chip_id_e)dpu_pcb_transformation_cpu_to_dpu(&rank->description->configuration.pcb_transformation, identity_result);

    if (identity_found != rank->description->signature.chip_id) {
        LOG_CI(WARNING,
            rank,
            slice_id,
            "ERROR: invalid identity (expected: 0x%08x; found: 0x%08x)",
            rank->description->signature.chip_id,
            identity_found);
        status = DPU_ERR_INTERNAL;
        goto err_query;
    }

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

static dpu_temperature_e
from_celsius_to_dpu_enum(uint8_t temperature)
{
    if (temperature < 50)
        return DPU_TEMPERATURE_LESS_THAN_50;
    if (temperature < 60)
        return DPU_TEMPERATURE_BETWEEN_50_AND_60;
    if (temperature < 70)
        return DPU_TEMPERATURE_BETWEEN_60_AND_70;
    if (temperature < 80)
        return DPU_TEMPERATURE_BETWEEN_70_AND_80;
    if (temperature < 90)
        return DPU_TEMPERATURE_BETWEEN_80_AND_90;
    if (temperature < 100)
        return DPU_TEMPERATURE_BETWEEN_90_AND_100;
    if (temperature < 110)
        return DPU_TEMPERATURE_BETWEEN_100_AND_110;

    return DPU_TEMPERATURE_GREATER_THAN_110;
}

static dpu_error_t
dpu_thermal_config(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    dpu_temperature_e temperature = from_celsius_to_dpu_enum(rank->description->timings.std_temperature);

    LOG_CI(DEBUG,
        rank,
        slice_id,
        "thermal threshold set at %d°C (value: 0x%04x)",
        rank->description->timings.std_temperature,
        temperature);

    safe_add_query(query, dpu_query_build_thermal_configuration_for_control(slice_id, temperature), transaction, status, end);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

static dpu_error_t
dpu_set_group(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    if (rank->runtime.control_interface.slice_info[slice_id].all_dpus_are_enabled) {
        safe_add_query(query, dpu_query_build_select_all_for_control(slice_id), transaction, status, err_query);
        safe_add_query(query, dpu_query_build_write_group_for_previous(slice_id, DPU_ENABLED_GROUP), transaction, status, end);
    } else {
        uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
        for (int each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
            if (dpu_get(rank, slice_id, each_dpu)->enabled) {
                safe_add_query(
                    query, dpu_query_build_write_group_for_dpu(slice_id, each_dpu, DPU_ENABLED_GROUP), transaction, status, end);
            } else {
                safe_add_query(
                    query, dpu_query_build_write_group_for_dpu(slice_id, each_dpu, DPU_DISABLED_GROUP), transaction, status, end);
            }
        }
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
end:
    return status;
}

// TODO: Should find a way to tell upper layers that iram repair took place if any.
/* This function fails if only ONE DPU is impossible to repair. */
static dpu_error_t
dpu_check_for_iram_repair(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    dpu_planner_status_e planner_status;
    dpu_error_t status;
    dpu_query_t query;
    dpu_member_id_t dpu_id;

    if (rank->description->configuration.do_iram_repair) {
        LOG_CI(VERBOSE, rank, slice_id, "IRAM repair enabled");
        /* 1/ Fill the IRAM with "0", and read it back: if any bit is stuck at "1" => fix this problem. */
        status = find_faulty_iram_bits_blocked_at(transaction, rank, slice_id, false);
        if (status != DPU_OK)
            goto err;

        /* 2/ Fill the IRAM with "1", and read it back: if any bit is stuck at "0" => fix this problem. */
        status = find_faulty_iram_bits_blocked_at(transaction, rank, slice_id, true);
        if (status != DPU_OK)
            goto err;

        for (dpu_id = 0; dpu_id < rank->description->topology.nr_of_dpus_per_control_interface; ++dpu_id) {
            struct dpu_t *dpu = dpu_get(rank, slice_id, dpu_id);
            status = try_to_repair_iram(transaction, dpu);
            if (status != DPU_OK)
                goto err;
        }
    } else {
        // Default configuration
        LOG_CI(VERBOSE, rank, slice_id, "IRAM repair disabled");
        build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        status = dpu_configure_iram_repair_for_previous(rank, slice_id, 0, 0, 0, 0, 0, 0, 0, 1);
        if (status != DPU_OK)
            goto err;
    }

err:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    return status;
}

static dpu_error_t
dpu_check_for_wram_repair(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_query_t query;

    if (rank->description->configuration.do_wram_repair) {
        LOG_CI(VERBOSE, rank, slice_id, "WRAM repair enabled");
        /* 1/ Fill the WRAM with "0", and read it back: if any bit is stuck at "1" => fix this problem. */
        status = find_faulty_wram_bits_blocked_at(transaction, rank, slice_id, false);
        if (status != DPU_OK)
            goto err;

        /* 2/ Fill the WRAM with "1", and read it back: if any bit is stuck at "0" => fix this problem. */
        status = find_faulty_wram_bits_blocked_at(transaction, rank, slice_id, true);
        if (status != DPU_OK)
            goto err;

        for (dpu_member_id_t dpu_id = 0; dpu_id < rank->description->topology.nr_of_dpus_per_control_interface; ++dpu_id) {
            struct dpu_t *dpu = dpu_get(rank, slice_id, dpu_id);
            status = try_to_repair_wram(transaction, dpu);
            if (status != DPU_OK)
                goto err;
        }
    } else {
        LOG_CI(VERBOSE, rank, slice_id, "WRAM repair disabled");
        // Default configuration
        for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
            build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
            safe_execute_transaction(transaction, rank, planner_status, status, err);
            status = dpu_configure_wram_repair_for_previous(rank, slice_id, each_wram_bank, 0, 0, 0, 0, 0, 0, 0, 1);
            if (status != DPU_OK)
                goto err;
        }
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err);

err:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    return status;
}

static dpu_error_t
dpu_clear_debug_replace(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;

    for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
        safe_add_query(
            query, dpu_query_build_debug_std_replace_clear_for_dpu(slice_id, each_dpu), transaction, status, err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_clear_pc_mode(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    dpu_pc_mode_e *pc_modes;

    uint8_t nr_of_dpus_per_control_interface = rank->description->topology.nr_of_dpus_per_control_interface;
    if ((pc_modes = malloc(nr_of_dpus_per_control_interface * sizeof(*pc_modes))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    safe_add_query(query, dpu_query_build_write_pc_mode_for_previous(slice_id, pc_mode), transaction, status, err_query);

    for (dpu_member_id_t dpu_id = 0; dpu_id < nr_of_dpus_per_control_interface; ++dpu_id) {
        if (dpu_get(rank, slice_id, dpu_id)->enabled) {
            safe_add_query(
                query, dpu_query_build_read_pc_mode_for_dpu(slice_id, dpu_id, pc_modes + dpu_id), transaction, status, err_query);
        }
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    for (dpu_member_id_t dpu_id = 0; dpu_id < nr_of_dpus_per_control_interface; ++dpu_id) {
        struct dpu_t *dpu = dpu_get(rank, slice_id, dpu_id);
        if (dpu->enabled) {
            if (pc_modes[dpu_id] != pc_mode) {
                LOG_DPU(WARNING, dpu, "ERROR: invalid PC mode (expected: %d, found: %d)", pc_mode, pc_modes[dpu_id]);
                status = DPU_ERR_INTERNAL;
                break;
            }
        }
    }

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    free(pc_modes);
end:
    return status;
}

static dpu_error_t
dpu_clear_faults_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;

    /* 0/ Selection */
    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    /* 1/ POISON fault */
    safe_add_query(query, dpu_query_build_clear_poison_fault_for_previous(slice_id), transaction, status, err_query);
    /* 2/ BKP fault */
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_previous(slice_id), transaction, status, err_query);
    /* 3/ MEM fault */
    safe_add_query(
        query, dpu_query_build_read_and_clear_mem_fault_for_previous(slice_id, &ignored), transaction, status, err_query);
    /* 4/ DMA fault */
    safe_add_query(
        query, dpu_query_build_read_and_clear_dma_fault_for_previous(slice_id, &ignored), transaction, status, err_query);
    /* 5/ General fault */
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_previous(slice_id), transaction, status, err_query);
    /* 5-bis/ Interception Fault */
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_previous(slice_id, &ignored), transaction, status, err_query);

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_clear_faults_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    /* 1/ BKP fault */
    safe_add_query(query, dpu_query_build_clear_bkp_fault_for_dpu(slice_id, dpu_id), transaction, status, err_query);
    /* 2/ MEM fault */
    safe_add_query(
        query, dpu_query_build_read_and_clear_mem_fault_for_dpu(slice_id, dpu_id, &ignored), transaction, status, err_query);
    /* 3/ DMA fault */
    safe_add_query(
        query, dpu_query_build_read_and_clear_dma_fault_for_dpu(slice_id, dpu_id, &ignored), transaction, status, err_query);
    /* 4/ POISON fault */
    safe_add_query(query, dpu_query_build_clear_poison_fault_for_dpu(slice_id, dpu_id), transaction, status, err_query);
    /* 5/ General fault */
    safe_add_query(query, dpu_query_build_clear_dpu_fault_state_for_dpu(slice_id, dpu_id), transaction, status, err_query);
    /* 5-bis/ Interception Fault */
    safe_add_query(query, dpu_query_build_read_bkp_fault_for_dpu(slice_id, dpu_id, &ignored), transaction, status, err_query);

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_clear_thread_run_reg_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;

    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    for (dpu_thread_t each_thread = 0; each_thread < rank->description->dpu.nr_of_threads; ++each_thread) {
        safe_add_query(query,
            dpu_query_build_clear_run_thread_for_previous(slice_id, each_thread, &ignored),
            transaction,
            status,
            err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_clear_notify_bit_reg_for_rank(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;
    uint8_t offset = rank->description->dpu.nr_of_threads;

    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    for (dpu_notify_bit_id_t each_notify_bit = 0; each_notify_bit < rank->description->dpu.nr_of_notify_bits; ++each_notify_bit) {
        dpu_notify_bit_id_t real_notify_bit = offset + each_notify_bit;
        safe_add_query(query,
            dpu_query_build_read_and_clear_notify_bit_for_previous(slice_id, real_notify_bit, &ignored),
            transaction,
            status,
            err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_clear_notify_bit_reg_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;
    uint8_t offset = rank->description->dpu.nr_of_threads;

    for (dpu_notify_bit_id_t each_notify_bit = 0; each_notify_bit < rank->description->dpu.nr_of_notify_bits; ++each_notify_bit) {
        dpu_notify_bit_id_t real_notify_bit = offset + each_notify_bit;
        safe_add_query(query,
            dpu_query_build_read_and_clear_notify_bit_for_dpu(slice_id, dpu_id, real_notify_bit, &ignored),
            transaction,
            status,
            err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_set_stack_up(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored, previous_stack_up;
    dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[slice_id].enabled_dpus;

    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    safe_add_query(query, dpu_query_build_read_and_set_stack_up_for_previous(slice_id, &ignored), transaction, status, err_query);
    safe_add_query(
        query, dpu_query_build_read_and_set_stack_up_for_previous(slice_id, &previous_stack_up), transaction, status, err_query);
    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    if (previous_stack_up != mask_all) {
        LOG_CI(
            WARNING, rank, slice_id, "ERROR: invalid stack mode (expected: 0x%08x, found: 0x%08x)", mask_all, previous_stack_up);
        status = DPU_ERR_INTERNAL;
        goto err_query;
    }

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    return status;
}

static dpu_error_t
dpu_reset_internal_state(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;
    uint32_t result;
    iram_size_t internal_state_reset_size;
    dpuinstruction_t *internal_state_reset = fetch_internal_reset_program(&internal_state_reset_size);

    if (internal_state_reset == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[slice_id].enabled_dpus;

    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err_query);

    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(
            slice_id, mask_all, 0, (dpuinstruction_t *)internal_state_reset, internal_state_reset_size),
        transaction,
        status,
        err_query);

    for (dpu_thread_t each_thread = 0; each_thread < rank->description->dpu.nr_of_threads; ++each_thread) {
        safe_add_query(
            query, dpu_query_build_boot_thread_for_previous(slice_id, each_thread, &ignored), transaction, status, err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(query, dpu_query_build_read_dpu_run_state_for_previous(slice_id, &result), transaction, status, err_query);

    do {
        safe_execute_transaction(transaction, rank, planner_status, status, err_query);
    } while ((result & mask_all) != 0);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    free(internal_state_reset);
end:
    return status;
}

static dpu_error_t
dpu_reset_internal_state_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status = DPU_OK;
    dpu_query_t query;
    dpu_planner_status_e planner_status;
    uint32_t ignored;
    uint32_t result;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;

    iram_size_t internal_state_reset_size;
    dpuinstruction_t *internal_state_reset = fetch_internal_reset_program(&internal_state_reset_size);

    if (internal_state_reset == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_dpu(
            slice_id, member_id, mask_one, 0, (dpuinstruction_t *)internal_state_reset, internal_state_reset_size),
        transaction,
        status,
        err_query);

    for (dpu_thread_t each_thread = 0; each_thread < rank->description->dpu.nr_of_threads; ++each_thread) {
        safe_add_query(query,
            dpu_query_build_boot_thread_for_dpu(slice_id, member_id, each_thread, &ignored),
            transaction,
            status,
            err_query);
    }

    safe_execute_transaction(transaction, rank, planner_status, status, err_query);

    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    safe_add_query(
        query, dpu_query_build_read_dpu_run_state_for_dpu(slice_id, member_id, &result), transaction, status, err_query);

    do {
        safe_execute_transaction(transaction, rank, planner_status, status, err_query);
    } while ((result & mask_one) != 0);

err_query:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    free(internal_state_reset);
end:
    return status;
}

static dpu_error_t
dpu_check_wavegen_mux_status_for_dpu(dpu_transaction_t transaction, struct dpu_t *dpu, uint8_t expected)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status;
    dpu_dma_ctrl_t dpu_dma_ctrl = 0;

    status = wavegen_mux_status(transaction, dpu, expected, 0x02, &dpu_dma_ctrl);
    if (status != DPU_OK)
        goto err;

    if (dpu_dma_ctrl & 0x80) {
        LOG_DPU(WARNING, dpu, "MRAM collision detected, either host or DPU wrote in the MRAM without permission. Clearing it...");
        status = wavegen_mux_status(transaction, dpu, expected, 0x82, &dpu_dma_ctrl);
        if (status != DPU_OK)
            goto err;
    }

err:
    return status;
}

static dpu_error_t
host_release_access(dpu_transaction_t transaction, struct dpu_t *dpu)
{
#define WAVEGEN_MUX_DPU_EXPECTED 0x03
    dpu_error_t status;
    dpu_planner_status_e planner_status;
    dpu_query_t query;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, dpu_id), transaction, status, err);

    DMA_CTRL_WRITE(slice_id, 0x80, 0x01); // dpu_bank_ctrl
    DMA_CTRL_WRITE(slice_id, 0x81, 0x00); // dpu_wrmask_ctrl
    DMA_CTRL_WRITE(slice_id, 0x82, 0x01); // dpu_wr_ctrl
    DMA_CTRL_WRITE(slice_id, 0x84, 0x01); // dpu_wr_en_ctrl

    /* Clear WaveForm Generator Configuration Path and flush reg_replace_instr of readop2 */
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    status = dpu_check_wavegen_mux_status_for_dpu(transaction, dpu, WAVEGEN_MUX_DPU_EXPECTED);
    if (status != DPU_OK)
        goto err;

err:
    return status;
}

static dpu_error_t
host_get_access(dpu_transaction_t transaction, struct dpu_t *dpu)
{
#define WAVEGEN_MUX_HOST_EXPECTED 0x00
    dpu_error_t status;
    dpu_planner_status_e planner_status;
    dpu_query_t query;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, dpu_id), transaction, status, err);
    DMA_CTRL_WRITE(slice_id, 0x80, 0x00); // dpu_bank_ctrl
    DMA_CTRL_WRITE(slice_id, 0x81, 0x00); // dpu_wrmask_ctrl
    DMA_CTRL_WRITE(slice_id, 0x82, 0x00); // dpu_wr_ctrl
    DMA_CTRL_WRITE(slice_id, 0x84, 0x00); // dpu_wr_en_ctrl

    /* Clear WaveForm Generator Configuration Path and flush reg_replace_instr of readop2 */
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);

    status = dpu_check_wavegen_mux_status_for_dpu(transaction, dpu, WAVEGEN_MUX_HOST_EXPECTED);
    if (status != DPU_OK)
        goto err;

err:
    return status;
}

static dpu_error_t
wavegen_mux_status(dpu_transaction_t transaction,
    struct dpu_t *dpu,
    uint8_t expected,
    uint8_t cmd,
    dpu_dma_ctrl_t *ret_dpu_dma_ctrl)
{
#define TIMEOUT_MUX_STATUS (10000)
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_dma_ctrl_t dpu_dma_ctrl = 0;
    uint32_t timeout = TIMEOUT_MUX_STATUS;
    dpu_query_t query;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;

    // Check Mux control through dma_rdat_ctrl of fetch1
    // 1 - Select WaveGen Read register @0xFF and set it @0x02  (mux and collision ctrl)
    // 2 - Flush readop2 (Pipeline to DMA cfg data path)
    // 3 - Read dpu_dma_ctrl
    DMA_CTRL_WRITE(slice_id, 0xFF, cmd); // Select read register Mux ctrl
    // Clear WaveForm Generator Configuration Path and flush reg_replace_instr of readop2
    safe_add_query(query, dpu_query_build_clear_dma_control_for_previous(slice_id), transaction, status, err);

    do {
        safe_add_query(query, dpu_query_build_read_dma_control_for_previous(slice_id, &dpu_dma_ctrl), transaction, status, err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        dpu_transaction_free_queries_for_slice(transaction, slice_id);
        // Expected 0x3 for DPU4 since it is the only one to be refreshed
        // Expected 0x0 for others DPU since no refresh has been issued
        LOG_DPU(VERBOSE, dpu, "XMA Init = 0x%02x (expected = 0x%02x)", dpu_dma_ctrl, expected);
        timeout--;
    } while (timeout && (dpu_dma_ctrl & 0x7F) != expected); // Do not check Collision Error bit

    if (!timeout) {
        LOG_DPU(WARNING, dpu, "Timeout waiting for result to be correct");
        return rank->description->configuration.disable_api_safe_checks ? DPU_OK : DPU_ERR_TIMEOUT;
    }

    *ret_dpu_dma_ctrl = dpu_dma_ctrl;

err:
    return status;
}

static void
check_if_memory_address_is_faulty_and_update_context(struct dpu_memory_repair_t *repair_info,
    uint32_t address,
    uint64_t expected,
    uint64_t found)
{
    uint64_t faulty_bits = expected ^ found;

    if (faulty_bits != 0) {
        uint32_t previous_nr_of_corrupted_addr = repair_info->nr_of_corrupted_addresses++;
        uint32_t index = previous_nr_of_corrupted_addr;
        uint32_t current_max_index
            = (previous_nr_of_corrupted_addr > NB_MAX_REPAIR_ADDR) ? NB_MAX_REPAIR_ADDR : previous_nr_of_corrupted_addr;

        for (uint32_t each_known_corrupted_addr_idx = 0; each_known_corrupted_addr_idx < current_max_index;
             ++each_known_corrupted_addr_idx) {
            if (repair_info->corrupted_addresses[previous_nr_of_corrupted_addr].address == address) {
                index = each_known_corrupted_addr_idx;
                repair_info->nr_of_corrupted_addresses--;
                break;
            }
        }

        if (index < NB_MAX_REPAIR_ADDR) {
            repair_info->corrupted_addresses[index].address = address;

            if (expected == 0) {
                repair_info->corrupted_addresses[index].bits_set_to_one = faulty_bits;
            } else {
                repair_info->corrupted_addresses[index].bits_set_to_zero = faulty_bits;
            }
        }
    }
}

static dpu_error_t
find_faulty_iram_bits_blocked_at(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, bool bit_is_set)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status;
    dpu_planner_status_e planner_status;
    dpu_query_t query;
    dpuinstruction_t *iram_content;

    dpuinstruction_t instruction_value = bit_is_set ? 0x0000FFFFFFFFFFFFL : 0x0000000000000000L;
    uint8_t byte_value = (uint8_t)instruction_value;
    uint8_t nb_of_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
    iram_size_t iram_size = rank->description->memories.iram_size;
    dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[slice_id].enabled_dpus;

    iram_content = malloc(iram_size * sizeof(*iram_content));
    if (iram_content == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    memset(iram_content, byte_value, iram_size * sizeof(*iram_content));

    // Configuration to repair something else than address 0
    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    status = dpu_configure_iram_repair_for_previous(rank, slice_id, 0, 1, 1, 0, 1, 1, 0, 1);
    if (status != DPU_OK)
        goto err;

    // Write + Read address 0
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(slice_id, mask_all, 0, iram_content, 1),
        transaction,
        status,
        err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    for (dpu_member_id_t each_dpu = 0; each_dpu < nb_of_dpus; ++each_dpu) {
        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, each_dpu), transaction, status, err);
        safe_add_query(
            query, dpu_query_build_read_iram_instruction_for_previous(slice_id, 0, 1, iram_content), transaction, status, err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        dpu_transaction_free_queries_for_slice(transaction, slice_id);

        struct dpu_t *dpu = dpu_get(rank, slice_id, each_dpu);
        check_if_memory_address_is_faulty_and_update_context(&dpu->repair.iram_repair, 0, instruction_value, iram_content[0]);
    }

    // Configuration to repair address 0
    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    status = dpu_configure_iram_repair_for_previous(rank, slice_id, 0, 0, 0, 0, 0, 0, 0, 1);
    if (status != DPU_OK)
        goto err;

    // Write + Read the rest of the IRAM
    safe_add_query(query,
        dpu_query_build_write_iram_instruction_for_previous(
            slice_id, mask_all, 1, iram_content + 1, (iram_size_t)(iram_size - 1)),
        transaction,
        status,
        err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);

    for (dpu_member_id_t each_dpu = 0; each_dpu < nb_of_dpus; ++each_dpu) {
        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, each_dpu), transaction, status, err);
        safe_add_query(query,
            dpu_query_build_read_iram_instruction_for_previous(slice_id, 1, (iram_size_t)(iram_size - 1), iram_content + 1),
            transaction,
            status,
            err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        dpu_transaction_free_queries_for_slice(transaction, slice_id);

        struct dpu_t *dpu = dpu_get(rank, slice_id, each_dpu);
        for (iram_addr_t each_address = 1; each_address < iram_size; ++each_address) {
            check_if_memory_address_is_faulty_and_update_context(
                &dpu->repair.iram_repair, each_address, instruction_value, iram_content[each_address]);
        }
    }

err:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    free(iram_content);
end:
    return status;
}

static inline uint8_t
count_nr_of_faulty_bits(uint64_t data)
{
    return __builtin_popcountll(data);
}

static inline uint8_t
find_index_of_first_faulty_bit(uint64_t data)
{
    return __builtin_ctz(data);
}

static inline uint8_t
find_index_of_last_faulty_bit(uint64_t data)
{
    return 63 - __builtin_clz(data);
}

static bool
extract_memory_repair_configuration(struct dpu_t *dpu,
    struct dpu_memory_repair_t *repair_info,
    uint8_t *AB_msbs,
    uint8_t *A_lsbs,
    uint8_t *B_lsbs,
    uint8_t *CD_msbs,
    uint8_t *C_lsbs,
    uint8_t *D_lsbs,
    uint8_t *even_index,
    uint8_t *odd_index)
{
    *AB_msbs = 0xFF;
    *CD_msbs = 0xFF;
    *A_lsbs = 0xFF;
    *B_lsbs = 0xFF;
    *C_lsbs = 0xFF;
    *D_lsbs = 0xFF;
    *even_index = 0xFF;
    *odd_index = 0xFF;

    uint32_t nr_of_corrupted_addr = repair_info->nr_of_corrupted_addresses;

    LOG_DPU(DEBUG, dpu, "repair info: number of corrupted addresses: %d", nr_of_corrupted_addr);

    for (uint32_t each_corrupted_addr_index = 0; each_corrupted_addr_index < nr_of_corrupted_addr; ++each_corrupted_addr_index) {
        LOG_DPU(DEBUG,
            dpu,
            "repair info: #%d address: 0x%04x bits_set_to_zero: 0x%016lx bits_set_to_one: 0x%016lx",
            each_corrupted_addr_index,
            repair_info->corrupted_addresses[each_corrupted_addr_index].address,
            repair_info->corrupted_addresses[each_corrupted_addr_index].bits_set_to_zero,
            repair_info->corrupted_addresses[each_corrupted_addr_index].bits_set_to_one);
    }

    if (nr_of_corrupted_addr > NB_MAX_REPAIR_ADDR) {
        LOG_DPU(WARNING, dpu, "ERROR: too many corrupted addresses (%d > %d)", nr_of_corrupted_addr, NB_MAX_REPAIR_ADDR);
        return false;
    }

    for (uint32_t each_corrupted_addr_index = 0; each_corrupted_addr_index < nr_of_corrupted_addr; ++each_corrupted_addr_index) {
        uint32_t address = repair_info->corrupted_addresses[each_corrupted_addr_index].address;
        uint8_t msbs = (uint8_t)(address >> 4);
        uint8_t lsbs = (uint8_t)(address & 0xF);

        if (*A_lsbs == 0xFF) {
            *AB_msbs = msbs;
            *A_lsbs = lsbs;
        } else if ((*B_lsbs == 0xFF) && (*AB_msbs == msbs)) {
            *B_lsbs = lsbs;
        } else if (*C_lsbs == 0xFF) {
            *CD_msbs = msbs;
            *C_lsbs = lsbs;
        } else if ((*D_lsbs == 0xFF) && (*CD_msbs == msbs)) {
            *D_lsbs = lsbs;
        } else {
            LOG_DPU(WARNING, dpu, "ERROR: corrupted addresses are too far apart");
            return false;
        }
    }

    dpuinstruction_t faulty_bits = 0L;

    for (uint8_t each_faulty_address = 0; each_faulty_address < nr_of_corrupted_addr; ++each_faulty_address) {
        faulty_bits |= repair_info->corrupted_addresses[each_faulty_address].bits_set_to_zero;
        faulty_bits |= repair_info->corrupted_addresses[each_faulty_address].bits_set_to_one;
    }

    uint8_t nr_of_faulty_bits = count_nr_of_faulty_bits(faulty_bits);

    if (nr_of_faulty_bits > 2) {
        LOG_DPU(WARNING, dpu, "ERROR: too many corrupted bits (%d > %d)", nr_of_faulty_bits, 2);
        return false;
    }

    uint8_t first_index;
    uint8_t last_index;

    switch (count_nr_of_faulty_bits(faulty_bits)) {
        default:
            return false;
        case 2:
            last_index = find_index_of_last_faulty_bit(faulty_bits);

            if ((last_index & 1) == 1) {
                *odd_index = last_index;
            } else {
                *even_index = last_index;
            }
            // FALLTHROUGH
        case 1:
            first_index = find_index_of_first_faulty_bit(faulty_bits);

            if ((first_index & 1) == 1) {
                if (*odd_index != 0xFF) {
                    LOG_DPU(WARNING, dpu, "ERROR: both corrupted bit indices are odd");
                    return false;
                }

                *odd_index = first_index;
            } else {
                if (*even_index != 0xFF) {
                    LOG_DPU(WARNING, dpu, "ERROR: both corrupted bit indices are even");
                    return false;
                }

                *even_index = first_index;
            }
            // FALLTHROUGH
        case 0:
            break;
    }

    // We can repair the memory! Let's choose default values then return the configuration.
    if (*A_lsbs == 0xFF) {
        *A_lsbs = 0xF;
    }
    if (*B_lsbs == 0xFF) {
        *B_lsbs = 0xF;
    }
    if (*C_lsbs == 0xFF) {
        *C_lsbs = 0xF;
    }
    if (*D_lsbs == 0xFF) {
        *D_lsbs = 0xF;
    }

    if (*even_index == 0xFF) {
        *even_index = 0;
    }

    if (*odd_index == 0xFF) {
        *odd_index = 1;
    }

    LOG_DPU(DEBUG,
        dpu,
        "valid repair config: AB_MSBs: 0x%02x A_LSBs: 0x%01x B_LSBs: 0x%01x CD_MSBs: 0x%02x C_LSBs: 0x%01x D_LSBs: 0x%01x",
        *AB_msbs,
        *A_lsbs,
        *B_lsbs,
        *CD_msbs,
        *C_lsbs,
        *D_lsbs);

    return true;
}

static dpu_error_t
try_to_repair_iram(dpu_transaction_t transaction, struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status;
    bool fail_to_repair = false;
    uint8_t AB_msbs, CD_msbs, A_lsbs, B_lsbs, C_lsbs, D_lsbs, even_index, odd_index;
    dpu_query_t query;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    if (!extract_memory_repair_configuration(
            dpu, &dpu->repair.iram_repair, &AB_msbs, &A_lsbs, &B_lsbs, &CD_msbs, &C_lsbs, &D_lsbs, &even_index, &odd_index)) {
        LOG_DPU(WARNING, dpu, "ERROR: cannot repair IRAM");
        fail_to_repair = true;
        status = DPU_ERR_CORRUPTED_MEMORY;
        goto end;
    }

    safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, dpu_id), transaction, status, end);
    status = dpu_configure_iram_repair_for_previous(
        rank, slice_id, AB_msbs, A_lsbs, B_lsbs, CD_msbs, C_lsbs, D_lsbs, even_index, odd_index);

end:
    dpu->repair.iram_repair.fail_to_repair = fail_to_repair;
    return status;
}

static dpu_error_t
find_faulty_wram_bits_blocked_at(dpu_transaction_t transaction, struct dpu_rank_t *rank, dpu_slice_id_t slice_id, bool bit_is_set)
{
    LOG_CI(VERBOSE, rank, slice_id, "");
    dpu_error_t status = DPU_OK;
    dpu_planner_status_e planner_status;
    dpu_query_t query;
    dpuword_t *wram_content;

    dpuword_t word_value = bit_is_set ? 0xFFFFFFFF : 0x00000000;
    uint8_t byte_value = (uint8_t)word_value;
    uint8_t nb_of_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
    wram_size_t wram_size = rank->description->memories.wram_size;
    dpu_selected_mask_t mask_all = rank->runtime.control_interface.slice_info[slice_id].enabled_dpus;

    wram_content = malloc(wram_size * sizeof(*wram_content));
    if (wram_content == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    memset(wram_content, byte_value, wram_size * sizeof(*wram_content));

    // Configuration to repair something else than address 0 (for each wram bank)
    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
        status = dpu_configure_wram_repair_for_previous(rank, slice_id, each_wram_bank, 0, 1, 1, 0, 1, 1, 0, 1);
        if (status != DPU_OK)
            goto err;
    }

    // Write + Read address 0 (for each wram bank)
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(slice_id, mask_all, 0, wram_content, NR_OF_WRAM_BANKS),
        transaction,
        status,
        err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    for (dpu_member_id_t each_dpu = 0; each_dpu < nb_of_dpus; ++each_dpu) {
        struct dpu_t *dpu = dpu_get(rank, slice_id, each_dpu);
        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, each_dpu), transaction, status, err);
        safe_add_query(query,
            dpu_query_build_read_wram_word_for_previous(slice_id, 0, NR_OF_WRAM_BANKS, wram_content),
            transaction,
            status,
            err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        dpu_transaction_free_queries_for_slice(transaction, slice_id);

        for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
            check_if_memory_address_is_faulty_and_update_context(
                &dpu->repair.wram_repair[each_wram_bank], 0, word_value, wram_content[each_wram_bank]);
        }
    }

    // Configuration to repair address 0 (for each wram bank)
    build_select_query_for_all_enabled_dpus(rank, slice_id, query, transaction, status, err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);
    dpu_transaction_free_queries_for_slice(transaction, slice_id);

    for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
        status = dpu_configure_wram_repair_for_previous(rank, slice_id, each_wram_bank, 0, 0, 0, 0, 0, 0, 0, 1);
        if (status != DPU_OK)
            goto err;
    }

    // Write + Read the rest of the WRAM
    safe_add_query(query,
        dpu_query_build_write_wram_word_for_previous(
            slice_id, mask_all, NR_OF_WRAM_BANKS, wram_content + NR_OF_WRAM_BANKS, (wram_size_t)(wram_size - NR_OF_WRAM_BANKS)),
        transaction,
        status,
        err);
    safe_execute_transaction(transaction, rank, planner_status, status, err);

    for (dpu_member_id_t each_dpu = 0; each_dpu < nb_of_dpus; ++each_dpu) {
        struct dpu_t *dpu = dpu_get(rank, slice_id, each_dpu);
        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, each_dpu), transaction, status, err);
        safe_add_query(query,
            dpu_query_build_read_wram_word_for_previous(
                slice_id, NR_OF_WRAM_BANKS, (wram_size_t)(wram_size - NR_OF_WRAM_BANKS), wram_content + NR_OF_WRAM_BANKS),
            transaction,
            status,
            err);
        safe_execute_transaction(transaction, rank, planner_status, status, err);
        dpu_transaction_free_queries_for_slice(transaction, slice_id);

        for (wram_addr_t each_address = NR_OF_WRAM_BANKS; each_address < wram_size; ++each_address) {
            check_if_memory_address_is_faulty_and_update_context(&dpu->repair.wram_repair[each_address % NR_OF_WRAM_BANKS],
                each_address / NR_OF_WRAM_BANKS,
                word_value,
                wram_content[each_address]);
        }
    }

err:
    dpu_transaction_free_queries_for_slice(transaction, slice_id);
    free(wram_content);
end:
    return status;
}

static dpu_error_t
try_to_repair_wram(dpu_transaction_t transaction, struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status = DPU_OK;
    uint8_t AB_msbs, CD_msbs, A_lsbs, B_lsbs, C_lsbs, D_lsbs, even_index, odd_index;
    dpu_query_t query;
    uint8_t each_wram_bank;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    for (each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
        if (!extract_memory_repair_configuration(dpu,
                &dpu->repair.wram_repair[each_wram_bank],
                &AB_msbs,
                &A_lsbs,
                &B_lsbs,
                &CD_msbs,
                &C_lsbs,
                &D_lsbs,
                &even_index,
                &odd_index)) {
            LOG_DPU(WARNING, dpu, "ERROR: cannot repair WRAM bank #%d", each_wram_bank);
            dpu->repair.wram_repair[each_wram_bank].fail_to_repair = true;
            status = DPU_ERR_CORRUPTED_MEMORY;
            goto end;
        }

        safe_add_query(query, dpu_query_build_select_dpu_for_control(slice_id, dpu_id), transaction, status, end);
        status = dpu_configure_wram_repair_for_previous(
            rank, slice_id, each_wram_bank, AB_msbs, A_lsbs, B_lsbs, CD_msbs, C_lsbs, D_lsbs, even_index, odd_index);
        dpu->repair.wram_repair[each_wram_bank].fail_to_repair = false;
    }

end:
    return status;
}

static inline uint32_t
nr_cycles_for(double duration_in_ns, double frequency_in_mhz)
{
    return ceil(duration_in_ns / (1000. / frequency_in_mhz));
}

static void
fill_timing_configuration(dpu_dma_engine_configuration_t dma_configuration,
    dpu_waveform_generator_configuration_t wavegen_configuration,
    uint32_t fck_frequency,
    uint8_t fck_division,
    uint8_t refresh_mode)
{
#define REFRESH_START_VALUE (2)
#define REFRESH_END_VALUE (2)

#define REFRESH_ACTIVATE_DURATION (51.2)
#define REFRESH_PRECHARGE_DURATION (26.9)

#define REFRESH_NR_PULSES_DEFAULT (4)

    uint32_t tRFC;
    uint32_t refresh_mode_log2;
    switch (refresh_mode) {
        case 1:
            tRFC = 345;
            refresh_mode_log2 = 0;
            break;
        case 2:
            tRFC = 255;
            refresh_mode_log2 = 1;
            break;
        default:
            tRFC = 155;
            refresh_mode_log2 = 2;
            break;
    }

    double half_fck_frequency = fck_frequency / 2.;
    uint32_t refresh_start = REFRESH_START_VALUE;
    uint32_t refresh_activate = nr_cycles_for(REFRESH_ACTIVATE_DURATION, half_fck_frequency) - 1;
    uint32_t refresh_precharge = nr_cycles_for(REFRESH_PRECHARGE_DURATION, half_fck_frequency) - 1;
    uint32_t refresh_end = REFRESH_END_VALUE;

    uint32_t refresh_nr_pulses = REFRESH_NR_PULSES_DEFAULT / refresh_mode;
    uint32_t refresh;
    uint32_t refresh_dma;
    bool config_ok;

    do {
        refresh = refresh_start + 1 + refresh_nr_pulses * (refresh_activate + 1)
            + (refresh_nr_pulses - 1) * (refresh_precharge + 1) + nr_cycles_for(14., half_fck_frequency);

        refresh_dma = floor(tRFC / (((double)(4 * fck_division * 1000)) / fck_frequency)) - 1;

        uint32_t refresh_cycles_total = 1 + refresh_start + 1 + refresh_nr_pulses * (refresh_activate + 1)
            + refresh_nr_pulses * (refresh_precharge + 1) + refresh_end + 1;

        uint32_t dma_cycles = (refresh_dma + 1) * 4 * fck_division / 2;

        if (dma_cycles <= refresh_cycles_total) {
            refresh_activate--;
            refresh_precharge--;
            config_ok = false;
        } else {
            config_ok = true;
        }
    } while (!config_ok);

    dma_configuration->refresh_access_number = refresh_dma;
    wavegen_configuration->timing_completion.refresh_start = refresh_start;
    wavegen_configuration->timing_completion.refresh_activ = refresh_activate;
    wavegen_configuration->timing_completion.refresh_prech = refresh_precharge;
    wavegen_configuration->timing_completion.refresh_end = refresh_end;
    wavegen_configuration->refresh_and_row_hammer_info
        = (((refresh >> 8) & 1) << 14) | ((refresh_mode_log2 & 0x3) << 8) | (refresh & 0xFF);
}

static void
fetch_dma_and_wavegen_configs(uint32_t fck_frequency,
    uint8_t clock_division,
    uint8_t refresh_mode,
    struct _dpu_dma_engine_configuration_t *dma_config,
    struct _dpu_waveform_generator_configuration_t *wavegen_config)
{
    dpu_clock_division_t clock_div = from_division_factor_to_dpu_enum(clock_division);

    const struct _dpu_dma_engine_configuration_t *reference_dma_config;
    const struct _dpu_waveform_generator_configuration_t *reference_wavegen_config;

    switch (clock_div) {
        case DPU_CLOCK_DIV4: {
            reference_dma_config = &dma_engine_clock_div4_config;
            reference_wavegen_config = &waveform_generator_clock_div4_config;
            break;
        }
        case DPU_CLOCK_DIV3: {
            reference_dma_config = &dma_engine_clock_div3_config;
            reference_wavegen_config = &waveform_generator_clock_div3_config;
            break;
        }
        default /*DPU_CLOCK_DIV2 (and the MRAM invalid DPU_CLOCK_DIV8 config)*/: {
            reference_dma_config = &dma_engine_clock_div2_config;
            reference_wavegen_config = &waveform_generator_clock_div2_config;
            clock_division = 2;
            break;
        }
    }

    memcpy(dma_config, reference_dma_config, sizeof(*dma_config));
    memcpy(wavegen_config, reference_wavegen_config, sizeof(*wavegen_config));

    fill_timing_configuration(dma_config, wavegen_config, fck_frequency, clock_division, refresh_mode);
}
