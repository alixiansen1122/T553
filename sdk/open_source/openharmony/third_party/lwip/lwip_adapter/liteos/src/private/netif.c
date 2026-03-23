/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: netif
 */
#include "lwip/netif.h"

/**
 * Finds network interface given reference number.
 *
 * @param num  reference to network interface.
 *
 * @return
 * - pointer to network interface
 * - NULL if not found
 */
struct netif *netif_find_num(const uint8_t num)
{
    struct netif *netif;

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        if (num == netif->num) {
            return netif;
        }
    }
    return NULL;
}