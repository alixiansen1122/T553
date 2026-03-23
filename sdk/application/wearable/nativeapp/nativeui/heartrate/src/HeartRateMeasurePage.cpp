/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateMeasurePage
 * Create: 2025-06
 */

#include <string>
#include <random>
#include <chrono>
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "settings/model/SettingDisplayModel.h"
#include "ui_resource_heartRate.h"
#include "heartrate/HeartRatePresenter.h"
#include "heartrate/HeartRateModel.h"
#include "heartrate/HeartRateMeasurePage.h"

namespace OHOS {
static constexpr uint16_t HEARTRATE_BUTTON_TWO_POSITION_X = 142;
static constexpr uint16_t HEARTRATE_BUTTON_TWO_POSITION_Y = 370;
static constexpr uint16_t HEARTRATE_LABEL_FRONT = 40;
static constexpr uint16_t HEARTRATE_LABEL_FRONT_BOTTOM = 36;
static constexpr uint16_t HEARTRATE_LABEL2_POSITION_X = 133;
static constexpr uint16_t HEARTRATE_LABEL2_POSITION_Y = 200;
static constexpr uint16_t HEARTRATE_LABEL2_WIDTH = 180;
static constexpr uint16_t HEARTRATE_LABEL2_HEIGHT = 62;
static constexpr uint16_t HEARTRATE_LABLE_TITLE_POSITION_X = 195;
static constexpr uint16_t HEARTRATE_LABLE_TITLE_POSITION_Y = 40;
static constexpr uint16_t HEARTRATE_LABLE_TITLE_WIDTH = 260;
static constexpr uint16_t HEARTRATE_LABLE_TITLE_HEIGHT = 53;
static constexpr uint16_t HEARTRATE_LABLE_TITLE_FONT_SIZE = 38;
static constexpr uint16_t HEARTRATE_LABLEBACK_FONT_SIZE = 30;
static constexpr uint16_t VIDEO_WIDTH = 454;
static constexpr uint16_t VIDEO_HEIGHT = 454;
static constexpr uint16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr uint16_t BUFFER_QUEUE_SIZE = 3;
static constexpr uint16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr uint16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr uint16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr uint16_t HEART_TIME_NUM = 15;

REGIST_SLICE_PAGE(VIEW_HEARTRATE, HeartRateAllView::HEARTRATE_MEASUREMENT_PAGE, HeartRateMeasurePage, false);

HeartRateMeasurePage::HeartRateMeasurePage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateMeasurePage::HeartRateMeasurePage");
}

HeartRateMeasurePage::~HeartRateMeasurePage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelTimeOrResult_ != nullptr) {
        delete labelTimeOrResult_;
        labelTimeOrResult_ = nullptr;
    }

    if (labelTips_ != nullptr) {
        delete labelTips_;
        labelTips_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
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

    if (chartLineResultGroup_ != nullptr) {
        delete chartLineResultGroup_;
        chartLineResultGroup_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(HEART_RATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeartRateMeasurePage::~HeartRateMeasurePage");
}

void HeartRateMeasurePage::OnStart(void *data)
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

    InitVideoView();

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(HEARTRATE_LABLE_TITLE_POSITION_X, HEARTRATE_LABLE_TITLE_POSITION_Y,
                             HEARTRATE_LABLE_TITLE_WIDTH, HEARTRATE_LABLE_TITLE_HEIGHT);
    labelTitle_->SetText("心率");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, HEARTRATE_LABLE_TITLE_FONT_SIZE);

    labelTimeOrResult_ = new UILabel();
    if (labelTimeOrResult_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTimeOrResult_ fail");
        return;
    }
    labelTimeOrResult_->SetPosition(HEARTRATE_LABEL2_POSITION_X,
                                    HEARTRATE_LABEL2_POSITION_Y);
    labelTimeOrResult_->Resize(HEARTRATE_LABEL2_WIDTH, HEARTRATE_LABEL2_HEIGHT);
    labelTimeOrResult_->SetStyle(STYLE_TEXT_FONT, HEARTRATE_LABEL_FRONT);
    labelTimeOrResult_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelTimeOrResult_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTimeOrResult_->SetFont(BOLD_VECTOR_FONT_FILENAME, HEARTRATE_LABEL_FRONT);

    labelTips_ = new UILabel();
    if (labelTips_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTips_ fail");
        return;
    }
    labelTips_->SetPosition(HEARTRATE_BUTTON_TWO_POSITION_X, HEARTRATE_BUTTON_TWO_POSITION_Y);
    labelTips_->Resize(HEARTRATE_LABEL2_WIDTH, HEARTRATE_LABEL2_HEIGHT);
    labelTips_->SetStyle(STYLE_TEXT_FONT, HEARTRATE_LABEL_FRONT_BOTTOM);
    labelTips_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelTips_->SetText("正在测量");
    labelTips_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTips_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, HEARTRATE_LABEL_FRONT_BOTTOM);
    group_->Add(labelTimeOrResult_);
    group_->Add(labelTips_);

    InitAnimator();
    gettimeofday(&startTime_, nullptr);
    group_->Add(labelTitle_);

    AddViewToPageContainer(group_);
    animator_->Start();

    SetTestStatus(false);
}

