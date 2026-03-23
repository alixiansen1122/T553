/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui core interface
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef _LOS_TRACEUI_H
#define _LOS_TRACEUI_H

#include "dfx/dfx_traceui.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRACEUI_FRAME_LEN_MAX 0xFF
#define TRACEUI_FRAME_LEN_ALIGN sizeof(uintptr_t)

typedef enum {
    TRACEUI_TASK_RUNNING = 1,
    TRACEUI_TASK_DELAY   = 2,
    TRACEUI_TASK_PENDING = 3,
} TraceuiTaskStat;

typedef struct {
    uint32_t startTime; // unit: cycle
    uint8_t taskid; // current or pre sched switch task id
    uint8_t cpuid; // which cpu core
    uint8_t eventType; // TraceuiEventType
    uint8_t frameLen; // include frame header
} TraceuiFrameHeader;

typedef struct {
    TraceuiFrameHeader header;
    uint8_t prePrio; // task priority before task switch
    uint8_t preState; // TraceuiTaskStat
    uint8_t nextTaskId; // taskid after task switch
    uint8_t nextPrio; // task priority after task switch
} TraceuiFrameTaskSwitch;

typedef struct {
    TraceuiFrameHeader header;
    uint8_t wakePrio;
    uint8_t wakeTaskId;
} TraceuiFrameTaskWakeup;

typedef struct {
    TraceuiFrameHeader header;
    uintptr_t name; // string pointer
} TraceuiFrameTraceBegin;

typedef struct {
    TraceuiFrameHeader header;
    uintptr_t name; // string pointer
    char str[0];
} TraceuiFrameTraceBeginFormat;

typedef struct {
    TraceuiFrameHeader header;
} TraceuiFrameTraceEnd;

typedef struct {
    TraceuiFrameHeader header;
    uintptr_t name; // string pointer
    int cookie;
} TraceuiFrameTraceAsync;

typedef struct {
    TraceuiFrameHeader header;
    uintptr_t name; // string pointer
    int value;
} TraceuiFrameTraceInt;

typedef struct {
    TraceuiFrameHeader header;
    uint8_t hwiNum;
} TraceuiFrameHwiEnter;

typedef struct {
    TraceuiFrameHeader header;
    uint8_t hwiNum;
} TraceuiFrameHwiExit;

typedef struct {
    TraceuiFrameHeader header;
    char str[0];
} TraceuiFrameLog;

void TraceuiEventTaskSwitch(uint32_t newTaskId);
void TraceuiEventTaskWakeup(uint32_t wakeTaskId);
void TraceuiEventHwiEnter(uint32_t hwiNum);
void TraceuiEventHwiExit(uint32_t hwiNum);

// useage: uint32_t pos = 0; while ((frame = TraceuiWalkBuf(buf, bufSize, &pos)) != NULL) {}
const TraceuiFrameHeader* TraceuiWalkBuf(const uint8_t *buf, uint32_t bufSize, uint32_t *pos);

#ifdef __cplusplus
}
#endif
#endif