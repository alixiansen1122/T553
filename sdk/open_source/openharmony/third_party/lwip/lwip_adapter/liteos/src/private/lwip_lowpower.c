/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lwip_public.h"
#include "lwip_private.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"

static int g_low_power_vote[LOW_MAX_MODULE] = {0};

void lwip_lowpower_inc(lowpower_module mod)
{
#if LWIP_TCPIP_CORE_LOCKING
    if (mod >= LOW_MAX_MODULE || mod < 0) {
        return;
    }

    LOCK_TCPIP_CORE();
    g_low_power_vote[mod]++;
    for (uint8_t index = 0; index < LOW_MAX_MODULE; index++) {
        if (g_low_power_vote[index] < 0) { /* some module prevent entering low power mode */
            UNLOCK_TCPIP_CORE();
            return;
        }
    }
    set_lowpower_mod(LOW_TMR_LOWPOWER_MOD);
    UNLOCK_TCPIP_CORE();
#else
    netifapi_enable_lowpower();
#endif
}

void lwip_lowpower_dec(lowpower_module mod)
{
#if LWIP_TCPIP_CORE_LOCKING
    if (mod >= LOW_MAX_MODULE || mod < 0) {
        return;
    }

    LOCK_TCPIP_CORE();
    if (g_low_power_vote[mod] > 0) {
        g_low_power_vote[mod]--;
    }

    if (g_low_power_vote[mod] > 0) {
        UNLOCK_TCPIP_CORE();
        return;
    }
    set_lowpower_mod(LOW_TMR_NORMAL_MOD);
    UNLOCK_TCPIP_CORE();
#else
    netifapi_disable_lowpower();
#endif
}

#endif