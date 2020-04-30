/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_COMMANDS_H
#define UFI_DPU_COMMANDS_H

#define CI_FRAME_DPU_OPERATION_EMPTY ((0x0000000000000000L))
#define CI_FRAME_DPU_OPERATION_BIT_ORDER(ne, stutter, c2d, d2c)                                                                  \
    ((0x0100000000000000L) | ((uint64_t)(ne) << 0) | ((uint64_t)(stutter) << 8) | ((uint64_t)(c2d) << 16)                        \
        | ((uint64_t)(d2c) << 32))
#define CI_FRAME_DPU_OPERATION_IDENTITY ((0x01FF000000000000L))
#define CI_FRAME_DPU_OPERATION_SOFTWARE_RESET(factor, cycle_accurate)                                                            \
    ((0x01FF00000000FF00L) | ((uint64_t)(factor) << 0) | ((uint64_t)(cycle_accurate) << 4))
#define CI_FRAME_DPU_OPERATION_THERMAL_CONFIG(threshold) ((0x01FF000000FF0000L) | ((uint64_t)(threshold) << 0))
#define CI_FRAME_DPU_OPERATION_CMD_DURATION_FUTUR(duration) ((0x01FF000000FFFF00L) | ((uint64_t)(duration) << 0))
#define CI_FRAME_DPU_OPERATION_RES_DURATION_FUTUR(duration) ((0x01FF0000FF000000L) | ((uint64_t)(duration) << 0))
#define CI_FRAME_DPU_OPERATION_RES_SAMPLING_FUTUR(sampling) ((0x01FF0000FF00FF00L) | ((uint64_t)(sampling) << 0))
#define CI_FRAME_DPU_OPERATION_BYTE_ORDER ((0x7777777777777777L))
#define CI_FRAME_DPU_OPERATION_NOP ((0xFF00000000000000L))
#define CI_FRAME_DPU_OPERATION_SELECT_DPU_STRUCTURE                                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_SELECT_DPU_FRAME(dpu_id)                                                                          \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(255) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_SELECT_GROUP_STRUCTURE                                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_SELECT_GROUP_FRAME(group_id)                                                                      \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(255) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_SELECT_ALL_STRUCTURE                                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_SELECT_ALL_FRAME                                                                                  \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_DPU_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b1010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_ALL_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b100) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)   \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_DPU_FRAME(dpu_id, group_id)                                                       \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(12) << 16)                          \
        | ((uint64_t)(group_id) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_ALL_FRAME(group_id)                                                               \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(12) << 8) | ((uint64_t)(group_id) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_WRITE_GROUP_FOR_PREVIOUS_FRAME(group_id)                                                          \
    ((0x3300000000000000L) | ((uint64_t)(12) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(255) << 16)                       \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_STRUCTURE                                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_DPU_FRAME(dpu_id)                                                               \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(16) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_READ_FOR_PREVIOUS_FRAME                                                                  \
    ((0x3300000000000000L) | ((uint64_t)(16) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_STRUCTURE(byte5)                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(byte5) << 16)                  \
        | ((uint64_t)(167) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_STRUCTURE(byte5)                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(byte5) << 16)                  \
        | ((uint64_t)(167) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_STRUCTURE                                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(167) << 16)                     \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(255) << 16)                      \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_DPU_FRAME(dpu_id, byte0, byte1, byte2, byte3, byte4)                           \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(byte0) << 16)                       \
        | ((uint64_t)(byte1) << 24) | ((uint64_t)(byte2) << 32) | ((uint64_t)(byte3) << 40) | ((uint64_t)(byte4) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_GROUP_FRAME(group_id, byte0, byte1, byte2, byte3, byte4)                       \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(byte0) << 16)                      \
        | ((uint64_t)(byte1) << 24) | ((uint64_t)(byte2) << 32) | ((uint64_t)(byte3) << 40) | ((uint64_t)(byte4) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_ALL_FRAME(byte0, byte1, byte2, byte3, byte4, byte5)                            \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(byte0) << 8) | ((uint64_t)(byte1) << 16)                         \
        | ((uint64_t)(byte2) << 24) | ((uint64_t)(byte3) << 32) | ((uint64_t)(byte4) << 40) | ((uint64_t)(byte5) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_WRITE_FOR_PREVIOUS_FRAME(byte0, byte1, byte2, byte3, byte4, byte5)                       \
    ((0x3300000000000000L) | ((uint64_t)(byte0) << 0) | ((uint64_t)(byte1) << 8) | ((uint64_t)(byte2) << 16)                     \
        | ((uint64_t)(byte3) << 24) | ((uint64_t)(byte4) << 32) | ((uint64_t)(byte5) << 40) | ((uint64_t)(167) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(255) << 16)                      \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_CTRL_CLEAR_FOR_PREVIOUS_FRAME                                                                 \
    ((0x3300000000000000L) | ((uint64_t)(0) << 0) | ((uint64_t)(0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)         \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(166) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(dpu_id)                                                 \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(17) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME                                                    \
    ((0x3300000000000000L) | ((uint64_t)(17) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(dpu_id)                                                 \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(18) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME                                                    \
    ((0x3300000000000000L) | ((uint64_t)(18) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_STRUCTURE                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_STRUCTURE                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_DPU_FRAME(dpu_id)                                                 \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(19) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_THREAD_INDEX_READ_FOR_PREVIOUS_FRAME                                                    \
    ((0x3300000000000000L) | ((uint64_t)(19) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_DPU_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_DPU_FRAME(dpu_id)                                                                 \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(20) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_LSB_READ_FOR_PREVIOUS_FRAME                                                                    \
    ((0x3300000000000000L) | ((uint64_t)(20) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_DPU_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_DPU_FRAME(dpu_id)                                                                 \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(21) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MSB_READ_FOR_PREVIOUS_FRAME                                                                    \
    ((0x3300000000000000L) | ((uint64_t)(21) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_DPU_STRUCTURE                                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_DPU_FRAME(dpu_id)                                                                \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(22) << 16) | ((uint64_t)(2) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_READ_FOR_PREVIOUS_FRAME                                                                   \
    ((0x3300000000000000L) | ((uint64_t)(22) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)        \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_STRUCTURE                                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b10) << 8) | ((uint64_t)(0) << 16)                     \
        | ((uint64_t)(164) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(255) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b10) << 8) | ((uint64_t)(0) << 16)                     \
        | ((uint64_t)(164) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(255) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_STRUCTURE                                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b1) << 8) | ((uint64_t)(164) << 16)                     \
        | ((uint64_t)(0) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b10111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16)                      \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_DPU_FRAME(dpu_id, pc_mode)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(pc_mode) << 16)                     \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_GROUP_FRAME(group_id, pc_mode)                                                  \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(pc_mode) << 16)                    \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_ALL_FRAME(pc_mode)                                                              \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(pc_mode) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)   \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_PC_MODE_WRITE_FOR_PREVIOUS_FRAME(pc_mode)                                                         \
    ((0x3300000000000000L) | ((uint64_t)(pc_mode) << 0) | ((uint64_t)(0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)   \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(164) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_STRUCTURE                                                       \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_DPU_FRAME(dpu_id)                                                        \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(128) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_GROUP_FRAME(group_id)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(128) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_ALL_FRAME                                                                \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(128) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_READ_FOR_PREVIOUS_FRAME                                                           \
    ((0x3300000000000000L) | ((uint64_t)(128) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_STRUCTURE                                                        \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_DPU_FRAME(dpu_id)                                                         \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(134) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_ALL_FRAME                                                                 \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(134) << 8) | ((uint64_t)(255) << 16) | ((uint64_t)(2) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_CLR_FOR_PREVIOUS_FRAME                                                            \
    ((0x3300000000000000L) | ((uint64_t)(134) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_STRUCTURE                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_STRUCTURE                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_DPU_FRAME(dpu_id)                                                \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(135) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_GROUP_FRAME(group_id)                                            \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(135) << 16)                        \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_ALL_FRAME                                                        \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(135) << 8) | ((uint64_t)(255) << 16) | ((uint64_t)(2) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_FAULT_STATE_SET_AND_STEP_FOR_PREVIOUS_FRAME                                                   \
    ((0x3300000000000000L) | ((uint64_t)(135) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_STRUCTURE                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_DPU_FRAME(dpu_id)                                                          \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(132) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_GROUP_FRAME(group_id)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(132) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_ALL_FRAME                                                                  \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(132) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DPU_RUN_STATE_READ_FOR_PREVIOUS_FRAME                                                             \
    ((0x3300000000000000L) | ((uint64_t)(132) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_STRUCTURE                                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_STRUCTURE                                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_DPU_FRAME(dpu_id)                                                              \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(176) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_ALL_FRAME                                                                      \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(176) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_READ_FOR_PREVIOUS_FRAME                                                                 \
    ((0x3300000000000000L) | ((uint64_t)(176) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_DPU_FRAME(dpu_id)                                                             \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(178) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_ALL_FRAME                                                                     \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(178) << 8) | ((uint64_t)(255) << 16) | ((uint64_t)(2) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_CLEAR_FOR_PREVIOUS_FRAME                                                                \
    ((0x3300000000000000L) | ((uint64_t)(178) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_STRUCTURE                                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_DPU_FRAME(dpu_id)                                                               \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(179) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_BKP_FAULT_SET_FOR_PREVIOUS_FRAME                                                                  \
    ((0x3300000000000000L) | ((uint64_t)(179) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_DPU_FRAME(dpu_id)                                                           \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(180) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_READ_FOR_PREVIOUS_FRAME                                                              \
    ((0x3300000000000000L) | ((uint64_t)(180) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_STRUCTURE                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_DPU_FRAME(dpu_id)                                                          \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(182) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_ALL_FRAME                                                                  \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(182) << 8) | ((uint64_t)(255) << 16) | ((uint64_t)(2) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_CLEAR_FOR_PREVIOUS_FRAME                                                             \
    ((0x3300000000000000L) | ((uint64_t)(182) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_STRUCTURE                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_DPU_FRAME(dpu_id)                                                            \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(183) << 16)                         \
        | ((uint64_t)(255) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_POISON_FAULT_SET_FOR_PREVIOUS_FRAME                                                               \
    ((0x3300000000000000L) | ((uint64_t)(183) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_DPU_FRAME(dpu_id)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(130) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_ALL_FRAME                                                              \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(130) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DMA_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME                                                         \
    ((0x3300000000000000L) | ((uint64_t)(130) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_DPU_FRAME(dpu_id)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(244) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_ALL_FRAME                                                              \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(244) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_MEM_FAULT_READ_AND_CLR_FOR_PREVIOUS_FRAME                                                         \
    ((0x3300000000000000L) | ((uint64_t)(244) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_STRUCTURE                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_STRUCTURE                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_STRUCTURE                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_STRUCTURE                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_DPU_FRAME(dpu_id)                                                       \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(240) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_GROUP_FRAME(group_id)                                                   \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(240) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_ALL_FRAME                                                               \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(240) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_CLR_FOR_PREVIOUS_FRAME                                                          \
    ((0x3300000000000000L) | ((uint64_t)(240) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_STRUCTURE                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_STRUCTURE                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_STRUCTURE                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_STRUCTURE                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_DPU_FRAME(dpu_id)                                                       \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(242) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_GROUP_FRAME(group_id)                                                   \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8) | ((uint64_t)(242) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_ALL_FRAME                                                               \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)(242) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_STACK_UP_READ_AND_SET_FOR_PREVIOUS_FRAME                                                          \
    ((0x3300000000000000L) | ((uint64_t)(242) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_DPU_STRUCTURE(thread_id)                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_GROUP_STRUCTURE(thread_id)                                                        \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_ALL_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_DPU_FRAME(dpu_id, thread_id)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_GROUP_FRAME(group_id, thread_id)                                                  \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_ALL_FRAME(thread_id)                                                              \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_BOOT_FOR_PREVIOUS_FRAME(thread_id)                                                         \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_DPU_STRUCTURE(thread_id)                                                        \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_GROUP_STRUCTURE(thread_id)                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_ALL_STRUCTURE                                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_STRUCTURE                                                              \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_DPU_FRAME(dpu_id, thread_id)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_GROUP_FRAME(group_id, thread_id)                                                \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_ALL_FRAME(thread_id)                                                            \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_RESUME_FOR_PREVIOUS_FRAME(thread_id)                                                       \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((137452375244800l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_STRUCTURE(thread_id)                                                       \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_STRUCTURE(thread_id)                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_STRUCTURE                                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_DPU_FRAME(dpu_id, thread_id)                                                   \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_GROUP_FRAME(group_id, thread_id)                                               \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_ALL_FRAME(thread_id)                                                           \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_CLR_RUN_FOR_PREVIOUS_FRAME(thread_id)                                                      \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_STRUCTURE(thread_id)                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_STRUCTURE(thread_id)                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_DPU_FRAME(dpu_id, thread_id)                                                  \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_GROUP_FRAME(group_id, thread_id)                                              \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_ALL_FRAME(thread_id)                                                          \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_THREAD_READ_RUN_FOR_PREVIOUS_FRAME(thread_id)                                                     \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_STRUCTURE(data, address)                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b11) << 8)                                             \
        | ((uint64_t)((((data) >> (((5) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 24)     \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 32) | ((uint64_t)(71) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_STRUCTURE(data, address)                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b11) << 8)                                             \
        | ((uint64_t)((((data) >> (((5) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 24)     \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 32) | ((uint64_t)(71) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_STRUCTURE(address)                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b1) << 8)                                              \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(71) << 32) | ((uint64_t)(2) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_STRUCTURE(address)                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11111111) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 16) | ((uint64_t)(71) << 24) | ((uint64_t)(2) << 32)             \
        | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME(dpu_id, data)                                                \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((data) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((data) >> (((1) << 3))) & 0xFFl)) << 24)        \
        | ((uint64_t)((((data) >> (((2) << 3))) & 0xFFl)) << 32) | ((uint64_t)((((data) >> (((3) << 3))) & 0xFFl)) << 40)        \
        | ((uint64_t)((((data) >> (((4) << 3))) & 0xFFl)) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME(group_id, data)                                            \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)((((data) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((data) >> (((1) << 3))) & 0xFFl)) << 24)        \
        | ((uint64_t)((((data) >> (((2) << 3))) & 0xFFl)) << 32) | ((uint64_t)((((data) >> (((3) << 3))) & 0xFFl)) << 40)        \
        | ((uint64_t)((((data) >> (((4) << 3))) & 0xFFl)) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME(data)                                                        \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0) | ((uint64_t)((((data) >> (((0) << 3))) & 0xFFl)) << 8)                        \
        | ((uint64_t)((((data) >> (((1) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((data) >> (((2) << 3))) & 0xFFl)) << 24)        \
        | ((uint64_t)((((data) >> (((3) << 3))) & 0xFFl)) << 32) | ((uint64_t)((((data) >> (((4) << 3))) & 0xFFl)) << 40)        \
        | ((uint64_t)((((data) >> (((5) << 3))) & 0xFFl)) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME(data, address)                                          \
    ((0x3300000000000000L) | ((uint64_t)((((data) >> (((0) << 3))) & 0xFFl)) << 0)                                               \
        | ((uint64_t)((((data) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)((((data) >> (((2) << 3))) & 0xFFl)) << 16)         \
        | ((uint64_t)((((data) >> (((3) << 3))) & 0xFFl)) << 24) | ((uint64_t)((((data) >> (((4) << 3))) & 0xFFl)) << 32)        \
        | ((uint64_t)((((data) >> (((5) << 3))) & 0xFFl)) << 40) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(64) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE0_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(64) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(65) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE1_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(65) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(66) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE2_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(66) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(67) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE3_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(67) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(68) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE4_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(68) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b1110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_STRUCTURE                                                            \
    ((0x1100000000000000L) | ((uint64_t)(0b11) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_DPU_FRAME(dpu_id, address)                                                    \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 16) | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 24)  \
        | ((uint64_t)(69) << 32) | ((uint64_t)(2) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IRAM_READ_BYTE5_FOR_PREVIOUS_FRAME(address)                                                       \
    ((0x3300000000000000L) | ((uint64_t)((((address) >> (((0) << 3))) & 0xFFl)) << 0)                                            \
        | ((uint64_t)((((address) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)(69) << 16) | ((uint64_t)(2) << 24)              \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_STRUCTURE(address)                                                   \
    ((0x1100000000000000L) | ((uint64_t)(0b11111111) << 0) | ((uint64_t)(0b11) << 8)                                             \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((0) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 16)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((1) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 24)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((3) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 32)                                                                                                               \
        | ((uint64_t)(153) << 40))
#define CI_FRAME_DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME(data, address)                                                 \
    ((0x3300000000000000L) | ((uint64_t)((((data) >> (((0) << 3))) & 0xFFl)) << 0)                                               \
        | ((uint64_t)((((data) >> (((1) << 3))) & 0xFFl)) << 8) | ((uint64_t)((((data) >> (((2) << 3))) & 0xFFl)) << 16)         \
        | ((uint64_t)((((data) >> (((3) << 3))) & 0xFFl)) << 24)                                                                 \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((2) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 32)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((5) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 40)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((136340582694912l | (((((uint64_t)0) >> 0) & 7) << 17) | (((((uint64_t)0) >> 3) & 3) << 32)                    \
                     | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                                 \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 7) << 39)    \
                     | (((((uint64_t)((address) << 2)) >> 7) & 1) << 24) | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15)     \
                     | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14) | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13)    \
                     | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)                                                        \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0)))                                                    \
                    >> (((4) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 48))
#define CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_STRUCTURE(address)                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((3) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 16)                                                                                                               \
        | ((uint64_t)(153) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_STRUCTURE                                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_DPU_FRAME(dpu_id, address)                                                     \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((2) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 16)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((5) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 24)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((4) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 32)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((0) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 40)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((1) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 48))
#define CI_FRAME_DPU_OPERATION_WRAM_READ_WORD_FOR_PREVIOUS_FRAME(address)                                                        \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((2) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 0)                                                                                                                \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((5) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 8)                                                                                                                \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((4) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 16)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((0) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 24)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((1) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 32)                                                                                                               \
        | ((uint64_t)(                                                                                                           \
               ((((123159328063488l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 1) << 27)                  \
                     | (((((uint64_t)((address) << 2)) >> 0) & 15) << 20) | (((((uint64_t)((address) << 2)) >> 4) & 15) << 16)   \
                     | (((((uint64_t)((address) << 2)) >> 8) & 1) << 15) | (((((uint64_t)((address) << 2)) >> 9) & 1) << 14)     \
                     | (((((uint64_t)((address) << 2)) >> 10) & 1) << 13) | (((((uint64_t)((address) << 2)) >> 11) & 1) << 12)   \
                     | (((((uint64_t)((address) << 2)) >> 12) & 4095) << 0) | (((((uint64_t)0) >> 0) & 31) << 39)))              \
                    >> (((3) << 3)))                                                                                             \
                   & 0xFFl))                                                                                                     \
            << 40)                                                                                                               \
        | ((uint64_t)(153) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b101010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b1010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_DPU_FRAME(dpu_id, msb, lsb_A, lsb_B)                                        \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(144) << 16)                         \
        | ((uint64_t)(msb) << 24) | ((uint64_t)(145) << 32) | ((uint64_t)((lsb_B) | ((lsb_A) << 4)) << 40)                       \
        | ((uint64_t)(255) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_AB_FOR_PREVIOUS_FRAME(msb, lsb_A, lsb_B)                                           \
    ((0x3300000000000000L) | ((uint64_t)(144) << 0) | ((uint64_t)(msb) << 8) | ((uint64_t)(145) << 16)                           \
        | ((uint64_t)((lsb_B) | ((lsb_A) << 4)) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40)                         \
        | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b101010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b1010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_DPU_FRAME(dpu_id, msb, lsb_C, lsb_D)                                        \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(146) << 16)                         \
        | ((uint64_t)(msb) << 24) | ((uint64_t)(147) << 32) | ((uint64_t)((lsb_D) | ((lsb_C) << 4)) << 40)                       \
        | ((uint64_t)(255) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_CD_FOR_PREVIOUS_FRAME(msb, lsb_C, lsb_D)                                           \
    ((0x3300000000000000L) | ((uint64_t)(146) << 0) | ((uint64_t)(msb) << 8) | ((uint64_t)(147) << 16)                           \
        | ((uint64_t)((lsb_D) | ((lsb_C) << 4)) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40)                         \
        | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b101010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_STRUCTURE                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b1010) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_DPU_FRAME(dpu_id, odd_index, even_index, timing)                            \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(151) << 16)                         \
        | ((uint64_t)(((odd_index) >> 1)) << 24) | ((uint64_t)(150) << 32)                                                       \
        | ((uint64_t)((((timing) << 5) | ((even_index) >> 1))) << 40) | ((uint64_t)(255) << 48))
#define CI_FRAME_DPU_OPERATION_IREPAIR_CONFIG_OE_FOR_PREVIOUS_FRAME(odd_index, even_index, timing)                               \
    ((0x3300000000000000L) | ((uint64_t)(151) << 0) | ((uint64_t)(((odd_index) >> 1)) << 8) | ((uint64_t)(150) << 16)            \
        | ((uint64_t)((((timing) << 5) | ((even_index) >> 1))) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40)          \
        | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_STRUCTURE                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b1111111) << 0) | ((uint64_t)(0b1) << 8) | ((uint64_t)(165) << 16)                     \
        | ((uint64_t)(0) << 24) | ((uint64_t)(255) << 32) | ((uint64_t)(2) << 40))
#define CI_FRAME_DPU_OPERATION_REGISTER_FILE_TIMING_CONFIG_FOR_PREVIOUS_FRAME(rf_timing)                                         \
    ((0x3300000000000000L) | ((uint64_t)(0) << 0) | ((uint64_t)(rf_timing) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME(duration)                                              \
    ((0x3300000000000000L) | ((uint64_t)(4) << 0) | ((uint64_t)(duration) << 8) | ((uint64_t)(255) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_CONFIG_SAMPLING_FOR_PREVIOUS_FRAME(sampling)                                              \
    ((0x3300000000000000L) | ((uint64_t)(5) << 0) | ((uint64_t)(sampling) << 8) | ((uint64_t)(255) << 16)                        \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_CMD_BUS_SYNC_FOR_PREVIOUS_FRAME                                                                   \
    ((0x3300000000000000L) | ((uint64_t)(7) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b10111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16)                      \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_RES_BUS_CONFIG_DURATION_FOR_PREVIOUS_FRAME(duration)                                              \
    ((0x3300000000000000L) | ((uint64_t)(duration) << 0) | ((uint64_t)(0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)  \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(246) << 48))
#define CI_FRAME_DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_STRUCTURE                                                               \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_RES_BUS_SYNC_FOR_PREVIOUS_FRAME                                                                   \
    ((0x3300000000000000L) | ((uint64_t)(247) << 0) | ((uint64_t)(255) << 8) | ((uint64_t)(2) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_DPU_STRUCTURE(thread_id)                                                          \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_GROUP_STRUCTURE(thread_id)                                                        \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_ALL_STRUCTURE                                                                     \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_STRUCTURE                                                                \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_DPU_FRAME(dpu_id, thread_id)                                                      \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_GROUP_FRAME(group_id, thread_id)                                                  \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_ALL_FRAME(thread_id)                                                              \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_READ_FOR_PREVIOUS_FRAME(thread_id)                                                         \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136353132052480l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_DPU_STRUCTURE(thread_id)                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_STRUCTURE(thread_id)                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_ALL_STRUCTURE                                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_DPU_FRAME(dpu_id, thread_id)                                                       \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_GROUP_FRAME(group_id, thread_id)                                                   \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_ALL_FRAME(thread_id)                                                               \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_CLR_FOR_PREVIOUS_FRAME(thread_id)                                                          \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((136902619430912l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_DPU_STRUCTURE(thread_id)                                                           \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_GROUP_STRUCTURE(thread_id)                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(152) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_ALL_STRUCTURE                                                                      \
    ((0x1100000000000000L) | ((uint64_t)(0b1111110) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(152) << 16)                     \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_STRUCTURE                                                                 \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_DPU_FRAME(dpu_id, thread_id)                                                       \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_GROUP_FRAME(group_id, thread_id)                                                   \
    ((0x3300000000000000L) | ((uint64_t)(9) << 0) | ((uint64_t)(group_id) << 8)                                                  \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_ALL_FRAME(thread_id)                                                               \
    ((0x3300000000000000L) | ((uint64_t)(8) << 0)                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_NOTIFY_SET_FOR_PREVIOUS_FRAME(thread_id)                                                          \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((138002131058688l | (((((uint64_t)24) >> 0) & 31) << 34) | (((((uint64_t)0) >> 0) & 15) << 24)          \
                            | (((((uint64_t)(thread_id)) >> 0) & 15) << 20) | (((((uint64_t)(thread_id)) >> 4) & 15) << 16)      \
                            | (((((uint64_t)0) >> 0) & 65535) << 0)))                                                            \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(152) << 48))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_STRUCTURE                                                  \
    ((0x1100000000000000L) | ((uint64_t)(0b11111110) << 0) | ((uint64_t)(0b0) << 8)                                              \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(158) << 24) | ((uint64_t)(2) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_STRUCTURE                                             \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(2) << 16)                        \
        | ((uint64_t)(0) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_DPU_FRAME(dpu_id)                                              \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8)                                                   \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 48))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_STOP_ENABLED_FOR_PREVIOUS_FRAME                                                 \
    ((0x3300000000000000L)                                                                                                       \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((2) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 0)                                                                                                                \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((5) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 8)                                                                                                                \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((4) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 16)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((0) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 24)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((1) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 32)                                                                                                               \
        | ((uint64_t)(((((139582679023616l | (((((uint64_t)0) >> 0) & 15) << 24) | (((((uint64_t)0) >> 0) & 65535) << 0)))       \
                           >> (((3) << 3)))                                                                                      \
               & 0xFFl))                                                                                                         \
            << 40)                                                                                                               \
        | ((uint64_t)(158) << 48))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_STRUCTURE                                                         \
    ((0x1100000000000000L) | ((uint64_t)(0b10) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)    \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_STRUCTURE                                                    \
    ((0x1100000000000000L) | ((uint64_t)(0b0) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)     \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_DPU_FRAME(dpu_id)                                                     \
    ((0x3300000000000000L) | ((uint64_t)(10) << 0) | ((uint64_t)(dpu_id) << 8) | ((uint64_t)(156) << 16) | ((uint64_t)(2) << 24) \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_REPLACE_CLEAR_FOR_PREVIOUS_FRAME                                                        \
    ((0x3300000000000000L) | ((uint64_t)(156) << 0) | ((uint64_t)(2) << 8) | ((uint64_t)(0) << 16) | ((uint64_t)(0) << 24)       \
        | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40) | ((uint64_t)(0) << 48))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_STRUCTURE                                                        \
    ((0x1100000000000000L) | ((uint64_t)(0b111111) << 0) | ((uint64_t)(0b0) << 8) | ((uint64_t)(156) << 16)                      \
        | ((uint64_t)(2) << 24) | ((uint64_t)(0) << 32) | ((uint64_t)(0) << 40))
#define CI_FRAME_DPU_OPERATION_DEBUG_STD_SAMPLE_PC_FOR_PREVIOUS_FRAME                                                            \
    ((0x3300000000000000L) | ((uint64_t)(((((136764643606528l)) >> (((2) << 3))) & 0xFFl)) << 0)                                 \
        | ((uint64_t)(((((136764643606528l)) >> (((5) << 3))) & 0xFFl)) << 8)                                                    \
        | ((uint64_t)(((((136764643606528l)) >> (((4) << 3))) & 0xFFl)) << 16)                                                   \
        | ((uint64_t)(((((136764643606528l)) >> (((0) << 3))) & 0xFFl)) << 24)                                                   \
        | ((uint64_t)(((((136764643606528l)) >> (((1) << 3))) & 0xFFl)) << 32)                                                   \
        | ((uint64_t)(((((136764643606528l)) >> (((3) << 3))) & 0xFFl)) << 40) | ((uint64_t)(158) << 48))
#endif /* UFI_DPU_COMMANDS_H */
