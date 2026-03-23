/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainActivityView
 * Author:
 * Create: 2021-09-11
 */

#ifndef MAIN_ACTIVITY_VIEW_H
#define MAIN_ACTIVITY_VIEW_H

#include <string>
#include "View.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "ChangeSliceListener.h"
#include "font/ui_font_bitmap.h"
#include "font/ui_font.h"
#include "graphic_config.h"
#include "activity/ActivityModel.h"
#include "activity/ActivityPresenter.h"
#include "activity/ActivityWeekView.h"

namespace OHOS {
class ActivityPresenter;
class MainActivityView : public View<ActivityPresenter> {
public:
    MainActivityView();
    ~MainActivityView() override;
    void OnStart() override;
    static MainActivityView *GetInstance(void);
private:
    ActivityWeekView *activityWeekView{nullptr};
};
} // namespace OHOS
#endif