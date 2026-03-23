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

#include "ui_test_font_global.h"
#if (ENABLE_FONT_VECTOR_GLOBAL && ENABLE_FONT_GLOBAL_TEST)
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // _WIN32
#include "common/screen.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "font/font_global_manager.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
#include "ui_resource_string.h"

namespace OHOS {
static const char* BIN_LANG_EN = RES_PATH"font/en.lang";
static const char* BIN_LANG_ZH = RES_PATH"font/zh.lang";

void UITestFontGlobal::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - LABEL_HEIGHT);
        positionX_ = 50; // 50: init position x
        positionY_ = 5;  // 5: init position y
        const char* resFiles[] = {BIN_LANG_ZH, BIN_LANG_EN};
        FontGlobalManager::GetInstance()->RegisterLanguageInfo(resFiles, (sizeof(resFiles) / sizeof(char*)), 0);
    }
}

void UITestFontGlobal::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    wildcardLabel_ = nullptr;
    FontGlobalManager::GetInstance()->UnRegisterLanguageInfo();
}

const UIView* UITestFontGlobal::GetTestView()
{
    wildcardLabel_ = new UILabelExt();
    container_->Add(wildcardLabel_);
    wildcardLabel_->SetPosition(250, 5, 100, 60); // 250: X, 5: Y, 100: WIDTH, 60: HEIGHT
    char temp[30]; // 30: length
    const char* staticText = FontGlobalManager::GetInstance()->GetText(0);

    if (sprintf_s(temp, sizeof(temp), "%s %s", "Wildcard:", staticText) > 0) {
        wildcardLabel_->SetText(temp);
        GRAPHIC_LOGW("Add Wildcard example.");
    }

    TestUILableStaticText001();
    return container_;
}

void UITestFontGlobal::ResetWildcard()
{
    if (wildcardLabel_ == nullptr) {
        wildcardLabel_ = new UILabelExt();
        container_->Add(wildcardLabel_);
        wildcardLabel_->SetPosition(250, 5, 100, 60); // 250: X, 5: Y, 100: WIDTH, 60: HEIGHT
    }
    char temp[30];  // 30: length
    const char* staticText = FontGlobalManager::GetInstance()->GetText(0);

    if (sprintf_s(temp, sizeof(temp), "%s %s", "Wildcard:", staticText) > 0) {
        wildcardLabel_->SetText(temp);
        GRAPHIC_LOGW("Wildcard Text changed.");
    }
}

void UITestFontGlobal::AddTestUILables()
{
    constexpr int16_t Y_STEP = 50;
    constexpr int16_t BTTON_OFSET = 100;
    if (container_ == nullptr) {
        return;
    }
    for (int i = 0; i < STR_MAX_ID; i++) {
        UILabelExt* labelE = new UILabelExt();
        labelE->SetPosition(positionX_, positionY_);
        labelE->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        labelE->SetTextId(i);
        container_->Add(labelE);
        // add buttons
        UILabelButtonExt* buttonE = new UILabelButtonExt();
        buttonE->Resize(BUTTON_WIDHT1, BUTTON_HEIGHT1);
        buttonE->SetOnClickListener(this);
        buttonE->SetPosition(positionX_ + BTTON_OFSET, positionY_);
        buttonE->SetTextId(i);
        container_->Add(buttonE);
        positionY_ += Y_STEP;
    }
}

bool UITestFontGlobal::OnClick(UIView& view, const ClickEvent& event)
{
    static int32_t langId = 0;
    langId = (langId + 1) % FontGlobalManager::GetInstance()->GetTotalLangId();
    FontGlobalManager::GetInstance()->SetCurrentLangId(langId);

    ResetWildcard();

    return false;
}

void UITestFontGlobal::TestUILableStaticText001()
{
    if (container_ == nullptr) {
        return;
    }
    AddTestUILables();
}
} // namespace OHOS

#endif // ENABLE_FONT_VECTOR_GLOBAL