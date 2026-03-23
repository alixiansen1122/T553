/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialVideoView.h"

namespace OHOS {
static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr int16_t BUFFER_QUEUE_SIZE = 2;
static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;

static inline int32_t MediaByteAlign(int16_t value, int16_t align)
{
    return (((value) + (align) - 1) & (~((align) - 1)));
}

DialVideoView::DialVideoView()
{
}

DialVideoView::~DialVideoView()
{
    if (videoPlayer_ != nullptr) {
        delete videoPlayer_;
    }
}

bool DialVideoView::SetVideoRes(FILE* fp, uint32_t offset)
{
    if (videoPlayer_ != nullptr) {
        GRAPHIC_LOGE("Cannot set video res repeatedly!\n");
        return false;
    }

    fp_ = fp;
    offset_ = offset;

    Surface* surface = GetSurface();
    Rect area = GetRect();
    int16_t w = area.GetWidth();
    int16_t h = area.GetHeight();
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

    int32_t fd = fileno(fp_);
    if (fd == -1) {
        return false;
    }

    videoPlayer_ = new MediaVideoPlay(surface, fd, offset_, true);
    videoPlayer_->SetSyncExitMode(true);
    return true;
}

bool DialVideoView::StartPlay()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    videoPlayer_->StartVideoPlay();
    return true;
}

bool DialVideoView::ResumePlay()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    videoPlayer_->ResumeVideoPlay();
    return true;
}

bool DialVideoView::PausePlay()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    videoPlayer_->PauseVideoPlay();
    return true;
}

bool DialVideoView::StopPlay()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    videoPlayer_->StopVideoPlay();
    return true;
}

bool DialVideoView::IsPaused()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    return videoPlayer_->isPause_;
}

bool DialVideoView::IsPlayed()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    return videoPlayer_->isPlayed_;
}

bool DialVideoView::IsStopped()
{
    if (videoPlayer_ == nullptr) {
        return false;
    }
    return videoPlayer_->isExited_;
}

MediaVideoPlay* DialVideoView::GetVideoPlay()
{
    return videoPlayer_;
}
}
