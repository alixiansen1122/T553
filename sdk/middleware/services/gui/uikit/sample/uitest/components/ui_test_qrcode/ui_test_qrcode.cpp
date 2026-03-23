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

#include "ui_test_qrcode.h"
#include "common/screen.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t TITLE_HEIGHT = 29;
const int16_t QRCODE_X = 50;
const int16_t QRCODE_Y = 30;
const int16_t QRCODE_WIDTH = 250;
const int16_t QRCODE_HEIGHT = 250;
const int16_t GROUP_HEIGHT_100 = 100;
const int16_t GROUP_HEIGHT_300 = 300;
const int16_t LAYOUT_OFFSET = 10;
const int16_t CONTAINER_Y = 100;
} // namespace

void UITestQrcode::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->SetPosition(0, CONTAINER_Y);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - CONTAINER_Y);
    }
}

void UITestQrcode::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestQrcode::GetTestView()
{
    TestQrcode001();
    TestQrcode002();
    TestQrcode003();
    TestQrcode004();
    TestQrcode005();
    TestQrcode006();
    TestQrcode007();
    TestQrcode008();
    return container_;
}

UIViewGroup* UITestQrcode::CreateTestCaseGroup(const char* title) const
{
    UIViewGroup* group = new UIViewGroup();
    group->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group->SetPosition(0, 0);
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    group->Add(titleLabel);
    return group;
}

void UITestQrcode::TestQrcode001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 60 * 60");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_100);
        group->SetViewId("TestQrcode001");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, 60, 60); // 60: width and height
        const char* str = "Hello\n HarmonyOS Lite GUI";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
    }
}
void UITestQrcode::TestQrcode002()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" qrcode 120 * 250");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode002");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y);
        const char* str = "Hello\n HarmonyOS Lite GUI";
        qrcode->SetQrcodeInfo(str);
        qrcode->SetWidth(120); // 120: width
        qrcode->SetHeight(250); // 250: height
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode001", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode003()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 200, MAX_LENGTH");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode003");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y);
        const char* str =
            "Hello\n GUIddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
        ColorType backgroundColor = Color::Green();
        ColorType qrColor = Color::Blue();
        qrcode->SetQrcodeInfo(str, backgroundColor, qrColor);
        qrcode->SetWidth(250);  // 250: width
        qrcode->SetHeight(200); // 200: height
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode002", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode004()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("Error: qrcode 50 * 50, width < min qrcode size");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_100);
        group->SetViewId("TestQrcode004");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, 50, 50); // 50: width and height
        const char* str =
            "Hello\n GUIddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            dddddddddddddddddddddddddddddddddddddddddddddddddddddd";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode003", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode005()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("Error: qrcode 250 * 250, len(val) > MAX_LENGTH");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode005");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, QRCODE_WIDTH, QRCODE_HEIGHT);
        const char* str =
            "Hello\n GUIddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode004", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode006()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("Error: qrcode 250 * 250, str = \"\" ");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode006");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, QRCODE_WIDTH, QRCODE_HEIGHT);
        const char* str = "";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode005", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode007()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, str = \" \"");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode007");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, QRCODE_WIDTH, QRCODE_HEIGHT);
        const char* str = " ";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode006", LAYOUT_OFFSET);
    }
}

void UITestQrcode::TestQrcode008()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("Error: qrcode 250 * 250, str = nullptr");
        group->Resize(Screen::GetInstance().GetWidth(), GROUP_HEIGHT_300);
        group->SetViewId("TestQrcode008");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(QRCODE_X, QRCODE_Y, QRCODE_WIDTH, QRCODE_HEIGHT);
        const char* str = nullptr;
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("TestQrcode007", LAYOUT_OFFSET);
    }
}
} // namespace OHOS