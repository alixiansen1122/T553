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

#include <unistd.h>
#include <cstdint>
#include <ctime>
#include "graphic_config.h"
#include "securec.h"
#include "hal_tick.h"
#include "gfx_utils/graphic_log.h"
#include "dfx/dfx_record.h"

namespace OHOS {
#if ENABLE_DFX_CMD
static uint32_t g_lastTime;
static uint32_t g_currentTime;
static uint32_t g_tpSetTime;
static ShowType g_dfxShow = ShowType::DFX_SHOW_DISABLE;
static DfxRecord g_dfxRecord = {0};
static const uint16_t MS_TO_US = 1000;

static uint32_t GetTimeUs()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000000 + time.tv_nsec / 1000; // 1000000, 1000: time conversion for s to us and ms to us.
}

void DfxShowType(ShowType type)
{
    g_dfxShow = type;
}

bool IsShowTPRaw()
{
    return (g_dfxShow == ShowType::DFX_SHOW_TP);
}

bool IsShowPoint()
{
    return (g_dfxShow == ShowType::DFX_SHOW_POINT);
}

void DfxTPSetEvent(uint32_t time)
{
    g_dfxRecord.setEventCnt++;
    if (time > g_dfxRecord.setEventGapMax) {
        g_dfxRecord.setEventGapMax = time;
    } else if ((time < g_dfxRecord.setEventGapMin) || (g_dfxRecord.setEventGapMin == 0)) {
        g_dfxRecord.setEventGapMin = time;
    }
    if (g_tpSetTime == 0) {
        g_tpSetTime = GetTimeUs();
    }
}

void DfxTPGetEvent()
{
    g_dfxRecord.getEventCnt++;
    uint32_t temp = GetTimeUs() - g_tpSetTime;
    g_dfxRecord.getToSetTimeTotal += temp;
    if (temp > g_dfxRecord.getToSetTimeMax) {
        g_dfxRecord.getToSetTimeMax = temp;
    }
    g_tpSetTime = 0;
}

void DfxRecordShow()
{
    if (g_dfxShow != ShowType::DFX_SHOW_POINT) {
        return;
    }

    g_currentTime = GetTimeUs();
    if ((g_currentTime - g_lastTime) / MS_TO_US > 1000) { // 1000: 1000ms
        if (g_dfxShow == ShowType::DFX_SHOW_POINT) {
            uint32_t g2sAvg = (g_dfxRecord.getEventCnt == 0) ? 0 :
                g_dfxRecord.getToSetTimeTotal / (g_dfxRecord.getEventCnt * MS_TO_US);
            GRAPHIC_LOGP("TP: sc: %u, gc: %u, sMin: %u, sMax: %u, g2sAvg: %u, g2sMax: %u", g_dfxRecord.setEventCnt,
                g_dfxRecord.getEventCnt, g_dfxRecord.setEventGapMin, g_dfxRecord.setEventGapMax, g2sAvg,
                g_dfxRecord.getToSetTimeMax / MS_TO_US);
        }
        g_lastTime = g_currentTime;
        memset_s(&g_dfxRecord, sizeof(g_dfxRecord), 0, sizeof(g_dfxRecord));
    }
}
#else
void DfxShowType(ShowType type)
{
    return;
}

bool IsShowTPRaw()
{
    return false;
}

bool IsShowPoint()
{
    return false;
}

void DfxTPSetEvent(uint32_t time)
{
    return;
}
void DfxTPGetEvent()
{
    return;
}

void DfxRecordShow()
{
    return;
}
#endif // ENABLE_DFX_CMD
}