void HeartRateMeasurePage::OnResume()
{
    videoPlay_->StartVideoPlay();
    SettingDisplayModel::GetInstance().EnableSteadyOn();
}

void HeartRateMeasurePage::OnPause()
{
    videoPlay_->StopVideoPlay();
    SettingDisplayModel::GetInstance().DisableSteadyOn();
}

void HeartRateMeasurePage::InitAnimator()
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

void HeartRateMeasurePage::SetTimeAndResultText(std::string str)
{
    labelTimeOrResult_->SetText(str.c_str());
}

void HeartRateMeasurePage::SetTestStatus(bool value)
{
    measureStatus_ = value;
}

void HeartRateMeasurePage::HeartAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    if (page_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "page_ is nullptr");
        return;
    }
    static int count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    int time = current_time.tv_sec - page_->startTime_.tv_sec;
    std::string strTime;
    // 测量结束，修改文本，显示当前心率
    if (time >= HEART_TIME_NUM) {
        page_->StopAnimator();
        heartRateMeasurementInfo info;
        info.heartRate = 80; // 80：心率打桩数据
        info.time = current_time;
        strTime = std::to_string(info.heartRate) + "次/分";
        HeartRateModel::GetInstance().SetHeartRate(info);
        page_->SetTestStatus(true);
        page_->labelTips_->SetText("当前心率");
        std::string strHeartRate = std::to_string(info.heartRate);
        if (page_->chartLineResultGroup_ == nullptr) {
            page_->chartLineResultGroup_ = new HeartRateResultGroup();
            if (page_->chartLineResultGroup_ == nullptr) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "chartLineResultGroup_ fail");
                return;
            }
        }
        page_->chartLineResultGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
        page_->chartLineResultGroup_->SetVisible(false);
        page_->group_->Add(page_->chartLineResultGroup_);
        page_->chartLineResultGroup_->labelHeartRateValue_->SetText(strHeartRate.c_str());
        page_->chartLineResultGroup_->SetVisible(true);
    } else {
        // 测量中，文本仅更新倒计时
        strTime = std::to_string(HEART_TIME_NUM - time) + "s";
    }
    page_->SetTimeAndResultText(strTime);
}

void HeartRateMeasurePage::StopAnimator()
{
    animator_->Stop();
}

bool HeartRateMeasurePage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    // 在测量完成后 才能拉起心率线状图
    if (measureStatus_) {
        if (event.GetDragDirection() == DragEvent::DIRECTION_TOP_TO_BOTTOM) {
            chartLineResultGroup_->SetVisible(false);
        } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
            chartLineResultGroup_->SetVisible(true);
        }
    }
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        StopAnimator();
        NativeAbility::GetInstance().SwitchPageInSlice(HeartRateAllView::HEARTRATE_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void HeartRateMeasurePage::InitVideoView()
{
    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new surfaceView_ fail");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetPreview(
        ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_HEART_RATE_PREVIEW2, HEART_RATE_IMAGE));
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
    std::string uri("/user/res/video/heart.mp4");
    videoPlay_ = new MediaVideoPlay(surface, nullptr, uri, false);
    if (videoPlay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new videoPlay_ fail");
        return;
    }
}
}
