/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libudev.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <dpu_description.h>

/* Header shared with driver */
#include "dpu_rank_ioctl.h"
#include "dpu_region_address_translation.h"
#include "dpu_region_constants.h"
#include "hw_dpu_sysfs.h"

#define init_udev_enumerator(enumerate, udev, sysname, subname, parent, devices, label)                                          \
    udev = udev_new();                                                                                                           \
    if (!udev) {                                                                                                                 \
        printf("%s: can't create udev.\n", __FUNCTION__);                                                                        \
        goto label;                                                                                                              \
    }                                                                                                                            \
    enumerate = udev_enumerate_new(udev);                                                                                        \
    if ((sysname) != NULL)                                                                                                       \
        udev_enumerate_add_match_sysname(enumerate, sysname);                                                                    \
    else if ((subname) != NULL)                                                                                                  \
        udev_enumerate_add_match_subsystem(enumerate, subname);                                                                  \
    else                                                                                                                         \
        goto label;                                                                                                              \
    if ((parent) != NULL)                                                                                                        \
        udev_enumerate_add_match_parent(enumerate, parent);                                                                      \
    udev_enumerate_scan_devices(enumerate);                                                                                      \
    devices = udev_enumerate_get_list_entry(enumerate);

#define dpu_sys_get_integer_sysattr(name, udev, type, format)                                                                    \
    const char *str;                                                                                                             \
    type value;                                                                                                                  \
                                                                                                                                 \
    str = udev_device_get_sysattr_value(rank_fs->udev.dev, name);                                                                \
    if (str == NULL)                                                                                                             \
        return (type)0;                                                                                                          \
                                                                                                                                 \
    sscanf(str, format, &value);                                                                                                 \
                                                                                                                                 \
    return value;

#define dpu_sys_set_integer_sysattr(name, udev, value, format)                                                                   \
    char str[32];                                                                                                                \
                                                                                                                                 \
    sprintf(str, format, value);                                                                                                 \
                                                                                                                                 \
    return udev_device_set_sysattr_value(rank_fs->udev.dev, name, str) < 0;

#define dpu_sys_get_string_sysattr(name, udev)                                                                                   \
    const char *str;                                                                                                             \
                                                                                                                                 \
    str = udev_device_get_sysattr_value(rank_fs->udev.dev, name);                                                                \
    if (str == NULL)                                                                                                             \
        return NULL;                                                                                                             \
                                                                                                                                 \
    return str;

uint64_t
dpu_sysfs_get_region_size(struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("size", udev_dax, uint64_t, "%" SCNu64) }

