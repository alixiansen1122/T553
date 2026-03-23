/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchPage
 * Create: 2025-06-06
 */

#include <string>
#include <sys/time.h>
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "NativeRegisterManager.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_stopwatch.h"
#include "main/LoadImg.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "stopwatch/StopwatchView.h"
#include "stopwatch/StopwatchPresenter.h"
#include "stopwatch/StopwatchModel.h"
#include "stopwatch/StopwatchPage.h"

namespace OHOS {
static const char *STOPWATCH_APP_NAME = "秒表";
static const char *STOPWATCH_START_PAUSE = "stopwatch_start_pause";    // 开始/暂停按钮
static const char *STOPWATCH_RESET_RECORD = "stopwatch_reset_record";  // 重置/分段按钮
static const char *STOPWATCH_RECORD_LIST = "stopwatch_record_list";
static constexpr uint16_t NUM_TEN = 10;
static constexpr uint16_t UNIT_SEC_TO_MSEC = 1000;
static constexpr uint16_t UNIT_SECS_PER_MIN = 60;
static constexpr uint16_t UNIT_SECS_PER_HOUR = 3600;
static constexpr uint16_t MAX_RECORD_SIZE = 20;
static constexpr uint16_t STOPWATCH_HAND_POSITION_XPOS = 224;
static constexpr uint16_t STOPWATCH_HAND_POSITION_YPOS = 4;
static constexpr uint16_t STOPWATCH_HAND_CENTER_XPOS = 3;
static constexpr uint16_t STOPWATCH_HAND_CENTER_YPOS = 223;
static constexpr uint16_t STOPWATCH_HAND_WIDTH = 2;
static constexpr uint16_t STOPWATCH_HAND_HEIGHT = 5;
static constexpr uint16_t STOPWATCH_IMAGE_SCALE_XPOS = 4;
static constexpr uint16_t STOPWATCH_IMAGE_SCALE_YPOS = 4;
static constexpr uint16_t STOPWATCH_LABEL_CURTIME_XPOS = 199;
static constexpr uint16_t STOPWATCH_LABEL_CURTIME_YPOS = 54;
static constexpr uint16_t STOPWATCH_LABEL_CURTIME_WIDTH = 67;
static constexpr uint16_t STOPWATCH_LABEL_CURTIME_HEIGHT = 24;
static constexpr uint16_t STOPWATCH_LABEL_CURTIME_FONT_SIZE = 24;
static constexpr uint16_t STOPWATCH_LABEL_APPNAME_XPOS = 197;
static constexpr uint16_t STOPWATCH_LABEL_APPNAME_YPOS = 82;
static constexpr uint16_t STOPWATCH_LABEL_APPNAME_WIDTH = 64;
static constexpr uint16_t STOPWATCH_LABEL_APPNAME_HEIGHT = 38;
static constexpr uint16_t STOPWATCH_LABEL_APPNAME_FONT_SIZE = 32;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_XPOS = 78;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_YPOS1 = 176;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_YPOS2 = 82;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_WIDTH = 302;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_HEIGHT = 90;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_FONT_SIZE = 72;
static constexpr uint16_t STOPWATCH_LABEL_ELSTIME_FONT_OPA = 76;
static constexpr uint16_t STOPWATCH_BUTTON_SRART_XPOS = 250;
static constexpr uint16_t STOPWATCH_BUTTON_SRART_YPOS = 302;
static constexpr uint16_t STOPWATCH_BUTTON_PAUSE_XPOS = 110;
static constexpr uint16_t STOPWATCH_BUTTON_PAUSE_YPOS = 302;
static constexpr uint16_t STOPWATCH_BUTTON_WIDTH = 96;
static constexpr uint16_t STOPWATCH_BUTTON_HEIGHT = 96;
static constexpr uint16_t STOPWATCH_LIST_RECORD_XPOS = 73;
static constexpr uint16_t STOPWATCH_LIST_RECORD_YPOS = 170;
static constexpr uint16_t STOPWATCH_LIST_RECORD_WIDTH = 312;
static constexpr uint16_t STOPWATCH_LIST_RECORD_HEIGHT = 122;
static constexpr uint16_t STOPWATCH_LIST_REBOUND_SIZE = 60;
static constexpr uint16_t STOPWATCH_LIST_ITEM_WIDTH = 312;
static constexpr uint16_t STOPWATCH_LIST_ITEM_HEIGHT = 60;

REGIST_SLICE_PAGE(VIEW_STOPWATCH, STOPWATCH_PAGES::STOPWATCH_MAIN_PAGE, StopwatchPage, true);

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

static uint64_t GetMilliSeconds(uint64_t uiElapsedTime)
{
    return (uiElapsedTime % (UNIT_SECS_PER_HOUR * UNIT_SEC_TO_MSEC)) % (UNIT_SECS_PER_MIN * UNIT_SEC_TO_MSEC);
}

static std::string FormatElapsedTime(uint64_t uiElapsedTime)
{
    std::string strElapsedTime;
    std::string hours;
    std::string minute;
    std::string seconds;
    std::string centisecs;
    uint64_t hour = uiElapsedTime / (UNIT_SECS_PER_HOUR * UNIT_SEC_TO_MSEC);
    uiElapsedTime %= (UNIT_SECS_PER_HOUR * UNIT_SEC_TO_MSEC);
    uint64_t min = uiElapsedTime / (UNIT_SECS_PER_MIN * UNIT_SEC_TO_MSEC);
    uiElapsedTime %= (UNIT_SECS_PER_MIN * UNIT_SEC_TO_MSEC);
    uint64_t sec = uiElapsedTime / UNIT_SEC_TO_MSEC;
    uiElapsedTime %= UNIT_SEC_TO_MSEC;
    uint64_t csec = uiElapsedTime / NUM_TEN;
    if (hour >= 0 && hour < NUM_TEN) {
        hours = "0" + std::to_string(hour);
    } else {
        hours = std::to_string(hour);
    }
    if (min >= 0 && min < NUM_TEN) {
        minute = "0" + std::to_string(min);
    } else {
        minute = std::to_string(min);
    }
    if (sec >= 0 && sec < NUM_TEN) {
        seconds = "0" + std::to_string(sec);
    } else {
        seconds = std::to_string(sec);
    }
    if (csec >= 0 && csec < NUM_TEN) {
        centisecs = "0" + std::to_string(csec);
    } else {
        centisecs = std::to_string(csec);
    }
    strElapsedTime = minute + ":" + seconds + "." + centisecs;
    return strElapsedTime;
}

static UIView *CreateViewCb(uint8_t type)
{
    StopwatchItemView *item = new StopwatchItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, STOPWATCH_LIST_ITEM_WIDTH, STOPWATCH_LIST_ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    StopwatchItemView *item = dynamic_cast<StopwatchItemView *>(view);
    if (item == nullptr) {
        return;
    }
    StopwatchItem *itemInfo = static_cast<StopwatchItem *>(data);
    item->SetItemInfo(*itemInfo);
}

void StopwatchPage::StopwatchAnimatorCallback::Callback(UIView *view)
{
    UNUSED(view);
    uint64_t uiElapsedTime = StopwatchModel::GetInstance().UpdateElapsedTime();
    uint64_t millisecs = GetMilliSeconds(uiElapsedTime);
    page_->sweepClock_->SetSweepTime24Hour(0, 0, millisecs);
    page_->sweepClock_->Invalidate();
    std::string strElapsedTime = FormatElapsedTime(uiElapsedTime);
    page_->elapsedTime_->SetText(strElapsedTime.c_str());
    if (uiElapsedTime >= UNIT_SECS_PER_HOUR * UNIT_SEC_TO_MSEC) {
        page_->TimeOverRange();
    }
}

void StopwatchPage::TimeAnimatorCallback::Callback(UIView *view)
{
    UILabel *timeLabel = dynamic_cast<UILabel *>(view);
    if (timeLabel == nullptr) {
        return;
    }
    std::string currTime = GetCurrentTimeStr();
    timeLabel->SetText(currTime.c_str());
    timeLabel->Invalidate();
}

StopwatchPage::StopwatchPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchPage::StopwatchPage");
}

StopwatchPage::~StopwatchPage()
{
    bool ret = ImageCacheManager::GetInstance().UnloadAllInMultiRes(STOPWATCH_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage UnloadStopwatchImage fail");
    }
    if (sweepClock_ != nullptr) {
        sweepClock_->RemoveAll();
        delete sweepClock_;
        sweepClock_ = nullptr;
    }
    if (currentTime_ != nullptr) {
        delete currentTime_;
        currentTime_ = nullptr;
    }
    if (appName_ != nullptr) {
        delete appName_;
        appName_ = nullptr;
    }
    if (elapsedTime_ != nullptr) {
        delete elapsedTime_;
        elapsedTime_ = nullptr;
    }
    if (startButton_ != nullptr) {
        delete startButton_;
        startButton_ = nullptr;
    }
    if (resetButton_ != nullptr) {
        delete resetButton_;
        resetButton_ = nullptr;
    }
    if (recordsList_ != nullptr) {
        delete recordsList_;
        recordsList_ = nullptr;
    }
    if (backGround_ != nullptr) {
        delete backGround_;
        backGround_ = nullptr;
    }
    if (imgScale_ != nullptr) {
        delete imgScale_;
        imgScale_ = nullptr;
    }
    if (secondHand_ != nullptr) {
        delete secondHand_;
        secondHand_ = nullptr;
    }
    if (stopwatchAnimator_ != nullptr) {
        delete stopwatchAnimator_;
        stopwatchAnimator_ = nullptr;
    }
    if (stopwatchAnimatorCallback_ != nullptr) {
        delete stopwatchAnimatorCallback_;
        stopwatchAnimatorCallback_ = nullptr;
    }
    if (timeAnimator_ != nullptr) {
        delete timeAnimator_;
        timeAnimator_ = nullptr;
    }
    if (timeCallback_ != nullptr) {
        delete timeCallback_;
        timeCallback_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchPage::~StopwatchPage");
}

void StopwatchPage::OnStart(void *data)
{
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(STOPWATCH_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage LoadStopwatchImage fail");
        return;
    }

    sweepClock_ = new UISweepClock();
    if (sweepClock_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage sweepClock_ new fail");
        return;
    }
    sweepClock_->SetPosition(0, 0);
    sweepClock_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    sweepClock_->SetDraggable(true);
    sweepClock_->SetTouchable(true);
    sweepClock_->SetOnDragListener(this);

    secondHand_ = new Image();
    if (secondHand_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage secondHand_ new fail");
        return;
    }
    LOADIMG::LoadImageImg(secondHand_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_HAND);
    sweepClock_->SetHandImage(UISweepClock::HandType::SECOND_HAND, *secondHand_,
                              {STOPWATCH_HAND_POSITION_XPOS, STOPWATCH_HAND_POSITION_YPOS},
                              {STOPWATCH_HAND_CENTER_XPOS, STOPWATCH_HAND_CENTER_YPOS});
    sweepClock_->SetHandLine(UISweepClock::HandType::HOUR_HAND,
                             {STOPWATCH_HAND_POSITION_XPOS, STOPWATCH_HAND_POSITION_YPOS},
                             {STOPWATCH_HAND_CENTER_XPOS, STOPWATCH_HAND_CENTER_YPOS}, Color::White(),
                             STOPWATCH_HAND_WIDTH, STOPWATCH_HAND_HEIGHT, OPA_TRANSPARENT);
    sweepClock_->SetHandLine(UISweepClock::HandType::MINUTE_HAND,
                             {STOPWATCH_HAND_POSITION_XPOS, STOPWATCH_HAND_POSITION_YPOS},
                             {STOPWATCH_HAND_CENTER_XPOS, STOPWATCH_HAND_CENTER_YPOS}, Color::White(),
                             STOPWATCH_HAND_WIDTH, STOPWATCH_HAND_HEIGHT, OPA_TRANSPARENT);
    sweepClock_->SetWorkMode(UISweepClock::WorkMode::NORMAL);
    uint64_t uiElapsedTime = StopwatchModel::GetInstance().GetElapsedTime();
    uint64_t millisecs = GetMilliSeconds(uiElapsedTime);
    sweepClock_->SetSweepTime24Hour(0, 0, millisecs);

    backGround_ = new UIImageView();
    if (backGround_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage backGround_ new fail");
        return;
    }
    backGround_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(backGround_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_BG);
    sweepClock_->Add(backGround_);

    imgScale_ = new UIImageView();
    if (imgScale_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage imgScale_ new fail");
        return;
    }
    imgScale_->SetPosition(STOPWATCH_IMAGE_SCALE_XPOS, STOPWATCH_IMAGE_SCALE_YPOS);
    LOADIMG::LoadImageViewImg(imgScale_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_SCALE);
    sweepClock_->Add(imgScale_);

    currentTime_ = new UILabel();
    if (currentTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage currentTime_ new fail");
        return;
    }
    currentTime_->SetPosition(STOPWATCH_LABEL_CURTIME_XPOS, STOPWATCH_LABEL_CURTIME_YPOS);
    currentTime_->Resize(STOPWATCH_LABEL_CURTIME_WIDTH, STOPWATCH_LABEL_CURTIME_HEIGHT);
    currentTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, STOPWATCH_LABEL_CURTIME_FONT_SIZE);
    currentTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    currentTime_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    sweepClock_->Add(currentTime_);

    appName_ = new UILabel();
    if (appName_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage appName_ new fail");
        return;
    }
    appName_->SetPosition(STOPWATCH_LABEL_APPNAME_XPOS, STOPWATCH_LABEL_APPNAME_YPOS);
    appName_->Resize(STOPWATCH_LABEL_APPNAME_WIDTH, STOPWATCH_LABEL_APPNAME_HEIGHT);
    appName_->SetText(STOPWATCH_APP_NAME);
    appName_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    appName_->SetTextColor(Color::White());
    appName_->SetFont(BOLD_VECTOR_FONT_FILENAME, STOPWATCH_LABEL_APPNAME_FONT_SIZE);
    appName_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    appName_->SetVisible(true);
    sweepClock_->Add(appName_);

    elapsedTime_ = new UILabel();
    if (elapsedTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage elapsedTime_ new fail");
        return;
    }
    elapsedTime_->SetPosition(STOPWATCH_LABEL_ELSTIME_XPOS, STOPWATCH_LABEL_ELSTIME_YPOS1);
    elapsedTime_->Resize(STOPWATCH_LABEL_ELSTIME_WIDTH, STOPWATCH_LABEL_ELSTIME_HEIGHT);
    elapsedTime_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    elapsedTime_->SetFont(BOLD_VECTOR_FONT_FILENAME, STOPWATCH_LABEL_ELSTIME_FONT_SIZE);
    elapsedTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    std::string strElapsedTime = FormatElapsedTime(uiElapsedTime);
    elapsedTime_->SetText(strElapsedTime.c_str());
    sweepClock_->Add(elapsedTime_);

    startButton_ = new UILabelButton();
    if (startButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage startButton_ new fail");
        return;
    }
    startButton_->SetPosition(STOPWATCH_BUTTON_SRART_XPOS, STOPWATCH_BUTTON_SRART_YPOS);
    startButton_->Resize(STOPWATCH_BUTTON_WIDTH, STOPWATCH_BUTTON_HEIGHT);
    startButton_->SetViewId(STOPWATCH_START_PAUSE);
    startButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    startButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    startButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    startButton_->SetOnClickListener(this);
    sweepClock_->Add(startButton_);

    resetButton_ = new UILabelButton();
    if (resetButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage resetButton_ new fail");
        return;
    }
    resetButton_->SetPosition(STOPWATCH_BUTTON_PAUSE_XPOS, STOPWATCH_BUTTON_PAUSE_YPOS);
    resetButton_->Resize(STOPWATCH_BUTTON_WIDTH, STOPWATCH_BUTTON_HEIGHT);
    resetButton_->SetViewId(STOPWATCH_RESET_RECORD);
    resetButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    resetButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    resetButton_->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    resetButton_->SetOnClickListener(this);
    sweepClock_->Add(resetButton_);

    recordsList_ = new UISimpleList();
    if (recordsList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage recordsList_ new fail");
        return;
    }
    recordsList_->SetPosition(STOPWATCH_LIST_RECORD_XPOS, STOPWATCH_LIST_RECORD_YPOS);
    recordsList_->Resize(STOPWATCH_LIST_RECORD_WIDTH, STOPWATCH_LIST_RECORD_HEIGHT);
    recordsList_->SetViewId(STOPWATCH_RECORD_LIST);
    recordsList_->SetThrowDrag(true);
    recordsList_->SetDraggable(true);
    recordsList_->SetReboundSize(STOPWATCH_LIST_REBOUND_SIZE);
    recordsList_->EnableAutoAlign(true);
    recordsList_->SetElastic(true);
    recordsList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    recordsList_->SetOnClickListener(this);
    recordsList_->SetVisible(false);
    sweepClock_->Add(recordsList_);

    AddViewToPageContainer(sweepClock_);

    stopwatchAnimatorCallback_ = new StopwatchAnimatorCallback(this);
    if (stopwatchAnimatorCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage stopwatchAnimatorCallback_ new fail");
        return;
    }

    stopwatchAnimator_ = new Animator(stopwatchAnimatorCallback_, nullptr, 0, true);
    if (stopwatchAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage stopwatchAnimator_ new fail");
        return;
    }

    timeCallback_ = new TimeAnimatorCallback();
    if (timeCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage timeCallback_ new fail");
        return;
    }

    timeAnimator_ = new Animator(timeCallback_, currentTime_, 0, true);
    if (timeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchPage timeAnimator_ new fail");
        return;
    }

    bool isRunning = StopwatchModel::GetInstance().GetRunningState();
    DrawButtonStatusView(isRunning);
    bool isVisible = StopwatchModel::GetInstance().GetRecordListVisible();
    if (StopwatchModel::GetInstance().GetTimeOverRangeState()) {
        isVisible = false;
    }
    DrawRecordListVisibleView(isVisible);
}

