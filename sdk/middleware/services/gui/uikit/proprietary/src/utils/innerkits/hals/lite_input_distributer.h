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

#ifndef GRAPHIC_LITE_INPUT_DISTRIBUTER_H
#define GRAPHIC_LITE_INPUT_DISTRIBUTER_H

#include <cstdio>
#include <list>
#include "dock/input_device.h"
#include "gfx_utils/input_event_info.h"
#include "graphic_mutex.h"

namespace OHOS {
const uint16_t INVALID_KEY_ID = 0xffff;
const uint8_t MAX_KEY_LIST_SIZE = 60;
const uint8_t MAX_TP_LIST_SIZE = 3;
class LiteInputDistributer : public HeapBase {
public:
    static LiteInputDistributer* GetInstance()
    {
        static LiteInputDistributer distributer;
        return &distributer;
    }

    void SendRawEvent(const RawEvent& rawEvent);
    void GetEventData(DeviceData& data, InputDevType type);
    void SetEventData(const RawEvent& event);
    bool IsEventEmpty(InputDevType type);
    void ClearEvent(InputDevType type);
#ifdef CONFIG_SUPPORTED_POWERMGR
    void SendEventToPM(const RawEvent& event);
#endif
private:
    LiteInputDistributer();
    ~LiteInputDistributer();

    GraphicMutex eventLock_;
    DeviceData deviceData_;
    std::list<DeviceData> keyList_;
    std::list<DeviceData> tpList_;
};
}
#endif
