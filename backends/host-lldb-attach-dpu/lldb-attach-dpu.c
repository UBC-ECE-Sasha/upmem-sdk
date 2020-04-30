/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dpu_program.h>
#include <dpu_management.h>

void
breakpoint_fct(__attribute__((unused)) struct dpu_t *dpu)
{
    pause();
}

void
exit_usage(char *program)
{
    fprintf(stderr, "Usage: %s <region_id>.<rank_id>.<slice_id>.<dpu_id> [<dpu_program_path>]", program);
    exit(-1);
}

int
main(int argc, char **argv)
{
    struct dpu_rank_t *rank;
    struct dpu_t *dpu;
    struct dpu_program_t program;
    dpu_error_t status;
    unsigned int region_id, rank_id;
    dpu_member_id_t dpu_id;
    dpu_slice_id_t slice_id;
    char *profile;

    if (!(argc == 2 || argc == 3)) {
        exit_usage(argv[0]);
    }

    if (sscanf(argv[1], "%u.%u.%u.%u", &region_id, &rank_id, (unsigned int *)&slice_id, (unsigned int *)&dpu_id) != 4) {
        exit_usage(argv[0]);
    }

    asprintf(&profile, "backend=hw,rankPath=/dev/dpu_region%u/dpu_rank%u", region_id, rank_id);

    status = dpu_get_rank_of_type(profile, &rank);
    if (status != DPU_OK) {
        fprintf(stderr, "%s: Could not allocate rank to attach\n", argv[0]);
        return -1;
    }

    dpu = dpu_get(rank, slice_id, dpu_id);
    if (dpu == NULL) {
        fprintf(stderr, "%s: Could not get DPU to attach\n", argv[0]);
        return -1;
    }

    if (argc == 3) {
        program.program_path = argv[2];
        dpu_set_program(dpu, &program);
    }

    breakpoint_fct(dpu);
    return 0;
}
