/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <dpu_types.h>
#include <dpu_rank.h>

#include "dpu_attributes.h"
#include "dpu_log_utils.h"
#include "dpu_query.h"
#include "dpu_query_parameters.h"
#include "dpu_transaction.h"
#include "dpu_packet.h"
#include "verbose_control.h"
#include "static_verbose.h"

static struct verbose_control *this_vc;
static struct verbose_control *
__vc()
{
    if (this_vc == NULL) {
        this_vc = get_verbose_control_for("ufi");
    }
    return this_vc;
}

extern uint64_t
dpu_operation_to_command_build(dpu_operation_type_e operation, dpu_query_t query);

static inline void
extract_and_update_results(dpu_packet_t packet, dpu_operation_t *operations, dpu_query_t *queries, uint8_t nb_of_slices)
{
    for (uint8_t each_operation = 0; each_operation < nb_of_slices; ++each_operation) {
        dpu_query_update_result(queries[each_operation],
            operations[each_operation],
            (uint8_t)packet->data[each_operation],
            *(uint32_t *)&packet->data[each_operation],
            *(uint64_t *)&packet->data[each_operation]);
    }
}

static void
save_slice_context(struct dpu_rank_t *rank, dpu_packet_t packet)
{
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;

    for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
        uint64_t command = packet->data[each_slice];

        /* Because structure register can't be read back from CI, we must store
         * the last value sent so that debugger can replay it before handing
         * control back to the application
         * Note: by storing this value BEFORE committing the command, in case of race
         * condition with the debugger, it will commit this structure before the debuggee and
         * the debuggee will replay it. This makes playing the structure command twice, but
         * that's ok.
         */
        if ((command & 0xFF00000000000000ULL) == 0x1100000000000000ULL /* Write structure */)
            rank->runtime.control_interface.slice_info[each_slice].structure_value = command;

        /* Transactions can use the previously selected target to avoid reselecting it at each
         * query: if the debugger arrives right after the query that selects the target and before a
         * query that uses the previous target, this command will fail. So store the target
         * information so that the debugger can replay it.
         */
        switch (packet->slice_target[each_slice].type) {
            case DPU_SLICE_TARGET_DPU:
                rank->runtime.control_interface.slice_info[each_slice].slice_target.type = packet->slice_target[each_slice].type;
                rank->runtime.control_interface.slice_info[each_slice].slice_target.dpu_id
                    = packet->slice_target[each_slice].dpu_id;
                break;
            case DPU_SLICE_TARGET_GROUP:
                rank->runtime.control_interface.slice_info[each_slice].slice_target.type = packet->slice_target[each_slice].type;
                rank->runtime.control_interface.slice_info[each_slice].slice_target.group_id
                    = packet->slice_target[each_slice].group_id;
                break;
            case DPU_SLICE_TARGET_ALL:
                rank->runtime.control_interface.slice_info[each_slice].slice_target.type = packet->slice_target[each_slice].type;
                break;
            default:
                break;
        }
    }
}

static uint8_t
set_expected_color_from_command(struct dpu_rank_t *rank, dpu_packet_t packet)
{
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;
    uint8_t expected_color = 0;

    for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
        uint64_t command = packet->data[each_slice];

        /* Sets the expected color and update the next color if needed
         * Note: soft reset command should not toggle the color bit, but
         * reset it. So in cni.c, when invoking a SOFT_RESET command,
         * we set the color so that it is cleared here.
         */
        expected_color |= rank->runtime.control_interface.color & (1 << each_slice);

        if (command & 0xFF00000000000000ULL && (command & 0xFF00000000000000ULL) != 0xFF00000000000000ULL /* NOP */)
            rank->runtime.control_interface.color ^= (1 << each_slice);
    }

    for (uint8_t each_operation = 0; each_operation < nb_of_slices; ++each_operation) {
        /* Adds the color to the expected packet */
        if (expected_color & (1 << each_operation))
            packet->expected[each_operation] |= 0x00FF000000000000ULL;
    }

    return expected_color;
}

