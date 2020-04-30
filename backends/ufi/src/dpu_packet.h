/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_DPU_PACKET_H
#define UFI_DPU_PACKET_H

#include <stdint.h>

typedef struct _dpu_packet_t {
    uint64_t *data;
    uint64_t *mask;
    uint64_t *expected;
    bool *result_ready;
    bool *byte_order_operations, any_byte_order_check, any_non_byte_order_check_operation;
    dpu_slice_target_t slice_target;
} * dpu_packet_t;

#endif // UFI_DPU_PACKET_H
