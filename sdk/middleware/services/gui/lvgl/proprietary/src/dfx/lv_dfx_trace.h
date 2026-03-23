/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DFX_TRACE_H
#define LV_DFX_TRACE_H
#include "lvgl.h"
#include "lv_conf.h"
#include "misc/lv_area.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIOD_TRACE_TASK,
    PERIOD_TRACE_REFR,
    PERIOD_TRACE_INDEV,
    PERIOD_TRACE_ANIM,
    PERIOD_TRACE_TOTAL_DRAW,
    PERIOD_TRACE_HW_DRAW,
    PERIOD_TRACE_FLUSH,
    PERIOD_TRACE_MAX
} PeriodTraceType;

typedef enum {
    FRAME_TRACE_SW_DRAW_IMGDECODED,
    FRAME_TRACE_HW_DRAW_IMGDECODED,
    FRAME_TRACE_SW_DRAW_TRANSFORM,
    FRAME_TRACE_HW_DRAW_TRANSFORM,
    FRAME_TRACE_HW_DRAW_IMGBLIT,
    FRAME_TRACE_HW_DRAW_PATH,
    FRAME_TRACE_SW_DRAW_RECT,
    FRAME_TRACE_SW_DRAW_SHADOW,
    FRAME_TRACE_SW_DRAW_BACKGROUND,
    FRAME_TRACE_SW_DRAW_BORDER_OUTLINE,
    FRAME_TRACE_HW_DRAW_RECT,
    FRAME_TRACE_HW_DRAW_ROUND_RECT,
    FRAME_TRACE_HW_DRAW_BLUR,
    FRAME_TRACE_SW_DRAW_LETTER,
    FRAME_TRACE_HW_DRAW_LETTER,
    FRAME_TRACE_SW_DRAW_ARC,
    FRAME_TRACE_SW_DRAW_LINE,
    FRAME_TRACE_SW_DRAW_POLYGON,
    FRAME_TRACE_SW_DRAW_BUFCOPY,
    FRAME_TRACE_SW_DRAW_LAYERC,
    FRAME_TRACE_SW_DRAW_LAYERA,
    FRAME_TRACE_SW_DRAW_LAYERB,
    FRAME_TRACE_SW_DRAW_LAYERD,
    FRAME_TRACE_HW_DRAW_SYNC,
    FRAME_TRACE_MAX
} FrameTraceType;

void EnablePeriodTrace(bool status);
void PeriodTraceStart(PeriodTraceType tag);
void PeriodTraceEnd(PeriodTraceType tag);
void ShowPeriodTrace(void);

void EnableFrameTrace(bool status);
void BeginFrameTrace(bool isStart);
void FrameTraceStart(FrameTraceType tag);
void FrameTraceEnd(FrameTraceType tag, const lv_area_t* area);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif