/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockModel
 * Create: 2021-09-22
 */
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "wearable_log.h"
#include "securec.h"
#include "los_mux.h"
#include "samgr_lite.h"
#include "graphic_mutex.h"
#include "UiConfig.h"
#include "alarm/AlarmClockModel.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "graphic_service.h"
#include "player/PlayersModel.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "uiservice/ui_service.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace OHOS {
#define BUFFER_SIZE 15

static GraphicMutex g_alarmClockDataMutex;
constexpr char *ALARM_CLOCK_DATA_PATH = (char *)"/user/res/alarm.bin";
constexpr char *ALARM_CLOCK_DIRECTORY = (char *)"/user/res/";
constexpr int ALARM_CLOCK_MAX = 5;
constexpr uint8_t DELAY_TEM_MIN = 10;
constexpr uint16_t CLOCK_FOUR = 4;
constexpr uint16_t CLOCK_MAX = 5;
constexpr uint16_t CLOCK_MUSIC_SET = 1;
constexpr uint32_t ALARM_CLOCK_NO_MODEL = 0;
constexpr uint32_t CLOCK_NUM = 6;
constexpr uint32_t FIRST_CLOCK_HOUR = 9;
constexpr uint32_t ONE_HOUR_MIN = 60;
constexpr uint32_t TWENTY_FOUR_HOUR = 24;

AlarmClockInfo g_alarmClockData[ALARM_CLOCK_MAX];

AlarmClockModel::AlarmClockModel()
{
    InitAlarmData();
}

AlarmClockModel::~AlarmClockModel()
{
    listAlarmClock.Clear();
    listAlarmClockNum = 0;
    alarmSerial = 0;
    sliceId = nullptr;
    for (uint16_t num = 0; num < CLOCK_MAX; num++) {
        delete currentAlarm[num];
        currentAlarm[num] = nullptr;
    }
    for (uint16_t num = 0; num < CLOCK_NUM; num++) {
        alarmName[num] = nullptr;
    }
}

AlarmClockModel *AlarmClockModel::GetInstance(void)
{
    static AlarmClockModel alarmClockModel;
    return &alarmClockModel;
}

int32_t AlarmClockModel::SaveGlobalAlarmToFile(void)
{
    std::ofstream fd(ALARM_CLOCK_DATA_PATH);
    if (!fd.good()) {
        (void)mkdir(ALARM_CLOCK_DIRECTORY, S_IREAD | S_IWRITE);
        std::ofstream fd(ALARM_CLOCK_DATA_PATH);
        if (!fd.good()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SaveGlobalAlarmToFile open fail");
            fd.close();
            return -1;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "mkdir success!!");
    }
    fd.write((const char *)(&g_alarmClockData), sizeof(g_alarmClockData));
    fd.close();
    return 0;
}

int32_t AlarmClockModel::GetAlarmGlobalData(uint8 id, AlarmClockInfo *info)
{
    int32_t ret;
    if (info == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetAlarmGlobalData info nullptr!!");
        return -1;
    }
    if (g_alarmClockData[id].alarmClockId != id) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "g_alarmClockData id %u error!", id);
        return -1;
    }
    ret = memcpy_s(info, sizeof(AlarmClockInfo), &g_alarmClockData[id], sizeof(AlarmClockInfo));
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetAlarmGlobalData fail. ret=%d", ret);
        return ret;
    }
    return 0;
}

void AlarmClockModel::PushAlarmClockData(AlarmClockQueueMsg alarmClockMsg)
{
    if (listAlarmClockNum >= LIST_ALARM_CLOCK_MAX) {
        return;
    }
    g_alarmClockDataMutex.Lock();
    listAlarmClockNum++;
    listAlarmClock.PushBack(alarmClockMsg);
    g_alarmClockDataMutex.Unlock();
    SaveGlobalAlarmToFile();
}

int32_t AlarmClockModel::FetchedAlarmClockData(AlarmClockQueueMsg *alarmClockMsg)
{
    if (alarmClockMsg == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "FetchedAlarmClockData alarmClockMsg nullptr!!");
        return -1;
    }
    if (listAlarmClock.IsEmpty()) {
        return -1;
    } else {
        g_alarmClockDataMutex.Lock();
        if (listAlarmClockNum >= 1) {
            listAlarmClockNum--;
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "FetchedAlarmClockData listAlarmClockNum invalid, num=%u",
                listAlarmClockNum);
            return -1;
        }
        *alarmClockMsg = listAlarmClock.Front();
        listAlarmClock.PopFront();
        g_alarmClockDataMutex.Unlock();
    }
    return 0;
}

