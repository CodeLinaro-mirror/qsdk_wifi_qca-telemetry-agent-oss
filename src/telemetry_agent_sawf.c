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

struct telemetry_sawf_ctx *g_telemetry_sawf;

int telemetry_sawf_init_ctx(void)
{
	if (g_telemetry_sawf) {
		pr_sawf_err("SAWF ctx alloc failed");
		return -1;
	}

	g_telemetry_sawf = kzalloc(sizeof(struct telemetry_sawf_ctx),
				   GFP_ATOMIC);
	if (!g_telemetry_sawf) {
		pr_sawf_err("SAWF ctx alloc failed");
		return -1;
	}
	pr_sawf_dbg("SAWF ctx is initialized");
	INIT_LIST_HEAD(&(g_telemetry_sawf->peer_list));
	return 0;
}

void telemetry_sawf_free_ctx(void)
{
	telemetry_sawf_free_sla_timer();

	kfree(g_telemetry_sawf);
	g_telemetry_sawf = NULL;

	pr_sawf_dbg("Agent SAWF context is freed");
}

struct telemetry_sawf_ctx *telemetry_sawf_get_ctx(void)
{
	if(g_telemetry_sawf)
		return g_telemetry_sawf;
	else
		return NULL;
}

int telemetry_sawf_set_sla_detect_cfg(uint8_t type,
				      uint8_t min_thruput_rate,
				      uint8_t max_thruput_rate,
				      uint8_t burst_size,
				      uint8_t service_interval,
				      uint8_t delay_bound,
				      uint8_t msdu_ttl,
				      uint8_t msdu_rate_loss)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_sla_param *sla_detect;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return -1;
	}

	if (type >= SAWF_SLA_DETECT_MAX) {
		pr_sawf_err("Wrong SLA Detect Type");
		return -1;
	}

	sla_detect = &sawf_ctx->sla_detect.sla_detect[type];

	sla_detect->min_thruput_rate = min_thruput_rate;
	sla_detect->max_thruput_rate = max_thruput_rate;
	sla_detect->burst_size = burst_size;
	sla_detect->service_interval = service_interval;
	sla_detect->delay_bound = delay_bound;
	sla_detect->msdu_ttl = msdu_ttl;
	sla_detect->msdu_rate_loss = msdu_rate_loss;

	pr_sawf_dbg("type %d", type);
	pr_sawf_dbg("detect->min_thruput_rate:%d",
		    sla_detect->min_thruput_rate);
	pr_sawf_dbg("detect->max_thruput_rate:%d",
		    sla_detect->max_thruput_rate);
	pr_sawf_dbg("detect->burst_size:%d", sla_detect->burst_size);
	pr_sawf_dbg("detect->service_interval:%d",
		    sla_detect->service_interval);
	pr_sawf_dbg("detect->delay_bound:%d", sla_detect->delay_bound);
	pr_sawf_dbg("detect->msdu_ttl:%d", sla_detect->msdu_ttl);
	pr_sawf_dbg("detect->msdu_rate_loss:%d", sla_detect->msdu_rate_loss);

	return 0;
}

int telemetry_sawf_set_svclass_cfg(bool enable,
				   uint8_t svc_id,
				   uint32_t min_thruput_rate,
				   uint32_t max_thruput_rate,
				   uint32_t burst_size,
				   uint32_t service_interval,
				   uint32_t delay_bound,
				   uint32_t msdu_ttl,
				   uint32_t msdu_rate_loss)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_svc_class_param *svc_class;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return -1;
	}

	if (svc_id >= SAWF_MAX_SVC_CLASS) {
		pr_sawf_err("Wrong Service ID");
		return -1;
	}
	svc_class =  &sawf_ctx->svc_class.svc_class[svc_id];

	if (enable) {
		svc_class->min_thruput_rate = min_thruput_rate;
		svc_class->max_thruput_rate = max_thruput_rate;
		svc_class->burst_size = burst_size;
		svc_class->service_interval = service_interval;
		svc_class->delay_bound = delay_bound;
		svc_class->msdu_ttl = msdu_ttl;
		svc_class->msdu_rate_loss = msdu_rate_loss;
		set_bit(svc_id, sawf_ctx->svc_class.service_ids);
	} else {
		memset(svc_class, 0,
		       sizeof(struct telemetry_sawf_svc_class_param));
		clear_bit(svc_id, sawf_ctx->svc_class.service_ids);
	}
	return 0;
}

