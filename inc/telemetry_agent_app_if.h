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

#define MAX_SOCS 3
#define MAX_PDEV_LINKS 2
#define MAX_MLD_LINKS 4
#define MAX_PEERS 128
#define NUM_TIDS 8
#define RFS_INIT_DATA 1
#define RFS_STATS_DATA 2
#define EHTCAP_TXRX_MCS_NSS_IDX_MAX 2


/*
   Buffer Format
   --------------------------------------------
   |          |                  |            |
   |  HEADER  |  Payload (stats) | Tail Data  |
   |          |                  |            |
   --------------------------------------------
 */

struct telemetry_agent_header {
	u_int32_t   start_magic_num;
	u_int32_t   stats_version;
	u_int32_t   stats_type;
	u_int32_t   payload_len;
} __attribute__ ((__packed__));


enum link_band_caps {
	LINK_BAND_2GHz,
	LINK_BAND_5GHz,
	LINK_BAND_5GHz_LOW,
	LINK_BAND_5GHz_HIGH,
	LINK_BAND_6Ghz,
	LINK_BAND_6Ghz_LOW,
	LINK_BAND_6GHz_HIGH,
	LINK_BAND_INVALID,
};

enum t2lm_direction {
	T2LM_DL_DIRECTION,
	T2LM_UL_DIRECTION,
	T2LM_BIDI_DIRECTION,
	T2LM_MAX_VALID_DIRECTION = T2LM_BIDI_DIRECTION,
	T2LM_INVALID_DIRECTION,
};

struct agent_peer_stats_obj {
	uint8_t peer_mld_mac[6];
	uint8_t peer_link_mac[6];
	uint8_t tx_airtime_consumption;
	uint8_t rx_airtime_consumption;
	uint8_t m1_stats;
	uint8_t m2_stats;
	uint8_t rssi;
};

struct agent_link_stats {
	uint8_t hw_link_id;
	enum link_band_caps band;
	uint8_t available_airtime;
	uint32_t m3_stats;
	uint32_t m4_stats;
	int num_peers;
	struct agent_peer_stats_obj peer_stats[MAX_PEERS];
};

struct agent_soc_stats {
	uint8_t soc_id;
	uint8_t num_peers;
	int num_links;
	struct agent_link_stats  link_stats[MAX_PDEV_LINKS];
};

/* Periodic Stats */
struct relayfs_stats {
	int num_soc;
	struct agent_soc_stats soc_stats[MAX_SOCS];
};


/*
 * Init time interface information - complete view.
 */

struct link_map_of_tids {
	enum t2lm_direction direction; /* 0-DL, 1-UL, 2-BiDi */
	uint8_t default_link_mapping;
	uint8_t tid_present[NUM_TIDS]; /* TID present on this link */
};

struct agent_peer_init_stats {
	uint8_t mld_mac_addr[6];      /* peer MLD mac */
	uint8_t link_mac_addr[6];     /* peer MLD link mac */
	struct link_map_of_tids t2lm_info[T2LM_MAX_VALID_DIRECTION]; /* T2LM mapping */
	uint8_t chan_bw;                     /* peer link channel bandwidth */
	uint16_t chan_freq;                  /* channel center frequency */
	uint8_t link_tx_power;               /* tx power of the STA */
	uint8_t psd_flag;                    /* is psd channel */
	uint8_t psd_eirp;                    /* PSD power level */
	uint32_t mcs_nss_map[EHTCAP_TXRX_MCS_NSS_IDX_MAX];    /* mcs-nss map */
};

struct agent_link_init_stats {
	uint8_t hw_link_id;
	enum link_band_caps band;
	int num_peers;
	struct agent_peer_init_stats peer_stats[MAX_PEERS];
};

struct agent_soc_init_stats {
	uint8_t soc_id;
	uint8_t num_peers;
	int num_links;
	struct agent_link_init_stats link_stats[MAX_PDEV_LINKS];
};

struct relayfs_init_stats {
	int num_soc;
	struct agent_soc_init_stats soc_stats[MAX_SOCS];
};


struct telemetry_buffer {
	struct telemetry_agent_header header;
	union {
		struct relayfs_stats periodic_stats;
		struct relayfs_init_stats init_stats;
	} u;

	u_int32_t   end_magic_num;
};
