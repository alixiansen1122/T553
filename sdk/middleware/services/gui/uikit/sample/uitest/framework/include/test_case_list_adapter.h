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

#ifndef TEST_CASE_LIST_ADAPTER_H
#define TEST_CASE_LIST_ADAPTER_H

#include "components/abstract_adapter.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class TestCaseListAdapter : public AbstractAdapter {
public:
    TestCaseListAdapter(const TestCaseListAdapter &) = delete;
    TestCaseListAdapter &operator=(const TestCaseListAdapter &) = delete;
    TestCaseListAdapter() {}
    ~TestCaseListAdapter() override
    {
        if (listener_ != nullptr) {
            delete listener_;
            listener_ = nullptr;
        }
    }
    UIView* GetView(UIView* inView, int16_t index) override;
    int16_t GetItemWidthWithMargin(int16_t index) override;
    int16_t GetItemHeightWithMargin(int16_t index) override;
    uint16_t GetCount() override;
private:
    UIView::OnClickListener* listener_{nullptr};
};
} // namespace OHOS
#endif // TEST_CASE_LIST_ADAPTER_H