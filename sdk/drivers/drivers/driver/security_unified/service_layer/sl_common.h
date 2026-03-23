/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides common utils for security_unified service layer. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef SL_COMMON_H
#define SL_COMMON_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Convert security-driven error codes to standard error codes.
 * @param  [in]  from_errcode Error codes returned by the security driver interface.
 * @retval Converted error code. For details, see @ref errcode_t
 * @else
 * @brief  将安全驱动的错误码转换成标准错误码
 * @param  [in]  from_errcode 安全驱动接口返回的错误码
 * @retval 转换后的错误码，参考 @ref errcode_t
 * @endif
 */
errcode_t crypto_sl_common_get_errcode(int32_t from_errcode);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif