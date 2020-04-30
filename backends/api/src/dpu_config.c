/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <math.h>
#include <stdint.h>
#include <string.h>

#include <dpu_config.h>
#include <dpu_custom.h>
#include <dpu_error.h>
#include <dpu_description.h>
#include <dpu_management.h>
#include <dpu_types.h>

#include <dpu/ufi_bit_config.h>
#include <dpu/ufi.h>
#include <dpu_api_log.h>
#include <dpu_attributes.h>
#include <dpu_internals.h>
#include <dpu_mask.h>
#include <dpu_predef_programs.h>
#include <dpu_rank.h>
#include <verbose_control.h>
#include <dpu_instruction_encoder.h>

static dpu_error_t
dpu_byte_order(struct dpu_rank_t *rank);
static dpu_error_t
dpu_soft_reset(struct dpu_rank_t *rank, dpu_clock_division_t clock_division);
static dpu_error_t
dpu_carousel_config(struct dpu_rank_t *rank, const struct dpu_carousel_config *config);
static dpu_error_t
dpu_ci_shuffling_box_config(struct dpu_rank_t *rank, const struct dpu_bit_config *config);
static dpu_error_t
dpu_bit_config(struct dpu_rank_t *rank, struct dpu_bit_config *config);
static dpu_error_t
dpu_identity(struct dpu_rank_t *rank);
static dpu_error_t
dpu_thermal_config(struct dpu_rank_t *rank, uint8_t thermal_config);
static dpu_error_t
dpu_init_groups(struct dpu_rank_t *rank, const bool *all_dpus_are_enabled_save, const dpu_selected_mask_t *enabled_dpus_save);
static dpu_error_t
dpu_iram_repair_config(struct dpu_rank_t *rank);
static dpu_error_t
dpu_wram_repair_config(struct dpu_rank_t *rank);
static dpu_error_t
dpu_clear_debug(struct dpu_rank_t *rank);
static dpu_error_t
dpu_clear_run_bits(struct dpu_rank_t *rank);
static dpu_error_t
dpu_set_pc_mode(struct dpu_rank_t *rank, enum dpu_pc_mode pc_mode);
static dpu_error_t
dpu_set_stack_direction(struct dpu_rank_t *rank, bool stack_is_up);
static dpu_error_t
dpu_reset_internal_state(struct dpu_rank_t *rank);
static dpu_error_t
dpu_init_mram_mux(struct dpu_rank_t *rank);

static dpu_error_t
try_to_repair_iram(struct dpu_rank_t *rank);
static dpu_error_t
try_to_repair_wram(struct dpu_rank_t *rank);

static bool
byte_order_values_are_compatible(uint64_t reference, uint64_t found);
static enum dpu_temperature
from_celsius_to_dpu_enum(uint8_t temperature);

static dpu_error_t
host_get_access_for_dpu(struct dpu_t *dpu);
static dpu_error_t
wavegen_mux_status_for_dpu(struct dpu_t *dpu, uint8_t expected, uint8_t cmd, uint8_t *ret_dpu_dma_ctrl);
static dpu_error_t
host_release_access_for_dpu(struct dpu_t *dpu);
static dpu_error_t
dpu_check_wavegen_mux_status_for_dpu(struct dpu_t *dpu, uint8_t expected);

static dpu_error_t
host_get_access_for_rank(struct dpu_rank_t *rank);
static dpu_error_t
wavegen_mux_status_for_rank(struct dpu_rank_t *rank,
    uint8_t expected,
    uint8_t cmd,
    dpu_member_id_t *mask_collision,
    dpu_member_id_t mask_dpu);
static dpu_error_t
host_release_access_for_rank(struct dpu_rank_t *rank);
static dpu_error_t
dpu_check_wavegen_mux_status_for_rank(struct dpu_rank_t *rank, uint8_t expected);

static void
fetch_dma_and_wavegen_configs(uint32_t fck_frequency,
    uint8_t clock_division,
    uint8_t refresh_mode,
    struct dpu_dma_config *dma_config,
    struct dpu_wavegen_config *wavegen_config);

#define EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION 0x000103FF0F8FCFEFULL

#define REFRESH_MODE_VALUE 4

/* Bit set when the DPU has the control of the bank */
#define MUX_DPU_BANK_CTRL (1 << 0)
/* Bit set when the DPU can write to the bank */
#define MUX_DPU_WRITE_CTRL (1 << 1)
/* Bit set when the host or the DPU wrote to the bank without permission */
#define MUX_COLLISION_ERR (1 << 7)

#define WAVEGEN_MUX_HOST_EXPECTED 0x00
#define WAVEGEN_MUX_DPU_EXPECTED (MUX_DPU_BANK_CTRL | MUX_DPU_WRITE_CTRL)

/* clang-format off */

