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

File    : SEGGER_SYSVIEW_LiteOS.h
Purpose : Interface between FreeRTOS and SystemView.
Revision: $Rev: 7745 $

Notes:
  (1) Include this file at the end of FreeRTOSConfig.h
*/

#ifndef SYSVIEW_LITEOS_H
#define SYSVIEW_LITEOS_H

#include "SEGGER_SYSVIEW.h"
#include "los_event.h"
#include "los_mux.h"
#include "los_queue.h"
#include "los_sem.h"
#include "los_task.h"
#include "los_task_base.h"

/**
 * Configure the modules to be traced.
 */
#define ENABLE_TASK_TRACE 1
#define ENABLE_QUEUE_TRACE 0
#define ENABLE_EVENT_TRACE 0
#define ENABLE_MUX_TRACE 0
#define ENABLE_SEM_TRACE 0
#define ENABLE_MEM_TRACE 0
#define ENABLE_HWI_TRACE 0
#define ENABLE_SWTMR_TRACE 0

/*
 * Several functions take an TaskHandle_t parameter that can optionally be NULL,
 * where NULL is used to indicate that the handle of the currently executing
 * task should be used in place of the parameter.  This macro simply checks to
 * see if the parameter is NULL and returns a pointer to the appropriate TCB.
 */

#define SYSVIEW_LITEOS_MAX_NOF_TASKS PRODUCT_TSK_MAX_SUPPORT_NUM
#define traceMOVED_TASK_TO_READY_STATE(pstTaskCB) SEGGER_SYSVIEW_OnTaskStartReady((U32)pstTaskCB)
#define traceREADDED_TASK_TO_READY_STATE(pxTCB)
#define traceMOVED_TASK_TO_DELAYED_LIST(pstTaskCB) SEGGER_SYSVIEW_OnTaskStopReady((U32)pstTaskCB, (1u << 2))
#define traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST(pstTaskCB) SEGGER_SYSVIEW_OnTaskStopReady((U32)pstTaskCB, (1u << 2))
#define traceMOVED_TASK_TO_SUSPENDED_LIST(pstTaskCB) SEGGER_SYSVIEW_OnTaskStopReady((U32)pstTaskCB, ((3u << 3) | 3))
#define traceISR_EXIT_TO_SCHEDULER() SEGGER_SYSVIEW_RecordExitISRToScheduler()
#define traceISR_EXIT() SEGGER_SYSVIEW_RecordExitISR()
#define traceISR_ENTER() SEGGER_SYSVIEW_RecordEnterISR()
#if (portSTACK_GROWTH < 0)
#define traceTASK_CREATE(pstTaskCB)                                                            \
    if (pstTaskCB != NULL) {                                                                   \
        SEGGER_SYSVIEW_OnTaskCreate((U32)pstTaskCB);                                           \
        SYSVIEW_AddTask((U32)pstTaskCB, (pstTaskCB->taskName), pstTaskCB->priority,            \
            (U32)(pstTaskCB->topOfStack - pstTaskCB->stackSize), ((U32)pstTaskCB->stackSize)); \
    }
#else
#define traceTASK_CREATE(pstTaskCB)                                                            \
    if (pstTaskCB != NULL) {                                                                   \
        SEGGER_SYSVIEW_OnTaskCreate((U32)pstTaskCB);                                           \
        SYSVIEW_AddTask((U32)pstTaskCB, (pstTaskCB->taskName), pstTaskCB->priority,            \
            (U32)(pstTaskCB->topOfStack + pstTaskCB->stackSize), (U32)(pstTaskCB->stackSize)); \
    }
#endif

/* ********************************************************************
 *
 * Defines, fixed
 *
 * *********************************************************************
 */
#define ID_OFFSET (32u)

#define ID_LOS_TASK_SIGNAL (1u)
#define ID_LOS_TASK_DELETE (2u)
#define ID_LOS_TASK_RESUME (3u)
#define ID_LOS_TASK_PRIOSET (4u)

#define ID_LOS_TASK_SWITCH (5u)
#define ID_LOS_TASK_SUSPEND (6u)

#define ID_LOS_QUEUECREATE (20u)
#define ID_LOS_QUEUEDELETE (21u)
#define ID_LOS_QUEUEREAD (22u)
#define ID_LOS_QUEUEWRITE (23u)
#define ID_LOS_QUEUERW (24u)

