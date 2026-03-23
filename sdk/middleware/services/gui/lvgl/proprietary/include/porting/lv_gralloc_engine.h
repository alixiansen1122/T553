/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_GRALLOC_ENGINE_H
#define LV_GRALLOC_ENGINE_H

#include "display_gralloc.h"
#include "display_type.h"
#include "lv_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void* virAddr;
    uint64_t phyAddr;
#if ENABLE_GMMU
    uint32_t stride;
#endif
} LvGrallocBuffer;

bool LvInitGrallocEngine(void);
bool LvAllocBuffer(const AllocInfo* info, LvGrallocBuffer* buffer);
bool LvFreeBuffer(uint8_t* virAddr);
uint8_t* LvAllocBufferBySize(uint32_t size);
uint8_t* LvAllocBufferForImg(uint32_t size);
uint8_t* LvAllocBufferForPath(uint32_t size);
void LvDumpMem(void);
void LvSetMemMonitor(bool enable);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif