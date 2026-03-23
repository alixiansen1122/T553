/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui core implementation
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include <stdio.h>
#include "asm/interrupt_config.h"
#include "traceui_buf.h"
#include "traceui_os.h"
#include "traceui_core.h"

static uint32_t g_eventMask = 0;
static bool g_trace_record = false;
static bool g_record_full = false;

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
static void TaskSwitchHook(uint32_t oldTaskId, uint32_t newTaskId)
{
    UNUSED(oldTaskId);
    TraceuiEventTaskSwitch(newTaskId);
}
#endif

#ifdef LOSCFG_HWI_PRE_POST_PROCESS
static void HwiProcPreHook(HWI_HANDLE_T hwiNum)
{
    TraceuiEventHwiEnter(hwiNum);
}

static void HwiProcPostHook(HWI_HANDLE_T hwiNum)
{
    TraceuiEventHwiExit(hwiNum);
}
#endif

bool TraceuiRecordStart(uint32_t events, uint8_t *buf, uint32_t bufSize)
{
    g_eventMask = events;
    g_record_full = false;
    TraceuiBufInit(buf, bufSize);

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
    LOS_TaskSwitchHookReg(TaskSwitchHook);
#else
    TRACEUI_ERR("Warn: no task switch, need LOSCFG_BASE_CORE_TSK_MONITOR=y\n");
#endif
#ifdef LOSCFG_HWI_PRE_POST_PROCESS
    LOS_HwiPreHookReg(HwiProcPreHook);
    LOS_HwiPostHookReg(HwiProcPostHook);
#else
    TRACEUI_ERR("Warn: no interrupt, need LOSCFG_HWI_PRE_POST_PROCESS=y\n");
#endif

    g_trace_record = TRUE;
    return true;
}

uint32_t TraceuiRecordStop(void)
{
    g_trace_record = false;
    uint32_t ret = TraceuiGetBufUsedSize();
    TraceuiBufDeinit();
#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
    LOS_TaskSwitchHookReg(NULL);
#endif
#ifdef LOSCFG_HWI_PRE_POST_PROCESS
    LOS_HwiPreHookReg(NULL);
    LOS_HwiPostHookReg(NULL);
#endif
    return ret;
}

static uint32_t GetEventTypeSize(TraceuiEventType type)
{
    switch (type) {
        case TRACEUI_EVENT_TASK_SWITCH:
            return sizeof(TraceuiFrameTaskSwitch);
        case TRACEUI_EVENT_TASK_WAKEUP:
            return sizeof(TraceuiFrameTaskWakeup);
        case TRACEUI_EVENT_TRACE_BEGIN:
            return sizeof(TraceuiFrameTraceBegin);
        case TRACEUI_EVENT_TRACE_BEGIN_FORMAT:
            return sizeof(TraceuiFrameTraceBeginFormat);
        case TRACEUI_EVENT_TRACE_END:
            return sizeof(TraceuiFrameTraceEnd);
        case TRACEUI_EVENT_TRACE_ASYNC_BEGIN:
        case TRACEUI_EVENT_TRACE_ASYNC_END:
            return sizeof(TraceuiFrameTraceAsync);
        case TRACEUI_EVENT_TRACE_INT:
            return sizeof(TraceuiFrameTraceInt);
        case TRACEUI_EVENT_HWI_ENTER:
            return sizeof(TraceuiFrameHwiEnter);
        case TRACEUI_EVENT_HWI_EXIT:
            return sizeof(TraceuiFrameHwiExit);
        case TRACEUI_EVENT_LOG:
            return sizeof(TraceuiFrameLog);
        default:
            break;
    }
    return 0;
}

const TraceuiFrameHeader* TraceuiWalkBuf(const uint8_t *buf, uint32_t bufSize, uint32_t *pos)
{
    uint32_t size;
    const TraceuiFrameHeader *header;

    if (buf == NULL || pos == NULL) {
        return NULL;
    }
    if (*pos + sizeof(TraceuiFrameHeader) > bufSize) {
        return NULL;
    }

    header = (const TraceuiFrameHeader *)&buf[*pos];
    size = header->frameLen;
    if (size == 0 || *pos + size > bufSize) {
        TRACEUI_ERR("Error size: event type: 0x%x\n", header->eventType);
        return NULL;
    }
    *pos += size;
    return header;
}

static TraceuiTaskStat ConvertTaskStatus(uint16_t taskStatus)
{
    if (taskStatus & OS_TASK_STATUS_READY) {
        return TRACEUI_TASK_RUNNING;
    } else if (taskStatus & OS_TASK_STATUS_DELAY) {
        return TRACEUI_TASK_DELAY;
    }
    return TRACEUI_TASK_PENDING;
}

static inline uint8_t ConvertTaskId(uint32_t taskId)
{
    if ((taskId & 0xFFFFFF00) != 0) {
        TRACEUI_ERR("Error! taskId too large: 0x%x\n", taskId);
    }
    return (uint8_t)(taskId & 0xFF);
}

static inline uint8_t ConvertPriority(uint16_t prio)
{
    if ((prio & 0xFFFFFF00) != 0) {
        TRACEUI_ERR("Error! priority too large: 0x%x\n", prio);
    }
    return (uint8_t)(prio & 0xFF);
}

static TraceuiFrameHeader *GenFrame(TraceuiEventType event, uint32_t addLen)
{
    if ((g_eventMask & TRACEUI_EVENT_MASK(event)) == 0 || g_record_full) {
        return NULL;
    }

    uint32_t len = GetEventTypeSize(event) + addLen;
    if (len <= addLen) {
        TRACEUI_ERR("unsupport event type: %d\n", (int)event);
        return NULL;
    }
    len = (len + TRACEUI_FRAME_LEN_ALIGN - 1) / TRACEUI_FRAME_LEN_ALIGN * TRACEUI_FRAME_LEN_ALIGN;
    if (len > TRACEUI_FRAME_LEN_MAX) {
        TRACEUI_ERR("Error! frame len too long, event: %d\n", event);
        return NULL;
    }
    TraceuiFrameHeader *header = (TraceuiFrameHeader *)TraceuiGetBuf(len);
    if (header == NULL) {
        g_record_full = true;
        TRACEUI_PRINT("record full!\n");
        return NULL;
    }

    const LosTaskCB *curTask = OsCurrTaskGet();
    if (curTask == NULL) {
        return NULL;
    }

    header->startTime = TraceuiGetTimeCycle();
    header->eventType = event;
    header->taskid = ConvertTaskId(curTask->taskId);
    header->cpuid = TraceuiGetCpuId();
    header->frameLen = (uint8_t)len;
    return header;
}

void TraceuiEventTaskSwitch(uint32_t newTaskId)
{
    TraceuiFrameTaskSwitch *frame = (TraceuiFrameTaskSwitch *)GenFrame(TRACEUI_EVENT_TASK_SWITCH, 0);
    if (frame == NULL) {
        return;
    }

    const LosTaskCB *curTask = OsCurrTaskGet();
    const LosTaskCB *newTask = OS_TCB_FROM_TID(newTaskId);
    if (curTask == NULL || newTask == NULL) {
        return;
    }

    frame->prePrio = ConvertPriority(curTask->priority);
    frame->preState = ConvertTaskStatus(curTask->taskStatus);
    frame->nextTaskId = ConvertTaskId(newTask->taskId);
    frame->nextPrio = ConvertPriority(newTask->priority);
}

void TraceuiEventTaskWakeup(uint32_t wakeTaskId)
{
    TraceuiFrameTaskWakeup *frame = (TraceuiFrameTaskWakeup *)GenFrame(TRACEUI_EVENT_TASK_WAKEUP, 0);
    if (frame == NULL) {
        return;
    }

    const LosTaskCB *wakeTask = OS_TCB_FROM_TID(wakeTaskId);
    if (wakeTask == NULL) {
        return;
    }

    frame->wakePrio = ConvertPriority(wakeTask->priority);
    frame->wakeTaskId = ConvertTaskId(wakeTask->taskId);
}

