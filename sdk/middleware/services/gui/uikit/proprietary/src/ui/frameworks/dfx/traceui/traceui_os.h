/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui operating system interface
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef TRACEUI_OS_H
#define TRACEUI_OS_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <securec.h>
#include "los_tick.h"
#include "los_hwi.h"
#include "los_hwi_pri.h"
#include "los_task.h"
#include "los_task_pri.h"

#define TRACEUI_ERR(fmt, ...) printf("ERR: " fmt, ##__VA_ARGS__)
#define TRACEUI_DBG(fmt, ...) printf("DBG: " fmt, ##__VA_ARGS__)
#define TRACEUI_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

// get current timestamp in cycle
static inline uint32_t TraceuiGetTimeCycle(void)
{
    return (uint32_t)HalClockGetCycles();
}

// convert cycle to nanosecond
static inline uint64_t TraceuiCycleToNs(uint32_t cycle)
{
    return (uint64_t)cycle * OS_SYS_NS_PER_SECOND / g_sysClock;
}

// get current cpu core id
static inline uint8_t TraceuiGetCpuId(void)
{
    return ArchCurrCpuid();
}

// get hardware interrupt name
static inline const char *TraceuiGetIrqName(uint32_t irqNum)
{
    static char name[16];
    const HwiHandleInfo *form = OsGetHwiForm(irqNum);
    if (form != NULL && (void *)form->registerInfo != NULL && ((HWI_IRQ_PARAM_S *)form->registerInfo)->pName != NULL) {
        if (sprintf_s(name, sizeof(name), "IRQ-%s", ((HWI_IRQ_PARAM_S *)form->registerInfo)->pName) > 0) {
            return name;
        }
    }
    (void)sprintf_s(name, sizeof(name), "IRQ-%u", irqNum);
    return name;
}

// get task name
static inline const char *TraceuiGetTaskName(uint32_t taskId)
{
    static char name[16];
    LosTaskCB *task = OS_TCB_FROM_TID(taskId);
    if (task == NULL || task->taskName == NULL) {
        (void)sprintf_s(name, sizeof(name), "task-%u", taskId);
        return name;
    }
    return task->taskName;
}

static inline bool TraceuiIsIdleTask(uint32_t taskId)
{
    Percpu *perCpu;
    for (int32_t i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        perCpu = OsPercpuGetByID(i);
        if (perCpu != NULL && taskId == perCpu->idleTaskId) {
            return true;
        }
    }
    return false;
}

#endif
