/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_DESCRIPTION_H
#define DPU_DESCRIPTION_H

#include <stdbool.h>
#include <stdint.h>

#include <dpu_types.h>
#include <dpu_chip_id.h>

/**
 * @file dpu_description.h
 * @brief C API for DPU rank description.
 */

/**
 * @struct dpu_description_t
 * @brief Description of the characteristics of a DPU rank
 */
typedef struct _dpu_description_t {
    struct {
        dpu_chip_id_e config_id;
        uint32_t chip_id;
    } signature;

    struct {
        struct dpu_carousel_config carousel;
        uint8_t reset_wait_duration;
        uint8_t std_temperature;
        uint8_t clock_division;
    } timings;

    struct {
        uint8_t nr_of_control_interfaces;
        uint8_t nr_of_dpus_per_control_interface;
    } topology;

    struct {
        mram_size_t mram_size;
        wram_size_t wram_size;
        iram_size_t iram_size;
        mram_size_t dbg_mram_size;
    } memories;

    struct {
        uint8_t nr_of_threads;
        uint32_t nr_of_atomic_bits;
        uint32_t nr_of_notify_bits;
        uint8_t nr_of_work_registers_per_thread;
    } dpu;

    struct {
        bool mram_access_by_dpu_only;
        bool api_must_switch_mram_mux;
        bool init_mram_mux;

        bool do_iram_repair;
        bool do_wram_repair;

        bool enable_cycle_accurate_behavior;

        bool disable_api_safe_checks;

        bool disable_reset_on_alloc;

        struct dpu_bit_config pcb_transformation;
        uint32_t fck_frequency_in_mhz;
    } configuration;

    uint32_t refcount;

    struct {
        void *data;
        void (*free)(void *data);
    } _internals;
} * dpu_description_t;

/**
 * @fn dpu_acquire_description
 * @brief Acquires the ownership of the specified description, incrementing its reference counter.
 * @param description the description to be acquired
 */
void
dpu_acquire_description(dpu_description_t description);

/**
 * @fn dpu_free_description
 * @brief Releases the ownership of the specified description, decrementing its reference counter. If nobody owns the
 *        description anymore, the description is freed.
 * @param description the description to be freed
 */
void
dpu_free_description(dpu_description_t description);

#endif // DPU_DESCRIPTION_H
