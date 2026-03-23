/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */


#ifndef UI_TEST_VECTOR_FONT_H
#define UI_TEST_VECTOR_FONT_H

#include "graphic_config.h"
#if ENABLE_VECTOR_FONT
#include "ui_test.h"
#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include <string>

namespace OHOS {
class UITestVectorFont : public UITest {
public:
    UITestVectorFont() {}
    ~UITestVectorFont() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void Font_FontEngine_Vector_Test_CH_Lang_001();
    void Font_FontEngine_Vector_Test_CH_Lang_002();
    void Font_FontEngine_Vector_Test_JA_Lang_001();
    void Font_FontEngine_Vector_Test_JA_Lang_002();
    void Font_FontEngine_Vector_Test_KR_Lang_001();
    void Font_FontEngine_Vector_Test_VI_Lang_001();
#if ENABLE_MULTI_FONT
    /**
     * @brief Test JA language
     */
    void Font_FontEngine_MultiLanguage_Test_JA_Lang_001();

    /**
     * @brief Test UnRegister JA language
     */
    void Font_FontEngine_MultiLanguage_Test_JA_Lang_002();

    /**
     * @brief Test KR language
     */
    void Font_FontEngine_MultiLanguage_Test_KR_Lang_001();

    /**
     * @brief Test VI language
     */
    void Font_FontEngine_MultiLanguage_Test_VI_Lang_001();

    /**
     * @brief Test CS language
     */
    void Font_FontEngine_MultiLanguage_Test_CS_Lang_001();

    /**
     * @brief Test EL language
     */
    void Font_FontEngine_MultiLanguage_Test_EL_Lang_001();

    /**
     * @brief Test RO language
     */
    void Font_FontEngine_MultiLanguage_Test_RO_Lang_001();
#endif
    UIScrollView* container_ = nullptr;
    void InnerTestTitle(const char* title);
};
} // namespace OHOS
#endif // ENABLE_VECTOR_FONT
#endif // UI_TEST_VECTOR_FONT_H
