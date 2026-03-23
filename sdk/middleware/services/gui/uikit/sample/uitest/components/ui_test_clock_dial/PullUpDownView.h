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

#ifndef PULL_UP_DOWN_VIEW_H
#define PULL_UP_DOWN_VIEW_H

#include "AppGroupView.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class PullUpDownView : public AppGroupView {
public:
    PullUpDownView();
    ~PullUpDownView() override;
    PullUpDownView(const PullUpDownView &) = delete;
    PullUpDownView &operator=(const PullUpDownView &) = delete;
    bool InitView() override;
    void UpdateBlueToothStatus(int status);
private:
    UIImageView *finPhone{nullptr};
    UIImageView *droAlarm{nullptr};
    UIImageView *droSetting{nullptr};
    UILabel *dnotDisturbLable{nullptr};
    UILabel *briScreenLable{nullptr};
    UILabel *finPhoneLable{nullptr};
    UILabel *droAlarmLable{nullptr};
    UILabel *droSettingLable{nullptr};
    UIImageView *bluetooth{nullptr};
    UIImageView *battery{nullptr};
    UILabel *batteryLable{nullptr};
    UILabel *dateLable{nullptr};

    void InitViewImg(void);
    void InitViewLable(void);
    void InitViewInfo(void);
    static PullUpDownView *GetInstance(void);

    // UIView::OnClickListener *clickListener{nullptr};
};
}
#endif