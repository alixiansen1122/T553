/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: board misc port config. \n
 *
 * History: \n
 * 2023-12-04, Create file. \n
 */
#ifndef TIOT_BOARD_MISC_PORT_CONFIG_H
#define TIOT_BOARD_MISC_PORT_CONFIG_H

#include "pm_definition.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* 板级低功耗ID */
#if (defined SUPPORT_GNSS_FEATURE)
#define TIOT_WAKEUP_LOCK_ID    PM_ID_TIOT
#else
#define TIOT_WAKEUP_LOCK_ID    PM_ID_SYS
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif