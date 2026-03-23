/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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


#include "display.h"
#include "ace_log.h"
#include "errcode.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif

constexpr char KEEPSCREENON[] = "keepScreenOn";
constexpr char MODE[] = "mode";
constexpr char VALUE[] = "value";
uint32_t g_keepOnTimeout = 5000; // 5000ms
brightness_mode_t g_brightness_mode =  static_cast<brightness_mode_t>(50);
uint8_t g_brightness_value = 50;
const char * ERR_GENERAL = "display error";
const char * ERR_NOT_SUPPORT = "display set is not support";
const int DISPLAY_SUCCESS = 0; 
const int DISPLAY_FAIL = -1;
const int DISPLAY_NOT_SUPPORT = -2;

namespace OHOS {
namespace ACELite {
static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
{
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue errInfo;
    if (ret == DISPLAY_FAIL) {
        errInfo = JSI::CreateString(ERR_GENERAL);
    } else {
        errInfo = JSI::CreateString(ERR_NOT_SUPPORT);
    }
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

static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
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

JSIValue DisplayModule::SetKeepScreenOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result;
    bool keepScreenOn = JSI::GetBooleanProperty(args[0], KEEPSCREENON);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (keepScreenOn) {
        screen_context_t * curScreenCtx = power_display_get_screen_context();
        g_keepOnTimeout = curScreenCtx->keep_on_timeout;
        if (display_api->get_screen_state() != SCREEN_ON) {
            display_api->turn_on_screen();
        }
        result = display_api->set_screen_set_keepon_timeout(0);
        result = display_api->set_screen_set_keepon_timeout(3600000); // 3600000ms
    }  else {
        result = display_api->set_screen_set_keepon_timeout(0);
        result = display_api->set_screen_set_keepon_timeout(g_keepOnTimeout);
    }
    if (result == ERRCODE_SUCC) {
        ret = DISPLAY_SUCCESS;
    }
    else {
        ret = DISPLAY_FAIL;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::SetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result;
    brightness_mode_t mode = static_cast<brightness_mode_t>(JSI::GetNumberProperty(args[0], MODE));
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    screen_context_t * curScreenCtx = power_display_get_screen_context();
    g_brightness_mode = curScreenCtx->brightness_mode;
    if (mode <= AUTO) {
        if (display_api->get_screen_state() != SCREEN_ON) {
            display_api->turn_on_screen();
        }
        result = display_api->set_brightness_mode(mode);
    }  else {
        result = display_api->set_brightness_mode(g_brightness_mode);
    }
    if (result == ERRCODE_SUCC) {
        ret = DISPLAY_SUCCESS;
    }
    else {
        ret = DISPLAY_FAIL;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::GetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    JSIValue result = JSI::CreateObject();
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    brightness_mode_t mode = display_api->get_brightness_mode();
    JSI::SetNumberProperty(result, MODE, static_cast<double>(mode));
#endif
    if (mode >= 0) {
        SuccessCallBack(thisVal, args[0], result);
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    JSI::ReleaseValue(result);
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::SetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result;
    uint8_t brightness_value = static_cast<uint8_t>(JSI::GetNumberProperty(args[0], VALUE));
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    screen_context_t * curScreenCtx = power_display_get_screen_context();
    uint8_t g_brightness_value = curScreenCtx->brightness_value;
    if (brightness_value >= 0) {
        if (display_api->get_screen_state() != SCREEN_ON) {
            display_api->turn_on_screen();
        }
        result = display_api->set_brightness(brightness_value);
    }  else {
        result = display_api->set_brightness(g_brightness_mode);
    }
    if (result == ERRCODE_SUCC) {
        ret = DISPLAY_SUCCESS;
    }
    else {
        ret = DISPLAY_FAIL;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::GetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    JSIValue result = JSI::CreateObject();
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    uint8_t brightness_value = display_api->get_brightness();
    JSI::SetNumberProperty(result, VALUE, static_cast<double>(brightness_value));
#endif
    if (brightness_value >= 0) {
        SuccessCallBack(thisVal, args[0], result);
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    JSI::ReleaseValue(result);
    return JSI::CreateUndefined();
}

void InitBrightnessModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "setKeepScreenOn", DisplayModule::SetKeepScreenOn);
    JSI::SetModuleAPI(exports, "setMode", DisplayModule::SetMode);
    JSI::SetModuleAPI(exports, "getMode", DisplayModule::GetMode);
    JSI::SetModuleAPI(exports, "setValue", DisplayModule::SetValue);
    JSI::SetModuleAPI(exports, "getValue", DisplayModule::GetValue);
}
} // namespace ACELite
} // namespace OHOS