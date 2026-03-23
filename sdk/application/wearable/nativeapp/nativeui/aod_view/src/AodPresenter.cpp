/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodPresenter
 * Author:
 * Create: 2024-10-30
 */

#include "aod_view/AodPresenter.h"
#include "NativeRegisterManager.h"
#include "main/dial/DialViewGroup.h"

namespace OHOS {

REGIST_SLICE(VIEW_AOD, AodView, AodPresenter);

void AodPresenter::Callback()
{
    if (view_ != nullptr) {
        view_->Update();
    }
}

void AodPresenter::OnResume()
{
    Init();
}

void AodPresenter::OnPause()
{
    Deinit();
}
} // OHOS
