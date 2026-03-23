/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "uisample/UISamplePresenter.h"
#include "uisample/UISampleView.h"
#include "uisample/UISampleModel.h"
#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"

namespace OHOS {

REGIST_MENU(VIEW_UISAMPLE, UISampleView, UISamplePresenter, PNG_APPLIST_UI_SAMPLE_IMAGE, PNG_APPLIST_DEFAULT_IMG, "示例");

    UISamplePresenter::UISamplePresenter()
    {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UISamplePresenter::UISamplePresenter");
        uisampleModel_ = &UISampleModel::GetInstance();
    }

    UISamplePresenter::~UISamplePresenter() {}

    void UISamplePresenter::OnResume(void)
    {
        SendMsgToUIService(UI_ADD_SLEEP_VETO, 0, nullptr, 0);
    }

    void UISamplePresenter::OnPause(void)
    {
        SendMsgToUIService(UI_RM_SLEEP_VETO, 0, nullptr, 0);
        return;
    }
}
