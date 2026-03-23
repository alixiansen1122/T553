/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera stream manager header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef CAMERA_HAL_STREAM_MNG_H
#define CAMERA_HAL_STREAM_MNG_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "common_type.h"

namespace OHOS {
namespace Camera {
typedef void (*CaptureDone)(const void *imp, int32_t streamId, const HalBuffer &buffer);

typedef struct {
    CaptureDone cb;
    void *imp;
} CaptureDoneCb;

class StreamMng {
public:
    RetCode IsStreamsSupported(const std::vector<HalStreamInfo> &streamInfos, HalStreamSupportType &type);
    RetCode CreateStreams(const std::vector<HalStreamInfo> &streamInfos);
    RetCode ReleaseStreams(const std::vector<int32_t> &streamIds);
    RetCode SetStreamInfo(int32_t streamId);
    RetCode Capture(int32_t streamId, HalBuffer &buffer);
    RetCode SetCaptureDoneCb(int32_t streamId, CaptureDoneCb cb);
    RetCode ChangeToOfflineStream(const std::vector<int> &streamIds);
public:
    explicit StreamMng(uint32_t camId);
    ~StreamMng();
    StreamMng(const StreamMng &other) = delete;
    StreamMng(StreamMng &&other) = delete;
    StreamMng& operator=(const StreamMng &other) = delete;
    StreamMng& operator=(StreamMng &&other) = delete;
private:
    static void StreamCb(uint32_t streamId, const HalBuffer *halBuffer, uint32_t bufferNum, const void *usr);
private:
    uint32_t m_camId;
    std::map<int32_t, uint32_t> m_idMap;
    std::map<int32_t, CaptureDoneCb> m_cbMap;
};
} // namespace Camera
} // namespace OHOS

#endif
