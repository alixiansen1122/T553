/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef __NET_WEBSOCKET_DEBUG_H__
#define __NET_WEBSOCKET_DEBUG_H__

#include <stdio.h>

#define WS_DBG_LEVEL WEBSOCKET_DBG_OFF

#define WS_INFO_LEVEL WEBSOCKET_DBG_ON

#define WS_ERR_LEVEL WEBSOCKET_DBG_ON

/** flag for WEBSOCKET_DEBUGF to enable that debug message */
#define WEBSOCKET_DBG_ON 0x80U
/** flag for WEBSOCKET_DEBUGF to disable that debug message */
#define WEBSOCKET_DBG_OFF 0x00U

#define WEBSOCKET_DEBUG

#ifdef WEBSOCKET_DEBUG
#define WS_PLATFORM_DIAG(x) \
    do {                    \
        printf x;           \
    } while (0)
#define WEBSOCKET_DEBUGF(debug, message)  \
    do {                                  \
        if (((debug)&WEBSOCKET_DBG_ON)) { \
            WS_PLATFORM_DIAG(message);    \
        }                                 \
    } while (0)

#else /* WEBSOCKET_DEBUG */
#define WEBSOCKET_DEBUGF(debug, message)
#endif /* WEBSOCKET_DEBUG */

#endif