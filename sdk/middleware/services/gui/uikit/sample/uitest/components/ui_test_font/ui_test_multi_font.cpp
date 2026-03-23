/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ui_test_multi_font.h"
#include "components/ui_label_ext.h"
#include "components/ui_label_button_ext.h"
#include "font/ui_multi_font_manager.h"
#include "font/ui_font.h"

#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
namespace OHOS {
static constexpr uint16_t WIDTH = 250;
static constexpr uint16_t HEIGHT = 100;
static constexpr uint16_t GAP_50 = 50;
static constexpr uint16_t GAP_100 = 100;

static char* g_fontFiles[3] = {"HarmonyOS_Sans_SC_Regular.ttf", "HarmonyOS_Sans_Light_Italic.ttf",
    "HarmonyOS_Sans_Naskh_Arabic_Light.ttf"};
static uint8_t g_fontId[3] = {0};
static uint8_t g_isShaping[3] = {0, 0, 1};

void UITestMultiFont::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
    labelX_ = GAP_100;
    labelY_ = GAP_50;
}

void UITestMultiFont::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    UIMultiFontManager::GetInstance()->ClearSearchFontList();
}

const UIView* UITestMultiFont::GetTestView()
{
    RegisterFont();
    TestArabic();
    TestChineseAndArabic();
    TestArabicAndChinese();
    TestItalian();
    TestChineseAndItalian();
    return container_;
}

void UITestMultiFont::RegisterFont()
{
    // The first font is registered in GraphicStartup.
    for (uint8_t i = 1; i < 3; i++) { // 3 : size
        g_fontId[i] = UIFont::GetInstance()->RegisterFontInfo(g_fontFiles[i], g_isShaping[i]);
        if (g_fontId[i] == INVALID_UCHAR_ID) {
            GRAPHIC_LOGW("Set TTF(%s) failed.", g_fontFiles[i]);
        }
    }
}

void UITestMultiFont::TestArabic()
{
    uint8_t searchList[] = {g_fontId[0], g_fontId[1]}; // 0, 1: g_fontId index
    UIMultiFontManager::GetInstance()->SetSearchFontList(g_fontId[2], searchList, sizeof(searchList)); // 2: index
    UILabelExt* label = new UILabelExt();
    label->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label->SetText("EN, CN, and AR. 英语中文阿拉伯语。الله‎");
    container_->Add(label);
    labelY_ += HEIGHT + GAP_50;

    UILabelExt* label1 = new UILabelExt();
    label1->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label1->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label1->SetText("EN, CN, and AR. 英语中文阿拉伯语。الله‎");
    label1->SetDirect(TEXT_DIRECT_RTL);
    container_->Add(label1);
    labelY_ += HEIGHT + GAP_50;
}

void UITestMultiFont::TestChineseAndArabic()
{
    uint8_t searchList[] = {g_fontId[1], g_fontId[2]}; // 1, 2: g_fontId index
    UIMultiFontManager::GetInstance()->SetSearchFontList(g_fontId[0], searchList, sizeof(searchList));
    UILabelExt* label = new UILabelExt();
    label->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label->SetText("中文和阿拉伯语混排。الأسانسير، علشان القطط ماتاكلش منها. وننساها، ونعود الى أوراقنا موصدين الباب بإحكام.");
    container_->Add(label);
    labelY_ += HEIGHT + GAP_50;

    UILabelExt* label1 = new UILabelExt();
    label1->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label1->SetText("中文和阿拉伯语混排。الأسانسير، علشان القطط ماتاكلش منها. وننساها، ونعود الى أوراقنا موصدين الباب بإحكام.");
    label1->SetDirect(TEXT_DIRECT_RTL);
    container_->Add(label1);
    labelY_ += HEIGHT + GAP_50;
}

void UITestMultiFont::TestArabicAndChinese()
{
    uint8_t searchList[] = {g_fontId[0], g_fontId[1]}; // 0, 1: g_fontId index
    UIMultiFontManager::GetInstance()->SetSearchFontList(g_fontId[2], searchList, sizeof(searchList)); // 2: index
    UILabelExt* label = new UILabelExt();
    label->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label->SetText("中文和阿拉伯语混排。"
        "الأسانسير، علشان القطط ماتاكلش منها. وننساها، ونعود الى أوراقنا موصدين الباب بإحكام.");
    container_->Add(label);
    labelY_ += HEIGHT + GAP_50;

    UILabelExt* label1 = new UILabelExt();
    label1->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label1->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label1->SetText("中文和阿拉伯语混排。"
        "الأسانسير، علشان القطط ماتاكلش منها. وننساها، ونعود الى أوراقنا موصدين الباب بإحكام.");
    label1->SetDirect(TEXT_DIRECT_RTL);
    container_->Add(label1);
    labelY_ += HEIGHT + GAP_50;

    UILabelExt* label2 = new UILabelExt();
    label2->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label2->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label2->SetText("1. تشبع الأكسجين في الدم (SpO2) هو النسبة المئوية للهيموغلوبين المؤكسد (HbO2)"
                    " في الدم، وهو عامل فسيولوجي مهم للحفاظ على وظائف الجسم الطبيعية.");
    label2->SetDirect(TEXT_DIRECT_RTL);
    container_->Add(label2);
    labelY_ += HEIGHT + GAP_50;

    UILabelExt* label3 = new UILabelExt();
    label3->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label3->SetFont("HarmonyOS_Sans_Naskh_Arabic_Light.ttf", DEFAULT_VECTOR_FONT_SIZE);
    label3->SetText("ABCتشبع الأكسجين في الدم (SpO2) هو النسبة المئوية للهيموغلوبين المؤكسد (HbO2)"
                    " في الدم، وهو عامل فسيولوجي مهم للحفاظ على وظائف الجسم الطبيعية.");
    label3->SetDirect(TEXT_DIRECT_RTL);
    container_->Add(label3);
    labelY_ += HEIGHT + GAP_50;
}

void UITestMultiFont::TestItalian()
{
    uint8_t searchList[] = {g_fontId[1], g_fontId[2]}; // 1, 2: g_fontId index
    UIMultiFontManager::GetInstance()->SetSearchFontList(g_fontId[0], searchList, sizeof(searchList));
    UILabelExt* label = new UILabelExt();
    label->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label->SetText("Vista in primavera, la città è circonfusa di una luce eterea.");
    container_->Add(label);
    labelY_ += HEIGHT + GAP_50;
}

void UITestMultiFont::TestChineseAndItalian()
{
    uint8_t searchList[] = {g_fontId[1], g_fontId[2]}; // 1, 2: g_fontId index
    UIMultiFontManager::GetInstance()->SetSearchFontList(g_fontId[0], searchList, sizeof(searchList));
    UILabelExt* label = new UILabelExt();
    label->SetPosition(labelX_, labelY_, WIDTH, HEIGHT);
    label->SetText("中文和意大利语混排。Vista in primavera, la città è circonfusa di una luce eterea.");
    container_->Add(label);
    labelY_ += HEIGHT + GAP_50;
}
}
#endif