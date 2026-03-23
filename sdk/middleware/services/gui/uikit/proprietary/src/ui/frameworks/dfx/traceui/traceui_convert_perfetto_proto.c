/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui frame convert to perfetto protobuf trace implementation
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include <stdio.h>
#include <string.h>
#include "securec.h"
#include "traceui_os.h"
#include "traceui_core.h"

/*
 * Perfetto UI use PID 0 for idle task, but liteos not.
 * So need convert liteos all pid = pid + PERFETTO_PID_INC,
 * and liteos idle task id force convert to 0
 */
#define PERFETTO_PID_INC      10000
#define PACKET_LEN_SIZE       2 // define protobuf max packet len use 2 * (8 - 1) = 14bit
#define MAX_ENC_BUF_SIZE      (TRACEUI_FRAME_LEN_MAX * 2) // alloc buffer size for encode trace

// protobuf wire type, reference: https://protobuf.dev/programming-guides/encoding/#structure
typedef enum {
    WIRE_VARINT = 0, // int32, int64, uint32, uint64, sint32, sint64, bool, enum
    WIRE_I64    = 1, // fixed64, sfixed64, double
    WIRE_LEN    = 2, // string, bytes, embedded messages, packed repeated fields
    WIRE_SGROUP = 3, // group start (deprecated)
    WIRE_EGROUP = 4, // group end (deprecated)
    WIRE_I32    = 5, // fixed32, sfixed32, float
} WireType;

// reference: external/perfetto/src/trace_processor/types/task_state.h
typedef enum {
    TASK_STATE_RUNNABLE = 0,
    TASK_STATE_INTERRUPTIBLE_SLEEP = 1,
    TASK_STATE_UNINTERRUPTIBLE_SLEEP = 2,
    TASK_STATE_STOPED = 4,
    TASK_STATE_TRACED = 8,
    TASK_STATE_EXIT_DEAD = 16,
    TASK_STATE_EXIT_ZOMBIE = 32,
    TASK_STATE_TASK_DEAD = 64,
    TASK_STATE_WAKE_KILL = 128,
    TASK_STATE_WAKING = 256,
    TASK_STATE_PARKED = 512,
    TASK_STATE_NO_LOAD = 1024,
    TASK_STATE_TASK_NEW = 2048,
    TASK_STATE_RUN_PLUS = 4096, // guess
    TASK_STATE_VALID = 0x8000,
} TaskStateId;

// perfetto proto struct. reference android code: external/perfetto/protos/perfetto/trace/perfetto_trace.proto
typedef enum {
    FTRACE_EVENT_PRINT_BUF  = 2,
} PrintFtraceEventId;

typedef struct {
    const char *buf; // FTRACE_EVENT_PRINT_BUF
} PrintFtraceEvent;

typedef enum {
    SCHED_SWITCH_PREV_COMM  = 1, // string
    SCHED_SWITCH_PREV_PID   = 2, // int32
    SCHED_SWITCH_PREV_PRIO  = 3, // int32
    SCHED_SWITCH_PREV_STATE = 4, // int64
    SCHED_SWITCH_NEXT_COMM  = 5, // string
    SCHED_SWITCH_NEXT_PID   = 6, // int32
    SCHED_SWITCH_NEXT_PRIO  = 7, // int32
} SchedSwitchFtraceEventId;

typedef struct {
    const char *prevComm;
    int32_t prevPid;
    int32_t prevPrio;
    int64_t prevState;
    const char *nextComm;
    int32_t nextPid;
    int32_t nextPrio;
} SchedSwitchFtraceEvent;

typedef enum {
    SCHED_WAKING_COMM       = 1,
    SCHED_WAKING_PID        = 2,
    SCHED_WAKING_PRIO       = 3,
    SCHED_WAKING_SUCCESS    = 4,
    SCHED_WAKING_TARGET_CPU = 5,
} SchedWakingFtraceEventId;

typedef struct {
    const char *comm;
    int32_t pid;
    int32_t prio;
    int32_t success;
    int32_t targetCpu;
} SchedWakingFtraceEvent;

typedef enum {
    IRQ_HANDLER_ENTRY_IRQ   = 1,
    IRQ_HANDLER_ENTRY_NAME  = 2,
} IrqHandlerEntryFtraceEventId;

typedef struct {
    int32_t irq; // IRQ_HANDLER_ENTRY_IRQ
    const char *name; // IRQ_HANDLER_ENTRY_NAME
} IrqHandlerEntryFtraceEvent;

typedef enum {
    IRQ_HANDLER_EXIT_IRQ     = 1,
    IRQ_HANDLER_EXIT_RET     = 2,
} IrqHandlerExitFtraceEventId;

typedef struct {
    int32_t irq; // IRQ_HANDLER_EXIT_IRQ
    int32_t ret; // IRQ_HANDLER_EXIT_RET
} IrqHandlerExitFtraceEvent;

