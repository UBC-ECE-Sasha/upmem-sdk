/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_PROFILE_H
#define DPU_PROFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include "dpu_properties.h"

// todo: to be generated?

/* All */
#define DPU_PROFILE_PROPERTY_BACKEND "backend"
#define DPU_PROFILE_PROPERTY_CHIP_ID "chipId"
#define DPU_PROFILE_PROPERTY_TRY_REPAIR_IRAM "tryRepairIram"
#define DPU_PROFILE_PROPERTY_TRY_REPAIR_WRAM "tryRepairWram"
#define DPU_PROFILE_PROPERTY_CLOCK_DIVISION "clockDivision"
#define DPU_PROFILE_PROPERTY_FCK_FREQUENCY "fckFrequency"
#define DPU_PROFILE_PROPERTY_ENABLE_PROFILING                                                                                    \
    "enableProfiling" // "nop": nopify the mcount call
                      // "statistics": use statistical profiling
                      // "mcount": use mcount implementation (default profiling option)
                      // "samples": use the debug unit to sample the PC of random running threads
#define DPU_PROFILE_PROPERTY_PROFILING_DPU_ID "profilingDpuId" // default is 0
#define DPU_PROFILE_PROPERTY_PROFILING_SLICE_ID "profilingSliceId" // default is 0
#define DPU_PROFILE_PROPERTY_MCOUNT_ADDRESS "mcountAddress" // Instruction address (not byte address)
#define DPU_PROFILE_PROPERTY_RET_MCOUNT_ADDRESS "retMcountAddress" // Instruction address (not byte address)
#define DPU_PROFILE_PROPERTY_THREAD_PROFILING_ADDRESS "threadProfilingAddress" // Instruction address (not byte address)
#define DPU_PROFILE_PROPERTY_MRAM_ACCESS_BY_DPU_ONLY "mramAccessByDpuOnly"
#define DPU_PROFILE_PROPERTY_DISABLED_MASK "disabledMask"
#define DPU_PROFILE_PROPERTY_DISABLE_API_SAFE_CHECKS "disableSafeChecks"
#define DPU_PROFILE_PROPERTY_DISABLE_MUX_SWITCH "disableMuxSwitch"

/* Fsim */
#define DPU_PROFILE_PROPERTY_NR_OF_DPUS_PER_CI "nrDpusPerCI"
#define DPU_PROFILE_PROPERTY_MRAM_SIZE "mramSize"
#define DPU_PROFILE_PROPERTY_WRAM_SIZE "wramSize"
#define DPU_PROFILE_PROPERTY_IRAM_SIZE "iramSize"
#define DPU_PROFILE_PROPERTY_HAS_DBG_MRAM "hasDbgMram"
#define DPU_PROFILE_PROPERTY_SIMULATE_CYCLE_ACCURACY "simulateCycleAccuracy"
#define DPU_PROFILE_PROPERTY_PIPELINE_DEPTH "pipelineDepth"
#define DPU_PROFILE_PROPERTY_DO_PROFILING "doProfiling"
#define DPU_PROFILE_PROPERTY_PROFILE_INSTRUCTION "profileInstruction"
#define DPU_PROFILE_PROPERTY_PROFILE_DMA "profileDma"
#define DPU_PROFILE_PROPERTY_PROFILE_MEMORY "profileMemory"
#define DPU_PROFILE_PROPERTY_PROFILE_ILA "profileIla"

/* Modelsim */
#define DPU_PROFILE_PROPERTY_MODELSIM_MRAM_FILE "modelsimMramFile"
#define DPU_PROFILE_PROPERTY_MODELSIM_WORK_DIR "modelsimWorkDir"
#define DPU_PROFILE_PROPERTY_MODELSIM_EXEC_PATH "modelsimExecPath"
#define DPU_PROFILE_PROPERTY_MODELSIM_TCL_SCRIPT "modelsimTclScript"
#define DPU_PROFILE_PROPERTY_MODELSIM_ENABLE_GUI "modelsimEnableGui"
#define DPU_PROFILE_PROPERTY_MODELSIM_BYPASS_MRAM_SWITCH "modelsimBypassMramSwitch"