void StopwatchPage::OnStop() {}

void StopwatchPage::OnPause()
{
    if (stopwatchAnimator_ != nullptr) {
        if (stopwatchAnimator_->GetState() != Animator::STOP) {
            stopwatchAnimator_->Stop();
        }
    }
    if (timeAnimator_ != nullptr) {
        if (timeAnimator_->GetState() != Animator::STOP) {
            timeAnimator_->Stop();
        }
    }
}

void StopwatchPage::OnResume()
{
    if (stopwatchAnimator_ != nullptr) {
        bool isRunning = StopwatchModel::GetInstance().GetRunningState();
        if (isRunning && stopwatchAnimator_->GetState() == Animator::STOP) {
            stopwatchAnimator_->Start();
        }
    }
    if (timeAnimator_ != nullptr) {
        if (timeAnimator_->GetState() == Animator::STOP) {
            timeAnimator_->Start();
        }
    }
}

void StopwatchPage::DrawButtonStatusView(bool isRunning)
{
    elapsedTime_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    if (isRunning) {
        LOADIMG::LoadBtnImage(startButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_PAUSE, IMAGE_STOPWATCH_PAUSE);
        startButton_->Invalidate();

        resetButton_->Enable();
        LOADIMG::LoadBtnImage(resetButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_RECORD, IMAGE_STOPWATCH_RECORD);
        auto &recordList = StopwatchModel::GetInstance().GetRecordsList();
        if (recordList.Size() >= MAX_RECORD_SIZE) {
            resetButton_->Disable();
            LOADIMG::LoadBtnImage(resetButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_RECORD_DISABLE,
                                  IMAGE_STOPWATCH_RECORD_DISABLE);
        }
        resetButton_->Invalidate();
    } else {
        LOADIMG::LoadBtnImage(startButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_START, IMAGE_STOPWATCH_START);
        startButton_->Invalidate();

        resetButton_->Enable();
        LOADIMG::LoadBtnImage(resetButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_RESET, IMAGE_STOPWATCH_RESET);
        uint64_t elapsedTime = StopwatchModel::GetInstance().GetElapsedTime();
        if (elapsedTime == 0) {
            resetButton_->Disable();
            LOADIMG::LoadBtnImage(resetButton_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_RESET_DISABLE,
                                  IMAGE_STOPWATCH_RESET_DISABLE);
            elapsedTime_->SetStyle(STYLE_TEXT_OPA, STOPWATCH_LABEL_ELSTIME_FONT_OPA);
        }
        resetButton_->Invalidate();
    }
}

