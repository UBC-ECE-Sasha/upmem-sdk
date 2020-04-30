/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_ERROR_H
#define DPU_ERROR_H

/**
 * @file dpu_error.h
 * @brief Define the possible status returned by the API functions.
 */

/**
 * @enum dpu_error_t
 * @brief Status returned by any API operation to specify the success or failure of the operation.
 * @var DPU_OK                                  The operation was successful.
 * @var DPU_ERR_INTERNAL                        Error returned when there is an unexpected error in the API.
 * @var DPU_ERR_SYSTEM                          Error returned when a system function (eg. malloc) failed.
 * @var DPU_ERR_DRIVER                          Error returned when there is an unexpected error in the rank driver.
 * @var DPU_ERR_ALLOCATION                      Error returned when there is an error during the rank allocation.
 * @var DPU_ERR_INVALID_DPU_SET                 Error returned when trying to do an invalid operation on a DPU set.
 * @var DPU_ERR_INVALID_THREAD_ID               Error returned when the specified thread id is invalid.
 * @var DPU_ERR_INVALID_NOTIFY_ID               Error returned when the specified notify id is invalid.
 * @var DPU_ERR_INVALID_WRAM_ACCESS             Error returned when the WRAM access parameters are invalid.
 * @var DPU_ERR_INVALID_IRAM_ACCESS             Error returned when the IRAM access parameters are invalid.
 * @var DPU_ERR_INVALID_MRAM_ACCESS             Error returned when the MRAM access parameters are invalid.
 * @var DPU_ERR_INVALID_SYMBOL_ACCESS           Error returned when the symbol access parameters are invalid.
 * @var DPU_ERR_MRAM_BUSY                       Error returned when the DPU MRAM is not accessible to the host.
 * @var DPU_ERR_TRANSFER_ALREADY_SET            Error returned when trying to prepare a memory transfer on an already prepared dpu
 * @var DPU_ERR_DIFFERENT_DPU_PROGRAMS          Error returned when fetching a DPU symbol on DPUs with different programs
 * @var DPU_ERR_CORRUPTED_MEMORY                Error returned when the DPU MRAM is corrupted and cannot be repaired.
 * @var DPU_ERR_DPU_DISABLED                    Error returned when accessing a disabled DPU.
 * @var DPU_ERR_DPU_ALREADY_RUNNING             Error returned when trying to boot an already running DPU.
 * @var DPU_ERR_INVALID_MEMORY_TRANSFER         Error returned when the specified memory transfer is unknown.
 * @var DPU_ERR_INVALID_LAUNCH_POLICY           Error returned when the specified launch policy is unknown.
 * @var DPU_ERR_DPU_FAULT                       Error returned when the DPU is in fault.
 * @var DPU_ERR_ELF_INVALID_FILE                Error returned when the ELF file is invalid.
 * @var DPU_ERR_ELF_NO_SUCH_FILE                Error returned when the ELF file is not found.
 * @var DPU_ERR_ELF_NO_SUCH_SECTION             Error returned when the section is not found in the ELF file.
 * @var DPU_ERR_TIMEOUT                         Error returned when the operation timed out.
 * @var DPU_ERR_INVALID_PROFILE                 Error returned when the DPU profile passed during allocation is invalid.
 * @var DPU_ERR_UNKNOWN_SYMBOL                  Error returned when fetching an undefined symbol in a DPU program.
 * @var DPU_ERR_LOG_FORMAT                      Error returned when the parsing of the log buffer is not what we expect.
 * @var DPU_ERR_LOG_CONTEXT_MISSING             Error returned when we cannot find log information in the dpu structure.
 * @var DPU_ERR_LOG_BUFFER_TOO_SMALL            Error returned when the log buffer was too small to contain all messages
 */
typedef enum dpu_error_t {
    DPU_OK,
    DPU_ERR_INTERNAL,
    DPU_ERR_SYSTEM,
    DPU_ERR_DRIVER,
    DPU_ERR_ALLOCATION,
    DPU_ERR_INVALID_DPU_SET,
    DPU_ERR_INVALID_THREAD_ID,
    DPU_ERR_INVALID_NOTIFY_ID,
    DPU_ERR_INVALID_WRAM_ACCESS,
    DPU_ERR_INVALID_IRAM_ACCESS,
    DPU_ERR_INVALID_MRAM_ACCESS,
    DPU_ERR_INVALID_SYMBOL_ACCESS,
    DPU_ERR_MRAM_BUSY,
    DPU_ERR_TRANSFER_ALREADY_SET,
    DPU_ERR_DIFFERENT_DPU_PROGRAMS,
    DPU_ERR_CORRUPTED_MEMORY,
    DPU_ERR_DPU_DISABLED,
    DPU_ERR_DPU_ALREADY_RUNNING,
    DPU_ERR_INVALID_MEMORY_TRANSFER,
    DPU_ERR_INVALID_LAUNCH_POLICY,
    DPU_ERR_DPU_FAULT,
    DPU_ERR_ELF_INVALID_FILE,
    DPU_ERR_ELF_NO_SUCH_FILE,
    DPU_ERR_ELF_NO_SUCH_SECTION,
    DPU_ERR_TIMEOUT,
    DPU_ERR_INVALID_PROFILE,
    DPU_ERR_UNKNOWN_SYMBOL,
    DPU_ERR_LOG_FORMAT,
    DPU_ERR_LOG_CONTEXT_MISSING,
    DPU_ERR_LOG_BUFFER_TOO_SMALL,
    DPU_ERR_VPD_INVALID_FILE,
} dpu_error_t;

/*
 * @fn dpu_error_to_string
 * @brief Transform a dpu_error_t into a string.
 * @param status the api status to stringify
 * @return The string associated to the specified status.
 */
const char *
dpu_error_to_string(dpu_error_t status);

#endif // DPU_ERROR_H
