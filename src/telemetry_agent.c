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

#include "../inc/telemetry_agent.h"

struct telemetry_agent_object g_agent_obj; 
struct telemetry_buffer stats_buffer;

/**
 *   print_mac_addr: prints the mac address.
 *   @mac: pointer to the mac address
 *
 *   return pointer to string
 */
static char *print_mac_addr(const uint8_t *mac)
{
	static char buf[32] = {'\0', };
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return buf;
}

void telemetry_agent_stats_work(struct work_struct *work) 
{
	/* schedule_delayed_work(&g_agent_obj.stats_work, msecs_to_jiffies(STATS_FREQUECY)) */
	int i, j, k;
	struct agent_soc_db *psoc_db = NULL;
	struct agent_pdev_db *pdev_db = NULL;
	struct agent_peer_db *peer_db = NULL;
	int num_socs = 0;
	int num_pdevs = 0;
	int num_peers = 0, num_mlo_peers = 0;
	struct agent_psoc_iface_init_obj soc_stats = {0};
	struct agent_pdev_iface_init_obj pdev_stats = {0};
	struct agent_peer_iface_init_obj peer_stats = {0};

	struct agent_soc_init_stats *rfs_soc_stats = NULL;
	struct agent_link_init_stats *rfs_link_stats = NULL;
	struct agent_peer_init_stats *rfs_peer_stats = NULL;

	printk("Agent > %s\n", __func__);
	printk("Dumping Telemetry data base\n");

	stats_buffer.header.start_magic_num = 0xDEADBEAF;
	stats_buffer.header.stats_version = 1;
	stats_buffer.header.stats_type = RFS_INIT_DATA;
	stats_buffer.header.payload_len = sizeof(struct telemetry_buffer);

	printk("Agent:%s, payloadlen: %d \n", __func__, 
			stats_buffer.header.payload_len);

	num_socs = g_agent_obj.agent_db.num_socs;
	stats_buffer.u.init_stats.num_soc = num_socs;

	for (i = 0; i< num_socs; i++) {
		psoc_db = &g_agent_obj.agent_db.psoc_db[i];
		rfs_soc_stats = &stats_buffer.u.init_stats.soc_stats[i];

		rfs_soc_stats->soc_id = soc_stats.soc_id;
		rfs_soc_stats->max_peers = soc_stats.max_peers;
		rfs_soc_stats->num_peers = soc_stats.num_peers;

		printk("AgentDB-WorkQ> %s: psoc: %p psoc_id: %d\n",__func__,
				psoc_db->psoc_obj_ptr, psoc_db->soc_id);
		/* call wifi driver and prepare RelayFS Message */
		//wifi_driver_get_psoc_info(psoc_db->psoc_obj_ptr, &soc_stats);
		num_pdevs = psoc_db->num_pdevs;

		for (j = 0; j< num_pdevs; j++) {
			pdev_db  = &psoc_db->pdev_db[j];
			rfs_link_stats = &rfs_soc_stats->link_stats[j];
			//wifi_driver_get_pdev_info(pdev_db->pdev_obj_ptr, &pdev_stats);
			rfs_link_stats->hw_link_id = pdev_stats.link_id;
			rfs_link_stats->band = pdev_stats.band;
			/* stats_buffer.u.init_stats.soc_stats[i].link_stats[j].soc_id
			  = pdev_stats.soc_id; */

			printk("AgentDB-WorkQ> %s: psoc: %p pdev: %p pdev_id: %d\n",
                    __func__,
					pdev_db->psoc_obj_ptr ,
					pdev_db->pdev_obj_ptr, pdev_db->pdev_id
				  );
			num_peers = pdev_db->num_peers;
			num_mlo_peers = 0;
			for (k = 0; k < num_peers; k++) {
				peer_db = &pdev_db->peer_db[k];
				if (1) {
					//wifi_driver_get_peer_info(peer_db->peer_obj_ptr, &peer_stats)) {
					rfs_peer_stats = &rfs_link_stats->peer_stats[num_mlo_peers];
					memcpy(&rfs_peer_stats->mld_mac_addr[0],
							peer_stats.peer_mld_mac, 6);
					memcpy(&rfs_peer_stats->link_mac_addr[0],
							peer_stats.peer_link_mac, 6);
					/*
					rfs_peer_stats->link_id = peer_stats.link_id;
					rfs_peer_stats->link_id = peer_stats.link_id;
					*/
					rfs_peer_stats->chan_bw = peer_stats.bw;
					rfs_peer_stats->chan_freq = peer_stats.freq;
					num_mlo_peers++;
				}

				printk("AgentDB - WorkQ> %s: psoc: %p pdev: %p \n peer: %p peer_mac: %s\n",__func__,
						peer_db->psoc_obj_ptr,
						peer_db->pdev_obj_ptr,
						peer_db->peer_obj_ptr,
                        print_mac_addr(&peer_db->peer_mac_addr[0])
					  );
			} /* peer */
				rfs_link_stats->num_peers = num_mlo_peers;
		} /* pdev */
	} /* soc */

	stats_buffer.end_magic_num = 0xBEAFDEAD;
	relay_write(g_agent_obj.rfs_channel, &stats_buffer,
            sizeof(struct telemetry_buffer));
	relay_flush(g_agent_obj.rfs_channel);

	return;
}

