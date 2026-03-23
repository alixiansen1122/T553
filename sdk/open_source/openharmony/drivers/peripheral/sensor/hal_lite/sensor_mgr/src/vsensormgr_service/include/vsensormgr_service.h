/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor task mgr service
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef VSENSOR_MGR_SEVICE_H
#define VSENSOR_MGR_SEVICE_H

#include "sensor_typedef.h"
#include "soc_osal.h"
#include "hsensormgr_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t VsensorMgrInit(void);
int32_t VsensorMgrRegister(const Vsensor *vsensor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif