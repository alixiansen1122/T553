/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodMainPage
 * Create: 2025-06
 */

#include <string>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include "components/root_view.h"
#include "sys/time.h"
#include "ui_test_group.h"
#include "main/LoadImg.h"
#include "securec.h"
#include "ui_resource_blood.h"
#include "blood/BloodPresenter.h"
#include "blood/BloodMainPage.h"

namespace OHOS {
static constexpr uint16_t BLOOD_BUTTON_POSITION_X = 110;
static constexpr uint16_t BLOOD_BUTTON_HEIGHT = 76;
static constexpr uint16_t BLOOD_TITLE_POSITION_X = 195;
static constexpr uint16_t BLOOD_TITLE_POSITION_Y = 40;
static constexpr uint16_t BLOOD_TITLE_WIDTH = 260;
static constexpr uint16_t BLOOD_TITLE_HEIGHT = 53;
static constexpr uint16_t BLOOD_TITLE_FONT_SIZE = 38;
static constexpr uint16_t BLOOD_BUTTON_TWO_POSITION_Y = 330;
static constexpr uint16_t BLOOD_BUTTON_TWO_WIDTH = 232;
static constexpr uint16_t BLOOD_LABLETEST_POSITION_X = 158;
static constexpr uint16_t BLOOD_LABLETEST_POSITION_Y = 281;
static constexpr uint16_t BLOOD_LABLETEST_WIDTH = 200;
static constexpr uint16_t BLOOD_LABLETEST_HEIGHT = 40;
static constexpr uint16_t BLOOD_LABLETEST_SIZE_FONT = 24;
static constexpr uint16_t VIDEO_WIDTH = 454;
static constexpr uint16_t VIDEO_HEIGHT = 454;
static constexpr uint16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr uint16_t BUFFER_QUEUE_SIZE = 3;
static constexpr uint16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr uint16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr uint16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr uint16_t BLOOD_BUTTON_FONT_SIZE = 35;
static constexpr uint16_t BLOOD_DATE_TIME_ONE = 90;
static constexpr uint16_t BLOOD_DATE_TIME_TWO = 3000;
static constexpr uint16_t BLOOD_DATE_TIME_THREE = 60;
static constexpr uint16_t BLOOD_DATE_TIME_FOUR = 3600;
static constexpr uint16_t BLOOD_OPA = 50;
static constexpr char *BLOOD_START_BUTTON = "startButton";

REGIST_SLICE_PAGE(VIEW_BLOOD, BLOOD_PAGES::BLOOD_MAIN_PAGE, BloodMainPage, true);

BloodMainPage::BloodMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodMainPage::BloodMainPage");
}

BloodMainPage::~BloodMainPage()
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

    ImageCacheManager::GetInstance().UnloadSingleRes(BLOOD_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BloodMainPage::~BloodMainPage");
}

void BloodMainPage::OnStart(void *data)
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

    buttonStart_ = new UILabelButton();
    if (buttonStart_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new buttonStart_ fail");
        return;
    }
    buttonStart_->SetPosition(BLOOD_BUTTON_POSITION_X, BLOOD_BUTTON_TWO_POSITION_Y);
    buttonStart_->Resize(BLOOD_BUTTON_TWO_WIDTH, BLOOD_BUTTON_HEIGHT);
    buttonStart_->SetViewId(BLOOD_START_BUTTON);
    buttonStart_->SetOnClickListener(this);
    buttonStart_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::RELEASED);
    buttonStart_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::PRESSED);
    buttonStart_->SetStyleForState(STYLE_BACKGROUND_COLOR, 0x14FFFFFF, UIButton::INACTIVE);
    buttonStart_->SetText("开始测量");
    buttonStart_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_BUTTON_FONT_SIZE);
    buttonStart_->SetVisible(true);

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelTitle_ fail");
        return;
    }
    labelTitle_->SetPosition(BLOOD_TITLE_POSITION_X, BLOOD_TITLE_POSITION_Y, BLOOD_TITLE_WIDTH, BLOOD_TITLE_HEIGHT);
    labelTitle_->SetText("血氧");
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, BLOOD_TITLE_FONT_SIZE);
    SetTestTime();
    group_->Add(buttonStart_);
    group_->Add(labelPreTest_);
    group_->Add(labelTitle_);

    AddViewToPageContainer(group_);
}

void BloodMainPage::OnResume()
{
    videoPlay_->StartVideoPlay();
}

void BloodMainPage::OnPause()
{
    videoPlay_->StopVideoPlay();
}

void BloodMainPage::SetTestTime()
{
    labelPreTest_ = new UILabel();
    if (labelPreTest_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new labelPreTest_ fail");
        return;
    }
    labelPreTest_->SetPosition(BLOOD_LABLETEST_POSITION_X, BLOOD_LABLETEST_POSITION_Y, BLOOD_LABLETEST_WIDTH,
                               BLOOD_LABLETEST_HEIGHT);
    labelPreTest_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BLOOD_LABLETEST_SIZE_FONT);
    labelPreTest_->SetStyle(STYLE_TEXT_COLOR, 0xFFCCCCCC);
    labelPreTest_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelPreTest_->SetVisible(false);
    bloodMeasurementInfo info = BloodModel::GetInstance().GetBlood();
    struct timeval current_time;
    if (info.blood == 0) {
        info.blood = BLOOD_DATE_TIME_ONE;
        gettimeofday(&current_time, nullptr);
        int64_t now = current_time.tv_sec;
        struct timeval time1;
        time1.tv_sec = now - BLOOD_DATE_TIME_TWO;
        info.time = time1;
    }
    if (info.blood != 0) {
        struct timeval tv = info.time;
        std::time_t time = tv.tv_sec;
        std::tm *tm_info = std::localtime(&time);
        std::stringstream ss;
        if (IsToday(tv)) {
            gettimeofday(&current_time, nullptr);
            int64_t now = current_time.tv_sec;
            int64_t diff_seconds = now - tv.tv_sec;
            if (diff_seconds < 0) {
                ss << info.blood << "%  "
                   << "刚刚";
            } else if (diff_seconds < BLOOD_DATE_TIME_THREE) {
                ss << info.blood << "%  (" << diff_seconds << "秒前)";
            } else if (diff_seconds < BLOOD_DATE_TIME_FOUR) {
                int minutes = diff_seconds / BLOOD_DATE_TIME_THREE;
                ss << info.blood << "%  (" << minutes << "分钟前)";
            } else {
                int hours = diff_seconds / BLOOD_DATE_TIME_FOUR;
                ss << info.blood << "%  (" << hours << "小时前)";
            }
        } else {
            ss << info.blood << "%  " << (tm_info->tm_mon + 1) << "/" << tm_info->tm_mday;
        }
        labelPreTest_->SetText(ss.str().c_str());
        int textWidth = labelPreTest_->GetTextWidth();
        int posX = (HORIZONTAL_RESOLUTION - textWidth) / 2;
        labelPreTest_->SetX(posX);
        labelPreTest_->SetVisible(true);
    }
}

bool BloodMainPage::IsToday(const struct timeval &tv)
{
    time_t now = time(nullptr);
    std::tm *now_tm = localtime(&now);
    std::time_t given_time = tv.tv_sec;
    std::tm *given_tm = std::localtime(&given_time);

    return (now_tm->tm_year == given_tm->tm_year) && (now_tm->tm_mon == given_tm->tm_mon) &&
           (now_tm->tm_mday == given_tm->tm_mday);
}

bool BloodMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool BloodMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), BLOOD_START_BUTTON) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(BLOOD_PAGES::BLOOD_READY_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    UNUSED(event);
    return true;
}

void BloodMainPage::InitImageAnimator()
{
    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitImageAnimator new UILiteSurfaceView fail");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetPreview(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_BLOOD_PREVIEW1, BLOOD_IMAGE));
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
    std::string uri("/user/res/video/bloodBackGround.mp4");
    videoPlay_ = new MediaVideoPlay(surface, nullptr, uri, false);
    if (videoPlay_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new videoPlay_ fail");
        return;
    }
}
}
