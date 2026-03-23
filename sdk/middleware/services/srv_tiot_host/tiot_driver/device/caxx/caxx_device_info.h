/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * Description: CAXX device info header. \n
 *
 * History: \n
 * 2024-01-02, Create file. \n
 */
#ifndef CAXX_DEVICE_INFO_H
#define CAXX_DEVICE_INFO_H

#include "tiot_device_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_caxx_device_info  CAXX Device Info
 * @ingroup  tiot_device_caxx
 * @{
 */

/**
 * @brief  CAXX device get info, same for all CAXX devices.
 * @return CAXX device info.
 */
const tiot_device_info *caxx_device_get_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CAXX_DEVICE_INFO_H */