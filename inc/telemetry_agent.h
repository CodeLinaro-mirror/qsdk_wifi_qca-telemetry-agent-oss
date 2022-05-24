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
#ifndef __TELEMETRY_AGENT_H__
#define __TELEMETRY_AGENT_H__

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/relay.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include "telemetry_agent_wifi_driver_if.h"
#include "telemetry_agent_app_if.h"

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define AGENT_MAX_BUFFER_SIZE 46200
#define MAX_SUB_BUFFERS 10
#define STATS_FREQUECY  5000 /*in milli seconds */

struct agent_peer_db {
    void *peer_obj_ptr;
    void *pdev_obj_ptr;
    void *psoc_obj_ptr;
    uint8_t peer_mac_addr[6];
    uint8_t m1_stats;
    uint8_t m2_stats;
};

struct agent_pdev_db {
    void *pdev_obj_ptr;
    void *psoc_obj_ptr;
    uint8_t pdev_id;
    uint32_t m3_stats;
    uint8_t m4_stats;
    int num_peers;
	/* FIXME: convert to list */
    struct agent_peer_db peer_db[MAX_PEERS];
};

struct agent_soc_db {
	void *psoc_obj_ptr;
	uint8_t soc_id;
	uint8_t num_pdevs;
	struct agent_pdev_db pdev_db[MAX_PDEV_LINKS];
};

struct agent_telemtry_db {
	uint8_t num_socs;
	struct agent_soc_db psoc_db[MAX_SOCS];
};

struct telemetry_agent_object {
    /* RelayFS */
    uint32_t num_subbufs;
    uint32_t subbuf_size;
    /* relay(fs) channel */
    struct rchan *rfs_channel;
    struct dentry *dir_ptr;
    /* delyed work */
    struct delayed_work stats_work_init;
    /* delyed work periodic*/
    struct delayed_work stats_work_periodic;
    /* pdev and peer objects */
	struct agent_telemtry_db agent_db;
};


extern int register_telemetry_agent_ops(struct telemetry_agent_ops *agent_ops);
extern int unregister_telemetry_agent_ops(struct telemetry_agent_ops *agent_ops);
void telemetry_agent_stats_work(struct work_struct *work);
int telemetry_agent_register_umac_ops(void);
int telemetry_agent_init_relayfs(struct telemetry_agent_object *agent_obj);
#endif /* __TELEMETRY_AGENT_H__ */