void TraceuiEventTraceBegin(const char *name)
{
    TraceuiFrameTraceBegin *frame = (TraceuiFrameTraceBegin *)GenFrame(TRACEUI_EVENT_TRACE_BEGIN, 0);
    if (frame == NULL) {
        return;
    }
    frame->name = (uintptr_t)name;
}

void TraceuiEventTraceBeginFormat(const char *name, const char *fmt, ...)
{
    char buf[TRACEUI_FRAME_LEN_MAX - sizeof(TraceuiFrameTraceBeginFormat)];
    va_list ap;
    va_start(ap, fmt);
    int fmtStrLen = vsprintf_s(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (fmtStrLen <= 0) {
        TRACEUI_ERR("format error!\n");
        return;
    }
    fmtStrLen++; // add '\0' len

    TraceuiFrameTraceBeginFormat *frame;
    frame = (TraceuiFrameTraceBeginFormat *)GenFrame(TRACEUI_EVENT_TRACE_BEGIN_FORMAT, fmtStrLen);
    if (frame == NULL) {
        return;
    }
    frame->name = (uintptr_t)name;
    (void)memcpy_s(frame->str, fmtStrLen, buf, fmtStrLen);
}

void TraceuiEventTraceEnd(void)
{
    TraceuiFrameTraceEnd *frame = (TraceuiFrameTraceEnd *)GenFrame(TRACEUI_EVENT_TRACE_END, 0);
    if (frame == NULL) {
        return;
    }
}

void TraceuiEventTraceAsyncBegin(const char *name, int cookie)
{
    TraceuiFrameTraceAsync *frame = (TraceuiFrameTraceAsync *)GenFrame(TRACEUI_EVENT_TRACE_ASYNC_BEGIN, 0);
    if (frame == NULL) {
        return;
    }
    frame->name = (uintptr_t)name;
    frame->cookie = cookie;
}

void TraceuiEventTraceAsyncEnd(const char *name, int cookie)
{
    TraceuiFrameTraceAsync *frame = (TraceuiFrameTraceAsync *)GenFrame(TRACEUI_EVENT_TRACE_ASYNC_END, 0);
    if (frame == NULL) {
        return;
    }
    frame->name = (uintptr_t)name;
    frame->cookie = cookie;
}

void TraceuiEventTraceInt(const char *name, int value)
{
    TraceuiFrameTraceInt *frame = (TraceuiFrameTraceInt *)GenFrame(TRACEUI_EVENT_TRACE_INT, 0);
    if (frame == NULL) {
        return;
    }
    frame->name = (uintptr_t)name;
    frame->value = value;
}

void TraceuiEventHwiEnter(uint32_t hwiNum)
{
    // skip timer irq: It happens too frequently, and takes little time.
    if (hwiNum == NUM_HAL_INTERRUPT_TIMER) {
        return;
    }

    TraceuiFrameHwiEnter *frame = (TraceuiFrameHwiEnter *)GenFrame(TRACEUI_EVENT_HWI_ENTER, 0);
    if (frame == NULL) {
        return;
    }

    frame->hwiNum = hwiNum;
}

void TraceuiEventHwiExit(uint32_t hwiNum)
{
    // skip timer irq: It happens too frequently, and takes little time.
    if (hwiNum == NUM_HAL_INTERRUPT_TIMER) {
        return;
    }

    TraceuiFrameHwiExit *frame = (TraceuiFrameHwiExit *)GenFrame(TRACEUI_EVENT_HWI_EXIT, 0);
    if (frame == NULL) {
        return;
    }

    frame->hwiNum = hwiNum;
}

void TraceuiEventLog(const char *fmt, ...)
{
    char buf[TRACEUI_FRAME_LEN_MAX - sizeof(TraceuiFrameLog)];
    va_list ap;
    va_start(ap, fmt);
    int fmtStrLen = vsprintf_s(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (fmtStrLen < 0) {
        TRACEUI_ERR("format error!\n");
        return;
    }
    fmtStrLen++; // add '\0' len

    TraceuiFrameLog *frame = (TraceuiFrameLog *)GenFrame(TRACEUI_EVENT_LOG, fmtStrLen);
    if (frame == NULL) {
        return;
    }
    (void)memcpy_s(frame->str, fmtStrLen, buf, fmtStrLen);
}
