/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerCountDown
 * Create: 2025-06-09
 */

#include <cmath>
#include <string>
#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "pwm.h"
#include "main/LoadImg.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "timers/TimerView.h"
#include "timers/TimerPresenter.h"
#include "timers/TimerMainPage.h"
#include "timers/TimerCountDown.h"

namespace OHOS {
static constexpr uint16_t NUM_TEN = 10;
static constexpr uint16_t TIMING_TITLE_POSITION_X = 170;
static constexpr uint16_t TIMING_TITLE_POSITION_Y = 110;
static constexpr uint16_t TIMING_TITLE_SIZE_WIDTH = 114;
static constexpr uint16_t TIMING_TITLE_SIZE_HEIGHT = 38;
static constexpr uint16_t TIMING_TITLE_FONT = 38;
static constexpr uint16_t TIMING_LABEL_CURTIME_XPOS = 177;
static constexpr uint16_t TIMING_LABEL_CURTIME_YPOS = 54;
static constexpr uint16_t TIMING_LABEL_CURTIME_WIDTH = 100;
static constexpr uint16_t TIMING_LABEL_CURTIME_HEIGHT = 30;
static constexpr uint16_t TIMING_LABEL_CURTIME_FONT_SIZE = 28;
static constexpr uint16_t TIMING_START_IMAGE_XPOS = 251;
static constexpr uint16_t TIMING_START_IMAGE_YPOS = 306;
static constexpr uint16_t TIMING_START_IMAGE_SIZE = 96;
static constexpr uint16_t TIMING_RESET_IMAGE_XPOS = 107;
static constexpr uint16_t TIMING_RESET_IMAGE_YPOS = 306;
static constexpr uint16_t TIMING_RESET_IMAGE_SIZE = 96;
static constexpr uint16_t TIMING_TIMER_TIME_XPOS = 85;
static constexpr uint16_t TIMING_TIMER_TIME_YPOS = 182;
static constexpr uint16_t TIMING_TIMER_TIME_WIDTH = 322;
static constexpr uint16_t TIMING_TIMER_TIME_HEIGHT = 90;
static constexpr uint16_t TIMING_TIMER_TIME_FONT_SIZE = 72;
static constexpr uint16_t HOUR_TIME = 3600;
static constexpr uint16_t MIN_TIME = 60;
static constexpr uint16_t TIMING_CIRCLE_LINE_WIDTH = 10;
static constexpr uint16_t TIMING_CIRCLE = 227;
static constexpr uint16_t CIRCLE_RADIUS = 222;
static constexpr uint16_t TIMING_CIRCLE_END = 360;
static constexpr uint32_t TIMING_CIRCLE_RANGE = 100000;
static constexpr uint16_t MS_ONE_SECONDS = 1000;
static constexpr uint16_t REMIND_MIN_MS = 16;
static constexpr char *RESET_OR_EXIT_BUTTON  = "resetOrExitButton";
static constexpr char *START_OR_PAUSE_BUTTON  = "startOrPauseButton";

static std::string GetCurrentTimeStr()
{
    struct tm localTime;
    time_t seconds = time(nullptr);
    localtime_r(&seconds, &localTime);
    std::string currTime;
    if (localTime.tm_hour < NUM_TEN) {
        currTime += "0";
    }
    currTime += std::to_string(localTime.tm_hour) + ":";
    if (localTime.tm_min < NUM_TEN) {
        currTime += "0";
    }
    currTime += std::to_string(localTime.tm_min);
    return currTime;
}

void TimerCountDown::OnCreateView(void *data)
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImg_ fail");
        return;
    }
    LOADIMG::LoadImageViewImg(bgImg_, TIMER_IMAGE, IMAGE_TIMER_BG);
    bgImg_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    // 标题
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(TIMING_TITLE_POSITION_X, TIMING_TITLE_POSITION_Y);
    labelTitle_->Resize(TIMING_TITLE_SIZE_WIDTH, TIMING_TITLE_SIZE_HEIGHT);
    labelTitle_->SetText("计时器");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, TIMING_TITLE_FONT);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    currentTime_ = new UILabel();
    if (currentTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "currentTime_ fail");
        return;
    }
    currentTime_->SetPosition(TIMING_LABEL_CURTIME_XPOS, TIMING_LABEL_CURTIME_YPOS);
    currentTime_->Resize(TIMING_LABEL_CURTIME_WIDTH, TIMING_LABEL_CURTIME_HEIGHT);
    currentTime_->SetFont(BOLD_VECTOR_FONT_FILENAME, TIMING_LABEL_CURTIME_FONT_SIZE);
    currentTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    currentTime_->SetVisible(true);
    
    fragmentView_.Add(bgImg_);
    fragmentView_.Add(labelTitle_);
    fragmentView_.Add(currentTime_);

    CircleProgress();
    CreateButton();
    CreateTimerTime();
    InitAnimator();

    currentTimeCallback_ = new TimeAnimatorCallback();
    if (currentTimeCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "currentTimeCallback_ fail");
        return;
    }
    currentTimeAnimator_ = new Animator(currentTimeCallback_, currentTime_, 0, true);
    if (currentTimeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "currentTimeAnimator_ fail");
        return;
    }
    currentTimeAnimator_->Start();
}

