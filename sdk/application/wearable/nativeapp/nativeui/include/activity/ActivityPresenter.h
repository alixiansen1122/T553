/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ActivityPresenter
 * Author:
 * Create: 2021-09-11
 */

#ifndef ACTIVITY_PRESENTER_H
#define ACTIVITY_PRESENTER_H

#include <ctime>
#include "Presenter.h"
#include "activity/MainActivityView.h"

namespace OHOS {
static constexpr const char *ACTIVITY_WEEK_VIEW = "activityweekview";

class MainActivityView;
class ActivityPresenter : public Presenter<MainActivityView>, public UIView::OnDragListener {
public:
    ActivityPresenter();
    ~ActivityPresenter();
    static ActivityPresenter *GetInstance();
    bool OnDrag(UIView& view, const DragEvent& event) override;
};
}
#endif