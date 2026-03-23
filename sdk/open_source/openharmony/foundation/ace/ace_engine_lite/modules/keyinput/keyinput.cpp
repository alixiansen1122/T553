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

#include "keyinput.h"
#include "js_async_work.h"
#include "adapter.h"
#include "js_ability_impl.h"
#include "ace_log.h"
#include "abilityms_slite_client.h"

typedef struct : public OHOS::HeapBase {
    char *msgBody;
    uint16_t status;
} KeyMsgParams;

typedef struct : public OHOS::HeapBase {
    int16_t rotate;
} RotateMsgParams;

namespace OHOS {
namespace ACELite {
KeyNotifyCallback KeyInputModule::notifyCallback_ = nullptr;
OHOS::ACELite::JSIValue keyCallback = nullptr;
OHOS::ACELite::JSIValue rotateCallback = nullptr;
OHOS::ACELite::JSIValue context = nullptr;
OHOS::ACELite::JSIValue onClickCallback = nullptr;
OHOS::ACELite::JSIValue onPressCallback = nullptr;

static bool CheckIsJsValueDefined(OHOS::ACELite::JSIValue jsValue)
{
    if (jsValue == nullptr) {
        return false;
    }

    if (OHOS::ACELite::JSI::ValueIsUndefined(jsValue)) {
        return false;
    }

    return true;
}

static void ReleaseJsValue(OHOS::ACELite::JSIValue &jsValue)
{
    if (CheckIsJsValueDefined(jsValue)) {
        OHOS::ACELite::JSI::ReleaseValue(jsValue);
        jsValue = OHOS::ACELite::JSI::CreateUndefined();
    }
    return;
}

static int32_t ACECopyStr(char **destStr, const char *srcStr)
{
    if (destStr == nullptr || srcStr == nullptr) {
        return -1;
    }

    uint32_t length = strlen(srcStr) + 1;
    *destStr = static_cast<char *>(AdapterMalloc(length));
    if (*destStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for destination string");
        return -1;
    }

    if (memset_s(*destStr, length, 0, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to set memory for destination string");
        AdapterFree(*destStr);
        return -1;
    }
    if (memcpy_s(*destStr, length, srcStr, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy source string to destination string");
        AdapterFree(*destStr);
        return -1;
    }

    return 0;
}

static void ExecuteSendKeyMsgToJS(void *data)
{
    KeyMsgParams *params = static_cast<KeyMsgParams *>(data);
    if (params == nullptr) {
        return;
    }

    char *msgBody = params->msgBody;
    uint16_t status = params->status;
    OHOS::ACELite::JSIValue dataInfo = OHOS::ACELite::JSI::CreateObject();
    OHOS::ACELite::JSI::SetStringProperty(dataInfo, "keyName", msgBody);

    switch (status) {
        case InputDevice::STATE_PRESS:
            if (onPressCallback != nullptr && context != nullptr) {
                OHOS::ACELite::JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
                OHOS::ACELite::JSI::CallFunction(onPressCallback, context, callbackArgv, OHOS::ACELite::ARGC_ONE);
            }
            break;
        case InputDevice::STATE_RELEASE:
            if (onClickCallback != nullptr && context != nullptr) {
                OHOS::ACELite::JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
                OHOS::ACELite::JSI::CallFunction(onClickCallback, context, callbackArgv, OHOS::ACELite::ARGC_ONE);
            }
            break;
        default:
            break;
    }

    if (keyCallback != nullptr && context != nullptr) {
        OHOS::ACELite::JSI::SetNumberProperty(dataInfo, "status", status);
        OHOS::ACELite::JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
        OHOS::ACELite::JSI::CallFunction(keyCallback, context, callbackArgv, OHOS::ACELite::ARGC_ONE);
    }

    OHOS::ACELite::JSI::ReleaseValue(dataInfo);
    AdapterFree(msgBody);
    delete params;
    params = nullptr;
    return;
}

static void ExecuteSendRotateMsgToJS(void *data)
{
    RotateMsgParams *params = static_cast<RotateMsgParams *>(data);
    if (params == nullptr) {
        return;
    }

    int16_t rotate = params->rotate;
    delete params;
    params = nullptr;
    OHOS::ACELite::JSIValue dataInfo = OHOS::ACELite::JSI::CreateObject();
    OHOS::ACELite::JSI::SetNumberProperty(dataInfo, "rotate", rotate);
    OHOS::ACELite::JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
    OHOS::ACELite::JSI::CallFunction(rotateCallback, context, callbackArgv, OHOS::ACELite::ARGC_ONE);
    OHOS::ACELite::JSI::ReleaseValue(dataInfo);
    return;
}

JSIValue KeyInputModule::SetKeyListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    bool ret = false;
    ReleaseKeyCallback();
    if (argsNum > 0) {
        context = OHOS::ACELite::JSI::AcquireValue(thisVal);
        keyCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
        if ((!JSI::ValueIsUndefined(keyCallback)) && JSI::ValueIsFunction(keyCallback)) {
            ret = true;
        } else {
            HILOG_DEBUG(HILOG_MODULE_ACE, "setKeyListener: unset listener.");
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "setKeyListener failed, argsNum is %d.", argsNum);
    }
    return JSI::CreateBoolean(ret);
}

void KeyInputModule::ReleaseKeyCallback()
{
    ReleaseJsValue(keyCallback);
    keyCallback = nullptr;
    return;
}

JSIValue KeyInputModule::SetRotateListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    bool ret = false;
    ReleaseRotateCallbacks();
    if (argsNum > 0) {
        context = OHOS::ACELite::JSI::AcquireValue(thisVal);
        rotateCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
        if ((!JSI::ValueIsUndefined(rotateCallback)) && JSI::ValueIsFunction(rotateCallback)) {
            RotateListener::GetInstance()->SetRegister(true);
            ret = true;
            // you could enable rotary encoder's driver here.
        } else {
            HILOG_DEBUG(HILOG_MODULE_ACE, "setRotateListener: unset listener.");
            // you could disable rotary encoder's driver here.
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "setRotateListener failed, argsNum is %d.", argsNum);
        // you could disable rotary encoder's driver here.
    }
    return JSI::CreateBoolean(ret);
}

void KeyInputModule::ReleaseRotateCallbacks()
{
    OHOS::ACELite::RotateListener::GetInstance()->SetRegister(false);
    ReleaseJsValue(rotateCallback);
    rotateCallback = nullptr;
    return;
}

void OHOS::ACELite::KeyInputModule::SetNotifyKeyListener(KeyNotifyCallback callback)
{
    notifyCallback_ = callback;
}

KeyNotifyCallback OHOS::ACELite::KeyInputModule::GetNotifyKeyListener(void)
{
    return notifyCallback_;
}

void KeyInputModule::DefineProperty(
    JSIValue target, const char *propName, JSIFunctionHandler getter, JSIFunctionHandler setter)
{
    JSPropertyDescriptor descriptor;
    descriptor.getter = getter;
    descriptor.setter = setter;
    JSI::DefineNamedProperty(target, propName, descriptor);
}

JSIValue KeyInputModule::OnClickSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    bool ret = false;
    if (argsNum > 0) {
        context = OHOS::ACELite::JSI::AcquireValue(thisVal);
        onClickCallback = OHOS::ACELite::JSI::AcquireValue(args[0]);
        if (JSI::ValueIsUndefined(onClickCallback) || !JSI::ValueIsFunction(onClickCallback)) {
            ReleaseOnClickCallbacks();
        } else {
            ret = true;
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnClickSetter failed, argsNum is %d.", argsNum);
    }
    return JSI::CreateBoolean(ret);
}

JSIValue KeyInputModule::OnClickGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return (onClickCallback == nullptr) ? JSI::CreateUndefined() : onClickCallback;
}

void KeyInputModule::ReleaseOnClickCallbacks()
{
    ReleaseJsValue(onClickCallback);
    onClickCallback = nullptr;
    return;
}

JSIValue KeyInputModule::OnPressSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    bool ret = false;
    if (argsNum > 0) {
        context = OHOS::ACELite::JSI::AcquireValue(thisVal);
        onPressCallback = OHOS::ACELite::JSI::AcquireValue(args[0]);
        if (JSI::ValueIsUndefined(onPressCallback) || !JSI::ValueIsFunction(onPressCallback)) {
            ReleaseOnPressCallbacks();
        } else {
            ret = true;
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnPressSetter failed, argsNum is %d.", argsNum);
    }
    return JSI::CreateBoolean(ret);
}

JSIValue KeyInputModule::OnPressGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return (onPressCallback == nullptr) ? JSI::CreateUndefined() : onPressCallback;
}

