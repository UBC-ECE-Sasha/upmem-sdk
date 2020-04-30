/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_QUERY_CONSTRUCTORS_H
#define UFI_DPU_QUERY_CONSTRUCTORS_H

#include <stdint.h>
#include "dpu_query.h"

dpu_query_t
dpu_query_build_ignored_for_control(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_bit_order_for_control(dpu_slice_id_t slice_id,
    uint16_t c2d,
    uint16_t d2c,
    uint8_t ne,
    uint8_t stutter,
    uint32_t *result);

dpu_query_t
dpu_query_build_identity_for_control(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_software_reset_for_control(dpu_slice_id_t slice_id, uint8_t cycle_accurate, dpu_clock_division_t factor);

dpu_query_t
dpu_query_build_thermal_configuration_for_control(dpu_slice_id_t slice_id, dpu_temperature_e threshold);

dpu_query_t
dpu_query_build_command_duration_configuration_for_control(dpu_slice_id_t slice_id, uint8_t duration);

dpu_query_t
dpu_query_build_result_duration_configuration_for_control(dpu_slice_id_t slice_id, uint8_t duration);

dpu_query_t
dpu_query_build_result_sampling_configuration_for_control(dpu_slice_id_t slice_id, uint8_t sampling);

dpu_query_t
dpu_query_build_byte_order_for_control(dpu_slice_id_t slice_id, uint64_t *result);

dpu_query_t
dpu_query_build_nop_for_control(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_select_dpu_for_control(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_select_group_for_control(dpu_slice_id_t slice_id, dpu_group_id_t group_id);

dpu_query_t
dpu_query_build_select_all_for_control(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_write_group_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_group_id_t group_id);

dpu_query_t
dpu_query_build_write_group_for_all(dpu_slice_id_t slice_id, dpu_group_id_t group_id);

dpu_query_t
dpu_query_build_write_group_for_previous(dpu_slice_id_t slice_id, dpu_group_id_t group_id);

dpu_query_t
dpu_query_build_read_dma_control_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_dma_ctrl_t *result);

dpu_query_t
dpu_query_build_read_dma_control_for_previous(dpu_slice_id_t slice_id, dpu_dma_ctrl_t *result);

dpu_query_t
dpu_query_build_write_dma_control_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5);

dpu_query_t
dpu_query_build_write_dma_control_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5);

dpu_query_t
dpu_query_build_write_dma_control_for_all(dpu_slice_id_t slice_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5);

dpu_query_t
dpu_query_build_write_dma_control_for_previous(dpu_slice_id_t slice_id,
    dpu_dma_ctrl_t byte0,
    dpu_dma_ctrl_t byte1,
    dpu_dma_ctrl_t byte2,
    dpu_dma_ctrl_t byte3,
    dpu_dma_ctrl_t byte4,
    dpu_dma_ctrl_t byte5);

dpu_query_t
dpu_query_build_clear_dma_control_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_read_dma_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_dma_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_bkp_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_bkp_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_mem_fault_thread_index_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_mem_fault_thread_index_for_previous(dpu_slice_id_t slice_id, dpu_thread_t *result);

dpu_query_t
dpu_query_build_read_pc_lsb_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint8_t *result);

dpu_query_t
dpu_query_build_read_pc_lsb_for_previous(dpu_slice_id_t slice_id, uint8_t *result);

dpu_query_t
dpu_query_build_read_pc_msb_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint8_t *result);

dpu_query_t
dpu_query_build_read_pc_msb_for_previous(dpu_slice_id_t slice_id, uint8_t *result);

dpu_query_t
dpu_query_build_read_pc_mode_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_pc_mode_e *result);

dpu_query_t
dpu_query_build_read_pc_mode_for_previous(dpu_slice_id_t slice_id, dpu_pc_mode_e *result);

dpu_query_t
dpu_query_build_write_pc_mode_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_pc_mode_e pc_mode);

dpu_query_t
dpu_query_build_write_pc_mode_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, dpu_pc_mode_e pc_mode);

dpu_query_t
dpu_query_build_write_pc_mode_for_all(dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode);

dpu_query_t
dpu_query_build_write_pc_mode_for_previous(dpu_slice_id_t slice_id, dpu_pc_mode_e pc_mode);

dpu_query_t
dpu_query_build_read_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_fault_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_fault_state_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_fault_state_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_clear_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_clear_dpu_fault_state_for_all(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_clear_dpu_fault_state_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_set_and_step_dpu_fault_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_set_and_step_dpu_fault_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id);