__API_SYMBOL__ const struct dpu_dma_config dma_engine_cas_config = {
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
__API_SYMBOL__ const struct dpu_dma_config dma_engine_clock_div2_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 9,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct dpu_wavegen_config waveform_generator_clock_div2_config = {
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
__API_SYMBOL__ const struct dpu_dma_config dma_engine_clock_div3_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 8,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct dpu_wavegen_config waveform_generator_clock_div3_config = {
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
__API_SYMBOL__ const struct dpu_dma_config dma_engine_clock_div4_config = {
    /* refresh_access_number computed later depending on frequency */
    .column_read_latency = 7,
    .minimal_access_number = 0,
    .default_time_origin = -4,
    .ldma_to_sdma_time_origin = -10,
    .xdma_time_start_activate = 9,
    .xdma_time_start_access = 18,
    .sdma_time_start_wb_f1 = 0,
};

__API_SYMBOL__ const struct dpu_wavegen_config waveform_generator_clock_div4_config = {
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
static void
save_enabled_dpus(struct dpu_rank_t *rank,
    bool *all_dpus_are_enabled_save,
    dpu_selected_mask_t *enabled_dpus_save,
    bool update_save)
{
    dpu_description_t desc = rank->description;
    uint8_t nr_cis = desc->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = desc->topology.nr_of_dpus_per_control_interface;
    dpu_selected_mask_t all_dpus_mask = dpu_mask_all(nr_dpus);

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        struct dpu_configuration_slice_info_t *ci_info = &rank->runtime.control_interface.slice_info[each_ci];

        all_dpus_are_enabled_save[each_ci]
            = !update_save ? ci_info->all_dpus_are_enabled : all_dpus_are_enabled_save[each_ci] & ci_info->all_dpus_are_enabled;
        enabled_dpus_save[each_ci] = !update_save ? ci_info->enabled_dpus : enabled_dpus_save[each_ci] & ci_info->enabled_dpus;

        /* Do not even talk to CIs where ALL dpus are disabled. */
        if (!ci_info->enabled_dpus)
            continue;

        ci_info->all_dpus_are_enabled = true;
        ci_info->enabled_dpus = all_dpus_mask;

        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
            DPU_GET_UNSAFE(rank, each_ci, each_dpu)->enabled = true;
        }
    }
}

__API_SYMBOL__ dpu_error_t
dpu_reset_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status;

    dpu_lock_rank(rank);

    dpu_description_t desc = rank->description;
    struct dpu_bit_config *bit_config = &desc->configuration.pcb_transformation;

    struct dpu_dma_config std_dma_config;
    struct dpu_wavegen_config wavegen_config;

    fetch_dma_and_wavegen_configs(desc->configuration.fck_frequency_in_mhz,
        desc->timings.clock_division,
        REFRESH_MODE_VALUE,
        &std_dma_config,
        &wavegen_config);

    const struct dpu_dma_config *dma_config = (rank->type == CYCLE_ACCURATE_SIMULATOR) ? &dma_engine_cas_config : &std_dma_config;

    /* All DPUs are enabled during the reset */
    bool all_dpus_are_enabled_save[DPU_MAX_NR_CIS];
    dpu_selected_mask_t enabled_dpus_save[DPU_MAX_NR_CIS];

    save_enabled_dpus(rank, all_dpus_are_enabled_save, enabled_dpus_save, false);

    FF(dpu_custom_for_rank(rank, DPU_COMMAND_EVENT_START, (dpu_custom_command_args_t)DPU_EVENT_RESET));

    FF(dpu_byte_order(rank));
    FF(dpu_soft_reset(rank, DPU_CLOCK_DIV8));
    FF(dpu_bit_config(rank, bit_config));
    FF(dpu_ci_shuffling_box_config(rank, bit_config));
    FF(dpu_soft_reset(rank, from_division_factor_to_dpu_enum(desc->timings.clock_division)));
    FF(dpu_ci_shuffling_box_config(rank, bit_config));
    FF(dpu_identity(rank));
    FF(dpu_thermal_config(rank, desc->timings.std_temperature));

    FF(dpu_carousel_config(rank, &desc->timings.carousel));

    FF(dpu_iram_repair_config(rank));
    save_enabled_dpus(rank, all_dpus_are_enabled_save, enabled_dpus_save, true);

    FF(dpu_wram_repair_config(rank));
    save_enabled_dpus(rank, all_dpus_are_enabled_save, enabled_dpus_save, true);

    FF(dpu_dma_config(rank, dma_config));
    FF(dpu_dma_shuffling_box_config(rank, bit_config));
    FF(dpu_wavegen_config(rank, &wavegen_config));

    FF(dpu_clear_debug(rank));
    FF(dpu_clear_run_bits(rank));

    FF(dpu_set_pc_mode(rank, DPU_PC_MODE_16));
    FF(dpu_set_stack_direction(rank, true));
    FF(dpu_reset_internal_state(rank));

    FF(dpu_init_mram_mux(rank));
    FF(dpu_init_groups(rank, all_dpus_are_enabled_save, enabled_dpus_save));

    FF(dpu_custom_for_rank(rank, DPU_COMMAND_ALL_SOFT_RESET, NULL));
    FF(dpu_custom_for_rank(rank, DPU_COMMAND_EVENT_END, (dpu_custom_command_args_t)DPU_EVENT_RESET));

end:
    dpu_unlock_rank(rank);
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_reset_dpu(struct dpu_t *dpu)
{
    LOG_DPU(VERBOSE, dpu, "");

    dpu_error_t status;
    struct dpu_rank_t *rank = dpu->rank;
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t member_id = dpu->dpu_id;
    dpu_selected_mask_t mask_one = dpu_mask_one(member_id);

    dpu_lock_rank(rank);

    /* There is no explicit reset command for a DPU.
     * We just stop the DPU, if it is running, and set the PCs to 0.
     */

    struct _dpu_context_t context;
    struct _dpu_description_t *description = dpu_get_description(rank);

    uint8_t nr_of_dpu_threads = description->dpu.nr_of_threads;

    context.scheduling = NULL;
    context.pcs = NULL;

    if ((context.scheduling = malloc(nr_of_dpu_threads * sizeof(*(context.scheduling)))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    if ((context.pcs = malloc(nr_of_dpu_threads * sizeof(*(context.pcs)))) == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    for (dpu_thread_t each_thread = 0; each_thread < nr_of_dpu_threads; ++each_thread) {
        context.scheduling[each_thread] = 0xFF;
    }

    context.nr_of_running_threads = 0;
    context.bkp_fault = false;
    context.dma_fault = false;
    context.mem_fault = false;

    FF(dpu_initialize_fault_process_for_dpu(dpu, &context));

    dpuinstruction_t stop_instruction = STOPci(BOOT_CC_TRUE, 0);

    uint8_t mask = CI_MASK_ONE(slice_id);
    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS];

    FF(ufi_select_dpu(rank, &mask, member_id));

    iram_array[slice_id] = &stop_instruction;
    FF(ufi_iram_write(rank, mask, iram_array, 0, 1));

    for (uint8_t each_thread = 0; each_thread < description->dpu.nr_of_threads; ++each_thread) {
        FF(ufi_thread_boot(rank, mask, each_thread, NULL));
    }

    uint8_t result[DPU_MAX_NR_CIS];
    do {
        FF(ufi_read_dpu_run(rank, mask, result));
    } while ((result[slice_id] & mask_one) != 0);

end:
    free(context.pcs);
    free(context.scheduling);
    dpu_unlock_rank(rank);
    return status;
}

static dpu_error_t
dpu_carousel_config(struct dpu_rank_t *rank, const struct dpu_carousel_config *config)
{
    LOG_RANK(VERBOSE,
        rank,
        "cmd_duration: %d cmd_sampling: %d res_duration: %d res_sampling: %d",
        config->cmd_duration,
        config->cmd_sampling,
        config->res_duration,
        config->res_sampling);

    dpu_error_t status;
    uint8_t mask = ALL_CIS;

    FF(ufi_select_all(rank, &mask));
    FF(ufi_carousel_config(rank, mask, config));

end:
    return status;
}

static dpu_error_t
dpu_ci_shuffling_box_config(struct dpu_rank_t *rank, const struct dpu_bit_config *config)
{
    LOG_RANK(
        VERBOSE, rank, "c2d: 0x%04x, d2c: 0x%04x, nibble_swap: 0x%02x", config->cpu2dpu, config->dpu2cpu, config->nibble_swap);
    dpu_error_t status;
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    FF(ufi_bit_config(rank, mask, config, NULL));

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_dma_config(struct dpu_rank_t *rank, const struct dpu_dma_config *config)
{
    LOG_RANK(VERBOSE,
        rank,
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
    dpu_error_t status;

    uint64_t dma_engine_timing = 0L;
    dma_engine_timing |= (((uint64_t)config->refresh_access_number) & 0x7Ful) << 36u;
    dma_engine_timing |= (((uint64_t)config->column_read_latency) & 0x0Ful) << 32u;
    dma_engine_timing |= (((uint64_t)config->minimal_access_number) & 0x07ul) << 29u;
    dma_engine_timing |= (((uint64_t)config->default_time_origin) & 0x7Ful) << 22u;
    dma_engine_timing |= (((uint64_t)config->ldma_to_sdma_time_origin) & 0x7Ful) << 15u;
    dma_engine_timing |= (((uint64_t)config->xdma_time_start_activate) & 0x1Ful) << 10u;
    dma_engine_timing |= (((uint64_t)config->xdma_time_start_access) & 0x1Ful) << 5u;
    dma_engine_timing |= (((uint64_t)config->sdma_time_start_wb_f1) & 0x1Ful) << 0u;

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    // Configure DMA Engine Timing
    FF(ufi_write_dma_ctrl(rank, mask, 0x20, (dma_engine_timing >> 0u) & 0xFFu));
    FF(ufi_write_dma_ctrl(rank, mask, 0x21, (dma_engine_timing >> 8u) & 0xFFu));
    FF(ufi_write_dma_ctrl(rank, mask, 0x22, (dma_engine_timing >> 16u) & 0xFFu));
    FF(ufi_write_dma_ctrl(rank, mask, 0x23, (dma_engine_timing >> 24u) & 0xFFu));
    FF(ufi_write_dma_ctrl(rank, mask, 0x24, (dma_engine_timing >> 32u) & 0xFFu));
    FF(ufi_write_dma_ctrl(rank, mask, 0x25, (dma_engine_timing >> 40u) & 0xFFu));

    // Clear DMA Engine Configuration Path and flush reg_replace_instr of readop2
    FF(ufi_clear_dma_ctrl(rank, mask));

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_dma_shuffling_box_config(struct dpu_rank_t *rank, const struct dpu_bit_config *config)
{
    LOG_RANK(VERBOSE,
        rank,
        "DMA shuffling box config: cpu_to_dpu: 0x%04x dpu_to_cpu: 0x%04x nibble_swap: 0x%02x",
        config->cpu2dpu,
        config->dpu2cpu,
        config->nibble_swap);

    dpu_error_t status;

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    // Configure Jedec Shuffling box
    FF(ufi_write_dma_ctrl(rank, mask, 0x10, (config->dpu2cpu >> 8) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, 0x11, (config->dpu2cpu >> 0) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, 0x12, (config->cpu2dpu >> 8) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, 0x13, (config->cpu2dpu >> 0) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, 0x14, config->nibble_swap));

    // Clear DMA Engine Configuration Path and flush reg_replace_instr of readop2
    FF(ufi_clear_dma_ctrl(rank, mask));

end:
    return status;
}

static dpu_error_t
dpu_wavegen_reg_config(struct dpu_rank_t *rank, uint8_t mask, uint8_t address, const struct dpu_wavegen_reg_config *config)
{
    dpu_error_t status;

    FF(ufi_write_dma_ctrl(rank, mask, address + 0, config->rise));
    FF(ufi_write_dma_ctrl(rank, mask, address + 1, config->fall));
    FF(ufi_write_dma_ctrl(rank, mask, address + 2, config->counter_enable));
    FF(ufi_write_dma_ctrl(rank, mask, address + 3, config->counter_disable));

end:
    return status;
}

static dpu_error_t
dpu_wavegen_timing_config(struct dpu_rank_t *rank, uint8_t mask, uint8_t address, const struct dpu_wavegen_timing_config *config)
{
    dpu_error_t status;

    FF(ufi_write_dma_ctrl(rank, mask, address + 0, config->activate));
    FF(ufi_write_dma_ctrl(rank, mask, address + 1, config->read));
    FF(ufi_write_dma_ctrl(rank, mask, address + 2, config->write));
    FF(ufi_write_dma_ctrl(rank, mask, address + 3, config->precharge));
    FF(ufi_write_dma_ctrl(rank, mask, address + 4, config->refresh_start));
    FF(ufi_write_dma_ctrl(rank, mask, address + 5, config->refresh_activ));
    FF(ufi_write_dma_ctrl(rank, mask, address + 6, config->refresh_prech));
    FF(ufi_write_dma_ctrl(rank, mask, address + 7, config->refresh_end));

end:
    return status;
}

static dpu_error_t
dpu_wavegen_vector_sampling_config(struct dpu_rank_t *rank,
    uint8_t mask,

    uint8_t address,
    const struct dpu_wavegen_vector_sampling_config *config)
{
    dpu_error_t status;

    FF(ufi_write_dma_ctrl(rank, mask, address + 0, config->rab_gross));
    FF(ufi_write_dma_ctrl(rank, mask, address + 1, config->cat_gross));
    FF(ufi_write_dma_ctrl(rank, mask, address + 2, config->dwbsb_gross));
    FF(ufi_write_dma_ctrl(rank, mask, address + 3, config->drbsb_gross));
    FF(ufi_write_dma_ctrl(rank, mask, address + 4, config->drbsb_fine));

end:
    return status;
}

static dpu_error_t
dpu_wavegen_rowhammer_and_refresh_config(struct dpu_rank_t *rank,
    uint8_t mask,
    uint8_t address,
    uint16_t rowhammer_info,
    uint8_t rowhammer_config)
{
    dpu_error_t status;

    FF(ufi_write_dma_ctrl(rank, mask, address + 0, (rowhammer_info >> 0) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, address + 1, (rowhammer_info >> 8) & 0xFF));
    FF(ufi_write_dma_ctrl(rank, mask, address + 2, rowhammer_config));

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_wavegen_config(struct dpu_rank_t *rank, const struct dpu_wavegen_config *config)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    FF(dpu_wavegen_reg_config(rank, mask, 0xC0, &config->MCBAB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xC4, &config->RCYCLKB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xC8, &config->WCYCLKB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xCC, &config->DWCLKB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xD0, &config->DWAEB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xD4, &config->DRAPB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xD8, &config->DRAOB));
    FF(dpu_wavegen_reg_config(rank, mask, 0xDC, &config->DWBSB_EN));

    FF(dpu_wavegen_timing_config(rank, mask, 0xE0, &config->timing_completion));
    FF(dpu_wavegen_vector_sampling_config(rank, mask, 0xE8, &config->vector_sampling));
    FF(dpu_wavegen_rowhammer_and_refresh_config(
        rank, mask, 0xFC, config->refresh_and_row_hammer_info, config->row_hammer_config));

    for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
        // Configuration MUST start with dpu_id = 4 because of refresh
        dpu_member_id_t target_dpu;

        if (rank->description->topology.nr_of_dpus_per_control_interface <= 4)
            target_dpu = each_dpu;
        else
            target_dpu = (each_dpu >= 4) ? (each_dpu - 4) : (each_dpu + 4);

        mask = ALL_CIS;
        FF(ufi_select_dpu(rank, &mask, target_dpu));
        FF(ufi_write_dma_ctrl(rank, mask, 0x83, 0x01));
        FF(ufi_clear_dma_ctrl(rank, mask));
    }

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_wavegen_read_status(struct dpu_t *dpu, uint8_t address, uint8_t *value)
{
    LOG_DPU(VERBOSE, dpu, "%d", address);

    if (!dpu->enabled) {
        return DPU_ERR_DPU_DISABLED;
    }

    dpu_error_t status = DPU_OK;

    struct dpu_rank_t *rank = dpu->rank;
    uint8_t mask = CI_MASK_ONE(dpu->slice_id);
    uint8_t results[DPU_MAX_NR_CIS];

    dpu_lock_rank(rank);

    FF(ufi_select_dpu(rank, &mask, dpu->dpu_id));
    FF(ufi_write_dma_ctrl(rank, mask, 0xFF, address & 3));
    FF(ufi_clear_dma_ctrl(rank, mask));
    FF(ufi_read_dma_ctrl(rank, mask, results));

    *value = results[dpu->slice_id];

end:
    dpu_unlock_rank(rank);
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

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    dpu_slice_id_t slice_id = dpu->slice_id;
    dpu_member_id_t dpu_id = dpu->dpu_id;

    dpu_member_id_t dpu_pair_base_id = (dpu_member_id_t)(dpu_id & ~1);
    struct dpu_t *pair_base_dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_pair_base_id);

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        return DPU_ERR_MRAM_BUSY;
    }

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

    FF(host_release_access_for_dpu(pair_base_dpu));

    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface) {
        struct dpu_t *paired_dpu = DPU_GET_UNSAFE(rank, slice_id, (dpu_member_id_t)(dpu_pair_base_id + 1));
        FF(host_release_access_for_dpu(paired_dpu));
    }

end:
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

    dpu_error_t status;
    dpu_member_id_t dpu_pair_base_id = (dpu_member_id_t)(dpu_id & ~1);
    struct dpu_t *pair_base_dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_pair_base_id);

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        return DPU_ERR_MRAM_BUSY;
    }

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

    FF(host_get_access_for_dpu(pair_base_dpu));

    if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface) {
        struct dpu_t *paired_dpu = DPU_GET_UNSAFE(rank, slice_id, (dpu_member_id_t)(dpu_pair_base_id + 1));
        FF(host_get_access_for_dpu(paired_dpu));
    }

end:
    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_host_release_access_for_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status = DPU_OK;

    dpu_lock_rank(rank);

    dpu_description_t desc = rank->description;
    dpu_member_id_t dpu_pair_base_id;
    uint8_t nr_cis = desc->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = desc->topology.nr_of_dpus_per_control_interface;

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        status = DPU_ERR_MRAM_BUSY;
        goto end;
    }

    /* Stores the state of the mux in case the debugger intervenes: we record the state before
     * actually placing the mux in this state. If we did record the state after setting the mux,
     * we could be interrupted between the setting of the mux and the recording of the state, and
     * then the debugger would miss a mux state.
     */
    for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
            dpu_pair_base_id = (dpu_member_id_t)(each_dpu & ~1);
            __dpu_clear_host_mux_mram_state(rank, each_slice, dpu_pair_base_id);
            if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface)
                __dpu_clear_host_mux_mram_state(rank, each_slice, dpu_pair_base_id + 1);
        }
    }

    if (!rank->description->configuration.api_must_switch_mram_mux && !rank->description->configuration.init_mram_mux) {
        /*
         * Yes, this is not an error, that just means that the following configuration should not be done since
         * it is useless and incorrect.
         */
        status = DPU_OK;
        goto end;
    }

    FF(host_release_access_for_rank(rank));

