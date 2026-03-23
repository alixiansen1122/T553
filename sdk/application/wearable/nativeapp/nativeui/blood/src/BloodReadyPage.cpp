/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodReadyPage
 * Create: 2025-06
 */

#include <string>
#include <cmath>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "surface.h"
#include "ui_resource_blood.h"
#include "blood/BloodPresenter.h"
#include "blood/BloodReadyPage.h"

namespace OHOS {
static constexpr uint16_t BLOOD_BUTTON_POSITION_X = 188;
static constexpr uint16_t BLOOD_BUTTON_WIDTH = 96;
static constexpr uint16_t BLOOD_BUTTON_HEIGHT = 96;
static constexpr uint16_t BLOOD_BUTTON_TWO_POSITION_Y = 288;
static constexpr uint16_t BLOOD_LABEL3_POSITION_X = 65;
static constexpr uint16_t BLOOD_LABEL3_POSITION_Y = 150;
static constexpr uint16_t BLOOD_LABLETIPSSATRT_POSITION_X = 110;
static constexpr uint16_t BLOOD_LABLETIPSSATRT_POSITION_Y = 200;
static constexpr uint16_t BLOOD_LABLETIPSSATRT_WIDTH = 424;
static constexpr uint16_t BLOOD_LABLETIPSSATRT_HEIGHT = 424;
static constexpr uint16_t BLOOD_LABLETIPSSATRT_FONT_SIZE = 36;
static constexpr uint16_t BLOOD_LABLEBACK_POSITION_X = 218;
static constexpr uint16_t BLOOD_LABLEBACK_POSITION_Y = 318;
static constexpr uint16_t BLOOD_LABLEBACK_WIDTH = 50;
static constexpr uint16_t BLOOD_LABLEBACK_HEIGHT = 50;
static constexpr uint16_t BLOOD_LABLEBACK_FONT_SIZE = 30;
static constexpr uint16_t BLOOD_PLAYBACK_POSITION_X = 140;
static constexpr uint16_t BLOOD_PLAYBACK_POSITION_Y = 250;
static constexpr uint16_t BLOOD_PLAYBACK_WIDTH = 172;
static constexpr uint16_t BLOOD_PLAYBACK_HEIGHT = 172;
static constexpr uint16_t BLOOD_PLAYBACK_VENTER_POSITION = 86;
static constexpr uint32_t BLOOD_PLAYBACK_RANGE = 100000;
static constexpr uint16_t BLOOD_PLAYBACK_RADIUS = 48;
static constexpr uint16_t BLOOD_PLAYBACK_START_ANGLE = 360;
static constexpr uint16_t BLOOD_PLAYBACK_STYLE_LINE_WIDTH = 12;
static constexpr uint16_t BLOOD_TIME = 3;
static constexpr uint16_t BLOOD_PROGRESS_TIME = 3;
static constexpr uint16_t BLOOD_TIME_USEC = 1000;
static constexpr char *BLOOD_START_BUTTON = "startButton";

REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_READY_PAGE, BloodReadyPage, false);

BloodReadyPage::BloodReadyPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodReadyPage::BloodReadyPage");
}

BloodReadyPage::~BloodReadyPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (buttonStart_ != nullptr) {
        delete buttonStart_;
        buttonStart_ = nullptr;
    }

    if (labelTips_ != nullptr) {
        delete labelTips_;
        labelTips_ = nullptr;
    }

    if (timeProgress_ != nullptr) {
        delete timeProgress_;
        timeProgress_ = nullptr;
    }

    if (labelProgressTime_ != nullptr) {
        delete labelProgressTime_;
        labelProgressTime_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodReadyPage::~BloodReadyPage");
}

void BloodReadyPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new group_ fail");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetOnDragListener(this);

    buttonStart_ = new UILabelButton();
    if (buttonStart_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonStart_ fail");
        return;
    }
    buttonStart_->SetPosition(BLOOD_BUTTON_POSITION_X, BLOOD_BUTTON_TWO_POSITION_Y);
    buttonStart_->Resize(BLOOD_BUTTON_WIDTH, BLOOD_BUTTON_HEIGHT);
    LOADIMG::LoadBtnImage(buttonStart_, BLOOD_IMAGE, IMAGE_BLOOD_RESET, IMAGE_BLOOD_RESET);
    buttonStart_->SetViewId(BLOOD_START_BUTTON);
    buttonStart_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonStart_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    buttonStart_->SetVisible(true);
    buttonStart_->SetOnClickListener(this);
    group_->Add(buttonStart_);

    labelTips_ = new UILabel();
    if (labelTips_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTips_ fail");
        return;
    }
    labelTips_->SetPosition(BLOOD_LABLETIPSSATRT_POSITION_X, BLOOD_LABLETIPSSATRT_POSITION_Y,
                            BLOOD_LABLETIPSSATRT_WIDTH, BLOOD_LABLETIPSSATRT_HEIGHT);
    labelTips_->SetText("请佩戴手表测量");
    labelTips_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLETIPSSATRT_FONT_SIZE);
    labelTips_->SetVisible(true);
    group_->Add(labelTips_);

    InitCircleProgress();

    InitAnimator();

    AddViewToPageContainer(group_);
}

