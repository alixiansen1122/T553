/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_VIEW_BITMAP_H
#define UI_TEST_VIEW_BITMAP_H

#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestViewBitmap : public UITest {
public:
    UITestViewBitmap() {}
    ~UITestViewBitmap() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void UIKitBitmapTestGetViewBitmap001();
    void UIKitBitmapTestGetScreenBitmap001();
private:
    UIScrollView* container_ = nullptr;
    UIImageView* viewBitmap_ = nullptr;
    UIImageView* screenBitmap_ = nullptr;
    UIView::OnClickListener* viewBitmapListener_ = nullptr;
    UIView::OnClickListener* screenBitmapListener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_VIEW_BITMAP_H
