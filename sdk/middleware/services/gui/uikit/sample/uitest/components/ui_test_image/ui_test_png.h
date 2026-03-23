/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Png Test
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef UI_TEST_PNG_H
#define UI_TEST_PNG_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "core/render_manager.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"

#if ENABLE_JPEG_AND_PNG
namespace OHOS {
class UITestPNG : public UITest {
public:
    UITestPNG() {}
    ~UITestPNG() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
private:
    void SetBackgroundColor();
    void TestPNG();
    void TestFillPNG();
    void TestContainPNG();
    void CreateLabel(const char* text);
    UIScrollView* container_ = nullptr;
};
}
#endif // ENABLE_JPEG_AND_PNG
#endif // UI_TEST_PNG_H