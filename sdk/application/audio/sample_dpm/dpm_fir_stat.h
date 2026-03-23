/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: user api for sample dpm fir module
 * Author: @CompanyNameTag
 */

#ifndef __DPM_FIR_STAT_H__
#define __DPM_FIR_STAT_H__

#ifndef DPM_PRINT_PROC_TIME
#define fir_time_consume_start()
#define fir_time_consume_stop(desc)

#else
#include <stdio.h>
#include "tcxo.h"
#define stat_printf printf

static unsigned long long g_start_time = 0;

#define fir_time_consume_start()           \
    do {                                   \
        g_start_time = uapi_tcxo_get_us(); \
    } while (0)

#define fir_time_consume_stop(desc)                                    \
    do {                                                               \
        g_start_time = uapi_tcxo_get_us() - g_start_time;              \
        stat_printf("%s time cost us:%06llu\n", (desc), g_start_time); \
    } while (0)

#endif /* DPM_PRINT_PROC_TIME */

#endif /* __DPM_FIR_STAT_H__ */
