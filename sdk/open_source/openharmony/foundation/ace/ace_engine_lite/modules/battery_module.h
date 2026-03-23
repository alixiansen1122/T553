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

#ifndef OHOS_ACELITE_BATTERY_MODULE_H
#define OHOS_ACELITE_BATTERY_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "product_adapter.h"
#include "abilityms_slite_client.h"

namespace OHOS {
namespace ACELite {

struct BatteryParams {
    JSIValue args = JSI::CreateUndefined();
    JSIValue thisVal = JSI::CreateUndefined();
};

class BatteryModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BatteryModule);
    BatteryModule() = default;
    ~BatteryModule() = default;
    static const char * const BATTERY_CHARGING_NAME;
    static const char * const BATTERY_LEVEL_NAME;
    static const bool DFAULT_BATTERY_CHARGING;
    static const double DFAULT_BATTERY_LEVEL;
    static JSIValue GetStatus(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

private:
    static JSIValue ExecuteAsyncWork(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc);
    static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue);
    static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret);
    static void ExecuteGetStatus(void *data);

};

void InitBatteryModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "getStatus", BatteryModule::GetStatus);
}

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_MODULE_H
