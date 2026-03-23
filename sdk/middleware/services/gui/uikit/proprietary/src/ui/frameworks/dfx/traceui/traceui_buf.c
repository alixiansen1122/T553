/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui buffer manager implementation
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include "los_typedef.h"
#include "los_memory.h"
#include "los_spinlock.h"
#include "traceui_buf.h"

static uint8_t *g_traceBuf = NULL;
static uint32_t g_traceBufPos = 0;
static uint32_t g_traceBufSize = 0;

LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_traceuiSpin);

void TraceuiBufInit(uint8_t *buf, uint32_t size)
{
    uint32_t irqState;

    LOS_SpinLockSave(&g_traceuiSpin, &irqState);
    g_traceBuf = buf;
    g_traceBufPos = 0;
    g_traceBufSize = size;
    LOS_SpinUnlockRestore(&g_traceuiSpin, irqState);
}

uint8_t *TraceuiGetBuf(uint32_t len)
{
    uint32_t irqState;
    uint8_t *retBuf = NULL;

    LOS_SpinLockSave(&g_traceuiSpin, &irqState);
    if (g_traceBufPos + len <= g_traceBufSize) {
        retBuf = &g_traceBuf[g_traceBufPos];
        g_traceBufPos += len;
    }
    LOS_SpinUnlockRestore(&g_traceuiSpin, irqState);
    return retBuf;
}

uint32_t TraceuiGetBufUsedSize(void)
{
    return g_traceBufPos;
}

void TraceuiBufDeinit(void)
{
    uint32_t irqState;

    LOS_SpinLockSave(&g_traceuiSpin, &irqState);
    g_traceBuf = NULL;
    g_traceBufPos = 0;
    g_traceBufSize = 0;
    LOS_SpinUnlockRestore(&g_traceuiSpin, irqState);
}
