/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description : vau debug file
 */

#ifndef API_VAU_DEBUG_H
#define API_VAU_DEBUG_H

#include <stdio.h>
#include <securec.h>
#include "td_base.h"
#include "soc_vau_type.h"
#ifdef NO_UART_DEBUG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VAU_API_PRINT printf
#define fractional_part(val) ((td_u32)((val) * 1000) % 1000)
#define VAU_THOUSAND 1000

#ifdef NO_UART_DEBUG
#define vau_print(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define vau_print(fmt, args...) printf(fmt, ##args)
#endif

#if defined(CONFIG_VAU_DFX_DEBUG)
#ifdef NO_UART_DEBUG
#define vau_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define vau_dbg(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define vau_err(fmt, args...) printf("[VAU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define vau_dbg(fmt, args...) printf("[VAU_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#elif defined(CONFIG_VAU_DFX_ERROR)
#ifdef NO_UART_DEBUG
#define vau_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define vau_err(fmt, args...) printf("[VAU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#define vau_dbg(fmt, args...)
#else
#define vau_err(fmt, args...)
#define vau_dbg(fmt, args...)
#endif

td_void vau_debug_print_surface_list(const ext_vau_surface_list *surface_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_VAU_DEBUG_H */