end:
    dpu_unlock_rank(rank);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_host_get_access_for_rank(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status = DPU_OK;

    dpu_lock_rank(rank);

    dpu_description_t desc = rank->description;
    dpu_member_id_t dpu_pair_base_id;
    uint8_t nr_cis = desc->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = desc->topology.nr_of_dpus_per_control_interface;

    if (rank->runtime.run_context.nb_dpu_running > 0) {
        LOG_RANK(WARNING,
            rank,
            "Host can not get access to the MRAM because %u DPU%s running.",
            rank->runtime.run_context.nb_dpu_running,
            rank->runtime.run_context.nb_dpu_running > 1 ? "s are" : " is");
        status = DPU_ERR_MRAM_BUSY;
        goto end;
    }

    /* Stores the state of the mux in case the debugger intervenes: we record the state before
     * actually placing the mux in this state. If we did record the state after setting the mux,
     * we could be interrupted between the setting of the mux and the recording of the state, and
     * then the debugger would miss a mux state.
     */
    for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
            dpu_pair_base_id = (dpu_member_id_t)(each_dpu & ~1);
            __dpu_set_host_mux_mram_state(rank, each_slice, dpu_pair_base_id);
            if ((dpu_pair_base_id + 1) < rank->description->topology.nr_of_dpus_per_control_interface)
                __dpu_set_host_mux_mram_state(rank, each_slice, dpu_pair_base_id + 1);
        }
    }

    if (!rank->description->configuration.api_must_switch_mram_mux && !rank->description->configuration.init_mram_mux) {
        /*
         * Yes, this is not an error, that just means that the following configuration should not be done since
         * it is useless and incorrect.
         */
        status = DPU_OK;
        goto end;
    }

    FF(host_get_access_for_rank(rank));

