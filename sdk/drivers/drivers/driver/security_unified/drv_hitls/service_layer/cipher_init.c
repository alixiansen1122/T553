/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#include "security_init.h"

#include "bsl_sal.h"
#include "drv_symc_aead.h"

#include "crypto_osal_lib.h"

static void *inner_malloc(uint32_t size)
{
    return malloc(size);
}

static void inner_free(void *addr)
{
    free(addr);
}

void uapi_drv_cipher_env_init(void)
{
    BSL_SAL_MemCallback mem_callback;
    mem_callback.pfMalloc = inner_malloc;
    mem_callback.pfFree = inner_free;

    (void)BSL_SAL_RegMemCallback(&mem_callback);
}

void uapi_drv_cipher_env_deinit(void)
{
    BSL_SAL_MemCallback mem_callback;
    mem_callback.pfMalloc = NULL;
    mem_callback.pfFree = NULL;

    (void)BSL_SAL_RegMemCallback(&mem_callback);
}