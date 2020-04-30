/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_TYPES_H
#define DPU_TYPES_H

/**
 * @file dpu_types.h
 * @brief Base DPU types in the C API.
 */

#include <stdint.h>

#include <dpu_target.h>

/**
 * @def DPU_MAX_NR_CIS
 * @brief Maximum number of Control Interfaces in a DPU rank.
 */
#define DPU_MAX_NR_CIS 8

/**
 * @def DPU_BOOT_THREAD
 * @brief Number of the DPU thread which is launched when booting the DPU.
 */
#define DPU_BOOT_THREAD 0

/**
 * @def DPU_MAX_NR_GROUPS
 * @brief Maximum number of groups in a Control Interface.
 */
#define DPU_MAX_NR_GROUPS 8

/**
 * @typedef dpu_rank_id_t
 * @brief ID of a DPU rank.
 */
typedef uint16_t dpu_rank_id_t;

/**
 * @typedef dpu_id_t
 * @brief ID of a DPU.
 */
typedef uint32_t dpu_id_t;

/**
 * @typedef dpu_slice_id_t
 * @brief ID of a DPU rank slice.
 */
typedef uint8_t dpu_slice_id_t;

/**
 * @typedef dpu_member_id_t
 * @brief ID of a DPU rank slice member.
 */
typedef uint8_t dpu_member_id_t;

/**
 * @typedef dpu_group_id_t
 * @brief ID of a DPU rank slice group.
 */
typedef uint8_t dpu_group_id_t;

/**
 * @typedef dpu_thread_t
 * @brief Index of a DPU thread.
 */
typedef uint8_t dpu_thread_t;

/**
 * @typedef dpu_notify_bit_id_t
 * @brief Index of a DPU notify bit.
 */
typedef uint8_t dpu_notify_bit_id_t;

/**
 * @typedef iram_addr_t
 * @brief Address in IRAM.
 */
typedef uint16_t iram_addr_t;

/**
 * @typedef wram_addr_t
 * @brief Address in MRAM.
 */
typedef uint32_t wram_addr_t;

/**
 * @typedef mram_addr_t
 * @brief Address in MRAM.
 */
typedef uint32_t mram_addr_t;

/**
 * @typedef dpu_mem_max_addr_t
 * @brief Bigger storage for a DPU memory address.
 */
typedef mram_addr_t dpu_mem_max_addr_t;

/**
 * @typedef iram_size_t
 * @brief Size in IRAM.
 */
typedef uint16_t iram_size_t;

/**
 * @typedef wram_size_t
 * @brief Size in WRAM.
 */
typedef uint32_t wram_size_t;

/**
 * @typedef mram_size_t
 * @brief Size in MRAM.
 */
typedef uint32_t mram_size_t;

/**
 * @typedef dpu_mem_max_size_t
 * @brief Bigger storage for a DPU memory size.
 */
typedef mram_size_t dpu_mem_max_size_t;

/**
 * @typedef dpuinstruction_t
 * @brief DPU instruction.
 */
typedef uint64_t dpuinstruction_t;

/**
 * @typedef dpuword_t
 * @brief DPU word in WRAM.
 */
typedef uint32_t dpuword_t;

/**
 * @typedef dpu_bitfield_t
 * @brief Bitfield of DPUs in a CI.
 */
typedef uint8_t dpu_bitfield_t;

#define DPU_BITFIELD_ALL ((dpu_bitfield_t)-1)

/**
 * @typedef dpu_ci_bitfield_t
 * @brief Bitfield of CIs in a rank.
 */
typedef uint8_t dpu_ci_bitfield_t;

typedef enum _dpu_clock_division_t {
    DPU_CLOCK_DIV8 = 0x0,
    DPU_CLOCK_DIV4 = 0x4,
    DPU_CLOCK_DIV3 = 0x3,
    DPU_CLOCK_DIV2 = 0x8,
} dpu_clock_division_t;

enum dpu_slice_target_type {
    DPU_SLICE_TARGET_NONE,
    DPU_SLICE_TARGET_DPU,
    DPU_SLICE_TARGET_ALL,
    DPU_SLICE_TARGET_GROUP,
    NR_OF_DPU_SLICE_TARGETS
};