void telemetry_agent_notify_app_init(void)
{
	printk("Agent> %s\n", __func__);
	schedule_delayed_work(&g_agent_obj.stats_work, msecs_to_jiffies(1000)); 
	return;
}

static int remove_buf_file_handler(struct dentry *dentry)
{
	debugfs_remove(dentry);
	return 0;
}

static struct dentry *create_buf_file_handler(const char *filename,
		struct dentry *parent,
		umode_t mode,
		struct rchan_buf *buf,
		int *is_global)
{
	struct dentry *buf_file;

	buf_file = debugfs_create_file(filename, mode, parent, buf,
			&relay_file_operations);
	if (IS_ERR(buf_file))
		return NULL;

	*is_global = 1;
	return buf_file;
}

static struct rchan_callbacks rfs_telemetry_agent_cb = {
	.create_buf_file = create_buf_file_handler,
	.remove_buf_file = remove_buf_file_handler,
};


int telemetry_agent_deinit_relayfs(struct telemetry_agent_object *agent_obj) 
{
	if (g_agent_obj.rfs_channel) {
		relay_close(g_agent_obj.rfs_channel);
	}

	debugfs_remove_recursive(g_agent_obj.dir_ptr);
	return STATUS_SUCCESS;
}

int telemetry_agent_init_relayfs(struct telemetry_agent_object *agent_obj) 
{
	agent_obj->dir_ptr = debugfs_create_dir("qca_telemetry", NULL);
	if (agent_obj->dir_ptr == NULL) 
		return -EPERM;

	agent_obj->rfs_channel = relay_open("telemetry_agent",
			agent_obj->dir_ptr,
			AGENT_MAX_BUFFER_SIZE, MAX_SUB_BUFFERS,
			&rfs_telemetry_agent_cb, NULL);

	if (!agent_obj->rfs_channel) {
		debugfs_remove_recursive(agent_obj->dir_ptr);
		agent_obj->dir_ptr = NULL;
		return -EPERM;
	}

	return STATUS_SUCCESS;
}

static int telemetry_agent_psoc_create_handler(void *arg, struct agent_psoc_obj *psoc_obj)
{
	int status = STATUS_SUCCESS;
	int soc_idx = g_agent_obj.agent_db.num_socs;
	struct agent_soc_db *psoc_db = &g_agent_obj.agent_db.psoc_db[soc_idx];

	printk("Agent> %s: psoc: %p psoc_id: %d\n",__func__,
			psoc_obj->psoc_back_pointer, psoc_obj->psoc_id);

	psoc_db->psoc_obj_ptr = psoc_obj->psoc_back_pointer;
	psoc_db->soc_id = soc_idx;
	psoc_db->num_pdevs = 0;

	printk("AgentDB> %s: psoc: %p psoc_id: %d\n",__func__,
			psoc_db->psoc_obj_ptr, psoc_db->soc_id);

	g_agent_obj.agent_db.num_socs++;

	return status;
}


static int telemetry_agent_pdev_create_handler(void *arg, struct agent_pdev_obj *pdev_obj)

{
	int status = STATUS_SUCCESS;
	/*
	 * FIXME: soc_idx:Correct thing is search psoc pointer in local db and retrun index
	 */

	struct agent_soc_db *psoc_db = NULL;
	struct agent_pdev_db *pdev_db = NULL;
	int pdev_idx; 

	psoc_db  = &g_agent_obj.agent_db.psoc_db[pdev_obj->psoc_id];
	pdev_idx = psoc_db->num_pdevs;
	pdev_db  = &psoc_db->pdev_db[pdev_idx];

	printk("Agent> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d\n",__func__,
			pdev_obj->psoc_back_pointer, pdev_obj->psoc_id,
			pdev_obj->pdev_back_pointer, pdev_obj->pdev_id
		  );

	pdev_db->psoc_obj_ptr =  pdev_obj->psoc_back_pointer;
	pdev_db->pdev_obj_ptr = pdev_obj->pdev_back_pointer;
	pdev_db->pdev_id = pdev_idx;
	pdev_db->num_peers = 0;

	printk("AgentDB> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d\n",__func__,
			pdev_db->psoc_obj_ptr , pdev_obj->psoc_id,
			pdev_db->pdev_obj_ptr, pdev_db->pdev_id
		  );
	psoc_db->num_pdevs++;
#if 0
	char buffer[200] = {'\0', };
	snprintf(buffer, sizeof(buffer), "Agent> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d\n",__func__,
			pdev_obj->psoc_back_pointer, pdev_obj->psoc_id,
			pdev_obj->pdev_back_pointer, pdev_obj->pdev_id
			);

	relay_write(g_agent_obj.rfs_channel, &buffer[0], strlen(buffer));
#endif

	return status;
}

