/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_MEMORY_H
#define DPU_MEMORY_H

#include <dpu_error.h>
#include <dpu_types.h>
#include <dpu_transfer_mram.h>

/**
 * @file dpu_memory.h
 * @brief C API to access DPU memory.
 */

/**
 * @def DPU_PRIMARY_MRAM
 * @brief ID for the main MRAM.
 */
#define DPU_PRIMARY_MRAM 0

/**
 * @def DPU_DEBUG_MRAM
 * @brief ID for the debug MRAM.
 */
#define DPU_DEBUG_MRAM 1

/**
 * @fn dpu_copy_to_symbol_dpu
 * @brief Copy data from the Host memory buffer to one of the DPU memories.
 * @param dpu the DPU
 * @param symbol the DPU symbol where the data is copied
 * @param symbol_offset the byte offset from the base DPU symbol address where to copy the data
 * @param src the host buffer containing the data to copy
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_symbol_dpu(struct dpu_t *dpu, struct dpu_symbol_t symbol, uint32_t symbol_offset, const void *src, size_t length);

/**
 * @fn dpu_copy_to_symbol_rank
 * @brief Copy data from the Host memory buffer to one of the DPU memories on the whole DPU rank.
 * @param dpu the DPU rank
 * @param symbol the DPU symbol where the data is copied
 * @param symbol_offset the byte offset from the base DPU symbol address where to copy the data
 * @param src the host buffer containing the data to copy
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_symbol_rank(struct dpu_rank_t *rank,
    struct dpu_symbol_t symbol,
    uint32_t symbol_offset,
    const void *src,
    size_t length);

/**
 * @fn dpu_copy_to_symbol_matrix
 * @brief Copy data from the Host memory buffers previously defined with `dpu_prepare_xfer` to one of the DPU memories on the
 * whole DPU rank.
 * @param dpu the DPU rank
 * @param symbol the DPU symbol where the data is copied
 * @param symbol_offset the byte offset from the base DPU symbol address where to copy the data
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_symbol_matrix(struct dpu_rank_t *rank, struct dpu_symbol_t symbol, uint32_t symbol_offset, size_t length);

/**
 * @fn dpu_copy_to_address_dpu
 * @brief Copy data from the Host memory buffer to one of the DPU memories.
 * @param dpu the DPU
 * @param address the DPU address where the data is copied
 * @param src the host buffer containing the data to copy
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_address_dpu(struct dpu_t *dpu, dpu_mem_max_addr_t address, const void *src, dpu_mem_max_size_t length);

/**
 * @fn dpu_copy_to_address_rank
 * @brief Copy data from the Host memory buffer to one of the DPU memories on the whole DPU rank.
 * @param rank the DPU rank
 * @param address the DPU address where the data is copied
 * @param src the host buffer containing the data to copy
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_address_rank(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, const void *src, dpu_mem_max_size_t length);

/**
 * @fn dpu_copy_to_address_matrix
 * @brief Copy data from the Host memory buffers previously defined with `dpu_prepare_xfer` to one of the DPU memories on the
 * whole DPU rank.
 * @param rank the DPU rank
 * @param address the DPU address where the data is copied
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_address_matrix(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, dpu_mem_max_size_t length);

/**
 * @fn dpu_copy_from_symbol_dpu
 * @brief Copy data from one of the DPU memories to the Host memory buffer.
 * @param dpu the DPU
 * @param symbol the DPU symbol from where the data is copied
 * @param symbol_offset the byte offset from the base DPU symbol address from where to copy the data
 * @param src the host buffer where the data is copied
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_symbol_dpu(struct dpu_t *dpu, struct dpu_symbol_t symbol, uint32_t symbol_offset, void *dst, size_t length);

/**
 * @fn dpu_copy_from_symbol_matrix
 * @brief Copy data from one of the DPU memories to the Host memory buffers previously defined with `dpu_prepare_xfer` on the
 * whole DPU rank.
 * @param rank the DPU rank
 * @param symbol the DPU symbol from where the data is copied
 * @param symbol_offset the byte offset from the base DPU symbol address from where to copy the data
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_symbol_matrix(struct dpu_rank_t *rank, struct dpu_symbol_t symbol, uint32_t symbol_offset, size_t length);

/**
 * @fn dpu_copy_from_address_dpu
 * @brief Copy data from one of the DPU memories to the Host memory buffer.
 * @param dpu the DPU
 * @param address the DPU address from where the data is copied
 * @param src the host buffer where the data is copied
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_address_dpu(struct dpu_t *dpu, dpu_mem_max_addr_t address, void *dst, dpu_mem_max_size_t length);

/**
 * @fn dpu_copy_from_address_matrix
 * @brief Copy data from one of the DPU memories to the Host memory buffers previously defined with `dpu_prepare_xfer` on the
 * whole DPU rank.
 * @param rank the DPU rank
 * @param address the DPU address from where the data is copied
 * @param length the number of bytes to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_address_matrix(struct dpu_rank_t *rank, dpu_mem_max_addr_t address, dpu_mem_max_size_t length);

/**
 * @fn dpu_copy_to_iram_for_rank
 * @brief Copy some instructions to the IRAM of all DPUs of a rank.
 * @param rank the DPU rank
 * @param iram_instruction_index where to start to copy the instructions in IRAM
 * @param source the buffer containing the instructions to copy
 * @param nb_of_instructions the number of instructions to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_iram_for_rank(struct dpu_rank_t *rank,
    iram_addr_t iram_instruction_index,
    const dpuinstruction_t *source,
    iram_size_t nb_of_instructions);

/**
 * @fn dpu_copy_to_iram_for_dpu
 * @brief Copy some instructions to the IRAM of a DPU.
 * @param dpu the DPU
 * @param iram_instruction_index where to start to copy the instructions in IRAM
 * @param source the buffer containing the instructions to copy
 * @param nb_of_instructions the number of instructions to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_iram_for_dpu(struct dpu_t *dpu,
    iram_addr_t iram_instruction_index,
    const dpuinstruction_t *source,
    iram_size_t nb_of_instructions);

/**
 * @fn dpu_copy_from_iram_for_dpu
 * @brief Copy some instructions from the IRAM of a DPU.
 * @param dpu the DPU
 * @param destination the buffer where the instructions will be stored
 * @param iram_instruction_index where to start to copy the instructions from IRAM
 * @param nb_of_instructions the number of instructions to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_iram_for_dpu(struct dpu_t *dpu,
    dpuinstruction_t *destination,
    iram_addr_t iram_instruction_index,
    iram_size_t nb_of_instructions);

/**
 * @fn dpu_copy_to_wram_for_rank
 * @brief Copy some words to the WRAM of all DPUs of a rank.
 * @param rank the DPU rank
 * @param wram_word_offset where to start to copy the words in WRAM
 * @param source the buffer containing the data to copy
 * @param nb_of_words the number of words to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_wram_for_rank(struct dpu_rank_t *rank,
    wram_addr_t wram_word_offset,
    const dpuword_t *source,
    wram_size_t nb_of_words);

/**
 * @fn dpu_copy_to_wram_for_dpu
 * @brief Copy some words to the WRAM of a DPU.
 * @param dpu the DPU
 * @param wram_word_offset where to start to copy the words in WRAM
 * @param source the buffer containing the data to copy
 * @param nb_of_words the number of words to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_to_wram_for_dpu(struct dpu_t *dpu, wram_addr_t wram_word_offset, const dpuword_t *source, wram_size_t nb_of_words);

/**
 * @fn dpu_copy_from_wram_for_dpu
 * @brief Copy some words from the WRAM of a DPU.
 * @param dpu the DPU
 * @param destination the buffer where the words will be stored
 * @param wram_word_offset where to start to copy the words from WRAM
 * @param nb_of_words the number of words to copy
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_copy_from_wram_for_dpu(struct dpu_t *dpu, dpuword_t *destination, wram_addr_t wram_word_offset, wram_size_t nb_of_words);

/**
 * @fn dpu_copy_to_mram
 * @brief Copy data to the MRAM of a DPU.
 * @param dpu the DPU
 * @param mram_byte_offset offset in wram from where to start the copy
 * @param source the data buffer for the MRAM data
 * @param nb_of_bytes the number of bytes to copy
 * @param mram_number the identifier of the MRAM to copy from
 * @return ``DPU_OK`` in case of success, ``DPU_ERR_MRAM_BUSY`` means the host cannot write at the moment to the
 * MRAM and the access must be retried later, another value precising the error otherwise.
 */
