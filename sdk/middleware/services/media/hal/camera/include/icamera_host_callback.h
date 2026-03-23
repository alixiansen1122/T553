/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: icamera host callback
* Author: Media Software Group
* Create: 2022-12-20
*/

/**
 * @file icamera_host_callback.h
 *
 * @brief Declares callbacks for status changes of cameras and flashes. The caller needs to implement the callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_CAMERA_HOST_CALLBACK_SERVER_H
#define HDI_CAMERA_HOST_CALLBACK_SERVER_H

#include <list>
#include <map>
#include <vector>
#include "types.h"

namespace OHOS::Camera {
enum {
    CMD_CAMERA_HOST_CALLBACK_ON_STATUS = 0,
    CMD_CAMERA_HOST_CALLBACK_ON_FLASHLIGHT_STATUS,
};

class ICameraHostCallback {
public:
    virtual ~ICameraHostCallback() {}

public:
    /**
     * @brief Called when the camera status changes to report the latest status.
     *
     * @param cameraId Indicates the ID of the camera whose status changes.
     * @param status Indicates the latest status of the camera.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCameraStatus(const std::string &cameraId, CameraStatus status) = 0;

    /**
     * @brief Called when the flash status changes to report the latest status.
     *
     * @param cameraId Indicates the ID of the camera to which the flash whose status changes is bound.
     * @param status Indicates the latest status of the flash.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status) = 0;
};
}
#endif // HDI_CAMERA_HOST_CALLBACK_SERVER_H
