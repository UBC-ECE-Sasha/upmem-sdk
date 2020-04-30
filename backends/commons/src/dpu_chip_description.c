/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>
#include <dpu_description.h>

static struct _dpu_description_t vD_description = {
    .signature.chip_id = vD,
    .topology.nr_of_control_interfaces = 8,
    .topology.nr_of_dpus_per_control_interface = 8,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 0,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_cas_description = {
    .signature.chip_id = vD_cas,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 1,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 0,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_fun_description = {
    .signature.chip_id = vD_fun,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 1,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 67108864,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_asic1_description = {
    .signature.chip_id = vD_asic1,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 1,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 0,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_asic8_description = {
    .signature.chip_id = vD_asic8,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 8,
    .timings.cmd_duration = 6,
    .timings.cmd_sampling = 3,
    .timings.res_duration = 6,
    .timings.res_sampling = 3,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 0,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_fpga1_description = {
    .signature.chip_id = vD_fpga1,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 1,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 67108864,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_fpga8_description = {
    .signature.chip_id = vD_fpga8,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 8,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 67108864,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_asic4_description = {
    .signature.chip_id = vD_asic4,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 4,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 0,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

static struct _dpu_description_t vD_fpga4_description = {
    .signature.chip_id = vD_fpga4,
    .topology.nr_of_control_interfaces = 1,
    .topology.nr_of_dpus_per_control_interface = 4,
    .timings.cmd_duration = 2,
    .timings.cmd_sampling = 1,
    .timings.res_duration = 2,
    .timings.res_sampling = 1,
    .timings.reset_wait_duration = 20,
    .timings.std_temperature = 110,
    .configuration.fck_frequency_in_mhz = 1000,
    .timings.clock_division = 2,
    .memories.mram_size = 67108864,
    .memories.dbg_mram_size = 67108864,
    .memories.wram_size = 16384,
    .memories.iram_size = 4096,
    .dpu.nr_of_threads = 24,
    .dpu.nr_of_atomic_bits = 256,
    .dpu.nr_of_notify_bits = 40,
    .dpu.nr_of_work_registers_per_thread = 24,
};

const dpu_description_t dpu_chip_descriptions[NEXT_DPU_CHIP_ID] = {
    [vD] = &vD_description,
    [vD_cas] = &vD_cas_description,
    [vD_fun] = &vD_fun_description,
    [vD_asic1] = &vD_asic1_description,
    [vD_asic8] = &vD_asic8_description,
    [vD_fpga1] = &vD_fpga1_description,
    [vD_fpga8] = &vD_fpga8_description,
    [vD_asic4] = &vD_asic4_description,
    [vD_fpga4] = &vD_fpga4_description,
};
