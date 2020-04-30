/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_PROGRAM_H
#define DPU_PROGRAM_H

#include <stdint.h>
#include <stdbool.h>

#include <dpu_types.h>
#include <dpu_error.h>
#include <dpu_elf.h>

/**
 * @file dpu_program.h
 * @brief C API for DPU program operations.
 */

struct dpu_program_t {
    uint8_t nr_threads_enabled;
    int32_t printf_buffer_address;
    int32_t printf_buffer_size;
    int32_t printf_write_pointer_address;
    int32_t printf_buffer_has_wrapped_address;
    dpu_elf_symbols_t *symbols;

    int32_t mcount_address;
    int32_t ret_mcount_address;
    int32_t thread_profiling_address;

    int32_t open_print_sequence_addr;
    int32_t close_print_sequence_addr;

    uint32_t reference_count;

    char *program_path;
};

void
dpu_init_program_ref(struct dpu_program_t *program);
void
dpu_take_program_ref(struct dpu_program_t *program);
void
dpu_free_program(struct dpu_program_t *program);

/**
 * @fn dpu_get_program
 * @brief Fetches the program runtime context of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @return The pointer on the runtime context of the DPU.
 */
struct dpu_program_t *
dpu_get_program(struct dpu_t *dpu);

/**
 * @fn dpu_set_program
 * @brief Sets the program runtime context of the specified DPU.
 * @param dpu the unique identifier of the DPU
 * @param program the value of the DPU program runtime context
 */
void
dpu_set_program(struct dpu_t *dpu, struct dpu_program_t *program);

/**
 * @fn dpu_load_elf_program_from_incbin
 * @param elf_info raw ELF information on the program stored when loading
 * @param path the path to print for debug purpose
 * @param buffer_start the in-memory buffer to load
 * @param buffer_size the size of the buffer
 * @param program information on the ELF program stored when loading
 * @param mram_size_hint size of the MRAM, to adjust some symbols
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_load_elf_program_from_memory(dpu_elf_file_t *elf_info,
    const char *path,
    uint8_t *buffer_start,
    size_t buffer_size,
    struct dpu_program_t *program,
    mram_size_t mram_size_hint);

/**
 * @fn dpu_load_elf_program
 * @param elf_info raw ELF information on the program stored when loading
 * @param path the ELF binary file path
 * @param program information on the ELF program stored when loading
 * @param mram_size_hint size of the MRAM, to adjust some symbols
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_load_elf_program(dpu_elf_file_t *elf_info, const char *path, struct dpu_program_t *program, mram_size_t mram_size_hint);

#endif // DPU_PROGRAM_H
