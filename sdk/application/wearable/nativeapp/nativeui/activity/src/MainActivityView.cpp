/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainActivityView
 * Author:
 * Create: 2021-10-18
 */

#include "components/ui_label.h"
#include "activity/MainActivityView.h"

namespace OHOS {

static MainActivityView *g_pMainActivityView = nullptr;

MainActivityView::MainActivityView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainActivityView::MainActivityView");
    g_pMainActivityView = this;
}

MainActivityView::~MainActivityView()
{
    delete activityWeekView;
    activityWeekView = nullptr;
    g_pMainActivityView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MainActivityView::~MainActivityView");
}

MainActivityView *MainActivityView::GetInstance()
{
    return g_pMainActivityView;
}

void MainActivityView::OnStart()
{
    activityWeekView = new ActivityWeekView();
    if (activityWeekView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainActivityView:: OnStart activityWeekView null");
        return;
    }
    activityWeekView->PreLoad();
    AddViewToRootContainer(activityWeekView);
}
}
