/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include "dpu_query.h"

void
dpu_query_target_dump(FILE *output, uint32_t indent, dpu_query_target_t target)
{
    fprintf(output, "%*starget @%p {\n", indent, "", target);
    fprintf(output,
        "%*stype = %s (%d)\n",
        indent + 4,
        "",
        DPU_SLICE_TARGET_TYPE_NAME(target->slice_target.type),
        target->slice_target.type);
    fprintf(output, "%*sslice_id = %d\n", indent + 4, "", target->slice_id);

    switch (target->slice_target.type) {
        case DPU_SLICE_TARGET_DPU:
            fprintf(output, "%*sdpu_id = %d\n", indent + 4, "", target->slice_target.dpu_id);
            break;
        case DPU_SLICE_TARGET_GROUP:
            fprintf(output, "%*sgroup_id = %d\n", indent + 4, "", target->slice_target.group_id);
            break;
        default:
            break;
    }

    fprintf(output, "%*s}\n", indent, "");
}