void TimerCountDown::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (timerClock_ != nullptr) {
        delete timerClock_;
        timerClock_ = nullptr;
    }

    if (resetOrExitImg_ != nullptr) {
        delete resetOrExitImg_;
        resetOrExitImg_ = nullptr;
    }

    if (startOrPauseImg_ != nullptr) {
        delete startOrPauseImg_;
        startOrPauseImg_ = nullptr;
    }

    if (circleProgress_ != nullptr) {
        delete circleProgress_;
        circleProgress_ = nullptr;
    }

    if (currentTime_ != nullptr) {
        delete currentTime_;
        currentTime_ = nullptr;
    }

    if (currentTimeCallback_ != nullptr) {
        delete currentTimeCallback_;
        currentTimeCallback_ = nullptr;
    }

    if (currentTimeAnimator_ != nullptr) {
        delete currentTimeAnimator_;
        currentTimeAnimator_ = nullptr;
    }

    if (countDownTimecallBack_ != nullptr) {
        delete countDownTimecallBack_;
        countDownTimecallBack_ = nullptr;
    }

    if (countDownTimeanimator_ != nullptr) {
        delete countDownTimeanimator_;
        countDownTimeanimator_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(TIMER_IMAGE);
}

void TimerCountDown::OnResume()
{
    if (TimerModel::GetInstance().GetExistTimer()) {
        SetAnimatorStatus(true);
        TimerModel::GetInstance().SetExistTimer(false);
    }
    if (currentTimeAnimator_ != nullptr) {
        if (currentTimeAnimator_->GetState() == Animator::STOP) {
            currentTimeAnimator_->Start();
        }
    }
    if (!startOrPauseImg_->IsTouchable()) {
        LOADIMG::LoadImageViewImg(startOrPauseImg_, TIMER_IMAGE, IMAGE_TIMER_PAUSE);
    }
}

void TimerCountDown::OnPause()
{
    if (TimerModel::GetInstance().GetIsCountDown()) {
        TimerModel::GetInstance().SetExistTimer(true);
    }
    if (currentTimeAnimator_ != nullptr) {
        if (currentTimeAnimator_->GetState() != Animator::STOP) {
            currentTimeAnimator_->Stop();
        }
    }
    SetAnimatorStatus(false);
}

void TimerCountDown::CreateButton()
{
    // 开始/暂停按钮
    startOrPauseImg_ = new UIImageView();
    if (startOrPauseImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "startOrPauseImg_ fail");
        return;
    }
    startOrPauseImg_->SetPosition(TIMING_START_IMAGE_XPOS, TIMING_START_IMAGE_YPOS);
    startOrPauseImg_->Resize(TIMING_START_IMAGE_SIZE, TIMING_START_IMAGE_SIZE);
    startOrPauseImg_->SetViewId(START_OR_PAUSE_BUTTON);
    startOrPauseImg_->SetTouchable(true);
    startOrPauseImg_->SetOnClickListener(this);
    LOADIMG::LoadImageViewImg(startOrPauseImg_, TIMER_IMAGE, IMAGE_TIMER_PLAY);

    fragmentView_.Add(startOrPauseImg_);

    // 复位/退出按钮
    ImageInfo *image2 = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_TIMER_EXIT, TIMER_IMAGE);
    resetOrExitImg_ = new UIImageView();
    if (resetOrExitImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "resetOrExitImg_ fail");
        return;
    }
    resetOrExitImg_->SetPosition(TIMING_RESET_IMAGE_XPOS, TIMING_RESET_IMAGE_YPOS);
    resetOrExitImg_->Resize(TIMING_RESET_IMAGE_SIZE, TIMING_RESET_IMAGE_SIZE);
    resetOrExitImg_->SetViewId(RESET_OR_EXIT_BUTTON);
    resetOrExitImg_->SetTouchable(true);
    resetOrExitImg_->SetOnClickListener(this);
    resetOrExitImg_->SetSrc(image2);
    fragmentView_.Add(resetOrExitImg_);
}

void TimerCountDown::CreateTimerTime()
{
    timerClock_ = new UIDigitalClock();
    if (timerClock_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "timerClock_ fail");
        return;
    }
    timerClock_->SetPosition(TIMING_TIMER_TIME_XPOS, TIMING_TIMER_TIME_YPOS);
    timerClock_->Resize(TIMING_TIMER_TIME_WIDTH, TIMING_TIMER_TIME_HEIGHT);
    timerClock_->SetFont(BOLD_VECTOR_FONT_FILENAME, TIMING_TIMER_TIME_FONT_SIZE);
    timerClock_->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR);
    timerClock_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);

    fragmentView_.Add(timerClock_);
}

void TimerCountDown::RefreshFragment()
{
    uint64_t totalTime = TimerModel::GetInstance().GetTotalTime();
    uint8_t hour = totalTime / HOUR_TIME;
    uint8_t minute = totalTime % HOUR_TIME / MIN_TIME;
    uint8_t second = totalTime % MIN_TIME;
    timerClock_->SetTime24Hour(hour, minute, second);
    TimerModel::GetInstance().SetEclipseTime(true, 0);
    LOADIMG::LoadImageViewImg(resetOrExitImg_, TIMER_IMAGE, IMAGE_TIMER_EXIT);
    circleProgress_->SetValue(TIMING_CIRCLE_RANGE);

}

void TimerCountDown::CircleProgress()
{
    circleProgress_ = new UICircleProgress();
    if (circleProgress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "circleProgress_ fail");
        return;
    }
    circleProgress_->SetPosition(0, 0);
    circleProgress_->Resize(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    circleProgress_->EnableBackground(true);
    circleProgress_->SetForegroundStyle(STYLE_LINE_COLOR, 0XFFFFBF00);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, 0XFF473D20);
    circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, TIMING_CIRCLE_LINE_WIDTH);
    circleProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH, TIMING_CIRCLE_LINE_WIDTH);
    circleProgress_->EnableBackground(true);
    circleProgress_->SetCenterPosition(TIMING_CIRCLE, TIMING_CIRCLE);
    circleProgress_->SetRadius(CIRCLE_RADIUS);
    circleProgress_->SetStartAngle(0);
    circleProgress_->SetEndAngle(TIMING_CIRCLE_END);
    circleProgress_->SetRange(TIMING_CIRCLE_RANGE, 0);
    circleProgress_->SetValue(TIMING_CIRCLE_RANGE);
    fragmentView_.Add(circleProgress_);
}

