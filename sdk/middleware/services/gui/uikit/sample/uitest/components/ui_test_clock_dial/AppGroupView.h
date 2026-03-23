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

#ifndef APP_GROUP_VIEW_H
#define APP_GROUP_VIEW_H

#include <string>
#include "components/ui_card_page.h"
#include "common/screen.h"

namespace OHOS {
class AppGroupView : public UICardPage {
public:
    AppGroupView()
    {
        SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
    ~AppGroupView() override {}
    virtual bool InitView()=0;
    bool initViewStatus{false};
};
} // namespace OHOS
#endif // APP_GROUP_VIEW_H