#define ID_LOS_EVENTINIT (30u)
#define ID_LOS_EVENTDESTROY (31u)
#define ID_LOS_EVENTREAD (32u)
#define ID_LOS_EVENTWRITE (33u)
#define ID_LOS_EVENTCLEAR (34u)
#define ID_LOS_CON_READ (35u)
#define ID_LOS_CON_WRITE (36u)

#define ID_LOS_MUXCREATE (40u)
#define ID_LOS_MUXDELETE (41u)
#define ID_LOS_MUXPEND (42u)
#define ID_LOS_MUXPOST (43u)

#define ID_LOS_SEMCREATE (50u)
#define ID_LOS_SEMDELETE (51u)
#define ID_LOS_SEMPEND (52u)
#define ID_LOS_SEMPOST (53u)

#define ID_LOS_MEMINIT (60u)
#define ID_LOS_MEMDEINIT (61u)
#define ID_LOS_MEMALLOC (62u)
#define ID_LOS_MEMFREE (63u)
#define ID_LOS_MEMREALLOC (64u)
#define ID_LOS_MEMALLOCALIGN (65u)

#define ID_LOS_SWTMR_CREATE (70u)
#define ID_LOS_SWTMR_START (71u)
#define ID_LOS_SWTMR_STOP (72u)
#define ID_LOS_SWTMR_EXPIRED (73u)
#define ID_LOS_SWTMR_DELETE (74u)

#define ID_LOS_HWI_CREATE (80u)
#define ID_LOS_HWI_DELETE (81u)
#define ID_LOS_HWI_CREATE_NODE (82u)
#define ID_LOS_HWI_DELETE_NODE (83u)
#define ID_LOS_HWI_TRIGGER (84u)
#define ID_LOS_HWI_ENABLE (85u)
#define ID_LOS_HWI_DISABLE (86u)
#define ID_LOS_HWI_CLEAR (87u)
#define ID_LOS_HWI_SETPRI (88u)
#define ID_LOS_HWI_HWI_RESPONSE_IN (89u)
#define ID_LOS_HWI_RESPONSE_OUT (90u)
#define ID_LOS_HWI_SETAFFINITY (91u)
#define ID_LOS_HWI_SENDIPI (92u)

/* ********************************************************************
 *
 * API functions
 *
 * *********************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif
void SYSVIEW_AddTask(U32 xHandle, const char *pcTaskName, unsigned uxCurrentPriority, U32 pxStack,
    unsigned uStackHighWaterMark);
void SYSVIEW_UpdateTask(U32 xHandle, const char *pcTaskName, unsigned uxCurrentPriority, U32 pxStack,
    unsigned uStackHighWaterMark);
void SYSVIEW_DeleteTask(U32 xHandle);
void SYSVIEW_SendTaskInfo(U32 TaskID, const char *sName, unsigned Prio, U32 StackBase, unsigned StackSize);
void SYSVIEW_RecordU32x4(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3);
void SYSVIEW_RecordU32x5(unsigned Id, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4);

/**
 * queue trace.
 */
#if (ENABLE_QUEUE_TRACE == 1)
void LOS_TRACE_QUEUE_CREATE(UINT32 queueId, UINT16 len, UINT16 msgSize, UINTPTR queue, UINT8 queueMemType);
void LOS_TRACE_QUEUE_DELETE(UINT32 queueId, UINT8 queueState, UINT16 readWriteableCnt);
void LOS_TRACE_QUEUE_RW(UINT32 queueId, UINT16 queueSize, UINT32 bufferSize, UINT32 operateType, UINT16 readableCnt,
    UINT16 writeableCnt, UINT32 timeout);
#else
#define LOS_TRACE_QUEUE_CREATE(...)
#define LOS_TRACE_QUEUE_DELETE(...)
#define LOS_TRACE_QUEUE_RW(...)
#endif

/**
 * event trace.
 */
#if (ENABLE_EVENT_TRACE == 1)
void LOS_TRACE_EVENT_CREATE(UINTPTR eventCB);
void LOS_TRACE_EVENT_READ(UINTPTR eventCB, UINT32 uwEventID, UINT32 eventMask, UINT32 mode, UINT32 timeout);
void LOS_TRACE_EVENT_WRITE(UINTPTR eventCB, UINT32 uwEventID, UINT32 events);
void LOS_TRACE_EVENT_CLEAR(UINTPTR eventCB, UINT32 uwEventID, UINT32 events);
void LOS_TRACE_EVENT_DELETE(UINTPTR eventCB, UINT32 ret);
void LOS_TRACE_EVENT_COND_READ(UINTPTR eventCB, UINT32 uwEventID);
void LOS_TRACE_EVENT_COND_WRITE(UINTPTR eventCB, UINT32 uwEventID);
#else
#define LOS_TRACE_EVENT_CREATE(...)
#define LOS_TRACE_EVENT_READ(...)
#define LOS_TRACE_EVENT_WRITE(...)
#define LOS_TRACE_EVENT_CLEAR(...)
#define LOS_TRACE_EVENT_DELETE(...)
#define LOS_TRACE_EVENT_COND_READ(...)
#define LOS_TRACE_EVENT_COND_WRITE(...)
#endif
/**
 * mutex trace.
 */
#if (ENABLE_MUX_TRACE == 1)
void LOS_TRACE_MUX_CREATE(UINT32 muxId);
void LOS_TRACE_MUX_POST(UINT32 muxHandle, UINT16 muxCount, UINT32 taskId);
void LOS_TRACE_MUX_PEND(UINT32 muxHandle, UINT16 muxCount, UINT32 taskId, UINT32 timeout);
void LOS_TRACE_MUX_DELETE(UINT32 muxHandle, UINT8 muxStat, UINT16 muxCount, UINT32 taskId);
#else
#define LOS_TRACE_MUX_CREATE(...)
#define LOS_TRACE_MUX_POST(...)
#define LOS_TRACE_MUX_PEND(...)
#define LOS_TRACE_MUX_DELETE(...)
#endif
/**
 * semphore trace.
 */
#if (ENABLE_SEM_TRACE == 1)
void LOS_TRACE_SEM_CREATE(UINT32 semId, UINT8 type, UINT16 count);
void LOS_TRACE_SEM_DELETE(UINT32 semHandle, UINT32 ret);
void LOS_TRACE_SEM_PEND(UINT32 semHandle, UINT16 semCount, UINT32 timeout);
void LOS_TRACE_SEM_POST(UINT32 semHandle, UINT8 semType, UINT16 semCount);
#else
#define LOS_TRACE_SEM_CREATE(...)
#define LOS_TRACE_SEM_DELETE(...)
#define LOS_TRACE_SEM_PEND(...)
#define LOS_TRACE_SEM_POST(...)
#endif

/**
 * memory alloc&free trace.
 */
#if (ENABLE_MEM_TRACE == 1)
void LOS_TRACE_MEM_INFO_REQ(VOID *pool);
void LOS_TRACE_MEM_ALLOC(VOID *pool, UINTPTR ptr, UINT32 size);
void LOS_TRACE_MEM_FREE(VOID *pool,  UINTPTR ptr);
void LOS_TRACE_MEM_REALLOC(VOID *pool, UINTPTR ptr, UINT32 size);
void LOS_TRACE_MEM_ALLOC_ALIGN(VOID *pool, UINTPTR ptr, UINT32 size, UINT32 boundary);
#else
#define LOS_TRACE_MEM_INFO_REQ(...)
#define LOS_TRACE_MEM_ALLOC(...)
#define LOS_TRACE_MEM_FREE(...)
#define LOS_TRACE_MEM_REALLOC(...)
#define LOS_TRACE_MEM_ALLOC_ALIGN(...)
#endif
/**
 * task trace.
 */
#if (ENABLE_TASK_TRACE == 1)
#ifdef LOSCFG_KERNEL_SMP
void LOS_TRACE_TASK_SIGNAL(UINT32 taskId, UINT32 mpSignal);
#endif
// void LOS_TRACE_TASK_DELAY(UINT32 tick);
void LOS_TRACE_TASK_RESUME(UINT32 taskId, UINT16 taskStatus, UINT16 priority);
void LOS_TRACE_TASK_DELETE(UINT32 taskId, UINT16 taskStatus, UINT32 usrStack);
void LOS_TRACE_TASK_PRIOSET(UINT32 taskId, UINT16 taskStatus, UINT16 priority, UINT16 taskPrio);
// sched trace
void LOS_TRACE_TASK_SWITCH(UINT32 taskId, UINT16 runPriority, UINT16 curTaskStatus, UINT16 newPriority,
    UINT16 newTaskStatus);
