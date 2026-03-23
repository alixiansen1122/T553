/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: watch connect cmd
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef WATCH_CONNECT_CMD_H
#define WATCH_CONNECT_CMD_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum watch_connect_type_id {
    WATCH_CONNECT_TYPE_ID_PING = 0x1,
    WATCH_CONNECT_TYPE_ID_SEND,
    WATCH_CONNECT_TYPE_ID_GET_VERSION,
    WATCH_CONNECT_TYPE_ID_CHECK_INSTALLED,
    WATCH_CONNECT_TYPE_ID_FILE_TRANSFER,
    WATCH_CONNECT_TYPE_ID_SPACE_QUERY,
} watch_connect_type_id;


errcode_t WatchConnectCmdPing(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);
errcode_t WatchConnectCmdSendMsg(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);
errcode_t WatchConnectCmdGetVersion(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);
errcode_t WatchConnectCmdCheckInstalled(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);
errcode_t WatchConnectCmdFileTransfer(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);
errcode_t WatchConnectCmdSpaceQuery(uint8_t cmdId, uint8_t type, void *userData, uint16_t dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif