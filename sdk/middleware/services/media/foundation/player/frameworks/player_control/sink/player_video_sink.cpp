/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "player_video_sink.h"
#include <unistd.h>
#include <sys/time.h>
#include "media_log.h"
#include "player_check.h"
#if defined(ENABLE_UIKIT)
#include "surface.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#endif

using namespace std;
namespace OHOS {
namespace Media {
const int32_t MAX_VIDEO_QUEUE_BUF_NUM = 32;
const float VIDEO_FRAME_RATE_MAX = 120.0f;
const int32_t FLOAT_INT_SCALE = 1000;
const int32_t SS2US = 1000000;
const int32_t US2MS = 1000;
#ifdef SUPPORT_VIDEO_DISPLAY
const int32_t DISPLAY_REGION_WIDTH = 480;
const int32_t DISPLAY_REGION_HEIGHT = 480;
const int32_t REGION_COEFFICIENT = 2;
#endif

// display delay estimated time include surface vsync 16.6ms and screen refresh 16.6ms
const int32_t DISPLAY_DELAY_ESTIMATED_TIME_MS = 33;

static int64_t GetCurTimeUs()
{
    struct timeval ts;
    ts.tv_sec = 0;
    ts.tv_usec = 0;
    gettimeofday(&ts, nullptr);
    return (((int64_t)ts.tv_sec) * SS2US) + ((int64_t)ts.tv_usec);
}

static int64_t GetCurTimeMs()
{
    int64_t curTimeUs = GetCurTimeUs();
    return (int64_t)(curTimeUs / US2MS);
}

VideoSink::VideoSink(void)
    : speed_(1.0f),
      paused_(false),
      started_(false),
      syncHdl_(nullptr),
      renderFrameCnt_(0),
      renderMode_(RENDER_MODE_NORMAL),
      rendStartTime_(-1),
      lastRendPts_(AV_INVALID_PTS),
      recievedEos_(false),
      eosPts_(AV_INVALID_PTS),
      pauseAfterPlay_(false),
      firstVidRend_(false),
      lastRendCnt_(0),
      vidRendStartTime_(AV_INVALID_PTS),
      eosSended_(false)
{
#ifdef SUPPORT_VIDEO_DISPLAY
    lastConfigRegionX_ = VIDEO_INVALID_REGION;
    lastConfigRegionY_ = VIDEO_INVALID_REGION;
    lastConfigRegionW_ = VIDEO_INVALID_REGION;
    lastConfigRegionH_ = VIDEO_INVALID_REGION;
#endif
    ResetRendStartTime();
    attr_.sinkType = SINK_TYPE_BUT;
    attr_.trackId = 0;
    attr_.vidAttr.format = 0;
    attr_.vidAttr.width = 0;
    attr_.vidAttr.height = 0;
    attr_.vidAttr.frameRate = 0.0;
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    attr_.vidAttr.surface = nullptr;
#endif
    lastRendSysTimeMs_ = GetCurTimeMs();
    callBack_.onEventCallback = nullptr;
    callBack_.priv = nullptr;
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
    mutex_ = MediaMutexCreate(nullptr);
}

VideoSink::~VideoSink()
{
    (void)DeInit();
}

int32_t VideoSink::DeInit()
{
    int32_t ret = HI_SUCCESS;
#ifdef SUPPORT_VIDEO_DISPLAY
    layerFuncs_->DeinitDisplay(0);
#endif
    MediaMutexDestroy(&mutex_);
    return ret;
}

int32_t VideoSink::Init(const SinkAttr &attr)
{
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    CHECK_NULL_RETURN(attr.vidAttr.surface, HI_FAILURE, "surface is nullptr");
#endif
    attr_ = attr;
#ifdef SUPPORT_VIDEO_DISPLAY
    (void)LayerInitialize(&layerFuncs_);
    layerFuncs_->InitDisplay(0);
#endif
    return 0;
}

void VideoSink::GetStatus(VideoSinkStatus &status)
{
    status.vidFrameCount = renderFrameCnt_;
    status.decHeight = attr_.vidAttr.height;
    status.decWidth = attr_.vidAttr.width;
    if (vidRendStartTime_ != AV_INVALID_PTS) {
        int64_t diffTimeMs = GetCurTimeMs() - vidRendStartTime_;
        if (diffTimeMs > MS_SCALE) {
            double frameCnt = lastRendCnt_;
            double time = diffTimeMs;
            double frameRate = frameCnt / time;
            if (frameRate >= VIDEO_FRAME_RATE_MAX || frameRate < 0) {
                status.fpsInteger = 0;
                status.fpsDecimal = 0;
                return;
            }

            uint32_t tmp = frameRate * FLOAT_INT_SCALE;
            status.fpsInteger = (tmp / FLOAT_INT_SCALE);
            status.fpsDecimal = (tmp % FLOAT_INT_SCALE);
            return;
        }
    }
    status.fpsInteger = 0;
    status.fpsDecimal = 0;
}

#ifdef SUPPORT_VIDEO_DISPLAY
void VideoSink::SetDefaultDisplayRegionInfo(void)
{
    lastConfigRegionX_ = 0;
    lastConfigRegionY_ = 0;
    lastConfigRegionW_ = DISPLAY_REGION_WIDTH;
    lastConfigRegionH_ = DISPLAY_REGION_HEIGHT;
}

void VideoSink::UpdateDisplayRegionInfo(int32_t x, int32_t y, int32_t w, int32_t h)
{
    lastConfigRegionX_ = x;
    lastConfigRegionY_ = y;
    lastConfigRegionW_ = w;
    lastConfigRegionH_ = h;
}

void VideoSink::CreateAndConfigLayer(void)
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    IRect attr;
    uint32_t devId = 0;
    uint32_t layerId = 0;
    int32_t right = lastConfigRegionX_ + lastConfigRegionW_ - 1;
    int32_t botttom = lastConfigRegionY_ + lastConfigRegionH_ - 1;
    /* Make sure the coordinates are even */
    x = lastConfigRegionX_ - lastConfigRegionX_ % REGION_COEFFICIENT;
    y = lastConfigRegionY_ - lastConfigRegionY_ % REGION_COEFFICIENT;
    w = right - x + 1;
    h = botttom - y + 1;
    w = w + w % REGION_COEFFICIENT;
    h = h + h % REGION_COEFFICIENT;
    attr.x = x;
    attr.y = y;
    attr.w = w;
    attr.h = h;
    LayerInfo lInfo;
    lInfo.width = w;
    lInfo.height = h;
    if (layerFuncs_ != nullptr) {
        layerFuncs_->CreateLayer(devId, &lInfo, &layerId);
        layerFuncs_->SetLayerSize(devId, layerId, &attr);
    }
}

void VideoSink::CheckConfigVideoOutput(void)
{
#if defined(ENABLE_UIKIT)
    Surface *surface = attr_.vidAttr.surface;
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface = attr_.vidAttr.surface;
#endif
    if (surface == nullptr) {
        SetDefaultDisplayRegionInfo();
        return CreateAndConfigLayer();
    }
    int32_t x = std::stoi(surface->GetUserData("region_position_x"));
    int32_t y = std::stoi(surface->GetUserData("region_position_y"));
    int32_t w = std::stoi(surface->GetUserData("region_width"));
    int32_t h = std::stoi(surface->GetUserData("region_height"));
    UpdateDisplayRegionInfo(x, y, w, h);

    CreateAndConfigLayer();
}
#endif

int32_t VideoSink::Start()
{
    vidRendStartTime_ = GetCurTimeMs();
#ifdef SUPPORT_VIDEO_DISPLAY
    CheckConfigVideoOutput();
#endif
    started_ = true;
    return HI_SUCCESS;
}

void VideoSink::SetRenderMode(RenderMode mode)
{
    // use to control report first video frame
    renderMode_ = mode;
}

int32_t VideoSink::Stop()
{
    ReleaseQueAllFrame();
    ResetRendStartTime();
    renderFrameCnt_ = 0;
#ifdef SUPPORT_VIDEO_DISPLAY
    if (layerFuncs_ != nullptr) {
        layerFuncs_->CloseLayer(0, 0);
    }
#endif
    started_ = false;
    return HI_SUCCESS;
}

int32_t VideoSink::Pause()
{
    if (paused_) {
        MEDIA_WARNING_LOG("vsink already paused");
        return HI_SUCCESS;
    }
    if (!started_) {
        MEDIA_ERR_LOG("vsink not in running");
        return HI_FAILURE;
    }

    ResetRendStartTime();
    paused_ = true;
    return HI_SUCCESS;
}

int32_t VideoSink::Resume(void)
{
    renderMode_ = RENDER_MODE_NORMAL;
    if (!paused_) {
        MEDIA_WARNING_LOG("vsink not in pause");
        return HI_FAILURE;
    }

    vidRendStartTime_ = GetCurTimeMs();
    lastRendSysTimeMs_ = GetCurTimeMs();
    paused_ = false;
    return HI_SUCCESS;
}

int32_t VideoSink::Flush(void)
{
    if (!started_) {
        MEDIA_ERR_LOG("vsink not in started");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

void VideoSink::ResetRendStartTime()
{
    lastRendCnt_ = 0;
    lastRendPts_ = AV_INVALID_PTS;
    vidRendStartTime_ = AV_INVALID_PTS;
}

int32_t VideoSink::Reset()
{
    ReleaseQueAllFrame();
    Flush();
    ResetRendStartTime();
    recievedEos_ = false;
    firstVidRend_ = false;
    return HI_SUCCESS;
}


int32_t VideoSink::RegisterCallBack(const PlayEventCallback &callback)
{
    callBack_ = callback;
    return 0;
}

void VideoSink::QueueRenderFrame(OutputInfo &frame, bool cacheQueue)
{
    if (frame.bufferCnt == 0) {
        return;
    }
    MediaMutexLock(mutex_);
    if (cacheQueue) {
        frameCacheQue_.push_back(frame);
    } else {
        frameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}

int32_t VideoSink::GetRenderFrame(OutputInfo &renderFrame, OutputInfo &frame)
{
    MediaMutexLock(mutex_);
    int32_t ret = SINK_QUE_EMPTY;
    if (frame.type == VIDEO_DECODER && frame.bufferCnt != 0) {
        frameCacheQue_.push_back(frame);
    }
    if (frameCacheQue_.size() != 0) {
        renderFrame = frameCacheQue_[0];
        ret = SINK_SUCCESS;
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

void VideoSink::ReleaseQueHeadFrame(void)
{
    MediaMutexLock(mutex_);
    if (frameCacheQue_.size() != 0) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}

void VideoSink::ReleaseQueAllFrame(void)
{
    size_t i;
    size_t queSize;
    MediaMutexLock(mutex_);
    queSize = frameCacheQue_.size();
    if (queSize > MAX_VIDEO_QUEUE_BUF_NUM) {
        MediaMutexUnLock(mutex_);
        return;
    }
    for (i = 0; i < queSize; i++) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
#if defined(ENABLE_UIKIT)
        Surface *surface = attr_.vidAttr.surface;
        if (surface != nullptr) {
            surface->CancelBuffer(static_cast<SurfaceBuffer *>(frame.vendorPrivate));
        }
#elif defined(ENABLE_LVGL)
        LvSurfaceView *surface = attr_.vidAttr.surface;
        if (surface != nullptr) {
            LvFlushBuffer(reinterpret_cast<lv_obj_t *>(surface), static_cast<LvSurfaceBuffer *>(frame.vendorPrivate));
        }
#endif
    }

    MediaMutexUnLock(mutex_);
}

bool VideoSink::HaveCacheFrame(void)
{
    MediaMutexLock(mutex_);

    bool haveCache = frameCacheQue_.size() != 0;

    MediaMutexUnLock(mutex_);

    return haveCache;
}

int32_t VideoSink::DequeReleaseFrame(OutputInfo &frame)
{
    MediaMutexLock(mutex_);
    if (frameReleaseQue_.size() == 0) {
        MediaMutexUnLock(mutex_);
        return SINK_QUE_EMPTY;
    }
    frame = frameReleaseQue_[0];
    frameReleaseQue_.erase(frameReleaseQue_.begin());
    MediaMutexUnLock(mutex_);
    return SINK_SUCCESS;
}

void VideoSink::RenderRptEvent(EventCbType event)
{
    if (callBack_.onEventCallback != nullptr) {
        if (event == EVNET_VIDEO_PLAY_EOS && eosSended_) {
            return;
        }
        callBack_.onEventCallback(callBack_.priv, event, 0, 0);
        if (event == EVNET_VIDEO_PLAY_EOS) {
            eosSended_ = true;
        }
    }
}

int32_t VideoSink::WriteToVideoDevice(OutputInfo &renderFrame)
{
#ifdef SUPPORT_VIDEO_DISPLAY
    if (layerFuncs_ != nullptr) {
        LayerBuffer layerBuf;
        layerBuf.data.virAddr = renderFrame.vendorPrivate;
        layerFuncs_->Flush(0, 0, &layerBuf);
    }
#else
#if defined(ENABLE_UIKIT)
    Surface *surface = attr_.vidAttr.surface;
    surface->FlushBuffer(static_cast<SurfaceBuffer *>(renderFrame.vendorPrivate));
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface = attr_.vidAttr.surface;
    LvFlushBuffer(reinterpret_cast<lv_obj_t *>(surface), static_cast<LvSurfaceBuffer *>(renderFrame.vendorPrivate));
#endif
#endif
    ReleaseQueHeadFrame();
    return SINK_SUCCESS;
}

void VideoSink::SyncReleaseVideoFrame(OutputInfo &renderFrame)
{
    ReleaseQueHeadFrame();
#if defined(ENABLE_UIKIT)
    Surface *surface = attr_.vidAttr.surface;
    surface->CancelBuffer(static_cast<SurfaceBuffer *>(renderFrame.vendorPrivate));
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface = attr_.vidAttr.surface;
    LvFlushBuffer(reinterpret_cast<lv_obj_t *>(surface), static_cast<LvSurfaceBuffer *>(renderFrame.vendorPrivate));
#endif
}

int32_t VideoSink::RenderFrame(OutputInfo &frame)
{
    int64_t crtPlayPts = 0;
    SyncRet syncRet = SYNC_RET_PLAY;
    OutputInfo renderFrame = {};

    /* the frame should be save to queue at state paused and none-started */
    if (!started_ || paused_ || (renderMode_ == RENDER_MODE_PAUSE_AFTER_PLAY && renderFrameCnt_ == 1)) {
        QueueRenderFrame(frame, started_ ? true : false);
        return SINK_SUCCESS;
    }
    if (GetRenderFrame(renderFrame, frame) != SINK_SUCCESS) {
        if (recievedEos_ == true) {
            RenderRptEvent(EVNET_VIDEO_PLAY_EOS);
            return SINK_RENDER_EOS;
        }
        return SINK_QUE_EMPTY;
    }

    int32_t diff = renderFrame.timeStamp - DISPLAY_DELAY_ESTIMATED_TIME_MS;
    crtPlayPts = (diff > 0) ? diff : 0;

    int32_t ret = (syncHdl_ != nullptr) ? syncHdl_->ProcVidFrame(crtPlayPts, syncRet) : HI_SUCCESS;
    if (ret != HI_SUCCESS) {
        ReleaseQueHeadFrame();
        MEDIA_ERR_LOG("process video frame pts: %lld failed", renderFrame.timeStamp);
        return SINK_RENDER_ERROR;
    }
    if (syncRet == SYNC_RET_PLAY) {
        ret = WriteToVideoDevice(renderFrame);
        if (renderFrameCnt_ == 0) {
            callBack_.onEventCallback(callBack_.priv, EVNET_FIRST_VIDEO_REND, renderFrame.timeStamp, 0);
        }
        renderFrameCnt_++;
    } else if (syncRet == SYNC_RET_DROP) {
        MEDIA_INFO_LOG("video too late pts: %lld", renderFrame.timeStamp);
        SyncReleaseVideoFrame(renderFrame);
        ret = SINK_SUCCESS;
    } else if (syncRet == SYNC_RET_REPEAT) {
        ret = SINK_RENDER_DELAY;
    } else {
        MEDIA_ERR_LOG("video invalid sync ret: %d", syncRet);
        SyncReleaseVideoFrame(renderFrame);
        ret = SINK_RENDER_ERROR;
    }

    /* render pts update after the frame that have been processed */
    if (ret == SINK_SUCCESS || ret == SINK_RENDER_ERROR) {
        lastRendPts_ = renderFrame.timeStamp;
    }
    return ret;
}

void VideoSink::SetSync(PlayerSync *sync)
{
    syncHdl_ = sync;
}

void VideoSink::RenderEos(void)
{
    recievedEos_ = true;
    eosPts_ = lastRendPts_;
}

void VideoSink::GetRenderPosition(int64_t &position)
{
    position = lastRendPts_;
}
}
}
