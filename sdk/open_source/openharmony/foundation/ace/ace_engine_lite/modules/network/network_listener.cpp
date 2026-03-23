/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: network wrapper
 * Author: SoftwarePlatform Group
 * Create: 2025-11-14
 */

#include "connection_manage.h"
#include "network_listener.h"

void net_connect_status_change(uint8_t type, uint8_t netConnectState)
{
    NetConnectChange(type, netConnectState);
}