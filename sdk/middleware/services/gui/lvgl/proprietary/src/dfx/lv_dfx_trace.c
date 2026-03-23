/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#include "lv_dfx_trace.h"
#include <time.h>
#include "tcxo.h"
#if LV_USE_DFX_CMD && LV_USE_DFX_TRACE
static uint64_t g_pTraceStartTime[PERIOD_TRACE_MAX] = {0};
static uint32_t g_pTraceTotalTime[PERIOD_TRACE_MAX] = {0};
static uint32_t g_pTraceMaxTime[PERIOD_TRACE_MAX] = {0};
static uint32_t g_hwTimePerFrame = 0;
static uint32_t g_flushCount = 0;
static uint32_t g_taskCount = 0;
static uint32_t g_refrCount = 0;
static uint32_t g_animCount = 0;
static uint32_t g_taskIntervalTimes = 0;
static uint32_t g_taskIntervalMaxTime = 0;
static uint32_t g_taskIntervalMinTime = 0xFFFFFFFF;
static uint32_t g_taskMinTime = 0xFFFFFFFF;
static uint64_t g_lastTime = 0;
static uint64_t g_currentTime = 0;
static bool g_periodTraceEnable = false;
static const uint16_t MS_TO_US = 1000;

#define DRAW_RECORD_MAX 50
static uint64_t g_drawStartTime = 0;
static uint64_t g_drawEndTime = 0;
static uint64_t g_firstDrawStartTime = 0;
static uint32_t g_fTraceCmd[DRAW_RECORD_MAX] = {0};
static uint32_t g_fTraceCostTime[DRAW_RECORD_MAX] = {0};
static uint32_t g_fTracePrepareTime[DRAW_RECORD_MAX] = {0};
static lv_area_t g_drawRect[DRAW_RECORD_MAX] = {0};

static bool g_hasLetter = false;
static uint32_t g_letterNum = 0;
static uint32_t g_letterIndex = 0;
static uint8_t g_drawCount = 0;
static uint32_t g_drawStack = 0;
static bool g_frameTraceBegin = false;
static bool g_enableFrameTrace = false;

static void ResetPeriodTrace(void)
{
    memset_s(g_pTraceStartTime, sizeof(uint64_t) * PERIOD_TRACE_MAX, 0, sizeof(uint64_t) * PERIOD_TRACE_MAX);
    memset_s(g_pTraceTotalTime, sizeof(uint32_t) * PERIOD_TRACE_MAX, 0, sizeof(uint32_t) * PERIOD_TRACE_MAX);
    memset_s(g_pTraceMaxTime, sizeof(uint32_t) * PERIOD_TRACE_MAX, 0, sizeof(uint32_t) * PERIOD_TRACE_MAX);
    g_flushCount = 0;
    g_taskCount = 0;
    g_refrCount = 0;
    g_animCount = 0;
    g_taskIntervalTimes = 0;
    g_taskIntervalMaxTime = 0;
    g_taskIntervalMinTime = 0xFFFFFFFF;
    g_taskMinTime = 0xFFFFFFFF;
}

void EnablePeriodTrace(bool status)
{
    g_periodTraceEnable = status;
    if (!status) {
        ResetPeriodTrace();
    } else {
        g_lastTime = uapi_tcxo_get_us();
    }
}

void PeriodTraceStart(PeriodTraceType tag)
{
    if (g_periodTraceEnable) {
        uint64_t time = uapi_tcxo_get_us();
        if ((tag == PERIOD_TRACE_TASK) && g_pTraceStartTime[tag] != 0 && g_taskCount != 0) {
            uint32_t interval = (uint32_t)(time - g_pTraceStartTime[tag]);
            if (interval > g_taskIntervalMaxTime) {
                g_taskIntervalMaxTime = interval;
            }
            if (interval < g_taskIntervalMinTime) {
                g_taskIntervalMinTime = interval;
            }
            g_taskIntervalTimes += interval;
        }
        g_pTraceStartTime[tag] = time;
    }
}

