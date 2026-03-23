/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera device base header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef CAMERA_DEVICE_BASE_H
#define CAMERA_DEVICE_BASE_H

#include "stream_mng.h"

namespace OHOS {
namespace Camera {
class CameraDeviceBase {
public:
    explicit CameraDeviceBase(uint32_t id);
    ~CameraDeviceBase();
    RetCode GetStreamMng(std::shared_ptr<StreamMng> &streamMng);
    RetCode UpdateSettings(const void *value, int size);
public:
    CameraDeviceBase(const CameraDeviceBase &other) = delete;
    CameraDeviceBase(CameraDeviceBase &&other) = delete;
    CameraDeviceBase& operator=(const CameraDeviceBase &other) = delete;
    CameraDeviceBase& operator=(CameraDeviceBase &&other) = delete;
private:
    uint32_t m_id;
};
} // namespace Camera
} // namespace OHOS

#endif // CAMERA_DEVICE_BASE_H