void KeyInputModule::ReleaseOnPressCallbacks()
{
    ReleaseJsValue(onPressCallback);
    onPressCallback = nullptr;
    return;
}

void KeyInputModule::OnDestroy()
{
    // you could disable rotary encoder's driver here.
    ReleaseKeyCallback();
    ReleaseRotateCallbacks();
    ReleaseOnClickCallbacks();
    ReleaseOnPressCallbacks();
    ReleaseJsValue(context);
    context = nullptr;
}

void InitKeyInputModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "setKeyListener", KeyInputModule::SetKeyListener);
    JSI::SetModuleAPI(exports, "setRotateListener", KeyInputModule::SetRotateListener);

    KeyInputModule::DefineProperty(exports, "onClick", KeyInputModule::OnClickGetter, KeyInputModule::OnClickSetter);
    KeyInputModule::DefineProperty(exports, "onPress", KeyInputModule::OnPressGetter, KeyInputModule::OnPressSetter);
    JSI::SetOnDestroy(exports, KeyInputModule::OnDestroy);
}

}  // namespace ACELite
}  // namespace OHOS

bool OHOS::ACELite::KeyListener::HasRegister()
{
    if (keyCallback != nullptr || onClickCallback != nullptr || onPressCallback != nullptr) {
        return true;
    }
    return false;
}

