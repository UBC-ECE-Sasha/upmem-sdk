/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_VPD_H
#define DPU_VPD_H

#include <stdint.h>

/**
 * @file dpu_vpd.h
 * @brief Definition of SRAM repair related structures.
 */

/* Support up to quad-rank DIMMs */
#define VPD_MAX_RANK 4

#define VPD_STRUCT_ID "UPMV"
#define VPD_STRUCT_VERSION (0x0002000)

#define VPD_REPAIR_IRAM 0
#define VPD_REPAIR_WRAM 1

struct repair_entry {
    uint8_t iram_wram; /* VPD_REPAIR_IRAM or VPD_REPAIR_WRAM */
    uint8_t rank;
    uint8_t ci;
    uint8_t dpu;
    uint8_t bank;
    uint8_t __padding;
    uint16_t address;
    uint64_t bits; /* bits to repair */
};

struct rank_data {
    uint64_t dpu_disabled; /* bitmap of disabled DPUs */
    uint64_t wram_repair; /* bitmap of WRAM requiring repairs (1 bit per DPU) */
    uint64_t iram_repair; /* bitmap of IRAM requiring repairs (1 bit per DPU) */
};

/* UPMEM DIMM Vital Product Data */
struct dimm_vpd {
    char struct_id[4]; /* contains 'U','P','M', 'V' for UPMEM VPD */
    uint32_t struct_ver; /* 0x0002000 for version 2.0 */
    uint8_t rank_count; /* total number of ranks on the DIMM */
    uint8_t __padding_0;
    uint16_t repair_count; /* number of entries in the SRAM repairs list */
    uint16_t fck_mhz; /* default or unique FCK frequency */
    uint16_t fck_min_mhz;
    uint16_t fck_max_mhz;
    uint8_t div_min;
    uint8_t div_max;
    uint32_t __padding_1;
    char sticker_name[16]; /* human friendly name printed on the DIMM e.g. B62 */
    struct rank_data ranks[VPD_MAX_RANK];
    /* followed by the SRAM repairs list with `repair_count` entries */
};

#endif /* DPU_VPD_H */
