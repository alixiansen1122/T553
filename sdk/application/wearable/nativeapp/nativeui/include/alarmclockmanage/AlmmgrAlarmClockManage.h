/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockManage
 * Create: 2021-09-22
 */

#ifndef ALMMGR_ALARM_CLOCK_MANAGE_H
#define ALMMGR_ALARM_CLOCK_MANAGE_H

#include "ohos_types.h"
#include "alarmclockmanage/AlarmClockManage.h"

#ifdef __cplusplus
extern "C" {
#endif

void AlmmgrInitManagement(void);
void AlmmgrAlarmClockEventProcess(const AlarmClockQueueMsg *msg);
void AlmmgrAlarmClockPacketProcess(const uint8 *packet);
int32 AlmmgrAddAlarmClock(const AlarmClockInfo *info);
int32 AlmmgrUpdataAlarmClock(const AlarmClockInfo *info);
int32 AlmmgrDeleteAlarmClock(const AlarmClockInfo *info);
int32 AlmmgrGetManagement(AlarmClockManagement *management);
uint16 AlmmgrGetEventAlarmCnt(void);
uint16 AlmmgrGetSmartAlarmCnt(void);

#ifdef __cplusplus
}
#endif

#endif // ALMMGR_ALARM_CLOCK_MANAGE_H
