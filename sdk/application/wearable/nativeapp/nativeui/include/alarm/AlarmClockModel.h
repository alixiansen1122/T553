/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AlarmClockModel
 * Create: 2021-09-22
 */

#ifndef ALARM_CLOCK_MODEL_H
#define ALARM_CLOCK_MODEL_H

#include "ohos_types.h"
#include "list.h"
#include "alarmclockmanage/AlmmgrAlarmClockManage.h"
#include "player.h"
#include "source.h"
#include "broadcast_feature.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "ChangeSliceListener.h"
#include "msg_center.h"

constexpr uint32_t LIST_ALARM_CLOCK_MAX = 20;
constexpr uint32_t ALARM_DEC_NUM_256 = 256;

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::Source;
using namespace OHOS::Media;
using namespace Audio;
using Audio::AudioManager;

struct AlarmPlayerCaseInfo {
    char sliceId[ALARM_DEC_NUM_256];
    int16_t index;
};

// 播放器
class AlarmPlayerListener : public InterruptListener {
public:
    AlarmPlayerListener() {}
    ~AlarmPlayerListener() {}

    void SetOwner(std::shared_ptr<Player> player);
    void RestFlag();
    bool IsPaused();
    bool IsDelayed();
    bool IsResumed();
    bool IsBeginStopped();
    bool IsEndStopped();
    void StopAlarm();
    void OnInterrupt(int32_t type, int32_t hint) override;

private:
    std::shared_ptr<Player> player_{nullptr};
    bool isPaused_{false};
    bool isDelayed_{false};
    bool isResumed_{false};
    bool isBeginStopped_{false};
    bool isEndStopped_{false};
};

// 播放器回调
class AlarmPlayerCallbackImpl : public OHOS::Media::PlayerCallback {
public:
    AlarmPlayerCallbackImpl() {}
    ~AlarmPlayerCallbackImpl() {}
    void OnPlaybackComplete();
    void OnError(int32_t errorType, int32_t errorCode);
    void OnInfo(int type, int extra);
    void OnRewindToComplete();
};

class AlarmClockModel {
public:
    AlarmClockModel();
    virtual ~AlarmClockModel();
    static AlarmClockModel *GetInstance(void);
    int32_t AddAlarmClock(AlarmClockInfo *alarmInfo);
    int32_t UpdataAlarmClock(AlarmClockInfo *alarmInfo);
    int32_t DeleteAlarmClock(AlarmClockInfo *alarmInfo);
    void SetSerialAdd(void);
    void SetSerialRdc(void);
    uint16 GetEventAlarmCnt(void);
    uint16 GetSmartAlarmCnt(void);
    uint32 GetAlarmData(void);
    bool GetClockTime(int16_t alarmNum, uint32_t *clockTime);
    bool GetClockWeek(int16_t alarmNum, uint8_t *alarmWeek);
    bool GetClockStatus(int16_t num, uint8_t *alarmEnable);
    void GetPopTimeData(int16 *hour, int16* min);
    bool GetClockListenerTime(uint32_t &alarmTime);
    bool GetListenClockId(uint8_t *alarmId);
    void GetAlarmCallId(uint8_t *alarmId);
    bool ButtonStatusRing(int16_t num, bool alarmStatus);
    void SetAlarmWeek(uint8_t week, int16_t alarmNum);
    void SetAlarmTime(uint32 time_hour, uint32 time_min, int16_t alarmNum);
    void SetDelayAlarm(uint8_t alarmId);
    void SetCancelAlarm(uint8_t alarmId);
    void DelAlarmDelay(int16_t num);
    void AlarmClockNotify();
    void PhoneCallAlarm();
    void AlarmNextDayNotify();
    void DeleteClockModel(uint16_t alarmNum);
    void PhoneCallNotify(void);
    void BackGroundNotify(void);
    void NextDayNotify(void);
    void ResetRingClockStatus(void);
    void DeactAudioInter(void);
    void PushAlarmClockData(AlarmClockQueueMsg alarmClockMsg);
    void PopInterCancelMusic(void);
    void PopInterPlayMusic(void);
    void SetAlarmInfo(uint32_t time_hour, uint32_t time_min, uint8 week, uint16_t alarmListener, uint16_t setListener);
    int GetAlarmNum(void);
    void SetSliceId(void);
    uint16_t JudgePreSlice(void);
    uint16 GetSliceId(void);
    void AlarmGetClockNum(void);
    int AlarmPlayerStat();
    int AlarmPlayerInit(void);
    int AlarmPlayerStart();
    int AlarmPlayerStop();
private:
    void SetAlarmId(uint8_t id);
    void DeleteAlarmId(uint8_t id);
    void NewFirstTime(void);
    uint8_t GetAlarmId(void);
    int32_t SaveGlobalAlarmToFile(void);
    int32_t GetAlarmGlobalData(uint8 id, AlarmClockInfo *info);
    int32_t InitAlarmClockData(void);
    int32_t FetchedAlarmClockData(AlarmClockQueueMsg *alarmClockMsg);
    int32_t GettManagement(AlarmClockManagement *management);
    int InitAlarmData(void);
    bool JudgeClockModel(uint16_t alarmNum);
    std::string uri;
    std::shared_ptr<PlayerCallback> playerCallback{nullptr};
    std::map<std::string, std::string> header;
    std::shared_ptr<Source> source{nullptr};
    std::shared_ptr<AlarmPlayerListener> playerInterruptListener{nullptr};
    AudioSession sessionId;
    AudioInterrupt interrupt;
    AlarmClockInfo *currentAlarm[5]{nullptr}; // 闹钟最大个数为5
    std::shared_ptr<Player> playerCtr{nullptr};
    AlarmClockManagement managementAlarm;
    uint8_t alarmId[5]{0}; // 事件闹钟的个数是5个
    uint16_t alarmSerial{0};
    uint16_t clockListener{0};
    uint16 preSliceId{0};
    uint32_t listAlarmClockNum{0};
    uint32_t lastRunTime{0};
    uint16_t popHour{0};
    uint16_t popMin{0};
    char *sliceId{nullptr};
    const char *alarmName[6] = {"clock1", "clock2", "clock3", "clock4", "clock5", "clock6"}; // 闹钟名字的最大个数为6
    List<AlarmClockQueueMsg> listAlarmClock;
};

extern "C" {
    void PushAlarmClockDataToModel(AlarmClockQueueMsg alarmClockMsg);
}
}
#endif // ALARM_CLOCK_MODEL_H
