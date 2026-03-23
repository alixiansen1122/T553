/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_CUSTOMER_H
#define MSG_CENTER_CUSTOMER_H

#include <cstdint>
#include "service.h"
#include "iunknown.h"
#include "broadcast_feature.h"
#include "graphic_service.h"
#include "jsi/jsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JS_ENABLE
void StartJsApp(char *bundleName);
void StopJsApp(char *bundleName);
void MsgCenterReportBtStatusToJS(uint16_t msgId);
bool MsgCenterNotifyProc(const Topic topic, Request* req);
#endif

#ifdef __cplusplus
}
#endif
#endif // MSG_CENTER_CUSTOMER_H