void StopwatchPage::DrawRecordListVisibleView(bool isRecordListVisible)
{
    if (isRecordListVisible) {
        appName_->SetVisible(false);
        recordsList_->SetVisible(true);
        elapsedTime_->SetPosition(STOPWATCH_LABEL_ELSTIME_XPOS, STOPWATCH_LABEL_ELSTIME_YPOS2);
    } else {
        appName_->SetVisible(true);
        recordsList_->SetVisible(false);
        elapsedTime_->SetPosition(STOPWATCH_LABEL_ELSTIME_XPOS, STOPWATCH_LABEL_ELSTIME_YPOS1);
    }
    RefreshRecordsList();
}

void StopwatchPage::RefreshRecordsList()
{
    auto &records = StopwatchModel::GetInstance().GetRecordsList();
    recordsList_->ClearAll();
    ListNode<StopwatchItem> *node = records.Head();
    while (node != records.End()) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        StopwatchItem &itemData = node->data_;
        content.data = reinterpret_cast<void *>(&itemData);
        recordsList_->AddContent(content);
        node = node->next_;
    }
    recordsList_->RefreshList();
}

bool StopwatchPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    bool isRunning = StopwatchModel::GetInstance().GetRunningState();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchPage::OnClick RunningState = %d", isRunning);
    if (strcmp(view.GetViewId(), STOPWATCH_START_PAUSE) == 0) {
        if (isRunning) {
            StopwatchModel::GetInstance().SetRunningState(false);
            DrawButtonStatusView(false);
            stopwatchAnimator_->Stop();
            StopwatchModel::GetInstance().SaveElapsedTime();
        } else {
            StopwatchModel::GetInstance().SetRunningState(true);
            if (StopwatchModel::GetInstance().GetTimeOverRangeState()) {
                DrawRecordListVisibleView(false);
                StopwatchModel::GetInstance().SetTimeOverRangeState(false);
            }
            DrawButtonStatusView(true);
            StopwatchModel::GetInstance().SetStartTime();
            stopwatchAnimator_->Start();
        }
    } else if (strcmp(view.GetViewId(), STOPWATCH_RESET_RECORD) == 0) {
        if (isRunning) {
            StopwatchModel::GetInstance().SetRecordListVisible(true);
            uint64_t uiElapsedTime = StopwatchModel::GetInstance().GetElapsedTime();
            std::string strElapsedTime = FormatElapsedTime(uiElapsedTime);
            auto &recordList = StopwatchModel::GetInstance().GetRecordsList();
            StopwatchItem item = {recordList.Size() + 1, strElapsedTime, true};
            StopwatchModel::GetInstance().AddRecordToList(item);
            DrawRecordListVisibleView(true);
            DrawButtonStatusView(true);
        } else {
            StopwatchModel::GetInstance().SetRecordListVisible(false);
            StopwatchModel::GetInstance().ResetCounter();
            elapsedTime_->SetText("00:00.00");
            sweepClock_->SetInitTime24Hour(0, 0, 0);
            sweepClock_->Invalidate();
            DrawRecordListVisibleView(false);
            DrawButtonStatusView(false);
        }
    } else if (strcmp(view.GetViewId(), STOPWATCH_RECORD_LIST) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(STOPWATCH_PAGES::STOPWATCH_RECORD_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool StopwatchPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

void StopwatchPage::TimeOverRange()
{
    stopwatchAnimator_->Stop();
    elapsedTime_->SetText("59:59.99");
    sweepClock_->SetSweepTime24Hour(0, 0, 0);
    sweepClock_->Invalidate();
    StopwatchModel::GetInstance().SetRunningState(false);
    DrawButtonStatusView(false);
    StopwatchModel::GetInstance().ResetCounter();
    StopwatchModel::GetInstance().SetTimeOverRangeState(true);
}
}
