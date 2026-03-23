/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_TEST_KALEIDOSCOPE_H
#define UI_TEST_KALEIDOSCOPE_H

#include "animator/animator.h"
#include "components/kaleidoscope_utils.h"
#include "components/ui_image_view.h"
#include "components/ui_kaleidoscope_view.h"
#include "components/ui_label_button.h"
#include "ui_test.h"

namespace OHOS {
class UITestKaleidoscope : public UITest, public UIView::OnClickListener {
public:
    UITestKaleidoscope() {}
    ~UITestKaleidoscope() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    class KaleidoscopeAnimator : public Animator, public AnimatorCallback {
    public:
        KaleidoscopeAnimator(KaleidoscopeUtils* kaleidoscopeUtils, UIImageView* img)
            : Animator(this, img, 0, true), kaleidoscopeUtils_(kaleidoscopeUtils) {}
        virtual ~KaleidoscopeAnimator() {}
        void Callback(UIView* view) override;

    private:
        KaleidoscopeUtils* kaleidoscopeUtils_;
    };

    void InitKaleidoscopeView();
    void InitKaleidoscopeImg();

    UIViewGroup* container_ = nullptr;
    ImageInfo* imgInfo_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;

    UIKaleidoscopeView* kaleidoscope_ = nullptr;

    UIImageView* img_ = nullptr;
    KaleidoscopeUtils* kaleidoscopeUtils_ = nullptr;
    KaleidoscopeAnimator* utilsAnimator_ = nullptr;
};
}
#endif