/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "securec.h"
#include "sys/time.h"
#include "ui_test_video_play.h"
#include "components/root_view.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif

namespace OHOS {
static constexpr int16_t VIDEO_WIDTH = 454;
static constexpr int16_t VIDEO_HEIGHT = 454;
static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr int16_t BUFFER_QUEUE_SIZE = 3;
static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t TIME_CONVERT = 60; // sec to min, and min to hor
static constexpr int16_t TEXT_BUFFER_SIEZ = 20;
static constexpr int16_t TEXT_FONT_SIEZ = 40;

#ifdef SUPPORT_OHOSFWK
static MediaVideoPlay *g_videoPlay = nullptr;
static bool g_lastPlayed = false;
static void ScreenStatusNotify(ScreenStatus status)
{
    if (g_videoPlay == nullptr) {
        return;
    }

    bool temp = g_videoPlay->isPlayed_;
    if (status == SCREEN_STATUS_OFF && g_videoPlay->isPlayed_) {
        g_videoPlay->PauseVideoPlay();
    } else if (status == SCREEN_STATUS_ON) {
        if (g_lastPlayed && g_videoPlay->isPause_) {
            g_videoPlay->ResumeVideoPlay();
        }
    }
    g_lastPlayed = temp;
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

void VideoAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetTime(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    RootView::GetInstance()->Invalidate();
}

void UITestVideoPlay::SetUp()
{
    container_ = new UIScrollView();
    container_->SetThrowDrag(true);
    container_->SetHorizontalScrollState(false);
    container_->Resize(VIDEO_WIDTH, VIDEO_HEIGHT);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    label_ = new UILabel();
    char filePath[PATH_MAX];
    if (realpath("/user/video/panda.mp4", filePath) == nullptr) {
        label_->SetPosition(60, 227, 330, 70); // 60, 227, 330, 70: Left, right, width, height.
        label_->SetText("please upload video resource.");
        label_->SetStyle(STYLE_BACKGROUND_COLOR, 0xff505050);
        label_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        label_->SetStyle(STYLE_BORDER_RADIUS, 20); // 20: Radius for label border.
        label_->SetStyle(STYLE_BACKGROUND_OPA, 150); // 150: Opacity for background.
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 25); // 25: Font size.
        label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        container_->Add(label_);
        GRAPHIC_LOGE("no video resource\n");
        return;
    }

    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetTime(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    label_->SetPosition(60, 50, 330, 70); // 60, 50, 330, 70: Left, right, width, height.
    label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Silver().full);
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_FONT_SIEZ); // 25: Font size.
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);

    callBack_ = new VideoAnimatorCallback(label_);
    animator_ = new Animator(callBack_, label_, 0, true);
    animator_->Start();

    button1_ = new UILabelButton();
    button1_->SetPosition(130, 360, 80, 40); // 130: x, 360: y, 80: width, 40: height
    button1_->SetText("Play");
    button1_->SetTextColor(Color::Silver());
    button1_->SetOnClickListener(this);

    button2_ = new UILabelButton();
    button2_->SetPosition(240, 360, 80, 40); // 240: x, 360: y, 80: width, 40: height
    button2_->SetText("Stop");
    button2_->SetTextColor(Color::Silver());
    button2_->SetOnClickListener(this);

    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        GRAPHIC_LOGE("surfaceView is nullptr\n");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetViewId("sur");
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    surfaceView_->SetSurfaceColorkey(colorKey_);
    container_->Add(surfaceView_);
    container_->Add(label_);
    container_->Add(button1_);
    container_->Add(button2_);

    Surface *surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(VIDEO_WIDTH, VIDEO_HEIGHT);
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    // jpeg decoder output buffer size calculation formula
    int32_t ySize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) * VIDEO_HEIGHT;
    int32_t uvSize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) *
        ALIGN_BYTE(VIDEO_HEIGHT, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;
    surface->SetSize(bufferSize);
    if (videoPlay_ == nullptr) {
        std::string uri("/user/video/panda.mp4");
        videoPlay_ = new MediaVideoPlay(surface, button1_, uri, false);
        if (videoPlay_ == nullptr) {
            GRAPHIC_LOGE("videoPlay_ is nullptr\n");
            return;
        }
#ifdef SUPPORT_OHOSFWK
        g_videoPlay = videoPlay_;
        registerNotifyScreenStatus(ScreenStatusNotify);
#endif
    }
}

void UITestVideoPlay::TearDown()
{
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = nullptr;
    deregisterNotifyScreenStatus(ScreenStatusNotify);
#endif
    if (animator_ != nullptr) {
        animator_->Stop();
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
    DeleteChildren(container_);
}

const UIView* UITestVideoPlay::GetTestView()
{
    return container_;
}

bool UITestVideoPlay::OnClick(UIView& view, const ClickEvent& event)
{
    int32_t ret = 0;
    if (videoPlay_ == nullptr) {
        GRAPHIC_LOGE("videoPlay_ is nullptr!\n");
        return false;
    }
    if (&view == button1_) {
        if (videoPlay_->isExited_ || !videoPlay_->isEntered_) {
            ret = videoPlay_->StartVideoPlay();
            return true;
        }
        if (videoPlay_->isPause_) {
            ret = videoPlay_->ResumeVideoPlay();
            if (ret == 0) {
                button1_->SetText("Pause");
            }
        } else {
            ret = videoPlay_->PauseVideoPlay();
            if (ret == 0) {
                button1_->SetText("Play");
            }
        }
    } else if (&view == button2_) {
        if (!videoPlay_->isEntered_) {
            GRAPHIC_LOGE("video is not played\n");
            return false;
        }
        videoPlay_->StopVideoPlay();
    }
    return true;
}
}
