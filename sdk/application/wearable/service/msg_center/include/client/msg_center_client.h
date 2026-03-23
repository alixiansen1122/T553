/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: msg center service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_CLIENT_H
#define MSG_CENTER_CLIENT_H

#include "iunknown.h"
#include "samgr_lite.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MSG_CENTER_SERVICE_NAME "MsgCenter"

typedef struct {
    INHERIT_IUNKNOWN;
    int32_t (*AsyncCall)(int32_t msgId, uint32_t msgValue, void *msgAllocBuf, int32_t dataLen);
} MsgCenterServiceAPI;

typedef struct MsgCenterSvcEntry {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(MsgCenterServiceAPI);
    Identity identity;
} MsgCenterSvcEntry;

int32_t MsgCenterSendMsg(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MSG_CENTER_CLIENT_H */
