/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraModel
 * Created: 2025-06-05
 */

#ifndef CAMERA_MODEL_H
#define CAMERA_MODEL_H

#include <string>
#include <list>
#include "graphic_timer.h"

namespace OHOS {
class CameraModel {
public:
    static CameraModel &GetInstance(void);
private:
    CameraModel();
    CameraModel(const CameraModel &);
    CameraModel &operator=(const CameraModel &);
    virtual ~CameraModel();
};
}

#endif // CAMERA_MODEL_H