int telemetry_sawf_set_sla_cfg(uint8_t svc_id,
			       uint8_t min_thruput_rate,
			       uint8_t max_thruput_rate,
			       uint8_t burst_size,
			       uint8_t service_interval,
			       uint8_t delay_bound,
			       uint8_t msdu_ttl,
			       uint8_t msdu_rate_loss)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_sla_param *sla_cfg;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return -1;
	}

	if (svc_id >= SAWF_MAX_SVC_CLASS) {
		pr_sawf_err("Wrong SLA Detect Type");
		return -1;
	}
	sla_cfg =  &sawf_ctx->sla.sla_cfg[svc_id];

	sla_cfg->min_thruput_rate = min_thruput_rate;
	sla_cfg->max_thruput_rate = max_thruput_rate;
	sla_cfg->burst_size = burst_size;
	sla_cfg->service_interval = service_interval;
	sla_cfg->delay_bound = delay_bound;
	sla_cfg->msdu_ttl = msdu_ttl;
	sla_cfg->msdu_rate_loss = msdu_rate_loss;

	set_bit(svc_id, sawf_ctx->sla.sla_service_ids);

	pr_sawf_dbg("Service ID: %d", svc_id);
	pr_sawf_dbg("cfg->min_thruput_rate %d", sla_cfg->min_thruput_rate);
	pr_sawf_dbg("cfg->max_thruput_rate %d", sla_cfg->max_thruput_rate);
	pr_sawf_dbg("cfg->burst_size %d", sla_cfg->burst_size);
	pr_sawf_dbg("cfg->service_interval %d", sla_cfg->service_interval);
	pr_sawf_dbg("cfg->delay_bound %d", sla_cfg->delay_bound);
	pr_sawf_dbg("cfg->msdu_ttl %d", sla_cfg->msdu_ttl);
	pr_sawf_dbg("cfg->msdu_rate_loss %d", sla_cfg->msdu_rate_loss);
	return 0;
}

int telemetry_sawf_set_mov_avg_params(uint32_t packet,
				      uint32_t window)
{
	struct telemetry_sawf_ctx *sawf_ctx;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return -1;
	}

	sawf_ctx->mov_avg.packet = packet;
	sawf_ctx->mov_avg.window = window;

	pr_sawf_dbg("sawf_ctx->mov_avg.packet %d", sawf_ctx->mov_avg.packet);
	pr_sawf_dbg("sawf_ctx->mov_avg.window %d", sawf_ctx->mov_avg.window);
	return 0;
}

int telemetry_sawf_set_sla_params(uint32_t num_packet,
				  uint32_t time_secs)
{
	struct telemetry_sawf_ctx *sawf_ctx;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return -1;
	}

	sawf_ctx->sla_param.num_packets = num_packet;
	sawf_ctx->sla_param.time_secs = time_secs;

	pr_sawf_dbg("sawf_ctx->sla_param.num_packets%d",
		    sawf_ctx->sla_param.num_packets);
	pr_sawf_dbg("sawf_ctx->sla_param.time_secs%d",
		    sawf_ctx->sla_param.time_secs);

	return 0;
}

struct telemetry_sawf_mov_avg_params *
telemetry_sawf_get_mov_avg_param(void)
{
	struct telemetry_sawf_ctx *sawf_ctx;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF context is NULL");
		return NULL;
	}

	return &(sawf_ctx->mov_avg);
}

struct telemetry_sawf_sla_params *
telemetry_sawf_get_sla_param(void)
{
	struct telemetry_sawf_ctx *sawf_ctx;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err(" SAWF context is NULL");
		return NULL;
	}

	return &(sawf_ctx->sla_param);
}

uint64_t *telemtry_sawf_delay_window_alloc(void)
{
	struct telemetry_sawf_mov_avg_params *mov_avg_param;
	uint64_t *mov_window_ptr;
	uint32_t window_size;

	mov_avg_param = telemetry_sawf_get_mov_avg_param();
	window_size = mov_avg_param->window * sizeof(uint64_t) * SAWF_MAX_TID * SAWF_MAXQ_PTID ;

	mov_window_ptr = kzalloc(window_size, GFP_ATOMIC);
	if (mov_window_ptr)
		return mov_window_ptr;
	else
		return NULL;
}

int telemtry_sawf_delay_window_free(uint64_t *mov_window)
{
	kfree(mov_window);
	return 0;
}

