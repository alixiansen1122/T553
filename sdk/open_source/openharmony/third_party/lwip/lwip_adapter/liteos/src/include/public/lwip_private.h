/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: private defines
 */

#ifndef LWIP_PRIVATE_H_
#define LWIP_PRIVATE_H_

#include <string.h>
#include "arch/sys_arch.h"
#include "lwip/netifapi.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include "lwip_public.h"

typedef struct lwip_status_change_callback {
    /** Pointer to next function in the list */
    struct lwip_status_change_callback *next;

    /** Function to call back */
    void (*callback)(void*);

    /** Context to pass back in callback function */
    void* callback_context;
} lwip_status_change_callback_s;

typedef struct netif_state {
    /** List of user callbacks for this netif */
    lwip_status_change_callback_s *status_change_callbacks;

    /** List of user callbacks for this netif */
    lwip_status_change_callback_s *status_link_callbacks;

#if LWIP_NETIF_HOSTNAME
    /** Hostname, or NULL if not set */
    char *hostname;
#endif

    /** Whether to start the stack and bring the interface up when created */
    uint8_t start_stack_init;

    uint8_t type;
} netif_state;

void bt_pan_open(void);

void bt_pan_close(void);

void bt_pan_register_net_callback(net_connection_callback callback);

void bt_pan_unregister_net_callback(void);

void cat1_register_net_callback(net_connection_callback callback);

void cat1_unregister_net_callback(void);

void cat1_open_inner(void);

void cat1_close_inner(void);

uint8_t cat1_get_net_state(void);

uint8_t pan_get_net_state(void);

#define LWIP_ADAPTER_DEBUG
#ifdef LWIP_ADAPTER_DEBUG
#define LWIP_DEBUG_LOG(message) LWIP_PLATFORM_DIAG(message)
#else
#define LWIP_DEBUG_LOG(message)
#endif

#endif /* LWIP_PRIVATE_H_ */
