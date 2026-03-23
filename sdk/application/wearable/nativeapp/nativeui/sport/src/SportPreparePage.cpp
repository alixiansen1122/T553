/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportPreparePage
 * Create: 2025-06-06
 */

#include <sys/time.h>
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "settings/model/SettingPowerModel.h"
#include "sport/SportView.h"
#include "sport/SportPreparePage.h"

namespace OHOS {
static constexpr uint16_t LOCATING_OVERTIME_SECOND = 2;
static constexpr uint16_t BATTERY_LEVEL1 = 10;
static constexpr uint16_t BATTERY_LEVEL2 = 20;
static constexpr uint16_t BATTERY_LEVEL3 = 40;
static constexpr uint16_t BATTERY_LEVEL4 = 70;
static constexpr uint16_t BATTERY_LEVEL5 = 100;
static constexpr uint16_t HEARTBEAT_IMAGE_X = 351;
static constexpr uint16_t HEARTBEAT_IMAGE_Y = 223;
static constexpr uint16_t HEARTBEAT_LABEL_X = 329;
static constexpr uint16_t HEARTBEAT_LABEL_Y = 182;
static constexpr uint16_t HEARTBEAT_LABEL_W = 102;
static constexpr uint16_t HEARTBEAT_LABEL_H = 48;
static constexpr uint16_t HEARTBEAT_LABEL_FONT = 32;
static constexpr uint16_t BATTERY_ICON_X = 159;
static constexpr uint16_t BATTERY_ICON_Y = 31;
static constexpr uint16_t BATTERY_LABEL_FONT = 20;
static constexpr uint16_t BATTERY_LABEL_X = 163;
static constexpr uint16_t BATTERY_LABEL_Y = 31;
static constexpr uint16_t BATTERY_LABEL_W = 35;
static constexpr uint16_t BATTERY_LABEL_H = 22;
static constexpr uint16_t GPS_X = 48;
static constexpr uint16_t GPS_Y = 225;
static constexpr uint16_t SIGNAL_X = 44;
static constexpr uint16_t SIGNAL_Y = 175;
static constexpr uint16_t TIME_LABEL_FONT = 32;
static constexpr uint16_t TIME_LABEL_X = 214;
static constexpr uint16_t TIME_LABEL_Y = 23;
static constexpr uint16_t TIME_LABEL_W = 82;
static constexpr uint16_t TIME_LABEL_H = 47;
static constexpr uint16_t LOCATING_LABEL_FONT = 32;
static constexpr uint16_t LOCATING_LABEL_X = 165;
static constexpr uint16_t LOCATING_LABEL_Y = 80;
static constexpr uint16_t LOCATING_LABEL_W = 164;
static constexpr uint16_t LOCATING_LABEL_H = 34;
static constexpr uint16_t CENTER_IMAGE_X = 132;
static constexpr uint16_t CENTER_IMAGE_Y = 132;
static constexpr uint16_t GO_BUTTON_X = 63;
static constexpr uint16_t GO_BUTTON_Y = 361;
static constexpr uint16_t GO_BUTTON_W = 328;
static constexpr uint16_t GO_BUTTON_H = 93;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_PREPARE, SportPreparePage, false);

void SportPreparePage::SportPrepareAnimatorCallback::Callback(UIView *view)
{
    UNUSED(view);

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

    time_t now = time(nullptr);
    if (!page_->isOverTime_ && (now - page_->startTime_ > LOCATING_OVERTIME_SECOND)) {
        page_->isOverTime_ = true;
        page_->InitGpsSignalImage();
    }
}

SportPreparePage::~SportPreparePage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (background_ != nullptr) {
        delete background_;
        background_ = nullptr;
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
    if (gpsImageView_ != nullptr) {
        delete gpsImageView_;
        gpsImageView_ = nullptr;
    }
    if (gpsSignalView_ != nullptr) {
        delete gpsSignalView_;
        gpsSignalView_ = nullptr;
    }
    if (heartRateImageView_ != nullptr) {
        delete heartRateImageView_;
        heartRateImageView_ = nullptr;
    }
    if (heartRateLabel_ != nullptr) {
        delete heartRateLabel_;
        heartRateLabel_ = nullptr;
    }
    if (locatingLabel_ != nullptr) {
        delete locatingLabel_;
        locatingLabel_ = nullptr;
    }
    if (goButton_ != nullptr) {
        delete goButton_;
        goButton_ = nullptr;
    }
    if (iconImageView_ != nullptr) {
        delete iconImageView_;
        iconImageView_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportPreparePage::~SportPreparePage");
}

void SportPreparePage::InitGpsSignalImage()
{
    locatingLabel_->SetVisible(false);
    LOADIMG::LoadImageViewImg(gpsSignalView_, SPORT_IMAGE, IMAGE_SPORT_SIGNAL2);
}

void SportPreparePage::InitTimeLableVIew()
{
    timeLabel_ = new UILabel();
    if (timeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage timeLabel_ new fail");
        return;
    }
    timeLabel_->SetPosition(TIME_LABEL_X, TIME_LABEL_Y, TIME_LABEL_W, TIME_LABEL_H);
    timeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TIME_LABEL_FONT);
    group_->Add(timeLabel_);
}

void SportPreparePage::InitBatteryView()
{
    batteryIcon_ = new UIImageView();
    if (batteryIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage batteryIcon_ new fail");
        return;
    }
    batteryIcon_->SetPosition(BATTERY_ICON_X, BATTERY_ICON_Y);
    batteryIcon_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    group_->Add(batteryIcon_);

    batteryLabel_ = new UILabel();
    if (batteryLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage batteryLabel_ new fail");
        return;
    }
    batteryLabel_->SetPosition(BATTERY_LABEL_X, BATTERY_LABEL_Y, BATTERY_LABEL_W, BATTERY_LABEL_H);
    batteryLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BATTERY_LABEL_FONT);
    batteryLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(batteryLabel_);
}

void SportPreparePage::InitCenterView()
{
    iconImageView_ = new UIImageView();
    if (iconImageView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage iconImageView_ new fail");
        return;
    }
    iconImageView_->SetPosition(CENTER_IMAGE_X, CENTER_IMAGE_Y);
    SportType sportType = SportModel::GetInstance().GetSportType();
    if (sportType == SPORT_TYPE_OUTDOOR_RUNNING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_OUTDOOR_RUNNING2);
    } else if (sportType == SPORT_TYPE_INDOOR_RUNNING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_INDOOR_RUNNING2);
    } else if (sportType == SPORT_TYPE_CYCLING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_CYCLING2);
    } else if (sportType == SPORT_TYPE_CLIMBING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_CLIMBING2);
    } else if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_INDOOR_SWIMMING2);
    } else if (sportType == SPORT_TYPE_OUTDOOR_SWIMMING) {
        LOADIMG::LoadImageViewImg(iconImageView_, SPORT_IMAGE, IMAGE_SPORT_OUTDOOR_SWIMMING2);
    }
    group_->Add(iconImageView_);
}

void SportPreparePage::InitGPSView()
{
    locatingLabel_ = new UILabel();
    if (locatingLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage locatingLabel_ new fail");
        return;
    }
    locatingLabel_->SetPosition(LOCATING_LABEL_X, LOCATING_LABEL_Y, LOCATING_LABEL_W, LOCATING_LABEL_H);
    locatingLabel_->SetText("定位中...");
    locatingLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LOCATING_LABEL_FONT);
    locatingLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(locatingLabel_);

    gpsSignalView_ = new UIImageView();
    if (gpsSignalView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage gpsSignalView_ new fail");
        return;
    }
    gpsSignalView_->SetPosition(SIGNAL_X, SIGNAL_Y);
    GpsSignal signal = SportModel::GetInstance().GetGpsSignal();
    if (signal == SPORT_GPS_SIGNAL0) {
        LOADIMG::LoadImageViewImg(gpsSignalView_, SPORT_IMAGE, IMAGE_SPORT_SIGNAL0);
    } else if (signal == SPORT_GPS_SIGNAL1) {
        LOADIMG::LoadImageViewImg(gpsSignalView_, SPORT_IMAGE, IMAGE_SPORT_SIGNAL1);
    } else if (signal == SPORT_GPS_SIGNAL2) {
        LOADIMG::LoadImageViewImg(gpsSignalView_, SPORT_IMAGE, IMAGE_SPORT_SIGNAL2);
    } else if (signal == SPORT_GPS_SIGNAL3) {
        LOADIMG::LoadImageViewImg(gpsSignalView_, SPORT_IMAGE, IMAGE_SPORT_SIGNAL3);
    }
    group_->Add(gpsSignalView_);

    gpsImageView_ = new UIImageView();
    if (gpsImageView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage gpsImageView_ new fail");
        return;
    }
    gpsImageView_->SetPosition(GPS_X, GPS_Y);
    LOADIMG::LoadImageViewImg(gpsImageView_, SPORT_IMAGE, IMAGE_SPORT_GPS);
    group_->Add(gpsImageView_);

    SportType sportType = SportModel::GetInstance().GetSportType();
    if (sportType == SPORT_TYPE_INDOOR_RUNNING || sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        locatingLabel_->SetVisible(false);
        gpsSignalView_->SetVisible(false);
        gpsImageView_->SetVisible(false);
    } else {
        locatingLabel_->SetVisible(true);
        gpsSignalView_->SetVisible(true);
        gpsImageView_->SetVisible(true);
    }
}

