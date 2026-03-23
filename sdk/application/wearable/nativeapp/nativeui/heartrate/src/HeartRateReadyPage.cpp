/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateReadyPage
 * Create: 2025-06
 */

#include <string>
#include <cmath>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "ui_resource_heartRate.h"
#include "heartrate/HeartRatePresenter.h"
#include "heartrate/HeartRateReadyPage.h"

namespace OHOS {
static constexpr uint16_t HEARTRATE_BUTTON_POSITION_X = 188;
static constexpr uint16_t HEARTRATE_BUTTON_WIDTH = 96;
static constexpr uint16_t HEARTRATE_BUTTON_HEIGHT = 96;
static constexpr uint16_t HEARTRATE_BUTTON_TWO_POSITION_Y = 288;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_X = 65;
static constexpr uint16_t HEARTRATE_LABEL3_POSITION_Y = 150;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_POSITION_X = 110;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_POSITION_Y = 200;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_WIDTH = 424;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_HEIGHT = 424;
static constexpr uint16_t HEARTRATE_LABLETIPSSATRT_FONT_SIZE = 36;
static constexpr uint16_t HEARTRATE_LABLEBACK_POSITION_X = 218;
static constexpr uint16_t HEARTRATE_LABLEBACK_POSITION_Y = 318;
static constexpr uint16_t HEARTRATE_LABLEBACK_WIDTH = 50;
static constexpr uint16_t HEARTRATE_LABLEBACK_HEIGHT = 50;
static constexpr uint16_t HEARTRATE_LABLEBACK_FONT_SIZE = 30;
static constexpr uint16_t HEARTRATE_PLAYBACK_POSITION_X = 140;
static constexpr uint16_t HEARTRATE_PLAYBACK_POSITION_Y = 250;
static constexpr uint16_t HEARTRATE_PLAYBACK_WIDTH = 172;
static constexpr uint16_t HEARTRATE_PLAYBACK_HEIGHT = 172;
static constexpr uint16_t HEARTRATE_PLAYBACK_VENTER_POSITION = 86;
static constexpr uint32_t HEARTRATE_PLAYBACK_RANGE = 100000;
static constexpr uint16_t HEARTRATE_PLAYBACK_RADIUS = 48;
static constexpr uint16_t HEARTRATE_PLAYBACK_START_ANGLE = 360;
static constexpr uint16_t HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH = 12;
static constexpr uint16_t HEARTRATE_TIME_NUM = 3;
static constexpr uint16_t HEARTRATE_PROGRESS_TIME = 3;
static constexpr uint16_t HEARTRATE_TIME_USEC = 1000;
static constexpr char *HEART_START_BUTTON = "startButton";

REGIST_SLICE_PAGE(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_READY_PAGE, HeartRateReadyPage, false);

HeartRateReadyPage::HeartRateReadyPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::HeartRateReadyPage");
}

HeartRateReadyPage::~HeartRateReadyPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
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

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_RATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateReadyPage::~HeartRateReadyPage");
}

void HeartRateReadyPage::OnStart(void *data)
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

    button_ = new UILabelButton();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button_ fail");
        return;
    }
    button_->SetPosition(HEARTRATE_BUTTON_POSITION_X, HEARTRATE_BUTTON_TWO_POSITION_Y);
    button_->Resize(HEARTRATE_BUTTON_WIDTH, HEARTRATE_BUTTON_HEIGHT);
    LOADIMG::LoadBtnImage(button_, HEART_RATE_IMAGE, IMAGE_HEART_RATE_RESET, IMAGE_HEART_RATE_RESET);
    button_->SetViewId(HEART_START_BUTTON);
    button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    button_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    button_->SetVisible(true);
    button_->SetOnClickListener(this);

    labelTips_ = new UILabel();
    if (labelTips_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTips_ fail");
        return;
    }
    labelTips_->SetPosition(HEARTRATE_LABLETIPSSATRT_POSITION_X, HEARTRATE_LABLETIPSSATRT_POSITION_Y,
                            HEARTRATE_LABLETIPSSATRT_WIDTH, HEARTRATE_LABLETIPSSATRT_HEIGHT);
    labelTips_->SetText("请佩戴手表测量");
    labelTips_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTRATE_LABLETIPSSATRT_FONT_SIZE);
    labelTips_->SetVisible(true);

    InitCircleProgress();

    InitAnimator();

    group_->Add(button_);
    group_->Add(labelTips_);
    AddViewToPageContainer(group_);
}

