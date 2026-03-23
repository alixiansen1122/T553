/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportCountdownPage
 * Create: 2025-06-06
 */

#include <sys/time.h>
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportCountdownPage.h"

namespace OHOS {
static constexpr uint16_t TEXT_X = 164;
static constexpr uint16_t TEXT_Y = 154;
static constexpr uint16_t TEXT_W = 132;
static constexpr uint16_t TEXT_H = 132;
static constexpr uint16_t TEXT_FONT = 120;
static constexpr uint16_t PROCESS_X = 90;
static constexpr uint16_t PROCESS_Y = 90;
static constexpr uint16_t PROCESS_W = 273;
static constexpr uint16_t PROCESS_H = 273;
static constexpr uint16_t PROCESS_CENTER_X = 135;
static constexpr uint16_t PROCESS_CENTER_Y = 135;
static constexpr uint16_t PROCESS_MAX_RANGE = 100;
static constexpr uint16_t PROCESS_RADIUS = 120;
static constexpr uint16_t PROCESS_START_ANGLE = 360;
static constexpr uint16_t PROCESS_LINE_WIDTH = 20;
static constexpr uint16_t COUNTDOWN_TIME = 3;
static constexpr uint32_t MICRO_SECONDS_PER_SECOND = 1000000;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_COUNTDOWN, SportCountdownPage, false);

void SportCountdownPage::UpdateCountdownView(uint64_t time)
{
    uint8_t reminTime = COUNTDOWN_TIME - (time / MICRO_SECONDS_PER_SECOND);
    if (reminTime != 0) {
        countdownText_->SetText(std::to_string(reminTime).c_str());
    }
    double precent = static_cast<double>(time) / (COUNTDOWN_TIME * MICRO_SECONDS_PER_SECOND);
    countdownProgress_->SetValue(precent * PROCESS_MAX_RANGE);
}

void SportCountdownPage::SportCountdownAnimatorCallback::Callback(UIView *view)
{
    UNUSED(view);
    struct timeval currTime = {0};
    gettimeofday(&currTime, nullptr);
    uint64_t time = ((uint64_t)currTime.tv_sec * MICRO_SECONDS_PER_SECOND + currTime.tv_usec) -
                    ((uint64_t)page_->startTime_.tv_sec * MICRO_SECONDS_PER_SECOND + page_->startTime_.tv_usec);
    page_->UpdateCountdownView(time);
    if (time / MICRO_SECONDS_PER_SECOND >= COUNTDOWN_TIME) {
        page_->animator_->Stop();
        SportModel::GetInstance().StartSport(true);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_DATA,
            TransitionType::TRANSITION_INVALID, false);
    }
}

SportCountdownPage::~SportCountdownPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (countdownText_ != nullptr) {
        delete countdownText_;
        countdownText_ = nullptr;
    }
    if (countdownProgress_ != nullptr) {
        delete countdownProgress_;
        countdownProgress_ = nullptr;
    }
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportCountdownPage::~SportCountdownPage");
}

void SportCountdownPage::OnStart(void *data)
{
    gettimeofday(&startTime_, nullptr);
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportCountdownPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    countdownText_ = new UILabel();
    if (countdownText_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportCountdownPage countdownText_ new fail");
        return;
    }
    countdownText_->SetPosition(TEXT_X, TEXT_Y, TEXT_W, TEXT_H);
    countdownText_->SetFont(BOLD_VECTOR_FONT_FILENAME, TEXT_FONT);
    countdownText_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    group_->Add(countdownText_);

    countdownProgress_ = new UICircleProgress();
    if (countdownProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportCountdownPage new countdownProgress_ fail");
        return;
    }
    countdownProgress_->SetPosition(PROCESS_X, PROCESS_Y, PROCESS_W, PROCESS_H);
    countdownProgress_->SetCenterPosition(PROCESS_CENTER_X, PROCESS_CENTER_Y);
    countdownProgress_->SetRange(PROCESS_MAX_RANGE, 0);
    countdownProgress_->SetRadius(PROCESS_RADIUS);
    countdownProgress_->SetStartAngle(PROCESS_START_ANGLE);
    countdownProgress_->SetEndAngle(0);
    countdownProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0xff14d3dd);
    countdownProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, PROCESS_LINE_WIDTH);
    countdownProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0xff549490);
    countdownProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, PROCESS_LINE_WIDTH);
    group_->Add(countdownProgress_);
    AddViewToPageContainer(group_);

    callback_ = new SportCountdownAnimatorCallback(this);
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportCountdownPage::OnStart callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportCountdownPage::OnStart new Animator animator_ fail");
        return;
    }
}

void SportCountdownPage::OnPause()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() != Animator::STOP) {
            animator_->Stop();
        }
    }
}

void SportCountdownPage::OnResume()
{
    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::STOP) {
            animator_->Start();
        }
    }
}

bool SportCountdownPage::OnDrag(UIView &view, const DragEvent &event)
{
    return false;
}

bool SportCountdownPage::OnClick(UIView &view, const ClickEvent &event)
{
    return false;
}
}