int32_t AlarmClockModel::InitAlarmClockData(void)
{
    (void)memset_s(&g_alarmClockData, sizeof(g_alarmClockData), 0, sizeof(g_alarmClockData));
    std::ifstream fd(ALARM_CLOCK_DATA_PATH);
    if (!fd.good()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitAlarmClockData open fail");
        fd.close();
        return -1;
    } else {
        fd.read((char *)(&g_alarmClockData), sizeof(g_alarmClockData));
        fd.close();
        return 0;
    }
    return 0;
}

int32_t AlarmClockModel::InitAlarmData(void)
{
    int ret;
    int num = 0;
    ret = AlmmgrGetManagement(&managementAlarm);
    if (ret != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::InitAlarmData get managementAlarm error!");
        return -1;
    }
    if (managementAlarm.eventAlarmInfo[0].alarmType == TYPE_INVALID_ALARM_CLOCK) {
        NewFirstTime();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::InitAlarmData new first alarm");
    }
    for (uint16_t i = 0; i < ALARM_CLOCK_MAX; i++) {
        for (int j = 0; j < ALARM_CLOCK_MAX; j++) {
            if (managementAlarm.eventAlarmInfo[j].index == i && managementAlarm.eventAlarmInfo[j].alarmType == TYPE_EVENT_ALARM_CLOCK) {
                currentAlarm[num] = &managementAlarm.eventAlarmInfo[j];
                SetAlarmId(currentAlarm[num]->alarmClockId);
                num++;
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
                    "AlarmClockModel::InitAlarmData get managementAlarm[%u], the unmber of alarm is %u!", j, alarmSerial);
                if (i > 0) {
                    alarmSerial++;
                }
            }
        }
    }
    return 0;
}

int32_t AlarmClockModel::GetAlarmNum(void)
{
    return alarmSerial;
}

int32_t AlarmClockModel::AddAlarmClock(AlarmClockInfo *alarmInfo)
{
    if (alarmInfo == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AddAlarmClock alarmInfo nullptr");
        return -1;
    }
    return AlmmgrAddAlarmClock(alarmInfo);
}

int32_t AlarmClockModel::UpdataAlarmClock(AlarmClockInfo *alarmInfo)
{
    if (alarmInfo == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::UpdataAlarmClock alarmInfo nullptr");
        return -1;
    }
    return AlmmgrUpdataAlarmClock(alarmInfo);
}

int32_t AlarmClockModel::DeleteAlarmClock(AlarmClockInfo *alarmInfo)
{
    if (alarmInfo == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::DeleteAlarmClock alarmInfo nullptr");
        return -1;
    }
    return AlmmgrDeleteAlarmClock(alarmInfo);
}

int32_t AlarmClockModel::GettManagement(AlarmClockManagement *management)
{
    return AlmmgrGetManagement(management);
}

void AlarmClockModel::SetSerialAdd(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetSerialAdd alarmSerial add!");
    alarmSerial++;
}

void AlarmClockModel::SetSerialRdc(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SerSerialRdc alarmSerial reduce!");
    alarmSerial--;
}

bool AlarmClockModel::GetClockTime(int16_t num, uint32_t *clockTime)
{
    if (num >= CLOCK_MAX || clockTime == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockTime alarmTime nullptr");
        return false;
    }
    if (currentAlarm[num] == nullptr || currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::currentAlarm[%d] nullptr!", num);
        return false;
    }
    *clockTime = currentAlarm[num]->alarmTime;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockTime1 get time, time = %x",
        currentAlarm[num]->alarmTime);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockTime1 get time, time id = %u",
        currentAlarm[num]->alarmClockId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockTime1 get time, time index = %u",
        currentAlarm[num]->index);
    return true;
}

