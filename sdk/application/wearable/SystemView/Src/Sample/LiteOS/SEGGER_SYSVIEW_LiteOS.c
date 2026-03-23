/* ********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V2.52a                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_FreeRTOS.c
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 7947 $
*/
#include "los_base.h"
#include "los_task.h"
#include "los_config.h"
#include "los_task_pri.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_LiteOS.h"
#include "string.h" // Required for memset

#define PRODUCT_TSK_MAX_SUPPORT_NUM 32 /* max num task */
typedef UINT32 TickType_t;
#define configTICK_RATE_HZ ((TickType_t)1000)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
typedef struct SYSVIEW_FREERTOS_TASK_STATUS SYSVIEW_FREERTOS_TASK_STATUS;

struct SYSVIEW_FREERTOS_TASK_STATUS {
    U32 xHandle;
    const char *pcTaskName;
    unsigned uxCurrentPriority;
    U32 pxStack;
    unsigned uStackHighWaterMark;
};

static SYSVIEW_FREERTOS_TASK_STATUS _aTasks[SYSVIEW_LITEOS_MAX_NOF_TASKS];
static unsigned _NumTasks;

static inline U64 MC_GetTick(void)
{
    return uapi_tcxo_get_ms();
}
/* ********************************************************************
 *
 * _cbSendTaskList()
 *
 * Function description
 * This function is part of the link between FreeRTOS and SYSVIEW.
 * Called from SystemView when asked by the host, it uses SYSVIEW
 * functions to send the entire task list to the host.
 */
static void _cbSendTaskList(void)
{
    unsigned n;

    for (n = 0; n < _NumTasks; n++) {
#if INCLUDE_uxTaskGetStackHighWaterMark // Report Task Stack High Watermark
        _aTasks[n].uStackHighWaterMark = uxTaskGetStackHighWaterMark((TaskHandle_t)_aTasks[n].xHandle);
#endif
        SYSVIEW_SendTaskInfo((U32)_aTasks[n].xHandle, _aTasks[n].pcTaskName, (unsigned)_aTasks[n].uxCurrentPriority,
            (U32)_aTasks[n].pxStack, (unsigned)_aTasks[n].uStackHighWaterMark);
    }
}

/* ********************************************************************
 *
 * _cbGetTime()
 *
 * Function description
 * This function is part of the link between FreeRTOS and SYSVIEW.
 * Called from SystemView when asked by the host, returns the
 * current system time in micro seconds.
 */
static U64 _cbGetTime(void)
{
    U64 Time;

    Time = MC_GetTick();
    Time *= portTICK_PERIOD_MS;
    Time *= 1000;
    return Time;
}

/* ********************************************************************
 *
 * Global functions
 *
 * *********************************************************************
 */
/* ********************************************************************
 *
 * SYSVIEW_AddTask()
 *
 * Function description
 * Add a task to the internal list and record its information.
 */
