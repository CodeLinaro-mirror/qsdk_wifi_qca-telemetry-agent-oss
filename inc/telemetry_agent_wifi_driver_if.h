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

struct agent_config_params {
    int command;
    int value;
};

struct agent_psoc_obj {
    void *psoc_back_pointer;
    spinlock_t psoc_lock;
    uint8_t psoc_id;
};

struct agent_pdev_obj {
    void *pdev_back_pointer;
    void *psoc_back_pointer;
    spinlock_t pdev_lock;
    uint8_t psoc_id;
    uint8_t pdev_id;
};

struct agent_peer_obj {
    void *peer_back_pointer;
    void *pdev_back_pointer;
    void *psoc_back_pointer;
    spinlock_t peer_lock;
    uint8_t psoc_id;
    uint8_t pdev_id;
    uint8_t peer_mac_addr[6];
};

struct telemetry_agent_ops {
    int  (*agent_psoc_create_handler) (void *arg, struct agent_psoc_obj *psoc_obj);
    int  (*agent_psoc_destroy_handler) (void *arg, struct agent_psoc_obj *psoc_obj);
    int  (*agent_pdev_create_handler) (void *arg, struct agent_pdev_obj *pdev_obj);
    int  (*agent_pdev_destroy_handler) (void *arg, struct agent_pdev_obj *pdev_obj);
    int  (*agent_peer_create_handler) (void *arg, struct agent_peer_obj *peer_obj);
    int  (*agent_peer_destroy_handler) (void *arg, struct agent_peer_obj *peer_obj);
    int  (*agent_set_param) (struct agent_config_params *params);
    int  (*agent_get_param) (struct agent_config_params *params);
	void (*agent_notify_app_init) (void);
};

