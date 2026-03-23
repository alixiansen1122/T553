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

#ifndef GRAPHIC_LITE_DFX_TYPE_H
#define GRAPHIC_LITE_DFX_TYPE_H

namespace OHOS {
/**
 * @brief Dfx record data struct.
 */
struct DfxRecord {
    /* touch event */
    uint8_t setEventCnt;        // Set tp event count.
    uint8_t getEventCnt;        // Get tp event count.
    uint32_t setEventGapMin;    // The min time gap for current tp event to last one.
    uint32_t setEventGapMax;    // The max time gap for current tp event to last one.
    uint32_t getToSetTimeTotal; // The total time accumulated by each gap from get event time to set event time.
    uint32_t getToSetTimeMax;   // The max for get event time to set event time.
};

enum class ShowType {
    DFX_SHOW_TP = 0,
    DFX_SHOW_POINT,
    DFX_SHOW_DISABLE
};

void DfxShowType(ShowType type);
bool IsShowTPRaw();
bool IsShowPoint();
void DfxTPSetEvent(uint32_t time);
void DfxTPGetEvent();
void DfxRecordShow();
}
#endif // GRAPHIC_LITE_DFX_TYPE_H
