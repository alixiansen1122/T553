/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodView
 * Author:
 * Create: 2024-10-30
 */

#include "aod_view/AodView.h"
#include "aod_view/AodPresenter.h"
#include "aod_view/AodModel.h"
#include "common/graphic_utils.h"
#include "UIWatchDialFactory.h"
#include "main/DialBinParser.h"
#include "main/dial/DialViewGroup.h"
#include "clock/AmbientClockView.h"
#include "main/dial/DialModelTime.h"
#include "clock/GradientAodView.h"
#include "common/task.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {
AodView::~AodView()
{
    if (clockDialFragment_ != nullptr) {
        delete clockDialFragment_;
        clockDialFragment_ = nullptr;
        clockDial_ = nullptr;
    }

    if (clockDial_ != nullptr) {
        delete clockDial_;
        clockDial_ = nullptr;
    }
}

void AodView::OnStart()
{
    DialSetting &setting = MainModel::GetInstance().GetDialSetting();
    if (setting.dialFlag == 0) { // 在线表盘
        clockDial_ = InitAodDial(setting);
    } else {
        clockDial_ = InitAodOffDial(setting);
    }
    if (clockDial_ == nullptr) {
        clockDial_ = InitDefaultAod();
    }
    if (clockDial_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AodView::OnStart: Failed to generate the AOD watch face.\n");
        return;
    }
    DialViewGroup *dial = dynamic_cast<DialViewGroup*>(clockDial_);
    if (dial != nullptr) {
        dial->PreLoad();
        presenter_->SetPeriod(dial->GetPeriod());
    } else if (clockDialFragment_ != nullptr) {
#ifndef __WIN32
        PeriodicUpdateInterface *interface = dynamic_cast<PeriodicUpdateInterface*>(clockDialFragment_);
        if (interface != nullptr) {
            presenter_->SetPeriod(interface->GetPeriod());
        }
#endif
    }
    isLoaded_ = true;
    AddViewToRootContainer(clockDial_);
}

void AodView::OnStop()
{
    if (clockDial_ != nullptr) {
        DialViewGroup *dial = dynamic_cast<DialViewGroup*>(clockDial_);
        if (dial != nullptr) {
            dial->OnInactive();
            dial->UnLoad();
        }
        isLoaded_ = false;
    }
}

UIViewGroup *AodView::InitAodDial(DialSetting &setting)
{
    clockDialFragment_ = UIWatchDialFactory::GetInstance().CreateAodDial(setting.dialId);
    if (clockDialFragment_ != nullptr) {
        return dynamic_cast<UIViewGroup*>(clockDialFragment_->GetFragmentView());
    }
    return nullptr;
}

UIViewGroup *AodView::InitAodOffDial(DialSetting &setting)
{
    DialPreviewInfo preview;
    memset_s(&preview, sizeof(preview), 0, sizeof(preview));
    FILE *fp = fopen(setting.dialFullName.c_str(), "rb");
    if (fp == nullptr) {
        return nullptr;
    }
    bool ret = DialBinParserManager::GetInstance()->GetDialPreviewInfo(setting.dialFullName, fp, &preview, false);
    fclose(fp);
    if (!ret) {
        return nullptr;
    }
    if (preview.capability == DIAL_DISPLAY_CAPABILITY_AMBITIENT_ONLY ||
        preview.capability == DIAL_DISPLAY_CAPABILITY_AMBITIENT_NORMAL) {
        DialViewGroup *dial = new DialViewGroup;
        dial->SetDial(setting.dialFullName, DisplayState::AOD);
        return dial;
    }
    return nullptr;
}

UIViewGroup *AodView::InitDefaultAod()
{
    DialSetting setting;
    AodModel::GetInstance().GetAodDialSetting(setting);
    if (setting.dialFlag == 0) {
        return InitAodDial(setting);
    }
    return InitAodOffDial(setting);
}

void AodView::Update()
{
    if (clockDial_ == nullptr || !isLoaded_) {
        return;
    }
 
    DialViewGroup *dial = dynamic_cast<DialViewGroup*>(clockDial_);
    if (dial != nullptr) {
        DialModelTime::GetInstance()->UpdateTime();
        dial->UpdateViewsByPeriodicUpdateData();
        return;
    }

#ifndef __WIN32
    PeriodicUpdateInterface *interface = dynamic_cast<PeriodicUpdateInterface*>(clockDialFragment_);
    if (interface != nullptr) {
        interface->Update();
    }
#endif
}
} // OHOS
