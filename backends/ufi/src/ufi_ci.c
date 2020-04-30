/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <dpu_types.h>
#include <dpu/ufi_ci_types.h>
#include <dpu/ufi_ci_commands.h>
#include <ufi_rank_utils.h>

#define NB_RETRY_FOR_VALID_RESULT 100

static void invert_color(struct dpu_rank_t *rank, u8 ci_mask);
static u8 compute_ci_mask(struct dpu_rank_t *rank, const u64 *commands);
static u32 compute_masks(struct dpu_rank_t *rank, const u64 *commands,
			 u64 *masks, u64 *expected, u8 *cis,
			 bool add_select_mask, bool *is_done);
static u32 exec_cmd(struct dpu_rank_t *rank, u64 *commands, u64 *data,
		    bool add_select_mask);
static bool determine_if_byte_discoveries_are_finished(struct dpu_rank_t *rank,
						       const u64 *data);
static bool
determine_if_commands_are_finished(struct dpu_rank_t *rank, const u64 *data,
				   const u64 *expected, const u64 *result_masks,
				   u8 expected_color, bool *is_done);

static void log_temperature(struct dpu_rank_t *rank, u64 *results);

static u32 debug_record_last_cmd(struct dpu_rank_t *rank, char direction,
				 u64 *commands)
{
	struct dpu_debug_context_t *debug = GET_DEBUG(rank);
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u32 idx_last;

	if (!LOGD_ENABLED(__vc()))
		return DPU_OK;

	idx_last = debug->cmds_buffer.idx_last;

	memcpy(debug->cmds_buffer.cmds[idx_last].data, commands,
	       nr_cis * sizeof(u64));
	debug->cmds_buffer.cmds[idx_last].direction = direction;

	debug->cmds_buffer.idx_last++;

	if (!debug->cmds_buffer.has_wrapped) {
		if (debug->cmds_buffer.idx_last >= debug->cmds_buffer.size) {
			debug->cmds_buffer.has_wrapped = true;
			debug->cmds_buffer.nb = debug->cmds_buffer.size;
		} else {
			debug->cmds_buffer.nb++;
		}
	}

	debug->cmds_buffer.idx_last %= debug->cmds_buffer.size;

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_commit_commands(struct dpu_rank_t *rank, u64 *commands)
{
	struct dpu_rank_handler *handler = GET_HANDLER(rank);
	u32 ret;

	ret = debug_record_last_cmd(rank, WRITE, commands);
	if (ret != DPU_OK)
		return ret;

	if (handler->commit_commands(rank, commands) != DPU_RANK_SUCCESS) {
		return DPU_ERR_DRIVER;
	}

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_update_commands(struct dpu_rank_t *rank, u64 *commands)
{
	struct dpu_rank_handler *handler = GET_HANDLER(rank);
	u32 ret;

	if (handler->update_commands(rank, commands) != DPU_RANK_SUCCESS) {
		return DPU_ERR_DRIVER;
	}

	ret = debug_record_last_cmd(rank, READ, commands);
	if (ret != DPU_OK)
		return ret;

	return DPU_OK;
}

__API_SYMBOL__ void ci_prepare_mask(u64 *buffer, u8 mask, u64 data)
{
	u8 each_ci;

	for (each_ci = 0; each_ci < DPU_MAX_NR_CIS; ++each_ci) {
		if (CI_MASK_ON(mask, each_ci)) {
			buffer[each_ci] = data;
		} else {
			buffer[each_ci] = CI_EMPTY;
		}
	}
}

__API_SYMBOL__ u32 ci_fill_selected_dpu_mask(struct dpu_rank_t *rank, u8 ci,
					     u8 *mask)
{
	u8 nr_dpus = GET_DESC(rank)->topology.nr_of_dpus_per_control_interface;
	struct dpu_configuration_slice_info_t *info =
		&(GET_CI_CONTEXT(rank)->slice_info[ci]);

	u8 dpu_mask;
	switch (info->slice_target.type) {
	case DPU_SLICE_TARGET_ALL:
		dpu_mask = (1 << nr_dpus) - 1;
		break;
	case DPU_SLICE_TARGET_DPU:
		dpu_mask = 1 << info->slice_target.dpu_id;
		break;
	case DPU_SLICE_TARGET_GROUP:
		dpu_mask = info->dpus_per_group[info->slice_target.group_id];
		break;
	default:
		return DPU_ERR_INTERNAL;
	}

	*mask = dpu_mask;

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_exec_void_cmd(struct dpu_rank_t *rank, u64 *commands)
{
	u64 ignored[DPU_MAX_NR_CIS];

	return exec_cmd(rank, commands, ignored, false);
}

__API_SYMBOL__ u32 ci_exec_wait_mask_cmd(struct dpu_rank_t *rank, u64 *commands)
{
	u64 ignored[DPU_MAX_NR_CIS];

	return exec_cmd(rank, commands, ignored, true);
}

__API_SYMBOL__ u32 ci_exec_8bit_cmd(struct dpu_rank_t *rank, u64 *commands,
				    u8 *results)
{
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u64 data[DPU_MAX_NR_CIS];
	u8 each_ci;
	u32 status;

	if ((status = exec_cmd(rank, commands, data, false)) != DPU_OK) {
		return status;
	}

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		if (commands[each_ci] != CI_EMPTY) {
			results[each_ci] = data[each_ci];
		}
	}

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_exec_32bit_cmd(struct dpu_rank_t *rank, u64 *commands,
				     u32 *results)
{
	u64 data[DPU_MAX_NR_CIS];
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u8 each_ci;
	u32 status;

	if ((status = exec_cmd(rank, commands, data, false)) != DPU_OK) {
		return status;
	}

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		if (commands[each_ci] != CI_EMPTY) {
			results[each_ci] = data[each_ci];
		}
	}

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_exec_byte_discovery(struct dpu_rank_t *rank,
					  u64 *commands, u64 *results)
{
	u32 nr_retries = NB_RETRY_FOR_VALID_RESULT;
	u8 ci_mask = compute_ci_mask(rank, commands);
	bool in_progress, timeout;
	u32 status;

	LOGV_PACKET(rank, commands, WRITE);

	invert_color(rank, ci_mask);

	if ((status = ci_commit_commands(rank, commands)) != DPU_OK) {
		return status;
	}

	do {
		if ((status = ci_update_commands(rank, results)) != DPU_OK) {
			return status;
		}

		in_progress = !determine_if_byte_discoveries_are_finished(
			rank, results);
		timeout = (nr_retries--) == 0;
	} while (in_progress && !timeout);

	if (in_progress) {
		return DPU_ERR_TIMEOUT;
	}

	LOGV_PACKET(rank, results, READ);

	return DPU_OK;
}

__API_SYMBOL__ u32 ci_exec_reset_cmd(struct dpu_rank_t *rank, u64 *commands)
{
	u32 status;
	struct dpu_control_interface_context *context = GET_CI_CONTEXT(rank);
	u8 nr_dpus = GET_DESC(rank)->topology.nr_of_dpus_per_control_interface;

	u8 clock_division = GET_DESC(rank)->timings.clock_division;
	u32 reset_duration = GET_DESC(rank)->timings.reset_wait_duration *
			     clock_division / 2;
	u64 data[DPU_MAX_NR_CIS];
	u32 each_iter;
	u8 each_ci, each_group;

	u8 reset_mask = compute_ci_mask(rank, commands);

	LOGV_PACKET(rank, commands, WRITE);

	invert_color(rank, reset_mask);

	if ((status = ci_commit_commands(rank, commands)) != DPU_OK) {
		return status;
	}

	// Software Reset has been sent. We need to wait "some time" before we can continue (~ 20 clock cycles)
	// For simulator backends, we need to run the simulation. We suppose that these function calls will be long enough
	// to put a real hardware backend in a stable state.

	for (each_iter = 0; each_iter < reset_duration; ++each_iter) {
		if ((status = ci_update_commands(rank, data)) != DPU_OK) {
			return status;
		}
	}

	/* Reset the color of the control interfaces */
	context->color &= ~reset_mask;
	context->fault_collide &= ~reset_mask;
	context->fault_decode &= ~reset_mask;

	/* Reset the group & select information */
	for (each_ci = 0; each_ci < DPU_MAX_NR_CIS; ++each_ci) {
		context->slice_info[each_ci].slice_target.type =
			DPU_SLICE_TARGET_NONE;
		context->slice_info[each_ci].dpus_per_group[0] =
			(1 << nr_dpus) - 1;

		for (each_group = 1; each_group < DPU_MAX_NR_GROUPS;
		     ++each_group) {
			context->slice_info[each_ci].dpus_per_group[each_group] =
				0;
		}
	}

	return DPU_OK;
}

static u32 exec_cmd(struct dpu_rank_t *rank, u64 *commands, u64 *data,
		    bool add_select_mask)
{
	u8 expected_color;
	u64 result_masks[DPU_MAX_NR_CIS] = {};
	u64 expected[DPU_MAX_NR_CIS] = {};
	bool is_done[DPU_MAX_NR_CIS] = {};
	u8 ci_mask;
	u32 status;
	bool in_progress, timeout;
	u32 nr_retries = NB_RETRY_FOR_VALID_RESULT;

	LOGV_PACKET(rank, commands, WRITE);

	if ((status = compute_masks(rank, commands, result_masks, expected,
				    &ci_mask, add_select_mask, is_done)) !=
	    DPU_OK) {
		return status;
	}

	expected_color = GET_CI_CONTEXT(rank)->color & ci_mask;
	invert_color(rank, ci_mask);

	if ((status = ci_commit_commands(rank, commands)) != DPU_OK) {
		return status;
	}

	do {
		if ((status = ci_update_commands(rank, data)) != DPU_OK) {
			return status;
		}

		in_progress = !determine_if_commands_are_finished(
			rank, data, expected, result_masks, expected_color,
			is_done);
		timeout = (nr_retries--) == 0;
	} while (in_progress && !timeout);

	if (in_progress) {
		/* Either we are in full log:
		 * and then log at least one packet as it has important info to debug.
		 */
		LOGV_PACKET(rank, data, READ);

		/* Or we are in debug command mode:
		 * and then dump the entire buffer of commands.
		 */
		LOGD_LAST_COMMANDS(rank);

		return DPU_ERR_TIMEOUT;
	}

	/* All results are ready here, and still present when reading the control interfaces.
     * We make sure that we have the correct results by reading again (we may have timing issues).
     * todo(#85): this can be somewhat costly. We should try to integrate this additional read in a lower layer.
     */
	if ((status = ci_update_commands(rank, data)) != DPU_OK) {
		return status;
	}

	LOGV_PACKET(rank, data, READ);

	log_temperature(rank, data);

	return DPU_OK;
}

static void invert_color(struct dpu_rank_t *rank, u8 ci_mask)
{
	GET_CI_CONTEXT(rank)->color ^= ci_mask;
}

static u8 compute_ci_mask(struct dpu_rank_t *rank, const u64 *commands)
{
	u8 ci_mask = 0;
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u8 each_ci;

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		if (commands[each_ci] != CI_EMPTY) {
			ci_mask |= (1 << each_ci);
		}
	}

	return ci_mask;
}

static u32 compute_masks(struct dpu_rank_t *rank, const u64 *commands,
			 u64 *masks, u64 *expected, u8 *cis,
			 bool add_select_mask, bool *is_done)
{
	u8 ci_mask = 0;

	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u8 nr_dpus = GET_DESC(rank)->topology.nr_of_dpus_per_control_interface;

	u8 each_ci;

	u8 dpu_mask;
	u32 status;

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		if (commands[each_ci] != CI_EMPTY) {
			ci_mask |= (1 << each_ci);
			masks[each_ci] |= 0xFF0000FF00000000l;
			expected[each_ci] |= 0x000000FF00000000l;

			if (add_select_mask) {
				masks[each_ci] |= (1 << nr_dpus) - 1;

				if ((status = ci_fill_selected_dpu_mask(
					     rank, each_ci, &dpu_mask)) !=
				    DPU_OK) {
					return status;
				}

				expected[each_ci] |= dpu_mask;
			}
		} else
			is_done[each_ci] = true;
	}

	*cis = ci_mask;

	return DPU_OK;
}

static bool determine_if_byte_discoveries_are_finished(struct dpu_rank_t *rank,
						       const u64 *data)
{
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u8 each_ci, each_byte;

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		bool finished = false;

		for (each_byte = 0; each_byte < sizeof(uint64_t); ++each_byte) {
			if (((data[each_ci] >> (each_byte << 3)) & 0xFF) == 0) {
				finished = true;
				break;
			}
		}

		if (!finished) {
			return false;
		}
	}

	return true;
}

