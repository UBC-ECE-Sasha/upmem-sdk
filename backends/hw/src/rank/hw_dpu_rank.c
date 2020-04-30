/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dpu_chip_config.h>
#include <string.h>
#include <dpu_profile.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/user.h>
#include <dpu_description.h>
#include <dpu_types.h>
#include <dpu_program.h>
#include <dpu_mask.h>
#include <dpu_log_utils.h>
#include <dpu_vpd.h>
#include <dpu_internals.h>

#include "dpu_attributes.h"
// TODO: will conflict with driver header
#include "dpu_rank.h"
#include "dpu_mask.h"

/* Header shared with driver */
#include "dpu_region_address_translation.h"
#include "dpu_region_constants.h"
#include "hw_dpu_sysfs.h"
#include "dpu_rank_ioctl.h"
#include "dpu_fpga_ila.h"

#include "dpu_module_compatibility.h"

#include "static_verbose.h"

const char *
get_rank_path(dpu_description_t description);

static struct verbose_control *this_vc;
static struct verbose_control *
__vc()
{
    if (this_vc == NULL) {
        this_vc = get_verbose_control_for("hw");
    }
    return this_vc;
}

extern struct dpu_region_address_translation power9_translate;
extern struct dpu_region_address_translation xeon_sp_translate;
extern struct dpu_region_address_translation fpga_aws_translate;

struct dpu_region_address_translation *backend_translate[] = {
#ifdef __x86_64__
    &xeon_sp_translate,
#else
    0,
#endif
    0, /* fpga_kc705 has no user backend */
    &fpga_aws_translate,
#ifdef __powerpc64__
    &power9_translate,
#else
    0,
#endif
    0, /* devicetree user backend not yet implemented */
};

static dpu_rank_status_e
hw_allocate(struct dpu_rank_t *rank, dpu_description_t description);
static dpu_rank_status_e
hw_free(struct dpu_rank_t *rank);
static dpu_rank_status_e
hw_commit_commands(struct dpu_rank_t *rank, dpu_rank_buffer_t buffer);
static dpu_rank_status_e
hw_update_commands(struct dpu_rank_t *rank, dpu_rank_buffer_t buffer);
static dpu_rank_status_e
hw_copy_to_rank(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);
static dpu_rank_status_e
hw_copy_from_rank(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);
static dpu_rank_status_e
hw_fill_description_from_profile(dpu_properties_t properties, dpu_description_t description);
static dpu_rank_status_e
hw_custom_operation(struct dpu_rank_t *rank,
    dpu_slice_id_t slice_id,
    dpu_member_id_t member_id,
    dpu_custom_command_t command,
    dpu_custom_command_args_t args);
static void
hw_print_lldb_message_on_fault(struct dpu_t *dpu, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id);

const __API_SYMBOL__ struct dpu_rank_handler hw_dpu_rank_handler = {
    .allocate = hw_allocate,
    .free = hw_free,
    .commit_commands = hw_commit_commands,
    .update_commands = hw_update_commands,
    .copy_to_rank = hw_copy_to_rank,
    .copy_from_rank = hw_copy_from_rank,
    .fill_description_from_profile = hw_fill_description_from_profile,
    .custom_operation = hw_custom_operation,
    .print_lldb_message_on_fault = hw_print_lldb_message_on_fault,
};

typedef struct _hw_dpu_rank_context_t {
    /* Hybrid mode: Address of control interfaces when memory mapped
     * Perf mode:   Base region address, mappings deal with offset to target control interfaces
     * Safe mode:   Buffer handed to the driver
     */
    uint64_t *control_interfaces;
    /* Real HW can have more control interfaces than exposed to the user
     * (the main cause being address inversion disabling which did not work
     * on the platform...), so we must 'expand' the array given by the user
     * into an array that comprises the right number of control interfaces.
     * The same applies for MRAM.
     */
    uint64_t *real_buffer_control_interfaces;
    struct dpu_transfer_mram *real_transfer_matrix;
} * hw_dpu_rank_context_t;

typedef struct _fpga_allocation_parameters_t {
    bool activate_ila;
    bool activate_filtering_ila;
    bool activate_mram_bypass;
    bool activate_mram_refresh_emulation;
    unsigned int mram_refresh_emulation_period;
    char *report_path;
    bool cycle_accurate;
} fpga_allocation_parameters_t;

typedef struct _hw_dpu_rank_allocation_parameters_t {
    struct dpu_rank_fs rank_fs;
    struct dpu_region_address_translation translate;
    struct dpu_region_interleaving interleave;
    uint64_t region_size;
    uint8_t mode, dpu_chip_id, backend_id;
    uint8_t channel_id, rank_id;
    uint8_t *ptr_region;
    bool bypass_module_compatibility;
    /* Backends specific */
    fpga_allocation_parameters_t fpga;
} * hw_dpu_rank_allocation_parameters_t;

static inline hw_dpu_rank_context_t
_this(struct dpu_rank_t *rank)
{
    return (hw_dpu_rank_context_t)(rank->_internals);
}

static inline hw_dpu_rank_allocation_parameters_t
_this_params(dpu_description_t description)
{
    return (hw_dpu_rank_allocation_parameters_t)(description->_internals.data);
}

static inline bool
fill_description_with_default_values_for(dpu_chip_id_e chip_id, dpu_description_t description)
{
    switch (chip_id) {
        default:
            return false;
        case vD_asic1:
        case vD_asic4:
        case vD_asic8:
        case vD_fpga1:
        case vD_fpga4:
        case vD_fpga8:
        case vD:
            break;
    }

    dpu_description_t default_description;

    if ((default_description = default_description_for_chip(chip_id)) == NULL) {
        return false;
    }

    memcpy(description, default_description, sizeof(*description));

    return true;
}

