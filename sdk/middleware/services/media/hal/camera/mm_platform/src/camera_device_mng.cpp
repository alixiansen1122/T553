/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: camera device mananger impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "camera_device_mng.h"
using namespace std;

namespace OHOS {
namespace Camera {
CameraDeviceMng::CameraDeviceMng()
{
}
CameraDeviceMng::~CameraDeviceMng()
{
    HAL_LOGI("\n");
    HwCameraDeinit();
}

RetCode CameraDeviceMng::Init()
{
    HwCameraInit();
    uint8_t num = 0;
    HwCameraGetDeviceNum(&num);
    for (uint8_t i = 0; i < num; i++) {
        m_cameraIds.push_back(i);
    }
    return OK;
}

RetCode CameraDeviceMng::SetDeviceStatusCb()
{
    return OK;
}

RetCode CameraDeviceMng::GetCameraIds(std::vector<std::string> &cameraIds)
{
    cameraIds.clear();
    for (uint8_t i = 0; i < m_cameraIds.size(); i++) {
        cameraIds.push_back(to_string(m_cameraIds[i]));
    }
    return OK;
}

RetCode CameraDeviceMng::GetCameraAbility(const std::string &cameraId, std::shared_ptr<DataTableManager> &dtMgr)
{
    HW_HAL_UNUSED(cameraId);
    HW_HAL_UNUSED(dtMgr);
    return OK;
}

RetCode CameraDeviceMng::OpenCamera(const std::string &cameraId, std::shared_ptr<CameraDeviceBase> &dev)
{
    uint32_t id = (uint32_t)std::stoi(cameraId.c_str());
    dev = make_shared<CameraDeviceBase>(id);
    return OK;
}

RetCode CameraDeviceMng::SetFlashlight(const std::string &cameraId, bool &isEnable)
{
    HW_HAL_UNUSED(cameraId);
    HW_HAL_UNUSED(isEnable);
    return OK;
}
} // namespace Camera
} // namespace OHOS