/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef AUTO_OTA_SERVICE_H
#define AUTO_OTA_SERVICE_H

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void auto_ota_dev_add(uint8_t *addr);
bool auto_ota_dev_del(uint8_t *addr);
void auto_ota_dev_show(void);
void auto_ota_task_main(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* AUTO_OTA_SERVICE_H */
