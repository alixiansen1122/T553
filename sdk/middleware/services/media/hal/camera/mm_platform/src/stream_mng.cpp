/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera stream manager impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "stream_mng.h"

using namespace std;
namespace OHOS {
namespace Camera {
const uint32_t MAX_STREAM_NUM = 12;

void StreamMng::StreamCb(uint32_t streamId, const HalBuffer *halBuffer, uint32_t bufferNum, const void *usr)
{
    StreamMng *mng = (StreamMng *)usr;
    auto it = mng->m_idMap.begin();
    for (; it != mng->m_idMap.end(); it++) {
        if (it->second != streamId) {
            continue;
        }
        auto itCb = mng->m_cbMap.find(it->first);
        if (itCb != mng->m_cbMap.end()) {
            for (uint32_t i = 0; i < bufferNum; i++) {
                itCb->second.cb(itCb->second.imp, it->first, halBuffer[i]);
            }
            break;
        }
    }
}

StreamMng::StreamMng(uint32_t camId)
    : m_camId(camId)
{
    m_idMap.clear();
    HwCameraSetBufferCallback(camId, StreamCb, (const HwCameraVoidPtr)this);
}

StreamMng::~StreamMng()
{
    HAL_LOGI("\n");
}

RetCode StreamMng::IsStreamsSupported(const std::vector<HalStreamInfo> &streamInfos, HalStreamSupportType &type)
{
    HW_HAL_UNUSED(streamInfos);
    HW_HAL_UNUSED(type);
    return OK;
}

RetCode StreamMng::CreateStreams(const std::vector<HalStreamInfo> &streamInfos)
{
    uint8_t num = (uint8_t)(streamInfos.size());
    if (num > MAX_STREAM_NUM) {
        return INVALID_PARAM;
    }
    uint32_t streamIdList[MAX_STREAM_NUM] = { 0 };
    vector<StreamAttr> streamAttrList;
    for (auto streamInfo : streamInfos) {
        streamAttrList.emplace_back(streamInfo.attr);
    }
    HwCameraStreamsCreate(m_camId, &(streamAttrList[0]), streamIdList, num);
    uint8_t i = 0;
    for (auto streamInfo : streamInfos) {
        HwCameraStreamOn(m_camId, streamIdList[i]);
        m_idMap.emplace(streamInfo.id, streamIdList[i++]);
    }
    return OK;
}

RetCode StreamMng::ReleaseStreams(const std::vector<int32_t> &streamIds)
{
    for (auto id : streamIds) {
        auto it = m_idMap.find(id);
        if (it != m_idMap.end()) {
            HwCameraStreamOff(m_camId, it->second);
            HwCameraStreamsDestroy(m_camId, &(it->second), 1);
            m_idMap.erase(id);
        }
        m_cbMap.erase(id);
    }
    return OK;
}

RetCode StreamMng::SetStreamInfo(int32_t streamId)
{
    HW_HAL_UNUSED(streamId);
    return OK;
}

RetCode StreamMng::Capture(int32_t streamId, HalBuffer &buffer)
{
    auto it = m_idMap.find(streamId);
    if (it == m_idMap.end()) {
        return OK;
    }
    HwCameraQueueBuf(m_camId, it->second, &buffer, 1);
    return OK;
}

RetCode StreamMng::SetCaptureDoneCb(int32_t streamId, CaptureDoneCb cb)
{
    if (cb.cb != nullptr) {
        m_cbMap.emplace(streamId, cb);
    }
    return OK;
}

RetCode StreamMng::ChangeToOfflineStream(const std::vector<int> &streamIds)
{
    HW_HAL_UNUSED(streamIds);
    return OK;
}
} // namespace Camera
} // namespace OHOS