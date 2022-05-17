/*
 * Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "../inc/telemetry_agent_sawf.h"

/**
 *   tlmtry_agent_sawf_upd_delay_stats: Update delay stats
 *   @peer_id: peer_id
 *   @svc_id: service_class id
 *   @stats: pointer to stats structure
 *   @params: pointer to moving average params structure
 *
 *   return QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
tlmtry_agent_sawf_upd_delay_stats(uint16_t peer_id, uint8_t svc_id, void *stats,
				  struct tlmtry_sawf_mvng_avg_params *params)
{

	struct dp_peer *peer;
	struct dp_peer_sawf *sawf_ctx;
	struct dp_peer_sawf_stats *sawf_stats_ctx;
	struct sawf_fw_mpdu_stats *src;
	uint8_t tid, q_idx;
	QDF_STATUS status;

	if (!stats || !params) {
		qdf_err("Invalid dst to fill");
		goto fail;
	}

	if (!soc) {
		qdf_err("Invalid soc context");
		goto fail;
	}

	peer = dp_peer_get_ref_by_id(soc, peer_id, DP_MOD_ID_CDP);
	if (!peer) {
		qdf_err("Invalid peer context for peer_id %d", peer_id);
		goto fail;
	}

	sawf_stats_ctx = dp_peer_sawf_stats_ctx_get(peer->txrx_peer);
	if (!sawf_stats_ctx) {
		qdf_err("No sawf stats ctx for peer_id %d", peer_id);
		goto fail;
	}

	sawf_ctx = dp_peer_sawf_ctx_get(peer);
	if (!sawf_ctx) {
		qdf_err("stats_ctx doesn't exist");
		goto fail;
	}
	/*
	 * Find msduqs of the peer from service class ID
	 */
	status = dp_sawf_find_msdu_from_svc_id(sawf_ctx, svc_id,
					       &tid, &q_idx);
	if (QDF_IS_STATUS_ERROR(status)) {
		qdf_err("No MSDU Queue found for svc id %u", svc_id);
		goto fail;
	}

	src = &sawf_stats_ctx->stats.svc_intval_stats[tid][q_idx];
	qdf_mem_copy((struct sawf_fw_mpdu_stats *)stats, src, sizeof(*src));

	return QDF_STATUS_SUCCESS;
fail:
	return QDF_STATUS_E_FAILURE;
}


/**
 *   tlmtry_agent_sawf_upd_svc_intvl_stats: Update service-interval stats
 *   @peer_id: peer_id
 *   @svc_id: service_class id
 *   @stats: pointer to stats structure
 *
 *   return QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
tlmtry_agent_sawf_upd_svc_intvl_stats(uint16_t peer_id, uint8_t svc_id,
				     void *stats)
{
	struct dp_peer *peer;
	struct dp_peer_sawf *sawf_ctx;
	struct dp_peer_sawf_stats *sawf_stats_ctx;
	struct sawf_fw_mpdu_stats *src;
	uint8_t tid, q_idx;
	QDF_STATUS status;

	if (!stats) {
		qdf_err("Invalid dst to fill");
		goto fail;
	}

	if (!soc) {
		qdf_err("Invalid soc context");
		goto fail;
	}

	peer = dp_peer_get_ref_by_id(soc, peer_id, DP_MOD_ID_CDP);
	if (!peer) {
		qdf_err("Invalid peer context for peer_id %d", peer_id);
		goto fail;
	}

	sawf_stats_ctx = dp_peer_sawf_stats_ctx_get(peer->txrx_peer);
	if (!sawf_stats_ctx) {
		qdf_err("No sawf stats ctx for peer_id %d", peer_id);
		goto fail;
	}

	sawf_ctx = dp_peer_sawf_ctx_get(peer);
	if (!sawf_ctx) {
		qdf_err("stats_ctx doesn't exist");
		goto fail;
	}
	/*
	 * Find msduqs of the peer from service class ID
	 */
	status = dp_sawf_find_msdu_from_svc_id(sawf_ctx, svc_id,
					       &tid, &q_idx);
	if (QDF_IS_STATUS_ERROR(status)) {
		qdf_err("No MSDU Queue found for svc id %u", svc_id);
		goto fail;
	}

	src = &sawf_stats_ctx->stats.svc_intval_stats[tid][q_idx];
	qdf_mem_copy((struct sawf_fw_mpdu_stats *)stats, src, sizeof(*src));

	return QDF_STATUS_SUCCESS;
fail:
	return QDF_STATUS_E_FAILURE;
}


/**
 *   tlmtry_agent_sawf_upd_burst_size_stats: Update burst_size stats
 *   @peer_id: peer_id
 *   @svc_id: service_class id
 *   @stats: pointer to stats structure
 *
 *   return QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
tlmtry_agent_sawf_upd_burst_size_stats(uint16_t peer_id, uint8_t svc_id,
				       void *stats)
{
	struct dp_peer *peer;
	struct dp_peer_sawf *sawf_ctx;
	struct dp_peer_sawf_stats *sawf_stats_ctx;
	struct sawf_fw_mpdu_stats *src;
	uint8_t tid, q_idx;
	QDF_STATUS status;

	if (!stats) {
		qdf_err("Invalid dst to fill");
		goto fail;
	}

	if (!soc) {
		qdf_err("Invalid soc context");
		goto fail;
	}

	peer = dp_peer_get_ref_by_id(soc, peer_id, DP_MOD_ID_CDP);
	if (!peer) {
		qdf_err("Invalid peer context for peer_id %d", peer_id);
		goto fail;
	}

	sawf_stats_ctx = dp_peer_sawf_stats_ctx_get(peer->txrx_peer);
	if (!sawf_stats_ctx) {
		qdf_err("No sawf stats ctx for peer_id %d", peer_id);
		goto fail;
	}

	sawf_ctx = dp_peer_sawf_ctx_get(peer);
	if (!sawf_ctx) {
		qdf_err("stats_ctx doesn't exist");
		goto fail;
	}
	/*
	 * Find msduqs of the peer from service class ID
	 */
	status = dp_sawf_find_msdu_from_svc_id(sawf_ctx, svc_id,
					       &tid, &q_idx);
	if (QDF_IS_STATUS_ERROR(status)) {
		qdf_err("No MSDU Queue found for svc id %u", svc_id);
		goto fail;
	}

	src = &sawf_stats_ctx->stats.burst_size_stats[tid][q_idx];
	qdf_mem_copy((struct sawf_fw_mpdu_stats *)stats, src, sizeof(*src));

	return QDF_STATUS_SUCCESS;
fail:
	return QDF_STATUS_E_FAILURE;
}

/**
 *   tlmtry_agent_sawf_upd_msdu_drop_stats: Update msdu drop stats
 *   @peer_id: peer_id
 *   @svc_id: service_class id
 *   @stats: pointer to stats structure
 *
 *   return QDF_STATUS_SUCCESS on success
 */
QDF_STATUS
tlmtry_agent_sawf_upd_msdu_drop_stats(uint16_t peer_id, uint8_t svc_id,
				      void *stats)
{
	struct dp_peer *peer;
	struct dp_peer_sawf *sawf_ctx;
	struct dp_peer_sawf_stats *sawf_stats_ctx;
	struct sawf_fw_mpdu_stats *src;
	uint8_t tid, q_idx;
	QDF_STATUS status;

	if (!stats) {
		qdf_err("Invalid dst to fill");
		goto fail;
	}

	if (!soc) {
		qdf_err("Invalid soc context");
		goto fail;
	}

	peer = dp_peer_get_ref_by_id(soc, peer_id, DP_MOD_ID_CDP);
	if (!peer) {
		qdf_err("Invalid peer context for peer_id %d", peer_id);
		goto fail;
	}

	sawf_stats_ctx = dp_peer_sawf_stats_ctx_get(peer->txrx_peer);
	if (!sawf_stats_ctx) {
		qdf_err("No sawf stats ctx for peer_id %d", peer_id);
		goto fail;
	}

	sawf_ctx = dp_peer_sawf_ctx_get(peer);
	if (!sawf_ctx) {
		qdf_err("stats_ctx doesn't exist");
		goto fail;
	}
	/*
	 * Find msduqs of the peer from service class ID
	 */
	status = dp_sawf_find_msdu_from_svc_id(sawf_ctx, svc_id,
					       &tid, &q_idx);
	if (QDF_IS_STATUS_ERROR(status)) {
		qdf_err("No MSDU Queue found for svc id %u", svc_id);
		goto fail;
	}

	src = &sawf_stats_ctx->stats.tx_stats.msdu_drop_stats;
	qdf_mem_copy((struct sawf_msdu_drop_stats *)stats, src, sizeof(*src));

	return QDF_STATUS_SUCCESS;
fail:
	return QDF_STATUS_E_FAILURE;
}
