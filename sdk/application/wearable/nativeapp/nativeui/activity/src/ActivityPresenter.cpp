/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: WeekStep
 * Author:
 * Create: 2021-09-11
 */

#include <string>
#include "gfx_utils/graphic_types.h"
#include "components/ui_checkbox.h"
#include "UiConfig.h"
#include "activity/ActivityPresenter.h"
#include "NativeRegisterManager.h"

namespace OHOS {

REGIST_SLICE(VIEW_MAIN_ACTIVITY, MainActivityView, ActivityPresenter);
    
static ActivityPresenter *g_pActivityPresenter = nullptr;
ActivityPresenter::ActivityPresenter()
{
    g_pActivityPresenter = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityPresenter");
}

ActivityPresenter::~ActivityPresenter()
{
    g_pActivityPresenter = nullptr;
}

ActivityPresenter *ActivityPresenter::GetInstance()
{
    return g_pActivityPresenter;
}


bool ActivityPresenter::OnDrag(UIView& view, const DragEvent& event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityPresenter::OnDrag");
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        if (strcmp(view.GetViewId(), ACTIVITY_WEEK_VIEW) == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ActivityPresenter::OnDrag on ACTIVITY_WEEK_VIEW");
            uint16_t preSlice = NativeAbility::GetInstance().GetPreSliceId();
            NativeAbility::GetInstance().ChangeSlice(preSlice);
        }
    }
    return true;
}
}