static bool determine_if_commands_are_finished(struct dpu_rank_t *rank,
					       const u64 *data,
					       const u64 *expected,
					       const u64 *result_masks,
					       u8 expected_color, bool *is_done)
{
	struct dpu_control_interface_context *context = GET_CI_CONTEXT(rank);
	u8 nr_cis = GET_DESC(rank)->topology.nr_of_control_interfaces;
	u8 each_ci;
	u8 color, nb_bits_set, ci_mask, ci_color;

	for (each_ci = 0; each_ci < nr_cis; ++each_ci) {
		if (!is_done[each_ci]) {
			u64 result = data[each_ci];

			/* The second case can happen when the debugger has restored the result */
			if ((result & result_masks[each_ci]) !=
				    expected[each_ci] &&
			    (result & CI_NOP) != CI_NOP) {
				return false;
			}

			/* From here, the result seems ok, we just need to check that the color is ok. */
			color = ((result & 0x00FF000000000000ULL) >> 48) & 0xFF;
			nb_bits_set = __builtin_popcount(color);
			ci_mask = 1 << each_ci;
			ci_color = expected_color & ci_mask;

			if (ci_color != 0) {
				if (nb_bits_set <= 3) {
					return false;
				}
			} else {
				if (nb_bits_set >= 5) {
					return false;
				}
			}

			is_done[each_ci] = true;

			/* We are in fault path, store this information */
			if (ci_color != 0) {
				nb_bits_set = 8 - nb_bits_set;
			}

			switch (nb_bits_set) {
			case 0:
				break;
			case 1:
				LOG_CI(VERBOSE, rank, each_ci,
				       "command decoding error detected");
				context->fault_decode |= ci_mask;
				break;
			case 2:
				LOG_CI(VERBOSE, rank, each_ci,
				       "command collision detected");
				context->fault_collide |= ci_mask;
				break;
			case 3:
				LOG_CI(VERBOSE, rank, each_ci,
				       "command collision detected");
				context->fault_collide |= ci_mask;
				LOG_CI(VERBOSE, rank, each_ci,
				       "command decoding error detected");
				context->fault_decode |= ci_mask;
				break;
			default:
				LOG_CI(WARNING, rank, each_ci,
				       "Number of bits set (%u) is inconsistent."
				       "Mark the result as not ready.",
				       nb_bits_set);
				return false;
			}

			is_done[each_ci] = true;
		}
	}

	return true;
}

static void log_temperature(struct dpu_rank_t *rank, u64 *results)
{
	if (LOG_TEMPERATURE_ENABLED() && LOG_TEMPERATURE_TRIGGERED(rank)) {
		LOG_TEMPERATURE(rank, results);
	}
}
