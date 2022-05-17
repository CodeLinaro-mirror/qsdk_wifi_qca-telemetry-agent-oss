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

#ifndef TELEMETRY_AGENT_SAWF_H
#define TELEMETRY_AGENT_SAWF_H

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/relay.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <qdf_status.h>
#include <qdf_types.h>
#include <qdf_list.h>


struct tlmtry_agent_sawf_sla_params{
	uint8_t threshold[SAWF_STATS_TYPE_MAX];
}

struct tlmtry_agent_sawf_sla_params
tlmtry_agent_svclass_sla[SAWF_MAX_SERVICE_CLASS];

qdf_list_t tlmtry_agent_sawf_peer_list;

QDF_STATUS
tlmtry_agent_sawf_upd_delay_stats(uint16_t peer_id, uint8_t svc_id,
				  void *stats, struct moving_avg_params);
QDF_STATUS
tlmtry_agent_sawf_upd_svc_intv_stats(uint16_t peer_id, uint8_t svc_id,
				     void *stats);

QDF_STATUS
tlmtry_agent_sawf_upd_burst_size_stats(uint16_t peer_id, uint8_t svc_id,
				       void *stats);

QDF_STATUS
tlmtry_agent_sawf_upd_msdu_drop_stats(uint16_t peer_id, uint8_t svc_id,
				      void *stats);

QDF_STATUS
tlmtry_agent_sawf_upd_msdu_ttl_drop_stats(uint16_t peer_id, uint8_t svc_id,
					 void *stats);

QDF_STATUS
telemtry_agent_sawf_upd_sla_detect_param(uint8_t num_pkts,
					 uint8_t time_in_secs);

QDF_STATUS
tlmtry_agent_sawf_upd_sla_param(uint8_t svc_id,
				struct tlmtry_agent_sawf_sla_params
				*service_class_sla);

void
tlmtry_agent_sawf_detect_breach_num_packet(void);

void
tlmtry_agent_sawf_detect_breach_in_time(void);
#endif /* TELEMETRY_AGENT_SAWF_H */
