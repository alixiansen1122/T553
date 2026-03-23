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

#ifndef GRAPHIC_LITE_ROTATE_EVENT_INJECTOR_H
#define GRAPHIC_LITE_ROTATE_EVENT_INJECTOR_H

#include "graphic_config.h"
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#include "dock/rotate_input_device.h"
#include "gfx_utils/list.h"

namespace OHOS {
class RotateEventInjector : public RotateInputDevice {
public:
    RotateEventInjector() : lastRotate(0) {}
    ~RotateEventInjector() override;
    bool SetRotateEvent(const DeviceData& data);
    bool Read(DeviceData& data) override;
    uint8_t GetLeftSize() const;
    bool IsEventEmpty() override;
private:
    List<DeviceData*> dataList_;
    int16_t lastRotate;
};
}
#endif // ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#endif // GRAPHIC_LITE_ROTATE_EVENT_INJECTOR_H
