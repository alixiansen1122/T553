/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: extend functions
 */

#ifndef INCLUDE_LWIP_PUBLIC_H
#define INCLUDE_LWIP_PUBLIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* 4g Cat1 netif */
    LWIP_NETIF_VOLTE = 0,
    /* bluetooth BT PAN netif */
    LWIP_NETIF_BT_PAN = 1,
    /* NULL netif */
    LWIP_NETIF_NONE
} lwip_netif_type;

typedef enum {
    CONNECTION_CONNECTING = 0,
    CONNECTION_CONNECTED = 1,

    CONNECTION_DISCONNECTING = 2,
    CONNECTION_DISCONNECTED = 3,

    CONNECTION_UNKNOWN = 100
} lwip_net_connection_state;

/**
 * @brief Defines the callback function net connection status change.
 *
 * @param netif_type net if type, refer to #lwip_netif_type.
 * @param net_connect_state net connection status, refer to #lwip_net_connection_state.
 */
typedef void (*net_connection_callback)(const uint8_t netif_type, const uint8_t net_connect_state);

/**
 * @brief This function register network connection status listener.
 * @param[in] conn_listener             net_connection_callback.
 * @return 0 if success; non-0 otherwise.
 */
int lwip_register_connect_listener(net_connection_callback conn_listener);

/**
 * @brief Defines the callback unregister function for listening net connection status change.
 */
void lwip_unregister_connect_listener(net_connection_callback conn_listener);

/**
 * @brief This function get current network connection status.
 * @param[in] netif_type             netif type, refer to #lwip_netif_type.
 * @param[in, out] net_connect_state      network connection statue, refer to #lwip_net_connection_state.
 * @return 0 if success; non-0 otherwise.
 */
int lwip_get_connect_status(uint8_t netif_type, uint8_t *net_connect_state);

struct netif *netif_find_num(const uint8_t num);

/**
 * @brief init and open 4G cat1 net interface.
 */
void lwip_cat1_open(void);

/**
 * @brief close 4G cat1 net interface.
 */
void lwip_cat1_close(void);

/**
 * @brief set cat1 netif ip addr, and wait.
 * Attention: this function will block the caller, make sure no task after that.
 */
void lwip_cat1_set_ip(void);

/**
 * @brief switch netif to 4G cat1.
 */
void lwip_netif_switch_volte(void);

/**
 * @brief switch netif to bluetooth pan.
 */
void lwip_netif_switch_bt_pan(void);

/**
 * @brief init and open bt pan net interface.
 */
void lwip_open_bt_pan(void);

/**
 * @brief close bt pan net interface.
 */
void lwip_close_bt_pan(void);

#ifdef __cplusplus
}
#endif

#endif