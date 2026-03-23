/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: svr dynload implementation.
 * Author:
 * Create: 2024-10-30
 */

#include "svr_dynload.h"
#include "stdint.h"
#include "los_ld_elflib.h"
#include "los_memory.h"
#include "memory_config.h"
#include "ohos_init.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void *g_dyn_handle = NULL;
char *g_dyn_path = NULL;

static void init(void)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    uint8_t *dyn_pool = NULL;
    dyn_pool =  (uint8_t *)PSRAM_APP_SYN_LOAD_ADDR;
    uint32_t ret = LOS_MemInit((void *)dyn_pool, PSRAM_APP_SYN_LOAD_LENGTH);
    LOS_DynMemPoolSet(dyn_pool);
#endif
}

APP_FEATURE_INIT_PRI(init, LAYER_INIT_LEVEL_4);

void dynload_load_library(const char* path)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    if ((g_dyn_handle != NULL) && (g_dyn_path != NULL) && (strcmp(path, g_dyn_path) == 0)) {
        return;
    }
    dynload_unload_library();
    g_dyn_handle = LOS_SoLoad(path);
    g_dyn_path = strdup(path);
#endif
}

void dynload_unload_library(void)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    if (g_dyn_handle != NULL) {
        LOS_ModuleUnload(g_dyn_handle);
        g_dyn_handle = NULL;
    }
    if (g_dyn_path != NULL) {
        free(g_dyn_path);
        g_dyn_path = NULL;
    }
#endif
}

void* dynload_get_symbol(char* name)
{
#if defined(LOSCFG_KERNEL_DYNLOAD) && defined(LOSCFG_DYNLOAD_DYN_FROM_FS)
    if (g_dyn_handle != NULL) {
        return LOS_FindSymByName(g_dyn_handle, name);
    }
#endif
    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif