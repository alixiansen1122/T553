/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lwip_public.h"
#include "lwip_private.h"

#if defined(SUPPORT_VOLTE)
static lwip_netif_type g_netif_state = LWIP_NETIF_VOLTE;
#elif defined(SUPPORT_BREDR)
static lwip_netif_type g_netif_state = LWIP_NETIF_BT_PAN;
#else
static lwip_netif_type g_netif_state = LWIP_NETIF_NONE;
#endif

typedef struct net_status_callback_t {
    net_connection_callback callback_fn;
    struct net_status_callback_t *next;
} net_status_callback_t;

static net_status_callback_t g_head_callback = {0};
static uint8_t g_net_is_register = 0;

static void net_status_litener(const uint8_t netif_type, const uint8_t net_connect_state)
{}

void lwip_netif_switch_volte(void)
{
#if defined(SUPPORT_VOLTE) && defined(SUPPORT_BREDR)
    if (g_netif_state == LWIP_NETIF_VOLTE) {
        return;
    }
    if (g_net_is_register == 0) {
        lwip_register_connect_listener(net_status_litener);
        g_net_is_register = 1;
    }
    bt_pan_close();
    cat1_open_inner();
    g_netif_state = LWIP_NETIF_VOLTE;
#endif
}

void lwip_netif_switch_bt_pan(void)
{
#if defined(SUPPORT_VOLTE) && defined(SUPPORT_BREDR)
    if (g_netif_state == LWIP_NETIF_BT_PAN) {
        return;
    }
    if (g_net_is_register == 0) {
        lwip_register_connect_listener(net_status_litener);
        g_net_is_register = 1;
    }
    cat1_close_inner();
    bt_pan_open();
    g_netif_state = LWIP_NETIF_BT_PAN;
#endif
}

void lwip_open_bt_pan(void)
{
#if defined(SUPPORT_BREDR)
    if (g_netif_state != LWIP_NETIF_BT_PAN) {
        return;
    }
    if (g_net_is_register == 0) {
        lwip_register_connect_listener(net_status_litener);
        g_net_is_register = 1;
    }
    bt_pan_open();
#endif
}

void lwip_close_bt_pan(void)
{
#if defined(SUPPORT_BREDR)
    if (g_netif_state != LWIP_NETIF_BT_PAN) {
        return;
    }
    if (g_net_is_register) {
        lwip_unregister_connect_listener(net_status_litener);
        g_net_is_register = 0;
    }
    bt_pan_close();
#endif
}

void lwip_cat1_open(void)
{
#if defined(SUPPORT_VOLTE)
    if (g_netif_state != LWIP_NETIF_VOLTE) {
        return;
    }
    if (g_net_is_register == 0) {
        lwip_register_connect_listener(net_status_litener);
        g_net_is_register = 1;
    }
    cat1_open_inner();
#endif
}

void lwip_cat1_close(void)
{
#if defined(SUPPORT_VOLTE)
    if (g_netif_state != LWIP_NETIF_VOLTE) {
        return;
    }
    if (g_net_is_register) {
        lwip_unregister_connect_listener(net_status_litener);
        g_net_is_register = 0;
    }
    cat1_close_inner();
#endif
}

int lwip_get_connect_status(uint8_t netif_type, uint8_t *net_connect_state)
{
    if (g_netif_state != netif_type) {
        *net_connect_state = CONNECTION_DISCONNECTED;
        return 0;
    }
#if defined(SUPPORT_VOLTE)
    if (netif_type == LWIP_NETIF_VOLTE) {
        *net_connect_state = cat1_get_net_state();
        return 0;
    }
#endif
#if defined(SUPPORT_BREDR)
    if (netif_type == LWIP_NETIF_BT_PAN) {
        *net_connect_state = pan_get_net_state();
        return 0;
    }
#endif
    return -1;
}

static void lwip_net_conn_callback(const uint8_t netif_type, const uint8_t net_connect_state)
{
    net_status_callback_t *iter_node = &g_head_callback;
    while (iter_node != NULL) {
        if (iter_node->callback_fn != NULL) {
            iter_node->callback_fn(netif_type, net_connect_state);
        }
        iter_node = iter_node->next;
    }
}

int lwip_register_connect_listener(net_connection_callback callback)
{
    net_status_callback_t *iter_node = &g_head_callback;
    if (callback == NULL) {
        return -1;
    }
#if defined(SUPPORT_VOLTE)
    cat1_register_net_callback(lwip_net_conn_callback);
#endif
#if defined(SUPPORT_BREDR)
    bt_pan_register_net_callback(lwip_net_conn_callback);
#endif
    if (g_head_callback.callback_fn == NULL) {
        g_head_callback.callback_fn = callback;
        return 0;
    }
    net_status_callback_t *ext_call_node = (net_status_callback_t *)malloc(sizeof(net_status_callback_t));
    if (ext_call_node == NULL) {
        return -1;
    }
    (void)memset_s(ext_call_node, sizeof(net_status_callback_t), 0, sizeof(net_status_callback_t));
    ext_call_node->callback_fn = callback;

    while (iter_node != NULL) {
        if (iter_node->callback_fn == callback) {
            free(ext_call_node);
            return 0;
        }
        if (iter_node->next == NULL) {
            iter_node->next = ext_call_node;
            return 0;
        }
        iter_node = iter_node->next;
    }
    return 0;
}

void lwip_unregister_connect_listener(net_connection_callback callback)
{
    net_status_callback_t *iter_node = &g_head_callback;
    net_status_callback_t *last = iter_node;
    if (callback == NULL) {
        return;
    }

    if (iter_node->callback_fn == callback) {
        if (iter_node->next != NULL) {
            g_head_callback.callback_fn = iter_node->next->callback_fn;
            g_head_callback.next = iter_node->next->next;
            free(iter_node->next);
            return;
        } else {
            iter_node->callback_fn = NULL;
            goto END;
        }
    }
    while (iter_node != NULL) {
        if (iter_node->callback_fn == callback) {
            last->next = iter_node->next;
            free(iter_node);
            return;
        }
        last = iter_node;
        iter_node = iter_node->next;
    }
    return;
END:
#if defined(SUPPORT_VOLTE)
    cat1_unregister_net_callback();
#endif
#if defined(SUPPORT_BREDR)
    bt_pan_unregister_net_callback();
#endif
}