void SYSVIEW_AddTask(U32 xHandle, const char *pcTaskName, unsigned uxCurrentPriority, U32 pxStack,
    unsigned uStackHighWaterMark)
{
    if (memcmp(pcTaskName, "IdleCore000", 11) == 0) {
        return;
    }

    if (_NumTasks >= SYSVIEW_LITEOS_MAX_NOF_TASKS) {
        SEGGER_SYSVIEW_Warn("SYSTEMVIEW: Could not record task information. Maximum number of tasks reached.");
        return;
    }

    _aTasks[_NumTasks].xHandle = xHandle;
    _aTasks[_NumTasks].pcTaskName = pcTaskName;
    _aTasks[_NumTasks].uxCurrentPriority = uxCurrentPriority;
    _aTasks[_NumTasks].pxStack = pxStack;
    _aTasks[_NumTasks].uStackHighWaterMark = uStackHighWaterMark;

    _NumTasks++;

    SYSVIEW_SendTaskInfo(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
}

/* ********************************************************************
 *
 * SYSVIEW_UpdateTask()
 *
 * Function description
 * Update a task in the internal list and record its information.
 */
void SYSVIEW_UpdateTask(U32 xHandle, const char *pcTaskName, unsigned uxCurrentPriority, U32 pxStack,
    unsigned uStackHighWaterMark)
{
    unsigned n;

    if (memcmp(pcTaskName, "IdleCore000", 11) == 0) {
        return;
    }

    for (n = 0; n < _NumTasks; n++) {
        if (_aTasks[n].xHandle == xHandle) {
            break;
        }
    }
    if (n < _NumTasks) {
        _aTasks[n].pcTaskName = pcTaskName;
        _aTasks[n].uxCurrentPriority = uxCurrentPriority;
        _aTasks[n].pxStack = pxStack;
        _aTasks[n].uStackHighWaterMark = uStackHighWaterMark;

        SYSVIEW_SendTaskInfo(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
    } else {
        SYSVIEW_AddTask(xHandle, pcTaskName, uxCurrentPriority, pxStack, uStackHighWaterMark);
    }
}

/* ********************************************************************
 *
 * SYSVIEW_DeleteTask()
 *
 * Function description
 * Delete a task from the internal list.
 */
void SYSVIEW_DeleteTask(U32 xHandle)
{
    unsigned n;

    if (_NumTasks == 0) {
        return; // Early out
    }
    for (n = 0; n < _NumTasks; n++) {
        if (_aTasks[n].xHandle == xHandle) {
            break;
        }
    }
    if (n == (_NumTasks - 1)) {
        //
        // Task is last item in list.
        // Simply zero the item and decrement number of tasks.
        //
        memset(&_aTasks[n], 0, sizeof(_aTasks[n]));
        _NumTasks--;
    } else if (n < _NumTasks) {
        //
        // Task is in the middle of the list.
        // Move last item to current position and decrement number of tasks.
        // Order of tasks does not really matter, so no need to move all following items.
        //
        _aTasks[n].xHandle = _aTasks[_NumTasks - 1].xHandle;
        _aTasks[n].pcTaskName = _aTasks[_NumTasks - 1].pcTaskName;
        _aTasks[n].uxCurrentPriority = _aTasks[_NumTasks - 1].uxCurrentPriority;
        _aTasks[n].pxStack = _aTasks[_NumTasks - 1].pxStack;
        _aTasks[n].uStackHighWaterMark = _aTasks[_NumTasks - 1].uStackHighWaterMark;
        memset(&_aTasks[_NumTasks - 1], 0, sizeof(_aTasks[_NumTasks - 1]));
        _NumTasks--;
    }
}

/* ********************************************************************
 *
 * SYSVIEW_SendTaskInfo()
 *
 * Function description
 * Record task information.
 */
void SYSVIEW_SendTaskInfo(U32 TaskID, const char *sName, unsigned Prio, U32 StackBase, unsigned StackSize)
{
    SEGGER_SYSVIEW_TASKINFO TaskInfo;

    memset(&TaskInfo, 0, sizeof(TaskInfo)); // Fill all elements with 0 to allow extending the structure in future
                                            // version without breaking the code
    TaskInfo.TaskID = TaskID;
    TaskInfo.sName = sName;
    TaskInfo.Prio = Prio;
    TaskInfo.StackBase = StackBase;
    TaskInfo.StackSize = StackSize;
    SEGGER_SYSVIEW_SendTaskInfo(&TaskInfo);
}

/* ********************************************************************
 *
 * SYSVIEW_RecordU32x4()
 *
 * Function description
 * Record an event with 4 parameters
 */
void SYSVIEW_RecordU32x4(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3)
{
    U8 aPacket[SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32];
    U8 *pPayload;
    //
    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);    // Prepare the packet for SystemView
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0); // Add the first parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1); // Add the second parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para2); // Add the third parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para3); // Add the fourth parameter to the packet
    //
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, Id); // Send the packet
}

/* ********************************************************************
 *
 * SYSVIEW_RecordU32x5()
 *
 * Function description
 * Record an event with 5 parameters
 */