void BloodReadyPage::InitAnimator()
{
    callBack_ = new BloodAnimatorCallback(this);
    if (callBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new callBack_ fail");
        return;
    }
    animator_ = new Animator(callBack_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new animator_ fail");
        return;
    }
}

int BloodReadyPage::InitCircleProgress()
{
    labelProgressTime_ = new UILabel();
    if (labelProgressTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelProgressTime_ fail");
        return OHOS_FAILURE;
    }
    labelProgressTime_->SetPosition(BLOOD_LABLEBACK_POSITION_X, BLOOD_LABLEBACK_POSITION_Y, BLOOD_LABLEBACK_WIDTH,
                                    BLOOD_LABLEBACK_HEIGHT);
    labelProgressTime_->SetText("3");
    labelProgressTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLEBACK_FONT_SIZE);
    labelProgressTime_->SetVisible(true);
    labelProgressTime_->SetVisible(false);
    group_->Add(labelProgressTime_);
    timeProgress_ = new UICircleProgress();
    if (timeProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeProgress_ fail");
        return OHOS_FAILURE;
    }
    timeProgress_->SetPosition(BLOOD_PLAYBACK_POSITION_X, BLOOD_PLAYBACK_POSITION_Y, BLOOD_PLAYBACK_WIDTH,
                               BLOOD_PLAYBACK_HEIGHT);
    timeProgress_->SetCenterPosition(BLOOD_PLAYBACK_VENTER_POSITION, BLOOD_PLAYBACK_VENTER_POSITION);
    timeProgress_->SetRange(BLOOD_PLAYBACK_RANGE, 0);
    timeProgress_->SetRadius(BLOOD_PLAYBACK_RADIUS);
    timeProgress_->SetStartAngle(BLOOD_PLAYBACK_START_ANGLE);
    timeProgress_->SetEndAngle(0);
    timeProgress_->SetValue(0);
    timeProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xFF491010);
    timeProgress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    timeProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xFFD72C2E);
    timeProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, BLOOD_PLAYBACK_STYLE_LINE_WIDTH);
    timeProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, BLOOD_PLAYBACK_STYLE_LINE_WIDTH);
    timeProgress_->SetVisible(false);
    group_->Add(timeProgress_);
    return OHOS_SUCCESS;
}

void BloodReadyPage::BloodAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    static int count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    long seconds = current_time.tv_sec - page_->startTime_.tv_sec;
    long micros = current_time.tv_usec - page_->startTime_.tv_usec;
    long time = (seconds * BLOOD_TIME_USEC) + (micros / BLOOD_TIME_USEC);
    page_->SetLabelProgressTime(time);
    if ((time / BLOOD_TIME_USEC) == BLOOD_TIME) {
        page_->ShowOnMeasurement();
        page_->StopAnimator();
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_MEASUREMENT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
}

void BloodReadyPage::StopAnimator()
{
    animator_->Stop();
}

void BloodReadyPage::SetLabelProgressTime(int time)
{
    int progressTime = BLOOD_PROGRESS_TIME - (time / BLOOD_TIME_USEC);
    std::string strTime = std::to_string(progressTime);
    labelProgressTime_->SetText(strTime.c_str());
    double percentage = static_cast<double>(time) / (BLOOD_TIME * BLOOD_TIME_USEC);
    double value = round(percentage * BLOOD_PLAYBACK_RANGE);
    timeProgress_->SetValue(value);
}

void BloodReadyPage::ShowOnMeasurement()
{
    buttonStart_->SetVisible(false);
    labelTips_->SetVisible(false);
    timeProgress_->SetVisible(false);
    labelProgressTime_->SetVisible(false);
}

bool BloodReadyPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        StopAnimator();
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool BloodReadyPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), BLOOD_START_BUTTON) == 0) {
        labelTips_->SetVisible(false);
        labelTips_->SetPosition(BLOOD_LABEL3_POSITION_X, BLOOD_LABEL3_POSITION_Y);
        labelTips_->SetText("请屏幕朝上偏紧佩戴，                  保持静止");
        labelTips_->SetVisible(true);
        buttonStart_->SetVisible(false);
        animator_->Start();
        timeProgress_->SetVisible(true);
        labelProgressTime_->SetVisible(true);
        gettimeofday(&startTime_, nullptr);
    }
    return true;
}
}
