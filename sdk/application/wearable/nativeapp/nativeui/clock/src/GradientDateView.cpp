/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientDateView
 * Create: 2025-04
 */

#include <vector>
#include "sys/time.h"
#include "clock/GradientDateView.h"
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "clock/ClockResourceImage.h"

namespace OHOS {
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr int16_t HOURLY_SYSTEM = 24;
static constexpr int16_t NUM2 = 2;
static constexpr int16_t SCREEN_HALF_W = HORIZONTAL_RESOLUTION / NUM2;
static constexpr int16_t SCREEN_HALF_H = VERTICAL_RESOLUTION / NUM2;
static constexpr int16_t TIME_SPLIT_X_DANCE = static_cast<int16_t>(0.015419 * HORIZONTAL_RESOLUTION);
static constexpr int16_t TIME_SPLIT_Y_DANCE = static_cast<int16_t>(0.096916 * VERTICAL_RESOLUTION);
static constexpr int16_t TIME_SPLIT_X = SCREEN_HALF_W - TIME_SPLIT_X_DANCE;
static constexpr int16_t TIME_SPLIT_Y = SCREEN_HALF_H - TIME_SPLIT_Y_DANCE;
static constexpr int16_t TIME_X_DANCE = static_cast<int16_t>(0.145374 * HORIZONTAL_RESOLUTION);
static constexpr int16_t TIME_Y_DANCE = static_cast<int16_t>(0.132159 * VERTICAL_RESOLUTION);
static constexpr int16_t TIME_POINT_Y = SCREEN_HALF_H - TIME_Y_DANCE;
static constexpr int16_t HOUR_POINT_X = SCREEN_HALF_W - TIME_X_DANCE;
static constexpr int16_t MINUTE_POINT_X = SCREEN_HALF_W + TIME_X_DANCE;
static constexpr int16_t WEEK_POINT_X_DANCE = static_cast<int16_t>(0.066079 * HORIZONTAL_RESOLUTION);
static constexpr int16_t WEEK_POINT_Y_DANCE = static_cast<int16_t>(0.127753 * VERTICAL_RESOLUTION);
static constexpr int16_t WEEK_POINT_X = SCREEN_HALF_W - WEEK_POINT_X_DANCE;
static constexpr int16_t WEEK_POINT_Y = SCREEN_HALF_H + WEEK_POINT_Y_DANCE;
static constexpr int16_t WEEK_POINT_Y_RECTANGLE = SCREEN_HALF_H + WEEK_POINT_Y_DANCE\
                                                    - static_cast<int16_t>(0.025974 * VERTICAL_RESOLUTION);
static constexpr int16_t DATE_POINT_Y_DANCE = WEEK_POINT_X_DANCE;
static constexpr int16_t DATE_POINT_Y = SCREEN_HALF_H + DATE_POINT_Y_DANCE;
static constexpr int16_t YEAR_POINT_X = SCREEN_HALF_W - static_cast<int16_t>(0.07489 * HORIZONTAL_RESOLUTION);
static constexpr int16_t MONTH_POINT_X = SCREEN_HALF_W + static_cast<int16_t>(0.024229 * HORIZONTAL_RESOLUTION);
static constexpr int16_t DAY_POINT_X =  SCREEN_HALF_W + static_cast<int16_t>(0.101322 * HORIZONTAL_RESOLUTION);
static constexpr int16_t DATE_LEFT_SPLIT_X = SCREEN_HALF_W - static_cast<int16_t>(0.028634 * HORIZONTAL_RESOLUTION);
static constexpr int16_t DATE_RIGHT_SPLIT_X = SCREEN_HALF_W + static_cast<int16_t>(0.048458 * HORIZONTAL_RESOLUTION);
static constexpr int16_t NUM4 = 4;
static constexpr int16_t SPACE = 0;

template <typename T>
void SafeDelete(T*& ptr)
{
    if (ptr != nullptr) {
        delete ptr;
        ptr = nullptr;
    }
}

static const std::vector<int> g_weekVector = {
    PNG_WEEK_SUN,
    PNG_WEEK_MON,
    PNG_WEEK_TUE,
    PNG_WEEK_WED,
    PNG_WEEK_THU,
    PNG_WEEK_FRI,
    PNG_WEEK_SAT,
};

static const std::vector<int> g_dateVector = {
    PNG_DATA_0,
    PNG_DATA_1,
    PNG_DATA_2,
    PNG_DATA_3,
    PNG_DATA_4,
    PNG_DATA_5,
    PNG_DATA_6,
    PNG_DATA_7,
    PNG_DATA_8,
    PNG_DATA_9,
};

static const std::vector<int> g_timeVector = {
    PNG_TIME_0,
    PNG_TIME_1,
    PNG_TIME_2,
    PNG_TIME_3,
    PNG_TIME_4,
    PNG_TIME_5,
    PNG_TIME_6,
    PNG_TIME_7,
    PNG_TIME_8,
    PNG_TIME_9,
};

GradientDateView::GradientDateView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientDateView::GradientDateView");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_OPA, 0);
}

