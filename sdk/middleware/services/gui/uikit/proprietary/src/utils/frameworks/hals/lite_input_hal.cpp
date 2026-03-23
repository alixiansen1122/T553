/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#include "hals/lite_input_hal.h"
#include "common/input_device_manager.h"
#include "common/key_code.h"
#include "gfx_utils/graphic_log.h"
#include "hals/lite_input_distributer.h"
#include "dock/lite_key_device.h"
#include "dock/lite_touch_device.h"
#include "dfx/dfx_record.h"

namespace OHOS {
static bool ConvertKeyId(uint16_t indevKey, uint16_t* key)
{
    if (indevKey == INDEV_KEY_POWER) {
        *key = static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER);
    } else if (indevKey == INDEV_KEY_FUNC) {
        *key = static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC);
    } else {
        GRAPHIC_LOGW("Unsupported KeyId.");
        return false;
    }
    return true;
}

static void EventCallback(InputDevData* data)
{
    if (data == nullptr) {
        return;
    }
    RawEvent event = {0};
    if (data->type == INDEV_TYPE_TOUCH) {
        if (IsShowTPRaw()) {
            GRAPHIC_LOGP("TP s: %u, x: %d, y: %d, t: %u", data->state, data->x, data->y,
                data->timestamp);
        }
        event.x = data->x;
        event.y = data->y;
        event.state = data->state;
        event.timestamp = data->timestamp;
        event.type = InputDevType::INDEV_TYPE_TOUCH;
    } else if (data->type == INDEV_TYPE_KEY) {
        if (!ConvertKeyId(data->keyId, &event.keyId)) {
            return;
        }
        event.state = data->state;
        event.timestamp = data->timestamp;
        event.type = InputDevType::INDEV_TYPE_KEY;
    } else {
        return;
    }
    LiteInputDistributer::GetInstance()->SendRawEvent(event);
}

void LiteInputHal::AddToIndevManager(uint8_t type)
{
    InputDevice* dev = nullptr;
    if (type == static_cast<uint8_t>(INDEV_TYPE_TOUCH)) {
        dev = new LiteTouchDevice();
    } else if (type == static_cast<uint8_t>(INDEV_TYPE_KEY)) {
        dev = new LiteKeyDevice();
    }
    InputDeviceManager::GetInstance()->Add(dev);
}

bool LiteInputHal::SetUp()
{
    if (InitInputManager(&inputFuncs_) != INPUT_SUCCESS) {
        return false;
    }
    if ((inputFuncs_->ScanInputDevice == nullptr) || (inputFuncs_->RegisterEventCallback == nullptr) ||
        (inputFuncs_->UnregisterReportCallback == nullptr) || (inputFuncs_->OpenInputDevice == nullptr) ||
        (inputFuncs_->CloseInputDevice == nullptr)) {
        DeinitInputManager(inputFuncs_);
        GRAPHIC_LOGE("input funcs is nullptr.");
        return false;
    }
    inputDevs_ = inputFuncs_->ScanInputDevice(&inputDevsNum_);
    for (uint8_t i = 0; i < inputDevsNum_; i++) {
        if (inputFuncs_->RegisterEventCallback(inputDevs_[i], EventCallback) != INPUT_SUCCESS) {
            continue;
        }
        if (inputFuncs_->OpenInputDevice(inputDevs_[i]) != INPUT_SUCCESS) {
            inputFuncs_->UnregisterReportCallback(inputDevs_[i]);
            continue;
        }
        AddToIndevManager(inputDevs_[i]);
    }
    return true;
}

void LiteInputHal::TearDown()
{
    for (uint8_t i = 0; i < inputDevsNum_; i++) {
        inputFuncs_->UnregisterReportCallback(inputDevs_[i]);
        inputFuncs_->CloseInputDevice(inputDevs_[i]);
    }
    if (inputDevs_ != nullptr) {
        free(inputDevs_);
        inputDevs_ = nullptr;
    }
    DeinitInputManager(inputFuncs_);
    inputDevsNum_ = 0;
}
}