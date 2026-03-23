/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera stream operator impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "stream_operator_impl.h"
#include <sys/prctl.h>
#include <unistd.h>
using namespace std;
namespace OHOS {
namespace Camera {
const int STREAM_FPS = 30;
const float YCBCR420SP_BUFFER_SIZE_COEFFICIENT = 1.5;
// Waiting time calculated based on the frame rate of 60, 1000 / 60 = 16ms
const int WAIT_FLUSHBUFFER_TO_SURFACE_TIME = 16000;

StreamOperatorImpl::StreamOperatorImpl(std::shared_ptr<StreamMng> &streamMng)
    : m_streamMng(streamMng)
{
    m_streamBaseInfoMap.clear();
    MediaHalMutexAttr attr = { true };
    m_infoMutex = MediaHalMutexCreate(&attr);
    Init();
}

void StreamOperatorImpl::ReleaseAll()
{
    (void)MediaHalMutexLock(m_infoMutex);
    for (auto baseInfo = m_streamBaseInfoMap.begin(); baseInfo != m_streamBaseInfoMap.end(); baseInfo++) {
        map<const SurfaceBuffer *, HalBuffer *> bufferMap = baseInfo->second->bufferMap;
        for (auto it = bufferMap.begin(); it != bufferMap.end(); it++) {
            delete (it->second);
            it->second = nullptr;
        }
    }
    m_streamBaseInfoMap.clear();
    (void)MediaHalMutexUnLock(m_infoMutex);
}

StreamOperatorImpl::~StreamOperatorImpl()
{
    HAL_LOGI("");
    ReleaseAll();
    m_isInit = false;
    MediaHalThreadJoin(&m_thread);
    MediaHalMutexDestroy(&m_infoMutex);
}

CamRetCode StreamOperatorImpl::Init()
{
    m_isInit = true;
    MediaHalThreadattr attr = { "CameraBufferLoop", 0, THREAD_SCHED_INVALID, 0, false };
    m_thread = MediaHalThreadCreate(BufferLoopThread, this, &attr);
    if (m_thread == nullptr) {
        HAL_LOGI("fork thread StreamCopyProcess failed");
        return INSUFFICIENT_RESOURCES;
    }

    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::IsStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::shared_ptr<StreamInfo> &info,
    StreamSupportType &type)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    HW_HAL_UNUSED(info);
    type = DYNAMIC_SUPPORTED;
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::IsStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>> &info,
    StreamSupportType &type)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    HW_HAL_UNUSED(info);
    type = DYNAMIC_SUPPORTED;
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos)
{
    vector<HalStreamInfo> halStreamInfos;
    for (auto streamInfo : streamInfos) {
        shared_ptr<StreamBaseInfo> streamBaseInfo = make_shared<StreamBaseInfo>();
        streamBaseInfo->captureIds.clear();
        streamBaseInfo->bufferMap.clear();
        streamBaseInfo->bufferQueue = streamInfo->bufferQueue_;
        if (memset_s(&(streamBaseInfo->attr), sizeof(streamBaseInfo->attr), 0, sizeof(streamBaseInfo->attr)) != EOK) {
            HAL_LOGE("memset_s failed");
        }
        streamBaseInfo->attr.width = streamInfo->width_;
        streamBaseInfo->attr.height = streamInfo->height_;
        streamBaseInfo->attr.format = FORMAT_YVU420; // streamInfo->format_;
        streamBaseInfo->attr.type = STREAM_PREVIEW; // streamInfo->intent_;
        streamBaseInfo->attr.fps = STREAM_FPS; // streamInfo->intent_;

        (void)MediaHalMutexLock(m_infoMutex);
        m_streamBaseInfoMap.emplace(streamInfo->streamId_, streamBaseInfo);
        (void)MediaHalMutexUnLock(m_infoMutex);
        HAL_LOGI("create stream id %d w = %d h = %d", streamInfo->streamId_,
            streamInfo->width_, streamInfo->height_);
        HalStreamInfo info;
        info.id = streamInfo->streamId_;
        info.attr = streamBaseInfo->attr;
        halStreamInfos.emplace_back(info);
        CaptureDoneCb captureDoneCb;
        captureDoneCb.cb = BufferDone;
        captureDoneCb.imp = this;
        m_streamMng->SetCaptureDoneCb(streamInfo->streamId_, captureDoneCb);
    }
    m_streamMng->CreateStreams(halStreamInfos);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::ReleaseStreams(const std::vector<int> &streamIds)
{
    m_streamMng->ReleaseStreams(streamIds);
    (void)MediaHalMutexLock(m_infoMutex);
    for (auto streamId : streamIds) {
        auto baseInfo = m_streamBaseInfoMap.find(streamId);
        if (baseInfo != m_streamBaseInfoMap.end()) {
            map<const SurfaceBuffer *, HalBuffer *> bufferMap = baseInfo->second->bufferMap;
            for (auto it = bufferMap.begin(); it != bufferMap.end(); it++) {
                delete (it->second);
                it->second = NULL;
            }
            m_streamBaseInfoMap.erase(streamId);
        }
    }
    (void)MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CommitStreams(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting)
{
    HW_HAL_UNUSED(mode);
    HW_HAL_UNUSED(modeSetting);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>> &attributes)
{
    HW_HAL_UNUSED(attributes);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::Surface> &producer)
{
    HW_HAL_UNUSED(streamId);
    HW_HAL_UNUSED(producer);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::DetachBufferQueue(int streamId)
{
    HW_HAL_UNUSED(streamId);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::Capture(int captureId, const std::shared_ptr<CaptureInfo> &info,  bool isStreaming)
{
    (void)MediaHalMutexLock(m_infoMutex);
    for (auto streamId : info->streamIds_) {
        HAL_LOGI("capture stream id = %d", streamId);
        auto pair = m_streamBaseInfoMap.find(streamId);
        if (pair != m_streamBaseInfoMap.end()) {
            CaptureId capture;
            capture.isStreaming = isStreaming;
            capture.captureId = captureId;
            pair->second->captureIds.emplace_back(capture);
        }
    }
    (void)MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::CancelCapture(int captureId)
{
    (void)MediaHalMutexLock(m_infoMutex);
    for (auto it = m_streamBaseInfoMap.begin(); it != m_streamBaseInfoMap.end(); it++) {
        for (auto itList = it->second->captureIds.begin(); itList != it->second->captureIds.end();) {
            if (itList->captureId == captureId) {
                it->second->captureIds.erase(itList++);
            } else {
                itList++;
            }
        }
    }
    (void)MediaHalMutexUnLock(m_infoMutex);
    return NO_ERROR;
}

CamRetCode StreamOperatorImpl::ChangeToOfflineStream(const std::vector<int> &streamIds,
    OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IOfflineStreamOperator> &offlineOperator)
{
    HW_HAL_UNUSED(streamIds);
    HW_HAL_UNUSED(callback);
    HW_HAL_UNUSED(offlineOperator);
    return NO_ERROR;
}

HalBuffer& StreamOperatorImpl::GetHalBuffer(const SurfaceBuffer *surfaceBuf,
    std::map<const SurfaceBuffer *, HalBuffer *> &bufferMap)
{
    auto it = bufferMap.find(surfaceBuf);
    if (it != bufferMap.end()) {
        return *(it->second);
    }
    HalBuffer *halBuffer = new HalBuffer;
    if (memset_s(halBuffer, sizeof(HalBuffer), 0, sizeof(HalBuffer)) != EOK) {
        HAL_LOGE("memset_s failed");
    }
    halBuffer->virAddr = (void *)(uintptr_t)(surfaceBuf->addr);
    halBuffer->size = static_cast<int32_t>(surfaceBuf->width * surfaceBuf->height * YCBCR420SP_BUFFER_SIZE_COEFFICIENT);
    halBuffer->phyAddr = surfaceBuf->addr;
    bufferMap.emplace(surfaceBuf, halBuffer);
    return *halBuffer;
}

void StreamOperatorImpl::BufferDone(const void *imp, int32_t streamId, const HalBuffer &buffer)
{
    StreamOperatorImpl *obj = (StreamOperatorImpl *)imp;
    (void)MediaHalMutexLock((obj->m_infoMutex));
    auto baseInfo = obj->m_streamBaseInfoMap.find(streamId);
    if (baseInfo == obj->m_streamBaseInfoMap.end()) {
        (void)MediaHalMutexUnLock((obj->m_infoMutex));
        return;
    }

    map<const SurfaceBuffer *, HalBuffer *> bufferMap = baseInfo->second->bufferMap;
    for (auto it = bufferMap.begin(); it != bufferMap.end(); it++) {
        if (it->second->virAddr == buffer.virAddr) {
            baseInfo->second->bufferQueue->FlushBuffer((SurfaceBuffer*)it->first);
            break;
        }
    }
    (void)MediaHalMutexUnLock((obj->m_infoMutex));
}

void *StreamOperatorImpl::BufferLoopThread(void *arg)
{
    HAL_LOGI("enter BufferLoopThread start");
    prctl(PR_SET_NAME, "CameraBufferLoop");
    StreamOperatorImpl *obj = (StreamOperatorImpl *)arg;
    CaptureId capture;
    SurfaceBuffer *surfaceBuf = nullptr;
    while (obj->m_isInit) {
        {
            (void)MediaHalMutexLock((obj->m_infoMutex));
            for (auto it = obj->m_streamBaseInfoMap.begin(); it != obj->m_streamBaseInfoMap.end(); it++) {
                if (it->second->bufferQueue == nullptr || it->second->captureIds.empty()) {
                    continue;
                }
                surfaceBuf = it->second->bufferQueue->RequestBuffer();
                if (surfaceBuf == nullptr) {
                    continue;
                }
                HAL_LOGD("start get buffer done");
                capture = it->second->captureIds.front();
                if (!capture.isStreaming) {
                    it->second->captureIds.pop_front();
                }
                obj->m_streamMng->Capture(it->first, GetHalBuffer(surfaceBuf, it->second->bufferMap));
            }
            (void)MediaHalMutexUnLock((obj->m_infoMutex));
        }
        usleep(WAIT_FLUSHBUFFER_TO_SURFACE_TIME);
    }
    return nullptr;
}

} // namespace Camera
} // namespace OHOS