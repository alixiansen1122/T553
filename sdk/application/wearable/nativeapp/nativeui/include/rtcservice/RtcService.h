/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: RtcService
 * Create: 2021-09-22
 */

#ifndef OHOS_RTC_SERVICE_H
#define OHOS_RTC_SERVICE_H

#include "service.h"
#include "iunknown.h"
#include "broadcast_feature.h"
#include "alarmclockmanage/AlarmClockManage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_SERVICE "rtcservice"

struct RtcService {
    INHERIT_SERVICE;
    Subscriber subscirber;
};
typedef struct RtcService RtcService;

void RtcServiceInit(void);

#ifdef __cplusplus
}
#endif

#endif // OHOS_RTC_SERVICE_H
