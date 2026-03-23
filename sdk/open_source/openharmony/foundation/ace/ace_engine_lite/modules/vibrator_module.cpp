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

#include <new>
#include "ace_log.h"
#include "js_app_context.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include <securec.h>
#include "vibrator_module.h"
#include "global.h"
#include "screen.h"

namespace OHOS {
namespace ACELite {

const char *const VibratorModule::VIBTATOR_MODE = "mode";

JSIValue VibratorModule::Vibrate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteVibrate);
}

JSIValue VibratorModule::ExecuteAsyncWork(
    const JSIValue thisVal, const JSIValue *args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Error in obtaining device information parameters");
        return undefValue;
    }
    VibratorParams *params = new (std::nothrow) VibratorParams();
    if (params == nullptr) {
        return undefValue;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    params->args = JSI::AcquireValue(args[0]);
    JsAsyncWork::DispatchAsyncWork(ExecuteFunc, reinterpret_cast<void *>(params));
    return undefValue;
}

void VibratorModule::ExecuteVibrate(void *data)
{
    VibratorParams *params = reinterpret_cast<VibratorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    JSIValue result = JSI::CreateObject();
    JSI::SetStringProperty(result, "result", "success");
    char *mode = JSI::GetStringProperty(args, VIBTATOR_MODE);
    if (strcmp(mode, "long") == 0 || strcmp(mode, "short") == 0) {
        SuccessCallBack(thisVal, args, result);
    } else {
        FailCallBack(thisVal, args, -1);
    }
    JSI::ReleaseString(mode);
    JSI::ReleaseValueList(args, thisVal, result, ARGS_END);
    delete params;
}

void VibratorModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue success = JSI::GetNamedProperty(args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(jsiValue)) {
            JSI::CallFunction(success, thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, thisVal, &jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}

void VibratorModule::FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue errInfo;
    errInfo = JSI::CreateString("Vibrator faild");
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue fail = JSI::GetNamedProperty(args, CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(fail)) {
        JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(fail, complete, errInfo, errCode, ARGS_END);
}

}  // namespace ACELite
}  // namespace OHOS
