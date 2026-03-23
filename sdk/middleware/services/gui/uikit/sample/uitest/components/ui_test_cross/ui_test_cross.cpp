/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UITestCross
 * Author:
 * Create: 2023-09
 */

#include "securec.h"
#include "sys/time.h"
#include "graphic_config.h"
#include "ui_cross_resource.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "components/root_view.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "ui_test_cross.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif

namespace OHOS {
static constexpr int16_t VIDEO_WIDTH = 454;
static constexpr int16_t VIDEO_HEIGHT = 454;
static constexpr int16_t CAMERA_DISTANCE = 600;
static constexpr int16_t EASE_RANGE = 100;
static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr int16_t BUFFER_QUEUE_SIZE = 2;
static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t TIME_CONVERT = 60; // sec to min, and min to hor
static constexpr int16_t TEXT_BUFFER_SIEZ = 20;
static constexpr int16_t TEXT_FONT_SIEZ = 40;
constexpr const char* VIDEO_PREVIEW = RES_PATH"demo_pure_video_loop_preview.bin";

#define MEDIA_BYTE_ALIGN(value, align) (((value) + (align) - 1) & (~((align) - 1)))

#ifdef SUPPORT_OHOSFWK
static MediaVideoPlay *g_videoPlay = nullptr;
static void ScreenStatusNotify(ScreenStatus status)
{
    if (g_videoPlay == nullptr) {
        return;
    }

    if (status == SCREEN_STATUS_OFF) {
        g_videoPlay->PauseVideoPlay();
    } else if (status == SCREEN_STATUS_ON) {
        if (g_videoPlay->isPause_) {
            g_videoPlay->ResumeVideoPlay();
        } else if (!g_videoPlay->isPlayed_) {
            g_videoPlay->StartVideoPlay();
        }
    }
}
#endif

static void GetTime(char* buf, int len)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int32_t second = tv.tv_sec % TIME_CONVERT;
    int32_t minute = tv.tv_sec / TIME_CONVERT;
    int32_t hour = (minute / TIME_CONVERT) % 24; // 24: 24 hours per day
    int32_t realMilliSecond = tv.tv_usec / USEC_TO_MILLI;
    minute = minute % TIME_CONVERT;

    sprintf_s(buf, len, "%02d : %02d : %02d : %03d", hour, minute, second, realMilliSecond);
}

void UITestCross::VideoAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetTime(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    RootView::GetInstance()->Invalidate();
}

#ifndef _WIN32
UITestCross::VideoPage::~VideoPage()
{
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = nullptr;
    deregisterNotifyScreenStatus(ScreenStatusNotify);
#endif
    RemoveAll();
    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }

    if (videoPlay_ != nullptr) {
        delete videoPlay_;
        videoPlay_ = nullptr;
    }

    if (surfaceView_ != nullptr) {
        delete surfaceView_;
        surfaceView_ = nullptr;
    }

    if (previewInfo_ != nullptr) {
        previewInfo_ = nullptr;
        ImageCacheManager::GetInstance().UnloadSingleRes(VIDEO_PREVIEW);
    }
}

void UITestCross::VideoPage::PageInit()
{
    label_ = new UILabel();
    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetTime(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    label_->SetPosition(60, 50, 330, 70); // 60, 50, 330, 70: Left, right, width, height.
    label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Silver().full);
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_FONT_SIEZ);
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);

    callBack_ = new VideoAnimatorCallback(label_);
    animator_ = new Animator(callBack_, label_, 0, true);

    colorKey_.full = 0xff212223;
    surfaceView_ = new UILiteSurfaceView();
    surfaceView_->SetViewId("surface view");
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    surfaceView_->SetSurfaceColorkey(colorKey_);

    if (previewInfo_ == nullptr) {
        previewInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(VIDEO_PREVIEW);
    }
    surfaceView_->SetPreview(previewInfo_);

    Add(surfaceView_);
    Add(label_);
    SetCoverable(true);

    Surface *surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(VIDEO_WIDTH, VIDEO_HEIGHT);
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    // jpeg decoder output buffer size calculation formula
    int32_t ySize = MEDIA_BYTE_ALIGN(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) * VIDEO_HEIGHT;
    int32_t uvSize = MEDIA_BYTE_ALIGN(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) *
        MEDIA_BYTE_ALIGN(VIDEO_HEIGHT, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;
    surface->SetSize(bufferSize);

    std::string uri("/user/video/demo_pure_video_loop.mp4");
    videoPlay_ = new MediaVideoPlay(surface, nullptr, uri, true);
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = videoPlay_;
    registerNotifyScreenStatus(ScreenStatusNotify);
#endif
}

void UITestCross::VideoPage::ScrollBegin(bool isActive)
{
    videoPlay_->PauseVideoPlay();
    animator_->Stop();
}

void UITestCross::VideoPage::ScrollEnd(bool isActive)
{
    if (isActive) {
        if (videoPlay_->isPause_) {
            videoPlay_->ResumeVideoPlay();
        } else if (!videoPlay_->isPlayed_) {
            videoPlay_->StartVideoPlay();
        }
        animator_->Start();
    } else {
        if (!videoPlay_->isExited_) {
            videoPlay_->StopVideoPlay();
        }
        animator_->Stop();
    }
}

