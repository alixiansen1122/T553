/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera device manager header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef CAMERA_DEVICE_MNG_H
#define CAMERA_DEVICE_MNG_H

#include "camera_device_base.h"

namespace OHOS {
namespace Camera {
class CameraDeviceMng {
public:
    CameraDeviceMng();
    ~CameraDeviceMng();
    RetCode SetDeviceStatusCb();
    RetCode GetCameraIds(std::vector<std::string> &cameraIds);
    RetCode GetCameraAbility(const std::string &cameraId, std::shared_ptr<DataTableManager> &dtMgr);
    RetCode OpenCamera(const std::string &cameraId, std::shared_ptr<CameraDeviceBase> &dev);
    RetCode SetFlashlight(const std::string &cameraId, bool &isEnable);
    RetCode Init();
public:
    CameraDeviceMng(const CameraDeviceMng &other) = delete;
    CameraDeviceMng(CameraDeviceMng &&other) = delete;
    CameraDeviceMng& operator=(const CameraDeviceMng &other) = delete;
    CameraDeviceMng& operator=(CameraDeviceMng &&other) = delete;
private:
    std::vector<uint32_t> m_cameraIds;
};
} // namespace Camera
} // namespace OHOS

#endif // CAMERA_DEVICE_MNG_H