typedef enum {
    FTRACE_EVENT_TIMESTAMP         = 1,
    FTRACE_EVENT_PID               = 2,
    FTRACE_EVENT_PRINT             = 3,
    FTRACE_EVENT_SCHED_SWITCH      = 4,
    FTRACE_EVENT_SCHED_WAKING      = 20,
    FTRACE_EVENT_IRQ_HANDLER_ENTRY = 36,
    FTRACE_EVENT_IRQ_HANDLER_EXIT  = 37,
} FtraceEventId;

typedef struct {
    uint64_t timestamp; // FTRACE_EVENT_TIMESTAMP
    uint32_t pid; // FTRACE_EVENT_PID
    FtraceEventId eventType;
    union {
        PrintFtraceEvent print; // FTRACE_EVENT_PRINT
        SchedSwitchFtraceEvent schedSwitch; // FTRACE_EVENT_SCHED_SWITCH
        SchedWakingFtraceEvent schedWaking; // FTRACE_EVENT_SCHED_WAKING
        IrqHandlerEntryFtraceEvent irqEntry; // FTRACE_EVENT_IRQ_HANDLER_ENTRY
        IrqHandlerExitFtraceEvent irqExit; // FTRACE_EVENT_IRQ_HANDLER_EXIT
    };
} FtraceEvent;

typedef enum {
    FTRACE_EVENT_BUNDLE_CPU   = 1,
    FTRACE_EVENT              = 2,
} FtraceEventBundleId;

typedef struct {
    uint32_t cpu; // FTRACE_EVENT_BUNDLE_CPU
    FtraceEvent event; // FTRACE_EVENT
} FtraceEventBundle;

typedef enum {
    CLOCK_SNAPSHOT_CLOCK_ID        = 1,
    CLOCK_SNAPSHOT_CLOCK_TIMESTAMP = 2,
} ClockSnapshotClockId;

typedef struct {
    uint32_t clockId; // CLOCK_SNAPSHOT_CLOCK_ID
    uint64_t timestamp; // CLOCK_SNAPSHOT_CLOCK_TIMESTAMP
} ClockSnapshotClock; // message Clock

typedef enum {
    BUILTIN_CLOCK_REALTIME         = 1,
    BUILTIN_CLOCK_REALTIME_COARSE  = 2,
    BUILTIN_CLOCK_MONOTONIC        = 3,
    BUILTIN_CLOCK_MONOTONIC_COARSE = 4,
    BUILTIN_CLOCK_MONOTONIC_RAW    = 5,
    BUILTIN_CLOCK_BOOT_TIME        = 6,
    BUILTIN_CLOCK_MAX
} BuiltinClockId;

typedef enum {
    CLOCK_SNAPSHOT_CLOCK          = 1,
    CLOCK_SNAPSHOT_BUILTIN_CLOCK  = 2,
} ClockSnapshotId;

typedef struct {
    uint32_t clockNum;
    ClockSnapshotClock *clock; // CLOCK_SNAPSHOT_CLOCK
    BuiltinClockId primaryTraceClock ; // CLOCK_SNAPSHOT_BUILTIN_CLOCK
} ClockSnapshot;

typedef enum {
    LID_DEFAULT  = 0,
    LID_RADIO    = 1,
    LID_EVENTS   = 2,
    LID_SYSTEM   = 3,
    LID_CRASH    = 4,
    LID_STATS    = 5,
    LID_SECURITY = 6,
    LID_KERNEL   = 7,
} ALogId;

typedef enum {
    PRIO_UNSPECIFIED = 0,
    PRIO_UNUSED = 1,
    PRIO_VERBOSE = 2,
    PRIO_DEBUG = 3,
    PRIO_INFO = 4,
    PRIO_WARN = 5,
    PRIO_ERROR = 6,
    PRIO_FATAL = 7,
} ALogPriority;

typedef enum {
    ALOG_EVENT_LOG_ID        = 1,
    ALOG_EVENT_PID           = 2,
    ALOG_EVENT_TID           = 3,
    ALOG_EVENT_UID           = 4,
    ALOG_EVENT_TIMESTAMP     = 5,
    ALOG_EVENT_TAG           = 6,
    ALOG_EVENT_PRIORITY      = 7,
    ALOG_EVENT_EVENT_MESSAGE = 8,
} ALogEventId; // perfetto_trace.proto: message LogEvent

typedef struct {
    ALogId logId; // ALOG_EVENT_LOG_ID
    int32_t pid; // ALOG_EVENT_PID
    int32_t tid; // ALOG_EVENT_TID
    int32_t uid; // ALOG_EVENT_UID
    uint64_t timestamp; // ALOG_EVENT_TIMESTAMP
    const char *tag; // ALOG_EVENT_TAG
    ALogPriority prio; // ALOG_EVENT_PRIORITY
    const char *message; // ALOG_EVENT_EVENT_MESSAGE
} ALogEvent;

typedef enum {
    ALOG_STATS_NUM_TOTAL   = 1,
    ALOG_STATS_NUM_FAILED  = 2,
    ALOG_STATS_NUM_SKIPPED = 3,
} ALogStatsId;

typedef struct {
    uint64_t numTotal;
    uint64_t numFailed;
    uint64_t numSkipped;
} ALogStats;

typedef enum {
    ALOG_EVENT       = 1,
    ALOG_STATS       = 2,
} ALogPacketId;

typedef struct {
    ALogPacketId type;
    union {
        ALogEvent events; // ALOG_EVENT
        ALogStats stats; // ALOG_STATS
    };
} ALogPacket;

typedef enum {
    FTRACE_EVENT_BUNDLE          = 1,
    CLOCK_SNAPSHOT               = 6,
    ALOG_PACKET                  = 39,
} TracePacketId;

typedef struct {
    TracePacketId packetType;
    union {
        FtraceEventBundle ftraceEvents; // FTRACE_EVENT_BUNDLE
        ClockSnapshot clockSnapshot; // CLOCK_SNAPSHOT
        ALogPacket log; // ALOG_PACKET
    };
} TracePacket;

typedef enum {
    TRACE_PACKET = 1,
} TraceId;

typedef struct {
    TracePacket packet; // TRACE_PACKET
} Trace;

typedef struct {
    uint8_t *buf;
    uint32_t len; // unused buffer len, 0 <= len <= size
    uint32_t size; // buffer size
} Buffer;

static uint32_t g_logCount; // all successed log count of TRACEUI_EVENT_LOG
static uint32_t g_traceTimeOffset = 0;
static bool g_isEncodeError;
static Buffer g_encTmpBuf; // avoid exceed the stack size

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// return used buf size
static uint32_t EncodeVarint(uint64_t value, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    uint64_t tmp = value;
    while (tmp > 0x7F && i + 1 < bufSize) {
        buf[i] = (uint8_t)((tmp & 0x7F) | 0x80);
        tmp >>= 7; // 7: varint only use low 7 bit for value
        i++;
    }
    if (tmp > 0x7F || i + 1 >= bufSize) {
        TRACEUI_ERR("buf too small!\n");
        g_isEncodeError = true;
        return 0;
    }
    buf[i] = (uint8_t)(tmp & 0x7F);
    i++;
    return i;
}

// return used buf size
static uint32_t EncodeVarintFixedSize(uint64_t value, uint8_t *buf, uint32_t bufSize, uint32_t typeSize)
{
    uint32_t i = 0;
    uint64_t tmp = value;

    if (typeSize > bufSize) {
        TRACEUI_ERR("buf too small!\n");
        g_isEncodeError = true;
        return 0;
    }
    while (i + 1 < typeSize) {
        buf[i] = (uint8_t)((tmp & 0x7F) | 0x80);
        i++;
        tmp >>= 7; // 7: varint only use low 7 bit for value
    }
    if (tmp > 0x7F) {
        TRACEUI_ERR("varint error! value %llu > fixed size: %u\n", (unsigned long long)tmp, typeSize);
        g_isEncodeError = true;
        return 0;
    }
    buf[i] = (uint8_t)(tmp & 0x7F);
    i++;
    return i;
}

// proto use TLV(Tag-Length-Value) format
static inline uint32_t EncodeTag(uint32_t field, WireType wireType, uint8_t *buf, uint32_t size)
{
    return EncodeVarint((field << 3) | wireType, buf, size); // 3: std message structure defined
}

/*
 * reference: https://protobuf.dev/programming-guides/proto2/#scalar
 * ProtoType  WireType
 * double     WIRE_I64
 * float      WIRE_I32
 * int32      WIRE_VARINT
 * int64      WIRE_VARINT
 * uint32     WIRE_VARINT
 * uint64     WIRE_VARINT
 * sint32     WIRE_VARINT + ZigZag
 * sint64     WIRE_VARINT + ZigZag
 * fixed32    WIRE_I32
 * fixed64    WIRE_I64
 * sfixed32   WIRE_I32
 * sfixed64   WIRE_I64
 * bool       WIRE_VARINT
 * string     WIRE_LEN
 * bytes      WIRE_LEN
 */
static inline uint32_t EncodeTagUint(uint32_t field, uint64_t value, uint8_t *buf, uint32_t size)
{
    uint32_t i = 0;
    i += EncodeTag(field, WIRE_VARINT, &buf[i], size - i);
    i += EncodeVarint(value, &buf[i], size - i);
    return g_isEncodeError ? 0 : i;
}

static inline uint32_t EncodeTagString(uint32_t field, const char *str, uint8_t *buf, uint32_t size)
{
    uint32_t strLen;
    uint32_t i = 0;

    i += EncodeTag(field, WIRE_LEN, &buf[i], size - i);
    strLen = strlen(str);
    i += EncodeVarint(strLen, &buf[i], size - i);
    if (memcpy_s(&buf[i], size - i, str, strLen) != EOK) {
        g_isEncodeError = true;
    }
    i += strLen;
    return g_isEncodeError ? 0 : i;
}

typedef uint32_t (*EncodeFn)(uintptr_t msg, uint8_t *buf, uint32_t bufSize);
static inline uint32_t EncodeTagMessage(uint32_t field, EncodeFn fn, uintptr_t msg, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    uint32_t lenOffset;
    uint32_t len;

    i += EncodeTag(field, WIRE_LEN, &buf[i], bufSize - i);

    // set len for place holder
    lenOffset = i;
    i += EncodeVarintFixedSize(0, &buf[lenOffset], bufSize - lenOffset, PACKET_LEN_SIZE);

    // call message process function
    len = fn(msg, &buf[i], bufSize - i);
    i += len;

    // reset len
    EncodeVarintFixedSize(len, &buf[lenOffset], bufSize - lenOffset, PACKET_LEN_SIZE);
    return g_isEncodeError ? 0 : i;
}

