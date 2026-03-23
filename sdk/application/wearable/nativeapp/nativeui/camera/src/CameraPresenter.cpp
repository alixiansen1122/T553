/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraPresenter
 * Created: 2025-06-05
 */

#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"
#include "camera/CameraView.h"
#include "camera/CameraModel.h"
#include "camera/CameraPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_CAMERA, CameraView, CameraPresenter, CAMERA_ICON, CAMERA_ICON, "相机");
}
