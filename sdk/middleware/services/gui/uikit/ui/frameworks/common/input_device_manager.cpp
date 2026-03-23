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

#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "gfx_utils/graphic_log.h"
#include "dock/pointer_input_device.h"
#include "dock/rotate_input_device.h"
#include "hals/lite_input_distributer.h"
#include "gfx_utils/input_event_info.h"

namespace OHOS {
InputDeviceManager* InputDeviceManager::GetInstance()
{
    static InputDeviceManager instance;
    return &instance;
}

void InputDeviceManager::Init()
{
    SetPeriod(0); // Input的默认处理周期受RTC/TE刷新周期控制
    TaskManager::GetInstance()->Add(this);
}

void InputDeviceManager::Add(InputDevice* device)
{
    if (device == nullptr) {
        GRAPHIC_LOGE("InputDeviceManager::Add invalid param\n");
        return;
    }
    deviceList_.PushBack(device);
}

void InputDeviceManager::Remove(InputDevice* device)
{
    if (device == nullptr) {
        return;
    }
    ListNode<InputDevice*>* node = deviceList_.Begin();
    while (node != deviceList_.End()) {
        if (node->data_ == device) {
            deviceList_.Remove(node);
            return;
        }
        node = node->next_;
    }
}

bool InputDeviceManager::IsEventExist()
{
    ListNode<InputDevice*>* node = deviceList_.Begin();
    while (node != deviceList_.End()) {
        if (!node->data_->IsEventEmpty()) {
            return true;
        }
        node = node->next_;
    }
    return false;
}

void InputDeviceManager::Callback()
{
    ListNode<InputDevice*>* node = deviceList_.Begin();
    while (node != deviceList_.End()) {
        node->data_->ProcessEvent();
        node = node->next_;
    }
}

void InputDeviceManager::Clear()
{
    deviceList_.Clear();
}

void InputDeviceManager::ClearPressableViews()
{
    ListNode<InputDevice*>* node = deviceList_.Begin();
    while (node != deviceList_.End()) {
        PointerInputDevice* device = dynamic_cast<PointerInputDevice*>(node->data_);
        if (device != nullptr) {
            device->ClearPressableViews();
        }
        node = node->next_;
    }
}

void InputDeviceManager::ClearRotateState()
{
    ListNode<InputDevice*>* node = deviceList_.Begin();
    while (node != deviceList_.End()) {
        RotateInputDevice* device = dynamic_cast<RotateInputDevice*>(node->data_);
        if (device != nullptr) {
            device->ResetRotateState();
        }
        node = node->next_;
    }
}

void InputDeviceManager::ClearInputDeviceState(InputDeviceManager::InputDeviceType type)
{
    switch (type) {
        case InputDeviceManager::InputDeviceType::ALL:
            InputDeviceManager::GetInstance()->ClearPressableViews();
            InputDeviceManager::GetInstance()->ClearRotateState();
            LiteInputDistributer::GetInstance()->ClearEvent(InputDevType::INDEV_TYPE_TOUCH);
            LiteInputDistributer::GetInstance()->ClearEvent(InputDevType::INDEV_TYPE_KEY);
            break;
        case InputDeviceManager::InputDeviceType::POINT:
            LiteInputDistributer::GetInstance()->ClearEvent(InputDevType::INDEV_TYPE_TOUCH);
            InputDeviceManager::GetInstance()->ClearPressableViews();
            break;
        case InputDeviceManager::InputDeviceType::ROTATE:
            InputDeviceManager::GetInstance()->ClearRotateState();
            break;
        case InputDeviceManager::InputDeviceType::KEY:
            LiteInputDistributer::GetInstance()->ClearEvent(InputDevType::INDEV_TYPE_KEY);
        default:
            GRAPHIC_LOGE("Cannot clear state of the input device: %d!", type);
            break;
    }
}
}  // namespace OHOS