dpu_query_t
dpu_query_build_set_and_step_dpu_fault_state_for_all(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_set_and_step_dpu_fault_state_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_read_dpu_run_state_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_run_state_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_run_state_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_dpu_run_state_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_bkp_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_bkp_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_clear_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_clear_bkp_fault_for_all(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_clear_bkp_fault_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_set_bkp_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_set_bkp_fault_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_read_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_poison_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_clear_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_clear_poison_fault_for_all(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_clear_poison_fault_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_set_poison_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_set_poison_fault_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_read_and_clear_dma_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_dma_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_dma_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_mem_fault_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_mem_fault_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_mem_fault_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_stack_up_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_stack_up_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_stack_up_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_stack_up_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_stack_up_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_stack_up_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_stack_up_for_all(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_stack_up_for_previous(dpu_slice_id_t slice_id, uint32_t *result);

dpu_query_t
dpu_query_build_boot_thread_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_boot_thread_for_group(dpu_slice_id_t slice_id, dpu_group_id_t group_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_boot_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_boot_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_resume_thread_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_resume_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_resume_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_resume_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_clear_run_thread_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_thread_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_clear_run_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_clear_run_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_clear_run_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_run_thread_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_thread_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_run_thread_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_thread_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_run_thread_for_all(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_run_thread_for_previous(dpu_slice_id_t slice_id, dpu_thread_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_write_iram_instruction_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size);

dpu_query_t
dpu_query_build_write_iram_instruction_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size);

dpu_query_t
dpu_query_build_write_iram_instruction_for_all(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size);

dpu_query_t
dpu_query_build_write_iram_instruction_for_previous(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    iram_addr_t address,
    dpuinstruction_t *data,
    iram_size_t size);

dpu_query_t
dpu_query_build_read_iram_instruction_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    iram_addr_t address,
    iram_size_t size,
    dpuinstruction_t *result);

dpu_query_t
dpu_query_build_read_iram_instruction_for_previous(dpu_slice_id_t slice_id,
    iram_addr_t address,
    iram_size_t size,
    dpuinstruction_t *result);

dpu_query_t
dpu_query_build_write_wram_word_for_previous(dpu_slice_id_t slice_id,
    dpu_selected_mask_t selected_dpus,
    wram_addr_t address,
    dpuword_t *data,
    wram_size_t size);

dpu_query_t
dpu_query_build_read_wram_word_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    wram_addr_t address,
    wram_size_t size,
    dpuword_t *result);

dpu_query_t
dpu_query_build_read_wram_word_for_previous(dpu_slice_id_t slice_id, wram_addr_t address, wram_size_t size, dpuword_t *result);

dpu_query_t
dpu_query_build_irepair_AB_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t lsb_A,
    uint8_t lsb_B,
    uint8_t msb);

dpu_query_t
dpu_query_build_irepair_AB_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t lsb_A, uint8_t lsb_B, uint8_t msb);

dpu_query_t
dpu_query_build_irepair_CD_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t lsb_C,
    uint8_t lsb_D,
    uint8_t msb);

dpu_query_t
dpu_query_build_irepair_CD_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t lsb_C, uint8_t lsb_D, uint8_t msb);

dpu_query_t
dpu_query_build_irepair_OE_configuration_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    uint8_t even_index,
    uint8_t odd_index,
    uint8_t timing);

dpu_query_t
dpu_query_build_irepair_OE_configuration_for_previous(dpu_slice_id_t slice_id,
    uint8_t even_index,
    uint8_t odd_index,
    uint8_t timing);

dpu_query_t
dpu_query_build_register_file_timing_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t rf_timing);

dpu_query_t
dpu_query_build_command_bus_duration_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t duration);

dpu_query_t
dpu_query_build_command_bus_sampling_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t sampling);

dpu_query_t
dpu_query_build_command_bus_synchronization_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_result_bus_duration_configuration_for_previous(dpu_slice_id_t slice_id, uint8_t duration);

dpu_query_t
dpu_query_build_result_bus_synchronization_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_read_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_clear_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_notify_bit_for_dpu(dpu_slice_id_t slice_id,
    dpu_member_id_t dpu_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_notify_bit_for_group(dpu_slice_id_t slice_id,
    dpu_group_id_t group_id,
    dpu_notify_bit_id_t thread_id,
    uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_notify_bit_for_all(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_read_and_set_notify_bit_for_previous(dpu_slice_id_t slice_id, dpu_notify_bit_id_t thread_id, uint32_t *result);

dpu_query_t
dpu_query_build_debug_std_replace_stop_enabled_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_debug_std_replace_stop_enabled_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_debug_std_replace_clear_for_dpu(dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

dpu_query_t
dpu_query_build_debug_std_replace_clear_for_previous(dpu_slice_id_t slice_id);

dpu_query_t
dpu_query_build_debug_std_sample_pc_for_previous(dpu_slice_id_t slice_id);

#endif /* UFI_DPU_QUERY_CONSTRUCTORS_H */