void SYSVIEW_RecordU32x5(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4)
{
    U8 aPacket[SEGGER_SYSVIEW_INFO_SIZE + 5 * SEGGER_SYSVIEW_QUANTA_U32];
    U8 *pPayload;
    //
    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);    // Prepare the packet for SystemView
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0); // Add the first parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1); // Add the second parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para2); // Add the third parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para3); // Add the fourth parameter to the packet
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para4); // Add the fifth parameter to the packet
    //
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, Id); // Send the packet
}

/* ******************************************************************
 *
 * SEGGER_SYSVIEW_RecordU32_String
 *
 * Function description
 * Record an event with 1 parameter and one description string
 */
void SEGGER_SYSVIEW_RecordU32_String(U32 EventID, U32 Para0, const char *sDescription)
{
    U8 aPacket[SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN + 1];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, sDescription, SEGGER_SYSVIEW_MAX_STRING_LEN);
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);
}

/* ******************************************************************
 *
 * SEGGER_SYSVIEW_RecordU32x2_String
 *
 * Function description
 * Record an event with 2 parameter and one description string
 */
void SEGGER_SYSVIEW_RecordU32x2_String(U32 EventID, U32 Para0, U32 Para1, const char *sDescription)
{
    U8 aPacket[SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN + 1];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1);
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, sDescription, SEGGER_SYSVIEW_MAX_STRING_LEN);
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);
}

/* ******************************************************************
 *
 * SEGGER_SYSVIEW_RecordU32x3_String
 *
 * Function description
 * Record an event with 3 parameter and one description string
 */
void SEGGER_SYSVIEW_RecordU32x3_String(U32 EventID, U32 Para0, U32 Para1, U32 Para2, const char *sDescription)
{
    U8 aPacket[SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN + 1];
    U8 *pPayload;

    pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para2);
    pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, sDescription, SEGGER_SYSVIEW_MAX_STRING_LEN);
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, EventID);
}


/**
 * queue trace.
 */
#if (ENABLE_QUEUE_TRACE == 1)
void LOS_TRACE_QUEUE_CREATE(UINT32 queueId, UINT16 len, UINT16 msgSize, UINTPTR queue, UINT8 queueMemType)
{
    SEGGER_SYSVIEW_RecordU32x4(ID_OFFSET + ID_LOS_QUEUECREATE, queueId, (UINT32)len, (UINT32)queue,
        (UINT32)queueMemType);
}
void LOS_TRACE_QUEUE_DELETE(UINT32 queueId, UINT8 queueState, UINT16 readWriteableCnt)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_QUEUEDELETE, queueId, (UINT32)queueState, (UINT32)readWriteableCnt);
}
void LOS_TRACE_QUEUE_RW(UINT32 queueId, UINT16 queueSize, UINT32 bufferSize, UINT32 operateType, UINT16 readableCnt,
    UINT16 writeableCnt, UINT32 timeout)
{
    SEGGER_SYSVIEW_RecordU32x5(ID_OFFSET + ID_LOS_QUEUERW, queueId, operateType, (UINT32)readableCnt,
        (UINT32)writeableCnt, timeout);
}
#endif

/**
 * event trace.
 */
#if (ENABLE_EVENT_TRACE == 1)
void LOS_TRACE_EVENT_CREATE(UINTPTR eventCB)
{
    SEGGER_SYSVIEW_RecordString(ID_OFFSET + ID_LOS_EVENTINIT, "EventCreate.");
}
void LOS_TRACE_EVENT_READ(UINTPTR eventCB, UINT32 uwEventID, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    if (mode == LOS_WAITMODE_OR) {
        SEGGER_SYSVIEW_RecordU32x2_String(ID_OFFSET + ID_LOS_EVENTREAD, uwEventID, eventMask, "OR");
    } else if (mode == LOS_WAITMODE_AND) {
        SEGGER_SYSVIEW_RecordU32x2_String(ID_OFFSET + ID_LOS_EVENTREAD, uwEventID, eventMask, "AND");
    }
}
void LOS_TRACE_EVENT_WRITE(UINTPTR eventCB, UINT32 uwEventID, UINT32 events)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_EVENTWRITE, uwEventID, events);
}
void LOS_TRACE_EVENT_CLEAR(UINTPTR eventCB, UINT32 uwEventID, UINT32 events)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_EVENTCLEAR, uwEventID, events);
}
void LOS_TRACE_EVENT_DELETE(UINTPTR eventCB, UINT32 ret)
{
    SEGGER_SYSVIEW_RecordU32_String(ID_OFFSET + ID_LOS_EVENTDESTROY, ret, "EventDestroy");
}

