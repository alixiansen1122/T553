/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodMeasurementPage
 * Create: 2025-06
 */

#include <string>
#include <random>
#include <chrono>
#include "ui_test_group.h"
#include "gfx_utils/graphic_math.h"
#include "main/LoadImg.h"
#include "surface.h"
#include "settings/model/SettingDisplayModel.h"
#include "ui_resource_blood.h"
#include "blood/BloodPresenter.h"
#include "blood/BloodMeasurementPage.h"

namespace OHOS {
static constexpr uint16_t BLOOD_BUTTON_TWO_POSITION_X = 152;
static constexpr uint16_t BLOOD_BUTTON_TWO_POSITION_Y = 330;
static constexpr uint16_t BLOOD_LABEL_FRONT = 72;
static constexpr uint16_t BLOOD_LABEL_FRONT_BOTTOM = 36;
static constexpr uint16_t BLOOD_LABEL2_POSITION_X = 183;
static constexpr uint16_t BLOOD_LABEL2_POSITION_Y = 162;
static constexpr uint16_t BLOOD_LABEL2_WIDTH = 180;
static constexpr uint16_t BLOOD_LABEL2_HEIGHT = 42;
static constexpr uint16_t VIDEO_WIDTH = 454;
static constexpr uint16_t VIDEO_HEIGHT = 454;
static constexpr uint16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr uint16_t BUFFER_QUEUE_SIZE = 3;
static constexpr uint16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr uint16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr uint16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr uint16_t BLOOD_LABLETITLE_POSITION_X = 195;
static constexpr uint16_t BLOOD_LABLETITLE_POSITION_Y = 40;
static constexpr uint16_t BLOOD_LABLETITLE_WIDTH = 260;
static constexpr uint16_t BLOOD_LABLETITLE_HEIGHT = 53;
static constexpr uint16_t BLOOD_LABLETITLE_FONT_SIZE = 38;
static constexpr uint16_t BLOOD_TIME = 15;
static constexpr uint16_t BLOOD_NUM = 60;
static constexpr uint16_t BLOOD_NUM_ONE = 40;

REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_MEASUREMENT_PAGE, BloodMeasurementPage, false);

BloodMeasurementPage::BloodMeasurementPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodMeasurementPage::BloodMeasurementPage");
}

BloodMeasurementPage::~BloodMeasurementPage()
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

    if (labelTime_ != nullptr) {
        delete labelTime_;
        labelTime_ = nullptr;
    }

    if (labelTips_ != nullptr) {
        delete labelTips_;
        labelTips_ = nullptr;
    }

    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
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

    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodMeasurementPage::~BloodMeasurementPage");
}

void BloodMeasurementPage::OnStart(void *data)
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

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(BLOOD_LABLETITLE_POSITION_X, BLOOD_LABLETITLE_POSITION_Y, BLOOD_LABLETITLE_WIDTH,
                             BLOOD_LABLETITLE_HEIGHT);
    labelTitle_->SetText("血氧");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_LABLETITLE_FONT_SIZE);
    labelTitle_->SetVisible(false);

    InitAnimator();
    gettimeofday(&startTime_, nullptr);
    StartAnimator();
    group_->Add(labelTitle_);
    AddViewToPageContainer(group_);
}

void BloodMeasurementPage::OnResume()
{
    videoPlay_->StartVideoPlay();
    SettingDisplayModel::GetInstance().EnableSteadyOn();
}

void BloodMeasurementPage::OnPause()
{
    videoPlay_->StopVideoPlay();
    SettingDisplayModel::GetInstance().DisableSteadyOn();
}

void BloodMeasurementPage::InitAnimator()
{
    labelTime_ = new UILabel();
    if (labelTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTime_ fail");
        return;
    }
    labelTime_->SetPosition(BLOOD_LABEL2_POSITION_X, BLOOD_LABEL2_POSITION_Y);
    labelTime_->Resize(BLOOD_LABEL2_WIDTH, BLOOD_LABEL2_HEIGHT);
    labelTime_->SetStyle(STYLE_TEXT_FONT, BLOOD_LABEL_FRONT);

    labelTime_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTime_->SetText("15s");
    labelTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTime_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_LABEL_FRONT);
    labelTime_->SetVisible(false);

    labelTips_ = new UILabel();
    if (labelTips_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTips_ fail");
        return;
    }
    labelTips_->SetPosition(BLOOD_BUTTON_TWO_POSITION_X, BLOOD_BUTTON_TWO_POSITION_Y);
    labelTips_->Resize(BLOOD_LABEL2_WIDTH, BLOOD_LABEL2_HEIGHT);
    labelTips_->SetStyle(STYLE_TEXT_FONT, BLOOD_LABEL_FRONT_BOTTOM);
    labelTips_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    labelTips_->SetText("正在测量");
    labelTips_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTips_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABEL_FRONT_BOTTOM);
    labelTips_->SetVisible(false);
    group_->Add(labelTime_);
    group_->Add(labelTips_);

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

void BloodMeasurementPage::BloodAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    static int count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    int time = current_time.tv_sec - page_->startTime_.tv_sec;
    std::string strTime1 = std::to_string(BLOOD_TIME - time) + "s";
    page_->SetLabelTime(strTime1);
    page_->ShowOnMeasurement();
    if (time >= BLOOD_TIME) {
        page_->StopAnimator();
        int random_number = BLOOD_NUM + rand() % BLOOD_NUM_ONE;
        struct timeval testTime;
        gettimeofday(&testTime, nullptr);
        bloodMeasurementInfo info;
        info.blood = random_number;
        info.time = testTime;
        BloodModel::GetInstance().SetBlood(info);
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_RESULT_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
}

void BloodMeasurementPage::StopAnimator()
{
    animator_->Stop();
}
void BloodMeasurementPage::StartAnimator()
{
    animator_->Start();
}

void BloodMeasurementPage::SetLabelTime(std::string str)
{
    labelTime_->SetText(str.c_str());
}

void BloodMeasurementPage::ShowOnMeasurement()
{
    labelTips_->SetVisible(true);
    labelTitle_->SetVisible(true);
    labelTime_->SetVisible(true);
}

bool BloodMeasurementPage::OnDrag(UIView &view, const DragEvent &event)
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

void BloodMeasurementPage::InitImageAnimator()
{
    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new surfaceView_ fail");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetPreview(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_BLOOD_PREVIEW2, BLOOD_IMAGE));
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
    std::string uri("/user/res/video/blood.mp4");
    videoPlay_ = new MediaVideoPlay(surface, nullptr, uri, false);
    if (videoPlay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new videoPlay_ fail");
        return;
    }
}
}
