/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: lwip hooks
 */

#ifndef SRC_LIB_LWIP_PLATFORM_INCLUDE_SYS_HOOKS_H_
#define SRC_LIB_LWIP_PLATFORM_INCLUDE_SYS_HOOKS_H_

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

/* Implementation for the IP4_ROUTE_SRC macro. */
struct netif *sys_hooks_ip4_route_src(const ip4_addr_t *dest, const ip4_addr_t *src);

#endif /* SRC_LIB_LWIP_PLATFORM_INCLUDE_SYS_HOOKS_H_ */
