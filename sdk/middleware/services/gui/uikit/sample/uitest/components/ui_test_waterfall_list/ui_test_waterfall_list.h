/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_TEST_WATERFALL_LIST_H
#define UI_TEST_WATERFALL_LIST_H

#include "ui_test.h"
#include "gfx_utils/list.h"
#include "components/ui_waterfall_list.h"
#include "components/ui_button.h"

namespace OHOS {
class UITestWaterfallList : public UITest, public UIView::OnClickListener, public UIView::OnLongPressListener {
public:
    UITestWaterfallList() {}
    ~UITestWaterfallList() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView *GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnLongPress(UIView& view, const LongPressEvent& event) override;

private:
    void HandleClickEventForRectScreen(const UIView& view);
    void ConfigButtonAttr(UIView& view, int16_t x, int16_t y, int16_t width, int16_t height);
    void SetUpButtons(int16_t xEnd, int16_t yEnd, int16_t xMiddle, int16_t yMiddle);
    void SetUpChildren();
    UIWaterfallList* waterfallList_ = nullptr;
    int16_t horMargin_ = 0;
    int16_t verMargin_ = 0;
    int16_t borderRadius_ = 0;
    UIViewGroup* container_ = nullptr;
    uint16_t imgSize_ = 0;
    uint16_t imgDistance_ = 0;
    float imgScaleFactor_ = 0;

    bool isIncreaseImgSize_ = true;
    UIButton* modifyImgSizeButton_ = nullptr;
    bool isIncreaseImgDistance_ = true;
    UIButton* modifyImgDistanceButton_ = nullptr;
    bool isIncreaseScaleFactor_ = true;
    UIButton* modifyScaleFatorButton_ = nullptr;
    bool isIncreaseHorMargin_ = true;
    UIButton* modifyHorMarginButton_ = nullptr;
    bool isIncreaseVerMargin_ = true;
    UIButton* modifyVerMarginButton_ = nullptr;
    bool isIncreaseBorderRadius_ = true;
    UIButton* modifyBorderRadiusButton_ = nullptr;
    bool isImageMode_ = true;
    UIButton* modifyModeButton_ = nullptr;
};
}
#endif // UI_TEST_WATERFALL_LIST_H