// 单位为毫秒计算
void TimerCountDown::SetTimerTime(int64_t onTime)
{
    uint64_t time = onTime / MS_ONE_SECONDS;
    uint16_t reminMs = onTime % MS_ONE_SECONDS;
    if (reminMs > REMIND_MIN_MS) {
        time++;
    }
    uint8_t hour = time / HOUR_TIME;
    uint8_t minute = time % HOUR_TIME / MIN_TIME;
    uint8_t second = time % MIN_TIME;
    timerClock_->SetTime24Hour(hour, minute, second);
    uint64_t totalTime = TimerModel::GetInstance().GetTotalTime();
    double percentage = static_cast<double>(onTime) / ((uint64_t)totalTime * MS_ONE_SECONDS);
    double value = round(percentage * TIMING_CIRCLE_RANGE);
    circleProgress_->SetValue(static_cast<int32_t>(value));
    circleProgress_->Invalidate();
    timerClock_->Invalidate();
}

void TimerCountDown::InitAnimator()
{
    countDownTimecallBack_ = new TimerCountDownCallback(this);
    if (countDownTimecallBack_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "countDownTimecallBack_ fail");
        return;
    }
    countDownTimeanimator_ = new Animator(countDownTimecallBack_, nullptr, 0, true);
    if (countDownTimeanimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "countDownTimeanimator_ fail");
        return;
    }
}

void TimerCountDown::TimerCountDownCallback::Callback(UIView *view)
{
    (void)view;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);

    uint64_t totalTime = TimerModel::GetInstance().GetTotalTime();
    uint64_t eclipseTime = TimerModel::GetInstance().GetEclipseTime();
    uint64_t s_time = TimerModel::GetInstance().GetStartTime();
    uint64_t c_time = (uint64_t)current_time.tv_sec * MS_ONE_SECONDS + current_time.tv_usec / MS_ONE_SECONDS;
    int64_t remainTime = (uint64_t)totalTime * MS_ONE_SECONDS  - eclipseTime - (c_time - s_time);
    if (remainTime < 0) {
        // 计时到设定时间，复位图标修改为退出图标，暂停图标修改为播放图标，并设置成不可点击态
        page_->startOrPauseImg_->SetTouchable(false);
        LOADIMG::LoadImageViewImg(page_->startOrPauseImg_, TIMER_IMAGE, IMAGE_TIMER_PAUSE);
        LOADIMG::LoadImageViewImg(page_->resetOrExitImg_, TIMER_IMAGE, IMAGE_TIMER_EXIT);
        TimerModel::GetInstance().SetIsPause(false);
        TimerModel::GetInstance().SetIsCountDown(false);
        page_->startOrPauseImg_->Invalidate();
        page_->resetOrExitImg_->Invalidate();
        page_->countDownTimeanimator_->Stop();
        page_->SetTimerTime(0);
        // 调用pwm接口实现马达震动，客户自实现
    } else {
        page_->SetTimerTime(remainTime);
    }
}

void TimerCountDown::SetAnimatorStatus(bool status)
{
    if (status) {
        LOADIMG::LoadImageViewImg(startOrPauseImg_, TIMER_IMAGE, IMAGE_TIMER_PAUSE_ORANGE);
        LOADIMG::LoadImageViewImg(resetOrExitImg_, TIMER_IMAGE, IMAGE_TIMER_RESTART);
        countDownTimeanimator_->Start();
        TimerModel::GetInstance().InitTimer();
    } else {
        countDownTimeanimator_->Stop();
        LOADIMG::LoadImageViewImg(startOrPauseImg_, TIMER_IMAGE, IMAGE_TIMER_PLAY);
        TimerModel::GetInstance().DisableTimer();
    }
}

bool TimerCountDown::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool TimerCountDown::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        if (TimerModel::GetInstance().GetIsCountDown()) {
            NativeAbility::GetInstance().ChangeSliceToApplist();
        } else {
            fragmentView_.SetVisible(false);
            TimerMainPage::GetInstance()->ShowMainPage();
        }
    }
    return true;
}

bool TimerCountDown::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}

