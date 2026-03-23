/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef SVR_BROADCAST_SERVICE_H
#define SVR_BROADCAST_SERVICE_H

#include "ohos_types.h"
#include "service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 广播服务名称 */
#ifndef BROADCAST_SERVICE_NAME
#define BROADCAST_SERVICE_NAME "BROADCAST"
#endif

/* 广播服务 */
typedef struct BroadcastService {
    INHERIT_SERVICE;
} BroadcastService;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* SVR_BROADCAST_SERVICE_H */