uint8_t dpu_sysfs_get_region_id(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("region_id", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_nb_channels(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("nb_channels", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_nb_dimms_per_channel(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("nb_dimms_per_channel", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_nb_ranks_per_dimm(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("nb_ranks_per_dimm", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_rank_line_size(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("rank_line_size", udev_region, uint16_t, "%hu") }

uint8_t dpu_sysfs_get_channel_line_size(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("channel_line_size", udev_region, uint16_t, "%hu") }

uint8_t dpu_sysfs_get_backend_id(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("backend_id", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_dpu_chip_id(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("dpu_chip_id", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_nb_ci(struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("nb_ci", udev_region, uint8_t, "%hhu") }

uint8_t dpu_sysfs_get_nb_dpus_per_ci(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("nb_dpus_per_ci", udev_region, uint8_t, "%hhu") }

uint32_t
    dpu_sysfs_get_mram_size(struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("mram_size", udev_region, uint32_t, "%u") }

uint8_t dpu_sysfs_get_nb_real_ci(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("nb_real_ci", udev_region, uint8_t, "%hhu")
}

const char *
dpu_sysfs_get_ci_mapping(struct dpu_rank_fs *rank_fs) { dpu_sys_get_string_sysattr("ci_mapping", udev_region) }

uint64_t dpu_sysfs_get_capabilities(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("capabilities", udev_region, uint64_t, "%" SCNx64)
}

int
dpu_sysfs_set_reset_ila(struct dpu_rank_fs *rank_fs,
    uint8_t val) { dpu_sys_set_integer_sysattr("reset_ila", udev_region, val, "%hhu") }

uint32_t dpu_sysfs_get_activate_ila(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("activate_ila", udev_region, uint8_t, "%hhu")
}

int
dpu_sysfs_set_activate_ila(struct dpu_rank_fs *rank_fs,
    uint8_t val) { dpu_sys_set_integer_sysattr("activate_ila", udev_region, val, "%hhu") }

uint32_t dpu_sysfs_get_activate_filtering_ila(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("activate_filtering_ila", udev_region, uint8_t, "%hhu")
}

int
dpu_sysfs_set_activate_filtering_ila(struct dpu_rank_fs *rank_fs,
    uint8_t val) { dpu_sys_set_integer_sysattr("activate_filtering_ila", udev_region, val, "%hhu") }

uint32_t dpu_sysfs_get_activate_mram_bypass(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("activate_mram_bypass", udev_region, uint8_t, "%hhu")
}

int
dpu_sysfs_set_activate_mram_bypass(struct dpu_rank_fs *rank_fs,
    uint8_t val) { dpu_sys_set_integer_sysattr("activate_mram_bypass", udev_region, val, "%hhu") }

uint32_t dpu_sysfs_get_mram_refresh_emulation_period(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("mram_refresh_emulation_period", udev_region, uint32_t, "%u")
}

int
dpu_sysfs_set_mram_refresh_emulation_period(struct dpu_rank_fs *rank_fs, uint32_t val)
{
    dpu_sys_set_integer_sysattr("mram_refresh_emulation_period", udev_region, val, "%u")
}

int
dpu_sysfs_set_inject_faults(struct dpu_rank_fs *rank_fs, uint8_t val)
{
    dpu_sys_set_integer_sysattr("inject_faults", udev_region, val, "%hhu")
}

int
dpu_sysfs_set_trace_command(struct dpu_rank_fs *rank_fs,
    uint8_t ci_mask) { dpu_sys_set_integer_sysattr("trace_command_mask", udev, ci_mask, "%hhu") }

uint32_t dpu_sysfs_get_fck_frequency(
    struct dpu_rank_fs *rank_fs) { dpu_sys_get_integer_sysattr("fck_frequency", udev, uint32_t, "%u") }

uint32_t dpu_sysfs_get_clock_division_min(struct dpu_rank_fs *rank_fs)
{
    dpu_sys_get_integer_sysattr("clock_division_min", udev, uint32_t, "%u")
}

static int
dpu_sysfs_try_to_allocate_rank(const char *dev_rank_path, struct dpu_rank_fs *rank_fs)
{
    struct udev_list_entry *dev_dax_list_entry;
    uint64_t capabilities;

    /* Whatever the mode, we keep an fd to dpu_rank so that
     * we have infos about how/who uses the rank
     */
    rank_fs->fd_rank = open(dev_rank_path, O_RDWR);
    if (rank_fs->fd_rank < 0)
        return -errno;

    /* udev_device_get_parent does not take a reference as stated in header */
    rank_fs->udev_region.dev = udev_device_get_parent(rank_fs->udev.dev);

    /* Dax device only exists if backend supports PERF mode */
    capabilities = dpu_sysfs_get_capabilities(rank_fs);

    if (capabilities & CAP_PERF) {
        /* There's only one dax device associated to the region,
         * but we use the enumerate match facility to find it.
         */
        init_udev_enumerator(rank_fs->udev_dax.enumerate,
            rank_fs->udev_dax.udev,
            "dax*",
            NULL,
            rank_fs->udev_region.dev,
            rank_fs->udev_dax.devices,
            err);

        udev_list_entry_foreach(dev_dax_list_entry, rank_fs->udev_dax.devices)
        {
            const char *path_dax, *dev_dax_path;

            path_dax = udev_list_entry_get_name(dev_dax_list_entry);
            rank_fs->udev_dax.dev = udev_device_new_from_syspath(rank_fs->udev_dax.udev, path_dax);
            dev_dax_path = udev_device_get_devnode(rank_fs->udev_dax.dev);

            rank_fs->fd_dax = open(dev_dax_path, O_RDWR);
            if (rank_fs->fd_dax >= 0)
                return 0;

            printf("%s: Error (%d) opening dax device %s\n", __FUNCTION__, errno, dev_dax_path);
            udev_device_unref(rank_fs->udev_dax.dev);
        }

        udev_enumerate_unref(rank_fs->udev_dax.enumerate);
        udev_unref(rank_fs->udev_dax.udev);
    } else
        return 0;

err:
    close(rank_fs->fd_rank);

    return -EINVAL;
}

void
dpu_sysfs_free_rank_fs(struct dpu_rank_fs *rank_fs)
{
    if (rank_fs->fd_dax)
        close(rank_fs->fd_dax);

    close(rank_fs->fd_rank);

    if (rank_fs->udev_dax.udev) {
        udev_unref(rank_fs->udev_dax.udev);
        udev_device_unref(rank_fs->udev_dax.dev);
        udev_enumerate_unref(rank_fs->udev_dax.enumerate);
    }

    udev_unref(rank_fs->udev.udev);
    udev_device_unref(rank_fs->udev.dev);
    udev_enumerate_unref(rank_fs->udev.enumerate);
}

void
dpu_sysfs_free_rank(struct dpu_rank_fs *rank_fs)
{
    dpu_sysfs_free_rank_fs(rank_fs);
}

// TODO allocation must be smarter than just allocating rank "one by one":
// it is better (at memory bandwidth point of view) to allocate ranks
// from unused channels rather than allocating ranks of a same channel
// (memory bandwidth is limited at a channel level)
int
dpu_sysfs_get_available_rank(const char *rank_path, struct dpu_rank_fs *rank_fs)
{
    struct udev_list_entry *dev_rank_list_entry;
    int eacces_count = 0;

    init_udev_enumerator(rank_fs->udev.enumerate, rank_fs->udev.udev, NULL, "dpu_rank", NULL, rank_fs->udev.devices, end);

    udev_list_entry_foreach(dev_rank_list_entry, rank_fs->udev.devices)
    {
        const char *path_rank, *dev_rank_path;

        path_rank = udev_list_entry_get_name(dev_rank_list_entry);
        rank_fs->udev.dev = udev_device_new_from_syspath(rank_fs->udev.udev, path_rank);
        dev_rank_path = udev_device_get_devnode(rank_fs->udev.dev);

        if (strlen(rank_path)) {
            if (!strcmp(dev_rank_path, rank_path)) {
                if (!dpu_sysfs_try_to_allocate_rank(dev_rank_path, rank_fs)) {
                    strcpy(rank_fs->rank_path, dev_rank_path);
                    return 0;
                } else {
                    printf("%s: Allocation of requested %s rank failed\n", __FUNCTION__, rank_path);
                    udev_device_unref(rank_fs->udev.dev);
                    break;
                }
            }
        } else {
            int res = dpu_sysfs_try_to_allocate_rank(dev_rank_path, rank_fs);
            if (!res) {
                strcpy(rank_fs->rank_path, dev_rank_path);
                return 0;
            }
            /* record whether we have found something that we cannot access */
            if (res == -EACCES)
                eacces_count++;
        }

        udev_device_unref(rank_fs->udev.dev);
    }
    udev_enumerate_unref(rank_fs->udev.enumerate);
    udev_unref(rank_fs->udev.udev);

end:
    return eacces_count ? -EACCES : -ENODEV;
}

// We assume there is only one chip id per machine: so we just need to get that info
// from the first rank.
int
dpu_sysfs_get_hardware_chip_id(uint8_t *chip_id)
{
    struct udev_list_entry *dev_rank_list_entry;
    struct dpu_rank_fs rank_fs;

    init_udev_enumerator(rank_fs.udev.enumerate, rank_fs.udev.udev, NULL, "dpu_rank", NULL, rank_fs.udev.devices, end);

    udev_list_entry_foreach(dev_rank_list_entry, rank_fs.udev.devices)
    {
        const char *path_rank;

        path_rank = udev_list_entry_get_name(dev_rank_list_entry);
        rank_fs.udev.dev = udev_device_new_from_syspath(rank_fs.udev.udev, path_rank);
        rank_fs.udev_region.dev = udev_device_get_parent(rank_fs.udev.dev);

        /* Get the chip id from the driver */
        *chip_id = dpu_sysfs_get_dpu_chip_id(&rank_fs);

        udev_device_unref(rank_fs.udev.dev);

        break;
    }
    udev_enumerate_unref(rank_fs.udev.enumerate);
    udev_unref(rank_fs.udev.udev);

    return 0;

end:
    return -1;
}

// We assume the topology is identical for all the ranks: so we just need to get that info
// from the first rank.
int
dpu_sysfs_get_hardware_description(dpu_description_t description, uint8_t *capabilities_mode)
{
    struct udev_list_entry *dev_rank_list_entry;
    struct dpu_rank_fs rank_fs;

    init_udev_enumerator(rank_fs.udev.enumerate, rank_fs.udev.udev, NULL, "dpu_rank", NULL, rank_fs.udev.devices, end);

    udev_list_entry_foreach(dev_rank_list_entry, rank_fs.udev.devices)
    {
        const char *path_rank;

        path_rank = udev_list_entry_get_name(dev_rank_list_entry);
        rank_fs.udev.dev = udev_device_new_from_syspath(rank_fs.udev.udev, path_rank);
        rank_fs.udev_region.dev = udev_device_get_parent(rank_fs.udev.dev);

        /* Get the real topology from the driver */
        uint8_t clock_division;
        uint32_t fck_frequency_in_mhz;
        description->topology.nr_of_dpus_per_control_interface = dpu_sysfs_get_nb_dpus_per_ci(&rank_fs);
        description->topology.nr_of_control_interfaces = dpu_sysfs_get_nb_ci(&rank_fs);
        description->memories.mram_size = dpu_sysfs_get_mram_size(&rank_fs);
        clock_division = dpu_sysfs_get_clock_division_min(&rank_fs);
        fck_frequency_in_mhz = dpu_sysfs_get_fck_frequency(&rank_fs);
        /* Keep clock_division and fck_frequency_in_mhz default value if sysfs returns 0 */
        if (clock_division)
            description->timings.clock_division = clock_division;
        if (fck_frequency_in_mhz)
            description->configuration.fck_frequency_in_mhz = fck_frequency_in_mhz;

        *capabilities_mode = dpu_sysfs_get_capabilities(&rank_fs);

        udev_device_unref(rank_fs.udev.dev);

        break;
    }
    udev_enumerate_unref(rank_fs.udev.enumerate);
    udev_unref(rank_fs.udev.udev);

    return 0;

end:
    return -1;
}

int
dpu_sysfs_get_kernel_module_version(unsigned int *major, unsigned int *minor)
{
    FILE *fp;

    if ((fp = fopen("/sys/module/dpu/version", "r")) == NULL)
        return -errno;

    if (fscanf(fp, "%u.%u", major, minor) != 2) {
        fclose(fp);
        return (errno != 0) ? -errno : -1; // errno can be 0 is there is no matching character
    }

    fclose(fp);
    return 0;
}
