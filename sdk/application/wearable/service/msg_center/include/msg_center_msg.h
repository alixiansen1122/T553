/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_MSG_H
#define MSG_CENTER_MSG_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_DISCONNECTED_TO_MSGCENTER,
    BT_CONNECTED_TO_MSGCENTER,
    MSGCENTER_ADD_SLEEP_VETO,
    MSGCENTER_RM_SLEEP_VETO,
    CALENDAR_TO_MSGCENTER,
} MsgCenterServiceMsgId;

#ifdef __cplusplus
}
#endif
#endif