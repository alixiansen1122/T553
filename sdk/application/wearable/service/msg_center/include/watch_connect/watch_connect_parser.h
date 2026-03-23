/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: watch connect msg
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef WATCH_CONNECT_PARSER_H
#define WATCH_CONNECT_PARSER_H

#include <stdint.h>
#include "errcode.h"
#include "watch_connect_msg.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
WatchConnectTlvMsgInfo *WatchConnectTlvDecode(uint8_t *userData, uint16_t dataLen);
void WatchConnectTlvEncode(WatchConnectTlvMsgInfo *msgInfo, uint8_t **userData, uint16_t *dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif