#include <cstdio>
#include <iostream>
#include "videocard/VideoCard.h"
#include "NativeAbility.h"
#include "msg_center.h"
#include "securec.h"
#include "main/DialBinParser.h"
#include "main/dial/ModelDialDataFactory.h"
#include "common/low_power_manager.h"
#include "common/image_cache_manager.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#include "main/MainPresenterSample.h"
#include "common/graphic_utils.h"

namespace OHOS {
static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr int16_t BUFFER_QUEUE_SIZE = 3;
static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;

static inline int32_t MediaByteAlign(int16_t value, int16_t align)
{
    return (((value) + (align) - 1) & (~((align) - 1)));
}

VideoCard::VideoCard()
{
    SetTouchable(true);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
}

VideoCard::~VideoCard()
{
    Deinit();
}

void VideoCard::OnActive(void)
{
    StartVideo();
}

void VideoCard::OnInactive()
{
    StopVideo();
}

void VideoCard::PreLoad(void)
{
    if (!resPreloaded_) {
        Init();
        resPreloaded_ = true;
    }
}

void VideoCard::ScrollBegin(bool isActive)
{
    PauseVideo();
}

void VideoCard::ScrollEnd(bool isActive)
{
    if (isActive) {
        StartVideo();
    } else {
        StopVideo();
        ClearCache();
    }
}

void VideoCard::SetFilePath(const std::string &path)
{
    Deinit();
    path_ = path;
    resPreloaded_ = false;
}

void VideoCard::SetPreviewFile(const std::string &path)
{
    hasPreview_ = true;
    previewPath_ = path;
}

void VideoCard::OnPause(void)
{
    PauseVideo();
}

void VideoCard::StopVideo()
{
    if (video_ == nullptr) {
        return;
    }

    if (video_->isPlayed_ || video_->isPause_) {
        video_->StopVideoPlay();
    }
}

void VideoCard::StartVideo()
{
    if (video_ == nullptr) {
        return;
    }

    if (video_->isPause_) {
        video_->ResumeVideoPlay();
    } else if (!video_->isPlayed_) {
        video_->StartVideoPlay();
    }
}

void VideoCard::PauseVideo()
{
    if (video_ == nullptr) {
        return;
    }

    if (video_->isPlayed_) {
        video_->PauseVideoPlay();
    }
}
void VideoCard::Deinit()
{
    Remove(surfaceView_);
    if (video_ != nullptr) {
        delete video_;
        video_ = nullptr;
    }
    if (surfaceView_ != nullptr) {
        delete surfaceView_;
        surfaceView_ = nullptr;
    }
}

void VideoCard::Init()
{
    if (surfaceView_ != nullptr || video_ != nullptr) {
        return;
    }
    surfaceView_ = new UILiteSurfaceView;
    if (surfaceView_ == nullptr) {
        return;
    }
    Rect area = GetRect();
    int16_t w = area.GetWidth();
    int16_t h = area.GetHeight();
    surfaceView_->SetPosition(0, 0, w, h);
    ColorType color;
    color.full = 0xff3d3a3c;
    surfaceView_->SetSurfaceColorkey(color);

    Surface* surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(w, h);
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    // jpeg decoder output buffer size calculation formula
    int32_t ySize = MediaByteAlign(w, JPEG_WIDTH_BYTE_ALIGNMENT) * h;
    int32_t uvSize = MediaByteAlign(w, JPEG_WIDTH_BYTE_ALIGNMENT) *
        MediaByteAlign(h, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;
    surface->SetSize(bufferSize);

    video_ = new MediaVideoPlay(surface, nullptr, path_, true);
    if (video_ == nullptr) {
        delete surfaceView_;
        surfaceView_ = nullptr;
        return;
    }
    if (hasPreview_) {
        ImageInfo *imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(previewPath_.c_str());
        if (imgInfo != nullptr) {
            surfaceView_->SetPreview(imgInfo);
        }
    }
    Add(surfaceView_);
    video_->SetSyncExitMode(true);
}
} // OHOS
