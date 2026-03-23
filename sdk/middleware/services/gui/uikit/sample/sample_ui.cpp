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

#include "sample_ui.h"
#include "debug_print.h"
#include "ui_test.h"
#include "ui_test_app.h"
#include "ui_test_group.h"
#include "test_case_list_adapter.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "graphic_config.h"
#include "graphic_service.h"
#include "common/graphic_startup.h"
#include "gfx_utils/mem_check.h"

int SampleUi()
{
    GraphicService::GetInstance()->PostGraphicEvent([] {
        OHOS::GraphicStartUp::InitFontEngine(MEM_POOL_UI_FONT, MEM_POOL_UI_FONT_SIZE, OHOS::VECTOR_FONT_DIR,
                                         DEFAULT_VECTOR_FONT_FILENAME);
        OHOS::UITestApp::GetInstance()->Start();
    });
    return 0;
}

class BackButton : public OHOS::UIButton {
public:
    static BackButton* GetInstance()
    {
        static BackButton instance;
        return &instance;
    }

    bool OnClickEvent(const OHOS::ClickEvent &event) override
    {
        OHOS::RootView::GetInstance()->RemoveAll();
        OHOS::UITest* uiTest = OHOS::UITestGroup::GetInstance()->GetTestCase(caseId.c_str());
        if (uiTest != nullptr) {
            uiTest->TearDown();
            PRINT("[Memory Info After TearDown]\n");
            OHOS::MemCheck::GetInstance()->DumpMemInfo();
        }
        OHOS::UITestApp::GetInstance()->Init();
        return true;
    }
    
    void SetCaseId(const char* s)
    {
        caseId = s;
    }

private:
    BackButton() : OHOS::UIButton()
    {
        opaScale_ = 0;
    }

    std::string caseId;
};

int StartCaseUi(const char* testCaseId)
{
    BackButton::GetInstance()->SetPosition(0,
        (OHOS::Screen::GetInstance().GetHeight() - 50) / 2, 50, 50); // 2 : half ;  50 : offset
    
    GraphicService::GetInstance()->PostGraphicEvent([testCaseId] {
        OHOS::UIViewGroup* rootView = OHOS::RootView::GetInstance();

        OHOS::UITest* uiTest = OHOS::UITestGroup::GetInstance()->GetTestCase(testCaseId);
        if (uiTest == nullptr) {
            PRINT("[Auto Test]: test case [%s] not find \n", testCaseId);
            return;
        }
        PRINT("[Memory Info Before Setup]\n");
        OHOS::MemCheck::GetInstance()->DumpMemInfo();
        uiTest->SetUp();
        OHOS::UIView* tempView = const_cast<OHOS::UIView*>(uiTest->GetTestView());
        if (tempView != nullptr) {
            rootView->RemoveAll();
            tempView->SetTouchable(true);
            tempView->SetViewId(testCaseId);
            tempView->SetPosition(tempView->GetX(), tempView->GetY());
            rootView->Add(tempView);
            BackButton::GetInstance()->SetCaseId(testCaseId);
            rootView->Add(BackButton::GetInstance());
            rootView->Invalidate();
            PRINT("[Auto Test]: test case view [%s] started \n", testCaseId);
        } else {
            PRINT("[Auto Test]: error, case view is null \n", testCaseId);
            return;
        }
        return;
    });
    return 0;
}
