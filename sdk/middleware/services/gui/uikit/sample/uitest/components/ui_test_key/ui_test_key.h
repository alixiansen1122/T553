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

#ifndef UI_TEST_KEY_H
#define UI_TEST_KEY_H

#include "common/key_code.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/input_device.h"
#include "events/key_event.h"
#include "ui_test.h"

namespace OHOS {
class TestKeyInputListener : public RootView::OnKeyActListener {
public:
    explicit TestKeyInputListener() {}
    virtual ~TestKeyInputListener() {}
    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        UILabel* id = dynamic_cast<UILabel*>(view.GetChildById("keyId"));
        if (id != nullptr) {
            uint16_t keyID = event.GetKeyId();
            if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {
                id->SetText("ZLITE_KEY_POWER");
            } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) {
                id->SetText("ZLITE_KEY_FUNC");
            }
        }

        UILabel* label = dynamic_cast<UILabel*>(view.GetChildById("keyState"));
        if (label != nullptr) {
            uint16_t state = event.GetState();
            if (state == InputDevice::STATE_RELEASE) {
                label->SetText("Release!");
            } else if (state == InputDevice::STATE_PRESS) {
                label->SetText("Press!");
            } else if (state == InputDevice::STATE_LONG_PRESS) {
                label->SetText("Long Press!");
            }
        }

        UILabel* style = dynamic_cast<UILabel*>(view.GetChildById("style"));
        if (style != nullptr) {
            uint16_t state = event.GetState();
            if (state == InputDevice::STATE_RELEASE) {
                style->SetStyle(STYLE_BORDER_RADIUS, 10); // 10: radius
                style->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
                style->SetStyle(STYLE_BORDER_OPA, 0); // 0: opa
                style->SetStyle(STYLE_BORDER_WIDTH, 20); // 20: width
            } else if (state == InputDevice::STATE_PRESS) {
                style->SetStyle(STYLE_BORDER_RADIUS, 20); // 20: radius
                style->SetStyle(STYLE_BORDER_COLOR, Color::Yellow().full);
                style->SetStyle(STYLE_BORDER_OPA, 255); // 255: width
                style->SetStyle(STYLE_BORDER_WIDTH, 10); // 10: width
            }
        }
        return true;
    }
};

class UITestKey : public UITest {
public:
    UITestKey() {}
    ~UITestKey() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
private:
    UIScrollView* container_ = nullptr;
    UILabel* id_ = nullptr;
    UILabel* state_ = nullptr;
    UILabel* style_ = nullptr;
    TestKeyInputListener* keyActListener_ = nullptr;
    UILabelButton* blur_ = nullptr;
    UIView::OnClickListener* blurListener_ = nullptr;

    void TestGaussianBlur();
};
} // namespace OHOS
#endif // UI_TEST_KEY_H