end:
    dpu_unlock_rank(rank);

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
dpu_byte_order(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;
    uint64_t byte_order_results[DPU_MAX_NR_CIS];
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    FF(ufi_byte_order(rank, mask, byte_order_results));

    for (dpu_slice_id_t slice_id = 0; slice_id < rank->description->topology.nr_of_control_interfaces; ++slice_id) {
        if (!CI_MASK_ON(mask, slice_id))
            continue;

        if (!byte_order_values_are_compatible(byte_order_results[slice_id], EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION)) {
            LOG_CI(WARNING,
                rank,
                slice_id,
                "ERROR: invalid byte order (reference: 0x%016lx; found: 0x%016lx)",
                EXPECTED_BYTE_ORDER_RESULT_AFTER_CONFIGURATION,
                byte_order_results[slice_id]);
            status = DPU_ERR_DRIVER;
        }
    }

end:
    return status;
}

static const char *
clock_division_to_string(dpu_clock_division_t clock_division)
{
    switch (clock_division) {
        case DPU_CLOCK_DIV8:
            return "CLOCK DIV 8";
        case DPU_CLOCK_DIV4:
            return "CLOCK DIV 4";
        case DPU_CLOCK_DIV3:
            return "CLOCK DIV 3";
        case DPU_CLOCK_DIV2:
            return "CLOCK DIV 2";
        default:
            return "CLOCK DIV UNKNOWN";
    }
}

static dpu_error_t
dpu_soft_reset(struct dpu_rank_t *rank, dpu_clock_division_t clock_division)
{
    LOG_RANK(VERBOSE, rank, "%s", clock_division_to_string(clock_division));
    dpu_error_t status;

    uint8_t cycle_accurate = rank->description->configuration.enable_cycle_accurate_behavior ? 1 : 0;
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    ufi_soft_reset(rank, mask, clock_division, cycle_accurate);

end:
    return status;
}

