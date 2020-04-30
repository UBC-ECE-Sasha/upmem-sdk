/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef HW_DPU_SYSFS_H
#define HW_DPU_SYSFS_H

#include <dpu_description.h>

struct dpu_rank_udev {
    struct udev *udev;
    struct udev_device *dev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
};

struct dpu_rank_fs {
    char rank_path[128];
    int fd_rank;
    int fd_dax;

    struct dpu_rank_udev udev, udev_dax, udev_region;
};

int
dpu_sysfs_get_available_rank(const char *rank_path, struct dpu_rank_fs *rank_fs);
void
dpu_sysfs_free_rank(struct dpu_rank_fs *rank_fs);

uint64_t
dpu_sysfs_get_region_size(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_region_id(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_dpu_chip_id(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_channels(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_dimms_per_channel(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_ranks_per_dimm(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_rank_line_size(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_channel_line_size(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_backend_id(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_ci(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_dpus_per_ci(struct dpu_rank_fs *rank_fs);
uint32_t
dpu_sysfs_get_mram_size(struct dpu_rank_fs *rank_fs);
uint8_t
dpu_sysfs_get_nb_real_ci(struct dpu_rank_fs *rank_fs);
const char *
dpu_sysfs_get_ci_mapping(struct dpu_rank_fs *rank_fs);
uint64_t
dpu_sysfs_get_capabilities(struct dpu_rank_fs *rank_fs);
int
dpu_sysfs_set_reset_ila(struct dpu_rank_fs *rank_fs, uint8_t val);

uint32_t
dpu_sysfs_get_activate_ila(struct dpu_rank_fs *rank_fs);
int
dpu_sysfs_set_activate_ila(struct dpu_rank_fs *rank_fs, uint8_t val);

uint32_t
dpu_sysfs_get_activate_filtering_ila(struct dpu_rank_fs *rank_fs);
int
dpu_sysfs_set_activate_filtering_ila(struct dpu_rank_fs *rank_fs, uint8_t val);

uint32_t
dpu_sysfs_get_activate_mram_bypass(struct dpu_rank_fs *rank_fs);
int
dpu_sysfs_set_activate_mram_bypass(struct dpu_rank_fs *rank_fs, uint8_t val);

uint32_t
dpu_sysfs_get_mram_refresh_emulation_period(struct dpu_rank_fs *rank_fs);
int
dpu_sysfs_set_mram_refresh_emulation_period(struct dpu_rank_fs *rank_fs, uint32_t val);

int
dpu_sysfs_set_inject_faults(struct dpu_rank_fs *rank_fs, uint8_t val);

int
dpu_sysfs_set_trace_command(struct dpu_rank_fs *rank_fs, uint8_t ci_mask);

int
dpu_sysfs_get_hardware_chip_id(uint8_t *chip_id);

int
dpu_sysfs_get_hardware_description(dpu_description_t description, uint8_t *capabilities_mode);

int
dpu_sysfs_get_kernel_module_version(unsigned int *major, unsigned int *minor);

#endif /* HW_DPU_SYSFS_H */
