/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraModel
 * Created: 2025-06-05
 */

#include <sys/time.h>
#include "ohos_timer.h"
#include "wearable_log.h"
#include "camera/CameraView.h"
#include "camera/CameraModel.h"

namespace OHOS {
CameraModel::CameraModel() {}

CameraModel::~CameraModel() {}

CameraModel &CameraModel::GetInstance()
{
    static CameraModel instance;
    return instance;
}
}
