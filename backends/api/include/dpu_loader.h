/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_LOADER_H
#define DPU_LOADER_H

#include <stdbool.h>
#include <stdint.h>

#include <dpu_types.h>
#include <dpu_error.h>
#include <dpu_elf.h>

/**
 * @file dpu_loader.h
 * @brief C API for ELF program loading on DPUs.
 */

/**
 * @enum dpu_loader_target_t
 * @brief Target destination for the DPU loader.
 * @var DPU_LOADER_TARGET_RANK loader will target a DPU rank
 * @var DPU_LOADER_TARGET_DPU  loader will target a DPU
 */
typedef enum _dpu_loader_target_t { DPU_LOADER_TARGET_RANK, DPU_LOADER_TARGET_DPU } dpu_loader_target_t;

/**
 * @struct dpu_loader_env_t
 * @brief DPU Loader environment, containing its target.
 */
typedef struct _dpu_loader_env_t {
    dpu_loader_target_t target;
    union {
        struct dpu_rank_t *rank;
        struct dpu_t *dpu;
    };
} * dpu_loader_env_t;

/**
 * @typedef mem_patch_function_t
 * @brief A function that will be called before loading content into memory to modify the content.
 */
typedef dpu_error_t (
    *mem_patch_function_t)(dpu_loader_env_t env, void *content, dpu_mem_max_addr_t address, dpu_mem_max_size_t size, bool init);

/**
 * @struct dpu_loader_context_t
 * @brief DPU Loader context.
 * @var patch_iram IRAM patch function
 * @var patch_wram WRAM patch function
 * @var patch_mram MRAM patch function
 * @var nr_of_instructions current number of loaded instructions
 * @var nr_of_wram_words current number of loaded WRAM words
 * @var nr_of_mram_bytes current number of loaded MRAM bytes
 * @var env DPU Loader environment
 */
typedef struct _dpu_loader_context_t {
    mem_patch_function_t patch_iram;
    mem_patch_function_t patch_wram;
    mem_patch_function_t patch_mram;

    uint32_t nr_of_instructions;
    uint32_t nr_of_wram_words;
    uint32_t nr_of_mram_bytes;
    uint32_t dummy;

    struct _dpu_loader_env_t env;
} * dpu_loader_context_t;

/**
 * @fn dpu_loader_fill_dpu_context
 * @brief Set up a DPU Loader context targetting the specified DPU.
 * @param context The context to fill
 * @param dpu The loader target
 */
void
dpu_loader_fill_dpu_context(dpu_loader_context_t context, struct dpu_t *dpu);

/**
 * @fn dpu_loader_fill_rank_context
 * @brief Set up a DPU Loader context targetting the specified DPU rank.
 * @param context The context to fill
 * @param rank The loader target
 */
void
dpu_loader_fill_rank_context(dpu_loader_context_t context, struct dpu_rank_t *rank);

/**
 * @fn dpu_elf_load
 * @brief Loads the specified ELF file using the specified DPU loader context
 *        to determine the patch functions and target.
 * @param file the ELF file to be loaded
 * @param context the DPU Loader context
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_elf_load(dpu_elf_file_t file, dpu_loader_context_t context);

#endif // DPU_LOADER_H
