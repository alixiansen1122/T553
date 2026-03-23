/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: watch connect js interface
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef WATCH_CONNECT_JS_H
#define WATCH_CONNECT_JS_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void WatchConnectGetVersionResponseToJS(uint32_t result);
void WatchConnectSendPingResponseToJS(uint32_t result);
void WatchConnectSendMsgResponseToJS(uint32_t result);
void WatchConnectSendMsgToJS(const char *msgBody);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif