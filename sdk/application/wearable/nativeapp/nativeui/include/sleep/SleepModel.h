/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepModel
 * Created: 2025-06-05
 */
#ifndef SLEEP_MODEL_H
#define SLEEP_MODEL_H

#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "graphic_timer.h"
#include "ohos_timer.h"
#include "wearable_log.h"
#include "diag_service.h"
#include "msg_center_device.h"
#include "diag_common.h"
#include "sleep/SleepPresenter.h"

namespace OHOS {
    enum SleepType : uint8_t {
        SLEEP_TYPE_DEEP = 1,             // 深睡
        SLEEP_TYPE_LIGHT = 2,            // 浅睡
        SLEEP_TYPE_EYEMOVE = 3,          // 快速动眼
        SLEEP_TYPE_SOBER = 4,            // 清醒
    };
    static const uint16_t SLEEP_DATA_DATE_RANGE = 11;
    static const uint16_t SLEEP_DATA_START_DATA_RANGE = 20;

    #pragma pack(1)
    struct SleepData {
        bool isSended;
        char date[SLEEP_DATA_DATE_RANGE];
        SleepType type;
        char typeSleepStart[SLEEP_DATA_START_DATA_RANGE];
        char typeSleepEnd[SLEEP_DATA_START_DATA_RANGE];
        char sleepStart[SLEEP_DATA_START_DATA_RANGE];
        char sleepEnd[SLEEP_DATA_START_DATA_RANGE];

        SleepData(
            bool isSended,
            const char* date,
            SleepType type,
            const char* typeSleepStart,
            const char* typeSleepEnd,
            const char* sleepStart,
            const char* sleepEnd)
        {
            this->isSended = isSended;
            strncpy_s(this->date, SLEEP_DATA_DATE_RANGE, date, SLEEP_DATA_DATE_RANGE - 1);
            this->type = type;
            strncpy_s(this->typeSleepStart, SLEEP_DATA_START_DATA_RANGE, typeSleepStart,
                SLEEP_DATA_START_DATA_RANGE - 1);
            strncpy_s(this->typeSleepEnd, SLEEP_DATA_START_DATA_RANGE, typeSleepEnd, SLEEP_DATA_START_DATA_RANGE - 1);
            strncpy_s(this->sleepStart, SLEEP_DATA_START_DATA_RANGE, sleepStart, SLEEP_DATA_START_DATA_RANGE - 1);
            strncpy_s(this->sleepEnd, SLEEP_DATA_START_DATA_RANGE, sleepEnd, SLEEP_DATA_START_DATA_RANGE - 1);
        }
    };
    #pragma pack()

    class SleepModel {
    public:
        static SleepModel &GetInstance(void);
        void SendData();
        uint8_t* SerializeSleepData(const SleepData* sleepData, size_t* outSize);
        void SetSleepDataVec(); // 存入睡眠数据接口
        void SetTargetTime(uint32_t minute);
        std::tuple<uint32_t, uint32_t> GetTargetTime();
        void SetData();
        std::tuple<uint32_t, uint32_t> GetSoberingTime();
        std::tuple<uint32_t, uint32_t> GetRapidEyeMovementTime();
        std::tuple<uint32_t, uint32_t> GetLightSleepTime();
        std::tuple<uint32_t, uint32_t> GetDeepSleepTime();
        std::tuple<uint32_t, uint32_t> GetSleepTime();
        void SetSoberingTime();
        void SetRapidEyeMovementTime();
        void SetLightSleepTime();
        void SetDeepSleepTime();
        bool GetRecord();
        uint32_t GetSleepTimeHours();
        uint32_t GetSleepTimeMinutes();
        std::tuple<uint32_t, uint32_t> MinutesToHours(uint32_t minutes);
        uint32_t TimeDiffInMinutes(const std::string& timeStr1, const std::string& timeStr2);
        uint32_t GetDetailData(int type);
        uint32_t GetTotalSleepTime();
        void LoadData();
        std::string GetTodayDate();
        
    private:
        SleepModel();
        virtual ~SleepModel();
        uint32_t sleepTarget_ = 0;
        std::vector<SleepData> sleepDataVector_;
        uint32_t soberingTime_;
        uint32_t eyeMoveTime_;
        uint32_t lightTime_;
        uint32_t deepTime_;
    };
}

#endif // SLEEP_MODEL_H
