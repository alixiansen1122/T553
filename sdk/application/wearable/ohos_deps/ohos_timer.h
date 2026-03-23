/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: OHOSLiteErrorCode header file.
 * Author: CompanyName
 * Create:
 */

#ifndef OHOS_TIMER_H
#define OHOS_TIMER_H

#include <stdint.h>
#if !defined(_WIN32)
#include "cmsis_os.h"
#else
#include "windows.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */
/* --------------------------------------------------------------------------------------------*
 * Defintion of timer code. The atomic timer are applicable to both the linux and windows
 *-------------------------------------------------------------------------------------------- */

uint32_t GetOSTick(uint32_t ms);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif // OHOS_TIMER_H
