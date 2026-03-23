/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightModel
 * Create: 2025-04-24
 */

#ifndef FLASHLIGHT_MODEL_H
#define FLASHLIGHT_MODEL_H

#include <cstdint>
#include <map>
#include <string>
#include <sys/time.h>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "FlashLightPresenter.h"
#include "ui_resource_flashlight.h"
#ifndef _WIN32
#include "time64.h"
#endif
namespace OHOS {
enum FlashLightBlinkFrequency { SLOW_BLINK = 0, FAST_BLINK, NOT_BLINK };

enum FlashLightColorAttribute {
    BLACK_COLOR = 0,
    WHITE_COLOR = 1,
    RED_COLOR = 2,
    ORANGE_COLOR = 3,
    YELLOW_COLOR = 4,
    GREEN_COLOR = 5,
    LIGHT_BLUE_COLOR = 6,
    DARK_BLUE_COLOR = 7,
    PURPLE_COLOR = 8,
    MULTI_COLOR = 9,
};

class FlashLightModel {
public:
    static FlashLightModel &GetInstance(void);
    // 设置手电筒颜色;
    FlashLightColorAttribute GetViewColorId();
    void SetViewColorId(FlashLightColorAttribute color);
    // 设置手电筒闪烁频率;
    FlashLightBlinkFrequency GetBlinkFrequency();
    void SetBlinkFrequency(FlashLightBlinkFrequency blinkFrequency);
    // 恢复默认参数;
    void SetDefault();
    // 获取当前时间;
    uint32_t GetSysCurrentTime(std::string &currentTime);

private:
    FlashLightModel(FlashLightColorAttribute color = FlashLightColorAttribute::WHITE_COLOR,
                    FlashLightBlinkFrequency blinkFrequency = FlashLightBlinkFrequency::NOT_BLINK);
    FlashLightModel(const FlashLightModel &);
    FlashLightModel &operator=(const FlashLightModel &);
    virtual ~FlashLightModel();

private:
#ifdef _WIN32
    struct timeval sysTime_ {
        0
    };
#else
    struct timeval64 sysTime_ {
        0
    };
#endif
    FlashLightColorAttribute color_;
    FlashLightBlinkFrequency blinkFrequency_;
};
}

#endif  // FLASHLIGHT_MODEL_H