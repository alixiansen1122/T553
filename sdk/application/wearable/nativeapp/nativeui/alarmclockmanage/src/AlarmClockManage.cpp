/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockManage
 * Create: 2021-09-22
 */

#include <sys/stat.h>
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <climits>
#include <list>

#include "wearable_log.h"
#include "securec.h"
#include "service_id_define.h"
#include "broadcast_service.h"
#include "samgr_lite.h"

#include "rtcservice/RtcService.h"
#include "alarmclocktimerconfig/AlarmClockTimerConfig.h"
#include "alarmclockmanage/AlarmClockManage.h"

#ifdef __cplusplus
extern "C" {
#endif

static constexpr uint32 DATETIME_PARA_NUM = 6;
static constexpr const char *ALARM_CLOCK_MANAGE_DATA_PATH =  (char *)"/user/data/alarmclock/alarmClockManage.bin";
static constexpr const char *ALARM_CLOCK_MANAGE_DIRECTORY =  (char *)"/user/data/alarmclock";

static int32 SaveManagementToFile(void);
static int32 LoadManagementFromFile(void);
static int32 EmptyManagement(void);
static bool IsAlarmClockValidToday(const AlarmClockInfo *info);
static int32 CreateListAlarmTodaySmart(void);
static int32 CreateListAlarmTodayEvent(void);
static int32 CreateListAlarmToday(void);
static int32 SortListAlarmToday(void);
static int32 AlarmInfoCheck(const AlarmClockInfo *alarmInfo);
static int32 AddAndUpdataListAlarmToday(const AlarmClockInfo *alarmInfo);
static int32 UpdataListAlarmToday(const AlarmClockInfo *alarmInfo);
static int32 DeleteAndUpdataListAlarmToday(uint8 id);
static void EventAlarmRingProcess(void);
static int32 SetFirstAlarmToday(FirstAlarmTodaySetType type);
static void UpdataAndSaveAlarmClockDelayNum(void);
static void UpdataAndSaveAlarmClockEnable(uint8 id);
static int32 DelayCurAlarmClock(uint8 alarmId);
static int32 CancelCurAlarmClock(uint8 alarmId);

AlarmClockManagement g_alarmClockManagement;

std::list<AlarmClockInfoToday> g_alarmTodayList;
/* private */
static int32 SaveManagementToFile(void)
{
    uint32 size;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SaveManagementToFile !!");

    size = sizeof(AlarmClockManagement);
    std::ofstream fd(ALARM_CLOCK_MANAGE_DATA_PATH);
    if (!fd.good()) {
        (void)mkdir(ALARM_CLOCK_MANAGE_DIRECTORY, S_IREAD | S_IWRITE);
        std::ofstream fd(ALARM_CLOCK_MANAGE_DATA_PATH);
        if (!fd.good()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SaveManagementToFile open fail");
            fd.close();
            return STATUS_FILE_OPEN_ERROR;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "mkdir success!!");
    }

    fd.write((const char *)(&g_alarmClockManagement), size);
    fd.close();
    return STATUS_OK;
}

static int32 LoadManagementFromFile(void)
{
    uint32 size;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadManagementFromFile !!");
    size = sizeof(AlarmClockManagement);
    (void)memset_s(&g_alarmClockManagement, size, 0, size);
    std::ifstream fd(ALARM_CLOCK_MANAGE_DATA_PATH);
    if (!fd.good()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadManagementFromFile open fail");
        fd.close();
        return STATUS_FILE_OPEN_ERROR;
    } else {
        fd.read((char *)(&g_alarmClockManagement), size);
        fd.close();
        return STATUS_OK;
    }
}

static int32 EmptyManagement(void)
{
    int32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EmptyManagement !!");
    ret = memset_s(&g_alarmClockManagement, sizeof(g_alarmClockManagement), 0, sizeof(g_alarmClockManagement));
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EmptyManagement memset_s failed !!");
        return STATUS_SEC_FUN_ERROR;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EmptyManagement succeed !!");

    return STATUS_OK;
}

static bool IsAlarmClockValidToday(const AlarmClockInfo *info)
{
    struct tm curTime;
    int32_t ret = AlarmGetCurrentTimeInfo(&curTime);
    if (ret != ERRCODE_SUCC) {
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "IsAlarmClockValidToday repeatPeriod=%x, tm_wday=%d!!",
        info->repeatPeriod, curTime.tm_wday);
    if ((info->repeatPeriod & (0x1 << curTime.tm_wday)) > 0) {
        return true;
    } else if (info->repeatPeriod == 0) {
        return true;
    } else {
        return false;
    }
}

static int32 CreateListAlarmTodaySmart(void)
{
    AlarmClockInfoToday info;
    uint32 size = sizeof(AlarmClockInfoToday);

    if (g_alarmClockManagement.smartAlarmCnt > 0) {
        /* 该闹钟今天是否使能判断 */
        if (IsAlarmClockValidToday(&g_alarmClockManagement.smartAlarmInfo) &&
            g_alarmClockManagement.smartAlarmInfo.alarmClockEnable) {
            (void)memset_s(&info, size, 0, size);

            info.alarmType = g_alarmClockManagement.smartAlarmInfo.alarmType;
            info.alarmClockId = g_alarmClockManagement.smartAlarmInfo.alarmClockId;
            info.alarmTime = g_alarmClockManagement.smartAlarmInfo.alarmTime +
                g_alarmClockManagement.smartAlarmInfo.delayNum * MINS_EVERY_DELAY;
            info.alarmTime = (((info.alarmTime >> TWO_BYTES_BITS) +
                (info.alarmTime & AND_FOR_LOW_TWO_BYTES) / MINUTES_PER_HOUR) << TWO_BYTES_BITS) +
                (info.alarmTime & AND_FOR_LOW_TWO_BYTES) % MINUTES_PER_HOUR;
            g_alarmTodayList.push_front(info);
        }
    }

    return STATUS_OK;
}

static int32 CreateListAlarmTodayEvent(void)
{
    uint32 i;
    AlarmClockInfoToday info;
    uint32 size = sizeof(AlarmClockInfoToday);

    if (g_alarmClockManagement.eventAlarmCnt == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CreateListAlarmTodayEvent eventAlarmCnt is 0!!");
        return STATUS_OK;
    }

    for (i = 0; i < EVENT_ALARM_CLOCK_MAX; i++) {
        /* 该闹钟今天是否使能判断 */
        if (IsAlarmClockValidToday(&g_alarmClockManagement.eventAlarmInfo[i]) &&
            g_alarmClockManagement.eventAlarmInfo[i].alarmClockEnable) {
            (void)memset_s(&info, size, 0, size);

            info.alarmType = g_alarmClockManagement.eventAlarmInfo[i].alarmType;
            info.alarmClockId = g_alarmClockManagement.eventAlarmInfo[i].alarmClockId;
            info.alarmTime = g_alarmClockManagement.eventAlarmInfo[i].alarmTime +
                g_alarmClockManagement.eventAlarmInfo[i].delayNum * MINS_EVERY_DELAY;
            info.alarmTime = (((info.alarmTime >> TWO_BYTES_BITS) +
                (info.alarmTime & AND_FOR_LOW_TWO_BYTES) / MINUTES_PER_HOUR) << TWO_BYTES_BITS) +
                (info.alarmTime & AND_FOR_LOW_TWO_BYTES) % MINUTES_PER_HOUR;
            g_alarmTodayList.push_front(info);
        }
    }

    return STATUS_OK;
}

static int32 CreateListAlarmToday(void)
{
    (void)CreateListAlarmTodaySmart();
    (void)CreateListAlarmTodayEvent();
    return STATUS_OK;
}

static bool Compare(const AlarmClockInfoToday &info1, const AlarmClockInfoToday &info2)
{
    if (info1.alarmTime > info2.alarmTime) {
        return false;
    } else if (info1.alarmTime == info2.alarmTime) {
        if (info1.alarmClockId > info2.alarmClockId) {
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}

static int32 SortListAlarmToday(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SortListAlarmToday!!");
    g_alarmTodayList.sort(Compare);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SortListAlarmToday end!!");
    return STATUS_OK;
}

static int32 AlarmInfoCheck(const AlarmClockInfo *alarmInfo)
{
    uint16 hour;
    uint16 min;

    if (alarmInfo == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmInfoCheck alarmInfo nullptr!!");
        return STATUS_POINTER_NULL;
    }

    if (alarmInfo->nameLen >= CLOCK_NAME_LENGTH || alarmInfo->nameLen != strlen(alarmInfo->alarmClockName)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmInfoCheck alarmClockName or nameLen error!!");
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "nameLen = %u, strlen = %u", alarmInfo->nameLen,
            strlen(alarmInfo->alarmClockName));
        return STATUS_FUN_PARA_INVALID;
    }

    hour = alarmInfo->alarmTime >> TWO_BYTES_BITS;
    min = alarmInfo->alarmTime & AND_FOR_LOW_TWO_BYTES;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmInfoCheck alarmTime valid, hour=%u, min=%u", hour, min);
    if (hour >= HOURS_PER_DAY || min >= MINUTES_PER_HOUR) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmInfoCheck alarmTime invalid, hour=%u, min=%u", hour, min);
        return STATUS_FUN_PARA_INVALID;
    }

    return STATUS_OK;
}

static int32 AddAndUpdataListAlarmToday(const AlarmClockInfo *alarmInfo)
{
    int32 ret;
    AlarmClockInfoToday info;
    uint32 size = sizeof(AlarmClockInfoToday);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddAndUpdataListAlarmToday!!");
    ret = AlarmInfoCheck(alarmInfo);
    if (ret != STATUS_OK) {
        return ret;
    }
    /* 将指定闹钟信息加入当天闹钟列表 */
    if (!IsAlarmClockValidToday(alarmInfo) || (!alarmInfo->alarmClockEnable)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddAndUpdataListAlarmToday alarmInfo invalid today!!");
        return STATUS_OK;
    }
    (void)memset_s(&info, size, 0, size);

    info.alarmType = alarmInfo->alarmType;
    info.alarmClockId = alarmInfo->alarmClockId;
    info.alarmTime = alarmInfo->alarmTime + alarmInfo->delayNum * MINS_EVERY_DELAY;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmTime : %lx, %lx, %lu!!", info.alarmTime,
        (info.alarmTime & AND_FOR_LOW_TWO_BYTES), (info.alarmTime & AND_FOR_LOW_TWO_BYTES) % MINUTES_PER_HOUR);
    info.alarmTime = (((info.alarmTime >> TWO_BYTES_BITS) +
        (info.alarmTime & AND_FOR_LOW_TWO_BYTES) / MINUTES_PER_HOUR) << TWO_BYTES_BITS) +
        (info.alarmTime & AND_FOR_LOW_TWO_BYTES) % MINUTES_PER_HOUR;
    g_alarmTodayList.push_front(info);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmType : %u, alarmClockId : %u, alarmTime : %lx!!",
        info.alarmType, info.alarmClockId, info.alarmTime);

    (void)SortListAlarmToday();
    (void)SetFirstAlarmToday(TYPE_CUR_NODE_NO_SKIP);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddAndUpdataListAlarmToday end!!");
    return STATUS_OK;
}

static int32 UpdataListAlarmToday(const AlarmClockInfo *alarmInfo)
{
    int32 ret;
    std::list< AlarmClockInfoToday>::iterator node;

    ret = AlarmInfoCheck(alarmInfo);
    if (ret != STATUS_OK) {
        return ret;
    }
    /* 将指定ID的闹钟信息刷新 */
    for (node = g_alarmTodayList.begin(); node != g_alarmTodayList.end(); node++) {
        if (node->alarmClockId == alarmInfo->alarmClockId) {
            if (IsAlarmClockValidToday(alarmInfo) && alarmInfo->alarmClockEnable) {
                node->alarmTime = alarmInfo->alarmTime + alarmInfo->delayNum * MINS_EVERY_DELAY;
                node->alarmTime = (((node->alarmTime >> TWO_BYTES_BITS) +
                    (node->alarmTime & AND_FOR_LOW_TWO_BYTES) / MINUTES_PER_HOUR) << TWO_BYTES_BITS) +
                    (node->alarmTime & AND_FOR_LOW_TWO_BYTES) % MINUTES_PER_HOUR;
                node->alarmType = alarmInfo->alarmType;
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdataListAlarmToday update info!!");
                break;
            } else {
                g_alarmTodayList.erase(node);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdataListAlarmToday delete info!!");
                break;
            }
        }
    }

    if (node == g_alarmTodayList.end() && alarmInfo->alarmClockId <= EVENT_ALARM_CLOCK_MAX) {
        (void)AddAndUpdataListAlarmToday(alarmInfo);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdataListAlarmToday add info!!");
        return STATUS_OK;
    }

    (void)SortListAlarmToday();
    (void)SetFirstAlarmToday(TYPE_CUR_NODE_NO_SKIP);

    return STATUS_OK;
}

static int32 DeleteAndUpdataListAlarmToday(uint8 id)
{
    std::list< AlarmClockInfoToday>::iterator node;
    /* 将指定闹钟信息删除 */
    for (node = g_alarmTodayList.begin(); node != g_alarmTodayList.end(); node++) {
        if (node->alarmClockId == id) {
            g_alarmTodayList.erase(node);
            break;
        }
    }

    if (node == g_alarmTodayList.end() || id > EVENT_ALARM_CLOCK_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DeleteAndUpdataListAlarmToday id is invalid, id = %u!!", id);
        return STATUS_ALARM_ID_INVALID;
    }

    (void)SetFirstAlarmToday(TYPE_CUR_NODE_NO_SKIP);

    return STATUS_OK;
}

static void EventAlarmRingProcess(void)
{
    struct tm curTime;
    AlarmClockInfoToday info;
    if (AlarmGetCurrentTimeInfo(&curTime) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EventAlarmRingProcess AlarmGetCurrentTimeInfo failed!!");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "EventAlarmRingProcess, hour:%u, min:%u", curTime.tm_hour, curTime.tm_min);
    info = g_alarmTodayList.front();
    if (((info.alarmTime >> TWO_BYTES_BITS) == curTime.tm_hour) &&
       ((info.alarmTime & AND_FOR_LOW_TWO_BYTES) == curTime.tm_min)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ring ring ring 3");
        UpdataAndSaveAlarmClockEnable(info.alarmClockId);
        (void)SetFirstAlarmToday(TYPE_CUR_NODE_SKIP); // 第一个闹钟已响应直接跳过
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EventAlarmRingProcess alarmTime or curTime invalid!!");
        /* 异常创建 */
    }
    return;
}

static int32 SetFirstAlarmToday(FirstAlarmTodaySetType type)
{
    struct tm curTime;
    FirstAlarmTodaySetType tempType = type;
    std::list< AlarmClockInfoToday>::iterator node;
    std::list< AlarmClockInfoToday>::iterator tempNode;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetFirstAlarmToday !!");
    if (AlarmGetCurrentTimeInfo(&curTime) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SetFirstAlarmToday AlarmGetCurrentTimeInfo failed!!");
        return STATUS_TIME_FUN_ERROR;
    }

    for (node = g_alarmTodayList.begin(); node != g_alarmTodayList.end();) {
        tempNode = node;
        tempNode++;
        if (tempType == TYPE_CUR_NODE_SKIP) { // 第一个闹钟如果已响应直接跳过并删除,有连续相同闹铃时间则连续跳过并删除
            if (node->alarmTime == tempNode->alarmTime) {
                tempType = TYPE_CUR_NODE_SKIP;
            } else {
                tempType = TYPE_CUR_NODE_NO_SKIP;
            }
            node = g_alarmTodayList.erase(node);
            continue;
        }

        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmTime : %lx!!", node->alarmTime);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "curTime: hour %ld, min %ld!!", curTime.tm_hour, curTime.tm_min);

        if (((node->alarmTime >> TWO_BYTES_BITS == (uint32)curTime.tm_hour) &&
            ((node->alarmTime & AND_FOR_LOW_TWO_BYTES) > (uint32)curTime.tm_min)) ||
            node->alarmTime >> TWO_BYTES_BITS > (uint32)curTime.tm_hour) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmTime > curTime!!");
            SetAlarm(ALARM_A, 0, node->alarmTime >> TWO_BYTES_BITS, node->alarmTime & AND_FOR_LOW_TWO_BYTES);
            break;
        } else { // 已过期的直接删除
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "alarmTime <= curTime!!");
            g_alarmTodayList.erase(node);
        }
        node++;
    }

    return STATUS_OK;
}

static void UpdataAndSaveAlarmClockDelayNum(void)
{
    uint32 i;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdataManagement !!");

    for (i = 0; i < EVENT_ALARM_CLOCK_MAX; i++) {
        g_alarmClockManagement.eventAlarmInfo[i].delayNum = 0;
    }
    g_alarmClockManagement.smartAlarmInfo.delayNum = 0;

    (void)SaveManagementToFile();

    return;
}

static void UpdataAndSaveAlarmClockEnable(uint8 id)
{
    if (id < EVENT_ALARM_CLOCK_MAX) {
        if (g_alarmClockManagement.eventAlarmInfo[id].repeatPeriod == 0) {
            g_alarmClockManagement.eventAlarmInfo[id].alarmClockEnable = false;
        }
    } else if (id == EVENT_ALARM_CLOCK_MAX) {
        if (g_alarmClockManagement.smartAlarmInfo.repeatPeriod == 0) {
            g_alarmClockManagement.smartAlarmInfo.alarmClockEnable = false;
        }
    }

    (void)SaveManagementToFile();

    return;
}

static int32 DelayCurAlarmClock(uint8 alarmId)
{
    uint8 id;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DelayCurAlarmClock !!");
    id = alarmId;

    if (id < EVENT_ALARM_CLOCK_MAX &&
        g_alarmClockManagement.eventAlarmInfo[id].alarmType == TYPE_EVENT_ALARM_CLOCK) {
        g_alarmClockManagement.eventAlarmInfo[id].delayNum++;
        g_alarmClockManagement.eventAlarmInfo[id].alarmClockEnable = true;
        if (g_alarmClockManagement.eventAlarmInfo[id].delayNum <= DELAY_COUNT_MAX) {
            (void)SaveManagementToFile();
            (void)UpdataListAlarmToday(&g_alarmClockManagement.eventAlarmInfo[id]);
        }
    } else if (id == EVENT_ALARM_CLOCK_MAX &&
        g_alarmClockManagement.smartAlarmInfo.alarmType == TYPE_SMART_ALARM_CLOCK) {
        g_alarmClockManagement.smartAlarmInfo.delayNum++;
        g_alarmClockManagement.smartAlarmInfo.alarmClockEnable = true;
        if (g_alarmClockManagement.smartAlarmInfo.delayNum <= DELAY_COUNT_MAX) {
            (void)SaveManagementToFile();
            (void)UpdataListAlarmToday(&g_alarmClockManagement.smartAlarmInfo);
        }
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DelayCurAlarmClock alarmClockId is invalid, alarmClockId = %u!!", id);
        return STATUS_ALARM_ID_INVALID;
    }

    return STATUS_OK;
}

static int32 CancelCurAlarmClock(uint8 alarmId)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CancelCurAlarmClock !!");
    return DeleteAndUpdataListAlarmToday(alarmId);
}
/* public */
void AlmmgrInitManagement(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrInitManagement !!");
    (void)AlarmTimerInit();
    (void)SetZeroAlarm();
    (void)EmptyManagement();
    (void)LoadManagementFromFile();
    UpdataAndSaveAlarmClockDelayNum();
    (void)EmptyListAlarmToday();
    (void)CreateListAlarmToday();
    (void)SortListAlarmToday();
    (void)DisplayListAlarmToday();
    (void)SetFirstAlarmToday(TYPE_CUR_NODE_NO_SKIP);

    return;
}

void AlmmgrAlarmClockEventProcess(const AlarmClockQueueMsg *alarmClockMsg)
{
    if (alarmClockMsg == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAlarmClockEventProcess alarmClockMsg is invalid!!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrAlarmClockEventProcess type = %lu ", alarmClockMsg->eventType);

    switch (alarmClockMsg->eventType) {
        case EVENT_ALARM_RING:
            EventAlarmRingProcess();
            break;
        case EVENT_ALARM_RING_CANCEL:
            CancelCurAlarmClock((uint8)(alarmClockMsg->data).value);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "id=%u", (uint8)(alarmClockMsg->data).value);
            break;
        case EVENT_ALARM_RING_DELAY:
            DelayCurAlarmClock((uint8)(alarmClockMsg->data).value);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "id=%u", (uint8)(alarmClockMsg->data).value);
            break;
        case EVENT_WECHAT_MSG_ARRIVED:
        case EVENT_CALL_INCOMING:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "eventType not support");
            break;
        default:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "eventType error type = %x ", alarmClockMsg->eventType);
            break;
    }

    return;
}

int32 AlmmgrAddAlarmClock(const AlarmClockInfo *info)
{
    uint8 id;
    uint32 size;
    int32 ret;

    ret = AlarmInfoCheck(info);
    if (ret != STATUS_OK) {
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock alarmTime!!");
    size = sizeof(AlarmClockInfo);
    id = info->alarmClockId;
    if (info->alarmType == TYPE_EVENT_ALARM_CLOCK &&
        g_alarmClockManagement.eventAlarmInfo[id].alarmType == TYPE_INVALID_ALARM_CLOCK) {
        if (id < EVENT_ALARM_CLOCK_MAX && g_alarmClockManagement.eventAlarmInfo[id].alarmClockEnable != TRUE) {
            ret = memcpy_s(&g_alarmClockManagement.eventAlarmInfo[id], size, info, size);
            if (ret != EOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock memcpy_s failed, ret = %d!!", ret);
                return STATUS_SEC_FUN_ERROR;
            }
            g_alarmClockManagement.eventAlarmCnt++;
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock invalid alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else if (info->alarmType == TYPE_SMART_ALARM_CLOCK &&
        g_alarmClockManagement.smartAlarmInfo.alarmType == TYPE_INVALID_ALARM_CLOCK) {
        if (id == EVENT_ALARM_CLOCK_MAX && g_alarmClockManagement.smartAlarmInfo.alarmClockEnable != TRUE) {
            ret = memcpy_s(&g_alarmClockManagement.smartAlarmInfo, size, info, size);
            if (ret != EOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock memcpy_s failed, ret = %d!!", ret);
                return STATUS_SEC_FUN_ERROR;
            }
            g_alarmClockManagement.smartAlarmCnt++;
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock invalid alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock invalid alarmType = %u!!", info->alarmType);
        return STATUS_ALARM_TYPE_INVALID;
    }

    (void)SaveManagementToFile();
    (void)AddAndUpdataListAlarmToday(info);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrAddAlarmClock end!!");
    return STATUS_OK;
}

int32 AlmmgrUpdataAlarmClock(const AlarmClockInfo *info)
{
    int32 ret = AlarmInfoCheck(info);
    if (ret != STATUS_OK) {
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrUpdataAlarmClock");

    uint32_t size = sizeof(AlarmClockInfo);
    uint8_t id = info->alarmClockId;
    if (info->alarmType == TYPE_EVENT_ALARM_CLOCK &&
        g_alarmClockManagement.eventAlarmInfo[id].alarmType == TYPE_EVENT_ALARM_CLOCK) {
        if (id < EVENT_ALARM_CLOCK_MAX) {
            ret = memcpy_s(&g_alarmClockManagement.eventAlarmInfo[id], size, info, size);
            if (ret != EOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrUpdataAlarmClock memcpy_s failed, ret = %d!!", ret);
                return STATUS_SEC_FUN_ERROR;
            }
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "event alarmClockId is invalid, alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else if (info->alarmType == TYPE_SMART_ALARM_CLOCK &&
        g_alarmClockManagement.smartAlarmInfo.alarmType == TYPE_SMART_ALARM_CLOCK) {
        if (id == EVENT_ALARM_CLOCK_MAX) {
            ret = memcpy_s(&g_alarmClockManagement.smartAlarmInfo, size, info, size);
            if (ret != EOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrUpdataAlarmClock memcpy_s failed, ret = %d!!", ret);
                return STATUS_SEC_FUN_ERROR;
            }
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "smart alarmClockId is invalid, alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "alarmType is invalid, alarmType = %u!!", info->alarmType);
        return STATUS_ALARM_TYPE_INVALID;
    }

    (void)SaveManagementToFile();
    (void)UpdataListAlarmToday(info);

    return STATUS_OK;
}

int32 AlmmgrDeleteAlarmClock(const AlarmClockInfo *info)
{
    int32 ret;
    uint32 size;
    uint8 id;

    ret = AlarmInfoCheck(info);
    if (ret != STATUS_OK) {
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlmmgrDeleteAlarmClock !!");
    size = sizeof(AlarmClockInfo);
    id = info->alarmClockId;
    if (info->alarmType == TYPE_EVENT_ALARM_CLOCK &&
        g_alarmClockManagement.eventAlarmInfo[id].alarmType == TYPE_EVENT_ALARM_CLOCK) {
        if (id < EVENT_ALARM_CLOCK_MAX) {
            (void)memset_s(&g_alarmClockManagement.eventAlarmInfo[id], size, 0, size);
            if (g_alarmClockManagement.eventAlarmCnt >= 1) {
                g_alarmClockManagement.eventAlarmCnt--;
            } else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "eventAlarmCnt invalid, cnt = %u!!",
                    g_alarmClockManagement.eventAlarmCnt);
                return STATUS_ALARM_CNT_ERROR;
            }
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "event alarmClockId is invalid, alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else if (info->alarmType == TYPE_SMART_ALARM_CLOCK &&
        g_alarmClockManagement.smartAlarmInfo.alarmType == TYPE_SMART_ALARM_CLOCK) {
        if (id == EVENT_ALARM_CLOCK_MAX) {
            (void)memset_s(&g_alarmClockManagement.smartAlarmInfo, size, 0, size);
            if (g_alarmClockManagement.smartAlarmCnt >= 1) {
                g_alarmClockManagement.smartAlarmCnt--;
            } else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "smartAlarmCnt invalid, cnt = %u!!",
                    g_alarmClockManagement.smartAlarmCnt);
                return STATUS_ALARM_CNT_ERROR;
            }
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "smart alarmClockId is invalid, alarmClockId = %u!!", id);
            return STATUS_ALARM_ID_INVALID;
        }
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "alarmType is invalid, alarmType = %u!!", info->alarmType);
        return STATUS_ALARM_TYPE_INVALID;
    }

    (void)SaveManagementToFile();
    (void)DeleteAndUpdataListAlarmToday(id);

    return STATUS_OK;
}

int32 AlmmgrGetManagement(AlarmClockManagement *management)
{
    int32 ret;
    uint32 size = sizeof(AlarmClockManagement);

    if (management == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrGetManagement management nullptr!!");
        return STATUS_POINTER_NULL;
    }

    ret = memcpy_s(management, size, &g_alarmClockManagement, size);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlmmgrGetManagement memcpy_s failed, ret = %d!!", ret);
        return STATUS_SEC_FUN_ERROR;
    }
    return STATUS_OK;
}

uint16 AlmmgrGetEventAlarmCnt(void)
{
    return g_alarmClockManagement.eventAlarmCnt;
}

uint16 AlmmgrGetSmartAlarmCnt(void)
{
    return g_alarmClockManagement.smartAlarmCnt;
}

int32 GetEventAlarmInfo(uint8 id, AlarmClockInfo *info)
{
    int32 ret;
    uint32 size = sizeof(AlarmClockInfo);

    if (id >= EVENT_ALARM_CLOCK_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetEventAlarmInfo id invalid!!");
        return STATUS_ALARM_ID_INVALID;
    }
    if (info == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetEventAlarmInfo info nullptr!!");
        return STATUS_POINTER_NULL;
    }

    ret = memcpy_s(info, size, &g_alarmClockManagement.eventAlarmInfo[id], size);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetEventAlarmInfo memcpy_s failed, ret = %d!!", ret);
        return STATUS_SEC_FUN_ERROR;
    }
    return STATUS_OK;
}

int32 GetSmartAlarmInfo(AlarmClockInfo *info)
{
    int32 ret;
    uint32 size = sizeof(AlarmClockInfo);

    if (info == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSmartAlarmInfo info nullptr!!");
        return STATUS_POINTER_NULL;
    }

    ret = memcpy_s(info, size, &g_alarmClockManagement.smartAlarmInfo, size);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSmartAlarmInfo memcpy_s failed, ret = %d!!", ret);
        return STATUS_SEC_FUN_ERROR;
    }
    return STATUS_OK;
}

void EmptyListAlarmToday(void)
{
    g_alarmTodayList.clear();
    return;
}

void DisplayListAlarmToday(void)
{
    std::list< AlarmClockInfoToday>::iterator node;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DisplayListAlarmToday !!");
    for (node = g_alarmTodayList.begin(); node != g_alarmTodayList.end(); node++) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "clock id : %u, type : %u, alarmTime : %lx ",
                      node->alarmClockId, node->alarmType, node->alarmTime);
    }

    return;
}

void UpdataListAndFirstAlarmToday(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdataListAndFirstAlarmToday !!");
    UpdataAndSaveAlarmClockDelayNum();
    (void)EmptyListAlarmToday();
    (void)CreateListAlarmToday();
    (void)SortListAlarmToday();
    (void)SetFirstAlarmToday(TYPE_CUR_NODE_NO_SKIP);
    return;
}

/* eg:SetDatetime 2021,12,23,16,52,33 */
int32 SetDatetime(int32 argc, char *argv[])
{
    int32 ret;
    uint32 i;
    AlarmClockEventData data;
    time_t second;
    struct tm setTime;
    struct timeval timeVal;
    struct timezone timeZone;
    char *endPtr = nullptr;
    int32 trans[DATETIME_PARA_NUM];
    /* 0, 5, 8, 11, 14, 17 are offset of year, month, day, hour, minute, second */
    uint32 offset[DATETIME_PARA_NUM] = {0, 5, 8, 11, 14, 17};

    if (argc != 1 || argv[0] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "set_datetime para invalid!!");
        goto help;
    }

    for (i = 1; i <= DATETIME_PARA_NUM; i++) {
        trans[i - 1] = (int)strtol((char *)argv[0] + offset[i - 1], &endPtr, 10); // 底为10
        if (endPtr == argv[0] || trans[i - 1] > LONG_MAX) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "set_datetime argv invalid, trans = %u!!", trans[i - 1]);
            goto help;
        }
    }

    setTime.tm_year = trans[0] - 1900; // 1900 is year
    setTime.tm_mon = trans[1] - 1; // 1 is month
    setTime.tm_mday = trans[2]; // 2 is day
    setTime.tm_hour = trans[3]; // 3 is hour
    setTime.tm_min = trans[4]; // 4 is minute
    setTime.tm_sec = trans[5]; // 5 is second

    second = mktime(&setTime);
    if (second == -1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "set_datetime mktime failed!!");
        return STATUS_TIME_FUN_ERROR;
    }
    ret = gettimeofday(&timeVal, &timeZone);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "set_datetime gettimeofday failed!!");
        return STATUS_TIME_FUN_ERROR;
    }
    timeVal.tv_sec = second;
    timeVal.tv_usec = 0;
    ret = settimeofday(&timeVal, &timeZone);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "set_datetime settimeofday failed!!");
        return STATUS_TIME_FUN_ERROR;
    }
    ret = memcpy_s((void *)&data.timeVal, sizeof(struct timeval), (const void *)&timeVal, sizeof(struct timeval));
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SetDatetime memcpy_s data.timeVal fail！ ret = %d", ret);
        return STATUS_SERVICE_FUN_ERROR;
    }
    AlarmTimerEventPublish(TOPIC_EVENT_UTCTIME_UPDATE, data);

    return STATUS_OK;
help:
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cmd format: SetDatetime Datetime ");
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Datetime format: year,month,day,hour,minute,second ");
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Datetime format \t eg1: 2021,12,23,16,52,33 \t eg2: 2021,02,03,06,02,03 ");
    return STATUS_FUN_PARA_INVALID;
}

/* eg:DispDatetime */
int32 DispDatetime(void)
{
    size_t ret;
    char buff[70]; // 70 is enough
    struct tm curTime;
    time_t second;

    second = time(nullptr);
    (void)gmtime_r((const time_t *)&second, &curTime);

    ret = strftime(buff, sizeof(buff), "%A %c", &curTime);
    if (ret > 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "disp_datetime curTime:%s!!", buff);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "disp_datetime strftime failed!!");
    }

    return STATUS_OK;
}
#ifdef __cplusplus
}
#endif
