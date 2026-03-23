/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingToggleButton
 * Create: 2025-06-01
 */
#ifndef SETTING_TOGGLE_BUTTON_H
#define SETTING_TOGGLE_BUTTON_H

#include "components/ui_toggle_button.h"

namespace OHOS {
/*
 * UIToggleButton的宽高比绑定, SettingToggleButton通过重写UIToggleButton的方法提供松散的宽高比
 */
class SettingToggleButton : public UIToggleButton {
public:
    SettingToggleButton();
    virtual ~SettingToggleButton() = default;
    void SetTrackPadding(int16_t padding);
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
protected:
    void CalculateSize() override;
#if DEFAULT_ANIMATION
    void Callback(UIView* view) override;
    void OnStop(UIView& view) override;
#endif
private:
    uint16_t trackCorner_ = 0;          // 轨道圆角半径
    int16_t  trackPadding_ = 0;         // 轨道内边距
    uint16_t thumbRadius_ = 0;          // 滑块半径
    Point leftCenter_ = {0, 0};
    Point rightCenter_ = {0, 0};
    Point currentCenter_ = {0, 0};
    ColorType bgColor_ = Color::White();
    Rect rectMid_ = {0, 0, 0, 0};
};
}
#endif
