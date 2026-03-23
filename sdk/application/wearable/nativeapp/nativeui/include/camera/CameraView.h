/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CameraView
 * Created: 2025-06-05
 */

#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H

#include "View.h"
namespace OHOS {
enum CAMERA_PAGES {
    CAMERA_MAIN_PAGE = 1, // 0 is invalid
};

class CameraPresenter;
class CameraView : public View<CameraPresenter> {
};
}
#endif // CAMERA_VIEW_H
