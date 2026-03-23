/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Msg of UIservers
 * Author: CompanyName
 * Create: 2021-8-8
 */

#ifndef UI_SERVERS_MSG_H
#define UI_SERVERS_MSG_H

#include <cstdint>
#include "ohos_types.h"
#include "message.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    HEART_RATE_PER_SEC,
    HEART_RATE_AVE,
    HEART_RATE_REST,
    HEART_RATE_MAX,
    HEART_RATE_MIN,
    HEALTH_SLEEP,
    BATTERY_LIFE,
} DataDistribute;

typedef struct {
    uint16_t type;
    uint32 value;
} HealthMsg;

int PushOHOSMsgToHealthModel(Request *request);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