void PeriodTraceEnd(PeriodTraceType tag)
{
    if (g_periodTraceEnable) {
        uint64_t time = uapi_tcxo_get_us();
        uint32_t record = (uint32_t)(time - g_pTraceStartTime[tag]);
        g_pTraceTotalTime[tag] += record;
        if (tag == PERIOD_TRACE_HW_DRAW) {
            g_hwTimePerFrame += record;
        } else {
            if (g_pTraceMaxTime[tag] < record) {
                g_pTraceMaxTime[tag] = record;
            }
        }

        if (tag == PERIOD_TRACE_TASK) {
            g_taskCount++;
            if (record < g_taskMinTime) {
                g_taskMinTime = record;
            }
        } else if (tag == PERIOD_TRACE_FLUSH) {
            if (g_pTraceMaxTime[PERIOD_TRACE_HW_DRAW] < g_hwTimePerFrame) {
                g_pTraceMaxTime[PERIOD_TRACE_HW_DRAW] = g_hwTimePerFrame;
            }
            g_hwTimePerFrame = 0;
            g_flushCount++;
        } else if (tag == PERIOD_TRACE_REFR) {
            g_refrCount++;
        } else if (tag == PERIOD_TRACE_ANIM) {
            g_animCount++;
        }
    }
}

void ShowPeriodTrace(void)
{
    if (g_periodTraceEnable) {
        g_currentTime = uapi_tcxo_get_us();
        if ((g_currentTime - g_lastTime) / MS_TO_US > 1000) { // 1000: 1000ms
            uint32_t taskIntervalAvg = (g_taskCount <= 1) ? 0 : (g_taskIntervalTimes / (g_taskCount - 1));
            uint32_t taskIntervalMinTime = (g_taskIntervalMinTime == 0xFFFFFFFF) ? 0 : g_taskIntervalMinTime;
            uint32_t taskMinTime = (g_taskMinTime == 0xFFFFFFFF) ? 0 : g_taskMinTime;
            uint32_t taskAvg = (g_taskCount == 0) ? 0 : (g_pTraceTotalTime[PERIOD_TRACE_TASK] / g_taskCount);
            uint32_t animAvg = (g_animCount == 0) ? 0 : g_pTraceTotalTime[PERIOD_TRACE_ANIM] / g_animCount;
            uint32_t indevAvg = (g_taskCount == 0) ? 0 : g_pTraceTotalTime[PERIOD_TRACE_INDEV] / g_taskCount;
            uint32_t drawAvg = (g_flushCount == 0) ? 0 : g_pTraceTotalTime[PERIOD_TRACE_TOTAL_DRAW] / g_flushCount;
            uint32_t hwAvg = (g_flushCount == 0) ? 0 : g_pTraceTotalTime[PERIOD_TRACE_HW_DRAW] / g_flushCount;
            uint32_t flushAvg = (g_flushCount == 0) ? 0 : g_pTraceTotalTime[PERIOD_TRACE_FLUSH] / g_flushCount;
            LV_LOGI("\n         MAX AVE MIN\nTaskINR  %u %u %u\nTask     %u %u %u\nAnim     %u %u\nIndev    %u %u\n"
                "Draw     %u %u\nHwDraw   %u %u\nFlush    %u %u\nAnimCnt  %u\nFlushCnt %u\nRefrCnt  %u\nTaskCnt  %u\n",
                g_taskIntervalMaxTime, taskIntervalAvg, taskIntervalMinTime,
                g_pTraceMaxTime[PERIOD_TRACE_TASK], taskAvg, taskMinTime,
                g_pTraceMaxTime[PERIOD_TRACE_ANIM], animAvg,
                g_pTraceMaxTime[PERIOD_TRACE_INDEV], indevAvg,
                g_pTraceMaxTime[PERIOD_TRACE_TOTAL_DRAW], drawAvg,
                g_pTraceMaxTime[PERIOD_TRACE_HW_DRAW], hwAvg,
                g_pTraceMaxTime[PERIOD_TRACE_FLUSH], flushAvg,
                g_animCount, g_flushCount, g_refrCount, g_taskCount);
            g_lastTime = g_currentTime;
            ResetPeriodTrace();
        }
    }
}

void EnableFrameTrace(bool status)
{
    g_enableFrameTrace = status;
}

