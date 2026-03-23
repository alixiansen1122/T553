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

#include "dfx/rotate_event_injector.h"
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#include "gfx_utils/graphic_log.h"

namespace {
const uint8_t MAX_LIST_SIZE = 100;
}

namespace OHOS {
RotateEventInjector::~RotateEventInjector()
{
    ListNode<DeviceData*>* node = dataList_.Begin();
    while (node != dataList_.End()) {
        if (node->data_ != nullptr) {
            DeviceData* deleteData = node->data_;
            delete deleteData;
            deleteData = nullptr;
        }
        node = node->next_;
    }
    dataList_.Clear();
}

bool RotateEventInjector::Read(DeviceData& data)
{
    if (dataList_.IsEmpty()) {
        lastRotate = 0;
    }
    if (dataList_.Size() > 0) {
        lastRotate = dataList_.Front()->rotate;
        ListNode<DeviceData*>* node = dataList_.Begin();
        if (node->data_ != nullptr) {
            DeviceData* deleteData = node->data_;
            delete deleteData;
            deleteData = nullptr;
        }
        dataList_.PopFront();
    }
    data.rotate = lastRotate;
    return false;
}

bool RotateEventInjector::SetRotateEvent(const DeviceData& data)
{
    if (dataList_.Size() >= MAX_LIST_SIZE) {
        GRAPHIC_LOGI("RotateEventInjector::SetRotateEvent data list is full.");
        return false;
    }

    DeviceData* tmpData = new DeviceData;
    if (tmpData == nullptr) {
        GRAPHIC_LOGE("RotateEventInjector::SetRotateEvent memory allocation failed Err!");
        return false;
    }
    tmpData->rotate = data.rotate;
    dataList_.PushBack(tmpData);
    return true;
}

uint8_t RotateEventInjector::GetLeftSize() const
{
    return MAX_LIST_SIZE - dataList_.Size();
}

bool RotateEventInjector::IsEventEmpty()
{
    return (dataList_.Size() <= 0);
}
}
#endif // ENABLE_DEBUG && ENABLE_ROTATE_INPUT