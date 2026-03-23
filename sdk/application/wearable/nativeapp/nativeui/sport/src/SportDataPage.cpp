/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportDataPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "settings/model/SettingPowerModel.h"
#include "sport/SportView.h"
#include "sport/SportDataPage.h"

namespace OHOS {
static constexpr uint16_t TARGET_HALF = 2;
static constexpr uint16_t NUM_ONE = 1;
static constexpr uint16_t NUM_THOUSAND = 1000;
static constexpr uint16_t BATTERY_LEVEL1 = 10;
static constexpr uint16_t BATTERY_LEVEL2 = 20;
static constexpr uint16_t BATTERY_LEVEL3 = 40;
static constexpr uint16_t BATTERY_LEVEL4 = 70;
static constexpr uint16_t BATTERY_LEVEL5 = 100;
static constexpr uint16_t UNIT_SECS_PER_MIN = 60;
static constexpr uint16_t HAND_POSITION_XPOS = 224;
static constexpr uint16_t HAND_POSITION_YPOS = 4;
static constexpr uint16_t HAND_CENTER_XPOS = 3;
static constexpr uint16_t HAND_CENTER_YPOS = 223;
static constexpr uint16_t HAND_WIDTH = 2;
static constexpr uint16_t HAND_HEIGHT = 5;
static constexpr uint16_t HEARTBEAT_MIN = 116;
static constexpr uint16_t HEARTBEAT_MID = 156;
static constexpr uint16_t HEARTBEAT_MAX = 195;
static constexpr uint16_t HEARTBEAT_CONVERT_UNIT = 600;
static constexpr uint16_t HEARTBEAT_BASE1 = 24000;
static constexpr uint16_t HEARTBEAT_BASE2 = 36000;
static constexpr uint16_t HEARTBEAT_IMAGE_X = 132;
static constexpr uint16_t HEARTBEAT_IMAGE_Y = 174;
static constexpr uint16_t BATTERY_ICON_X = 160;
static constexpr uint16_t BATTERY_ICON_Y = 405;
static constexpr uint16_t BATTERY_LABEL_FONT = 20;
static constexpr uint16_t BATTERY_LABEL_X = 164;
static constexpr uint16_t BATTERY_LABEL_Y = 405;
static constexpr uint16_t BATTERY_LABEL_W = 35;
static constexpr uint16_t BATTERY_LABEL_H = 20;
static constexpr uint16_t TIME_LABEL_FONT = 32;
static constexpr uint16_t TIME_LABEL_X = 219;
static constexpr uint16_t TIME_LABEL_Y = 395;
static constexpr uint16_t TIME_LABEL_W = 84;
static constexpr uint16_t TIME_LABEL_H = 47;
static constexpr uint16_t TOP_VALUE_LABEL_FONT = 100;
static constexpr uint16_t VALUE_LABEL_FONT = 56;
static constexpr uint16_t UNIT_LABEL_FONT = 24;
static constexpr uint16_t TOP_VALUE_LABEL_X = 142;
static constexpr uint16_t TOP_VALUE_LABEL_Y = 73;
static constexpr uint16_t TOP_VALUE_LABEL_W = 172;
static constexpr uint16_t TOP_VALUE_LABEL_H = 100;
static constexpr uint16_t TOP_UNIT_LABEL_X = 160;
static constexpr uint16_t TOP_UNIT_LABEL_Y = 173;
static constexpr uint16_t TOP_UNIT_LABEL_W = 166;
static constexpr uint16_t TOP_UNIT_LABEL_H = 25;
static constexpr uint16_t LEFT_VALUE_LABEL_X = 44;
static constexpr uint16_t LEFT_VALUE_LABEL_Y = 212;
static constexpr uint16_t LEFT_VALUE_LABEL_W = 175;
static constexpr uint16_t LEFT_VALUE_LABEL_H = 56;
static constexpr uint16_t LEFT_UNIT_LABEL_X = 44;
static constexpr uint16_t LEFT_UNIT_LABEL_Y = 272;
static constexpr uint16_t LEFT_UNIT_LABEL_W = 175;
static constexpr uint16_t LEFT_UNIT_LABEL_H = 25;
static constexpr uint16_t RIGHT_VALUE_LABEL_X = 227;
static constexpr uint16_t RIGHT_VALUE_LABEL_Y = 212;
static constexpr uint16_t RIGHT_VALUE_LABEL_W = 175;
static constexpr uint16_t RIGHT_VALUE_LABEL_H = 56;
static constexpr uint16_t RIGHT_UNIT_LABEL_X = 227;
static constexpr uint16_t RIGHT_UNIT_LABEL_Y = 272;
static constexpr uint16_t RIGHT_UNIT_LABEL_W = 175;
static constexpr uint16_t RIGHT_UNIT_LABEL_H = 25;
static constexpr uint16_t BOTTOM_VALUE_LABEL_X = 120;
static constexpr uint16_t BOTTOM_VALUE_LABEL_Y = 309;
static constexpr uint16_t BOTTOM_VALUE_LABEL_W = 221;
static constexpr uint16_t BOTTOM_VALUE_LABEL_H = 56;
static constexpr uint16_t BOTTOM_UNIT_LABEL_X = 205;
static constexpr uint16_t BOTTOM_UNIT_LABEL_Y = 369;
static constexpr uint16_t BOTTOM_UNIT_LABEL_W = 50;
static constexpr uint16_t BOTTOM_UNIT_LABEL_H = 25;
static constexpr uint16_t SEP_LINE_X = 225;
static constexpr uint16_t SEP_LINE_Y = 220;
static constexpr uint16_t SEP_LINE_W = 2;
static constexpr uint16_t SEP_LINE_H = 80;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_DATA, SportDataPage, false);

void SportDataPage::SportDataAnimatorCallback::Callback(UIView *view)
{
    UNUSED(view);

    uint16_t heartRate = SportModel::GetInstance().GetHeartRate();
    if (heartRate < HEARTBEAT_MIN) {
        heartRate = HEARTBEAT_BASE2;
    } else if (heartRate < HEARTBEAT_MID) {
        heartRate = (heartRate - HEARTBEAT_MIN) * HEARTBEAT_CONVERT_UNIT + HEARTBEAT_BASE2;
    } else if (heartRate < HEARTBEAT_MAX) {
        heartRate = (heartRate - HEARTBEAT_MID) * HEARTBEAT_CONVERT_UNIT;
    } else {
        heartRate = HEARTBEAT_BASE1;
    }
    page_->sweepClock_->SetSweepTime24Hour(0, 0, heartRate);
    page_->sweepClock_->Invalidate();

    std::string currTime = GetCurrentTimeStr();
    page_->timeLabel_->SetText(currTime.c_str());
    page_->timeLabel_->Invalidate();

    uint8_t battery = SettingPowerModel::GetInstance().GetBatteryPowerLevel();
    page_->batteryLabel_->SetText(std::to_string(battery).c_str());
    page_->batteryLabel_->Invalidate();

    if (battery < BATTERY_LEVEL1) {
        LOADIMG::LoadImageViewImg(page_->batteryIcon_, SPORT_IMAGE, IMAGE_SPORT_BATTERY1);
    } else if (battery < BATTERY_LEVEL2) {
        LOADIMG::LoadImageViewImg(page_->batteryIcon_, SPORT_IMAGE, IMAGE_SPORT_BATTERY2);
    } else if (battery < BATTERY_LEVEL3) {
        LOADIMG::LoadImageViewImg(page_->batteryIcon_, SPORT_IMAGE, IMAGE_SPORT_BATTERY3);
    } else if (battery < BATTERY_LEVEL4) {
        LOADIMG::LoadImageViewImg(page_->batteryIcon_, SPORT_IMAGE, IMAGE_SPORT_BATTERY4);
    } else if (battery <= BATTERY_LEVEL5) {
        LOADIMG::LoadImageViewImg(page_->batteryIcon_, SPORT_IMAGE, IMAGE_SPORT_BATTERY5);
    }

    page_->UpdateDataViewByType();
    page_->CheckTarget();
    page_->CheckRemind();
}

SportDataPage::~SportDataPage()
{
    if (sweepClock_ != nullptr) {
        sweepClock_->RemoveAll();
        delete sweepClock_;
        sweepClock_ = nullptr;
    }
    if (timeLabel_ != nullptr) {
        delete timeLabel_;
        timeLabel_ = nullptr;
    }
    if (batteryIcon_ != nullptr) {
        delete batteryIcon_;
        batteryIcon_ = nullptr;
    }
    if (batteryLabel_ != nullptr) {
        delete batteryLabel_;
        batteryLabel_ = nullptr;
    }
    if (heartRateScale_ != nullptr) {
        delete heartRateScale_;
        heartRateScale_ = nullptr;
    }
    if (heartRatePointer_ != nullptr) {
        delete heartRatePointer_;
        heartRatePointer_ = nullptr;
    }
    if (separateLine_ != nullptr) {
        delete separateLine_;
        separateLine_ = nullptr;
    }
    if (topValueLabel_ != nullptr) {
        delete topValueLabel_;
        topValueLabel_ = nullptr;
    }
    if (topUnitLabel_ != nullptr) {
        delete topUnitLabel_;
        topUnitLabel_ = nullptr;
    }
    if (leftValueLabel_ != nullptr) {
        delete leftValueLabel_;
        leftValueLabel_ = nullptr;
    }
    if (leftUnitLabel_ != nullptr) {
        delete leftUnitLabel_;
        leftUnitLabel_ = nullptr;
    }
    if (rightValueLabel_ != nullptr) {
        delete rightValueLabel_;
        rightValueLabel_ = nullptr;
    }
    if (rightUnitLabel_ != nullptr) {
        delete rightUnitLabel_;
        rightUnitLabel_ = nullptr;
    }
    if (bottomValueLabel_ != nullptr) {
        delete bottomValueLabel_;
        bottomValueLabel_ = nullptr;
    }
    if (bottomUnitLabel_ != nullptr) {
        delete bottomUnitLabel_;
        bottomUnitLabel_ = nullptr;
    }
    if (secondHand_ != nullptr) {
        delete secondHand_;
        secondHand_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportDataPage::~SportDataPage");
}

void SportDataPage::InitTimeLableVIew()
{
    timeLabel_ = new UILabel();
    if (timeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage timeLabel_ new fail");
        return;
    }
    timeLabel_->SetPosition(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_W, TIME_LABEL_H);
    timeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIME_LABEL_FONT);
    timeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    sweepClock_->Add(timeLabel_);
}

void SportDataPage::InitBatteryView()
{
    batteryIcon_ = new UIImageView();
    if (batteryIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage batteryIcon_ new fail");
        return;
    }
    batteryIcon_->SetPosition(BATTERY_ICON_X, BATTERY_ICON_Y);
    batteryIcon_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sweepClock_->Add(batteryIcon_);

    batteryLabel_ = new UILabel();
    if (batteryLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage batteryLabel_ new fail");
        return;
    }
    batteryLabel_->SetPosition(BATTERY_LABEL_X, BATTERY_LABEL_Y, BATTERY_LABEL_W, BATTERY_LABEL_H);
    batteryLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BATTERY_LABEL_FONT);
    batteryLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    sweepClock_->Add(batteryLabel_);
}

