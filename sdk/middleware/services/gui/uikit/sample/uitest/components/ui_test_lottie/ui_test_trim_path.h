/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie Test
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_TRIM_PATH_H
#define UI_TEST_TRIM_PATH_H

#include "ui_test.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label.h"
#include "components/ui_slider.h"
#include "components/ui_checkbox.h"
#include "lottie/lott_trim_path.h"

namespace OHOS {
class UITestTrimPath : public UITest, public UISlider::UISliderEventListener, public UICheckBox::OnChangeListener {
public:
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void OnChange(int32_t value) override;
    bool OnChange(UICheckBox::UICheckBoxState state) override;

private:
    void InitPath();
    void TrimPath();
    void InitSlider(UISlider*& slider, int16_t x, int16_t y);
    void InitLabel(UILabel*& label, const Rect& position, const char* str);
    UILabel* startLabel_{nullptr};
    UISlider* startSlider_{nullptr};
    float startValue_{0.0f};

    UILabel* endLabel_{nullptr};
    UISlider* endSlider_{nullptr};
    float endValue_{1.0f};

    UILabel* offsetLabel_{nullptr};
    UISlider* offsetSlider_{nullptr};
    float offsetValue_{0.0f};

    UILabel* parallelLabel_{nullptr};
    UICheckBox* parallelCheckBox_{nullptr};
    bool isParallel_{false};

    UIViewGroup* container_ = nullptr;
    PaintExt paint_{};
    LottTrimPath lottTrimPath_{};
    Path path_{0};
    UICanvasExt* canvas_{nullptr};
};
}
#endif