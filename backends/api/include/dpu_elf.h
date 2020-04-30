/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_ELF_H
#define DPU_ELF_H

#include <stdint.h>
#include <stdbool.h>
#include <dpu_error.h>

#define EM_DPU 245

/**
 * @file dpu_elf.h
 * @brief General purpose ELF loader, able to load and map any section
 *
 * Purpose of this loader is to provide an easy to use interface to upper level applications, in particular from
 * Java world, with functions to load and read ELF files, getting sections and associated symbols in a map-like
 * form.
 *
 * The first operation consists of loading the ELF file, using dpu_elf_open, or directly from memory, using dpu_elf_map.
 * Both functions return a "DPU elf descriptor".
 * The descriptor is used as reference to other functions and must be deleted with dpu_elf_close when finished.
 *
 * The information extracted from a file are:
 *   - The list of sections (dpu_elf_get_sections)
 *   - The contents of a given section (dpu_elf_load_section)
 *   - The symbols defined in a given section (dpu_elf_load_symbols)
 *   - The runtime information fetched from symbols (dpu_elf_get_runtime_info)
 */

/**
 * @struct dpu_elf_symbol_t
 * @brief Description of one symbol in its section
 * @var name the symbol name
 * @var value associated value
 * @var size associated size
 */
typedef struct dpu_elf_symbol {
    char *name;
    unsigned int value;
    unsigned int size;
} dpu_elf_symbol_t;

/**
 * @struct dpu_elf_symbols_t
 * @brief A map of symbols
 * @var nr_symbols number of enclosed symbols
 * @var symbols a table of nr_symbols descriptors
 */
typedef struct dpu_elf_symbols {
    unsigned int nr_symbols;
    dpu_elf_symbol_t *map;
} dpu_elf_symbols_t;

/**
 * @struct runtime_info_item_t
 * @brief One runtime information fetched (or not) from ELF file
 * @var exists whether the symbol defining the runtime information was found in the ELF file
 * @var value if exists, the fetched value, a default value otherwise
 * @var size if exists, the size of the symbol, `0` otherwise
 */
typedef struct _dpu_elf_runtime_info_item {
    uint32_t value;
    uint32_t size;
    bool exists;
} dpu_elf_runtime_info_item_t;

/**
 * @struct runtime_info_t
 * @brief Structure describing the runtime information of a DPU program
 */
typedef struct _dpu_elf_runtime_info {
    dpu_elf_runtime_info_item_t sys_heap_pointer_reset;
    dpu_elf_runtime_info_item_t sys_heap_pointer;
    dpu_elf_runtime_info_item_t sys_wq_table;
    dpu_elf_runtime_info_item_t sys_stack_table;
    dpu_elf_runtime_info_item_t printf_buffer;
    dpu_elf_runtime_info_item_t printf_state;
    dpu_elf_runtime_info_item_t sys_end;
    dpu_elf_runtime_info_item_t mcount;
    dpu_elf_runtime_info_item_t ret_mcount;
    dpu_elf_runtime_info_item_t thread_profiling;
    dpu_elf_runtime_info_item_t nr_threads;
    dpu_elf_runtime_info_item_t open_print_sequence;
    dpu_elf_runtime_info_item_t close_print_sequence;
    dpu_elf_symbols_t mutex_info;
    dpu_elf_symbols_t semaphore_info;
    dpu_elf_symbols_t barrier_info;
} dpu_elf_runtime_info_t;

/**
 * @typedef dpu_elf_file_t
 * @brief A "file pointer" to an ELF file
 *
 * Such a descriptor is created by dpu_elf_load_file, as a reference for subsequent requests. It must be
 * destroyed with dpu_elf_delete.
 */
typedef void *dpu_elf_file_t;

/**
 * @fn dpu_elf_open
 * @brief Loads an ELF file
 * @param path path to the ELF file
 * @param file set to the created file descriptor in case of success
 * @return DPU_OK, or an error code if the file is corrupted or not found
 */
dpu_error_t
dpu_elf_open(const char *path, dpu_elf_file_t *file);

/**
 * @fn dpu_elf_map
 * @brief Loads an ELF file stored in memory
 * @param buffer the buffer containing the ELF file contents
 * @param size   buffer size, in bytes
 * @param file   set to the created file descriptor in case of success
 * @param filename_or_null   the original file name if it exists, NULL otherwise
 * @return DPU_OK, or an error code if the file is corrupted
 */
dpu_error_t
dpu_elf_map(uint8_t *memory, unsigned int size, dpu_elf_file_t *file, const char *filename_or_null);

/**
 * @fn dpu_elf_close
 * @brief Deletes a DPU ELF file descriptor
 * @param file the deleted structure
 */
void
dpu_elf_close(dpu_elf_file_t file);

/**
 * @fn dpu_elf_load_section
 * @brief Reads the contents of a given section
 * @param file the DPU ELF file descriptor
 * @param name requested section name
 * @param size set to the section size, in bytes
 * @param buffer set to the section contents
 * @return Whether this section was loaded or an error occurred (typically DPU_ERR_ELF_NO_SUCH_SECTION)
 */
dpu_error_t
dpu_elf_load_section(dpu_elf_file_t file, const char *name, unsigned int *size, uint8_t **buffer);

/**
 * @fn dpu_elf_load_symbols
 * @brief Reads the symbols of a given section, returning their names and values.
 *
 * Notice that the function performs common "fixup" operations for specific sections (e.g. IRAM
 * address translation).
 *
 * The returned map must be free with dpu_elf_free_symbols.
 *
 * @param file the DPU ELF file descriptor
 * @param name requested section name
 * @param symbols set to the requested symbols map
 * @return Whether the symbols were found or an error occurred (typically DPU_ERR_ELF_NO_SUCH_SECTION).
 */
dpu_error_t
dpu_elf_load_symbols(dpu_elf_file_t file, const char *name, dpu_elf_symbols_t **symbols);

/**
 * @fn dpu_elf_free_symbols
 * @brief Frees symbols loaded by dpu_elf_load_symbols.
 * @param symbols the symbols to free
 */
void
dpu_elf_free_symbols(dpu_elf_symbols_t *symbols);

/**
 * @fn dpu_elf_get_sections
 * @brief Returns the list of sections found in the ELF file
 * @param file the DPU ELF file descriptor
 * @param nr_sections set to the number of sections found
 * @param sections set to an array of nr_sections section names, which must be freed when used
 * @return Whether the section list was successfully fetched
 */
dpu_error_t
dpu_elf_get_sections(dpu_elf_file_t file, unsigned int *nr_sections, char ***sections);

/**
 * @fn dpu_elf_get_runtime_info
 * @brief Parses the loaded symbol to construct the program runtime information
 *
 * Resulting information may either exist (found the proper symbol) or not (no such symbol found).
 *
 * @param file the DPU ELF file descriptor
 * @param runtime_info filled with the fetched information
 */
void
dpu_elf_get_runtime_info(dpu_elf_file_t file, dpu_elf_runtime_info_t *runtime_info);

/**
 * @fn free_runtime_info
 * @param runtime_info the structure to free
 */
void
free_runtime_info(dpu_elf_runtime_info_t *runtime_info);

dpu_error_t
dpu_elf_create_core_dump(const char *exe_path,
    const char *core_file_path,
    uint8_t *wram,
    uint8_t *mram,
    uint8_t *iram,
    uint8_t *context,
    uint32_t wram_size,
    uint32_t mram_size,
    uint32_t iram_size,
    uint32_t context_size);

#endif // DPU_ELF_H
