/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ACELITE_NFC_MODULE_H
#define OHOS_ACELITE_NFC_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "product_adapter.h"
#include "abilityms_slite_client.h"

namespace OHOS {
namespace ACELite {

struct  NfcCallBack {
    JSIValue onCallback;
    JSIValue jsCtx;
};

struct HceParam {
    uint8_t aid[300];
    void *data;
    int32 len;
    HceParam() : data(nullptr), len(0)
    {
        memset_s(aid, 300, 0, 300);
    }
};

typedef struct {
    uint8_t data[300];
    uint32_t dataLen;
    uint32_t index;
} HceStackOnData;

class NfcHceModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(NfcHceModule);
    NfcHceModule() = default;
    ~NfcHceModule() = default;
    static const char * const HCE_CMD;
    static const uint8_t AID_DYNAMIC_NUM;
    static const char * const TYPE;
    static NfcCallBack callback_;
    static JSIValue CreateHceService(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue StartHCE(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue StopHCE(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SendResponse(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void HceApduOnCb(void *data,  int8_t statusCode);
private:
};

void NfcHceInitModule(JSIValue exports)
{

    JSIValue serviceObj = JSI::CreateObject();
    JSIValue constructor = JSI::CreateFunction(NfcHceModule::CreateHceService);
    JSIValue start = JSI::CreateFunction(NfcHceModule::StartHCE);
    JSIValue stop = JSI::CreateFunction(NfcHceModule::StopHCE);
    JSIValue on = JSI::CreateFunction(NfcHceModule::On);
    JSIValue sendResponse = JSI::CreateFunction(NfcHceModule::SendResponse);

    JSI::SetNamedProperty(serviceObj, "startHCE", start);
    JSI::SetNamedProperty(serviceObj, "stopHCE", stop);
    JSI::SetNamedProperty(serviceObj, "on", on);
    JSI::SetNamedProperty(serviceObj, "sendResponse", sendResponse);
    JSI::SetNamedProperty(constructor, "prototype", serviceObj);
    JSI::SetNamedProperty(exports, "HceService", constructor);
    // JSI::ReleaseValueList(constructor, start, stop, on, sendResponse, serviceObj, ARGS_END);

}

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_MODULE_H
