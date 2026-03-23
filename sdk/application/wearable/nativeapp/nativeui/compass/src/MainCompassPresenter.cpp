/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainCompassPresenter.cpp
 * Author:
 * Create: 2021-12-21
 */
#include "compass/MainCompassPresenter.h"
#include "compass/MainCompassView.h"
#include "NativeRegisterManager.h"

namespace OHOS {

REGIST_MENU(VIEW_COMPASS, MainCompassView, MainCompassPresenter, PNG_APPLIST_COMPASS_IMAGE,
    PNG_APPLIST_DEFAULT_IMG, "指南针");

bool MainCompassPresenter::OnDrag(UIView& view, const DragEvent& event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

void MainCompassPresenter::OnResume()
{
    Init();
    if ((view_ != nullptr) && (view_->compassFragment_ != nullptr)) {
        view_->compassFragment_->ResumeView();
    }
}

void MainCompassPresenter::OnPause()
{
    Deinit();
    if ((view_ != nullptr) && (view_->compassFragment_ != nullptr)) {
        view_->compassFragment_->PauseView();
    }
}

void MainCompassPresenter::Callback()
{
    if ((view_ != nullptr) && (view_->compassFragment_ != nullptr)) {
        view_->compassFragment_->Update();
    }
}
}