void UITestCross::VideoPage::CoverBegin(bool isCovered)
{
    videoPlay_->PauseVideoPlay();
    animator_->Stop();
}

void UITestCross::VideoPage::CoverEnd(bool isCovered)
{
    if (!isCovered) {
        if (videoPlay_->isPause_) {
            videoPlay_->ResumeVideoPlay();
        } else if (!videoPlay_->isPlayed_) {
            videoPlay_->StartVideoPlay();
        }
        animator_->Start();
    }
}

void UITestCross::VideoPage::OnCovered()
{
    videoPlay_->PauseVideoPlay();
    animator_->Stop();
}

void UITestCross::VideoPage::OnUncovered()
{
    if (videoPlay_->isPause_) {
        videoPlay_->ResumeVideoPlay();
    } else if (!videoPlay_->isPlayed_) {
        videoPlay_->StartVideoPlay();
    }
    animator_->Start();
}

void UITestCross::VideoPage::StartVideo(void)
{
    if (videoPlay_ != nullptr && animator_ != nullptr) {
        videoPlay_->StartVideoPlay();
        animator_->Start();
    }
}
#endif

void UITestCross::SetUpPage(UICardPage** card, uint32_t resId)
{
    int16_t width = Screen::GetInstance().GetWidth();
    int16_t height = Screen::GetInstance().GetHeight();

    *card = new UICardPage();
    (*card)->SetPosition(0, 0, width, height);
    (*card)->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    UIImageView* img = new UIImageView();
    img->SetPosition(0, 0, width, height);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    img->SetSrc(ImageCacheManager::GetInstance().LoadOneInMultiRes(resId, CROSS_IMG_PATH));
    (*card)->Add(img);
    container_->HorAdd(*card);
}

void UITestCross::SetUp()
{
    container_ = new UICrossView();
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    callback_ = new CardFlipEffect();
    callback_->SetContainer(container_);

#ifndef _WIN32
    video_ = new VideoPage();
    video_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    video_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Silver().full);
    video_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    video_->PageInit();
    video_->StartVideo();
    video_->SetTouchable(true);
    video_->SetOnClickListener(this);
#endif
    SetUpPage(&card1_, CROSS_CAT);
    SetUpPage(&card2_, CROSS_DOVE);
    SetUpPage(&card3_, CROSS_DROP);
    SetUpPage(&card4_, CROSS_ROSE);

    container_->SetAnimatorTime(200); // 200: animator drag time(ms);
    container_->SetLoopState(true);
    container_->EnableScreenCap(true);
    container_->RegisterSwipeCallback(callback_);
#ifdef _WIN32
    container_->SetHorCurrentPage(card1_->GetViewIndex());
    card1_->SetCoverable(true);
#else
    container_->HorAdd(video_);
    container_->SetHorCurrentPage(video_->GetViewIndex());
#endif
    up_ = new PullUpDownView();
    up_->InitView();
    up_->SetViewId("Up View");
    up_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->VerAdd(up_, UICrossView::VPage::UP_PAGE);

    down_ = new UICardPage();
    down_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    down_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    down_->SetViewId("messageView");
    down_->SetTouchable(true);
    down_->SetOnClickListener(this);
    UILabel *msg = new UILabel();
    msg->SetPosition(177, 212, 100, 30); // 177, 212, 100, 30: Left, right, width, height.
    msg->SetText("无消息");
    msg->SetAlign(TEXT_ALIGNMENT_CENTER);
    msg->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: font size
    msg->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    down_->Add(msg);
    container_->VerAdd(down_, UICrossView::VPage::DOWN_PAGE);
}

void UITestCross::TearDown()
{
    if (container_ != nullptr) {
        delete container_;
        container_ = nullptr;
    }
#ifndef _WIN32
    if (video_ != nullptr) {
        delete video_;
        video_ = nullptr;
    }
#endif
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    if (up_ != nullptr) {
        delete up_;
        up_ = nullptr;
    }
    if (down_ != nullptr) {
        delete down_;
        down_ = nullptr;
    }
    if (card1_ != nullptr) {
        DeleteChildren(card1_);
        card1_ = nullptr;
    }
    if (card2_ != nullptr) {
        DeleteChildren(card2_);
        card2_ = nullptr;
    }
    if (card3_ != nullptr) {
        DeleteChildren(card3_);
        card3_ = nullptr;
    }
    if (card4_ != nullptr) {
        DeleteChildren(card4_);
        card4_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(CROSS_IMG_PATH);
}

const UIView* UITestCross::GetTestView()
{
    return container_;
}

bool UITestCross::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == down_) {
        static uint16_t pageIndex = 0;
        container_->SetVerCurrentPage(pageIndex);
        printf("SetVerCurrentPage: %d", pageIndex);
        pageIndex++;
        if (pageIndex > 1) {
            pageIndex = 0;
        }
#ifndef _WIN32
    } else if (&view == video_) {
        static uint16_t pageIndexFromCenter = 0;
        container_->SetVerCurrentPage(pageIndexFromCenter);
        printf("SetVerCurrentPage: %d", pageIndexFromCenter);
        if (pageIndexFromCenter == 0) {
            pageIndexFromCenter = 2; // 2: index
        } else {
            pageIndexFromCenter = 0;
        }
#endif
    }
    return true;
}
}
