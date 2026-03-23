/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: watchface JS intarface
 * Author: Software Group
 * Create: 2025-09-5
 */
 
#ifndef WATCHFACE_MODULE_H
#define WATCHFACE_MODULE_H
 
#include <stdint.h>
#include <map>
#include "jsi.h"
#include "WatchInterface.h"
 
namespace OHOS {
namespace ACELite {

typedef enum {
    WATCHFACE_SUCCESS          =  0,
    WATCHFACE_PARAM_ERROR      =  401,
    WATCHFACE_INSTALL_ERROR    =  2400001,
    WATCHFACE_GETINFO_ERROR    =  2400002,
    WATCHFACE_GETMSG_ERROR     =  2400003,
} WATCHFACE_ERRCODE;

typedef watch_dial_info WatchDialInfo;

struct DownloadStatus {
    uint32_t receivedSize;
    uint32_t totalSize;
    uint32_t uuid;
};

struct WatchfaceParams{
    JSIValue thisVal;
    JSIValue callback;
    double process;
    uint8_t installState;
};

class WatchfaceModule final : public MemoryHeap {
public:
    WatchfaceModule() = default;
    ~WatchfaceModule() = default;
    static JSIValue SetWatchFace(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetWatchFaceInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SetDownloadListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void FailCallback(void *data);
    static void SuccessCallback(void *data);
    static bool isTerminate_;
private:
    static bool GetDialInfo(uint32_t uuid, WatchDialInfo& watch, JSIValue& dialInfo);
    static bool isListenerRunning_;
};
void InitWatchFaceModule(JSIValue exports);
 
}  // namespace ACELite
}  // namespace OHOS
 
#endif  // WATCHFACE_MODULE_H