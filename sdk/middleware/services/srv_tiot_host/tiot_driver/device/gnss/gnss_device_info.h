/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: GNSS device info header. \n
 *
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef GNSS_DEVICE_INFO_H
#define GNSS_DEVICE_INFO_H

#include "tiot_device_info.h"
#include "tiot_controller_helper.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_gnss_device_info  GNSS Device Info
 * @ingroup  tiot_device_gnss
 * @{
 */

#ifdef CONFIG_GNSS_UART_WITH_FLOWCTRL
#define GNSS_UART_ATTR_FLOW_CTRL    TIOT_UART_ATTR_FLOW_CTRL_ENABLE
#else
#define GNSS_UART_ATTR_FLOW_CTRL    TIOT_UART_ATTR_FLOW_CTRL_DISABLE
#endif

#define GNSS_UART_DEFAULT_BAUDRATE 115200

/**
 * @brief  GNSS device get info, same for all GNSS devices.
 * @return GNSS device info.
 */
const tiot_device_info *gnss_device_get_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif