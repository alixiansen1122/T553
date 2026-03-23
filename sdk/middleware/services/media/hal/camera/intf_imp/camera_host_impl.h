/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera host impl header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef CAMERA_HOST_CAMERA_HOST_IMP_H
#define CAMERA_HOST_CAMERA_HOST_IMP_H

#include "icamera_host.h"
#include "camera_device_mng.h"

namespace OHOS {
namespace Camera {
class CameraHostImpl : public ICameraHost {
public:
    CameraHostImpl();
    ~CameraHostImpl() override;
    CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) override;
    CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) override;
    CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability) override;
    CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &device) override;
    CamRetCode SetFlashlight(const std::string &cameraId, bool &isEnable) override;
    CamRetCode Init();
public:
    CameraHostImpl(const CameraHostImpl &other) = delete;
    CameraHostImpl(CameraHostImpl &&other) = delete;
    CameraHostImpl& operator=(const CameraHostImpl &other) = delete;
    CameraHostImpl& operator=(CameraHostImpl &&other) = delete;
private:
    CameraDeviceMng *m_devMng;
};
} // namespace Camera
} // namespace OHOS

#endif // CAMERA_HOST_CAMERA_HOST_H
