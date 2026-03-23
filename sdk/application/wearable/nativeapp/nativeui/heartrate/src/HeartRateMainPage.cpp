/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateMainPage
 * Create: 2025-06
 */

#include <string>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "ui_resource_heartRate.h"
#include "heartrate/HeartRatePresenter.h"
#include "heartrate/HeartRateView.h"
#include "heartrate/HeartRateModel.h"
#include "heartrate/HeartRateMainPage.h"

namespace OHOS {
static constexpr uint16_t HEART_BUTTON_POSITION_X = 116;
static constexpr uint16_t HEART_BUTTON_HEIGHT = 76;
static constexpr uint16_t HEART_TITLE_POSITION_X = 195;
static constexpr uint16_t HEART_TITLE_POSITION_Y = 40;
static constexpr uint16_t HEART_TITLE_WIDTH = 260;
static constexpr uint16_t HEART_TITLE_HEIGHT = 53;
static constexpr uint16_t HEART_TITLE_FONT_SIZE = 38;
static constexpr uint16_t HEART_BUTTON_TWO_POSITION_Y = 330;
static constexpr uint16_t HEART_BUTTON_TWO_WIDTH = 232;
static constexpr uint16_t BOTTOM_TEXT_ONE_X = 138;
static constexpr uint16_t BOTTOM_TEXT_ONE_Y = 281;
static constexpr uint16_t BOTTOM_TEXT_WIDTH_ONE = 250;
static constexpr uint16_t BOTTOM_TEXT_HEIGHT_ONE = 36;
static constexpr uint16_t HEART_RATE_LIMIT_FONT_SIZE_TWO = 24;
static constexpr uint16_t VIDEO_WIDTH = 454;
static constexpr uint16_t VIDEO_HEIGHT = 454;
static constexpr uint16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr uint16_t BUFFER_QUEUE_SIZE = 3;
static constexpr uint16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr uint16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr uint16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr uint16_t HEART_BUTTON_FONT_SIZE = 35;
static constexpr uint16_t HEART_DATE_TIME_ONE = 90;
static constexpr uint16_t HEART_DATE_TIME_TWO = 3000;
static constexpr uint16_t HEART_DATE_TIME_THREE = 60;
static constexpr uint16_t HEART_DATE_TIME_FOUR = 3600;
static constexpr uint16_t HEART_DATE_MAX = 454;
static constexpr uint16_t HEART_DATE_WIDTH = 2;
static constexpr char *HEART_START_BUTTON = "startButton";

REGIST_SLICE_PAGE(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_MAIN_PAGE, HeartRateMainPage, true);

HeartRateMainPage::HeartRateMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateMainPage::HeartRateMainPage");
}

HeartRateMainPage::~HeartRateMainPage()
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

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelPreTest_ != nullptr) {
        delete labelPreTest_;
        labelPreTest_ = nullptr;
    }

    if (videoPlay_ != nullptr) {
        videoPlay_->StopVideoPlay();
        delete videoPlay_;
        videoPlay_ = nullptr;
    }

    if (surfaceView_ != nullptr) {
        delete surfaceView_;
        surfaceView_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_RATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateMainPage::~HeartRateMainPage");
}

void HeartRateMainPage::OnStart(void *data)
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

    InitImageAnimator();

    button_ = new UILabelButton();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button_ fail");
        return;
    }
    button_->SetPosition(HEART_BUTTON_POSITION_X, HEART_BUTTON_TWO_POSITION_Y);
    button_->Resize(HEART_BUTTON_TWO_WIDTH, HEART_BUTTON_HEIGHT);
    button_->SetViewId(HEART_START_BUTTON);
    button_->SetOnClickListener(this);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::RELEASED);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::PRESSED);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::INACTIVE);
    button_->SetText("开始测量");
    button_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_BUTTON_FONT_SIZE);
    button_->SetVisible(true);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(HEART_TITLE_POSITION_X, HEART_TITLE_POSITION_Y, HEART_TITLE_WIDTH, HEART_TITLE_HEIGHT);
    labelTitle_->SetText("心率");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, HEART_TITLE_FONT_SIZE);
    SetTestTime();
    group_->Add(button_);
    group_->Add(labelPreTest_);
    group_->Add(labelTitle_);
    AddViewToPageContainer(group_);
}