void *telemetry_sawf_alloc_peer(void *soc, void *sawf_stats_ctx,
				uint8_t *mac_addr,
				uint8_t svc_id, uint8_t host_q_id)
{
	struct telemetry_sawf_peer_ctx *peer_ctx;
	uint64_t *mov_window;
	struct peer_sawf_queue *peer_msduq;

	peer_ctx = kzalloc(sizeof(struct telemetry_sawf_peer_ctx), GFP_ATOMIC);
	if (!peer_ctx) {
		pr_sawf_err("peer context allocation failed");
		return NULL;
	}

	mov_window = telemtry_sawf_delay_window_alloc();
	if (!mov_window) {
		pr_sawf_err("windows allocation failed");
		kfree(peer_ctx);
		return NULL;
	}

	pr_sawf_dbg("Alloc peer ctx %pK for host QID:%d and service ID:%d",
		    peer_ctx, host_q_id, svc_id);

	peer_ctx->mov_window = mov_window;
	peer_ctx->sawf_ctx = sawf_stats_ctx;
	peer_ctx->soc = soc;
	memcpy(peer_ctx->mac_addr, mac_addr, MAC_ADDR_SIZE);

	peer_msduq = &(peer_ctx->peer_msduq);
	peer_msduq->msduq[host_q_id].svc_id = svc_id;

	set_bit(host_q_id, peer_msduq->active_queue);

	if (list_empty(&g_telemetry_sawf->peer_list))
		telemetry_sawf_init_sla_timer();

	list_add_tail(&peer_ctx->node, &g_telemetry_sawf->peer_list);

	return peer_ctx;
}

int telemetry_sawf_update_queue_info(void *telemetry_ctx, uint8_t host_q_id,
				     uint8_t tid, uint8_t msduq)
{
	struct telemetry_sawf_peer_ctx *peer_ctx;
	struct peer_sawf_queue *peer_msduq;

	peer_ctx = telemetry_ctx;
	peer_msduq = &(peer_ctx->peer_msduq);

	peer_msduq->msduq[host_q_id].htt_msduq = msduq;
	peer_msduq->msduq[host_q_id].tid = tid;
	peer_msduq->msduq_map[tid][msduq] = host_q_id;

	pr_sawf_info("Host Q Idx: %d | TID: %d | Queue ID: %d|",
		     peer_msduq->msduq[host_q_id].htt_msduq,
		     peer_msduq->msduq[host_q_id].tid = tid,
		     peer_msduq->msduq_map[tid][msduq]);

	return 0;
}

void telemetry_sawf_free_peer(void *telemetry_ctx)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;

	list_del(&(peer_ctx->node));

	if (list_empty(&g_telemetry_sawf->peer_list))
		telemetry_sawf_free_sla_timer();

	pr_sawf_dbg("Free SAWF ctx %pK", peer_ctx);
	telemtry_sawf_delay_window_free(peer_ctx->mov_window);
	peer_ctx->mov_window = NULL;

	kfree(peer_ctx);
}

static int
telemetry_get_tput_stats(struct telemetry_sawf_peer_ctx *peer,
			 uint64_t *in_bytes, uint64_t *in_cnt,
			 uint64_t *tx_bytes, uint64_t *tx_cnt,
			 uint8_t svc_id,
			 uint8_t tid, uint8_t queue)
{
	return 0;
}

static int
telemetry_get_drop_stats_num_sec(struct telemetry_sawf_peer_ctx *peer,
				    uint64_t *pass, uint64_t *drop,
				    uint64_t *drop_ttl,
				    uint8_t svc_id,
				    uint8_t tid, uint8_t queue)
{
	return 0;
}

static int
telemetry_get_mpdu_stats_num_sec(struct telemetry_sawf_peer_ctx *peer,
				 uint64_t *svc_int_pass, uint64_t *svc_int_fail,
				 uint64_t *burst_pass, uint64_t *burst_fail,
				 uint8_t svc_id,
				 uint8_t tid, uint8_t queue)
{
	return 0;
}

static int
telemetry_detect_breach_peer_delay_num_pkt(struct telemetry_sawf_peer_ctx *peer,
				     uint8_t svc_id,
				     uint8_t tid, uint8_t queue,
				     uint64_t pass, uint64_t fail)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_svc_class_param *svc_param;
	struct telemetry_sawf_sla_param *sla_param;
	uint64_t pass_p;
	uint32_t threshold;

	sawf_ctx = telemetry_sawf_get_ctx();
	svc_param = &sawf_ctx->svc_class.svc_class[svc_id];
	sla_param = &sawf_ctx->sla.sla_cfg[svc_id];

	if (sawf_ctx->sla.sla_cfg[svc_id].delay_bound) {
		threshold = sla_param->delay_bound;
		pass_p = (pass * 100)/(pass + fail);
		if (pass_p < threshold) {
			pr_sawf_err("SLA Delay Breach(num_pkt) Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("Delay threshold: %d", threshold);
			pr_sawf_err("Delay Success: %lld", pass);
			pr_sawf_err("Delay Failure: %lld", fail);
			pr_sawf_err("Pass Percentage: %lld", pass_p);
		}
	}

	return 0;
}

