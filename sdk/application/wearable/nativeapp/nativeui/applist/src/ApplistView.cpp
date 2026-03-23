/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ApplistView.cpp
 * Author:
 * Create: 2021-09-18
 */

#include "applist/ApplistView.h"
#include "applist/ApplistPresenter.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"

namespace OHOS {
void ApplistView::OnStart()
{
    if (desktopFragment_ != nullptr) {
        OnStop();
    }
    DesktopStyle style = SettingDesktopModel::GetInstance().GetDesktopStyle();
    desktopFragment_ = DesktopFactory::GetInstance().CreateDesktopStyle(style);
    desktopFragment_->SetPosition(0, 0, GetRootContainer()->GetWidth(), GetRootContainer()->GetHeight());
    desktopFragment_->CreateView();
    AddViewToRootContainer(desktopFragment_->GetFragmentView());
}

void ApplistView::OnStop()
{
    if (desktopFragment_ == nullptr) {
        return;
    }
    desktopFragment_->DestroyView();
    delete desktopFragment_;
    desktopFragment_ = nullptr;
}

void ApplistView::OnResume()
{
    if (desktopFragment_ == nullptr) {
        return;
    }
    desktopFragment_->ResumeView();
}

void ApplistView::AddAppItemToList(const AppItem &item)
{
    if (desktopFragment_ == nullptr) {
        return;
    }
    desktopFragment_->AddAppItemToList(item);
}

void ApplistView::ClearAppItemToList()
{
    if (desktopFragment_ == nullptr) {
        return;
    }
    desktopFragment_->ClearAppItemToList();
}

void ApplistView::RefreshAppList()
{
    if (desktopFragment_ == nullptr) {
        return;
    }
    desktopFragment_->RefreshAppList();
}
}
