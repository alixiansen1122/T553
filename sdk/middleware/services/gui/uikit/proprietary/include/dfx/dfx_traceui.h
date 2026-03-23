/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: graphical performance trace analysis tool interface
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef GRAPHIC_TRACEUI_H
#define GRAPHIC_TRACEUI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#include "graphic_config.h"
extern "C" {
#endif

// basic
#if ENABLE_DFX_TRACEUI
#define TRACEUI_BEGIN(nameStaticStr) TraceuiEventTraceBegin(nameStaticStr)
#define TRACEUI_BEGIN_FORMAT(nameStaticStr, fmt, ...) TraceuiEventTraceBeginFormat(nameStaticStr, fmt, ##__VA_ARGS__)
#define TRACEUI_END() TraceuiEventTraceEnd()
#define TRACEUI_ASYNC_BEGIN(nameStaticStr, cookieInt) TraceuiEventTraceAsyncBegin(nameStaticStr, cookieInt)
#define TRACEUI_ASYNC_END(nameStaticStr, cookieInt) TraceuiEventTraceAsyncEnd(nameStaticStr, cookieInt)
#define TRACEUI_INT(nameStaticStr, valueInt) TraceuiEventTraceInt(nameStaticStr, valueInt)
#define TRACEUI_LOG(fmt, ...) TraceuiEventLog(fmt, ##__VA_ARGS__)
#else
#define TRACEUI_BEGIN(nameStaticStr)
#define TRACEUI_BEGIN_FORMAT(nameStaticStr, fmt, ...)
#define TRACEUI_END()
#define TRACEUI_ASYNC_BEGIN(nameStaticStr, cookiePtr)
#define TRACEUI_ASYNC_END(nameStaticStr, cookiePtr)
#define TRACEUI_INT(nameStaticStr, valueInt)
#define TRACEUI_LOG(fmt, ...)
#endif

// traceui core
typedef enum {
    TRACEUI_EVENT_TASK_SWITCH,
    TRACEUI_EVENT_TASK_WAKEUP,
    TRACEUI_EVENT_HWI_ENTER,
    TRACEUI_EVENT_HWI_EXIT,
    TRACEUI_EVENT_TRACE_BEGIN,
    TRACEUI_EVENT_TRACE_BEGIN_FORMAT,
    TRACEUI_EVENT_TRACE_END,
    TRACEUI_EVENT_TRACE_ASYNC_BEGIN,
    TRACEUI_EVENT_TRACE_ASYNC_END,
    TRACEUI_EVENT_TRACE_INT,
    TRACEUI_EVENT_LOG,

    TRACEUI_EVENT_MAX
} TraceuiEventType;

#define TRACEUI_EVENTS_ALL         0xFFFFFFFF
#define TRACEUI_EVENT_MASK(event)  (1 << (event))

/**
 * @brief Start record trace. If the recording exceeds the buffer size, subsequent data will be ignored.
 *
 * @param events TraceuiEventType mask. mayge TRACEUI_EVENTS_ALL.
 *                or: TRACEUI_EVENT_MASK(TRACEUI_EVENT_TASK_SWITCH) | TRACEUI_EVENT_MASK(TRACEUI_EVENT_LOG)
 * @param buf Record to the target buffer.
 * @param bufSize target buffer size.
 * @return true for success.
 */
bool TraceuiRecordStart(uint32_t events, uint8_t *buf, uint32_t bufSize);

/**
 * @brief Stop record trace.
 *
 * @return Length of recording.
 */
uint32_t TraceuiRecordStop(void); // return trace size

/**
 * @brief Convert recording buffer to perfetto protobuf trace format.
 *
 * @param buf Input buffer of recording.
 * @param bufSize Input buffer size.
 * @param outPath Output trace file path.
 * @return true for success.
 */
bool TraceuiConvertToPerfettoProto(const uint8_t *buf, uint32_t bufSize, const char *outPath);

/**
 * @brief Trace the beginning of a context. This is often used to time function execution.
 *
 * @param name Static string for identify the context. The pointer must remain valid before trace convert.
 */
void TraceuiEventTraceBegin(const char *name);

/**
 * @brief Trace the beginning of a context. This is often used to time function execution.
 *
 * @param name Static string for identify the context. The pointer must remain valid before trace convert.
 * @param fmt Format string like printf. Name and format string together for identify the context.
 */
void TraceuiEventTraceBeginFormat(const char *name, const char *fmt, ...);

/**
 * @brief Trace the end of a context. This should match up (and occur after) a corresponding *TraceBegin*.
 */
void TraceuiEventTraceEnd(void);

/**
 * @brief Trace the beginning of an asynchronous event. The name and cookie used to begin an event must be
 * used to end it.
 *
 * @param name Static string for identify the context. The pointer must remain valid before trace convert.
 * @param cookie Provides a unique identifier for distinguishing simultaneous events.
 */
void TraceuiEventTraceAsyncBegin(const char *name, int cookie);

/**
 * @brief Trace the end of an asynchronous event. This should have a corresponding *TraceAsyncBegin*.
 */
void TraceuiEventTraceAsyncEnd(const char *name, int cookie);

/**
 * @brief Traces an integer value.  name is used to identify the counter.
 * This can be used to track how a value changes over time.
 *
 * @param name Static string for identify the context. The pointer must remain valid before trace convert.
 * @param value The current value at this moment.
 */
void TraceuiEventTraceInt(const char *name, int value);

/**
 * @brief Traces a log. To save recording memory, please keep the log as small as possible.
 *
 * @param fmt Format string like printf.
 */
void TraceuiEventLog(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif // GRAPHIC_TRACEUI_H