bool AlarmClockModel::GetClockWeek(int16_t num, uint8_t *alarmWeek)
{
    if (num >= CLOCK_MAX || alarmWeek == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockWeek alarmWeek nullptr");
        return false;
    }
    if (currentAlarm[num] == nullptr || currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockWeek get week! %p", currentAlarm[num]);
    *alarmWeek = currentAlarm[num]->repeatPeriod;
    return true;
}

bool AlarmClockModel::GetClockStatus(int16_t num, uint8_t *alarmStatus)
{
    if (num >= CLOCK_MAX || alarmStatus == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockStatus alarmStatus == nullptr error!");
        return false;
    }
    if (currentAlarm[num] == nullptr) {
        return false;
    } else if (currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
        return false;
    }
    *alarmStatus = currentAlarm[num]->alarmClockEnable;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockStatus alarm status is %u", *alarmStatus);
    return true;
}

bool AlarmClockModel::GetClockListenerTime(uint32_t &alarmGetTime)
{
    if (currentAlarm[clockListener] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::currentAlarm[%d] nullptr!", clockListener);
        return false;
    }
    alarmGetTime = currentAlarm[clockListener]->alarmTime;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockTime1 get time, time = %u",
        currentAlarm[clockListener]->alarmTime);
    return true;
}

bool AlarmClockModel::GetListenClockId(uint8_t *alarm)
{
    if (alarm == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::GetClockStatus alarm == nullptr error!");
    }
    if (currentAlarm[clockListener] == nullptr) {
        return false;
    } else if (currentAlarm[clockListener]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
        return false;
    }
    *alarm = currentAlarm[clockListener]->alarmClockId;
    return true;
}

bool AlarmClockModel::JudgeClockModel(uint16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::JudgeClockModel");
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        return false;
    } else {
        return true;
    }
}

uint8_t AlarmClockModel::GetAlarmId(void)
{
    for (int num = 0; num < CLOCK_MAX; num++) {
        if (alarmId[num] == 0) {
            return num;
        }
    }
    return 0;
}

void AlarmClockModel::DeleteAlarmId(uint8_t id)
{
    for (int num = 0; num < CLOCK_MAX; num++) {
        if (num == id) {
            alarmId[num] = 0;
        }
    }
    return;
}

void AlarmClockModel::SetAlarmId(uint8_t id)
{
    for (int num = 0; num < CLOCK_MAX; num++) {
        if (num == id) {
            alarmId[num] = 1;
        }
    }
    return;
}

void AlarmClockModel::NewFirstTime(void)
{
    uint32_t timeTem = FIRST_CLOCK_HOUR;
    if (currentAlarm[0] == nullptr) {
        currentAlarm[0] = new AlarmClockInfo();
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::NewFirstTime new alarm_clock!");
    }
    currentAlarm[0]->alarmClockId = GetAlarmId();
    currentAlarm[0]->alarmClockEnable = 0;
    currentAlarm[0]->alarmType = 1;
    currentAlarm[0]->nameLen = strlen(alarmName[0]);
    currentAlarm[0]->index = 0;
    currentAlarm[0]->delayNum = 0;
    if (strcpy_s(currentAlarm[0]->alarmClockName, CLOCK_NAME_LENGTH, alarmName[0]) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::NewFirstTime copy alarmname error!");
        return;
    }
    currentAlarm[0]->alarmTime = (timeTem << 16) | 0x0000; // 第一个闹钟的时间初始化为九点
    currentAlarm[0]->repeatPeriod = 0x7F;                  // 第一个闹钟的日期设置默认为一周七天全选
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "AlarmClockModel::JudgeClockModel currentAlarm[0]->alarm_time = %u, currentAlarm[0]->repeat_period =%u",
        currentAlarm[0]->alarmTime, currentAlarm[0]->repeatPeriod);

    int ret = AddAlarmClock(currentAlarm[0]);
    if (ret != 0) {
        if (currentAlarm[0] != nullptr) {
            delete currentAlarm[0];
            currentAlarm[0] = nullptr;
        }
        return;
    }
    SetAlarmId(currentAlarm[0]->alarmClockId);
    return;
}

void AlarmClockModel::DeleteClockModel(uint16_t num)
{
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::DeleteClockModel delete clock error!");
        return;
    }
    uint8_t clockId = currentAlarm[num]->alarmClockId;
    int status = DeleteAlarmClock(currentAlarm[num]);
    if (status != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::DeleteAlarmClock delete clock error!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "AlarmClockModel::DeleteAlarmClock = %d, currentAlarm[alarmListener] = %p, id = %u!", status, currentAlarm[num],
        currentAlarm[num]->alarmClockId);
    DeleteAlarmId(clockId);
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::DeleteClockModel alarmSerial = %u!", alarmSerial);
    for (; num < alarmSerial; num++) {
        AlarmClockInfo *tempAlarm = currentAlarm[num];
        tempAlarm->index = currentAlarm[num]->index;
        currentAlarm[num] = currentAlarm[num + 1];
        currentAlarm[num]->index = num;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::UpdataAlarmClock index = %u!",
            currentAlarm[num]->index);
        if (currentAlarm[num + 1] == nullptr || currentAlarm[num + 1]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::DeleteClockModel fail");
            currentAlarm[num] = tempAlarm;
            return;
        }
        int ret = UpdataAlarmClock(currentAlarm[num]);
        if (ret != 0) {
            currentAlarm[num] = tempAlarm;
            return;
        }
    }
    currentAlarm[alarmSerial] = nullptr;
}

bool AlarmClockModel::ButtonStatusRing(int16_t num, bool alarmStatus)
{
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        return false;
    } else if (currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
        return false;
    }
    uint8 tempAlarmEnable = currentAlarm[num]->alarmClockEnable;
    if (alarmStatus) {
        currentAlarm[num]->alarmClockEnable = 1;
    } else if (!alarmStatus) {
        currentAlarm[num]->alarmClockEnable = 0;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::ButtonStatusRing alarm[%d] status is %u", num,
        currentAlarm[num]->alarmClockEnable);
    int ret = UpdataAlarmClock(currentAlarm[num]);
    if (ret != 0) {
        currentAlarm[num]->alarmClockEnable = tempAlarmEnable;
        return false;
    }
    return true;
}

void AlarmClockModel::SetDelayAlarm(uint8_t alarm)
{
    AlarmClockQueueMsg *clockQueueMsg = new AlarmClockQueueMsg();
    if (clockQueueMsg == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetDelayAlarm new AlarmClockQueueMsg error");
        return;
    }
    currentAlarm[clockListener]->delayNum++;
    clockQueueMsg->eventType = EVENT_ALARM_RING_DELAY;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetDelayAlarm %u %u", clockQueueMsg->eventType, alarm);
    (clockQueueMsg->data).value = alarm;
    AlmmgrAlarmClockEventProcess(clockQueueMsg);
    return;
}

void AlarmClockModel::SetCancelAlarm(uint8_t alarm)
{
    AlarmClockQueueMsg *clockQueueMsg = new AlarmClockQueueMsg();
    if (clockQueueMsg == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetDelayAlarm new AlarmClockQueueMsg error");
        return;
    }
    clockQueueMsg->eventType = EVENT_ALARM_RING_CANCEL;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetCancelAlarm %u %u", clockQueueMsg->eventType, alarm);
    (clockQueueMsg->data).value = alarm;
    AlmmgrAlarmClockEventProcess(clockQueueMsg);
    return;
}

void AlarmClockModel::SetAlarmTime(uint32_t time_hour, uint32_t time_min, int16_t num)
{
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        currentAlarm[num] = new AlarmClockInfo();
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClcokModel::SetAlarmTime new alarm_clock! num = %d", num);
    }
    uint32 tempAlarmTime = currentAlarm[num]->alarmTime;
    currentAlarm[num]->alarmTime = ((time_hour << 16) | time_min); // 闹钟时间数据的小时位右边16位
    int ret = UpdataAlarmClock(currentAlarm[num]);
    if (ret != 0) {
        currentAlarm[num]->alarmTime = tempAlarmTime;
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmTime currentAlarm[[num]]->alarm_time = %d",
        currentAlarm[num]->alarmTime);
}

void AlarmClockModel::SetAlarmWeek(uint8_t week, int16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek set week!");
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        currentAlarm[num] = new AlarmClockInfo();
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClcokModel::SetAlarmTime new alarm_clock! num = %d", num);
    }
    uint8 tempAlarmWeek = currentAlarm[num]->repeatPeriod;
    currentAlarm[num]->repeatPeriod = week;
    int ret = UpdataAlarmClock(currentAlarm[num]);
    if (ret != 0) {
        currentAlarm[num]->repeatPeriod = tempAlarmWeek;
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek currentAlarm[num]->repeat_period = %u",
        currentAlarm[num]->repeatPeriod);
}

void AlarmClockModel::DelAlarmDelay(int16_t num)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek set week!");
    if (num >= CLOCK_MAX || currentAlarm[num] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClcokModel::SetAlarmTime new alarm_clock! num = %d", num);
        return;
    }
    uint8 tempAlarmDelay = currentAlarm[num]->delayNum;
    currentAlarm[num]->delayNum = 0;
    int ret = UpdataAlarmClock(currentAlarm[num]);
    if (ret != 0) {
        currentAlarm[num]->delayNum = tempAlarmDelay;
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek currentAlarm[num]->delayNum = %u",
        currentAlarm[num]->delayNum);
}

void AlarmClockModel::SetAlarmInfo(uint32_t time_hour, uint32_t time_min, uint8_t week, uint16_t alarmListener,
    uint16_t setListener)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmInfo!");
    if (currentAlarm[alarmListener] == nullptr || (currentAlarm[alarmListener]->alarmType == TYPE_INVALID_ALARM_CLOCK &&
        currentAlarm[alarmListener] != nullptr)) {
        currentAlarm[alarmListener] = new AlarmClockInfo();
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClcokModel::SetAlarmTime new alarm!");
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "hour = %u, minute = %u", time_hour, time_min);
    currentAlarm[alarmListener]->alarmTime = ((time_hour << 16) | time_min); // 闹钟时间数据的小时位右边16位
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::NewAlarmTime currentAlarm[alarmListener]->alarm_time = %u",
        currentAlarm[alarmListener]->alarmTime);
    currentAlarm[alarmListener]->alarmClockId = GetAlarmId();
    currentAlarm[alarmListener]->alarmClockEnable = 1;
    currentAlarm[alarmListener]->alarmType = 1;
    currentAlarm[alarmListener]->delayNum = 0;
    currentAlarm[alarmListener]->nameLen = strlen(alarmName[alarmListener]);
    currentAlarm[alarmListener]->index = alarmListener;
    currentAlarm[alarmListener]->repeatPeriod = week;
    if (strcpy_s(currentAlarm[alarmListener]->alarmClockName, strlen(alarmName[alarmListener]) + 1,
        alarmName[alarmListener]) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmInfo copy alarmname error!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::NewAlarmTime clock name = %s, id = %u",
        currentAlarm[alarmListener]->alarmClockName, currentAlarm[alarmListener]->alarmClockId);
    if (setListener == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek new time!");
        int ret = AddAlarmClock(currentAlarm[alarmListener]);
        if (ret != 0) {
            if (currentAlarm[alarmListener] != nullptr) {
                delete currentAlarm[alarmListener];
                currentAlarm[alarmListener] = nullptr;
            }
            return;
        }
        SetAlarmId(currentAlarm[alarmListener]->alarmClockId);
    } else {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::SetAlarmWeek modify time!");
        UpdataAlarmClock(currentAlarm[alarmListener]);
    }
}

void AlarmClockModel::PhoneCallNotify()
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlarmClockModel::PhoneCallAlarm, this));
}

void AlarmClockModel::BackGroundNotify()
{
    if (GetCurrCallNum() != 0 && GetScoConnectState() == HFP_SCO_STATE_CONNECTED) {
        PhoneCallNotify();
        return;
    }
    AlarmClockNotify();
}

void AlarmClockModel::NextDayNotify()
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlarmClockModel::AlarmNextDayNotify, this));
}

void AlarmClockModel::ResetRingClockStatus()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::ResetRingClockStatus");
    if (currentAlarm[clockListener] == nullptr || currentAlarm[clockListener]->alarmClockEnable == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlarmClockModel::ResetRingClockStatus error! clockListener = %u, alarmClockEnable = %u", clockListener,
            currentAlarm[clockListener]->alarmClockEnable);
        return;
    }
    if (currentAlarm[clockListener]->repeatPeriod == 0) {
        currentAlarm[clockListener]->alarmClockEnable = 0;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::ResetRingClockStatus");
    }
}

void AlarmClockModel::GetPopTimeData(int16 *hour, int16 *min)
{
    *hour = popHour;
    *min = popMin;
}

void AlarmClockModel::AlarmGetClockNum()
{
    struct tm curTime;
    if (AlarmGetCurrentTimeInfo(&curTime) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmTimerCallback AlarmGetCurrentTimeInfo failed!!");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmGetClockNum, hour:%u, min:%u", curTime.tm_hour, curTime.tm_min);

    for (uint16_t num = 0; num < CLOCK_MAX; num++) {
        if (currentAlarm[num] == nullptr || currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum get num(%d)fail!", num);
            return;
        }
        int32_t hour = currentAlarm[num]->alarmTime >> TWO_BYTES_BITS;
        int32_t min = ((currentAlarm[num]->alarmTime & AND_FOR_LOW_TWO_BYTES) + (currentAlarm[num]->delayNum * DELAY_TEM_MIN));
        hour = (hour + min / ONE_HOUR_MIN) % TWENTY_FOUR_HOUR;
        min = min % ONE_HOUR_MIN;
        if (curTime.tm_hour == hour && curTime.tm_min == min) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum %d:%d!",
                currentAlarm[num]->alarmTime >> TWO_BYTES_BITS, currentAlarm[num]->alarmTime & AND_FOR_LOW_TWO_BYTES);
            popHour = hour;
            popMin = min;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum %d:%d!", popHour, popMin);
            clockListener = num;
            return;
        }
    }
}

void AlarmClockModel::GetAlarmCallId(uint8_t *alarm)
{
    if (alarm == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::alarm is nullptr!");
        return;
    }

    struct tm curTime;
    if (AlarmGetCurrentTimeInfo(&curTime) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmTimerCallback AlarmGetCurrentTimeInfo failed!!");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmGetClockNum, hour:%u, min:%u", curTime.tm_hour, curTime.tm_min);

    for (uint16_t num = 0; num < CLOCK_MAX; num++) {
        if (currentAlarm[num] == nullptr || currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum get num fail!");
            return;
        }
        int32_t hour = currentAlarm[num]->alarmTime >> TWO_BYTES_BITS;
        int32_t min = ((currentAlarm[num]->alarmTime & AND_FOR_LOW_TWO_BYTES) + (currentAlarm[num]->delayNum * DELAY_TEM_MIN));
        hour = (hour + min / ONE_HOUR_MIN) % TWENTY_FOUR_HOUR;
        min = min % ONE_HOUR_MIN;
        if (curTime.tm_hour == hour && curTime.tm_min == min) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmGetClockNum %d:%d!",
                currentAlarm[num]->alarmTime >> TWO_BYTES_BITS, currentAlarm[num]->alarmTime & AND_FOR_LOW_TWO_BYTES);
            *alarm = currentAlarm[num]->alarmClockId;
            return;
        }
    }
}

void AlarmClockModel::PopInterCancelMusic()
{
    ScreenTurnOff();
    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&AlarmClockModel::AlarmPlayerStop, AlarmClockModel::GetInstance()));
}

void AlarmClockModel::PopInterPlayMusic()
{
    ScreenTurnOn(-1);
    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&AlarmClockModel::AlarmPlayerStart, AlarmClockModel::GetInstance()));
}

void AlarmClockModel::SetSliceId()
{
    preSliceId = NativeAbility::GetInstance().GetPreSliceId();
}

uint16 AlarmClockModel::GetSliceId()
{
    return preSliceId;
}