void SportDataPage::ShowUpHeartRate()
{
    heartRateScale_ = new UIImageView();
    if (heartRateScale_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage heartRateScale_ new fail");
        return;
    }
    heartRateScale_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(heartRateScale_, SPORT_IMAGE, IMAGE_SPORT_SCALE);
    sweepClock_->Add(heartRateScale_);

    heartRatePointer_ = new UIImageView();
    if (heartRatePointer_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage heartRatePointer_ new fail");
        return;
    }
    heartRatePointer_->SetPosition(HEARTBEAT_IMAGE_X, HEARTBEAT_IMAGE_Y);
    LOADIMG::LoadImageViewImg(heartRatePointer_, SPORT_IMAGE, IMAGE_SPORT_HEART2);
    sweepClock_->Add(heartRatePointer_);

    SportType sportType = SportModel::GetInstance().GetSportType();
    if (sportType == SPORT_TYPE_CLIMBING) {
        heartRatePointer_->SetVisible(false);
    } else {
        heartRatePointer_->SetVisible(true);
    }
}

void SportDataPage::ShowUpTopView()
{
    topValueLabel_ = new UILabel();
    if (topValueLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage topValueLabel_ new fail");
        return;
    }
    topValueLabel_->SetPosition(TOP_VALUE_LABEL_X, TOP_VALUE_LABEL_Y, TOP_VALUE_LABEL_W, TOP_VALUE_LABEL_H);
    topValueLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    topValueLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TOP_VALUE_LABEL_FONT);
    sweepClock_->Add(topValueLabel_);

    topUnitLabel_ = new UILabel();
    if (topUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage topUnitLabel_ new fail");
        return;
    }
    topUnitLabel_->SetPosition(TOP_UNIT_LABEL_X, TOP_UNIT_LABEL_Y, TOP_UNIT_LABEL_W, TOP_UNIT_LABEL_H);
    topUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    topUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNIT_LABEL_FONT);
    sweepClock_->Add(topUnitLabel_);
}

void SportDataPage::ShowUpLeftView()
{
    leftValueLabel_ = new UILabel();
    if (leftValueLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage leftValueLabel_ new fail");
        return;
    }
    leftValueLabel_->SetPosition(LEFT_VALUE_LABEL_X, LEFT_VALUE_LABEL_Y, LEFT_VALUE_LABEL_W, LEFT_VALUE_LABEL_H);
    leftValueLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    leftValueLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, VALUE_LABEL_FONT);
    sweepClock_->Add(leftValueLabel_);

    leftUnitLabel_ = new UILabel();
    if (leftUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage leftUnitLabel_ new fail");
        return;
    }
    leftUnitLabel_->SetPosition(LEFT_UNIT_LABEL_X, LEFT_UNIT_LABEL_Y, LEFT_UNIT_LABEL_W, LEFT_UNIT_LABEL_H);
    leftUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    leftUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNIT_LABEL_FONT);
    sweepClock_->Add(leftUnitLabel_);
}

void SportDataPage::ShowUpRightView()
{
    rightValueLabel_ = new UILabel();
    if (rightValueLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage rightValueLabel_ new fail");
        return;
    }
    rightValueLabel_->SetPosition(RIGHT_VALUE_LABEL_X, RIGHT_VALUE_LABEL_Y, RIGHT_VALUE_LABEL_W, RIGHT_VALUE_LABEL_H);
    rightValueLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    rightValueLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, VALUE_LABEL_FONT);
    sweepClock_->Add(rightValueLabel_);

    rightUnitLabel_ = new UILabel();
    if (rightUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage rightUnitLabel_ new fail");
        return;
    }
    rightUnitLabel_->SetPosition(RIGHT_UNIT_LABEL_X, RIGHT_UNIT_LABEL_Y, RIGHT_UNIT_LABEL_W, RIGHT_UNIT_LABEL_H);
    rightUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    rightUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNIT_LABEL_FONT);
    sweepClock_->Add(rightUnitLabel_);
}