static dpu_error_t
dpu_bit_config(struct dpu_rank_t *rank, struct dpu_bit_config *config)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;

    uint32_t bit_config_results[DPU_MAX_NR_CIS];
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    FF(ufi_bit_config(rank, mask, NULL, bit_config_results));

    uint32_t bit_config_result = bit_config_results[__builtin_ctz(mask)];

    /* Let's verify that all CIs have the bit config result as the first CI. */
    for (dpu_slice_id_t slice_id = 0; slice_id < rank->description->topology.nr_of_control_interfaces; ++slice_id) {
        if (!CI_MASK_ON(mask, slice_id))
            continue;

        if (bit_config_results[slice_id] != bit_config_result) {
            LOG_RANK(WARNING,
                rank,
                "inconsistent bit configuration between the different CIs (0x%08x != 0x%08x)",
                bit_config_results[slice_id],
                bit_config_result);
            status = DPU_ERR_INTERNAL;
            goto end;
        }
    }

    dpu_bit_config_compute(bit_config_result, config);

    config->stutter = 0;

    LOG_RANK(DEBUG,
        rank,
        "bit_order: 0x%08x nibble_swap: 0x%02x cpu_to_dpu: 0x%04x dpu_to_cpu: 0x%04x",
        bit_config_result,
        config->nibble_swap,
        config->cpu2dpu,
        config->dpu2cpu);

end:
    return status;
}

static dpu_error_t
dpu_identity(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;
    uint32_t identity_results[DPU_MAX_NR_CIS];
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    FF(ufi_identity(rank, mask, identity_results));

    uint32_t identity_result = identity_results[__builtin_ctz(mask)];
    for (dpu_slice_id_t slice_id = 0; slice_id < rank->description->topology.nr_of_control_interfaces; ++slice_id) {
        if (!CI_MASK_ON(mask, slice_id))
            continue;

        if (identity_results[slice_id] != identity_result) {
            LOG_RANK(WARNING,
                rank,
                "inconsistent identity between the different CIs (0x%08x != 0x%08x)",
                identity_results[slice_id],
                identity_result);
            status = DPU_ERR_INTERNAL;
            goto end;
        }
    }

    if (identity_result != rank->description->signature.chip_id) {
        LOG_RANK(WARNING,
            rank,
            "ERROR: invalid identity (expected: 0x%08x; found: 0x%08x)",
            rank->description->signature.chip_id,
            identity_result);
        status = DPU_ERR_INTERNAL;
    }

end:
    return status;
}

static enum dpu_temperature
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
dpu_thermal_config(struct dpu_rank_t *rank, uint8_t thermal_config)
{
    enum dpu_temperature temperature = from_celsius_to_dpu_enum(thermal_config);
    dpu_error_t status;
    LOG_RANK(VERBOSE, rank, "%d°C (value: 0x%04x)", thermal_config, temperature);
    uint8_t mask = ALL_CIS;

    FF(ufi_select_cis(rank, &mask));
    ufi_thermal_config(rank, mask, temperature);
end:
    return status;
}

static dpu_error_t
dpu_init_groups(struct dpu_rank_t *rank, const bool *all_dpus_are_enabled_save, const dpu_selected_mask_t *enabled_dpus_save)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status = DPU_OK;

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

    uint8_t ci_mask = 0;

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        if (all_dpus_are_enabled_save[each_ci]) {
            ci_mask |= CI_MASK_ONE(each_ci);
        } else if (enabled_dpus_save[each_ci]) {
            for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
                uint8_t single_ci_mask = CI_MASK_ONE(each_ci);
                uint8_t group = ((enabled_dpus_save[each_ci] & (1 << each_dpu)) != 0) ? DPU_ENABLED_GROUP : DPU_DISABLED_GROUP;
                FF(ufi_select_dpu(rank, &single_ci_mask, each_dpu));
                FF(ufi_write_group(rank, single_ci_mask, group));
            }
        }
    }

    if (ci_mask != 0) {
        FF(ufi_select_all(rank, &ci_mask));
        FF(ufi_write_group(rank, ci_mask, DPU_ENABLED_GROUP));
    }

    /* Set the rank context with the saved context */
    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        struct dpu_configuration_slice_info_t *ci_info = &rank->runtime.control_interface.slice_info[each_ci];

        ci_info->all_dpus_are_enabled = all_dpus_are_enabled_save[each_ci];
        ci_info->enabled_dpus = enabled_dpus_save[each_ci];

        for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus_per_ci; ++each_dpu) {
            DPU_GET_UNSAFE(rank, each_ci, each_dpu)->enabled = (ci_info->enabled_dpus & (1 << each_dpu)) != 0;
        }
    }

end:
    return status;
}

// TODO: Should find a way to tell upper layers that iram repair took place if any.
/* This function fails if only ONE DPU is impossible to repair. */
static dpu_error_t
dpu_iram_repair_config(struct dpu_rank_t *rank)
{
    dpu_error_t status;

    if (rank->description->configuration.do_iram_repair) {
        LOG_RANK(VERBOSE, rank, "IRAM repair enabled");
        // SRAM defects were filled at rank allocation
        FF(try_to_repair_iram(rank));
    } else {
        // Default configuration
        LOG_RANK(VERBOSE, rank, "IRAM repair disabled");
        struct dpu_repair_config config = { 0, 0, 0, 0, 0, 0, 0, 1 };
        struct dpu_repair_config *config_array[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = &config };

        uint8_t mask = ALL_CIS;
        FF(ufi_select_all(rank, &mask));
        FF(ufi_iram_repair_config(rank, mask, config_array));
    }

end:
    return status;
}

static dpu_error_t
dpu_wram_repair_config(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;

    if (rank->description->configuration.do_wram_repair) {
        LOG_RANK(VERBOSE, rank, "WRAM repair enabled");
        // SRAM defects were filled at rank allocation
        FF(try_to_repair_wram(rank));
    } else {
        // Default configuration
        LOG_RANK(VERBOSE, rank, "WRAM repair disabled");
        struct dpu_repair_config config = { 0, 0, 0, 0, 0, 0, 0, 1 };
        struct dpu_repair_config *config_array[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = &config };
        uint8_t mask = ALL_CIS;
        FF(ufi_select_all(rank, &mask));
        for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
            FF(ufi_wram_repair_config(rank, mask, each_wram_bank, config_array));
        }
    }

end:
    return status;
}

static dpu_error_t
dpu_clear_debug(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    FF(ufi_clear_debug_replace(rank, mask));
    FF(ufi_clear_fault_poison(rank, mask));
    FF(ufi_clear_fault_bkp(rank, mask));
    FF(ufi_clear_fault_dma(rank, mask));
    FF(ufi_clear_fault_mem(rank, mask));
    FF(ufi_clear_fault_dpu(rank, mask));
    FF(ufi_clear_fault_intercept(rank, mask));

end:
    return status;
}

static const char *
pc_mode_to_string(enum dpu_pc_mode pc_mode)
{
    switch (pc_mode) {
        case DPU_PC_MODE_12:
            return "12";
        case DPU_PC_MODE_13:
            return "13";
        case DPU_PC_MODE_14:
            return "14";
        case DPU_PC_MODE_15:
            return "15";
        case DPU_PC_MODE_16:
            return "16";
        default:
            return "UNKNOWN";
    }
}

