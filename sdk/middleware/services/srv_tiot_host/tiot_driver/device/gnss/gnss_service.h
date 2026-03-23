/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: GNSS service interface. \n
 *
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef GNSS_SERVICE_H
#define GNSS_SERVICE_H

#include "tiot_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_gnss_service_if  GNSS Service Interface
 * @ingroup  tiot_device_gnss
 * @{
 */

/**
 * @brief  GNSS service init.
 * @return ERRCODE_TIOT_SUCC (zero) if ok, negative if failed.
 */
int32_t gnss_service_init(void);

/**
 * @brief  GNSS service deinit.
 */
void gnss_service_deinit(void);

/**
 * @brief  GNSS service get ctrl.
 * @param  dev_id Device id.
 * @return Tiot controller instace.
 */
uintptr_t gnss_service_get_ctrl(uint8_t dev_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif