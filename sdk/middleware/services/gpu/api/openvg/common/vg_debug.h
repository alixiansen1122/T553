/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vg debug file
 */

#ifndef OPENVG_DEBUG_H
#define OPENVG_DEBUG_H

#include <stdio.h>
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

#define VG_PRINT printf

#if defined(CONFIG_VG_DFX_DEBUG)
#ifdef NO_UART_DEBUG
#define vg_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define vg_dbg(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define vg_err(fmt, args...) VG_PRINT("[VG_ERROR][%s %d]"#fmt"\n", __func__, __LINE__, ##args)
#define vg_dbg(fmt, args...) VG_PRINT("[VG_DEBUG][%s %d]"#fmt"\n", __func__, __LINE__, ##args)
#endif
#elif defined(CONFIG_VG_DFX_ERROR)
#ifdef NO_UART_DEBUG
#define vg_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define vg_err(fmt, args...) VG_PRINT("[VG_ERROR][%s %d]"#fmt"\n", __func__, __LINE__, ##args)
#endif
#define vg_dbg(fmt, args...) (void)0
#else
#define vg_err(fmt, args...) (void)0
#define vg_dbg(fmt, args...) (void)0
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* OPENVG_DEBUG_H */