void LOS_TRACE_EVENT_COND_READ(UINTPTR eventCB, UINT32 uwEventID)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_CON_READ, uwEventID);
}
void LOS_TRACE_EVENT_COND_WRITE(UINTPTR eventCB, UINT32 uwEventID)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_CON_WRITE, uwEventID);
}
#endif

/**
 * mutex trace.
 */
#if (ENABLE_EVENT_TRACE == 1)
void LOS_TRACE_MUX_CREATE(UINT32 muxId)
{
    SEGGER_SYSVIEW_RecordU32_String(ID_OFFSET + ID_LOS_MUXCREATE, muxId, "muxStat = mux_used");
}
void LOS_TRACE_MUX_POST(UINT32 muxHandle, UINT16 muxCount, UINT32 taskId)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_MUXPOST, muxHandle, (UINT32)muxCount, taskId);
}
void LOS_TRACE_MUX_PEND(UINT32 muxHandle, UINT16 muxCount, UINT32 taskId, UINT32 timeout)
{
    SEGGER_SYSVIEW_RecordU32x4(ID_OFFSET + ID_LOS_MUXPEND, muxHandle, (UINT32)muxCount, taskId, timeout);
}
void LOS_TRACE_MUX_DELETE(UINT32 muxHandle, UINT8 muxStat, UINT16 muxCount, UINT32 taskId)
{
    SEGGER_SYSVIEW_RecordU32x4(ID_OFFSET + ID_LOS_MUXDELETE, muxHandle, (UINT32)muxStat, (UINT32)muxCount, taskId);
}
#endif

/**
 * semphore trace.
 */
#if (ENABLE_SEM_TRACE == 1)
void LOS_TRACE_SEM_CREATE(UINT32 semId, UINT8 type, UINT16 count)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_SEMCREATE, semId, (UINT32)type, (UINT32)count);
}
void LOS_TRACE_SEM_DELETE(UINT32 semHandle, UINT32 ret)
{
    SEGGER_SYSVIEW_RecordU32x2_String(ID_OFFSET + ID_LOS_SEMDELETE, semHandle, ret, "semStat = OS_SEM_UNUSED");
}
void LOS_TRACE_SEM_PEND(UINT32 semHandle, UINT16 semCount, UINT32 timeout)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_SEMPEND, semHandle, (UINT32)semCount, timeout);
}
void LOS_TRACE_SEM_POST(UINT32 semHandle, UINT8 semType, UINT16 semCount)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_SEMPOST, semHandle, (UINT32)semType, (UINT32)semCount);
}
#endif

/**
 * memory alloc&free trace.
 */
#if (ENABLE_MEM_TRACE == 1)
void LOS_TRACE_MEM_INFO_REQ(VOID *pool)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_MEMINIT, (UINT32)pool);
}
void LOS_TRACE_MEM_ALLOC(VOID *pool, UINTPTR ptr, UINT32 size)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_MEMALLOC, (UINT32)pool, (UINT32)ptr, size);
}
void LOS_TRACE_MEM_FREE(VOID *pool, UINTPTR ptr)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_MEMFREE, (UINT32)pool, (UINT32)ptr);
}
void LOS_TRACE_MEM_REALLOC(VOID *pool, UINTPTR ptr, UINT32 size)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_MEMREALLOC, (UINT32)pool, (UINT32)ptr, size);
}
void LOS_TRACE_MEM_ALLOC_ALIGN(VOID *pool, UINTPTR ptr, UINT32 size, UINT32 boundary)
{
    SEGGER_SYSVIEW_RecordU32x4(ID_OFFSET + ID_LOS_MEMALLOCALIGN, (UINT32)pool, (UINT32)ptr, (UINT32)size,
        (UINT32)boundary);
}
#endif

/**
 * task trace.
 */
#if (ENABLE_TASK_TRACE == 1)
#ifdef LOSCFG_KERNEL_SMP
void LOS_TRACE_TASK_SIGNAL(UINT32 taskId, UINT32 mpSignal)
{
    ;
}
#endif
// void LOS_TRACE_TASK_DELAY(UINT32 tick);
void LOS_TRACE_TASK_RESUME(UINT32 taskId, UINT16 taskStatus, UINT16 priority)
{
    LosTaskCB *newTask = OS_TCB_FROM_TID(taskId);
    SEGGER_SYSVIEW_OnTaskStartReady((UINT32)newTask);
}
void LOS_TRACE_TASK_DELETE(UINT32 taskId, UINT16 taskStatus, UINT32 usrStack)
{
    LosTaskCB *newTask = OS_TCB_FROM_TID(taskId);
    SEGGER_SYSVIEW_RecordU32_String(ID_OFFSET + ID_LOS_TASK_DELETE, taskId, newTask->taskName);
    SYSVIEW_DeleteTask((UINT32)newTask);
}
void LOS_TRACE_TASK_PRIOSET(UINT32 taskId, UINT16 taskStatus, UINT16 priority, UINT16 taskPrio)
{
    LosTaskCB *newTask = OS_TCB_FROM_TID(taskId);
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_TASK_PRIOSET, SEGGER_SYSVIEW_ShrinkId((U32)newTask), priority);
    SYSVIEW_UpdateTask((UINT32)newTask, &(newTask->taskName[0]), (UINT32)priority, (UINT32)newTask->stackPointer, 0);
}

// sched trace
void LOS_TRACE_TASK_SWITCH(UINT32 taskId, UINT16 runPriority, UINT16 curTaskStatus, UINT16 newPriority,
    UINT16 newTaskStatus)
{
    LosTaskCB *newTask = OS_TCB_FROM_TID(taskId);
    if (memcmp(newTask->taskName, "IdleCore000", 11) == 0) {
        SEGGER_SYSVIEW_OnIdle();
    } else {
        SEGGER_SYSVIEW_OnTaskStartExec((UINT32)newTask);
    }
}
#ifndef LOSCFG_SCHED_LATENCY
void LOS_TRACE_TASK_SIGNAL(UINT32 taskId, UINT32 schedFlag) {}
#endif
void LOS_TRACE_TASK_SUSPEND(UINT32 taskId, UINT16 taskStatus, UINT32 currTaskId)
{
    LosTaskCB *newTask = OS_TCB_FROM_TID(taskId);
    SEGGER_SYSVIEW_OnTaskStopReady((UINT32)newTask, ((3u << 3) | 3));
}

void LOS_TRACE_MOVED_TASK_TO_READY_STATE(const LosTaskCB *pstTaskCB)
{
    SEGGER_SYSVIEW_OnTaskStartReady((UINT32)pstTaskCB);
}
void LOS_TRACE_MOVED_TASK_TO_DELAYED_LIST(const LosTaskCB *pstTaskCB)
{
    ;
}
void LOS_TRACE_MOVED_TASK_TO_SUSPENDED_LIST(const LosTaskCB *pstTaskCB)
{
    ;
}

void LOS_TRACE_ISR_EXIT_TO_SCHEDULER(void)
{
    ;
}
void LOS_TRACE_ISR_ENTER(U32 hwiIndex)
{
    ;
}
void LOS_TRACE_ISR_EXIT(U32 hwiIndex)
{
    ;
}
#endif

/**
 * swtmr trace.
 */
#if (ENABLE_SWTMR_TRACE == 1)
void LOS_TRACE_SWTMR_EXPIRED(UINT16 timerId)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_SWTMR_EXPIRED, (UINT32)timerId);
}
void LOS_TRACE_SWTMR_CREATE(UINT16 timerId)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_SWTMR_CREATE, (UINT32)timerId);
}
void LOS_TRACE_SWTMR_START(UINT16 timerId, UINT8 mode, UINT8 overrun, UINT32 interval, UINT32 expiry)
{
    SEGGER_SYSVIEW_RecordU32x5(ID_OFFSET + ID_LOS_SWTMR_START, (UINT32)timerId, (UINT32)mode, (UINT32)overrun,
        (UINT32)interval, (UINT32)expiry);
}
void LOS_TRACE_SWTMR_STOP(UINT16 timerId)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_SWTMR_STOP, (UINT32)timerId);
}
void LOS_TRACE_SWTMR_DELETE(UINT16 timerId)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_SWTMR_DELETE, (UINT32)timerId);
}
#endif

/**
 * hwi trace.
 */
// Machine timer interrupt too frequency
#define OS_TICK_INT_NUM_SYSTEM_VIEW 7
void LOS_TRACE_HWI_RESPONSE_IN(UINT32 hwiNum)
{
    if (hwiNum == OS_TICK_INT_NUM_SYSTEM_VIEW) {
        return;
    }
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + LOS_TRACE_HWI_RESPONSE_IN, hwiNum);
    SEGGER_SYSVIEW_RecordEnterISR();
}
void LOS_TRACE_HWI_RESPONSE_OUT(UINT32 hwiNum)
{
    if (hwiNum == OS_TICK_INT_NUM_SYSTEM_VIEW) {
        return;
    }
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_RESPONSE_OUT, hwiNum);
    SEGGER_SYSVIEW_RecordExitISR();
}
#if (ENABLE_HWI_TRACE == 1)
void LOS_TRACE_HWI_CREATE(UINT32 hwiNum, HWI_PRIOR_T hwiPrio, HWI_MODE_T hwiMode, UINTPTR hwiHandler)
{
    SEGGER_SYSVIEW_RecordU32x4(ID_OFFSET + ID_LOS_HWI_CREATE, hwiNum, (UINT32)hwiPrio, (UINT32)hwiMode,
        (UINT32)hwiHandler);
}
void LOS_TRACE_HWI_CREATE_NODE(UINT32 hwiNum, UINTPTR pDevId, UINT32 ret)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_HWI_CREATE_NODE, (UINT32)pDevId, ret);
}
void LOS_TRACE_HWI_DELETE(UINT32 hwiNum)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_DELETE, hwiNum);
}
void LOS_TRACE_HWI_DELETE_NODE(UINT32 hwiNum, UINTPTR pDevId, UINT32 ret)
{
    SEGGER_SYSVIEW_RecordU32x3(ID_OFFSET + ID_LOS_HWI_DELETE_NODE, hwiNum, (UINT32)pDevId, ret);
}
void LOS_TRACE_HWI_TRIGGER(UINT32 hwiNum)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_TRIGGER, hwiNum);
}
void LOS_TRACE_HWI_ENABLE(UINT32 hwiNum)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_ENABLE, hwiNum);
}
void LOS_TRACE_HWI_DISABLE(UINT32 hwiNum)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_DISABLE, hwiNum);
}
void LOS_TRACE_HWI_CLEAR(UINT32 hwiNum)
{
    SEGGER_SYSVIEW_RecordU32(ID_OFFSET + ID_LOS_HWI_CLEAR, hwiNum);
}
void LOS_TRACE_HWI_SETPRI(UINT32 hwiNum, HWI_PRIOR_T priority)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_HWI_SETPRI, hwiNum, (UINT32)priority);
}
void LOS_TRACE_HWI_SETAFFINITY(UINT32 hwiNum, UINT32 cpuMask)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_HWI_SETAFFINITY, hwiNum, cpuMask);
}
void LOS_TRACE_HWI_SENDIPI(UINT32 hwiNum, UINT32 cpuMask)
{
    SEGGER_SYSVIEW_RecordU32x2(ID_OFFSET + ID_LOS_HWI_SENDIPI, hwiNum, cpuMask);
}
#endif
/* ********************************************************************
 *
 * Public API structures
 *
 * *********************************************************************
 */
// Callbacks provided to SYSTEMVIEW by FreeRTOS
const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI = {
    _cbGetTime,
    _cbSendTaskList,
};

/* ************************** End of file *************************** */