void SportDataPage::ShowUpBottomView()
{
    bottomValueLabel_ = new UILabel();
    if (bottomValueLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage bottomValueLabel_ new fail");
        return;
    }
    bottomValueLabel_->SetPosition(BOTTOM_VALUE_LABEL_X, BOTTOM_VALUE_LABEL_Y, BOTTOM_VALUE_LABEL_W,
                                   BOTTOM_VALUE_LABEL_H);
    bottomValueLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    bottomValueLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, VALUE_LABEL_FONT);
    sweepClock_->Add(bottomValueLabel_);

    bottomUnitLabel_ = new UILabel();
    if (bottomUnitLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage bottomUnitLabel_ new fail");
        return;
    }
    bottomUnitLabel_->SetPosition(BOTTOM_UNIT_LABEL_X, BOTTOM_UNIT_LABEL_Y, BOTTOM_UNIT_LABEL_W, BOTTOM_UNIT_LABEL_H);
    bottomUnitLabel_->SetText("时间");
    bottomUnitLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    bottomUnitLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, UNIT_LABEL_FONT);
    sweepClock_->Add(bottomUnitLabel_);
}

void SportDataPage::ShowUpSeparateLine()
{
    separateLine_ = new UICanvas();
    if (separateLine_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage ShowUpSeparateLine new fail");
        return;
    }
    separateLine_->SetPosition(SEP_LINE_X, SEP_LINE_Y, SEP_LINE_W, SEP_LINE_H);
    separateLine_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    sweepClock_->Add(separateLine_);
}

void SportDataPage::OnStart(void *data)
{
    sweepClock_ = new UISweepClock();
    if (sweepClock_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage sweepClock_ new fail");
        return;
    }
    sweepClock_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    sweepClock_->SetOnDragListener(this);
    sweepClock_->SetDraggable(true);
    sweepClock_->SetTouchable(true);

    secondHand_ = new Image();
    if (secondHand_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "secondHand new fail");
        return;
    }
    LOADIMG::LoadImageImg(secondHand_, SPORT_IMAGE, IMAGE_SPORT_TRIANGLE);
    sweepClock_->SetHandImage(UISweepClock::HandType::SECOND_HAND, *secondHand_,
                              {HAND_POSITION_XPOS, HAND_POSITION_YPOS}, {HAND_CENTER_XPOS, HAND_CENTER_YPOS});
    sweepClock_->SetHandLine(UISweepClock::HandType::HOUR_HAND, {HAND_POSITION_XPOS, HAND_POSITION_YPOS},
                             {HAND_CENTER_XPOS, HAND_CENTER_YPOS}, Color::White(), HAND_WIDTH, HAND_HEIGHT,
                             OPA_TRANSPARENT);
    sweepClock_->SetHandLine(UISweepClock::HandType::MINUTE_HAND, {HAND_POSITION_XPOS, HAND_POSITION_YPOS},
                             {HAND_CENTER_XPOS, HAND_CENTER_YPOS}, Color::White(), HAND_WIDTH, HAND_HEIGHT,
                             OPA_TRANSPARENT);
    sweepClock_->SetWorkMode(UISweepClock::WorkMode::NORMAL);

    ShowUpHeartRate();
    InitTimeLableVIew();
    InitBatteryView();
    ShowUpTopView();
    ShowUpLeftView();
    ShowUpRightView();
    ShowUpBottomView();
    ShowUpSeparateLine();
    AddViewToPageContainer(sweepClock_);

    callback_ = new SportDataAnimatorCallback(this);
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage::OnStart callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportDataPage::OnStart new Animator animator_ fail");
        return;
    }
}

void SportDataPage::OnPause()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() != Animator::STOP) {
            animator_->Stop();
        }
    }
}

void SportDataPage::OnResume()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::STOP) {
            animator_->Start();
        }
    }
}

bool SportDataPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        SportModel::GetInstance().PauseAndReport(true);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_PAUSE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportDataPage::OnClick(UIView &view, const ClickEvent &event)
{
    return false;
}

void SportDataPage::UpdateDataViewByType()
{
    uint32_t elapsedTime = SportModel::GetInstance().GetElapsedTime();
    std::string sportTimeStr = FormatSportTime(elapsedTime);
    bottomValueLabel_->SetText(sportTimeStr.c_str());

    SportType sportType = SportModel::GetInstance().GetSportType();
    sport_info_t *sportInfo = SportModel::GetInstance().GetSportInfo(sportType);
    if (sportType == SPORT_TYPE_OUTDOOR_RUNNING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.heartRate).c_str());
        topUnitLabel_->SetText("心率(次/分钟)");
        leftValueLabel_->SetText(CalculateSpeed(sportInfo->data.distance, elapsedTime).c_str());
        leftUnitLabel_->SetText("配速(/km)");
        rightValueLabel_->SetText(ConvertDistance(sportInfo->data.distance).c_str());
        rightUnitLabel_->SetText("距离(km)");
    } else if (sportType == SPORT_TYPE_INDOOR_RUNNING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.heartRate).c_str());
        topUnitLabel_->SetText("心率(次/分钟)");
        leftValueLabel_->SetText(CalculateSpeed(sportInfo->data.distance, elapsedTime).c_str());
        leftUnitLabel_->SetText("配速(/km)");
        rightValueLabel_->SetText(ConvertDistance(sportInfo->data.distance).c_str());
        rightUnitLabel_->SetText("距离(km)");
    } else if (sportType == SPORT_TYPE_CYCLING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.heartRate).c_str());
        topUnitLabel_->SetText("心率(次/分钟)");
        leftValueLabel_->SetText(CalculateSpeed(sportInfo->data.distance, elapsedTime).c_str());
        leftUnitLabel_->SetText("配速(/km)");
        rightValueLabel_->SetText(ConvertDistance(sportInfo->data.distance).c_str());
        rightUnitLabel_->SetText("距离(km)");
    } else if (sportType == SPORT_TYPE_CLIMBING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.altitude).c_str());
        topUnitLabel_->SetText("海拔(m)");
        leftValueLabel_->SetText(std::to_string(sportInfo->data.ascent).c_str());
        leftUnitLabel_->SetText("累计上升(m)");
        rightValueLabel_->SetText(std::to_string(sportInfo->data.descent).c_str());
        rightUnitLabel_->SetText("累计下降(m)");
    } else if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.heartRate).c_str());
        topUnitLabel_->SetText("心率(次/分钟)");
        leftValueLabel_->SetText(std::to_string(sportInfo->data.calorie).c_str());
        leftUnitLabel_->SetText("热量(kcal)");
        rightValueLabel_->SetText(ConvertDistance(sportInfo->data.distance).c_str());
        rightUnitLabel_->SetText("距离(km)");
    } else if (sportType == SPORT_TYPE_OUTDOOR_SWIMMING) {
        topValueLabel_->SetText(std::to_string(sportInfo->data.heartRate).c_str());
        topUnitLabel_->SetText("心率(次/分钟)");
        leftValueLabel_->SetText(std::to_string(sportInfo->data.calorie).c_str());
        leftUnitLabel_->SetText("热量(kcal)");
        rightValueLabel_->SetText(ConvertDistance(sportInfo->data.distance).c_str());
        rightUnitLabel_->SetText("距离(km)");
    }
}