static int
telemetry_detect_breach_peer_drop_num_pkt(struct telemetry_sawf_peer_ctx *peer,
				     uint8_t svc_id,
				     uint8_t tid, uint8_t queue,
				     uint64_t pass, uint64_t drop,
				     uint64_t drop_ttl)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_svc_class_param *svc_param;
	struct telemetry_sawf_sla_param *sla_param;
	uint64_t pass_p;
	uint32_t threshold;

	sawf_ctx = telemetry_sawf_get_ctx();
	svc_param = &sawf_ctx->svc_class.svc_class[svc_id];
	sla_param = &sawf_ctx->sla.sla_cfg[svc_id];

	if (sawf_ctx->sla.sla_cfg[svc_id].msdu_ttl) {
		threshold = sla_param->msdu_ttl;
		pass_p = (drop_ttl * 100)/pass;
		if (pass_p < threshold) {
			pr_sawf_err("SLA MSDU TTL Loss Breach(num_pkt) Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("MSDU TTL Drop threshold: %d", threshold);
			pr_sawf_err("MSDU TTL Drop Success: %lld", pass);
			pr_sawf_err("MSDU TTL Drop Failure: %lld", drop_ttl);
			pr_sawf_err("Pass Percentage: %lld", pass_p);
		}
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].msdu_rate_loss) {
	}
		threshold = sla_param->msdu_rate_loss ;
		pass_p = svc_param->msdu_rate_loss * MSDU_LOSS_UNIT * pass;
		if (drop < ((pass_p * 100)/threshold)) {
			pr_sawf_err("SLA MSDU Loss Breach(num_pkt) Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("MSDU Drop Threshold: %d", threshold);
			pr_sawf_err("MSDU Drop Max Count: %lld", pass_p);
			pr_sawf_err("MSDU Drop Count: %lld", drop);
		}

	return 0;
}

static int
telemetry_detect_breach_peer_mov_avg(struct telemetry_sawf_peer_ctx *peer,
				     uint8_t svc_id,
				     uint8_t tid, uint8_t queue,
				     uint64_t mov_avg_delay)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_svc_class_param *svc_param;
	struct telemetry_sawf_sla_param *sla_param;
	uint32_t delay_bound;
	uint32_t threshold;
	uint32_t max_delay;

	sawf_ctx = telemetry_sawf_get_ctx();
	svc_param = &sawf_ctx->svc_class.svc_class[svc_id];
	sla_param = &sawf_ctx->sla.sla_cfg[svc_id];

	threshold = sla_param->delay_bound;

	if (threshold) {
		delay_bound = svc_param->delay_bound * DELAY_BOUND_UNIT;
		max_delay = delay_bound + ((delay_bound *(100 - threshold))/100) ;
		if (mov_avg_delay > max_delay) {
			pr_sawf_err("SLA Delay Breach Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("Configured Delay Bound: %d", delay_bound);
			pr_sawf_err("Delay Moving Average: %lld", mov_avg_delay);
			pr_sawf_err("Configured Delay Bound thresd: %d", threshold);
			pr_sawf_err("Max Delay Bound threshold:%d", max_delay);
		}
	}

	return 0;
}

