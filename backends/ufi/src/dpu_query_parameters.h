/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_QUERY_PARAMETERS_H
#define UFI_DPU_QUERY_PARAMETERS_H

#include <stdint.h>

typedef struct _dpu_query_bit_order_parameters_t {
    uint16_t d2c;
    uint16_t c2d;
    uint8_t ne;
    uint8_t stutter;
} * dpu_query_bit_order_parameters_t;

typedef struct _dpu_query_software_reset_parameters_t {
    dpu_clock_division_t factor;
    uint8_t cycle_accurate;
} * dpu_query_software_reset_parameters_t;

typedef struct _dpu_query_thermal_configuration_parameters_t {
    dpu_temperature_e threshold;
} * dpu_query_thermal_configuration_parameters_t;

typedef struct _dpu_query_command_duration_configuration_parameters_t {
    uint8_t duration;
} * dpu_query_command_duration_configuration_parameters_t;

typedef struct _dpu_query_result_duration_configuration_parameters_t {
    uint8_t duration;
} * dpu_query_result_duration_configuration_parameters_t;

typedef struct _dpu_query_result_sampling_configuration_parameters_t {
    uint8_t sampling;
} * dpu_query_result_sampling_configuration_parameters_t;

typedef struct _dpu_query_select_dpu_parameters_t {
    dpu_member_id_t dpu_id;
} * dpu_query_select_dpu_parameters_t;

typedef struct _dpu_query_select_group_parameters_t {
    dpu_group_id_t group_id;
} * dpu_query_select_group_parameters_t;

typedef struct _dpu_query_write_group_parameters_t {
    dpu_group_id_t group_id;
} * dpu_query_write_group_parameters_t;

typedef struct _dpu_query_write_dma_control_parameters_t {
    dpu_dma_ctrl_t byte0;
    dpu_dma_ctrl_t byte5;
    dpu_dma_ctrl_t byte1;
    dpu_dma_ctrl_t byte4;
    dpu_dma_ctrl_t byte3;
    dpu_dma_ctrl_t byte2;
} * dpu_query_write_dma_control_parameters_t;

typedef struct _dpu_query_write_pc_mode_parameters_t {
    dpu_pc_mode_e pc_mode;
} * dpu_query_write_pc_mode_parameters_t;

typedef struct _dpu_query_boot_thread_parameters_t {
    dpu_thread_t thread_id;
} * dpu_query_boot_thread_parameters_t;

typedef struct _dpu_query_resume_thread_parameters_t {
    dpu_thread_t thread_id;
} * dpu_query_resume_thread_parameters_t;

typedef struct _dpu_query_clear_run_thread_parameters_t {
    dpu_thread_t thread_id;
} * dpu_query_clear_run_thread_parameters_t;

typedef struct _dpu_query_read_run_thread_parameters_t {
    dpu_thread_t thread_id;
} * dpu_query_read_run_thread_parameters_t;

typedef struct _dpu_query_write_iram_instruction_parameters_t {
    dpu_selected_mask_t selected_dpus;
    iram_addr_t address;
    iram_size_t size;
    dpuinstruction_t *data;
} * dpu_query_write_iram_instruction_parameters_t;

typedef struct _dpu_query_read_iram_instruction_parameters_t {
    iram_addr_t address;
    iram_size_t size;
} * dpu_query_read_iram_instruction_parameters_t;

typedef struct _dpu_query_write_wram_word_parameters_t {
    dpu_selected_mask_t selected_dpus;
    wram_addr_t address;
    wram_size_t size;
    dpuword_t *data;
} * dpu_query_write_wram_word_parameters_t;

typedef struct _dpu_query_read_wram_word_parameters_t {
    wram_addr_t address;
    wram_size_t size;
} * dpu_query_read_wram_word_parameters_t;

typedef struct _dpu_query_irepair_AB_configuration_parameters_t {
    uint8_t msb;
    uint8_t lsb_A;
    uint8_t lsb_B;
} * dpu_query_irepair_AB_configuration_parameters_t;

typedef struct _dpu_query_irepair_CD_configuration_parameters_t {
    uint8_t msb;
    uint8_t lsb_C;
    uint8_t lsb_D;
} * dpu_query_irepair_CD_configuration_parameters_t;

typedef struct _dpu_query_irepair_OE_configuration_parameters_t {
    uint8_t even_index;
    uint8_t odd_index;
    uint8_t timing;
} * dpu_query_irepair_OE_configuration_parameters_t;

typedef struct _dpu_query_register_file_timing_configuration_parameters_t {
    uint8_t rf_timing;
} * dpu_query_register_file_timing_configuration_parameters_t;

typedef struct _dpu_query_command_bus_duration_configuration_parameters_t {
    uint8_t duration;
} * dpu_query_command_bus_duration_configuration_parameters_t;

typedef struct _dpu_query_command_bus_sampling_configuration_parameters_t {
    uint8_t sampling;
} * dpu_query_command_bus_sampling_configuration_parameters_t;

typedef struct _dpu_query_result_bus_duration_configuration_parameters_t {
    uint8_t duration;
} * dpu_query_result_bus_duration_configuration_parameters_t;

typedef struct _dpu_query_read_notify_bit_parameters_t {
    dpu_notify_bit_id_t thread_id;
} * dpu_query_read_notify_bit_parameters_t;

typedef struct _dpu_query_read_and_clear_notify_bit_parameters_t {
    dpu_notify_bit_id_t thread_id;
} * dpu_query_read_and_clear_notify_bit_parameters_t;

typedef struct _dpu_query_read_and_set_notify_bit_parameters_t {
    dpu_notify_bit_id_t thread_id;
} * dpu_query_read_and_set_notify_bit_parameters_t;

#endif /* UFI_DPU_QUERY_PARAMETERS_H */
