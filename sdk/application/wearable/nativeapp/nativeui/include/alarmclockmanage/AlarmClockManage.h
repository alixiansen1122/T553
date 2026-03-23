/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockManage
 * Create: 2021-09-22
 */

#ifndef ALARM_CLOCK_MANAGE_H
#define ALARM_CLOCK_MANAGE_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <climits>

#include "ohos_types.h"
#include "alarmclocktimerconfig/AlarmClockTimerConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_NAME_LENGTH           24
#define EVENT_ALARM_CLOCK_MAX       5
#define SMART_ALARM_CLOCK_MAX       1

#define TWO_BYTES_BITS                          16
#define MINUTES_PER_HOUR                        60
#define AND_FOR_LOW_TWO_BYTES                   0xff // 与运算求低两字节
#define MINS_EVERY_DELAY                        10
#define HOURS_PER_DAY                           24
#define DELAY_COUNT_MAX                         3

/// error status code values
typedef enum {
    STATUS_OK                      =  0,
    STATUS_POINTER_NULL            = -1,
    STATUS_ALARM_ID_INVALID        = -2,
    STATUS_ALARM_TYPE_INVALID      = -3,
    STATUS_ALARM_CNT_ERROR         = -4,
    STATUS_TIMER_ERROR             = -5,
    STATUS_SEC_FUN_ERROR           = -6,
    STATUS_FILE_OPEN_ERROR         = -7,
    STATUS_SERVICE_FUN_ERROR       = -8,
    STATUS_TIME_FUN_ERROR          = -9,
    STATUS_FUN_PARA_INVALID        = -10,
} AlarmClockErrorCode;

typedef enum {
    TYPE_INVALID_ALARM_CLOCK = 0,
    TYPE_EVENT_ALARM_CLOCK = 1,
    TYPE_SMART_ALARM_CLOCK,
} AlarmClockType;

typedef enum {
    TYPE_CUR_NODE_NO_SKIP = 0,
    TYPE_CUR_NODE_SKIP,
} FirstAlarmTodaySetType;

typedef enum {
    EVENT_ALARM_RING = 10,
    EVENT_ALARM_RING_CANCEL,
    EVENT_ALARM_RING_DELAY,
    EVENT_ALARM_INFO_UPDATE,
    EVENT_ALARM_LIST_TODAY_UPDATE,
    EVENT_WECHAT_MSG_ARRIVED,
    EVENT_CALL_INCOMING,
    EVENT_NO_EVENT_DEFAULT = 0xffffffff,
} AlarmClockEventType;

typedef struct {
    uint8 alarmClockId; // 事件闹钟0-4 智能闹钟5
    uint8 alarmClockEnable;
    uint8 alarmType;
    uint8 nameLen;
    char alarmClockName[CLOCK_NAME_LENGTH];
    uint32 alarmTime;
    uint32 sleepWindowTime;
    uint8 repeatPeriod;
    uint8 delayNum;
    uint8 smartDelayFirst;
    uint8 smartDelay;
    uint32 index;
} AlarmClockInfo;

typedef struct {
    uint8 alarmClockId;
    uint8 alarmType;
    uint8 reserved[2]; // 2 bytes for 4 bytes align
    uint32 alarmTime;
} AlarmClockInfoToday;

typedef struct {
    AlarmClockInfo smartAlarmInfo;
    AlarmClockInfo eventAlarmInfo[EVENT_ALARM_CLOCK_MAX];
    uint16 smartAlarmCnt;
    uint16 eventAlarmCnt;
} AlarmClockManagement;

typedef struct {
    uint32 eventType;
    AlarmClockEventData data;
} AlarmClockQueueMsg;

int32 GetEventAlarmInfo(uint8 id, AlarmClockInfo *info);
int32 GetSmartAlarmInfo(AlarmClockInfo *info);
void EmptyListAlarmToday(void);
void DisplayListAlarmToday(void);
void UpdataListAndFirstAlarmToday(void);

int32 SetDatetime(int32 argc, char *argv[]);
int32 DispDatetime(void);

#ifdef __cplusplus
}
#endif

#endif // ALARM_CLOCK_MANAGE_H
