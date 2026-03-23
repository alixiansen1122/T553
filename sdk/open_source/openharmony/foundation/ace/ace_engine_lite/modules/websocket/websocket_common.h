/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: websocket common definition
 * Author: Software Group
 * Create: 2025-11-19
 */

#ifndef WEBSOCKET_COMMON_H
#define WEBSOCKET_COMMON_H


namespace OHOS {
namespace ACELite {

#define net_malloc ace_malloc
#define net_free ace_free

enum WsEventId {
    WS_EVENT_OPEN = 0,
    WS_EVENT_MESSAGE = 1,
    WS_EVENT_CLOSE = 2,
    WS_EVENT_ERROR = 3,
    WS_EVENT_MAX = 4
};

}  // namespace ACELite
}  // namespace OHOS

#endif