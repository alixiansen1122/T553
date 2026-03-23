/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateModel
 * Create: 2025-03-23
 */

#ifndef DATE_MODEL_H
#define DATE_MODEL_H

#include <sys/time.h>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <sstream>
#include <unordered_map>
#include <sys/stat.h>
#include "graphic_timer.h"
#include "ohos_timer.h"
#include "hal_tick.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "cJSON.h"
#include "diag_service.h"
#include "gfx_utils/vector.h"
#ifndef _WIN32
#include "time64.h"
#define DATE_PATH "/user/app/user/ace/data/com.vendor.Weather"
#else
#define DATE_PATH APP_RES_PATH
#endif
namespace OHOS {
static constexpr int16 DATE_COUNT = 24;
static constexpr int16 DATE_HOUR = 12;
static constexpr int16 HOUR_COUNT = 24;
static constexpr uint64_t CENTI_SECOND_UNIT = 100;
static constexpr uint64_t MS_ONE_SECONDS = 1000;
static constexpr uint16_t NUM_THIRTY = 30;
static constexpr uint16_t NUM_TIME_MIN = 60;
static constexpr uint16_t NUM_TIME_HOUR = 3600;
static constexpr uint32_t NUM_TIME_DAY = 86400;
static constexpr uint16_t TIME_END = 16;
static constexpr uint8_t DATE_UNIT_COUNT = 3;
static constexpr uint8_t PAGE_STATUS_0 = 0; // 主页面滑动/按钮退出或在子页面通过按钮退出应用，清除selectButton_记录
static constexpr uint8_t PAGE_STATUS_1 = 1; // 从主页面进入子页面，设置resetPage，用于主页面恢复选择记录
static constexpr uint8_t PAGE_STATUS_2 = 2; // 从子页面回退到主页面，当在主页面未点击任何按钮，不修改selectButton_记录，退出到主页面时清除记录

typedef struct {
    std::string hour;
    std::string eventTitle;
    std::string address;
    std::string index;
} DateMsg;

typedef struct {
    const char *title;
    const char *describetion;
    const char *address;
    const char *index;
    bool visiable;
} DateItem;

typedef struct {
    std::string buttonViewId;
    const char *buttonDay;
    bool isFirst;
    bool isSelect;
} DateButtonItem;

typedef struct {
    std::string eventTitle;
    std::string time;
} DateToMessage;

static const uint16_t DATE_ITEM_COUNT = 10;

class DateModel {
public:
    static DateModel &GetInstance(void);
    virtual ~DateModel();
    void Init();
    bool InitDate();
    bool InitDateVec(std::string dateStr);
    const char *GetViewId();
    void InitDateTimer();
    void DisableDatePhoneTimer();
    void PollFileUntilChange(const std::string& filePath);
    void CalcClockTime(std::string hour, std::string eventTitle);
    void ClearData();
    uint8_t GetConnectStatus();
    std::vector<DateToMessage> dateClockVec_;
    std::vector<DateItem> listDate_;
    std::string titleName_ = "";
    std::string selectButton_ = "";
    int resetPage_ = 0;
    std::string messageIndex_;

private:
    DateModel();
    DateModel(const DateModel &);
    DateModel &operator=(const DateModel &);
    GraphicTimer *startCount_ = nullptr;
    std::unordered_map<std::string, std::vector<DateMsg>> keyValues_;
};
}

#endif // Date_MODEL_H