/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: websocket callback interfaces
 * Author: Software Group
 * Create: 2025-09-1
 */

#ifndef WEBSOCKET_MODULE_CALLBACK_H
#define WEBSOCKET_MODULE_CALLBACK_H

#include "jsi.h"
#include "net_websocket_type.h"
#include "http_errcode.h"
#include "websocket_common.h"

namespace OHOS {
namespace ACELite {

struct WsResult {
    int resCode;
    char* reason;
    char* data;
    int length;
    bool isBinary;
    int clientId;
    WsEventId eventId;
};

class WsCallback final : public MemoryHeap {
public:
    WsCallback() = default;
    ~WsCallback() = default;

    static void DispatchStateInfo(int resCode, const char* reason, WsEventId eventId, int clientId);
    static void DispatchMessage(const char *data, size_t length, bool isBinary, int clientId);

private:
    static void SendStateInfo(void *param);
    static void SendMessage(void *param);

private:
};

}  // namespace ACELite
}  // namespace OHOS

#endif