GradientDateView::~GradientDateView()
{
    if (!viewiInitStatus_) {
        return;
    }

    RemoveAll();
    SafeDelete(yearImgView_);
    SafeDelete(monthImgView_);
    SafeDelete(dayImgView_);
    SafeDelete(hourImgView_);
    SafeDelete(minuteImgView_);
    SafeDelete(backgroudImage_);
    ImageCacheManager::GetInstance().UnloadSingleRes(PNG_GRADIENT_CLOCK_RESOURCE);
    for (auto weekImg : weekInfo_) {
        if (weekImg != nullptr) {
            ImageCacheFree(*weekImg);
            delete weekImg;
            weekImg = nullptr;
        }
    }
    for (auto dateImg : dateNumImgInfo_) {
        if (dateImg != nullptr) {
            ImageCacheFree(*dateImg);
            delete dateImg;
            dateImg = nullptr;
        }
    }
}

bool GradientDateView::InitView(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GradientDateView::InitView");
    backgroudImage_ = new UIImageView();
    if (backgroudImage_) {
        backgroudImage_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        backgroudImage_->SetResizeMode(UIImageView::FILL);
        LoadClockImageById(backgroudImage_, PNG_TIME_SCALE);
        Add(backgroudImage_);
    }
    InitWeekImgView();
    InitDateImgViews();
    InitTimeImgViews();
    viewiInitStatus_ = true;
    return true;
}

void GradientDateView::InitWeekImgView(void)
{
    uint16_t length = g_weekVector.size();
    int16_t y = (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) ?\
                                                            WEEK_POINT_Y_RECTANGLE : WEEK_POINT_Y;
    weekImgView_.SetPosition(WEEK_POINT_X, y);
    if (!LoadGradientDateImages(weekInfo_, g_weekVector, length)) {
        return;
    }
    Add(&weekImgView_);
}

void GradientDateView::InitDateImgViews(void)
{
    uint16_t length = g_dateVector.size();
    if (!LoadGradientDateImages(dateNumImgInfo_, g_dateVector, length)) {
        return;
    }
    InitDigitalImgView(yearImgView_, {YEAR_POINT_X, DATE_POINT_Y}, NUM4, dateNumImgInfo_);
    InitDigitalImgView(monthImgView_, {MONTH_POINT_X, DATE_POINT_Y}, NUM2, dateNumImgInfo_);
    InitDigitalImgView(dayImgView_, {DAY_POINT_X, DATE_POINT_Y}, NUM2, dateNumImgInfo_);

    if (LoadClockImageById(&dateSplitLeft_, PNG_DATA_SPLIT)) {
        dateSplitLeft_.SetPosition(DATE_LEFT_SPLIT_X, DATE_POINT_Y);
        Add(&dateSplitLeft_);
    }

    if (LoadClockImageById(&dateSplitRight_, PNG_DATA_SPLIT)) {
        dateSplitRight_.SetPosition(DATE_RIGHT_SPLIT_X, DATE_POINT_Y);
        Add(&dateSplitRight_);
    }
}

void GradientDateView::InitTimeImgViews(void)
{
    uint16_t length = g_timeVector.size();
    if (!LoadGradientDateImages(timeNumImgInfo_, g_timeVector, length)) {
        return;
    }
    InitDigitalImgView(hourImgView_, {HOUR_POINT_X, TIME_POINT_Y}, NUM2, timeNumImgInfo_);
    InitDigitalImgView(minuteImgView_, {MINUTE_POINT_X, TIME_POINT_Y}, NUM2, timeNumImgInfo_);
    if (LoadClockImageById(&timeSplit_, PNG_TIME_SPLIT)) {
        timeSplit_.SetPosition(TIME_SPLIT_X, TIME_SPLIT_Y);
        Add(&timeSplit_);
    }
}