void SportDataPage::CheckTarget()
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (!sportInfo->isTargetSetted) {
        return;
    }
    if (sportInfo->targetType == SPORT_TARGET_DISTANCE) {
        SportType sportType = SportModel::GetInstance().GetSportType();
        if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
            if (!sportInfo->targetComplete && sportInfo->data.lap >= sportInfo->targetValue) {
                sportInfo->targetComplete = true;
                sportInfo->promptType = SPORT_SET_TYPE_TARGET;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            } else if (!sportInfo->targetHalf && sportInfo->data.lap >= sportInfo->targetValue / TARGET_HALF) {
                sportInfo->targetHalf = true;
                sportInfo->promptType = SPORT_SET_TYPE_TARGET;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            }
        } else {
            if (!sportInfo->targetComplete && sportInfo->data.distance >= sportInfo->targetValue * NUM_THOUSAND) {
                sportInfo->targetComplete = true;
                sportInfo->promptType = SPORT_SET_TYPE_TARGET;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            } else if (!sportInfo->targetHalf &&
                       sportInfo->data.distance >= sportInfo->targetValue * NUM_THOUSAND / TARGET_HALF) {
                sportInfo->targetHalf = true;
                sportInfo->promptType = SPORT_SET_TYPE_TARGET;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            }
        }
    } else if (sportInfo->targetType == SPORT_TARGET_TIME) {
        uint32_t elapsedTime = SportModel::GetInstance().GetElapsedTime();
        if (!sportInfo->targetComplete && elapsedTime >= sportInfo->targetValue * UNIT_SECS_PER_MIN) {
            sportInfo->targetComplete = true;
            sportInfo->promptType = SPORT_SET_TYPE_TARGET;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
        } else if (!sportInfo->targetHalf && elapsedTime >= sportInfo->targetValue * UNIT_SECS_PER_MIN / TARGET_HALF) {
            sportInfo->targetHalf = true;
            sportInfo->promptType = SPORT_SET_TYPE_TARGET;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
        }
    } else if (sportInfo->targetType == SPORT_TARGET_CALORIE) {
        if (!sportInfo->targetComplete && sportInfo->data.calorie >= sportInfo->targetValue) {
            sportInfo->targetComplete = true;
            sportInfo->promptType = SPORT_SET_TYPE_TARGET;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
        } else if (!sportInfo->targetHalf && sportInfo->data.calorie >= sportInfo->targetValue / TARGET_HALF) {
            sportInfo->targetHalf = true;
            sportInfo->promptType = SPORT_SET_TYPE_TARGET;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
        }
    }
}

void SportDataPage::CheckRemind()
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->enableHeartRemind) {
        if (sportInfo->data.heartRate >= GetHeartRateRangeBounder(sportInfo->heartRange) &&
            sportInfo->data.heartRate < GetHeartRateRangeBounder(sportInfo->heartRange + NUM_ONE)) {
            if (!sportInfo->isHeartReminded) {
                sportInfo->isHeartReminded = true;
                sportInfo->promptType = SPORT_SET_TYPE_HEARTRATE;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            }
        } else {
            sportInfo->isHeartReminded = false;
        }
    }

    if (sportInfo->remindType == SPORT_REMIND_INTERVAL_NONE || !sportInfo->isRemindSetted) {
        return;
    } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_DISTANCE) {
        SportType sportType = SportModel::GetInstance().GetSportType();
        if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
            if (sportInfo->data.lap >= (sportInfo->remindTimes + NUM_ONE) * sportInfo->remindValue) {
                sportInfo->remindTimes++;
                sportInfo->promptType = SPORT_SET_TYPE_REMIND;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            }
        } else {
            if (sportInfo->data.distance >=
                (sportInfo->remindTimes + NUM_ONE) * sportInfo->remindValue * NUM_THOUSAND) {
                sportInfo->remindTimes++;
                sportInfo->promptType = SPORT_SET_TYPE_REMIND;
                NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
            }
        }
    } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_TIME) {
        uint32_t elapsedTime = SportModel::GetInstance().GetElapsedTime();
        if (elapsedTime >= (sportInfo->remindTimes + NUM_ONE) * sportInfo->remindValue * UNIT_SECS_PER_MIN) {
            sportInfo->remindTimes++;
            sportInfo->promptType = SPORT_SET_TYPE_REMIND;
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_PROMPT,
                    TransitionType::TRANSITION_INVALID, false);
        }
    }
}
}