static uint32_t EncodePrintFtraceEvent(const PrintFtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagString(FTRACE_EVENT_PRINT_BUF, t->buf, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeSchedSwitchFtraceEvent(const SchedSwitchFtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagString(SCHED_SWITCH_PREV_COMM, t->prevComm, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_SWITCH_PREV_PID, t->prevPid, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_SWITCH_PREV_PRIO, t->prevPrio, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_SWITCH_PREV_STATE, t->prevState, &buf[i], bufSize - i);
    i += EncodeTagString(SCHED_SWITCH_NEXT_COMM, t->nextComm, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_SWITCH_NEXT_PID, t->nextPid, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_SWITCH_NEXT_PRIO, t->nextPrio, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeSchedWakingFtraceEvent(const SchedWakingFtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagString(SCHED_WAKING_COMM, t->comm, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_WAKING_PID, t->pid, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_WAKING_PRIO, t->prio, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_WAKING_SUCCESS, t->success, &buf[i], bufSize - i);
    i += EncodeTagUint(SCHED_WAKING_TARGET_CPU, t->targetCpu, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeIrqEntryFtraceEvent(const IrqHandlerEntryFtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(IRQ_HANDLER_ENTRY_IRQ, t->irq, &buf[i], bufSize - i);
    i += EncodeTagString(IRQ_HANDLER_ENTRY_NAME, t->name, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeIrqExitFtraceEvent(const IrqHandlerExitFtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(IRQ_HANDLER_EXIT_IRQ, t->irq, &buf[i], bufSize - i);
    i += EncodeTagUint(IRQ_HANDLER_EXIT_RET, t->ret, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeFtraceEvent(const FtraceEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(FTRACE_EVENT_TIMESTAMP, t->timestamp, &buf[i], bufSize - i);
    i += EncodeTagUint(FTRACE_EVENT_PID, t->pid, &buf[i], bufSize - i);
    if (t->eventType == FTRACE_EVENT_PRINT) {
        i += EncodeTagMessage(t->eventType, (EncodeFn)EncodePrintFtraceEvent, (uintptr_t)&t->print,
                              &buf[i], bufSize - i);
    } else if (t->eventType == FTRACE_EVENT_SCHED_SWITCH) {
        i += EncodeTagMessage(t->eventType, (EncodeFn)EncodeSchedSwitchFtraceEvent, (uintptr_t)&t->schedSwitch,
                              &buf[i], bufSize - i);
    } else if (t->eventType == FTRACE_EVENT_SCHED_WAKING) {
        i += EncodeTagMessage(t->eventType, (EncodeFn)EncodeSchedWakingFtraceEvent, (uintptr_t)&t->schedWaking,
                              &buf[i], bufSize - i);
    } else if (t->eventType == FTRACE_EVENT_IRQ_HANDLER_ENTRY) {
        i += EncodeTagMessage(t->eventType, (EncodeFn)EncodeIrqEntryFtraceEvent, (uintptr_t)&t->irqEntry,
                              &buf[i], bufSize - i);
    } else if (t->eventType == FTRACE_EVENT_IRQ_HANDLER_EXIT) {
        i += EncodeTagMessage(t->eventType, (EncodeFn)EncodeIrqExitFtraceEvent, (uintptr_t)&t->irqExit,
                              &buf[i], bufSize - i);
    } else {
        TRACEUI_ERR("unsupport ftrace type: %d\n", t->eventType);
        g_isEncodeError = true;
        return 0;
    }
    return i;
}

static uint32_t EncodeFtraceEventBundle(const FtraceEventBundle *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(FTRACE_EVENT_BUNDLE_CPU, t->cpu, &buf[i], bufSize - i);
    i += EncodeTagMessage(FTRACE_EVENT, (EncodeFn)EncodeFtraceEvent, (uintptr_t)&t->event, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeClockSnapshotClock(const ClockSnapshotClock *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(CLOCK_SNAPSHOT_CLOCK_ID, t->clockId, &buf[i], bufSize - i);
    i += EncodeTagUint(CLOCK_SNAPSHOT_CLOCK_TIMESTAMP, t->timestamp, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeClockSnapshot(const ClockSnapshot *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(CLOCK_SNAPSHOT_BUILTIN_CLOCK, t->primaryTraceClock, &buf[i], bufSize - i);
    for (uint32_t j = 0; j < t->clockNum; j++) {
        i += EncodeTagMessage(CLOCK_SNAPSHOT_CLOCK, (EncodeFn)EncodeClockSnapshotClock, (uintptr_t)&t->clock[j],
            &buf[i], bufSize - i);
    }
    return i;
}

static uint32_t EncodeALogEvent(const ALogEvent *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(ALOG_EVENT_LOG_ID, t->logId, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_EVENT_PID, t->pid, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_EVENT_TID, t->tid, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_EVENT_UID, t->uid, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_EVENT_TIMESTAMP, t->timestamp, &buf[i], bufSize - i);
    i += EncodeTagString(ALOG_EVENT_TAG, t->tag, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_EVENT_PRIORITY, t->prio, &buf[i], bufSize - i);
    i += EncodeTagString(ALOG_EVENT_EVENT_MESSAGE, t->message, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeALogStats(const ALogStats *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    i += EncodeTagUint(ALOG_STATS_NUM_TOTAL, t->numTotal, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_STATS_NUM_FAILED, t->numFailed, &buf[i], bufSize - i);
    i += EncodeTagUint(ALOG_STATS_NUM_SKIPPED, t->numSkipped, &buf[i], bufSize - i);
    return i;
}

static uint32_t EncodeALogPacket(const ALogPacket *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    if (t->type == ALOG_EVENT) {
        i += EncodeTagMessage(t->type, (EncodeFn)EncodeALogEvent, (uintptr_t)&t->events, &buf[i], bufSize - i);
    } else if (t->type == ALOG_STATS) {
        i += EncodeTagMessage(t->type, (EncodeFn)EncodeALogStats, (uintptr_t)&t->stats, &buf[i], bufSize - i);
    } else {
        TRACEUI_ERR("unsupport alog type: %d\n", t->type);
        g_isEncodeError = true;
        return 0;
    }
    return i;
}

static uint32_t EncodeTracePacket(const TracePacket *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    if (t->packetType == FTRACE_EVENT_BUNDLE) {
        i += EncodeTagMessage(t->packetType, (EncodeFn)EncodeFtraceEventBundle, (uintptr_t)&t->ftraceEvents,
            &buf[i], bufSize - i);
    } else if (t->packetType == CLOCK_SNAPSHOT) {
        i += EncodeTagMessage(t->packetType, (EncodeFn)EncodeClockSnapshot, (uintptr_t)&t->clockSnapshot,
            &buf[i], bufSize - i);
    } else if (t->packetType == ALOG_PACKET) {
        i += EncodeTagMessage(t->packetType, (EncodeFn)EncodeALogPacket, (uintptr_t)&t->log, &buf[i], bufSize - i);
    } else {
        TRACEUI_ERR("unsupport packetType: %d\n", t->packetType);
        g_isEncodeError = true;
        return 0;
    }
    return i;
}

static uint32_t EncodeTrace(const Trace *t, uint8_t *buf, uint32_t bufSize)
{
    uint32_t i = 0;
    g_isEncodeError = false;
    i += EncodeTagMessage(TRACE_PACKET, (EncodeFn)EncodeTracePacket, (uintptr_t)&t->packet, &buf[i], bufSize - i);
    return g_isEncodeError ? 0 : i;
}

static uint64_t CycleToNs(uint32_t cycle)
{
    return TraceuiCycleToNs(cycle - g_traceTimeOffset);
}

static uint32_t GetProtoPid(uint8_t taskId)
{
    if (TraceuiIsIdleTask(taskId)) {
        return 0;
    }
    return PERFETTO_PID_INC + taskId;
}

static bool BufInit(Buffer *b, uint32_t size)
{
    b->buf = malloc(size);
    if (b->buf == NULL) {
        TRACEUI_ERR("malloc error!\n");
        return false;
    }
    b->len = 0;
    b->size = size;
    return true;
}

static void BufDeinit(Buffer *b)
{
    if (b->buf != NULL) {
        free(b->buf);
        b->buf = NULL;
    }
    b->len = 0;
    b->size = 0;
}


static void BufReset(Buffer *b)
{
    b->len = 0;
}

static char *BufGet(Buffer *b, uint32_t size)
{
    char *buf;
    if (b->len + size > b->size) {
        TRACEUI_ERR("buf full!\n");
        g_isEncodeError = true;
        return NULL;
    }

    buf = (char *)&b->buf[b->len];
    b->len += size;
    return buf;
}

static char *BufCopyStr(Buffer *b, const char *str)
{
    uint32_t size = strlen(str) + 1;
    char *buf = BufGet(b, size);
    if (buf == NULL) {
        g_isEncodeError = true;
        return NULL;
    }

    if (strcpy_s(buf, size, str) != EOK) {
        TRACEUI_ERR("strcpy error!\n");
        g_isEncodeError = true;
        return NULL;
    }
    return buf;
}

static uint32_t GetProtoTaskStatus(TraceuiTaskStat stat)
{
    if (stat == TRACEUI_TASK_RUNNING) {
        return (uint32_t)(TASK_STATE_RUNNABLE | TASK_STATE_RUN_PLUS | TASK_STATE_VALID);
    }
    return (uint32_t)(TASK_STATE_INTERRUPTIBLE_SLEEP | TASK_STATE_VALID);
}

static void FillFtraceHeader(const TraceuiFrameHeader *frame, Trace *t, FtraceEventId eventId)
{
    t->packet.packetType = FTRACE_EVENT_BUNDLE;
    t->packet.ftraceEvents.cpu = frame->cpuid;
    t->packet.ftraceEvents.event.timestamp = CycleToNs(frame->startTime);
    t->packet.ftraceEvents.event.pid = GetProtoPid(frame->taskid);
    t->packet.ftraceEvents.event.eventType = eventId;
}

static bool TraceFrameToTrace(const TraceuiFrameHeader *frame, Trace *t)
{
    int ret = -1;
    uint32_t bufSize = TRACEUI_FRAME_LEN_MAX;
    char *buf = BufGet(&g_encTmpBuf, TRACEUI_FRAME_LEN_MAX);
    if (buf == NULL) {
        return false;
    }

    FillFtraceHeader(frame, t, FTRACE_EVENT_PRINT);
    if (frame->eventType == TRACEUI_EVENT_TRACE_BEGIN) {
        TraceuiFrameTraceBegin *f = (TraceuiFrameTraceBegin *)frame;
        ret = sprintf_s(buf, bufSize, "B|%d|%s", GetProtoPid(f->header.taskid), (const char *)f->name);
    } else if (frame->eventType == TRACEUI_EVENT_TRACE_BEGIN_FORMAT) {
        TraceuiFrameTraceBeginFormat *f = (TraceuiFrameTraceBeginFormat *)frame;
        ret = sprintf_s(buf, bufSize, "B|%d|%s%s", GetProtoPid(f->header.taskid), (const char *)f->name, f->str);
    } else if (frame->eventType == TRACEUI_EVENT_TRACE_END) {
        ret = sprintf_s(buf, bufSize, "E|%d", GetProtoPid(frame->taskid));
    } else if (frame->eventType == TRACEUI_EVENT_TRACE_ASYNC_BEGIN) {
        TraceuiFrameTraceAsync *f = (TraceuiFrameTraceAsync *)frame;
        ret = sprintf_s(buf, bufSize, "S|%d|%s|%d", GetProtoPid(f->header.taskid), (const char *)f->name, f->cookie);
    } else if (frame->eventType == TRACEUI_EVENT_TRACE_ASYNC_END) {
        TraceuiFrameTraceAsync *f = (TraceuiFrameTraceAsync *)frame;
        ret = sprintf_s(buf, bufSize, "F|%d|%s|%d", GetProtoPid(f->header.taskid), (const char *)f->name, f->cookie);
    } else if (frame->eventType == TRACEUI_EVENT_TRACE_INT) {
        TraceuiFrameTraceInt *f = (TraceuiFrameTraceInt *)frame;
        ret = sprintf_s(buf, bufSize, "C|%d|%s|%d", GetProtoPid(f->header.taskid), (const char *)f->name, f->value);
    }
    if (ret <= 0) {
        TRACEUI_ERR("frame to trace error! type: %u\n", frame->eventType);
        return false;
    }
    t->packet.ftraceEvents.event.print.buf = buf;
    return g_isEncodeError ? false : true;
}

static bool TaskSwitchFrameToTrace(const TraceuiFrameTaskSwitch *f, Trace *t)
{
    SchedSwitchFtraceEvent *event = &t->packet.ftraceEvents.event.schedSwitch;

    FillFtraceHeader(&f->header, t, FTRACE_EVENT_SCHED_SWITCH);
    event->prevComm = BufCopyStr(&g_encTmpBuf, TraceuiGetTaskName(f->header.taskid));
    event->prevPid = GetProtoPid(f->header.taskid);
    event->prevPrio = f->prePrio;
    event->prevState = GetProtoTaskStatus(f->preState);
    event->nextComm = BufCopyStr(&g_encTmpBuf, TraceuiGetTaskName(f->nextTaskId));
    event->nextPid = GetProtoPid(f->nextTaskId);
    event->nextPrio = f->nextPrio;
    return g_isEncodeError ? false : true;
}

static bool TaskWakeupFrameToTrace(TraceuiFrameTaskWakeup *f, Trace *t)
{
    SchedWakingFtraceEvent *event = &t->packet.ftraceEvents.event.schedWaking;

    FillFtraceHeader(&f->header, t, FTRACE_EVENT_SCHED_WAKING);
    event->comm = BufCopyStr(&g_encTmpBuf, TraceuiGetTaskName(f->wakeTaskId));
    event->pid = GetProtoPid(f->wakeTaskId);
    event->prio = f->wakePrio;
    event->success = 0;
    event->targetCpu = 0;
    return g_isEncodeError ? false : true;
}

static bool HwiEnterFrameToTrace(TraceuiFrameHwiEnter *f, Trace *t)
{
    IrqHandlerEntryFtraceEvent *event = &t->packet.ftraceEvents.event.irqEntry;

    FillFtraceHeader(&f->header, t, FTRACE_EVENT_IRQ_HANDLER_ENTRY);
    event->irq = f->hwiNum;
    event->name = BufCopyStr(&g_encTmpBuf, TraceuiGetIrqName(f->hwiNum));
    return g_isEncodeError ? false : true;
}

static bool HwiExitFrameToTrace(TraceuiFrameHwiExit *f, Trace *t)
{
    IrqHandlerExitFtraceEvent *event = &t->packet.ftraceEvents.event.irqExit;

    FillFtraceHeader(&f->header, t, FTRACE_EVENT_IRQ_HANDLER_EXIT);
    event->irq = f->hwiNum;
    event->ret = 0;
    return g_isEncodeError ? false : true;
}

static bool LogFrameToTrace(TraceuiFrameLog *f, Trace *t)
{
    t->packet.packetType = ALOG_PACKET;
    t->packet.log.type = ALOG_EVENT;
    t->packet.log.events.logId = LID_DEFAULT;
    t->packet.log.events.pid = GetProtoPid(f->header.taskid);
    t->packet.log.events.tid = GetProtoPid(f->header.taskid);
    t->packet.log.events.uid = 0;
    t->packet.log.events.timestamp = CycleToNs(f->header.startTime);
    t->packet.log.events.tag = BufCopyStr(&g_encTmpBuf, TraceuiGetTaskName(f->header.taskid));
    t->packet.log.events.prio = PRIO_INFO;
    t->packet.log.events.message = f->str;
    g_logCount++;
    return g_isEncodeError ? false : true;
}

static bool FrameToTrace(const TraceuiFrameHeader *frame, Trace *t)
{
    g_isEncodeError = false;
    switch (frame->eventType) {
        case TRACEUI_EVENT_TASK_SWITCH:
            return TaskSwitchFrameToTrace((TraceuiFrameTaskSwitch *)frame, t);
        case TRACEUI_EVENT_TASK_WAKEUP:
            return TaskWakeupFrameToTrace((TraceuiFrameTaskWakeup *)frame, t);
        case TRACEUI_EVENT_HWI_ENTER:
            return HwiEnterFrameToTrace((TraceuiFrameHwiEnter *)frame, t);
        case TRACEUI_EVENT_HWI_EXIT:
            return HwiExitFrameToTrace((TraceuiFrameHwiExit *)frame, t);
        case TRACEUI_EVENT_TRACE_BEGIN:
        case TRACEUI_EVENT_TRACE_BEGIN_FORMAT:
        case TRACEUI_EVENT_TRACE_END:
        case TRACEUI_EVENT_TRACE_ASYNC_BEGIN:
        case TRACEUI_EVENT_TRACE_ASYNC_END:
        case TRACEUI_EVENT_TRACE_INT:
            return TraceFrameToTrace(frame, t);
        case TRACEUI_EVENT_LOG:
            return LogFrameToTrace((TraceuiFrameLog *)frame, t);
        default:
            break;
    }
    TRACEUI_ERR("unsupport frame type: %d\n", frame->eventType);
    return false;
}

static uint32_t EncodeFrameContent(const TraceuiFrameHeader *frame, uint8_t *buf, uint32_t bufSize)
{
    Trace *t;

    BufReset(&g_encTmpBuf);
    t = (Trace *)BufGet(&g_encTmpBuf, sizeof(Trace));
    if (t == NULL) {
        return 0;
    }

    (void)memset_s(t, sizeof(Trace), 0, sizeof(Trace));
    if (!FrameToTrace(frame, t)) {
        return 0;
    }
    return EncodeTrace(t, buf, bufSize);
}


static uint32_t EncodeFullClockSnapshot(uint8_t *buf, uint32_t bufSize)
{
    Trace *t;
    ClockSnapshotClock *clocks;
    uint32_t count = 0;

    BufReset(&g_encTmpBuf);
    t = (Trace *)BufGet(&g_encTmpBuf, sizeof(Trace));
    clocks = (ClockSnapshotClock *)BufGet(&g_encTmpBuf, sizeof(ClockSnapshotClock) * BUILTIN_CLOCK_MAX);
    if (t == NULL || clocks == NULL) {
        return 0;
    }

    (void)memset_s(t, sizeof(Trace), 0, sizeof(Trace));
    for (BuiltinClockId i = BUILTIN_CLOCK_REALTIME; i <= BUILTIN_CLOCK_BOOT_TIME; i++) {
        clocks[count].clockId = (uint32_t)i;
        clocks[count].timestamp = 0;
        count++;
    }
    t->packet.packetType = CLOCK_SNAPSHOT;
    t->packet.clockSnapshot.clockNum = count;
    t->packet.clockSnapshot.clock = clocks;
    t->packet.clockSnapshot.primaryTraceClock = BUILTIN_CLOCK_BOOT_TIME;
    return EncodeTrace(t, buf, bufSize);
}

static uint32_t EncodeLogEventStatus(uint8_t *buf, uint32_t bufSize)
{
    Trace *t;

    BufReset(&g_encTmpBuf);
    t = (Trace *)BufGet(&g_encTmpBuf, sizeof(Trace));
    if (t == NULL) {
        return 0;
    }

    (void)memset_s(t, sizeof(Trace), 0, sizeof(Trace));
    t->packet.packetType = ALOG_PACKET;
    t->packet.log.type = ALOG_STATS;
    t->packet.log.stats.numTotal = g_logCount;
    t->packet.log.stats.numFailed = 0;
    t->packet.log.stats.numSkipped = 0;
    return EncodeTrace(t, buf, bufSize);
}

static bool StartConvert(const uint8_t *buf, uint32_t bufSize, FILE *fp)
{
    uint32_t walkIndex = 0;
    Buffer dstBuf;
    const TraceuiFrameHeader *frame = (const TraceuiFrameHeader *)buf;
    bool ret = false;

    if (!BufInit(&dstBuf, MAX_ENC_BUF_SIZE) || !BufInit(&g_encTmpBuf, MAX_ENC_BUF_SIZE)) {
        goto out;
    }

    // write prefix
    g_logCount = 0;
    g_traceTimeOffset = frame->startTime;
    dstBuf.len = EncodeFullClockSnapshot(dstBuf.buf, dstBuf.size);
    if (fwrite(dstBuf.buf, dstBuf.len, 1, fp) <= 0) {
        TRACEUI_ERR("fwrite failed!\n");
        goto out;
    }

    // write main trace
    while ((frame = TraceuiWalkBuf(buf, bufSize, &walkIndex)) != NULL) {
        dstBuf.len = EncodeFrameContent(frame, dstBuf.buf, dstBuf.size);
        if (dstBuf.len <= 0) {
            continue;
        }
        if (fwrite(dstBuf.buf, dstBuf.len, 1, fp) <= 0) {
            TRACEUI_ERR("fwrite failed!\n");
            goto out;
        }
    }

    // write tail
    dstBuf.len = EncodeLogEventStatus(dstBuf.buf, dstBuf.size);
    if (fwrite(dstBuf.buf, dstBuf.len, 1, fp) <= 0) {
        TRACEUI_ERR("fwrite failed!\n");
        goto out;
    }
    ret = true;

out:
    BufDeinit(&g_encTmpBuf);
    BufDeinit(&dstBuf);
    return ret;
}

bool TraceuiConvertToPerfettoProto(const uint8_t *buf, uint32_t bufSize, const char *outPath)
{
    FILE *fp = NULL;
    bool success = false;

    if (buf == NULL || outPath == NULL) {
        TRACEUI_ERR("parameter error! null!\n");
        goto out;
    }

    fp = fopen(outPath, "w");
    if (fp == NULL) {
        TRACEUI_ERR("open %s failed!\n", outPath);
        goto out;
    }
    success = StartConvert(buf, bufSize, fp);
    if (!success) {
        TRACEUI_ERR("convert %s failed!\n", outPath);
        goto out;
    }

out:
    if (fp != NULL) {
        fclose(fp);
    }
    return success;
}
