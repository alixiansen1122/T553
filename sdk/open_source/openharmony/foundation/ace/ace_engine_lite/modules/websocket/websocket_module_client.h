/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: websocket client interfaces
 * Author: Software Group
 * Create: 2025-10-20
 */

#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "jsi.h"
#include "net_websocket.h"
#include "net_websocket_type.h"
#include "websocket_common.h"

namespace OHOS {
namespace ACELite {

enum WsDataType {
    DATA_TYPE_STRING = 0,
    DATA_TYPE_OBJECT = 1,
    DATA_TYPE_ARRAY_BUFFER = 2
};

struct WsHandler {
    JSIValue send;
    JSIValue sendInit;
    JSIValue sendUpdate;
    JSIValue sendFinish;
    JSIValue connect;
    JSIValue close;
    JSIValue destroy;
    JSIValue on;
    JSIValue off;
};

class WsModuleClient final : public MemoryHeap {
public:
    WsModuleClient();
    ~WsModuleClient();

    int GetClientId(void) const
    {
        return clientId_;
    }

    int Connect(const char *url, const char *option);
    int Send(const char *data, size_t length, WsDataType dataType);
    int SendInit(uint32_t size);
    int SendUpdate(const char *data, size_t length);
    int SendFinish(const char *data, size_t length, WsDataType dataType);
    int Close(void);
    static int GetErrorCode(int8_t errorCode);

    WsHandler handler_;
    JSIValue context_;
    JSIValue WsEvents_[WS_EVENT_MAX];
private:
    void ReleaseJSIValue(JSIValue value)
    {
        if (!JSI::ValueIsUndefined(value)) {
            JSI::ReleaseValue(value);
        }
    }

    int SendBinary(const char *data, size_t bufferSize);

    static const uint32_t WS_MAX_DATA_LENGTH  = 20 * 1024;
    static const uint32_t WS_MAX_SEND_LENGTH  = 100 * 1024;
    int clientId_ = -1;
    char *msgBuffer_ = nullptr;
    uint32_t bufferSize_ = 0;
    uint32_t dataSize_ = 0;

};

}  // namespace ACELite
}  // namespace OHOS

#endif  // WEBSOCKET_CLIENT_H