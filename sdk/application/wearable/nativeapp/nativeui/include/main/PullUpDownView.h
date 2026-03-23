/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: WeekStep
 * Author:
 * Create: 2021-09-11
 */

#ifndef PULL_UP_DOWN_VIEW_H
#define PULL_UP_DOWN_VIEW_H

#include "graphic_types.h"
#include "wearable_log.h"
#include "components/ui_card_page.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class PullUpDownView : public UICardPage {
public:
    PullUpDownView();
    ~PullUpDownView() override;
    static PullUpDownView *GetInstance(void);
    bool InitView();
    void PreLoad(void) override;
    void UpdateBlueToothStatus(int status);
private:
    UIImageView *dnotDisturb{nullptr};
    UIImageView *briScreen{nullptr};
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
    bool viewiInitStatus{false};

    void InitSubViewImg(void);
    void InitViewImg(void);
    void InitViewLable(void);
    void InitViewInfo(void);

    UIView::OnClickListener *clickListener{nullptr};
};
}
#endif