void HeartRateMainPage::OnResume()
{
    videoPlay_->StartVideoPlay();
}

void HeartRateMainPage::OnPause()
{
    videoPlay_->StopVideoPlay();
}

void HeartRateMainPage::SetTestTime()
{
    labelPreTest_ = new UILabel();
    if (labelPreTest_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelPreTest_ fail");
        return;
    }
    labelPreTest_->SetPosition(BOTTOM_TEXT_ONE_X, BOTTOM_TEXT_ONE_Y, BOTTOM_TEXT_WIDTH_ONE, BOTTOM_TEXT_HEIGHT_ONE);
    labelPreTest_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEART_RATE_LIMIT_FONT_SIZE_TWO);
    labelPreTest_->SetStyle(STYLE_TEXT_COLOR, 0xFFCCCCCC);
    labelPreTest_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelPreTest_->SetVisible(false);
    heartRateMeasurementInfo info = HeartRateModel::GetInstance().GetHeartRate();
    struct timeval current_time;
    if (info.heartRate == 0) {
        info.heartRate = HEART_DATE_TIME_ONE;
        gettimeofday(&current_time, nullptr);
        int64_t now = current_time.tv_sec;
        struct timeval time1;
        time1.tv_sec = now - HEART_DATE_TIME_TWO;
        info.time = time1;
    }
    if (info.heartRate != 0) {
        struct timeval tv = info.time;
        std::time_t time = tv.tv_sec;
        std::tm *tm_info = std::localtime(&time);
        std::stringstream ss;
        if (IsToday(tv)) {
            gettimeofday(&current_time, nullptr);
            int64_t now = current_time.tv_sec;
            int64_t diff_seconds = now - tv.tv_sec;
            if (diff_seconds < 0) {
                ss << info.heartRate << "次/分  ("
                   << "刚刚)";
            } else if (diff_seconds < HEART_DATE_TIME_THREE) {
                ss << info.heartRate << "次/分 ("
                   << "刚刚)";
            } else if (diff_seconds < HEART_DATE_TIME_FOUR) {
                int minutes = diff_seconds / HEART_DATE_TIME_THREE;
                ss << info.heartRate << "次/分  (" << minutes << "分钟前)";
            } else {
                int hours = diff_seconds / HEART_DATE_TIME_FOUR;
                ss << info.heartRate << "次/分  (" << hours << "小时前)";
            }
        } else {
            ss << info.heartRate << "次/分  " << (tm_info->tm_mon + 1) << "/" << tm_info->tm_mday;
        }
        labelPreTest_->SetText(ss.str().c_str());
        labelPreTest_->SetText(ss.str().c_str());
        int textWidth = labelPreTest_->GetTextWidth();
        int posX = (HEART_DATE_MAX - textWidth) / HEART_DATE_WIDTH;
        labelPreTest_->SetX(posX);
        labelPreTest_->SetVisible(true);
    }
}

bool HeartRateMainPage::IsToday(const struct timeval &tv)
{
    time_t now = time(nullptr);
    std::tm *now_tm = localtime(&now);
    std::time_t given_time = tv.tv_sec;
    std::tm *given_tm = std::localtime(&given_time);

    return (now_tm->tm_year == given_tm->tm_year) && (now_tm->tm_mon == given_tm->tm_mon) &&
           (now_tm->tm_mday == given_tm->tm_mday);
}

bool HeartRateMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool HeartRateMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), HEART_START_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(HeartRateAllView::HEARTRATE_READY_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    UNUSED(event);
    return true;
}

void HeartRateMainPage::InitImageAnimator()
{
    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new surfaceView_ fail");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetPreview(
        ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_HEART_RATE_PREVIEW1, HEART_RATE_IMAGE));
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    surfaceView_->SetSurfaceColorkey(colorKey_);
    group_->Add(surfaceView_);

    Surface *surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(VIDEO_WIDTH, VIDEO_HEIGHT);
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    int32_t ySize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) * VIDEO_HEIGHT;
    int32_t uvSize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) *
                     ALIGN_BYTE(VIDEO_HEIGHT, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;
    surface->SetSize(bufferSize);
    std::string uri("/user/res/video/heartBackGround.mp4");
    videoPlay_ = new MediaVideoPlay(surface, nullptr, uri, false);
    if (videoPlay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new videoPlay_ fail");
        return;
    }
}
}