struct dpu_slice_target {
    enum dpu_slice_target_type type;
    union {
        dpu_member_id_t dpu_id;
        dpu_group_id_t group_id;
    };
};

#define DPU_SLICE_TARGET_TYPE_NAME(target_type)                                                                                  \
    ((((uint32_t)(target_type)) < NR_OF_DPU_SLICE_TARGETS) ? dpu_slice_target_names[target_type]                                 \
                                                           : "DPU_SLICE_TARGET_TYPE_UNKNOWN")
extern const char *dpu_slice_target_names[NR_OF_DPU_SLICE_TARGETS];

typedef enum __attribute((packed)) _dpu_event_kind_t {
    DPU_EVENT_RESET,
    DPU_EVENT_EXTRACT_CONTEXT,
    DPU_EVENT_RESTORE_CONTEXT,
    DPU_EVENT_MRAM_ACCESS_PROGRAM,
    DPU_EVENT_LOAD_PROGRAM,
    DPU_EVENT_DEBUG_ACTION,
} dpu_event_kind_t;

#define DPU_API_DEPRECATED __attribute__((deprecated))

#define DPU_MRAM_HEAP_POINTER_NAME "__sys_used_mram_end"

/**
 * @struct dpu_rank_t
 * @brief DPU rank context.
 */
struct dpu_rank_t;

/**
 * @struct dpu_t
 * @brief DPU context.
 */
struct dpu_t;

typedef enum _dpu_set_kind_t {
    DPU_SET_RANKS,
    DPU_SET_DPU,
} dpu_set_kind_t;

struct dpu_set_t {
    dpu_set_kind_t kind;
    union {
        struct {
            uint32_t nr_ranks;
            struct dpu_rank_t **ranks;
        } list;
        struct dpu_t *dpu;
    };
};

struct dpu_program_t;

/**
 * @struct dpu_symbol_t
 * @brief Information for a symbol from a DPU program.
 */
struct dpu_symbol_t {
    dpu_mem_max_addr_t address;
    dpu_mem_max_size_t size;
};

/**
 * @struct dpu_incbin_t
 * @brief Information on a binary embedded in the program with "DPU_INCBIN"
 */
struct dpu_incbin_t {
    uint8_t *buffer;
    size_t size;
    const char *path;
};

struct dpu_bit_config {
    uint16_t cpu2dpu;
    uint16_t dpu2cpu;
    uint8_t nibble_swap;
    uint8_t stutter;
};

struct dpu_carousel_config {
    uint8_t cmd_duration;
    uint8_t cmd_sampling;
    uint8_t res_duration;
    uint8_t res_sampling;
};

struct dpu_repair_config {
    uint8_t AB_msbs;
    uint8_t CD_msbs;
    uint8_t A_lsbs;
    uint8_t B_lsbs;
    uint8_t C_lsbs;
    uint8_t D_lsbs;
    uint8_t even_index;
    uint8_t odd_index;
};

enum dpu_temperature {
    DPU_TEMPERATURE_LESS_THAN_50 = 0,
    DPU_TEMPERATURE_BETWEEN_50_AND_60 = 1,
    DPU_TEMPERATURE_BETWEEN_60_AND_70 = 2,
    DPU_TEMPERATURE_BETWEEN_70_AND_80 = 3,
    DPU_TEMPERATURE_BETWEEN_80_AND_90 = 4,
    DPU_TEMPERATURE_BETWEEN_90_AND_100 = 5,
    DPU_TEMPERATURE_BETWEEN_100_AND_110 = 6,
    DPU_TEMPERATURE_GREATER_THAN_110 = 7,
};

enum dpu_pc_mode {
    DPU_PC_MODE_12 = 0,
    DPU_PC_MODE_13 = 1,
    DPU_PC_MODE_14 = 2,
    DPU_PC_MODE_15 = 3,
    DPU_PC_MODE_16 = 4,
};

#endif // DPU_TYPES_H
