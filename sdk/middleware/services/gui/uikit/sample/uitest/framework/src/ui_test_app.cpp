/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_test_app.h"
#include "common/screen.h"
#include "dfx/event_injector.h"
#include "ui_test.h"
#include "ui_test_group.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif
#include "gfx_utils/style.h"

namespace OHOS {
UITestApp* UITestApp::GetInstance()
{
    static UITestApp instance;
    return &instance;
}

void UITestApp::Start()
{
    UIViewGroup* rootView = RootView::GetInstance();
    rootView->SetPosition(0, 0);
    rootView->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rootView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    rootView->RemoveAll();
    Init();
}

void UITestApp::Init()
{
    if ((mainList_ == nullptr) && (adapter_ == nullptr)) {
        mainList_ = new UIList(UIList::VERTICAL);
        mainList_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        mainList_->SetThrowDrag(true);
        mainList_->SetReboundSize(100); // 100: rebound size
        mainList_->SetElastic(true);
        adapter_ = new TestCaseListAdapter();
        UITestGroup::GetInstance()->SetUpTestCase();
        mainList_->SetAdapter(adapter_);
    }
    UIViewGroup* rootView = RootView::GetInstance();
    rootView->Add(mainList_);
    rootView->Invalidate();
}

UITestApp::~UITestApp()
{
    if (mainList_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete mainList_;
        mainList_ = nullptr;
    }

    UITestGroup::GetInstance()->TearDownTestCase();
}
} // namespace OHOS