static bool
fill_dpu_region_interleaving_values(dpu_description_t description)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(description);

    params->interleave.mram_size = description->memories.mram_size;
    params->interleave.nb_dpus_per_ci = description->topology.nr_of_dpus_per_control_interface;
    params->interleave.nb_ci = description->topology.nr_of_control_interfaces;

    params->interleave.nb_channels = dpu_sysfs_get_nb_channels(&params->rank_fs);
    params->interleave.nb_dimms_per_channel = dpu_sysfs_get_nb_dimms_per_channel(&params->rank_fs);
    params->interleave.nb_ranks_per_dimm = dpu_sysfs_get_nb_ranks_per_dimm(&params->rank_fs);
    params->interleave.rank_line_size = dpu_sysfs_get_rank_line_size(&params->rank_fs);
    params->interleave.channel_line_size = dpu_sysfs_get_channel_line_size(&params->rank_fs);

    return true;
}

static bool
fill_address_translation_backend(hw_dpu_rank_allocation_parameters_t params)
{
    params->backend_id = dpu_sysfs_get_backend_id(&params->rank_fs);
    if (params->backend_id >= DPU_BACKEND_NUMBER)
        return false;

    if (!backend_translate[params->backend_id]) {
        LOG_FN(WARNING, "No perf mode is available for the backend %d", params->backend_id);
        return false;
    }
    memcpy(&params->translate, backend_translate[params->backend_id], sizeof(struct dpu_region_address_translation));
    params->translate.interleave = &params->interleave;

    return true;
}

__attribute__((used)) static void
hw_set_debug_mode(struct dpu_rank_t *rank, uint8_t mode)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    int ret;

    ret = ioctl(params->rank_fs.fd_rank, DPU_RANK_IOCTL_DEBUG_MODE, mode);
    if (ret)
        LOG_RANK(WARNING, rank, "Failed to change debug mode (%s)", strerror(errno));
}

static uint8_t *
get_array_ci_mapping(dpu_description_t description)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(description);
    const char *str_ci_mapping = dpu_sysfs_get_ci_mapping(&params->rank_fs);
    char *c;
    int i;
    uint8_t *ci_mapping;

    if (str_ci_mapping == NULL)
        return NULL;

    ci_mapping = malloc(description->topology.nr_of_control_interfaces * sizeof(uint64_t));
    if (!ci_mapping)
        return NULL;

    c = (char *)str_ci_mapping;
    do {
        if (*c == ' ')
            *c = 0;
    } while (*++c);

    c = (char *)str_ci_mapping;
    for (i = 0; i < description->topology.nr_of_control_interfaces; ++i) {
        sscanf(c, "%hhu", &ci_mapping[i]);
        c += strlen(c) + 1;
    }

    return ci_mapping;
}

static int
open_vpd_file(struct dpu_rank_t *rank, FILE **vpd)
{
    int status;

    const char *rank_path = get_rank_path(rank->description);
    char *dimm_serial_number = NULL;
    char *dimm_vpd_file = NULL;

    status = dpu_sysfs_get_dimm_serial_number(rank_path, &dimm_serial_number);
    if (status != 0) {
        return status;
    }

    if (asprintf(&dimm_vpd_file, "/usr/share/upmem/dimm-quirks/UPM%s.vpd", dimm_serial_number) == -1) {
        free(dimm_serial_number);
        return -ENOMEM;
    }
    free(dimm_serial_number);

    *vpd = fopen(dimm_vpd_file, "r");
    if (*vpd == NULL) {
        LOG_RANK(WARNING, rank, "unable to find %s", dimm_vpd_file);
        free(dimm_vpd_file);
        return -errno;
    }

    LOG_RANK(INFO, rank, "found %s", dimm_vpd_file);
    free(dimm_vpd_file);

    return 0;
}

static dpu_error_t
fill_sram_repairs_and_update_enabled_dpus(struct dpu_rank_t *rank)
{
    int status;

    FILE *vpd = NULL;
    const char *rank_path = get_rank_path(rank->description);
    int rank_index;

    status = open_vpd_file(rank, &vpd);
    if (status != 0) {
        LOG_RANK(WARNING, rank, "unable to open VPD file");
        return DPU_ERR_SYSTEM;
    }

    status = dpu_sysfs_get_rank_index(rank_path, &rank_index);
    if (status != 0) {
        LOG_RANK(WARNING, rank, "unable to get rank index");
        return DPU_ERR_SYSTEM;
    }

    struct dimm_vpd dimm_vpd;
    bool repair_requested
        = (rank->description->configuration.do_iram_repair) || (rank->description->configuration.do_wram_repair);
    struct repair_entry entry;
    uint16_t repair_cnt = 0;

    if (fread(&dimm_vpd, sizeof(dimm_vpd), 1, vpd) != 1) {
        LOG_RANK(WARNING, rank, "unable to parse VPD file");
        return DPU_ERR_VPD_INVALID_FILE;
    }

    if (memcmp(dimm_vpd.struct_id, VPD_STRUCT_ID, sizeof(dimm_vpd.struct_id)) != 0) {
        LOG_RANK(WARNING, rank, "invalid VPD structure ID");
        return DPU_ERR_VPD_INVALID_FILE;
    }

    if (dimm_vpd.struct_ver != VPD_STRUCT_VERSION) {
        LOG_RANK(WARNING, rank, "invalid VPD structure version");
        return DPU_ERR_VPD_INVALID_FILE;
    }

    if (repair_requested) {
        while (fread(&entry, sizeof(entry), 1, vpd) == 1) {
            repair_cnt++;

            if (entry.rank != rank_index)
                continue;

            struct dpu_t *dpu = DPU_GET_UNSAFE(rank, entry.ci, entry.dpu);

            struct dpu_memory_repair_t *repair_info;
            if (entry.iram_wram == 0) { // IRAM
                repair_info = &dpu->repair.iram_repair;
            } else { // WRAM
                repair_info = &dpu->repair.wram_repair[entry.bank];
            }

            uint32_t previous_nr_of_corrupted_addresses = repair_info->nr_of_corrupted_addresses++;
            if (previous_nr_of_corrupted_addresses < NB_MAX_REPAIR_ADDR) {
                repair_info->corrupted_addresses[previous_nr_of_corrupted_addresses].address = entry.address;
                repair_info->corrupted_addresses[previous_nr_of_corrupted_addresses].faulty_bits = entry.bits;
            }
        }

        if (repair_cnt != dimm_vpd.repair_count) {
            LOG_RANK(WARNING, rank, "malformed VPD file");
            return DPU_ERR_VPD_INVALID_FILE;
        }
    }

    uint8_t nr_cis = rank->description->topology.nr_of_control_interfaces;
    uint8_t nr_dpus = rank->description->topology.nr_of_dpus_per_control_interface;
    uint64_t disabled_mask = dimm_vpd.ranks[rank_index].dpu_disabled;

    for (uint8_t each_ci = 0; each_ci < nr_cis; ++each_ci) {
        dpu_selected_mask_t disabled_mask_for_ci = (dpu_selected_mask_t)((disabled_mask >> (nr_dpus * each_ci)) & 0xFFl);

        rank->runtime.control_interface.slice_info[each_ci].enabled_dpus
            &= dpu_mask_difference(dpu_mask_all(nr_dpus), disabled_mask_for_ci);
        rank->runtime.control_interface.slice_info[each_ci].all_dpus_are_enabled
            = dpu_mask_intersection(rank->runtime.control_interface.slice_info[each_ci].enabled_dpus, dpu_mask_all(nr_dpus))
            == dpu_mask_all(nr_dpus);
    }

    return DPU_OK;
}

/* Function used in dpu-diag */
__API_SYMBOL__ bool
is_kernel_module_compatible(void)
{
    /* 1/ Get module version */
    unsigned int major, minor;
    int ret = dpu_sysfs_get_kernel_module_version(&major, &minor);
    if (ret) {
        LOG_FN(WARNING, "Failed to get dpu kernel module version. Aborting rank allocation");
        return false;
    }

    /* 2/ Check compatibility */
    /* Do not use DPU_MODULE_MIN_MINOR directly in the comparison as the compiler
     * complains if DPU_MODULE_MIN_MINOR = 0 (comparison is always false)
     */
    unsigned int min_minor = DPU_MODULE_MIN_MINOR;
    if ((major != DPU_MODULE_EXPECTED_MAJOR) || (minor < min_minor)) {
        LOG_FN(WARNING, "SDK is not compatible with dpu kernel module. Aborting rank allocation");
        return false;
    }

    return true;
}

/* In perf script that measures memory bandwidth, we need for per-rank
 * statistics to get the equivalence rank pointer <=> rank path: use
 * this function for perf to probe and get the rank path from the rank
 * pointer.
 */
__PERF_PROFILING_SYMBOL__ __API_SYMBOL__ void
log_rank_path(struct dpu_rank_t *rank, char *path)
{
    LOG_RANK(INFO, rank, "rank path is: %s", path);
}

/* Function used in dpu-diag */
__API_SYMBOL__ const char *
get_rank_path(dpu_description_t description)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(description);
    return params->rank_fs.rank_path;
}

static dpu_rank_status_e
hw_allocate(struct dpu_rank_t *rank, dpu_description_t description)
{
    dpu_rank_status_e status;
    hw_dpu_rank_allocation_parameters_t params = _this_params(description);
    hw_dpu_rank_context_t rank_context;
    int ret;

    /* 1/ Make sure SDK is compatible with the kernel module */
    static bool compatibility_checked = false;
    if ((params->bypass_module_compatibility == false) && (compatibility_checked == false)) {
        if (is_kernel_module_compatible() == false) {
            status = DPU_RANK_SYSTEM_ERROR;
            goto end;
        }
        compatibility_checked = true;
    }

    /* 2/ Find an available rank whose mode is compatible with the one asked
     * by the user.
     * TODO: Maybe user wants to have a specific dpu_chip_id passed as argument,
     * so we could enforce the allocation for this specific id.
     */
    ret = dpu_sysfs_get_available_rank(params->rank_fs.rank_path, &params->rank_fs);
    if (ret) {
        LOG_RANK(WARNING,
            rank,
            "Failed to find available rank with mode %u%s",
            params->mode,
            ret == -EACCES ? ", you don't have permissions for existing devices" : "");
        status = DPU_RANK_SYSTEM_ERROR;
        goto end;
    }

    /* 3/ dpu_rank_handler initialization */
    if ((rank_context = malloc(sizeof(*rank_context))) == NULL) {
        status = DPU_RANK_SYSTEM_ERROR;
        goto free_physical_rank;
    }

    rank->_internals = rank_context;

    params->dpu_chip_id = dpu_sysfs_get_dpu_chip_id(&params->rank_fs);

    if (params->dpu_chip_id != description->signature.chip_id) {
        LOG_RANK(WARNING, rank, "Unexpected chip id %u (description is %u)", params->dpu_chip_id, description->signature.chip_id);
        status = DPU_RANK_SYSTEM_ERROR;
        goto free_rank_context;
    }

    rank->description = description;

    // TODO: When driver safe mode is fully implemented, this must be set at false in this case.
    rank->description->configuration.api_must_switch_mram_mux = true;
    rank->description->configuration.init_mram_mux = true;
    params->interleave.nb_real_ci = rank->description->topology.nr_of_control_interfaces;

    if (params->mode == DPU_REGION_MODE_SAFE) {
        rank_context->control_interfaces = malloc(description->topology.nr_of_control_interfaces * sizeof(uint64_t));
        if (!rank_context->control_interfaces) {
            LOG_RANK(WARNING, rank, "Failed to allocate memory for control interfaces %u", params->dpu_chip_id);
            status = DPU_RANK_SYSTEM_ERROR;
            goto free_rank_context;
        }

    } else if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID) {
        /* 4/ Retrieve interleaving infos */
        if (!fill_dpu_region_interleaving_values(description)) {
            LOG_RANK(WARNING, rank, "Failed to retrieve interleaving info");
            status = DPU_RANK_SYSTEM_ERROR;
            goto free_rank_context;
        }

        /* 5/ Initialize CPU backend for this rank */
        ret = fill_address_translation_backend(params);
        if (!ret) {
            LOG_RANK(WARNING, rank, "Failed to retrieve backend");
            status = DPU_RANK_SYSTEM_ERROR;
            goto free_rank_context;
        }

        // TODO implement init/destroy_rank
        // params->translate.init_rank(&params->translate, params->channel_id, params->rank_id);
        if (params->translate.init_region) {
            ret = params->translate.init_region(&params->translate);
            if (ret < 0) {
                LOG_RANK(WARNING, rank, "Failed to init region: %s", strerror(errno));
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_rank_context;
            }
        }

        if (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0) {
            rank_context->control_interfaces = malloc(description->topology.nr_of_control_interfaces * sizeof(uint64_t));
            if (!rank_context->control_interfaces) {
                LOG_RANK(WARNING, rank, "Failed to allocate memory for control interfaces %u", params->dpu_chip_id);
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_region;
            }
        } else if (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE)) {
            rank_context->control_interfaces
                = mmap(NULL, params->translate.hybrid_mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, params->rank_fs.fd_rank, 0);
            if (rank_context->control_interfaces == MAP_FAILED) {
                LOG_RANK(WARNING, rank, "Failed to mmap control interfaces %u", params->dpu_chip_id);
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_region;
            }
        }

        if (params->mode == DPU_REGION_MODE_PERF) {
            /* 6/ Mmap the whole physical region */
            params->region_size = dpu_sysfs_get_region_size(&params->rank_fs);

            /* mmap does not guarantee (at all) that the address will be aligned on hugepage size (1GB) but the driver does. */
            params->ptr_region = mmap(0, params->region_size, PROT_READ | PROT_WRITE, MAP_SHARED, params->rank_fs.fd_dax, 0);
            if (params->ptr_region == MAP_FAILED) {
                LOG_RANK(WARNING, rank, "Failed to mmap dax region: %s", strerror(errno));
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_ci;
            }

            rank_context->control_interfaces = (uint64_t *)params->ptr_region;
        }

        params->interleave.nb_real_ci = dpu_sysfs_get_nb_real_ci(&params->rank_fs);
        if (params->interleave.nb_real_ci != description->topology.nr_of_control_interfaces) {
            LOG_RANK(WARNING,
                rank,
                "The number of advertised CIs (%hhu) is different from the real number of CIs (%hhu)",
                description->topology.nr_of_control_interfaces,
                params->interleave.nb_real_ci);

            rank_context->real_buffer_control_interfaces = malloc(params->interleave.nb_real_ci * sizeof(uint64_t));
            if (!rank_context->real_buffer_control_interfaces) {
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_ptr_region;
            }

            rank_context->real_transfer_matrix = malloc(params->interleave.nb_real_ci
                * description->topology.nr_of_dpus_per_control_interface * sizeof(struct dpu_transfer_mram));
            if (!rank_context->real_transfer_matrix) {
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_real_ci;
            }

            params->interleave.ci_mapping = get_array_ci_mapping(description);
            if (!params->interleave.ci_mapping) {
                status = DPU_RANK_SYSTEM_ERROR;
                goto free_real_xfer_matrix;
            }
        }
    }

    /* 7/ Inform DPUs about their SRAM defects,
     * and update CIs runtime configuration
     */
    dpu_error_t repair_status = fill_sram_repairs_and_update_enabled_dpus(rank);
    if (repair_status == DPU_ERR_VPD_INVALID_FILE) {
        /* VPD file is corrupted, aborting rank allocation */
        status = DPU_RANK_SYSTEM_ERROR;
        goto free_real_xfer_matrix;
    } else if (repair_status != DPU_OK) {
        /* Failed to read VPD file, disabling repair but still return success */
        LOG_RANK(WARNING, rank, "disabling SRAM repair");
        description->configuration.do_iram_repair = false;
        description->configuration.do_wram_repair = false;
    }

    log_rank_path(rank, params->rank_fs.rank_path);

    return DPU_RANK_SUCCESS;

free_real_xfer_matrix:
    if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID)
        if (params->interleave.nb_real_ci != description->topology.nr_of_control_interfaces)
            free(rank_context->real_transfer_matrix);
free_real_ci:
    if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID)
        if (params->interleave.nb_real_ci != description->topology.nr_of_control_interfaces)
            free(rank_context->real_buffer_control_interfaces);
free_ptr_region:
    if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID)
        if (params->mode == DPU_REGION_MODE_PERF)
            munmap(params->ptr_region, params->region_size);
free_ci:
    if (params->mode == DPU_REGION_MODE_SAFE
        || (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0))
        free(rank_context->control_interfaces);
    else if (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE))
        munmap(rank_context->control_interfaces, params->translate.hybrid_mmap_size);
free_region:
    // TODO rank implementation
    // params->translate.destroy_rank(&params->translate, params->channel_id, params->rank_id);
    if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID)
        if (params->translate.destroy_region)
            params->translate.destroy_region(&params->translate);
free_rank_context:
    free(rank_context);
free_physical_rank:
    dpu_sysfs_free_rank(&params->rank_fs);
end:
    return status;
}

static dpu_rank_status_e
hw_free(struct dpu_rank_t *rank)
{
    hw_dpu_rank_context_t rank_context = _this(rank);
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);

    // TODO rank implementation
    // params->translate.destroy_rank(&params->translate, params->channel_id, params->rank_id);
    if (params->mode == DPU_REGION_MODE_PERF) {
        munmap(params->ptr_region, params->region_size);
        if (params->translate.destroy_region)
            params->translate.destroy_region(&params->translate);
    } else if (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE)) {
        munmap(rank_context->control_interfaces, params->translate.hybrid_mmap_size);
    } else
        free(rank_context->control_interfaces);

    if (params->mode == DPU_REGION_MODE_PERF || params->mode == DPU_REGION_MODE_HYBRID) {
        if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces) {
            free(rank_context->real_buffer_control_interfaces);
            free(rank_context->real_transfer_matrix);
            free(params->interleave.ci_mapping);
        }
    }

    dpu_sysfs_free_rank(&params->rank_fs);

    free(rank_context);

    return DPU_RANK_SUCCESS;
}

static inline uint8_t
get_slice_id(struct dpu_rank_t *rank, uint8_t real_slice_id)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    uint8_t i, nb_cis;

    nb_cis = rank->description->topology.nr_of_control_interfaces;

    for (i = 0; i < nb_cis; ++i)
        if (real_slice_id == params->interleave.ci_mapping[i])
            return i;

    return nb_cis;
}

static inline uint8_t
get_real_slice_id(struct dpu_rank_t *rank, uint8_t slice_id)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);

    return params->interleave.ci_mapping[slice_id];
}

static inline int
get_real_transfer_matrix_index(struct dpu_rank_t *rank, dpu_slice_id_t real_slice_id, dpu_id_t dpu_id)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);

    return dpu_id * params->interleave.nb_real_ci + real_slice_id;
}

static inline int
get_transfer_matrix_index(struct dpu_rank_t *rank, dpu_slice_id_t slice_id, dpu_id_t dpu_id)
{
    return dpu_id * rank->description->topology.nr_of_control_interfaces + slice_id;
}

static dpu_rank_status_e
hw_commit_commands(struct dpu_rank_t *rank, dpu_rank_buffer_t buffer)
{
    hw_dpu_rank_context_t rank_context = _this(rank);
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    dpu_rank_buffer_t ptr_buffer = buffer;
    int ret;

    if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces) {
        if (params->mode == DPU_REGION_MODE_PERF
            || (params->mode == DPU_REGION_MODE_HYBRID && params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE)) {
            memset(rank_context->real_buffer_control_interfaces, 0, params->interleave.nb_real_ci * sizeof(uint64_t));

            for (dpu_slice_id_t slice_id = 0; slice_id < rank->description->topology.nr_of_control_interfaces; ++slice_id)
                rank_context->real_buffer_control_interfaces[get_real_slice_id(rank, slice_id)] = buffer[slice_id];

            ptr_buffer = rank_context->real_buffer_control_interfaces;
        }
    }

    switch (params->mode) {
        case DPU_REGION_MODE_PERF:
            params->translate.write_to_cis(&params->translate,
                rank_context->control_interfaces,
                params->channel_id,
                params->rank_id,
                ptr_buffer,
                rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
            break;
        case DPU_REGION_MODE_HYBRID:
            if (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) {
                params->translate.write_to_cis(&params->translate,
                    rank_context->control_interfaces,
                    params->channel_id,
                    params->rank_id,
                    ptr_buffer,
                    rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
                break;
            }
            /* fall through */
        case DPU_REGION_MODE_SAFE:
            ret = ioctl(params->rank_fs.fd_rank, DPU_RANK_IOCTL_COMMIT_COMMANDS, ptr_buffer);
            if (ret) {
                LOG_RANK(WARNING, rank, "%s", strerror(errno));
                return DPU_RANK_SYSTEM_ERROR;
            }
            break;
        default:
            return DPU_RANK_SYSTEM_ERROR;
    }

    return DPU_RANK_SUCCESS;
}

static dpu_rank_status_e
hw_update_commands(struct dpu_rank_t *rank, dpu_rank_buffer_t buffer)
{
    hw_dpu_rank_context_t rank_context = _this(rank);
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    dpu_rank_buffer_t ptr_buffer = buffer;
    int ret;

    if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces) {
        if (params->mode == DPU_REGION_MODE_PERF
            || (params->mode == DPU_REGION_MODE_HYBRID && params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE))
            ptr_buffer = rank_context->real_buffer_control_interfaces;
    }

    switch (params->mode) {
        case DPU_REGION_MODE_PERF:
            params->translate.read_from_cis(&params->translate,
                rank_context->control_interfaces,
                params->channel_id,
                params->rank_id,
                ptr_buffer,
                rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
            break;
        case DPU_REGION_MODE_HYBRID:
            if (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) {
                params->translate.read_from_cis(&params->translate,
                    rank_context->control_interfaces,
                    params->channel_id,
                    params->rank_id,
                    ptr_buffer,
                    rank->description->topology.nr_of_control_interfaces * sizeof(uint64_t));
                break;
            }
            /* fall through */
        case DPU_REGION_MODE_SAFE:
            ret = ioctl(params->rank_fs.fd_rank, DPU_RANK_IOCTL_UPDATE_COMMANDS, ptr_buffer);
            if (ret) {
                LOG_RANK(WARNING, rank, "%s", strerror(errno));
                return DPU_RANK_SYSTEM_ERROR;
            }

            break;
        default:
            return DPU_RANK_SYSTEM_ERROR;
    }

    if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces) {
        if (params->mode == DPU_REGION_MODE_PERF
            || (params->mode == DPU_REGION_MODE_HYBRID && params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE)) {
            for (dpu_slice_id_t real_slice_id = 0; real_slice_id < params->interleave.nb_real_ci; ++real_slice_id) {
                uint8_t slice_id = get_slice_id(rank, real_slice_id);

                if (slice_id >= rank->description->topology.nr_of_control_interfaces)
                    continue;

                buffer[slice_id] = rank_context->real_buffer_control_interfaces[real_slice_id];
            }
        }
    }

    return DPU_RANK_SUCCESS;
}

static struct dpu_transfer_mram *
expand_transfer_matrix(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    hw_dpu_rank_context_t rank_context = _this(rank);
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    int xfer_idx, real_xfer_idx;

    memset(rank_context->real_transfer_matrix,
        0,
        params->interleave.nb_real_ci * rank->description->topology.nr_of_dpus_per_control_interface
            * sizeof(struct dpu_transfer_mram));

    for (dpu_id_t dpu_id = 0; dpu_id < rank->description->topology.nr_of_dpus_per_control_interface; ++dpu_id) {
        for (dpu_slice_id_t slice_id = 0; slice_id < rank->description->topology.nr_of_control_interfaces; ++slice_id) {
            xfer_idx = get_transfer_matrix_index(rank, slice_id, dpu_id);

            if (transfer_matrix[xfer_idx].ptr == NULL)
                continue;

            real_xfer_idx = get_real_transfer_matrix_index(rank, get_real_slice_id(rank, slice_id), dpu_id);

            rank_context->real_transfer_matrix[real_xfer_idx].mram_number = transfer_matrix[xfer_idx].mram_number;
            rank_context->real_transfer_matrix[real_xfer_idx].size = transfer_matrix[xfer_idx].size;
            rank_context->real_transfer_matrix[real_xfer_idx].offset_in_mram = transfer_matrix[xfer_idx].offset_in_mram;
            rank_context->real_transfer_matrix[real_xfer_idx].ptr = transfer_matrix[xfer_idx].ptr;
        }
    }

    return rank_context->real_transfer_matrix;
}

static dpu_rank_status_e
hw_copy_to_rank(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    struct dpu_transfer_mram *ptr_transfer_matrix = transfer_matrix;
    int ret;

    if (params->mode == DPU_REGION_MODE_PERF
        || (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0)) {
        if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces)
            ptr_transfer_matrix = expand_transfer_matrix(rank, transfer_matrix);
    }

    switch (params->mode) {
        case DPU_REGION_MODE_PERF:
            params->translate.write_to_rank(
                &params->translate, params->ptr_region, params->channel_id, params->rank_id, ptr_transfer_matrix);

            break;
        case DPU_REGION_MODE_HYBRID:
            if ((params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0) {
                params->translate.write_to_rank(
                    &params->translate, params->ptr_region, params->channel_id, params->rank_id, ptr_transfer_matrix);

                break;
            }
            /* fall through */
        case DPU_REGION_MODE_SAFE:
            ret = ioctl(params->rank_fs.fd_rank, DPU_RANK_IOCTL_WRITE_TO_RANK, ptr_transfer_matrix);
            if (ret) {
                LOG_RANK(WARNING, rank, "%s", strerror(errno));
                return DPU_RANK_SYSTEM_ERROR;
            }

            break;
        default:
            return DPU_RANK_SYSTEM_ERROR;
    }

    return DPU_RANK_SUCCESS;
}

static dpu_rank_status_e
hw_copy_from_rank(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix)
{
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);
    struct dpu_transfer_mram *ptr_transfer_matrix = transfer_matrix;
    int ret;

    if (params->mode == DPU_REGION_MODE_PERF
        || (params->mode == DPU_REGION_MODE_HYBRID && (params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0)) {
        if (params->interleave.nb_real_ci != rank->description->topology.nr_of_control_interfaces)
            ptr_transfer_matrix = expand_transfer_matrix(rank, transfer_matrix);
    }

    switch (params->mode) {
        case DPU_REGION_MODE_PERF:
            params->translate.read_from_rank(
                &params->translate, params->ptr_region, params->channel_id, params->rank_id, ptr_transfer_matrix);

            break;
        case DPU_REGION_MODE_HYBRID:
            if ((params->translate.capabilities & CAP_HYBRID_CONTROL_INTERFACE) == 0) {
                params->translate.read_from_rank(
                    &params->translate, params->ptr_region, params->channel_id, params->rank_id, ptr_transfer_matrix);

                break;
            }
            /* fall through */
        case DPU_REGION_MODE_SAFE:
            ret = ioctl(params->rank_fs.fd_rank, DPU_RANK_IOCTL_READ_FROM_RANK, ptr_transfer_matrix);
            if (ret) {
                LOG_RANK(WARNING, rank, "%s", strerror(errno));
                return DPU_RANK_SYSTEM_ERROR;
            }

            break;
        default:
            return DPU_RANK_SYSTEM_ERROR;
    }

    return DPU_RANK_SUCCESS;
}

#define validate(p)                                                                                                              \
    do {                                                                                                                         \
        if (!(p))                                                                                                                \
            return DPU_RANK_INVALID_PROPERTY_ERROR;                                                                              \
    } while (0)

static void
free_hw_parameters(void *description)
{
    hw_dpu_rank_allocation_parameters_t params = description;

    if (params->fpga.report_path)
        free(params->fpga.report_path);

    free(params);
}

static dpu_rank_status_e
hw_fill_description_from_profile(dpu_properties_t properties, dpu_description_t description)
{
    hw_dpu_rank_allocation_parameters_t parameters;
    uint32_t clock_division, refresh_emulation_period, fck_frequency;
    int ret;
    char *report_path, *rank_path, *region_mode_input = NULL;
    bool activate_ila = false, activate_filtering_ila = false, activate_mram_bypass = false, cycle_accurate = false;
    bool mram_access_by_dpu_only;
    uint8_t chip_id, capabilities_mode;
    bool bypass_module_compatibility;

    parameters = malloc(sizeof(*parameters));
    if (!parameters) {
        return DPU_RANK_SYSTEM_ERROR;
    }

    ret = dpu_sysfs_get_hardware_chip_id(&chip_id);
    if (ret == -1) {
        free(parameters);
        return DPU_RANK_SYSTEM_ERROR;
    }

    validate(fill_description_with_default_values_for((dpu_chip_id_e)chip_id, description));

    ret = dpu_sysfs_get_hardware_description(description, &capabilities_mode);
    if (ret == -1) {
        free(parameters);
        return DPU_RANK_SYSTEM_ERROR;
    }

    validate(fetch_string_property(properties, DPU_PROFILE_PROPERTY_HW_REGION_MODE, &region_mode_input, NULL));
    validate(fetch_string_property(properties, DPU_PROFILE_PROPERTY_RANK_PATH, &rank_path, NULL));
    validate(fetch_integer_property(
        properties, DPU_PROFILE_PROPERTY_CLOCK_DIVISION, &clock_division, description->timings.clock_division));
    validate((clock_division & ~0xFF) == 0);
    validate(fetch_integer_property(
        properties, DPU_PROFILE_PROPERTY_FCK_FREQUENCY, &fck_frequency, description->configuration.fck_frequency_in_mhz));
    validate(fetch_boolean_property(
        properties, DPU_PROFILE_PROPERTY_TRY_REPAIR_IRAM, &(description->configuration.do_iram_repair), true));
    validate(fetch_boolean_property(
        properties, DPU_PROFILE_PROPERTY_TRY_REPAIR_WRAM, &(description->configuration.do_wram_repair), true));

    /* FPGA specific */
    validate(
        fetch_string_property(properties, DPU_PROFILE_PROPERTY_FPGA_ILA_REPORT_PATH, &report_path, "/tmp/fpga_ila_report.csv"));
    validate(fetch_boolean_property(properties, DPU_PROFILE_PROPERTY_FPGA_ILA_ENABLED, &activate_ila, activate_ila));
    validate(fetch_boolean_property(
        properties, DPU_PROFILE_PROPERTY_FPGA_ILA_FILTERING_ENABLED, &activate_filtering_ila, activate_filtering_ila));
    validate(fetch_boolean_property(
        properties, DPU_PROFILE_PROPERTY_FPGA_ILA_MRAM_BYPASS_ENABLED, &activate_mram_bypass, activate_mram_bypass));
    validate(fetch_integer_property(
        properties, DPU_PROFILE_PROPERTY_FPGA_ILA_MRAM_EMUL_REFRESH_ENABLED, &refresh_emulation_period, 0));
    validate(fetch_boolean_property(properties, DPU_PROFILE_PROPERTY_FPGA_CYCLE_ACCURATE, &cycle_accurate, cycle_accurate));

    validate(fetch_boolean_property(properties, DPU_PROFILE_PROPERTY_MRAM_ACCESS_BY_DPU_ONLY, &mram_access_by_dpu_only, false));

    validate(fetch_boolean_property(properties, DPU_PROFILE_PROPERTY_MODULE_COMPAT, &bypass_module_compatibility, false));

    memset(&parameters->rank_fs, 0, sizeof(struct dpu_rank_fs));
    if (rank_path) {
        strcpy(parameters->rank_fs.rank_path, rank_path);
        free(rank_path);
    }

    if (region_mode_input) {
        if (!strcmp(region_mode_input, "safe"))
            parameters->mode = (uint8_t)DPU_REGION_MODE_SAFE;
        else if (!strcmp(region_mode_input, "perf"))
            parameters->mode = (uint8_t)DPU_REGION_MODE_PERF;
        else if (!strcmp(region_mode_input, "hybrid"))
            parameters->mode = (uint8_t)DPU_REGION_MODE_HYBRID;
        else {
            LOG_FN(WARNING,
                "Provided region mode (%s) is unknown, switching to default (%s)",
                region_mode_input,
                (capabilities_mode & CAP_PERF) ? "perf" : "safe");
            parameters->mode = (capabilities_mode & CAP_PERF) ? (uint8_t)DPU_REGION_MODE_PERF : (uint8_t)DPU_REGION_MODE_SAFE;
        }

        free(region_mode_input);
    } else {
        LOG_FN(WARNING, "Region mode not specified, switching to default (%s)", (capabilities_mode & CAP_PERF) ? "perf" : "safe");
        parameters->mode = (capabilities_mode & CAP_PERF) ? (uint8_t)DPU_REGION_MODE_PERF : (uint8_t)DPU_REGION_MODE_SAFE;
    }

    description->configuration.mram_access_by_dpu_only = mram_access_by_dpu_only;

    /* FPGA specific */
    parameters->fpga.activate_ila = activate_ila;
    parameters->fpga.activate_filtering_ila = activate_filtering_ila;
    parameters->fpga.activate_mram_bypass = activate_mram_bypass;
    parameters->fpga.activate_mram_refresh_emulation = refresh_emulation_period != 0;
    parameters->fpga.mram_refresh_emulation_period = refresh_emulation_period;
    parameters->fpga.report_path = report_path;

    parameters->bypass_module_compatibility = bypass_module_compatibility;

    description->configuration.enable_cycle_accurate_behavior = cycle_accurate;
    description->timings.clock_division = clock_division;
    description->configuration.fck_frequency_in_mhz = fck_frequency;
    description->_internals.data = parameters;
    description->_internals.free = free_hw_parameters;

    return DPU_RANK_SUCCESS;
}

static dpu_rank_status_e
hw_custom_operation(struct dpu_rank_t *rank,
    __attribute__((unused)) dpu_slice_id_t slice_id,
    __attribute__((unused)) dpu_member_id_t member_id,
    dpu_custom_command_t command,
    dpu_custom_command_args_t args)
{
    dpu_rank_status_e status = DPU_RANK_SUCCESS;
    hw_dpu_rank_allocation_parameters_t params = _this_params(rank->description);

    // Important Note: fpga with ILA support implements only one DPU behind the control interface
    // => that's why we use the DPU_COMMAND_DPU* versions of the custom commands, the operations on ILA
    // would normally apply on the control interface.
    switch (command) {
        case DPU_COMMAND_DPU_SOFT_RESET:
            if (params->fpga.activate_ila) {
                if (!reset_ila(&params->rank_fs)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }

                if (params->fpga.activate_filtering_ila) {
                    if (!activate_filter_ila(&params->rank_fs)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                } else {
                    if (!deactivate_filter_ila(&params->rank_fs)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                }

                set_mram_bypass_to(&params->rank_fs, params->fpga.activate_mram_bypass);
            }

            break;
        case DPU_COMMAND_DPU_PREEXECUTION:
            if (params->fpga.activate_ila) {
                if (!reset_ila(&params->rank_fs)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }

                if (params->fpga.activate_filtering_ila) {
                    if (!activate_filter_ila(&params->rank_fs)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                } else {
                    if (!deactivate_filter_ila(&params->rank_fs)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                }

                set_mram_bypass_to(&params->rank_fs, params->fpga.activate_mram_bypass);

                if (params->fpga.activate_mram_refresh_emulation) {
                    if (!enable_refresh_emulation(&params->rank_fs, params->fpga.mram_refresh_emulation_period)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                } else {
                    if (!disable_refresh_emulation(&params->rank_fs)) {
                        status = DPU_RANK_SYSTEM_ERROR;
                        break;
                    }
                }

                if (!activate_ila(&params->rank_fs)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }
            }

            break;
        case DPU_COMMAND_DPU_POSTEXECUTION:
            if (params->fpga.activate_ila) {
                if (!deactivate_ila(&params->rank_fs)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }

                if (!disable_refresh_emulation(&params->rank_fs)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }

                if (!dump_ila_report(&params->rank_fs, params->fpga.report_path)) {
                    status = DPU_RANK_SYSTEM_ERROR;
                    break;
                }
            }

            break;
        case DPU_COMMAND_SET_COMMAND_TRACE:
            if (dpu_sysfs_set_trace_command(&params->rank_fs, *((uint8_t *)args)))
                status = DPU_RANK_SYSTEM_ERROR;

            break;
        default:
            break;
    }

    return status;
}

static void
hw_print_lldb_message_on_fault(struct dpu_t *dpu, dpu_slice_id_t slice_id, dpu_member_id_t dpu_id)
{
    char *dpu_program = dpu->program->program_path;
    hw_dpu_rank_allocation_parameters_t params = (hw_dpu_rank_allocation_parameters_t)(dpu->rank->description->_internals.data);
    unsigned int region_id;
    unsigned int rank_id;

    sscanf(params->rank_fs.rank_path, "/dev/dpu_region%u/dpu_rank%u", &region_id, &rank_id);

    fprintf(stderr,
        "**************************************************************\n"
        "*** DPU %u:%u:%u:%u is in fault\n"
        "*** Attach the DPU by running: \n"
        "\t'dpu-lldb-attach-dpu %u.%u.%u.%u %s'\n"
        "**************************************************************\n",
        region_id,
        rank_id,
        slice_id,
        dpu_id,
        region_id,
        rank_id,
        slice_id,
        dpu_id,
        dpu_program);
}
