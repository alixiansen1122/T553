/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: http control interfaces
 * Author: Software Group
 * Create: 2025-06-06
 */

#ifndef WEBSOCKET_CLIENT_MODULE_H
#define WEBSOCKET_CLIENT_MODULE_H

#include <stdint.h>
#include "jsi.h"
#include <map>
#include <cstring>
#include "websocket_common.h"
#include "websocket_module_callback.h"

namespace OHOS {
namespace ACELite {

struct EventCallback {
    JSIValue callback;
    JSIValue context;
};

class WebSocketModule final : public MemoryHeap {
public:
    WebSocketModule() = default;
    ~WebSocketModule() = default;
    static JSIValue CreateWebSocket(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Send(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SendFinish(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Close(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static std::map<std::string, EventCallback *> eventCallbacks;
    static void OnTerminate();
private:
    static WsEventId GetEventId(const char *event);
    static char *HeaderJsonToString(char* headerJson);
};
void InitWebSocketClientModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS

#endif  // WEBSOCKET_CLIENT_MODULE_H