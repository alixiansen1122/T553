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

#include "lite_key_device.h"
#include "components/root_view.h"
#include "gfx_utils/input_event_info.h"
#include "lite_input_distributer.h"
#ifdef SUPPORT_OHOSFWK
#include "graphic_service.h"
#include "modules/keyinput/keyinput.h"
#endif

namespace OHOS {
bool LiteKeyDevice::Read(DeviceData& data)
{
    LiteInputDistributer::GetInstance()->GetEventData(data, InputDevType::INDEV_TYPE_KEY);
    return false;
}

void LiteKeyDevice::DispatchEvent(const DeviceData& data)
{
    if (data.keyId == INVALID_KEY_ID) {
        return;
    }
    KeyEvent event({data.keyId, data.state});
    RootView::GetInstance()->OnKeyEvent(event);
#ifdef SUPPORT_OHOSFWK
    if (!GraphicService::GetInstance()->IsNativeRunning()) {
        OHOS::ACELite::KeyListener::GetInstance()->OnKeyEvent(event);
    }
#endif
}

bool LiteKeyDevice::IsEventEmpty()
{
    return LiteInputDistributer::GetInstance()->IsEventEmpty(InputDevType::INDEV_TYPE_KEY);
}
}
