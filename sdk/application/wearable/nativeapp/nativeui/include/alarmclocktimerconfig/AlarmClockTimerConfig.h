/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockTimer
 * Create: 2021-09-22
 */

#ifndef ALARM_CLOCK_TIMER_CONFIG_H
#define ALARM_CLOCK_TIMER_CONFIG_H

#include <sys/time.h>

#include "ohos_types.h"
#include "ohos_timer.h"
#include "graphic_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALARM_INTERVAL_MS 1000

typedef union {
    struct timeval timeVal;
    uint32 value;
} AlarmClockEventData;

typedef enum {
    ALARM_STATUS_UNUSED,  /* The alarm timer is not used. */
    ALARM_STATUS_CREATED, /* The alarm timer is created. */
    ALARM_STATUS_RUNNING, /* The alarm timer is timing. */
} AlarmState;

typedef enum {
    ALARM_A,
    ALARM_B,
    ALARM_NONE,
} AlarmType;

typedef struct {
    uint8_t week;
    uint8_t hour;
    uint8_t minute;
    uint8_t reserved;
} AlarmInfo;

typedef void(*AlarmCallBack)(void);

typedef struct {
    AlarmState state;
    AlarmInfo info;
    AlarmCallBack callback;
} AlarmCtrl;

void AlarmTimerEventPublish(uint16 topic, AlarmClockEventData data);
int32_t AlarmTimerInit(void);
int32_t SetZeroAlarm(void);
int32_t SetAlarm(AlarmType type, uint8_t week, uint8_t hour, uint8_t minute);
int32_t AlarmGetCurrentTimeInfo(struct tm *timeInfo);

#ifdef __cplusplus
}
#endif

#endif // ALARM_CLOCK_TIMER_CONFIG_H