void BeginFrameTrace(bool isStart)
{
    if (g_enableFrameTrace) {
        g_frameTraceBegin = isStart;
    }
}

void FrameTraceStart(FrameTraceType tag)
{
    if (!g_frameTraceBegin || g_drawCount >= DRAW_RECORD_MAX) {
        return;
    }
    if (g_drawStack == 0) {
        g_drawStartTime = uapi_tcxo_get_us();
        if (g_drawCount == 0) {
            g_firstDrawStartTime = g_drawStartTime;
        } else {
            if ((tag == FRAME_TRACE_SW_DRAW_LETTER || tag == FRAME_TRACE_HW_DRAW_LETTER) && g_hasLetter) {
                g_fTracePrepareTime[g_letterIndex] += (uint32_t)(g_drawStartTime - g_drawEndTime);
            } else {
                g_fTracePrepareTime[g_drawCount] = (uint32_t)(g_drawStartTime - g_drawEndTime);
            }
        }
    }
    g_drawStack++;
}

void FrameTraceEnd(FrameTraceType tag, const lv_area_t* area)
{
    if (!g_frameTraceBegin || g_drawCount >= DRAW_RECORD_MAX) {
        return;
    }
    g_drawStack--;
    if (g_drawStack == 0) {
        g_drawEndTime = uapi_tcxo_get_us();
        uint32_t diffValue = (uint32_t)(g_drawEndTime - g_drawStartTime);
        if (tag == FRAME_TRACE_SW_DRAW_LETTER || tag == FRAME_TRACE_HW_DRAW_LETTER) {
            if (!g_hasLetter) {
                g_letterIndex = g_drawCount;
                g_fTraceCmd[g_drawCount] = tag;
                g_hasLetter = true;
                g_drawCount++;
                g_fTraceCostTime[g_letterIndex] = diffValue;
            } else {
                g_fTraceCostTime[g_letterIndex] += diffValue;
            }
            g_letterNum++;
        } else {
            if (area != NULL) {
                g_drawRect[g_drawCount] = *area;
            } else {
                memset_s(&g_drawRect[g_drawCount], sizeof(lv_area_t), 0, sizeof(lv_area_t));
            }
            g_fTraceCostTime[g_drawCount] = diffValue;
            g_fTraceCmd[g_drawCount] = tag;
            g_drawCount++;
        }
    }
}

void ShowFrameTrace(void)
{
    if (g_enableFrameTrace && g_frameTraceBegin) {
        static const char* g_drawTab[FRAME_TRACE_MAX] = {"sw_img", "hw_img", "sw_imgTrans", "hw_imgTrans",
            "hw_imgBlit", "hw_path", "sw_rect", "sw_shadow", "sw_bg", "sw_bo", "hw_rect", "hw_roundRect", "hw_blur",
            "sw_letter", "hw_letter", "sw_arc", "sw_line", "sw_polygon", "sw_bufCopy", "sw_layerC",
            "sw_layerA", "sw_layerB", "sw_layerD", "hw_sync"};
        LV_LOGI("\nCmd count: %u time: %u\n", g_drawCount, (uint32_t)(g_drawEndTime - g_firstDrawStartTime));
        for (uint32_t i = 0; i < g_drawCount; i++) {
            uint32_t cmd = g_fTraceCmd[i];
            if (i >= 1) {
                LV_LOGI("    prep: %u\n", g_fTracePrepareTime[i]);
            }
            if (g_hasLetter && (i == g_letterIndex)) {
                LV_LOGI("%u. %s: %u %u\n", i, g_drawTab[cmd], g_fTraceCostTime[i], g_letterNum);
            } else {
                LV_LOGI("%u. %s: %u {%d %d %d %d}\n", i, g_drawTab[cmd], g_fTraceCostTime[i],
                    g_drawRect[i].x1, g_drawRect[i].y1, g_drawRect[i].x2, g_drawRect[i].y2);
            }
        }
        g_drawCount = 0;
        g_frameTraceBegin = false;
        g_enableFrameTrace = false;
        g_hasLetter = false;
        g_letterNum = 0;
        g_letterIndex = 0;
        g_drawStack = 0;
    }
}
#endif  // LV_USE_DFX_TRACE