#ifndef LOSCFG_SCHED_LATENCY
void LOS_TRACE_TASK_SIGNAL(UINT32 taskId, UINT32 schedFlag);
#endif
void LOS_TRACE_TASK_SUSPEND(UINT32 taskId, UINT16 taskStatus, UINT32 currTaskId);
void LOS_TRACE_TASK_SWITCHED_IN(void);
void LOS_TRACE_MOVED_TASK_TO_READY_STATE(const LosTaskCB *pstTaskCB);
void LOS_TRACE_MOVED_TASK_TO_DELAYED_LIST(const LosTaskCB *pstTaskCB);
void LOS_TRACE_MOVED_TASK_TO_SUSPENDED_LIST(const LosTaskCB *pstTaskCB);
void LOS_TRACE_ISR_EXIT_TO_SCHEDULER(void);
void LOS_TRACE_ISR_ENTER(U32 hwiIndex);
void LOS_TRACE_ISR_EXIT(U32 hwiIndex);
#else
#define LOS_TRACE_TASK_SIGNAL(...)
#define LOS_TRACE_TASK_RESUME(...)
#define LOS_TRACE_TASK_DELETE(...)
#define LOS_TRACE_TASK_PRIOSET(...)
#define LOS_TRACE_TASK_SWITCH(...)
#define LOS_TRACE_TASK_SUSPEND(...)
#endif
/**
 * swtmr trace.
 */
#if (ENABLE_SWTMR_TRACE == 1)
void LOS_TRACE_SWTMR_EXPIRED(UINT16 timerId);
void LOS_TRACE_SWTMR_CREATE(UINT16 timerId);
void LOS_TRACE_SWTMR_START(UINT16 timerId, UINT8 mode, UINT8 overrun, UINT32 interval, UINT32 expiry);
void LOS_TRACE_SWTMR_STOP(UINT16 timerId);
void LOS_TRACE_SWTMR_DELETE(UINT16 timerId);
#else
#define LOS_TRACE_SWTMR_EXPIRED(...)
#define LOS_TRACE_SWTMR_CREATE(...)
#define LOS_TRACE_SWTMR_START(...)
#define LOS_TRACE_SWTMR_STOP(...)
#define LOS_TRACE_SWTMR_DELETE(...)
#endif

/**
 * hwi trace.
 */
void LOS_TRACE_HWI_RESPONSE_IN(UINT32 hwiNum);
void LOS_TRACE_HWI_RESPONSE_OUT(UINT32 hwiNum);
#if (ENABLE_HWI_TRACE == 1)
void LOS_TRACE_HWI_CREATE(UINT32 hwiNum, HWI_PRIOR_T hwiPrio, HWI_MODE_T hwiMode, UINTPTR hwiHandler);
void LOS_TRACE_HWI_CREATE_NODE(UINT32 hwiNum, UINTPTR pDevId, UINT32 ret);
void LOS_TRACE_HWI_DELETE(UINT32 hwiNum);
void LOS_TRACE_HWI_DELETE_NODE(UINT32 hwiNum, UINTPTR pDevId, UINT32 ret);
void LOS_TRACE_HWI_TRIGGER(UINT32 hwiNum);
void LOS_TRACE_HWI_ENABLE(UINT32 hwiNum);
void LOS_TRACE_HWI_DISABLE(UINT32 hwiNum);
void LOS_TRACE_HWI_CLEAR(UINT32 hwiNum);
void LOS_TRACE_HWI_SETPRI(UINT32 hwiNum, HWI_PRIOR_T priority);
void LOS_TRACE_HWI_SETAFFINITY(UINT32 hwiNum, UINT32 cpuMask);
void LOS_TRACE_HWI_SENDIPI(UINT32 hwiNum, UINT32 cpuMask);
#else
#define LOS_TRACE_HWI_CREATE(...)
#define LOS_TRACE_HWI_CREATE_NODE(...)
#define LOS_TRACE_HWI_DELETE(...)
#define LOS_TRACE_HWI_DELETE_NODE(...)
#define LOS_TRACE_HWI_TRIGGER(...)
#define LOS_TRACE_HWI_ENABLE(...)
#define LOS_TRACE_HWI_DISABLE(...)
#define LOS_TRACE_HWI_CLEAR(...)
#define LOS_TRACE_HWI_SETPRI(...)
#define LOS_TRACE_HWI_SETAFFINITY(...)
#define LOS_TRACE_HWI_SENDIPI(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSVIEW_LITEOS_H */

/* ************************** End of file *************************** */