static dpu_error_t
dpu_set_pc_mode(struct dpu_rank_t *rank, enum dpu_pc_mode pc_mode)
{
    LOG_RANK(VERBOSE, rank, "%s", pc_mode_to_string(pc_mode));
    dpu_error_t status;
    enum dpu_pc_mode pc_modes[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = pc_mode };

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));
    FF(ufi_set_pc_mode(rank, mask, pc_modes));

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

    for (dpu_member_id_t dpu_id = 0; dpu_id < nr_dpus_per_ci; ++dpu_id) {
        mask = ALL_CIS;
        FF(ufi_select_dpu(rank, &mask, dpu_id));
        FF(ufi_get_pc_mode(rank, mask, pc_modes));

        for (dpu_slice_id_t slice_id = 0; slice_id < nr_cis; ++slice_id) {
            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_id);
            if (dpu->enabled) {
                if (pc_modes[dpu_id] != pc_mode) {
                    LOG_DPU(WARNING, dpu, "ERROR: invalid PC mode (expected: %d, found: %d)", pc_mode, pc_modes[dpu_id]);
                    status = DPU_ERR_INTERNAL;
                    break;
                }
            }
        }
    }

end:
    return status;
}

static dpu_error_t
dpu_clear_run_bits(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");

    dpu_error_t status;

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));

    uint32_t nr_run_bits = rank->description->dpu.nr_of_threads + rank->description->dpu.nr_of_notify_bits;

    for (uint32_t each_bit = 0; each_bit < nr_run_bits; ++each_bit) {
        FF(ufi_clear_run_bit(rank, mask, each_bit, NULL));
    }

end:
    return status;
}

static inline const char *
stack_direction_to_string(bool stack_is_up)
{
    return stack_is_up ? "up" : "down";
}

static dpu_error_t
dpu_set_stack_direction(struct dpu_rank_t *rank, bool stack_is_up)
{
    LOG_RANK(VERBOSE, rank, "%s", stack_direction_to_string(stack_is_up));
    dpu_error_t status;
    uint8_t previous_directions[DPU_MAX_NR_CIS];
    bool stack_directions[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = stack_is_up };

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));
    FF(ufi_set_stack_direction(rank, mask, stack_directions, NULL));
    FF(ufi_set_stack_direction(rank, mask, stack_directions, previous_directions));

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus_per_ci = rank->description->topology.nr_of_dpus_per_control_interface;

    for (dpu_slice_id_t slice_id = 0; slice_id < nr_cis; ++slice_id) {
        for (dpu_member_id_t dpu_id = 0; dpu_id < nr_dpus_per_ci; ++dpu_id) {
            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, slice_id, dpu_id);
            if (dpu->enabled) {
                bool stack_if_effectively_up = dpu_mask_is_selected(previous_directions[slice_id], dpu_id);
                if (stack_if_effectively_up != stack_is_up) {
                    LOG_DPU(WARNING,
                        dpu,
                        "ERROR: invalid stack mode (expected: %s, found: %s)",
                        stack_direction_to_string(stack_is_up),
                        stack_direction_to_string(stack_if_effectively_up));
                    status = DPU_ERR_INTERNAL;
                }
            }
        }
    }

end:
    return status;
}

static dpu_error_t
dpu_reset_internal_state(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;
    iram_size_t internal_state_reset_size;
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_threads = rank->description->dpu.nr_of_threads;
    dpuinstruction_t *internal_state_reset = fetch_internal_reset_program(&internal_state_reset_size);

    if (internal_state_reset == NULL) {
        status = DPU_ERR_SYSTEM;
        goto end;
    }

    dpuinstruction_t *iram_array[DPU_MAX_NR_CIS] = { [0 ... DPU_MAX_NR_CIS - 1] = internal_state_reset };

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));
    FF(ufi_iram_write(rank, mask, iram_array, 0, internal_state_reset_size));

    for (dpu_thread_t each_thread = 0; each_thread < nr_threads; ++each_thread) {
        FF(ufi_thread_boot(rank, mask, each_thread, NULL));
    }

    uint8_t mask_all = (1 << nr_dpus) - 1;
    uint8_t state[DPU_MAX_NR_CIS];
    bool running;
    do {
        FF(ufi_read_dpu_run(rank, mask, state));

        running = false;
        for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            if (!CI_MASK_ON(mask, each_ci))
                continue;
            running = running || ((state[each_ci] & mask_all) != 0);
        }
    } while (running);

end:
    free(internal_state_reset);
    return status;
}

static dpu_error_t
dpu_init_mram_mux(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;

    if (!rank->description->configuration.api_must_switch_mram_mux && !rank->description->configuration.init_mram_mux) {
        return DPU_OK;
    }

    uint8_t mask = ALL_CIS;
    FF(ufi_select_all(rank, &mask));
    FF(ufi_set_mram_mux(rank, mask, true));

    for (dpu_slice_id_t each_ci = 0; each_ci < rank->description->topology.nr_of_control_interfaces; ++each_ci) {
        if (!CI_MASK_ON(mask, each_ci))
            continue;

        for (dpu_member_id_t each_dpu = 0; each_dpu < rank->description->topology.nr_of_dpus_per_control_interface; ++each_dpu) {
            FF(dpu_check_wavegen_mux_status_for_dpu(DPU_GET_UNSAFE(rank, each_ci, each_dpu), WAVEGEN_MUX_DPU_EXPECTED));
        }
    }

end:
    return status;
}

static dpu_error_t
dpu_check_wavegen_mux_status_for_dpu(struct dpu_t *dpu, uint8_t expected)
{
    LOG_DPU(VERBOSE, dpu, "");
    dpu_error_t status;
    uint8_t dpu_dma_ctrl = 0;

    FF(wavegen_mux_status_for_dpu(dpu, expected, 0x02, &dpu_dma_ctrl));

    if (dpu_dma_ctrl & MUX_COLLISION_ERR) {
        LOG_DPU(WARNING, dpu, "MRAM collision detected, either host or DPU wrote in the MRAM without permission. Clearing it...");
        FF(wavegen_mux_status_for_dpu(dpu, expected, 0x82, &dpu_dma_ctrl));
    }

end:
    return status;
}

static dpu_error_t
host_handle_access_for_dpu(struct dpu_t *dpu, bool set_mux_for_dpu)
{
    dpu_error_t status;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    uint8_t ci_mask = CI_MASK_ONE(dpu->slice_id);

    FF(ufi_select_dpu_even_disabled(rank, &ci_mask, dpu->dpu_id));
    FF(ufi_set_mram_mux(rank, ci_mask, set_mux_for_dpu));

    FF(dpu_check_wavegen_mux_status_for_dpu(dpu, set_mux_for_dpu ? WAVEGEN_MUX_DPU_EXPECTED : WAVEGEN_MUX_HOST_EXPECTED));

end:
    return status;
}

static dpu_error_t
host_release_access_for_dpu(struct dpu_t *dpu)
{
    return host_handle_access_for_dpu(dpu, true);
}

static dpu_error_t
host_get_access_for_dpu(struct dpu_t *dpu)
{
    return host_handle_access_for_dpu(dpu, false);
}

