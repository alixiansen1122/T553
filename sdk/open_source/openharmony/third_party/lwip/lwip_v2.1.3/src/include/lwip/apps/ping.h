/**
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: Provides PING for lwip. \n
 * Author: CompanyName \n
 */

#ifndef LWIP_PING_H
#define LWIP_PING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/arch.h"

u32_t os_start_ping(char *para, u32_t param_len);

#ifdef __cplusplus
}
#endif

#endif