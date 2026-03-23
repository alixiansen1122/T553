/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera operator header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef STREAM_OPERATOR_IMPL_STREAM_OPERATOR_IMPL_H
#define STREAM_OPERATOR_IMPL_STREAM_OPERATOR_IMPL_H

#include <thread>
#include <list>
#include "istream_operator.h"
#include "icamera_device.h"
#include "stream_mng.h"
#include "media_hal_thread_adapt.h"

namespace OHOS {
namespace Camera {
class StreamOperatorImpl : public IStreamOperator {
public:
    CamRetCode Init();
    CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::shared_ptr<StreamInfo> &info,
        StreamSupportType &type) override;
    CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::vector<std::shared_ptr<StreamInfo>> &info,
        StreamSupportType &type) override;
    CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) override;
    CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    CamRetCode CommitStreams(OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting) override;
    CamRetCode GetStreamAttributes(
        std::vector<std::shared_ptr<StreamAttribute>> &attributes) override;
    CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::Surface> &producer) override;
    CamRetCode DetachBufferQueue(int streamId) override;
    CamRetCode Capture(int captureId, const std::shared_ptr<CaptureInfo> &info,  bool isStreaming) override;
    CamRetCode CancelCapture(int captureId) override;
    CamRetCode ChangeToOfflineStream(const std::vector<int> &streamIds,
        OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IOfflineStreamOperator> &offlineOperator) override;
    static void BufferDone(const void *imp, int32_t streamId, const HalBuffer &buffer);
public:
    explicit StreamOperatorImpl(std::shared_ptr<StreamMng> &streamMng);
    ~StreamOperatorImpl() override;
    StreamOperatorImpl(const StreamOperatorImpl &other) = delete;
    StreamOperatorImpl(StreamOperatorImpl &&other) = delete;
    StreamOperatorImpl& operator=(const StreamOperatorImpl &other) = delete;
    StreamOperatorImpl& operator=(StreamOperatorImpl &&other) = delete;
private:
    void ReleaseAll();
    static void *BufferLoopThread(void *arg);
    static HalBuffer& GetHalBuffer(const SurfaceBuffer *surfaceBuf,
    std::map<const SurfaceBuffer *, HalBuffer *> &bufferMap);
    using CaptureId = struct {
        bool isStreaming;
        int32_t captureId;
    };
    using StreamBaseInfo = struct {
        StreamAttr attr;
        std::list<CaptureId> captureIds;
        OHOS::Surface *bufferQueue;
        std::map<const SurfaceBuffer *, HalBuffer *> bufferMap;
    };
private:
    std::shared_ptr<StreamMng> m_streamMng;
    MediaHalThreadIdHandle m_thread;
    bool m_isInit;
    std::map<int32_t, std::shared_ptr<StreamBaseInfo>> m_streamBaseInfoMap;
    MediaHalMutexHandle m_infoMutex;
};
} // namespace Camera
} // namespace OHOS

#endif
