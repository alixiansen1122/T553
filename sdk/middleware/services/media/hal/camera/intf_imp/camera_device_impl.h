/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera device header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef CAMERA_DEVICE_IMPL_CAMERA_DEVICE_IMPL_H
#define CAMERA_DEVICE_IMPL_CAMERA_DEVICE_IMPL_H

#include "icamera_device.h"
#include "camera_device_base.h"

namespace OHOS {
namespace Camera {
class StreamOperator;
class CameraDeviceImpl : public ICameraDevice {
public:
    CamRetCode GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IStreamOperator> &streamOperator) override;
    CamRetCode UpdateSettings(const void *value, int size) override;
    CamRetCode SetResultMode(const ResultCallbackMode &mode) override;
    CamRetCode GetEnabledResults(std::vector<MetaType> &results) override;
    CamRetCode EnableResult(const std::vector<MetaType> &results) override;
    CamRetCode DisableResult(const std::vector<MetaType> &results) override;
    void Close() override;
public:
    CameraDeviceImpl(const std::string &cameraId, std::shared_ptr<CameraDeviceBase> &deviceBase);
    CameraDeviceImpl();
    ~CameraDeviceImpl() override;
    CameraDeviceImpl(const CameraDeviceImpl& other) = delete;
    CameraDeviceImpl(CameraDeviceImpl &&other) = delete;
    CameraDeviceImpl& operator=(const CameraDeviceImpl &other) = delete;
    CameraDeviceImpl& operator=(CameraDeviceImpl &&other) = delete;
private:
    std::string m_cameraId;
    std::shared_ptr<CameraDeviceBase> m_deviceBase;
};
} // namespace Camera
} // namespace OHOS

#endif // CAMERA_DEVICE_CAMERA_DEVICE_H