void HeartRateReadyPage::InitAnimator()
{
    callBack_ = new HeartAnimatorCallback(this);
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

int HeartRateReadyPage::InitCircleProgress()
{
    labelProgressTime_ = new UILabel();
    if (labelProgressTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelProgressTime_ fail");
        return OHOS_FAILURE;
    }
    labelProgressTime_->SetPosition(HEARTRATE_LABLEBACK_POSITION_X, HEARTRATE_LABLEBACK_POSITION_Y,
                                    HEARTRATE_LABLEBACK_WIDTH, HEARTRATE_LABLEBACK_HEIGHT);
    labelProgressTime_->SetText("3");
    labelProgressTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTRATE_LABLEBACK_FONT_SIZE);
    labelProgressTime_->SetVisible(false);
    group_->Add(labelProgressTime_);

    timeProgress_ = new UICircleProgress();
    if (timeProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeProgress_ fail");
        return OHOS_FAILURE;
    }
    timeProgress_->SetPosition(HEARTRATE_PLAYBACK_POSITION_X, HEARTRATE_PLAYBACK_POSITION_Y,
                               HEARTRATE_PLAYBACK_WIDTH, HEARTRATE_PLAYBACK_HEIGHT);
    timeProgress_->SetCenterPosition(HEARTRATE_PLAYBACK_VENTER_POSITION, HEARTRATE_PLAYBACK_VENTER_POSITION);
    timeProgress_->SetRange(HEARTRATE_PLAYBACK_RANGE, 0);
    timeProgress_->SetRadius(HEARTRATE_PLAYBACK_RADIUS);
    timeProgress_->SetStartAngle(HEARTRATE_PLAYBACK_START_ANGLE);
    timeProgress_->SetEndAngle(0);
    timeProgress_->SetValue(0);
    timeProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xFF491010);
    timeProgress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    timeProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xFFD72C2E);
    timeProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH);
    timeProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, HEARTRATE_PLAYBACK_STYLE_LINE_WIDTH);
    timeProgress_->SetVisible(false);
    group_->Add(timeProgress_);
    return OHOS_SUCCESS;
}

void HeartRateReadyPage::HeartAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    static int count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    long seconds = current_time.tv_sec - page_->startTime_.tv_sec;
    long micros = current_time.tv_usec - page_->startTime_.tv_usec;
    long time = (seconds * HEARTRATE_TIME_USEC) + (micros / HEARTRATE_TIME_USEC);
    page_->SetLabelProgressTime(time);
    if ((time / HEARTRATE_TIME_USEC) == HEARTRATE_TIME_NUM) {
        page_->StopAnimator();
        NativeAbility::GetInstance().SwitchPageInSlice(HeartRateAllView::HEARTRATE_MEASUREMENT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
}

void HeartRateReadyPage::StopAnimator()
{
    button_->SetVisible(false);
    labelTips_->SetVisible(false);
    timeProgress_->SetVisible(false);
    labelProgressTime_->SetVisible(false);
    animator_->Stop();
}

void HeartRateReadyPage::SetLabelProgressTime(int time)
{
    int progressTime = HEARTRATE_PROGRESS_TIME - (time / HEARTRATE_TIME_USEC);
    std::string strTime = std::to_string(progressTime);
    labelProgressTime_->SetText(strTime.c_str());
    double percentage = static_cast<double>(time) / (HEARTRATE_TIME_NUM * HEARTRATE_TIME_USEC);
    double value = round(percentage * 100000);
    timeProgress_->SetValue(value);
}

bool HeartRateReadyPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        StopAnimator();
        NativeAbility::GetInstance().SwitchPageInSlice(HeartRateAllView::HEARTRATE_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool HeartRateReadyPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), HEART_START_BUTTON) == 0) {
        labelTips_->SetVisible(false);
        labelTips_->SetPosition(HEARTRATE_LABEL3_POSITION_X, HEARTRATE_LABEL3_POSITION_Y);
        labelTips_->SetText("请屏幕朝上偏紧佩戴，                  保持静止");
        labelTips_->SetVisible(true);
        button_->SetVisible(false);
        animator_->Start();
        timeProgress_->SetVisible(true);
        labelProgressTime_->SetVisible(true);
        gettimeofday(&startTime_, nullptr);
    }
    return true;
}
}