static dpu_error_t
dpu_check_wavegen_mux_status_for_rank(struct dpu_rank_t *rank, uint8_t expected)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status;
    dpu_member_id_t mask_collision = 0;

    FF(wavegen_mux_status_for_rank(rank, expected, 0x02, &mask_collision, 0));

    if (mask_collision) {
        LOG_RANK(
            WARNING, rank, "MRAM collision detected, either host or DPU wrote in the MRAM without permission. Clearing it...");
        FF(wavegen_mux_status_for_rank(rank, expected, 0x82, NULL, mask_collision));
    }

end:
    return status;
}

static dpu_error_t
host_handle_access_for_rank(struct dpu_rank_t *rank, bool set_mux_for_dpu)
{
    dpu_error_t status;

    uint8_t mask = ALL_CIS;

    FF(ufi_select_all(rank, &mask));
    FF(ufi_set_mram_mux(rank, mask, set_mux_for_dpu));

    FF(dpu_check_wavegen_mux_status_for_rank(rank, set_mux_for_dpu ? WAVEGEN_MUX_DPU_EXPECTED : WAVEGEN_MUX_HOST_EXPECTED));

end:
    return status;
}

static dpu_error_t
host_release_access_for_rank(struct dpu_rank_t *rank)
{
    return host_handle_access_for_rank(rank, true);
}

static dpu_error_t
host_get_access_for_rank(struct dpu_rank_t *rank)
{
    return host_handle_access_for_rank(rank, false);
}

#define TIMEOUT_MUX_STATUS (10000)

static dpu_error_t
wavegen_mux_status_for_dpu(struct dpu_t *dpu, uint8_t expected, uint8_t cmd, uint8_t *ret_dpu_dma_ctrl)
{
    dpu_error_t status;
    uint8_t dpu_dma_ctrl;
    uint8_t result_array[DPU_MAX_NR_CIS];
    uint32_t timeout = TIMEOUT_MUX_STATUS;

    struct dpu_rank_t *rank = dpu_get_rank(dpu);
    uint8_t ci_mask = CI_MASK_ONE(dpu->slice_id);

    FF(ufi_select_dpu_even_disabled(rank, &ci_mask, dpu->dpu_id));

    // Check Mux control through dma_rdat_ctrl of fetch1
    // 1 - Select WaveGen Read register @0xFF and set it @0x02  (mux and collision ctrl)
    // 2 - Flush readop2 (Pipeline to DMA cfg data path)
    // 3 - Read dpu_dma_ctrl
    FF(ufi_write_dma_ctrl(rank, ci_mask, 0xFF, cmd));
    FF(ufi_clear_dma_ctrl(rank, ci_mask));

    do {
        FF(ufi_read_dma_ctrl(rank, ci_mask, result_array));
        dpu_dma_ctrl = result_array[dpu->slice_id];
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

end:
    return status;
}

static dpu_error_t
wavegen_mux_status_for_rank(struct dpu_rank_t *rank,
    uint8_t expected,
    uint8_t cmd,
    dpu_member_id_t *mask_collision,
    dpu_member_id_t mask_dpu)
{
    dpu_error_t status;
    uint8_t dpu_dma_ctrl;
    uint8_t result_array[DPU_MAX_NR_CIS];
    uint32_t timeout;
    uint8_t ci_mask = ALL_CIS, mask;
    uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    bool should_retry;

    /* ci_mask retains the real disabled CIs, whereas mask does not take
     * care of disabled dpus (and then CIs) since it should switch mux of
     * disabled dpus: but not in the case a CI is completely deactivated.
     */

    // Check Mux control through dma_rdat_ctrl of fetch1
    // 1 - Select WaveGen Read register @0xFF and set it @0x02  (mux and collision ctrl)
    // 2 - Flush readop2 (Pipeline to DMA cfg data path)
    // 3 - Read dpu_dma_ctrl
    ci_mask = ALL_CIS;
    FF(ufi_select_all(rank, &ci_mask));
    FF(ufi_write_dma_ctrl(rank, ci_mask, 0xFF, cmd));
    FF(ufi_clear_dma_ctrl(rank, ci_mask));

    for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
        dpu_selected_mask_t mask_one = dpu_mask_one(each_dpu);

        if (mask_dpu && !(mask_dpu & mask_one))
            continue;

        timeout = TIMEOUT_MUX_STATUS;

        do {
            should_retry = false;

            mask = ALL_CIS;
            FF(ufi_select_dpu_even_disabled(rank, &mask, each_dpu));
            FF(ufi_read_dma_ctrl(rank, mask, result_array));

            for (dpu_slice_id_t each_slice = 0; each_slice < nr_cis; ++each_slice) {
                if (!CI_MASK_ON(ci_mask, each_slice))
                    continue;

                dpu_dma_ctrl = result_array[each_slice];

                if ((dpu_dma_ctrl & 0x7F) != expected)
                    should_retry = true;

                /* In case of collision, only save the dpu, not the CI, since
                 * we have to write to all CIs at once anyway.
                 */
                if (mask_collision && (dpu_dma_ctrl & MUX_COLLISION_ERR))
                    *mask_collision = *mask_collision | mask_one;
            }

            timeout--;
        } while (timeout && should_retry); // Do not check Collision Error bit

        if (!timeout) {
            LOG_RANK(WARNING, rank, "Timeout waiting for result to be correct");
            return rank->description->configuration.disable_api_safe_checks ? DPU_OK : DPU_ERR_TIMEOUT;
        }
    }

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
    struct dpu_repair_config *repair_config)
{
    repair_config->AB_msbs = 0xFF;
    repair_config->CD_msbs = 0xFF;
    repair_config->A_lsbs = 0xFF;
    repair_config->B_lsbs = 0xFF;
    repair_config->C_lsbs = 0xFF;
    repair_config->D_lsbs = 0xFF;
    repair_config->even_index = 0xFF;
    repair_config->odd_index = 0xFF;

    uint32_t nr_of_corrupted_addr = repair_info->nr_of_corrupted_addresses;

    LOG_DPU(DEBUG, dpu, "repair info: number of corrupted addresses: %d", nr_of_corrupted_addr);

    for (uint32_t each_corrupted_addr_index = 0; each_corrupted_addr_index < nr_of_corrupted_addr; ++each_corrupted_addr_index) {
        LOG_DPU(DEBUG,
            dpu,
            "repair info: #%d address: 0x%04x faulty_bits: 0x%016lx",
            each_corrupted_addr_index,
            repair_info->corrupted_addresses[each_corrupted_addr_index].address,
            repair_info->corrupted_addresses[each_corrupted_addr_index].faulty_bits);
    }

    if (nr_of_corrupted_addr > NB_MAX_REPAIR_ADDR) {
        LOG_DPU(WARNING, dpu, "ERROR: too many corrupted addresses (%d > %d)", nr_of_corrupted_addr, NB_MAX_REPAIR_ADDR);
        return false;
    }