dpu_error_t
dpu_copy_to_mram(struct dpu_t *dpu,
    mram_addr_t mram_byte_offset,
    const uint8_t *source,
    mram_size_t nb_of_bytes,
    uint8_t mram_number);

/**
 * @fn dpu_copy_from_mram
 * @brief Copy data from the MRAM of a DPU.
 * @param dpu the DPU
 * @param destination the data buffer for the MRAM data
 * @param mram_byte_offset offset in wram from where to start the copy
 * @param nb_of_bytes the number of bytes to copy
 * @param mram_number the identifier of the MRAM to copy from
 * @return ``DPU_OK`` in case of success, ``DPU_ERR_MRAM_BUSY`` means the host cannot write at the moment to the
 * MRAM and the access must be retried later, another value precising the error otherwise.
 */
dpu_error_t
dpu_copy_from_mram(struct dpu_t *dpu,
    uint8_t *destination,
    mram_addr_t mram_byte_offset,
    mram_size_t nb_of_bytes,
    uint8_t mram_number);

/**
 * @fn dpu_copy_to_mrams
 * @brief Copy data to the MRAMs.
 * @param rank the DPU rank
 * @param transfer_matrix Dynamically allocated matrix[#dpus_per_slice][#slice] describing the transfers for the whole rank
 * @return ``DPU_OK`` in case of success, ``DPU_ERR_MRAM_BUSY`` means the host cannot write at the moment to the
 * MRAM and the access must be retried later, another value precising the error otherwise.
 */
dpu_error_t
dpu_copy_to_mrams(struct dpu_rank_t *rank, const struct dpu_transfer_mram *transfer_matrix);

/**
 * @fn dpu_copy_from_mrams
 * @brief Copy data from the MRAMs.
 * @param rank the DPU rank
 * @param transfer_matrix Dynamically allocated matrix[#dpus_per_slice][#slice] describing the transfers for the whole rank
 * @return ``DPU_OK`` in case of success, ``DPU_ERR_MRAM_BUSY`` means the host cannot write at the moment to the
 * MRAM and the access must be retried later, another value precising the error otherwise.
 */
dpu_error_t
dpu_copy_from_mrams(struct dpu_rank_t *rank, struct dpu_transfer_mram *transfer_matrix);

#endif // DPU_MEMORY_H
