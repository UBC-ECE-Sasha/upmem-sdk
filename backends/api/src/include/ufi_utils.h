/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef UFI_UTILS_H
#define UFI_UTILS_H

#include <dpu_error.h>
#include <dpu_ufi.h>

#define DPU_ENABLED_GROUP 0
#define DPU_DISABLED_GROUP 1

#define safe_add_query(q, b, t, s, l)                                                                                            \
    do {                                                                                                                         \
        if (((q) = (b)) == NULL) {                                                                                               \
            (s) = DPU_ERR_SYSTEM;                                                                                                \
            goto l;                                                                                                              \
        }                                                                                                                        \
        dpu_transaction_add_query_tail(t, q);                                                                                    \
    } while (0)

#define safe_execute_transaction(t, rank, r, s, label)                                                                           \
    do {                                                                                                                         \
        if (((r) = dpu_planner_execute_transaction(t, (rank)->handler_context->handler, (rank))) != DPU_PLANNER_SUCCESS) {       \
            (s) = map_planner_status_to_api_status(r);                                                                           \
            goto label;                                                                                                          \
        }                                                                                                                        \
    } while (0)

#define build_select_query_for_all_enabled_dpus(rank, slice, q, t, s, l)                                                         \
    do {                                                                                                                         \
        if ((rank)->runtime.control_interface.slice_info[slice].all_dpus_are_enabled) {                                          \
            safe_add_query(q, dpu_query_build_select_all_for_control(slice), t, s, l);                                           \
        } else {                                                                                                                 \
            safe_add_query(q, dpu_query_build_select_group_for_control(slice, DPU_ENABLED_GROUP), t, s, l);                      \
        }                                                                                                                        \
    } while (0)

dpu_error_t
map_planner_status_to_api_status(dpu_planner_status_e planner_status);
dpu_error_t
map_rank_status_to_api_status(dpu_rank_status_e rank_status);

#endif // UFI_UTILS_H
