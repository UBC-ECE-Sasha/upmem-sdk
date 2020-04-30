/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_RANK_H
#define DPU_RANK_H

#include <dpu_description.h>
#include <dpu_types.h>
#include <dpu_runner.h>
#include <dpu_profiler.h>
#include <dpu_debug.h>

#include <dpu_rank_handler.h>
#include <dpu_ufi_types.h>
#include <time.h>

#define NB_MAX_REPAIR_ADDR 4
#define NR_OF_WRAM_BANKS 4

struct dpu_memory_repair_address_t {
    uint32_t address;
    uint64_t faulty_bits;
};

struct dpu_memory_repair_t {
    struct dpu_memory_repair_address_t corrupted_addresses[NB_MAX_REPAIR_ADDR];
    uint32_t nr_of_corrupted_addresses;
    bool fail_to_repair;
};

/* This structure contains all infos that must be shared by
 * any application so that a debugger can take control of it and
 * restores its context for the application to resume correctly.
 */
struct dpu_configuration_slice_info_t {
    uint64_t structure_value; // structure register is overwritten by debuggers, we need a place where debugger
    // can access the last structure so that it replays the last write_structure command.
    struct dpu_slice_target slice_target;

    dpu_bitfield_t host_mux_mram_state; // Contains the state of all the MRAM muxes

    dpu_selected_mask_t dpus_per_group[DPU_MAX_NR_GROUPS];

    dpu_selected_mask_t enabled_dpus;
    bool all_dpus_are_enabled;
};

struct dpu_command_t {
    uint64_t data[DPU_MAX_NR_CIS];
    char direction;
};

struct dpu_circular_buffer_commands_t {
    struct dpu_command_t *cmds; // Circular buffer that contains the last commands
    uint32_t size, nb, idx_last;
    bool has_wrapped;
};

struct dpu_debug_context_t {
    dpu_ci_bitfield_t
        debug_color; // color expected when a debugger takes the control, so that we restore the same color when leaving.

    uint32_t debug_result[DPU_MAX_NR_CIS]; // Contains the result when the debugger attached the host application

    struct dpu_configuration_slice_info_t
        debug_slice_info[DPU_MAX_NR_CIS]; // Used by the debugger when attaching a process: is a copy of the above structure.

    bool is_rank_for_debugger;

    struct dpu_circular_buffer_commands_t cmds_buffer;
};

struct dpu_control_interface_context {
    dpu_ci_bitfield_t fault_decode;
    dpu_ci_bitfield_t fault_collide;

    dpu_ci_bitfield_t color;
    struct dpu_configuration_slice_info_t slice_info[DPU_MAX_NR_CIS]; // Used for the current application to hold slice info
};

struct dpu_poll_thread_context_t {
    bool thr_exists;
    int thr_has_work; /* -1 means kill yourself, 0 means nothing to do, 1 means job to do */
    pthread_t thr_id;
    pthread_cond_t thr_cond;
    pthread_mutex_t thr_mutex;
};

typedef struct _dpu_run_context_t {
    dpu_bitfield_t dpu_running[DPU_MAX_NR_CIS];
    dpu_bitfield_t dpu_in_fault[DPU_MAX_NR_CIS];
    uint8_t nb_dpu_running;

    struct dpu_poll_thread_context_t poll_thread;
} * dpu_run_context_t;

struct dpu_runtime_state_t {
    struct dpu_control_interface_context control_interface;
    struct _dpu_run_context_t run_context;
};

struct dpu_rank_t {
    dpu_type_t type;
    dpu_rank_id_t rank_id;
    dpu_description_t description;
    struct dpu_runtime_state_t runtime;
    struct dpu_debug_context_t debug;
    struct _dpu_profiling_context_t profiling_context;

    struct _dpu_rank_handler_context_t *handler_context;

    pthread_mutex_t mutex;

    struct dpu_t *dpus;

    uint64_t cmds[DPU_MAX_NR_CIS];

    struct timespec temperature_sample_time;
    void *_internals;
};

struct dpu_t {
    struct dpu_rank_t *rank;
    dpu_slice_id_t slice_id;
    dpu_member_id_t dpu_id;
    bool enabled;

    struct dpu_program_t *program;

    struct {
        struct dpu_memory_repair_t iram_repair;
        struct dpu_memory_repair_t wram_repair[NR_OF_WRAM_BANKS];
    } repair;

    struct dpu_poll_thread_context_t poll_thread;

    dpu_context_t debug_context;

    void *transfer_buffer;
};

#ifndef struct_dpu_transfer_mram_t
#define struct_dpu_transfer_mram_t
struct dpu_transfer_mram {
    void *ptr;
    uint32_t offset_in_mram : 30;
    uint32_t mram_number : 2;
    uint32_t size;
};
#endif

#endif // DPU_RANK_H
