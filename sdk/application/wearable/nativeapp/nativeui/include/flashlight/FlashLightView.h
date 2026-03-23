/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightView
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_VIEW_H
#define FLASHLIGHT_VIEW_H
#include "View.h"
namespace OHOS {

enum FlashLightAllView {
    FLASH_LIGHT_VIEW = 1,      // 手电筒主界面
    FLASH_LIGHT_PLAY_VIEW,     // 手电筒显示播放
    FLASH_LIGHT_SET_VIEW,      // 手电筒设置
    FLASH_LIGHT_COLOR_VIEW,    // 手电筒颜色
    FLASH_LIGHT_BLINK_VIEW,    // 手电筒闪烁
    FLASH_LIGHT_RECOVER_VIEW,  // 手电筒默认
    FLASH_LIGHT_ALL_VIEW,
};

class FlashLightPresenter;
class FlashLightView : public View<FlashLightPresenter> {};
}
#endif  // FLASHLIGHT_VIEW_H