void GradientDateView::InitDigitalImgView(DialDigitalImgView* &dialView, Point pos,
    uint16_t integerLength, std::vector<ImageInfo*> &numImgInfo)
{
    if (dialView != nullptr) {
        delete dialView;
        dialView = nullptr;
    }
    uint16_t numImgLength = numImgInfo.size();
    if (numImgLength == 0) {
        return;
    }
    dialView = new DialDigitalImgView();
    if (!dialView) {
        GRAPHIC_LOGE("dialView malloc memory Failed\n");
        return;
    }

    const OHOS::ImageInfo **dataArray = new const OHOS::ImageInfo *[numImgLength];
    if (!dataArray) {
        GRAPHIC_LOGE("dataArray malloc memory Failed\n");
        return;
    }

    for (size_t i = 0; i < numImgLength; ++i) {
        dataArray[i] = numImgInfo[i];
    }

    dialView->SetNumRes(dataArray, numImgLength);

    delete[] dataArray;
    dialView->SetIntegerLength(integerLength);
    dialView->SetParent(this);
    dialView->SetAlignPos(pos);
    dialView->SetAlign(ImgAlign::CENTER);
    dialView->SetSpace(SPACE);
    Add(dialView);
}

void GradientDateView::UpdateTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t second = tv.tv_sec;
    uint64_t uSecond = tv.tv_usec;
    uint64_t minute = second / MIN_TO_HOUR;
    uint64_t hour = minute / MIN_TO_HOUR + timezone / (MIN_TO_HOUR * MIN_TO_HOUR);
    uint8_t realHour = static_cast<uint8_t>(hour % HOURLY_SYSTEM);
    uint8_t realMinute = static_cast<uint8_t>(minute % MIN_TO_HOUR);

    time_t time_struct = second;
    struct tm *local_time = localtime(&time_struct);
    uint16_t year = local_time->tm_year + 1900;    // tm_year begin 1900
    uint16_t month = local_time->tm_mon + 1;       // tm_mon is 0-11, need + 1
    uint16_t day = local_time->tm_mday;
    uint16_t weekDay = local_time->tm_wday;
    realHour = local_time->tm_hour;
    realMinute = local_time->tm_min;

    if (weekInfo_.size() > 0) {
        weekImgView_.SetSrc(weekInfo_.at(weekDay));
    }

    HandleDigitalData(yearImgView_, static_cast<float>(year));
    HandleDigitalData(monthImgView_, static_cast<float>(month));
    HandleDigitalData(dayImgView_, static_cast<float>(day));
    HandleDigitalData(hourImgView_, static_cast<float>(realHour));
    HandleDigitalData(minuteImgView_, static_cast<float>(realMinute));
}

void GradientDateView::HandleDigitalData(DialDigitalImgView* digitalView, float data)
{
    if (digitalView != nullptr) {
        digitalView->HandleFloatData(data);
    }
}

bool GradientDateView::LoadClockImageById(UIImageView* view, uint32_t resId)
{
    if (view == nullptr) {
        GRAPHIC_LOGE("view == nullptr, Please check!");
        return false;
    }
    FILE* fp = fopen(PNG_GRADIENT_CLOCK_RESOURCE, "rb");
    if (fp == nullptr) {
        GRAPHIC_LOGE("cannot open file: %s", PNG_GRADIENT_CLOCK_RESOURCE);
        return false;
    }
    ImageInfo* imgInfo = ImageCacheManager::GetInstance().
            LoadOneInMultiRes(resId, PNG_GRADIENT_CLOCK_RESOURCE, fp);
    view->SetSrc(imgInfo);
    fclose(fp);
    return true;
}

bool GradientDateView::LoadGradientDateImages(std::vector<ImageInfo*> &imgInfos,
    std::vector<int> resIds, uint16_t length)
{
    FILE* fp = fopen(PNG_GRADIENT_CLOCK_RESOURCE, "rb");
    if (fp == nullptr) {
        GRAPHIC_LOGE("cannot open file: %s", PNG_GRADIENT_CLOCK_RESOURCE);
        return false;
    }

    uint32_t imgSize = sizeof(ImageInfo);
    for (uint16_t i = 0; i < length; i++) {
        ImageInfo* imgInfo =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(resIds[i], PNG_GRADIENT_CLOCK_RESOURCE, fp);
        if (!imgInfo) {
            GRAPHIC_LOGE("LoadOneInMultiRes Failed: resId = %d\n", resIds[i]);
            imgInfos.clear();
            break;
        }
        imgInfos.push_back(imgInfo);
    }
    fclose(fp);
    return true;
}

}