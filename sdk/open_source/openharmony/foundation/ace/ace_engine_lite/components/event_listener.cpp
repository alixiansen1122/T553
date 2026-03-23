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
#include "event_listener.h"
#include "ace_log.h"
#include "jerryscript.h"
#include "js_fwk_common.h"
#include "root_view.h"

namespace OHOS {
namespace ACELite {
#ifdef JS_TOUCH_EVENT_SUPPORT
KeyBoardEventListener::KeyBoardEventListener(jerry_value_t fn, const uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

KeyBoardEventListener::~KeyBoardEventListener()
{
    jerry_release_value(fn_);
}

bool KeyBoardEventListener::OnKeyAct(UIView &view, const KeyEvent &event)
{
    if (jerry_value_is_undefined(fn_)) {
        return false;
    }

    jerry_value_t *args = ConvertKeyEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

ViewOnTouchStartListener::ViewOnTouchStartListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchStartListener::~ViewOnTouchStartListener()
{
    jerry_release_value(fn_);
}

bool ViewOnTouchStartListener::OnPress(UIView &view, const PressEvent &event)
{
    return CallBaseEvent(fn_, event, id_, view);
}

ViewOnTouchMoveListener::ViewOnTouchMoveListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchMoveListener::~ViewOnTouchMoveListener()
{
    jerry_release_value(fn_);
}

bool ViewOnTouchMoveListener::OnDrag(UIView &view, const DragEvent &event)
{
    if (jerry_value_is_undefined(fn_)) {
        return false;
    }
    jerry_value_t *args = ConvertDragEventInfo(event, id_, view);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

ViewOnTouchEndListener::ViewOnTouchEndListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchEndListener::~ViewOnTouchEndListener()
{
    jerry_release_value(fn_);
}

bool ViewOnTouchEndListener::OnRelease(UIView &view, const ReleaseEvent &event)
{
    return CallBaseEvent(fn_, event, id_, view);
}

ViewOnTouchCancelListener::ViewOnTouchCancelListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchCancelListener::~ViewOnTouchCancelListener()
{
    jerry_release_value(fn_);
}
bool ViewOnTouchCancelListener::OnCancel(UIView &view, const CancelEvent &event)
{
    return CallBaseEvent(fn_, event, id_, view);
}
#endif // JS_TOUCH_EVENT_SUPPORT

ViewOnRotateListener::ViewOnRotateListener(jerry_value_t fn)
{
    fn_ = jerry_acquire_value(fn);
}

ViewOnRotateListener::~ViewOnRotateListener()
{
    jerry_release_value(fn_);
}

bool ViewOnRotateListener::OnRotateStart(UIView& view, const RotateEvent& event)
{
    if (jerry_value_is_undefined(fn_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnRotateStart received, but no JS function to call");
        return false;
    }    
    HILOG_DEBUG(HILOG_MODULE_ACE, "OnRotateStart received");
    jerry_value_t *args = ConvertRotateEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

bool ViewOnRotateListener::OnRotate(UIView& view, const RotateEvent& event)
{
    if (jerry_value_is_undefined(fn_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnRotate received, but no JS function to call");
        return false;
    }    
    HILOG_DEBUG(HILOG_MODULE_ACE, "OnRotate received");
    jerry_value_t *args = ConvertRotateEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

bool ViewOnRotateListener::OnRotateEnd(UIView& view, const RotateEvent& event)
{
    if (jerry_value_is_undefined(fn_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnRotateEnd received, but no JS function to call");
        return false;
    }    
    HILOG_DEBUG(HILOG_MODULE_ACE, "OnRotateEnd received");
    jerry_value_t *args = ConvertRotateEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

KeyInputEventListener::KeyInputEventListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}
 
KeyInputEventListener::~KeyInputEventListener()
{
    jerry_release_value(fn_);
}
 
void KeyInputEventListener::OnKeyPress(void)
{
    pressFlag = 1; // 短按标志
    return;
}
 
void KeyInputEventListener::OnKeyLongPress(const KeyEvent& event)
{
    longPressFlag = 1; // 长按标志
 
    uint16_t keyID = event.GetKeyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {
        // 切换到关机/重启卡片
        NativeAbility::GetInstance().ChangeSlice(VIEW_REBOOT);
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) {
        // 切换到查看系统版本卡片
    }
    return;
}
 
void KeyInputEventListener::OnKeyRelease(const KeyEvent& event)
{
    uint16_t currentViewId = NativeAbility::GetInstance().GetCurSliceId();
    uint16_t keyID = event.GetKeyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {
        if (longPressFlag == 0 && pressFlag == 1) { // 短按电源键
            // 退出JS应用
            JsAppContext::GetInstance()->TerminateAbility();
        }
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) {
        if (longPressFlag == 0 && pressFlag == 1) { // 短按功能键
            // 短按功能键处理
        }
    }
 
    pressFlag = 0;
    longPressFlag = 0;
    return;
}
 
bool KeyInputEventListener::OnKeyAct(UIView& view, const KeyEvent& event)
{
    UNUSED(view);
 
    uint8_t state = event.GetState();
 
    switch (state) {
        case InputDevice::STATE_PRESS: // 短按
            OnKeyPress();
            break;
#ifdef __LITEOS_M__
        case InputDevice::STATE_LONG_PRESS: // 长按
            OnKeyLongPress(event);
            break;
#endif
        case InputDevice::STATE_RELEASE: // 释放
            OnKeyRelease(event);
            break;
        default:
            break;
    }
    return true;
}

void ViewOnSwipeListener::SetStopPropagation(bool isStopPropogation)
{
    isStopPropagation_ = isStopPropogation;
}

bool ViewOnSwipeListener::OnDragStart(UIView& view, const DragEvent &event)
{
    UNUSED(view);
    UNUSED(event);
    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragStart received");
    return isStopPropagation_;
}

bool ViewOnSwipeListener::OnDrag(UIView& view, const DragEvent& event)
{
    UNUSED(view);
    UNUSED(event);
    // HILOG_DEBUG(HILOG_MODULE_ACE, "OnDrag received");
    return isStopPropagation_;
}

bool ViewOnSwipeListener::OnDragEnd(UIView& view, const DragEvent &event)
{
    if (JSUndefined::Is(fn_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnDragEnd received, but no JS function to call");
        return isStopPropagation_;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragEnd received");

    JSValue arg = EventUtil::CreateSwipeEvent(view, event);
    EventUtil::InvokeCallback(JSUndefined::Create(), fn_, arg);

    return isStopPropagation_;
}

void ValueChangeListener::OnChange(UIView &view, const char *value)
{
    if (IS_UNDEFINED(fn_)) {
        return;
    }

    jerry_value_t textValue = jerry_create_string(reinterpret_cast<const jerry_char_t *>(value));
    jerry_value_t args[1] = {textValue};
    if (jerry_value_is_function(fn_)) {
        CallJSFunctionAutoRelease(fn_, UNDEFINED, args, 1);
    }
    ReleaseJerryValue(textValue, VA_ARG_END_FLAG);
    return;
}
} // namespace ACELite
} // namespace OHOS
