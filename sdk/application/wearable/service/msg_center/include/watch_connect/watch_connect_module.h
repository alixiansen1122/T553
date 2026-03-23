/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: native api watch connect info header file for js
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef WATCH_CONNECT_MODULE_H
#define WATCH_CONNECT_MODULE_H
#include "jsi/jsi.h"
#include "jsi/jsi_types.h"
#include "js_ability.h"
#include "js_async_work.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {
namespace ACELite {
typedef struct : public OHOS::HeapBase {
    uint32_t result;
} WatchConnectAsyncParams;

typedef struct : public OHOS::HeapBase {
    char *msgBody;
} WatchConnectAsyncMsgParams;

class WatchConnectModule {
public:
    WatchConnectModule(const WatchConnectModule&) = delete;
    WatchConnectModule& operator=(const WatchConnectModule&) = delete;
    ~WatchConnectModule() {}

    static JSIValue GetLocalVersion(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetPeerVersion(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue SetPeerInfo(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue Ping(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue SendMsg(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue RegisterReceiver(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnregisterReceiver(const JSIValue thisval, const JSIValue *args, uint8_t argsNum);

    // JS清理接口
    static void OnDestroy();
    static void OnTerminate();

    // 回调和属性

    static JSIValue jsGetPeerVersionContext;
    static JSIValue jsGetPeerVersionCbk;

    static JSIValue jsPingCbkContext;
    static JSIValue jsPingSuccessCbk;
    static JSIValue jsPingFailCbk;
    static JSIValue jsPingOnPingResultCbk;

    static JSIValue jsSendMsgCbkContext;
    static JSIValue jsSendMsgSuccessCbk;
    static JSIValue jsSendMsgFailCbk;
    static JSIValue jsSendMsgOnSendResultCbk;
    static JSIValue jsSendMsgOnSendProgressCbk;

    static JSIValue jsReceiverCbkContext;
    static JSIValue jsReceiverSuccessCbk;
    static JSIValue jsReceiverFailCbk;
    static JSIValue jsReceiverOnReceiveMessageCbk;
    static JSIValue jsReceiverProcessCbk;

private:
    static void SyncFailCallback(JSIValue &funCb, const JSIValue &context, const char * const data,
        uint16_t code);
    static int32_t SendMsgProc(const JSIValue &args);
    static void Release(void);
    static void ReleaseGetPeerVersionJsValue(void);
    static void ReleasePingJsValue(void);
    static void ReleaseSendMsgJsValue(void);
    static void ReleaseRegisterReceiverJsValue();
    static void ReleaseJsValue(JSIValue &jsValue);
};
void InitWatchConnectModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS
#endif /* WATCH_CONNECT_MODULE_H */