int AlarmClockModel::AlarmPlayerStat()
{
    int32_t ret;

    // 设置回调函数
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmPlayerStat playerCtr nullptr");
        return OHOS_FAILURE;
    }
    playerCtr->SetPlayerCallback(playerCallback);
    // 申请资源
    source = std::make_shared<Source>(uri, header);
    if (source.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "source new fail");
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPlayer uri %s.", uri.c_str());
    playerCtr->SetSource(*source);
    playerCtr->SetAudioStreamType(AUDIO_STREAM_ALARM_CLOCK);
    playerCtr->Prepare();

    // 申请焦点
    AudioManager &amIntance = AudioManager::GetInstance();
    sessionId = amIntance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "session invalid");
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MakeSessionId  %d", sessionId);
    playerInterruptListener = std::make_shared<AlarmPlayerListener>();
    if (playerInterruptListener.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerInterruptListener new fail");
        return OHOS_FAILURE;
    }
    interrupt = { AUDIO_STREAM_ALARM_CLOCK, sessionId, playerInterruptListener };
    // 激活音频中断 5s超时
    if (amIntance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivateAudioInterrupt faild");
        return OHOS_FAILURE;
    }
    playerCtr->SetAudioSessionId(sessionId);
    // 播放
    playerCtr->Play();
    return OHOS_SUCCESS;
}

int AlarmClockModel::AlarmPlayerInit(void)
{
    // 本地播放文件路径和状态
    sliceId = (char *)"alarm.mp3"; // 播放音乐的文件名
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayerInit start!");

    std::string strSongName = sliceId;
    std::string pureName = strSongName.substr(0, strSongName.rfind(".")); // 播放音乐的路径需要引用.号
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayerInit, url:%s ", sliceId);
    std::string strSrcMusic(APP_MUSIC_PATH);
    std::string tempStr("/");
    std::string strName = sliceId;
    uri = strSrcMusic + tempStr + strName;
    // 创建 AM

    playerCtr = std::make_shared<Player>();
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCtr new fail");
        return OHOS_FAILURE;
    }
    playerCallback = std::make_shared<AlarmPlayerCallbackImpl>();
    if (playerCallback.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "playerCallback new fail");
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayerInit end!");
    return OHOS_SUCCESS;
}

int AlarmClockModel::AlarmPlayerStop(void)
{
    int32_t ret = 0;
    AudioManager &amIntance = AudioManager::GetInstance();
    if (playerCtr.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmPlayerStop playerCtr nullptr");
        return OHOS_FAILURE;
    }

    ret = playerCtr->Stop();
    playerCtr->Reset();
    playerCtr->Release();
    if (amIntance.DeactivateAudioInterrupt(interrupt) != 0) {
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Reset, ret:%d", ret);
    return OHOS_SUCCESS;
}

int AlarmClockModel::AlarmPlayerStart(void)
{
    AlarmPlayerInit();
    AlarmPlayerStat();
    return OHOS_SUCCESS;
}

void AlarmPlayerCallbackImpl::OnPlaybackComplete()
{
    /* msg center已处理，此处注释 */
    // GraphicService::GetInstance()->PostGraphicEvent(
    //     std::bind(&OHOS::AlarmClockModel::DeactAudioInter, OHOS::AlarmClockModel::GetInstance()));
}

uint16_t AlarmClockModel::JudgePreSlice(void)
{
    uint32_t targetId = NativeAbility::GetInstance().GetPreTargetId();
    uint16_t preSlice = static_cast<uint16_t>(targetId & SLICE_MASK);
    uint16_t prePage = static_cast<uint16_t>(targetId >> PAGE_OFFSET);
    if (preSlice == VIEW_ALARM_POP_UP || preSlice < VIEW_SETTING) {
        preSlice = VIEW_MAIN_SAMPLE;
    } else if ((preSlice == VIEW_PHONE_MENU && prePage == PhoneMenuPages::PHONE_MENU_CALLING_PAGE) &&
        (GetCurrCallNum() == 0 || GetScoConnectState() != HFP_SCO_STATE_CONNECTED)) {
        int num;
        PhoneCallInfo callList[MULTI_CALL_MAX_NUME]{};
        GetCallInfoInCallList(callList, MULTI_CALL_MAX_NUME, &num);
        if (GetCurrCallNum() == 1 && callList[0].lastStatus == HFP_HF_CALL_STATE_INCOMING) {
            return preSlice;
        }
        preSlice = VIEW_MAIN_SAMPLE;
    }
    return preSlice;
}

void AlarmClockModel::DeactAudioInter(void)
{
    bool alarmStatus;
    uint8_t alarm = 0;
    alarmStatus = AlarmClockModel::GetInstance()->GetListenClockId(&alarm);
    if (!alarmStatus) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPopUpPresenter::CancelAlarmRing get alarm Id false!");
        return;
    }
    AlarmClockModel::GetInstance()->ResetRingClockStatus();
    AlarmClockModel::GetInstance()->SetDelayAlarm(alarm);
    PopInterCancelMusic();
}

