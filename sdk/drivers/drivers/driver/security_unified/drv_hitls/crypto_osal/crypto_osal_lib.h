/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides IPC source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-04-18, Create file. \n
 */
#ifndef CRYPTO_OSAL_LIB_H
#define CRYPTO_OSAL_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"

#define crypto_print(...)

#define crypto_ioremap_nocache(phys_addr, size)     (void *)(uintptr_t)(phys_addr)
#define crypto_iounmap(...)

#endif