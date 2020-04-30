/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>
#include <dpu_types.h>

dpu_clock_division_t
from_division_factor_to_dpu_enum(uint8_t factor)
{
    switch (factor) {
        default:
            return DPU_CLOCK_DIV2;
        case 3:
            return DPU_CLOCK_DIV3;
        case 4:
            return DPU_CLOCK_DIV4;
        case 8:
            return DPU_CLOCK_DIV8;
    }
}