void SportPreparePage::InitHeartRateView()
{
    heartRateLabel_ = new UILabel();
    if (heartRateLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage heartRateLabel_ new fail");
        return;
    }
    heartRateLabel_->SetPosition(HEARTBEAT_LABEL_X, HEARTBEAT_LABEL_Y, HEARTBEAT_LABEL_W, HEARTBEAT_LABEL_H);
    heartRateLabel_->SetText("95");  // 桩数据
    heartRateLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTBEAT_LABEL_FONT);
    heartRateLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(heartRateLabel_);

    heartRateImageView_ = new UIImageView();
    if (heartRateImageView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage heartRateImageView_ new fail");
        return;
    }
    heartRateImageView_->SetPosition(HEARTBEAT_IMAGE_X, HEARTBEAT_IMAGE_Y);
    LOADIMG::LoadImageViewImg(heartRateImageView_, SPORT_IMAGE, IMAGE_SPORT_HEART1);
    group_->Add(heartRateImageView_);
}

void SportPreparePage::InitGoButtonView()
{
    goButton_ = new UILabelButton();
    if (goButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage goButton_ new fail");
        return;
    }
    goButton_->SetPosition(GO_BUTTON_X, GO_BUTTON_Y, GO_BUTTON_W, GO_BUTTON_H);
    goButton_->SetViewId(SPORT_PREPARE_GO_BUTTON);
    goButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    goButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    goButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    SportType sportType = SportModel::GetInstance().GetSportType();
    if (sportType == SPORT_TYPE_OUTDOOR_RUNNING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO1, IMAGE_SPORT_GO1);
    } else if (sportType == SPORT_TYPE_INDOOR_RUNNING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO2, IMAGE_SPORT_GO2);
    } else if (sportType == SPORT_TYPE_CYCLING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO3, IMAGE_SPORT_GO3);
    } else if (sportType == SPORT_TYPE_CLIMBING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO4, IMAGE_SPORT_GO4);
    } else if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO6, IMAGE_SPORT_GO6);
    } else if (sportType == SPORT_TYPE_OUTDOOR_SWIMMING) {
        LOADIMG::LoadBtnImage(goButton_, SPORT_IMAGE, IMAGE_SPORT_GO5, IMAGE_SPORT_GO5);
    }
    goButton_->SetOnClickListener(this);
    group_->Add(goButton_);
}

void SportPreparePage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    SportType sportType = SportModel::GetInstance().GetSportType();
    if (sportType == SPORT_TYPE_OUTDOOR_RUNNING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG6);
    } else if (sportType == SPORT_TYPE_INDOOR_RUNNING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG1);
    } else if (sportType == SPORT_TYPE_CYCLING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG4);
    } else if (sportType == SPORT_TYPE_CLIMBING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG3);
    } else if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG2);
    } else if (sportType == SPORT_TYPE_OUTDOOR_SWIMMING) {
        LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG5);
    }
    group_->Add(background_);
}

void SportPreparePage::OnStart(void *data)
{
    startTime_ = time(nullptr);  // 用于在 animator 回调中, 模拟显示 '定位中'
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    InitBackgroundView();
    InitTimeLableVIew();
    InitBatteryView();
    InitCenterView();
    InitGPSView();
    InitHeartRateView();
    InitGoButtonView();
    AddViewToPageContainer(group_);

    callback_ = new SportPrepareAnimatorCallback(this);
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage::OnStart callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportPreparePage::OnStart new Animator animator_ fail");
        return;
    }
}

void SportPreparePage::OnPause()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() != Animator::STOP) {
            animator_->Stop();
        }
    }
}

void SportPreparePage::OnResume()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::STOP) {
            animator_->Start();
        }
    }
}

bool SportPreparePage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_MENU,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportPreparePage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_PREPARE_GO_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_COUNTDOWN,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}