    for (uint32_t each_corrupted_addr_index = 0; each_corrupted_addr_index < nr_of_corrupted_addr; ++each_corrupted_addr_index) {
        uint32_t address = repair_info->corrupted_addresses[each_corrupted_addr_index].address;
        uint8_t msbs = (uint8_t)(address >> 4);
        uint8_t lsbs = (uint8_t)(address & 0xF);

        if (repair_config->A_lsbs == 0xFF) {
            repair_config->AB_msbs = msbs;
            repair_config->A_lsbs = lsbs;
        } else if ((repair_config->B_lsbs == 0xFF) && (repair_config->AB_msbs == msbs)) {
            repair_config->B_lsbs = lsbs;
        } else if (repair_config->C_lsbs == 0xFF) {
            repair_config->CD_msbs = msbs;
            repair_config->C_lsbs = lsbs;
        } else if ((repair_config->D_lsbs == 0xFF) && (repair_config->CD_msbs == msbs)) {
            repair_config->D_lsbs = lsbs;
        } else {
            LOG_DPU(WARNING, dpu, "ERROR: corrupted addresses are too far apart");
            return false;
        }
    }

    dpuinstruction_t faulty_bits = 0L;

    for (uint32_t each_faulty_address = 0; each_faulty_address < nr_of_corrupted_addr; ++each_faulty_address) {
        faulty_bits |= repair_info->corrupted_addresses[each_faulty_address].faulty_bits;
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
                repair_config->odd_index = last_index;
            } else {
                repair_config->even_index = last_index;
            }
            // FALLTHROUGH
        case 1:
            first_index = find_index_of_first_faulty_bit(faulty_bits);

            if ((first_index & 1) == 1) {
                if (repair_config->odd_index != 0xFF) {
                    LOG_DPU(WARNING, dpu, "ERROR: both corrupted bit indices are odd");
                    return false;
                }

                repair_config->odd_index = first_index;
            } else {
                if (repair_config->even_index != 0xFF) {
                    LOG_DPU(WARNING, dpu, "ERROR: both corrupted bit indices are even");
                    return false;
                }

                repair_config->even_index = first_index;
            }
            // FALLTHROUGH
        case 0:
            break;
    }

    // We can repair the memory! Let's choose default values then return the configuration.
    if (repair_config->A_lsbs == 0xFF) {
        repair_config->A_lsbs = 0xF;
    }
    if (repair_config->B_lsbs == 0xFF) {
        repair_config->B_lsbs = 0xF;
    }
    if (repair_config->C_lsbs == 0xFF) {
        repair_config->C_lsbs = 0xF;
    }
    if (repair_config->D_lsbs == 0xFF) {
        repair_config->D_lsbs = 0xF;
    }

    if (repair_config->even_index == 0xFF) {
        repair_config->even_index = 0;
    }

    if (repair_config->odd_index == 0xFF) {
        repair_config->odd_index = 1;
    }

    LOG_DPU(DEBUG,
        dpu,
        "valid repair config: AB_MSBs: 0x%02x A_LSBs: 0x%01x B_LSBs: 0x%01x CD_MSBs: 0x%02x C_LSBs: 0x%01x D_LSBs: 0x%01x",
        repair_config->AB_msbs,
        repair_config->A_lsbs,
        repair_config->B_lsbs,
        repair_config->CD_msbs,
        repair_config->C_lsbs,
        repair_config->D_lsbs);

    return true;
}

static dpu_error_t
try_to_repair_iram(struct dpu_rank_t *rank)
{
    dpu_error_t status = DPU_OK;

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;

    struct dpu_repair_config configs[DPU_MAX_NR_CIS];
    struct dpu_repair_config *config_array[DPU_MAX_NR_CIS];

    struct dpu_configuration_slice_info_t *ci_infos = rank->runtime.control_interface.slice_info;
    bool all_disabled = true;

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        config_array[each_ci] = &configs[each_ci];
    }

    for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
        uint8_t ci_mask = ALL_CIS;

        for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

            if (!extract_memory_repair_configuration(dpu, &dpu->repair.iram_repair, config_array[each_ci])) {
                LOG_DPU(WARNING, dpu, "ERROR: cannot repair IRAM");
                dpu->repair.iram_repair.fail_to_repair = true;
                dpu->enabled = false;
                ci_infos[each_ci].enabled_dpus &= ~(1 << each_dpu);
                ci_infos[each_ci].all_dpus_are_enabled = false;
            }
        }

        FF(ufi_select_dpu(rank, &ci_mask, each_dpu));
        FF(ufi_iram_repair_config(rank, ci_mask, config_array));
    }

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        if (ci_infos[each_ci].enabled_dpus) {
            all_disabled = false;
            break;
        }
    }

    if (all_disabled) {
        LOG_RANK(WARNING, rank, "No enabled dpus in this rank due to memory corruption.");
        return DPU_ERR_CORRUPTED_MEMORY;
    }

end:
    return status;
}

static dpu_error_t
try_to_repair_wram(struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_error_t status = DPU_OK;

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;

    struct dpu_repair_config configs[DPU_MAX_NR_CIS];
    struct dpu_repair_config *config_array[DPU_MAX_NR_CIS];

    struct dpu_configuration_slice_info_t *ci_infos = rank->runtime.control_interface.slice_info;
    bool all_disabled = true;

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        config_array[each_ci] = &configs[each_ci];
    }

    for (dpu_member_id_t each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
        for (uint8_t each_wram_bank = 0; each_wram_bank < NR_OF_WRAM_BANKS; ++each_wram_bank) {
            uint8_t ci_mask = ALL_CIS;

            for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
                struct dpu_t *dpu = DPU_GET_UNSAFE(rank, each_ci, each_dpu);

                if (!extract_memory_repair_configuration(dpu, &dpu->repair.wram_repair[each_wram_bank], config_array[each_ci])) {
                    LOG_DPU(WARNING, dpu, "ERROR: cannot repair WRAM bank #%d", each_wram_bank);
                    dpu->repair.wram_repair[each_wram_bank].fail_to_repair = true;
                    dpu->enabled = false;
                    ci_infos[each_ci].enabled_dpus &= ~(1 << each_dpu);
                    ci_infos[each_ci].all_dpus_are_enabled = false;
                }
            }

            FF(ufi_select_dpu(rank, &ci_mask, each_dpu));
            FF(ufi_wram_repair_config(rank, ci_mask, each_wram_bank, config_array));
        }
    }

    for (dpu_slice_id_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        if (ci_infos[each_ci].enabled_dpus) {
            all_disabled = false;
            break;
        }
    }

    if (all_disabled) {
        LOG_RANK(WARNING, rank, "No enabled dpus in this rank due to memory corruption.");
        return DPU_ERR_CORRUPTED_MEMORY;
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
fill_timing_configuration(struct dpu_dma_config *dma_configuration,
    struct dpu_wavegen_config *wavegen_configuration,
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
    struct dpu_dma_config *dma_config,
    struct dpu_wavegen_config *wavegen_config)
{
    dpu_clock_division_t clock_div = from_division_factor_to_dpu_enum(clock_division);

    const struct dpu_dma_config *reference_dma_config;
    const struct dpu_wavegen_config *reference_wavegen_config;

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
