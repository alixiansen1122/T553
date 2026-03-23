/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: CAXX service header. \n
 *
 * History: \n
 * 2024-01-02, Create file. \n
 */
#ifndef CAXX_SERVICE_H
#define CAXX_SERVICE_H

#include "tiot_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_caxx_service_if  CAXX Service Interface
 * @ingroup  tiot_device_caxx
 * @{
 */

/**
 * @brief  CAXX service init.
 * @return ERRCODE_TIOT_SUCC (zero) if ok, negative if failed.
 */
int32_t caxx_service_init(void);

/**
 * @brief  CAXX service deinit.
 */
void caxx_service_deinit(void);

/**
 * @brief  CAXX service get ctrl.
 * @param  dev_id Device id.
 * @return Tiot controller instace.
 */
uintptr_t caxx_service_get_ctrl(uint8_t dev_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CAXX_SERVICE_H */