/* FPGA */
#define DPU_PROFILE_PROPERTY_FPGA_ILA_REPORT_PATH "reportFileName"
#define DPU_PROFILE_PROPERTY_FPGA_ILA_ENABLED "analyzerEnabled"
#define DPU_PROFILE_PROPERTY_FPGA_ILA_FILTERING_ENABLED "analyzerFilteringEnabled"

#define DPU_PROFILE_PROPERTY_FPGA_ILA_MRAM_BYPASS_ENABLED "mramBypassEnabled"
#define DPU_PROFILE_PROPERTY_FPGA_ILA_MRAM_EMUL_REFRESH_ENABLED "mramEmulateRefresh"

#define DPU_PROFILE_PROPERTY_FPGA_CYCLE_ACCURATE "cycleAccurate"

/* HW */
#define DPU_PROFILE_PROPERTY_HW_REGION_MODE "regionMode"
#define DPU_PROFILE_PROPERTY_RANK_PATH "rankPath"
#define DPU_PROFILE_PROPERTY_MODULE_COMPAT "ignoreVersion"

/* Backup SPI */
#define DPU_PROFILE_PROPERTY_BACKUP_SPI_USB_SERIAL "usbSerial"
#define DPU_PROFILE_PROPERTY_BACKUP_SPI_CHIP_SELECT "chipSelect"

static inline bool
fetch_integer_property(dpu_properties_t properties, const char *name, uint32_t *value, uint32_t default_value)
{
    const char *value_string;
    char *tailptr;
    long int parsed;

    if ((value_string = dpu_properties_get(properties, name)) == DPU_PROPERTIES_NO_SUCH_PROPERTY) {
        *value = default_value;
        return true;
    }

    if ((*value_string == '\0') || isspace((unsigned char)*value_string)) {
        return false;
    }

    errno = 0;
    parsed = strtol(value_string, &tailptr, 0);

    if ((*tailptr != '\0') || (parsed > UINT_MAX) || (errno == ERANGE && parsed == LONG_MAX)
        || (errno == ERANGE && parsed == LONG_MIN)) {
        return false;
    }

    *value = (uint32_t)parsed;
    return true;
}

static inline bool
fetch_long_property(dpu_properties_t properties, const char *name, uint64_t *value, uint64_t default_value)
{
    const char *value_string;
    char *tailptr;
    unsigned long int parsed;

    if ((value_string = dpu_properties_get(properties, name)) == DPU_PROPERTIES_NO_SUCH_PROPERTY) {
        *value = default_value;
        return true;
    }

    if ((*value_string == '\0') || isspace((unsigned char)*value_string)) {
        return false;
    }

    errno = 0;
    parsed = strtoul(value_string, &tailptr, 0);

    if ((*tailptr != '\0') || (errno == ERANGE && parsed == ULONG_MAX)) {
        return false;
    }

    *value = (uint64_t)parsed;
    return true;
}

static inline bool
fetch_boolean_property(dpu_properties_t properties, const char *name, bool *value, bool default_value)
{
    const char *value_string;

    if ((value_string = dpu_properties_get(properties, name)) == DPU_PROPERTIES_NO_SUCH_PROPERTY) {
        *value = default_value;
        return true;
    }

    *value = (strcasecmp("false", value_string) != 0) && (strcasecmp("0", value_string) != 0);
    return true;
}

static inline bool
fetch_string_property(dpu_properties_t properties, const char *name, char **value, const char *default_value)
{
    const char *value_string;

    if ((value_string = dpu_properties_get(properties, name)) == DPU_PROPERTIES_NO_SUCH_PROPERTY) {
        value_string = default_value;
    }

    if (value_string == NULL) {
        *value = NULL;
    } else {
        if ((*value = malloc(strlen(value_string) + 1)) == NULL) {
            return false;
        }

        strcpy(*value, value_string);
    }

    return true;
}

#endif // DPU_PROFILE_H