bool TimerCountDown::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), RESET_OR_EXIT_BUTTON) == 0) {
        if (TimerModel::GetInstance().GetIsCountDown()) {
            // 此时计时器处于计时状态，点击按钮，重置timerClock_时间为倒计时初始选择时间，修改复位图标为退出图标，暂停图标变为开始图标，
            SetAnimatorStatus(false);
            TimerModel::GetInstance().SetIsCountDown(false);
            uint64_t totalTime = TimerModel::GetInstance().GetTotalTime();
            uint8_t hour = totalTime / HOUR_TIME;
            uint8_t minute = totalTime % HOUR_TIME / MIN_TIME;
            uint8_t second = totalTime % MIN_TIME;
            timerClock_->SetTime24Hour(hour, minute, second);
            TimerModel::GetInstance().SetEclipseTime(true, 0);
            LOADIMG::LoadImageViewImg(resetOrExitImg_, TIMER_IMAGE, IMAGE_TIMER_EXIT);
            circleProgress_->SetValue(TIMING_CIRCLE_RANGE);
        } else if (!TimerModel::GetInstance().GetIsCountDown() && TimerModel::GetInstance().GetIsPause()) {
            // 此时计时器处于暂停状态，点击复位按钮，重置timerClock_时间为倒计时初始选择时间，将复位图标变为退出图标，
            TimerModel::GetInstance().SetIsCountDown(false);
            uint64_t totalTime = TimerModel::GetInstance().GetTotalTime();
            uint8_t hour = totalTime / HOUR_TIME;
            uint8_t minute = totalTime % HOUR_TIME / MIN_TIME;
            uint8_t second = totalTime % MIN_TIME;
            timerClock_->SetTime24Hour(hour, minute, second);
            TimerModel::GetInstance().SetEclipseTime(true, 0);
            LOADIMG::LoadImageViewImg(resetOrExitImg_, TIMER_IMAGE, IMAGE_TIMER_EXIT);
            circleProgress_->SetValue(TIMING_CIRCLE_RANGE);
            TimerModel::GetInstance().SetIsPause(false);
        } else {
            NativeAbility::GetInstance().SwitchPageInSlice(TIMER_PAGES::TIMER_MAIN_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        }
    } else if (strcmp(view.GetViewId(), START_OR_PAUSE_BUTTON) == 0) {
        if (TimerModel::GetInstance().GetIsCountDown()) {
            // 计时器此时处于运行状态，点击暂停按钮，暂停图标变为开始图标
            SetAnimatorStatus(false);
            TimerModel::GetInstance().SetIsCountDown(false);
            struct timeval current_time;
            gettimeofday(&current_time, nullptr);
            uint64_t time = ((uint64_t)current_time.tv_sec * MS_ONE_SECONDS + current_time.tv_usec / MS_ONE_SECONDS) -
                            TimerModel::GetInstance().GetStartTime();
            TimerModel::GetInstance().SetEclipseTime(false, time);
            TimerModel::GetInstance().SetIsPause(true);
        } else {
            // 计时器此时处于停止/暂停状态，点击开始按钮，开始图标变为暂停图标，并获取点击时的时间，用作开始时间
            SetAnimatorStatus(true);
            struct timeval current_time;
            gettimeofday(&current_time, nullptr);
            uint64_t s_time =
                (uint64_t)current_time.tv_sec * MS_ONE_SECONDS + current_time.tv_usec / MS_ONE_SECONDS;  // 转换成ms
            TimerModel::GetInstance().SetStartTime(s_time);
            TimerModel::GetInstance().SetIsCountDown(true);
            TimerModel::GetInstance().SetIsPause(false);
        }
    }
    return true;
}

void TimerCountDown::TimeAnimatorCallback::Callback(UIView *view)
{
    UILabel *timeLabel = dynamic_cast<UILabel *>(view);
    if (timeLabel == nullptr) {
        return;
    }
    std::string currTime = GetCurrentTimeStr();
    timeLabel->SetText(currTime.c_str());
    timeLabel->Invalidate();
}
}