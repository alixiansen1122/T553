/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description : dpu debug file
 */

#ifndef API_DPU_DEBUG_H
#define API_DPU_DEBUG_H

#include <stdio.h>
#include "td_base.h"
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

#if defined(CONFIG_DPU_DFX_DEBUG)
#ifdef NO_UART_DEBUG
#define dpu_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define dpu_dbg(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define dpu_err(fmt, args...) printf("[DPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define dpu_dbg(fmt, args...) printf("[DPU_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#elif defined(CONFIG_DPU_DFX_ERROR)
#ifdef NO_UART_DEBUG
#define dpu_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define dpu_err(fmt, args...) printf("[DPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#define dpu_dbg(fmt, args...)
#else
#define dpu_err(fmt, args...)
#define dpu_dbg(fmt, args...)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_DPU_DEBUG_H */