static int
telemetry_detect_breach_peer_per_sec(struct telemetry_sawf_peer_ctx *peer,
				     uint8_t svc_id,
				     uint8_t tid, uint8_t queue)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_throughput *tput;
	struct telemetry_sawf_svc_class_param *svc_param;
	struct telemetry_sawf_sla_param *sla_param;
	uint64_t in_bytes = 0, in_cnt = 0;
	uint64_t tx_bytes = 0, tx_cnt = 0;
	uint64_t in_rate = 0, eg_rate = 0;
	uint32_t throughput;
	uint32_t threshold;

	sawf_ctx = telemetry_sawf_get_ctx();
	svc_param = &sawf_ctx->svc_class.svc_class[svc_id];
	sla_param = &sawf_ctx->sla.sla_cfg[svc_id];

	telemetry_get_tput_stats(peer, &in_bytes, &in_cnt,
				 &tx_bytes, &tx_cnt, svc_id, tid, queue);

	tput = &peer->stats[tid][queue].tx.throughput;
	in_rate = in_bytes - tput->last_in_bytes;
	eg_rate = tx_bytes - tput->last_bytes;

	tput->last_in_bytes = in_bytes;
	tput->last_bytes = tx_bytes;
	tput->in_rate = in_rate;
	tput->eg_rate = eg_rate;


	if (!test_bit(svc_id, sawf_ctx->sla.sla_service_ids)) {
		pr_sawf_dbg("SLA Breach not configure for SVC ID: %d",
			    svc_id);
		return 0;
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].min_thruput_rate) {
		throughput = svc_param->min_thruput_rate * THROUGHPUT_UNIT;
		if (in_rate > svc_param->min_thruput_rate &&
		    eg_rate < (throughput * threshold)/100) {
			pr_sawf_err("SLA Min Throughput Breach Detected peer service ID:%d",
				    svc_id);
			pr_sawf_err("Configured Min Throughput: %d", throughput);
			pr_sawf_err("Ingres Throughput: %d", tput->in_rate);
			pr_sawf_err("Egress Throughput: %d", tput->eg_rate);
		}
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].max_thruput_rate) {
		throughput = sla_param->max_thruput_rate * THROUGHPUT_UNIT;
		if (in_rate > svc_param->max_thruput_rate &&
		    eg_rate < (throughput * threshold)/100) {
			pr_sawf_err("SLA Max Throughput Breach Detected peer service ID:%d",
				    svc_id);
			pr_sawf_err("Configured Max Throughput: %d", throughput);
			pr_sawf_err("Ingres Throughput: %d", tput->in_rate);
			pr_sawf_err("Egress Throughput: %d", tput->eg_rate);
		}
	}

	return 0;
}

static int
telemetry_detect_breach_peer_num_sec(struct telemetry_sawf_peer_ctx *peer,
				     uint8_t svc_id, uint8_t tid, uint8_t queue)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_drops *msdu_drop;
	struct telemetry_service_interval *svc_int;
	struct telemetry_burst_size *burst;
	struct telemetry_sawf_svc_class_param *svc_param;
	struct telemetry_sawf_sla_param *sla_param;
	uint32_t threshold;

	uint64_t svc_int_pass = 0, svc_int_fail = 0;
	uint64_t burst_pass = 0, burst_fail = 0;
	uint64_t pass = 0, drop = 0, drop_ttl = 0;

	uint64_t curr_svc_int_pass = 0, curr_svc_int_fail = 0;
	uint64_t curr_burst_pass = 0, curr_burst_fail = 0;
	uint64_t curr_pass = 0, curr_drop = 0, curr_drop_ttl = 0;
	uint64_t pass_p;

	sawf_ctx = telemetry_sawf_get_ctx();
	svc_param = &sawf_ctx->svc_class.svc_class[svc_id];
	sla_param = &sawf_ctx->sla.sla_cfg[svc_id];

	telemetry_get_mpdu_stats_num_sec(peer,
					 &svc_int_pass, &svc_int_fail,
					 &burst_pass, &burst_fail,
					 svc_id, tid, queue);

	burst = &peer->stats[tid][queue].tx.burst_size;
	curr_burst_pass = burst_pass - burst->last_success;
	curr_burst_fail = burst_fail - burst->last_failure;

	svc_int = &peer->stats[tid][queue].tx.service_interval;
	curr_svc_int_pass = svc_int_pass - svc_int->last_success;
	curr_svc_int_fail = svc_int_fail - svc_int->last_failure;

	burst->last_success = burst_pass;
	burst->last_failure = burst_fail;
	svc_int->last_success = svc_int_pass;
	svc_int->last_failure = svc_int_fail;

	telemetry_get_drop_stats_num_sec(peer,
					 &pass, &drop, &drop_ttl,
					 svc_id,
					 tid, queue);

	msdu_drop = &peer->stats[tid][queue].tx.msdu_drop;
	curr_pass = pass - msdu_drop->last_xmit_cnt_seconds;
	curr_drop = drop - msdu_drop->last_drop_cnt_seconds;
	curr_drop_ttl = drop_ttl - msdu_drop->last_ttl_drop_cnt_seconds;

	msdu_drop->last_xmit_cnt_seconds = pass;
	msdu_drop->last_drop_cnt_seconds = drop;
	msdu_drop->last_ttl_drop_cnt_seconds = drop_ttl;

#ifdef VIVEK
	pr_sawf_dbg("Detect svc_intvl/burst_size/msdu_drop num sec"
		    "SLA breach for svcid:%d", svc_id);
	pr_sawf_dbg("curr_svc_int_pass:%lld", curr_svc_int_pass);
	pr_sawf_dbg("curr_svc_int_fail:%lld", curr_svc_int_fail);
	pr_sawf_dbg("curr_pass:%lld", curr_pass );
	pr_sawf_dbg("curr_drop:%lld", curr_drop);
	pr_sawf_dbg("curr_drop_ttl:%lld", curr_drop_ttl);
	pr_sawf_dbg("curr_burst_pass:%lld", curr_burst_pass);
	pr_sawf_dbg("curr_burst_fail:%lld", curr_burst_fail);
#endif

	if (!test_bit(svc_id, sawf_ctx->sla.sla_service_ids)) {
		pr_sawf_dbg("SLA Breach not configure for SVC ID: %d",
			    svc_id);
		return 0;
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].burst_size) {
		threshold = sla_param->burst_size;
		pass_p = (curr_burst_pass * 100)/(curr_burst_pass + curr_burst_fail);
		if (pass_p < threshold) {
			pr_sawf_err("SLA Burst Size(per_x_sec) Breach Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("Burst Size threshold: %d", threshold);
			pr_sawf_err("Burst Size success: %lld", curr_burst_pass);
			pr_sawf_err("Burst Size Failure: %lld", curr_burst_fail);
			pr_sawf_err("Pass Percentage: %lld", pass_p);
		}
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].service_interval) {
		threshold = sla_param->service_interval;
		pass_p = (curr_svc_int_pass * 100)/(curr_svc_int_pass + curr_svc_int_fail);
		if (pass_p < threshold) {
			pr_sawf_err("SLA Service Interval(per_s_sec) Breach Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("Service Interval threshold: %d", threshold);
			pr_sawf_err("Service Interval success: %lld", curr_burst_pass);
			pr_sawf_err("Service Interval Failure: %lld", curr_burst_fail);
			pr_sawf_err("Pass Percentage: %lld", pass_p);
		}
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].msdu_ttl) {
		threshold = sla_param->msdu_ttl;
		pass_p = (curr_drop_ttl * 100)/curr_pass;
		if (pass_p < threshold) {
			pr_sawf_err("SLA MSDU TTL(per_x_sec) Breach Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("MSDU TTL Drop threshold: %d", threshold);
			pr_sawf_err("MSDU TTL Drop Success: %lld", curr_pass);
			pr_sawf_err("MSDU TTL Drop Failure: %lld", curr_drop_ttl);
			pr_sawf_err("Pass Percentage: %lld", pass_p);
		}
	}

	if (sawf_ctx->sla.sla_cfg[svc_id].msdu_rate_loss) {
		threshold = sla_param->msdu_rate_loss ;
		pass_p = svc_param->msdu_rate_loss * MSDU_LOSS_UNIT * curr_pass;
		if (curr_drop < ((pass_p * 100)/threshold)) {
			pr_sawf_err("SLA MSDU Loss(per_x_sec) Breach Detected peer service ID: %d",
				    svc_id);
			pr_sawf_err("MSDU Drop Threshold: %d", threshold);
			pr_sawf_err("MSDU Drop Max Count: %lld", pass_p);
			pr_sawf_err("MSDU Drop Count: %lld", curr_drop);
		}
	}

	return 0;
}

static int
telemetry_detect_breach_peer(struct telemetry_sawf_peer_ctx *peer,
			     enum telemetry_sawf_sla_detect_type type)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct peer_sawf_queue *peer_queue;
	uint8_t bit, svc_id;
	uint8_t tid, queue;

	if (!peer) {
		pr_sawf_err("Peer ctx is null");
		return -1;
	}

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return -1;
	}

	peer_queue = &peer->peer_msduq;
	for_each_set_bit(bit, peer_queue->active_queue, SAWF_MAX_QUEUES) {
#ifdef VIVEK
		pr_sawf_dbg("Host Q: %d | Service ID: %d| TID: %d| MSDUQ: %d",
			    bit, peer_queue->msduq[bit].svc_id,
			    peer_queue->msduq[bit].tid,
			    peer_queue->msduq[bit].htt_msduq);
#endif

		tid = peer_queue->msduq[bit].tid;
		queue = peer_queue->msduq[bit].htt_msduq;
		svc_id = peer_queue->msduq[bit].svc_id;

		switch (type) {
		case SAWF_SLA_DETECT_PER_SECOND:
			telemetry_detect_breach_peer_per_sec(peer, svc_id,
							     tid, queue);
			break;
		case SAWF_SLA_DETECT_NUM_SECOND:
			telemetry_detect_breach_peer_num_sec(peer, svc_id,
							     tid, queue);
			break;
		default:
			break;
		}
	}

	return 0;
}

static int telemetry_detect_breach(enum telemetry_sawf_sla_detect_type type)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct list_head *node;
	struct telemetry_sawf_peer_ctx *peer;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return -1;
	}

	list_for_each(node, &sawf_ctx->peer_list) {
		peer = list_entry(node, struct telemetry_sawf_peer_ctx, node);
		if (!peer) {
			pr_sawf_err("Peer ctx is null");
			continue;
		}

		switch (type) {
		case SAWF_SLA_DETECT_PER_SECOND:
			telemetry_detect_breach_peer(peer, SAWF_SLA_DETECT_PER_SECOND);
			break;
		case SAWF_SLA_DETECT_NUM_SECOND:
			telemetry_detect_breach_peer(peer, SAWF_SLA_DETECT_NUM_SECOND);
			break;
		default:
			break;
		}
	}
	return 0;
}

int telemetry_sawf_update_peer_delay(void *telemetry_ctx, uint8_t tid, uint8_t queue,
				uint64_t pass, uint64_t fail)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;
	uint64_t curr_success, curr_failure;
	struct telemetry_sawf_stats *stats;
	struct sawf_msduq *msduq;
	uint8_t msduq_id;

	stats = &peer_ctx->stats[tid][queue];

	curr_success = pass - stats->delay.last_success;
	curr_failure = fail - stats->delay.last_failure;

	msduq_id = peer_ctx->peer_msduq.msduq_map[tid][queue];
	msduq = &peer_ctx->peer_msduq.msduq[msduq_id];
	telemetry_detect_breach_peer_delay_num_pkt(peer_ctx, msduq->svc_id,
						   curr_success, curr_failure,
						   tid, queue);

	return 0;
}

static inline
uint64_t *telemetry_get_mov_window_offset(void *telemetry_ctx, uint8_t tid,
					  uint8_t queue,
					  uint32_t curr_win)
{
	struct telemetry_sawf_peer_ctx *peer_ctx;
	uint32_t offset;
	uint32_t max_win;
	uint64_t *mov_win_start;
	uint64_t *queue_offset;
	uint64_t *q_win_offest;

	peer_ctx = telemetry_ctx;
	max_win = g_telemetry_sawf->mov_avg.window;
	mov_win_start = peer_ctx->mov_window;
	offset = ((tid * SAWF_MAXQ_PTID) + queue);
	queue_offset = mov_win_start  + (offset * sizeof(uint64_t) * max_win);
	q_win_offest = queue_offset + (curr_win* sizeof(uint64_t));

	return q_win_offest;
}

int telemetry_sawf_update_peer_delay_mov_avg(void *telemetry_ctx,
					     uint8_t tid, uint8_t queue,
					     uint64_t window_delay)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;
	struct telemetry_sawf_stats *stats;
	uint32_t max_win;
	uint64_t *q_window;
	struct telemetry_sawf_delay *delay;
	uint32_t curr_win;
	struct sawf_msduq *msduq;
	uint8_t msduq_id;

	stats = &peer_ctx->stats[tid][queue];
	delay = &stats->delay;
	max_win = g_telemetry_sawf->mov_avg.window;

	if (delay->cur_window < max_win) {
		curr_win = delay->cur_window;
		q_window = telemetry_get_mov_window_offset(telemetry_ctx,
							   tid, queue,
							   curr_win);
		*q_window = window_delay;
		delay->windows_total = delay->windows_total + window_delay;
		pr_sawf_dbg("DELAY: Total delay for window(%lld) is %lld", delay->cur_window, delay->windows_total);
		delay->cur_window++;
	} else if (delay->cur_window == max_win) {
		delay->mov_avg = delay->windows_total/max_win;
		pr_sawf_dbg("========================================");
		pr_sawf_dbg("DELAY: Max window: %d", max_win);
		pr_sawf_dbg("DELAY: Average delay for window(%lld) is %lld", delay->windows_total, delay->mov_avg);
		pr_sawf_dbg("========================================");
		//Detect moving average SLA
		delay->cur_window = 0;
		delay->windows_total = 0;
		msduq_id = peer_ctx->peer_msduq.msduq_map[tid][queue];
		msduq = &peer_ctx->peer_msduq.msduq[msduq_id];
		telemetry_detect_breach_peer_mov_avg(peer_ctx, msduq->svc_id,
						     tid, queue,
						     delay->mov_avg);
	}

	return 0;
}

int telemetry_sawf_update_msdu_drop(void *telemetry_ctx, uint8_t tid,
				    uint8_t queue, uint64_t pass,
				    uint64_t fail_drop, uint64_t fail_ttl)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;
	struct telemetry_sawf_stats *stats;
	struct telemetry_sawf_drops *msdu_drop;
	uint64_t curr_pass, curr_fail, curr_fail_ttl;
	struct sawf_msduq *msduq;
	uint8_t msduq_id;


	stats = &peer_ctx->stats[tid][queue];
	msdu_drop = &stats->tx.msdu_drop;

	curr_pass = pass - msdu_drop->last_xmit_cnt_packets - pass;
	curr_fail = fail_drop - msdu_drop->last_drop_cnt_packets;
	curr_fail_ttl = fail_ttl - msdu_drop->last_ttl_drop_cnt_packets;

	msduq_id = peer_ctx->peer_msduq.msduq_map[tid][queue];
	msduq = &peer_ctx->peer_msduq.msduq[msduq_id];
	telemetry_detect_breach_peer_drop_num_pkt(peer_ctx, msduq->svc_id,
						  tid, queue,
						  curr_pass, curr_fail,
						  curr_fail_ttl);
	return 0;
}

int telemetry_sawf_get_rate(void *telemetry_ctx, uint8_t tid, uint8_t queue,
			    uint32_t *rate)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;
	struct telemetry_sawf_stats *stats;

	stats = &peer_ctx->stats[tid][queue];

	*rate = stats->tx.throughput.eg_rate;

	return 0;
}

int telemetry_sawf_get_mov_avg(void *telemetry_ctx, uint8_t tid, uint8_t queue,
			       uint32_t *mov_avg)
{
	struct telemetry_sawf_peer_ctx *peer_ctx = telemetry_ctx;
	struct telemetry_sawf_stats *stats;

	stats = &peer_ctx->stats[tid][queue];

	*mov_avg = stats->delay.mov_avg;

	return 0;
}

int telemetry_sawf_init_sla_timer(void)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_sla_params *sla_param;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return -1;
	}

	sla_param = telemetry_sawf_get_sla_param();

	timer_setup(&sawf_ctx->per_sec_timer,
		    telemetry_detect_sla_per_sec, 0);
	timer_setup(&sawf_ctx->num_sec_timer,
		    telemetry_detect_sla_num_sec, 0);

	if (!find_first_bit(sawf_ctx->sla.sla_service_ids,
			    SAWF_MAX_SVC_CLASS)) {

		pr_sawf_dbg("SLA not configured for any service class");
		return 0;
	}

	add_timer(&sawf_ctx->per_sec_timer);
	add_timer(&sawf_ctx->num_sec_timer);

	mod_timer(&sawf_ctx->per_sec_timer,
		  jiffies + msecs_to_jiffies(1000));
	mod_timer(&sawf_ctx->num_sec_timer,
		  jiffies + msecs_to_jiffies(sla_param->time_secs * 1000));

	return 0;
}

int telemetry_sawf_free_sla_timer(void)
{
	struct telemetry_sawf_ctx *sawf_ctx;

	sawf_ctx = telemetry_sawf_get_ctx();
	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return -1;
	}

	del_timer(&sawf_ctx->per_sec_timer);
	del_timer(&sawf_ctx->num_sec_timer);

	return 0;
}

void telemetry_detect_sla_per_sec(struct timer_list *timer)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_sla_param *sla_detect;
	sawf_ctx = from_timer(sawf_ctx, timer, per_sec_timer);

	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return;
	}

	sla_detect = &sawf_ctx->sla_detect.sla_detect[SAWF_SLA_DETECT_PER_SECOND];

	if (sla_detect->min_thruput_rate || sla_detect->max_thruput_rate) {
		telemetry_detect_breach(SAWF_SLA_DETECT_PER_SECOND);
	}

	mod_timer(&sawf_ctx->per_sec_timer, MSEC_TO_JIFFIES(1000));
}

void telemetry_detect_sla_num_sec(struct timer_list *timer)
{
	struct telemetry_sawf_ctx *sawf_ctx;
	struct telemetry_sawf_sla_param *sla_detect;
	sawf_ctx = from_timer(sawf_ctx, timer, num_sec_timer);

	if (!sawf_ctx) {
		pr_sawf_err("SAWF ctx is null");
		return;
	}

	sla_detect = &sawf_ctx->sla_detect.sla_detect[SAWF_SLA_DETECT_NUM_SECOND];

	//Check for SLA breaches
	if (sla_detect->burst_size || sla_detect->service_interval
	    || sla_detect->msdu_ttl || sla_detect->msdu_rate_loss) {
		telemetry_detect_breach(SAWF_SLA_DETECT_NUM_SECOND);
	}

	mod_timer(&sawf_ctx->num_sec_timer,
		  MSEC_TO_JIFFIES(sawf_ctx->sla_param.time_secs * 1000));
}