void AlarmPlayerCallbackImpl::OnError(int32_t errorType, int32_t errorCode)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnError test, errorType %d, errorCode %d", errorType, errorCode);
    // 此处实现代码处理错误事件
}

void AlarmPlayerCallbackImpl::OnInfo(int type, int extra)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnInfo test, type %d, extra %d.", type, extra);
    // 此处实现代码处理普通事件
}

void AlarmPlayerCallbackImpl::OnRewindToComplete()
{
    // 此处实现代码处理进度控制完成的事件
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnRewindToComplete test");
}

void AlarmClockModel::AlarmNextDayNotify()
{
    for (uint8_t num = 0; num < CLOCK_MAX; num++) {
        if (currentAlarm[num] == nullptr || currentAlarm[num]->alarmType == TYPE_INVALID_ALARM_CLOCK) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::currentAlarm[%u] is nullptr");
            return;
        }
        currentAlarm[num]->delayNum = 0;
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmNextDayNotify currentAlarm[%u]->delayNum = %u",
            num, currentAlarm[num]->delayNum);
    }
}

void AlarmClockModel::AlarmClockNotify()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::AlarmClockNotify");

    if (!PlayersModel::GetInstance()->GetAudioInitStatus()) {
        PlayersModel::GetInstance()->AudioInit();
    }
    ScreenTurnOn(-1);
    AlarmGetClockNum();
    NativeAbility::GetInstance().ChangeSlice(VIEW_ALARM_POP_UP, TransitionType::TRANSITION_INVALID, PRIORITY_TWO);
    return;
}

void AlarmClockModel::PhoneCallAlarm()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmClockModel::PhoneCallAlarm");
    uint8_t alarm = 0;
    OHOS::AlarmClockModel::GetInstance()->AlarmGetClockNum();
    OHOS::AlarmClockModel::GetInstance()->GetAlarmCallId(&alarm);
    OHOS::AlarmClockModel::GetInstance()->SetDelayAlarm(alarm);
}

uint32 AlarmClockModel::GetAlarmData()
{
    return lastRunTime;
}

uint16 AlarmClockModel::GetEventAlarmCnt(void)
{
    return AlmmgrGetEventAlarmCnt();
}

uint16 AlarmClockModel::GetSmartAlarmCnt(void)
{
    return AlmmgrGetSmartAlarmCnt();
}

bool AlarmPlayerListener::IsPaused()
{
    return isPaused_;
}

bool AlarmPlayerListener::IsDelayed()
{
    return isDelayed_;
}

bool AlarmPlayerListener::IsResumed()
{
    return isResumed_;
}

bool AlarmPlayerListener::IsBeginStopped()
{
    return isBeginStopped_;
}

bool AlarmPlayerListener::IsEndStopped()
{
    return isEndStopped_;
}

static void Stop(void)
{
    uint8_t alarmId = 0;
    bool alarmStatus;
    AlarmClockModel::GetInstance()->PopInterCancelMusic();
    alarmStatus = AlarmClockModel::GetInstance()->GetListenClockId(&alarmId);
    if (!alarmStatus) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlarmPlayerListener::CancelAlarmRing get alarm Id false!");
        return;
    }
    AlarmClockModel::GetInstance()->ResetRingClockStatus();
    AlarmClockModel::GetInstance()->SetDelayAlarm(alarmId);
}

void AlarmPlayerListener::StopAlarm(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AlarmPlayerListener::StopAlarm] start");
    GraphicService::GetInstance()->PostGraphicEvent(Stop);
}

void AlarmPlayerListener::OnInterrupt(int32_t type, int32_t hint)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlarmPlayerListener OnInterrupt go in, type %d, hint %d.", type, hint);
    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
        isDelayed_ = true;
    }

    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
        StopAlarm();
    }
    return;
}
}

void PushAlarmClockDataToModel(AlarmClockQueueMsg alarmClockMsg)
{
    OHOS::AlarmClockModel::GetInstance()->PushAlarmClockData(alarmClockMsg);
}

#ifdef __cplusplus
}
#endif