static int  telemetry_agent_peer_create_handler(void *arg, struct agent_peer_obj *peer_obj)

{
	int status = STATUS_SUCCESS;
	struct agent_soc_db *psoc_db = NULL;
	struct agent_pdev_db *pdev_db = NULL;
	struct agent_peer_db *peer_db = NULL;
	int peer_idx = 0;

	psoc_db = &g_agent_obj.agent_db.psoc_db[peer_obj->psoc_id];
	pdev_db = &psoc_db->pdev_db[peer_obj->pdev_id];
	peer_idx = pdev_db->num_peers;
	peer_db = &pdev_db->peer_db[peer_idx];

	printk("Agent> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d \n peer: %p peer_mac: %s\n",__func__,
			peer_obj->psoc_back_pointer, peer_obj->psoc_id,
			peer_obj->pdev_back_pointer, peer_obj->pdev_id,
			peer_obj->peer_back_pointer, print_mac_addr(&peer_obj->peer_mac_addr[0])
		  );

	peer_db->psoc_obj_ptr = peer_obj->psoc_back_pointer;
	peer_db->pdev_obj_ptr = peer_obj->pdev_back_pointer;
	peer_db->peer_obj_ptr = peer_obj->peer_back_pointer;

	memcpy(&peer_db->peer_mac_addr[0], &peer_obj->peer_mac_addr[0], 6);

	printk("AgentDB> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d \n peer: %p peer_mac: %s\n",__func__,
			peer_db->psoc_obj_ptr, peer_obj->psoc_id,
			peer_db->pdev_obj_ptr, peer_obj->pdev_id,
			peer_db->peer_obj_ptr, print_mac_addr(&peer_db->peer_mac_addr[0])
		  );

	pdev_db->num_peers++;	
	return status;
}


static int telemetry_agent_psoc_destroy_handler(void *arg, struct agent_psoc_obj *psoc_obj)

{
	int status = STATUS_SUCCESS;
	printk("Agent> %s: psoc: %p psoc_id: %d\n",__func__,
			psoc_obj->psoc_back_pointer, psoc_obj->psoc_id);


	return status;
}

static int telemetry_agent_pdev_destroy_handler(void *arg, struct agent_pdev_obj *pdev_obj)

{
	int status = STATUS_SUCCESS;
	printk("Agent> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d\n",__func__,
			pdev_obj->psoc_back_pointer, pdev_obj->psoc_id,
			pdev_obj->pdev_back_pointer, pdev_obj->pdev_id
		  );

	return status;
}

static int  telemetry_agent_peer_destroy_handler(void *arg, struct agent_peer_obj *peer_obj)
{
	int status = STATUS_SUCCESS;
	printk("Agent> %s: psoc: %p psoc_id: %d pdev: %p pdev_id: %d \n peer: %p peer_mac: %s\n",__func__,
			peer_obj->psoc_back_pointer, peer_obj->psoc_id,
			peer_obj->pdev_back_pointer, peer_obj->pdev_id,
			peer_obj->peer_back_pointer, print_mac_addr(&peer_obj->peer_mac_addr[0])
		  );

	return status;
}

static int telemetry_agent_set_param_handler(struct agent_config_params *params)
{
	int status = STATUS_SUCCESS;

	return status;
}

static int telemetry_agent_get_param_handler(struct agent_config_params *params)
{
	int status = STATUS_SUCCESS;

	return status;
}

struct telemetry_agent_ops agent_ops = {
	agent_psoc_create_handler:telemetry_agent_psoc_create_handler,
	agent_psoc_destroy_handler:telemetry_agent_psoc_destroy_handler,
	agent_pdev_create_handler:telemetry_agent_pdev_create_handler,
	agent_pdev_destroy_handler:telemetry_agent_pdev_destroy_handler,
	agent_peer_create_handler:telemetry_agent_peer_create_handler,
	agent_peer_destroy_handler:telemetry_agent_peer_destroy_handler,
	agent_set_param:telemetry_agent_set_param_handler,
	agent_get_param:telemetry_agent_get_param_handler,
	agent_notify_app_init:telemetry_agent_notify_app_init,
};

static int telemetry_agent_init_module(void)
{
	int status = 0;

	register_telemetry_agent_ops(&agent_ops);
	if(telemetry_agent_init_relayfs(&g_agent_obj) != STATUS_SUCCESS) {
		return -EPERM;
	}
	INIT_DELAYED_WORK(&g_agent_obj.stats_work, telemetry_agent_stats_work);

	return status;
}

static void telemetry_agent_exit_module(void)
{
	unregister_telemetry_agent_ops(&agent_ops);
	memset(&g_agent_obj, 0, sizeof(struct telemetry_agent_object));
	return;
}

module_init(telemetry_agent_init_module);
module_exit(telemetry_agent_exit_module);
MODULE_LICENSE("Dual BSD/GPL");