void OHOS::ACELite::KeyListener::SendToJS(const KeyEvent &event)
{
    if (!OHOS::ACELite::KeyListener::HasRegister()) {
        return;
    }

    char *type = "UNKNOWN";
    switch (event.GetKeyId()) {
        case static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER):
            type = "POWER";
            break;
        case static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC):
            type = "FUNCTION";
            break;
        default:
            break;
    }
    int32_t ret = KeySendMsgToJS(type, event.GetState());
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "KeyListener: OnKeyEvent send event error");
    }
}

void OHOS::ACELite::KeyListener::OnKeyEvent(const KeyEvent &event)
{
    // Prioritize handling notification events
    KeyNotifyCallback notifyCallback = KeyInputModule::GetNotifyKeyListener();
    if (notifyCallback != nullptr) {
        // If the upper layer does not notify, continue executing the JS event
        if (notifyCallback(event.GetKeyId(), event.GetState())) {
            return;
        }
    }
    switch (event.GetState()) {
        case InputDevice::STATE_PRESS:
            isPress_ = true;
            SendToJS(event);
            break;
        case InputDevice::STATE_LONG_PRESS:
            isLongPress_ = true;
            OnKeyLongPress(event);
            break;
        case InputDevice::STATE_RELEASE:
            OnRelease(event);
            break;
        default:
            break;
    }
}

void OHOS::ACELite::KeyListener::OnRelease(const KeyEvent &event)
{
    if (OHOS::ACELite::KeyListener::HasRegister()) {
        SendToJS(event);
    } else if (isPress_ && !isLongPress_) {
        if (event.GetKeyId() == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {
            ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
            if (elementName != nullptr) {
                OHOS::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName);
                FreeElement(elementName);
            } else {
                HILOG_ERROR(HILOG_MODULE_ACE, "KeyListener: OnRelease elementName is nullptr");
            }
        }
    }
    isPress_ = false;
}

void OHOS::ACELite::KeyListener::OnKeyLongPress(const KeyEvent &event)
{
    if (isLongPress_) {
        if (event.GetKeyId() == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {
            NativeAbility::GetInstance().ChangeSlice(VIEW_REBOOT);
        }
    }
    // we don't handle isLongPress & isRelease, so reset isLongPress_ to false here.
    isLongPress_ = false;
}

void OHOS::ACELite::RotateListener::OnRotateEvent(const RotateEvent &event)
{
    if (!OHOS::ACELite::RotateListener::HasRegister()) {
        return;
    }
    int ret = RotateSendMsgToJS(event.GetRotate());
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RotateListener: OnRotateEvent send event error");
    }
}

int32_t KeySendMsgToJS(const char *msgBody, uint16_t status)
{
    KeyMsgParams *params = new KeyMsgParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params");
        return -1;
    }
    params->status = status;
    int32_t copyRet = OHOS::ACELite::ACECopyStr(&params->msgBody, msgBody);

    if (copyRet != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy msgBody");
        AdapterFree(params->msgBody);
        delete params;
        params = nullptr;
        return copyRet;
    }

    bool ret = OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(
        OHOS::ACELite::ExecuteSendKeyMsgToJS, static_cast<void *>(params));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to dispatch async work");
        AdapterFree(params->msgBody);
        delete params;
        params = nullptr;
        return -1;
    }

    return 0;
}

int32_t RotateSendMsgToJS(int16_t rotate)
{
    RotateMsgParams *params = new RotateMsgParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params");
        return -1;
    }
    params->rotate = rotate;
    bool ret = OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(
        OHOS::ACELite::ExecuteSendRotateMsgToJS, static_cast<void *>(params));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to dispatch async work");
        delete params;
        params = nullptr;
        return -1;
    }
    return 0;
}
