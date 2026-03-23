/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistPresenter.cpp
 * Author:
 * Create: 2021-09-18
 */

#include "applist/ApplistPresenter.h"
#include "NativeRegisterManager.h"
#include "wearable_log.h"

namespace OHOS {
REGIST_SLICE(VIEW_APPLIST, ApplistView, ApplistPresenter);

ApplistPresenter::ApplistPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ApplistPresenter::ApplistPresenter()");
    model = &ApplistModel::GetInstance();
    model->Init();
    model->SetPresenter(this);
}

ApplistPresenter::~ApplistPresenter()
{
    model->SetPresenter(nullptr);
}

void ApplistPresenter::OnStart()
{
    if (view_ == nullptr) {
        return;
    }
    const AppItem *itemInfo = model->GetApplistItems();
    for (uint8 i = 0; i < model->GetAppListNum(); i++) {
        view_->AddAppItemToList(itemInfo[i]);
    }
    view_->RefreshAppList();
}

void ApplistPresenter::OnResume()
{
    view_->OnResume();
}

ApplistView* ApplistPresenter::GetAppListView()
{
    return view_;
}
}
