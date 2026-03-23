/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_BITMAP_FONT_H
#define UI_TEST_BITMAP_FONT_H

#include "ui_test.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class UITestBitmapFont : public UITest {
public:
    UITestBitmapFont() {}
    ~UITestBitmapFont() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void TestBitmapDynamicFont();
    void TestBitmapStaticFont();
private:
    UIScrollView* container_ = nullptr;
};
}
#endif