static dpu_planner_status_e
execute_packet(struct dpu_rank_t *rank, dpu_rank_handler_t rank_handler, dpu_packet_t packet)
{
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;

#define PACKET_LOG_TEMPLATE                                                                                                      \
    "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX "                                                       \
    "XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXX"
    char packet_str[strlen(PACKET_LOG_TEMPLATE) + 1];
    if (LOGV_ENABLED(__vc())) {
        for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
            sprintf(packet_str + each_slice * 17,
                "%016lx%s",
                packet->data[each_slice],
                (each_slice != (nb_of_slices - 1)) ? " " : "");
        }
        LOG_RANK(VERBOSE, rank, "[W] %s", packet_str);
    }

#define NB_RETRY_FOR_VALID_RESULT 100
    bool results_are_ready = true;
    uint32_t nb_retry = 0;
    uint8_t expected_color;

    /* 1/ From the commands, determine what control interface context must be saved (for debugging purposes) */
    save_slice_context(rank, packet);

    /* 2/ From the commands, update the expected color for each control interface */
    expected_color = set_expected_color_from_command(rank, packet);

    /* 3/ Execute the packet */
    if (rank_handler->commit_commands(rank, &packet->data) != DPU_RANK_SUCCESS) {
        return DPU_PLANNER_RANK_ERROR;
    }

    /* 4/ Loop until result validity with a timeout */

    /* system.h: After having written a BYTE ORDER command to the control interface, the UPmem HCPU firmware
     * reads the control interface until the read value has the following property:
     * one of its byte is nul.
     */
    if (packet->any_byte_order_check) {
        nb_retry = 0;

        do {
            results_are_ready = true;

            if (rank_handler->update_commands(rank, &packet->data) != DPU_RANK_SUCCESS) {
                return DPU_PLANNER_RANK_ERROR;
            }

            for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
                if (packet->byte_order_operations[each_slice]) {
                    for (uint8_t each_byte = 0; each_byte < sizeof(uint64_t); ++each_byte) {
                        if (((packet->data[each_slice] >> (each_byte << 3)) & 0xFF) == 0) {
                            packet->result_ready[each_slice] = true;
                            break;
                        }
                    }
                }
            }

            for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
                if (packet->byte_order_operations[each_slice] && !packet->result_ready[each_slice]) {
                    results_are_ready = false;
                    break;
                }
            }

            nb_retry++;
        } while (!results_are_ready && nb_retry < NB_RETRY_FOR_VALID_RESULT);
    }

    if (!results_are_ready && nb_retry == NB_RETRY_FOR_VALID_RESULT) {
        LOG_RANK(WARNING, rank, "Timeout waiting for valid results !");
        return DPU_PLANNER_TIMEOUT_WAITING_FOR_VALID_RESULTS;
    }

    if (packet->any_non_byte_order_check_operation) {
        nb_retry = 0;

        do {
            results_are_ready = true;

            if (rank_handler->update_commands(rank, &packet->data) != DPU_RANK_SUCCESS) {
                return DPU_PLANNER_RANK_ERROR;
            }

            for (uint8_t each_operation = 0; each_operation < nb_of_slices; ++each_operation) {
                if (!packet->result_ready[each_operation]) {
                    uint64_t result = (packet->data[each_operation] & packet->mask[each_operation]);

                    if (result == packet->expected[each_operation])
                        packet->result_ready[each_operation] = true;
                    /* The second case can happen when the debugger has restored the result */
                    else if ((result & 0xFF00000000000000ULL) == 0xFF00000000000000ULL) {
                        LOG_RANK(
                            VERBOSE, rank, "the application was interrupted by a debugger that restored the expected result.");
                        packet->result_ready[each_operation] = true;
                    } else {
                        /* Let's ignore CMD_FAULT_DECODE and CMD_FAULT_COLLIDE for the moment,
                         * but we need to mark the result as ready in those cases too.
                         */
                        uint64_t xor_result_expected = result ^ packet->expected[each_operation];
                        /* Ignore the coloring bits and check if the result would be ok */
                        uint64_t xor_result_expected_without_color = xor_result_expected & 0xFF00FFFFFFFFFFFFULL;
                        uint8_t color = ((result & 0x00FF000000000000ULL) >> 48) & 0xFF;
                        uint8_t nb_bits_set = __builtin_popcount(color);

                        if (xor_result_expected_without_color)
                            results_are_ready = false;
                        else {
                            /* From here, the result seems ok, we just need to check that
                             * the color is ok.
                             */
                            if (expected_color & (1 << each_operation)) {
                                if (nb_bits_set <= 3)
                                    results_are_ready = false;
                            } else {
                                if (nb_bits_set >= 5)
                                    results_are_ready = false;
                            }
                        }

                        /* We are in fault path, store this information */
                        if (expected_color & (1 << each_operation)) {
                            switch (nb_bits_set) {
                                case 5:
                                    LOG_RANK(VERBOSE, rank, "Command collision detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_collide |= (1 << each_operation);
                                    LOG_RANK(VERBOSE, rank, "Command decoding error detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_decode |= (1 << each_operation);
                                    break;
                                case 6:
                                    LOG_RANK(VERBOSE, rank, "Command collision detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_collide |= (1 << each_operation);
                                    break;
                                case 7:
                                    LOG_RANK(VERBOSE, rank, "Command decoding error detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_decode |= (1 << each_operation);
                                    break;
                                case 8:
                                    break;
                                default:
                                    LOG_RANK(WARNING,
                                        rank,
                                        "Color expected to be 1, but number of bits set (%u) is inconsistent."
                                        "Mark the result as not ready.",
                                        nb_bits_set);
                                    results_are_ready = false;
                            }
                        } else {
                            switch (nb_bits_set) {
                                case 3:
                                    LOG_RANK(VERBOSE, rank, "Command collision detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_collide |= (1 << each_operation);
                                    LOG_RANK(VERBOSE, rank, "Command decoding error detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_decode |= (1 << each_operation);
                                    break;
                                case 2:
                                    LOG_RANK(VERBOSE, rank, "Command collision detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_collide |= (1 << each_operation);
                                    break;
                                case 1:
                                    LOG_RANK(VERBOSE, rank, "Command decoding error detected on slice %d", each_operation);
                                    rank->runtime.control_interface.fault_decode |= (1 << each_operation);
                                    break;
                                case 0:
                                    break;
                                default:
                                    LOG_RANK(WARNING,
                                        rank,
                                        "Color expected to be 0, but number of bits set (%u) is inconsistent."
                                        "Mark the result as not ready.",
                                        nb_bits_set);
                                    results_are_ready = false;
                            }
                        }
                    }
                }
            }

            nb_retry++;
        } while (!results_are_ready && nb_retry < NB_RETRY_FOR_VALID_RESULT);
    }

    if (!results_are_ready && nb_retry == NB_RETRY_FOR_VALID_RESULT) {
        LOG_RANK(WARNING, rank, "Timeout waiting for valid results !");
        return DPU_PLANNER_TIMEOUT_WAITING_FOR_VALID_RESULTS;
    }

    /* All results are ready here, and still present when reading the control interfaces.
     * We make sure that we have the correct results by reading again (we may have timing issues).
     * todo(#85): this can be somewhat costly. We should try to integrate this additional read in a lower layer.
     */
    if (rank_handler->update_commands(rank, &packet->data) != DPU_RANK_SUCCESS) {
        return DPU_PLANNER_RANK_ERROR;
    }

    if (LOGV_ENABLED(__vc())) {
        for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
            sprintf(packet_str + each_slice * 17,
                "%016lx%s",
                packet->data[each_slice],
                (each_slice != (nb_of_slices - 1)) ? " " : "");
        }
        LOG_RANK(VERBOSE, rank, "[R] %s", packet_str);
    }

    return DPU_PLANNER_SUCCESS;
}

static void
operation_to_packet(struct dpu_rank_t *rank, dpu_operation_t *operations, dpu_query_t *queries, dpu_packet_t packet)
{
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;

    for (uint8_t each_operation = 0; each_operation < nb_of_slices; ++each_operation) {
        uint64_t command = dpu_operation_to_command_build(operations[each_operation]->type, queries[each_operation]);
        dpu_operation_type_e type = operations[each_operation]->type;
        dpu_query_t query = queries[each_operation];

        // If the query is NULL, an empty operation will be played. Just say that we are not changing the target type.
        if (query == NULL) {
            packet->slice_target[each_operation].type = DPU_SLICE_TARGET_PREVIOUS;
        } else {
            packet->slice_target[each_operation].type = query->target.slice_target.type;
            packet->slice_target[each_operation].dpu_id = query->target.slice_target.dpu_id;
            packet->slice_target[each_operation].group_id = query->target.slice_target.group_id;
        }
        packet->data[each_operation] = command;

        switch (type) {
            case DPU_OPERATION_EMPTY:
                packet->mask[each_operation] = 0xFFFF000000000000ULL;
                packet->expected[each_operation] = 0x0000000000000000ULL;
                packet->result_ready[each_operation] = true;
                break;
            case DPU_OPERATION_SOFTWARE_RESET:
                /*
                 * This operation resets the Control Interface and does not return anything...
                 * The upper layer should actively wait "some time" to be sure that the command has been handled.
                 */
                packet->result_ready[each_operation] = true;
                break;
            case DPU_OPERATION_BYTE_ORDER:
                packet->any_byte_order_check = true;
                packet->byte_order_operations[each_operation] = true;
                break;
            case DPU_OPERATION_NOP:
                packet->mask[each_operation] = 0xFFFF000000000000ULL;
                packet->expected[each_operation] = 0xFF00000000000000ULL;
                break;
            case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_DPU_FRAME:
            case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_GROUP_FRAME:
            case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_ALL_FRAME:
            case DPU_OPERATION_IRAM_WRITE_INSTRUCTION_FOR_PREVIOUS_FRAME: {
                packet->any_non_byte_order_check_operation = true;
                dpu_selected_mask_t selected_dpus
                    = ((dpu_query_write_iram_instruction_parameters_t)(query->parameters))->selected_dpus;

                packet->mask[each_operation] = 0xFFFF00FFFFFFFFFFULL;
                packet->expected[each_operation] = 0x000000FF00000000ULL | ((uint64_t)selected_dpus);

                break;
            }
            case DPU_OPERATION_WRAM_WRITE_WORD_FOR_PREVIOUS_FRAME: {
                packet->any_non_byte_order_check_operation = true;
                dpu_selected_mask_t selected_dpus = ((dpu_query_write_wram_word_parameters_t)(query->parameters))->selected_dpus;

                packet->mask[each_operation] = 0xFFFF00FFFFFFFFFFULL;
                packet->expected[each_operation] = 0x000000FF00000000ULL | ((uint64_t)selected_dpus);

                break;
            }
            default:
                packet->any_non_byte_order_check_operation = true;
                packet->mask[each_operation] = 0xFFFF00FF00000000ULL;
                packet->expected[each_operation] = 0xFF00000000ULL;

                break;
        }
    }
}

static dpu_planner_status_e
execute_operation(dpu_packet_t packet,
    dpu_operation_t *operations,
    dpu_query_t *queries,
    struct dpu_rank_t *rank,
    dpu_rank_handler_t rank_handler)
{
    dpu_planner_status_e planner_status;
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;

    /* 1/ Convert operations into a packet of commands */
    packet->any_byte_order_check = false;
    packet->any_non_byte_order_check_operation = false;
    /* mask and expected are init by operation_to_packet, so no need to clear
     * them.
     */
    memset(packet->byte_order_operations, 0, nb_of_slices * sizeof(bool));
    memset(packet->result_ready, 0, nb_of_slices * sizeof(bool));

    operation_to_packet(rank, operations, queries, packet);

    /* 2/ Execute the commands */
    planner_status = execute_packet(rank, rank_handler, packet);
    if (planner_status != DPU_PLANNER_SUCCESS)
        return planner_status;

    /* 3/ Retrieve the results */
    extract_and_update_results(packet, operations, queries, nb_of_slices);

    return DPU_PLANNER_SUCCESS;
}

static dpu_planner_status_e
packet_allocate(struct dpu_rank_t *rank, dpu_packet_t packet)
{
    uint8_t nb_of_slices = rank->description->topology.nr_of_control_interfaces;
    dpu_planner_status_e planner_status;

    packet->data = calloc(nb_of_slices, sizeof(uint64_t));
    if (!packet->data)
        return DPU_PLANNER_SYSTEM_ERROR;

    packet->mask = calloc(nb_of_slices, sizeof(uint64_t));
    if (!packet->mask) {
        planner_status = DPU_PLANNER_SYSTEM_ERROR;
        goto free_data;
    }

    packet->expected = calloc(nb_of_slices, sizeof(uint64_t));
    if (!packet->expected) {
        planner_status = DPU_PLANNER_SYSTEM_ERROR;
        goto free_mask;
    }

    packet->byte_order_operations = calloc(nb_of_slices, sizeof(bool));
    if (!packet->byte_order_operations) {
        planner_status = DPU_PLANNER_SYSTEM_ERROR;
        goto free_expected;
    }

    packet->result_ready = calloc(nb_of_slices, sizeof(bool));
    if (!packet->result_ready) {
        planner_status = DPU_PLANNER_SYSTEM_ERROR;
        goto free_byte_order_operations;
    }

    packet->slice_target = calloc(nb_of_slices, sizeof(struct _dpu_slice_target_t));
    if (!packet->slice_target) {
        planner_status = DPU_PLANNER_SYSTEM_ERROR;
        goto free_result_ready;
    }

    return DPU_PLANNER_SUCCESS;

free_result_ready:
    free(packet->result_ready);
free_byte_order_operations:
    free(packet->byte_order_operations);
free_expected:
    free(packet->expected);
free_mask:
    free(packet->mask);
free_data:
    free(packet->data);

    return planner_status;
}

static void
packet_free(dpu_packet_t packet)
{
    free(packet->slice_target);
    free(packet->result_ready);
    free(packet->byte_order_operations);
    free(packet->expected);
    free(packet->mask);
    free(packet->data);
}

/* This is backend responsability to correctly send the command :
 *  - byte ordering
 *  - interleaving
 *  - 0xFF in MSB of command to mask it
 *
 * Bit ordering is command dependent, we cannot ask any backend to
 * decode the command that it is sending in order to know if bit
 * ordering must be done: this operation must be done by upper software
 * layers.
 */
__API_SYMBOL__ dpu_planner_status_e
dpu_planner_execute_transaction(dpu_transaction_t transaction, dpu_rank_handler_t rank_handler, struct dpu_rank_t *rank)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_operation_step_t operations;
    dpu_operation_t *current_operations;
    dpu_query_t *current_queries;
    dpu_operation_t empty_operation;
    struct _dpu_packet_t packet;
    uint8_t nb_of_slices = transaction->rank_nr_of_ci;
    dpu_planner_status_e result;

    if ((empty_operation = dpu_operation_new(DPU_OPERATION_EMPTY)) == NULL) {
        result = DPU_PLANNER_SYSTEM_ERROR;
        goto end;
    }

    INIT_LIST_HEAD(&empty_operation->list);

    result = packet_allocate(rank, &packet);
    if (result != DPU_PLANNER_SUCCESS) {
        result = DPU_PLANNER_SYSTEM_ERROR;
        goto free_empty_operation;
    }

    if ((operations = dpu_operation_step_new(transaction->rank_nr_of_ci)) == NULL) {
        result = DPU_PLANNER_SYSTEM_ERROR;
        goto free_packet;
    }

    if ((current_operations = malloc(nb_of_slices * sizeof(*current_operations))) == NULL) {
        result = DPU_PLANNER_SYSTEM_ERROR;
        goto free_operations;
    }

    for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
        current_operations[each_slice] = empty_operation;
    }

    if ((current_queries = calloc(nb_of_slices, sizeof(*current_queries))) == NULL) {
        result = DPU_PLANNER_SYSTEM_ERROR;
        goto free_current_operations;
    }

    while (true) {
        uint8_t nb_of_empty_slices = 0;

        for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
            if (list_empty(&operations->list[each_slice])) {

                if (((current_queries[each_slice] == NULL) && list_empty(&transaction->list[each_slice]))
                    || ((current_queries[each_slice] != NULL)
                        && (list_is_last(&current_queries[each_slice]->list, &transaction->list[each_slice])))) {
                    nb_of_empty_slices++;
                    continue;
                }

                dpu_query_t next_query;

                if (current_queries[each_slice] == NULL) {
                    next_query = list_entry(transaction->list[each_slice].next, struct _dpu_query_t, list);
                } else {
                    next_query = list_entry(current_queries[each_slice]->list.next, struct _dpu_query_t, list);
                }

                current_queries[each_slice] = next_query;
                next_query->_index = 0;

                dpu_query_fill_operations_list(next_query, &operations->list[each_slice]);
            }

            if (!list_empty(&operations->list[each_slice])) {
                dpu_operation_t cur_op = list_entry(operations->list[each_slice].next, struct _dpu_operation_t, list);
                uint64_t command = dpu_operation_to_command_build(cur_op->type, current_queries[each_slice]);

                /* Do not resend the same write_structure command */
                if ((command & 0xFF00000000000000ULL) == 0x1100000000000000ULL /* Write structure */) {
                    if (rank->runtime.control_interface.slice_info[each_slice].structure_value == command) {
                        dpu_operation_free(cur_op);
                        /* We won't have multiple write structure in a row */
                        if (!list_empty(&operations->list[each_slice]))
                            current_operations[each_slice]
                                = list_entry(operations->list[each_slice].next, struct _dpu_operation_t, list);
                    } else
                        current_operations[each_slice] = cur_op;
                } else
                    current_operations[each_slice] = cur_op;
            }
        }

        if (nb_of_empty_slices == nb_of_slices) {
            break;
        }

        result = execute_operation(&packet, current_operations, current_queries, rank, rank_handler);

        for (uint8_t each_slice = 0; each_slice < nb_of_slices; ++each_slice) {
            if (current_operations[each_slice] != empty_operation) {
                dpu_operation_free(current_operations[each_slice]);
                current_operations[each_slice] = empty_operation;
            }
        }

        if (result != DPU_PLANNER_SUCCESS) {
            break;
        }
    }

    free(current_queries);
free_current_operations:
    free(current_operations);
free_operations:
    dpu_operation_step_free(operations);
free_packet:
    packet_free(&packet);
free_empty_operation:
    dpu_operation_free(empty_operation);
end:
    return result;
}

__API_SYMBOL__ dpu_planner_status_e
dpu_planner_execute_transfer(dpu_rank_handler_t rank_handler,
    struct dpu_rank_t *rank,
    dpu_transfer_type_e transfer_type,
    struct dpu_transfer_mram *transfer_matrix)
{
    LOG_RANK(VERBOSE, rank, "");
    dpu_planner_status_e status = DPU_PLANNER_SUCCESS;

    switch (transfer_type) {
        case DPU_TRANSFER_FROM_MRAM:
            if (rank_handler->copy_from_rank(rank, transfer_matrix) != DPU_RANK_SUCCESS) {
                status = DPU_PLANNER_RANK_ERROR;
            }
            break;
        case DPU_TRANSFER_TO_MRAM:
            if (rank_handler->copy_to_rank(rank, transfer_matrix) != DPU_RANK_SUCCESS) {
                status = DPU_PLANNER_RANK_ERROR;
            }
            break;

        default:
            status = DPU_PLANNER_INVALID_TRANSFER_TYPE_ERROR;
            break;
    }

    return status;
}
