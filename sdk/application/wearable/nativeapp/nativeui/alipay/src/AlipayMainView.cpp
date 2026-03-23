/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay app main view.
 * Author:
 * Create:
 */

#include "alipay/AlipayMainView.h"
#include "UiConfig.h"
#include "alipay_feature.h"

namespace OHOS {
#define ALIPAY_BIND_SUCCESS_RES_TIMER_LENGTH 3000
#define ALIPAY_BIND_FAILURE_RES_TIMER_LENGTH 8000

static AlipayMainView *g_alipayMainView = nullptr;

static const int16 LABEL_FIRST_X = 200;
static const int16 LABEL_FIRST_Y = 60;
static const int16 LABEL_FIRST_WIDTH = 150;
static const int16 LABEL_FIRST_HEIGHT = 45;
static const int16 LABEL_FIRST_FONT = 35;

AlipayMainView::AlipayMainView()
{
    g_alipayMainView = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::AlipayMainView");
}

AlipayMainView::~AlipayMainView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::~AlipayMainView");
    int16 numGroup;
    for (numGroup = ALIPAY_SET_ZERO; numGroup < ALIPAY_INTERFACES_MAX; numGroup++) {
        if (container[numGroup] != nullptr) {
            container[numGroup] = nullptr;
        }
    }
    DeleteViceInter();
    group = nullptr;
    g_alipayMainView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::~AlipayMainView");
}

AlipayMainView *AlipayMainView::GetInstance(void)
{
    return g_alipayMainView;
}

void AlipayMainView::OnStart()
{
    alipayInterfacesId startView;
    bool bindFlag = AlipayModel::GetInstance()->AlipayGetBindstate();
    if (!bindFlag) {
        alipayBind = new AlipayBind(presenter_);
        if (alipayBind == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView:: OnStart alipayBind null error!");
            return;
        }
        alipayBind->InitView();
        startView = ALIPAY_BIND_VIEW;
        container[startView] = alipayBind;
        AlipayModel::GetInstance()->AlipayStartBind();
    } else {
        alipayList = new AlipayList(presenter_);
        if (alipayList == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView:: OnStart alipayList null error!");
            return;
        }
        alipayList->InitListView();
        startView = ALIPAY_LIST_VIEW;
        container[startView] = alipayList;
    }
    AlipayModel::GetInstance()->SetAlipayInterLis(startView);
    group = container[startView];
    alipayWhichInter = startView;

    if (group == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::OnStart group null error!");
        return;
    }
    AddViewToRootContainer(group);
    return;
}

void AlipayMainView::DeleteViceInter(void)
{
    if (alipayBind != nullptr) {
        delete alipayBind;
        alipayBind = nullptr;
    }
    if (alipayExitBind != nullptr) {
        delete alipayExitBind;
        alipayExitBind = nullptr;
    }
    if (alipayBindRes != nullptr) {
        delete alipayBindRes;
        alipayBindRes = nullptr;
    }
    if (alipayBindAni != nullptr) {
        delete alipayBindAni;
        alipayBindAni = nullptr;
    }

    if (alipayPayCode != nullptr) {
        delete alipayPayCode;
        alipayPayCode = nullptr;
    }
    if (alipayBarCode != nullptr) {
        delete alipayBarCode;
        alipayBarCode = nullptr;
    }
    if (alipaySetting != nullptr) {
        delete alipaySetting;
        alipaySetting = nullptr;
    }
    if (alipayExitBindSetting != nullptr) {
        delete alipayExitBindSetting;
        alipayExitBindSetting = nullptr;
    }
    if (alipayUnbindFinish != nullptr) {
        delete alipayUnbindFinish;
        alipayUnbindFinish = nullptr;
    }

    if (alipayHelp != nullptr) {
        delete alipayHelp;
        alipayHelp = nullptr;
    }

    if (alipayList != nullptr) {
        delete alipayList;
        alipayList = nullptr;
    }

    if (alipayGetTransList != nullptr) {
        delete alipayGetTransList;
        alipayGetTransList = nullptr;
    }
    if (alipayTransTips != nullptr) {
        delete alipayTransTips;
        alipayTransTips = nullptr;
    }
    if (alipayTransCode != nullptr) {
        delete alipayTransCode;
        alipayTransCode = nullptr;
    }
    if (alipayTransEnable != nullptr) {
        delete alipayTransEnable;
        alipayTransEnable = nullptr;
    }
    if (alipayTransRes != nullptr) {
        delete alipayTransRes;
        alipayTransRes = nullptr;
    }
    if (alipayTransList != nullptr) {
        delete alipayTransList;
        alipayTransList = nullptr;
    }

    if (alipayTransAni != nullptr) {
        delete alipayTransAni;
        alipayTransAni = nullptr;
    }

    return;
}

int16 AlipayMainView::GetWhichInter(void)
{
    return alipayWhichInter;
}

int16 AlipayMainView::GetListenInter(void)
{
    return interListen;
}

void AlipayMainView::DropView(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView");
    if (&view == container[ALIPAY_PAY_BARCODE_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_PAY_BARCODE_VIEW]");
        ChangeView(ALIPAY_PAY_BARCODE_VIEW, ALIPAY_PAY_VIEW);
    } else if (&view == container[ALIPAY_PAY_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_PAY_VIEW]");
        AlipayModel::GetInstance()->AlipayStopSwitchTimer();
        ChangeView(ALIPAY_PAY_VIEW, ALIPAY_LIST_VIEW);
    } else if (&view == container[ALIPAY_SETTING_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_SETTING_VIEW]");
        ChangeView(ALIPAY_SETTING_VIEW, ALIPAY_LIST_VIEW);
    } else if (&view == container[ALIPAY_HELP_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_HELP_VIEW]");
        ChangeView(ALIPAY_HELP_VIEW, ALIPAY_LIST_VIEW);
    } else if (&view == container[ALIPAY_BIND_ANI_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_BIND_ANI_VIEW]");
        ChangeView(ALIPAY_BIND_ANI_VIEW, ALIPAY_BIND_EXIT_VIEW);
    } else if (&view == container[ALIPAY_LIST_VIEW] || &view == container[ALIPAY_BIND_VIEW]) {
        if (&view == container[ALIPAY_BIND_VIEW]) {
            AlipayModel::GetInstance()->AlipayFinishBind();
        }
        AlipayModel::GetInstance()->AlipayStopSwitchTimer();
        NativeAbility::GetInstance().ChangeSliceToApplist();

    } else if (&view == container[ALIPAY_TRANS_LIST_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_TRANS_LIST_VIEW]");
        ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_LIST_VIEW);
    } else if (&view == container[ALIPAY_TRANS_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_TRANS_VIEW]");
        ChangeView(ALIPAY_TRANS_VIEW, ALIPAY_TRANS_LIST_VIEW);
    } else if (&view == container[ALIPAY_TRANS_ENABLE_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_TRANS_ENABLE_VIEW]");
        AlipayModel::GetInstance()->AlipayTransStopGetListsTimer();
        ChangeView(ALIPAY_TRANS_ENABLE_VIEW, ALIPAY_LIST_VIEW);
    }

    return;
}

void AlipayMainView::SwitchView(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::SwitchView");
    if (&view == container[ALIPAY_PAY_VIEW]) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_PAY_VIEW]");
        ChangeView(ALIPAY_PAY_VIEW, ALIPAY_PAY_BARCODE_VIEW);
    } else if (&view == container[ALIPAY_BIND_RES_VIEW] && presenter_->AlipayGetBindFlag()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::DropView::container[ALIPAY_BIND_VIEW]");
        AlipayModel::GetInstance()->AlipayStopSwitchTimer();
        ChangeView(ALIPAY_BIND_RES_VIEW, ALIPAY_LIST_VIEW);
    }
    return;
}

bool AlipayMainView::ChangePayView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangePayView");
    AlipayModel::GetInstance()->AlipayStartSwitchTimer(60000);
    if (container[thisView] == nullptr) {
        alipayPayCode = new AlipayPayCode(presenter_);
        if (alipayPayCode == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangePayView alipayPayCode null error!");
            return false;
        }
        if (!alipayPayCode->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangePayView  InitView null error!");
            return false;
        }

        container[thisView] = alipayPayCode;
        AddViewToRootContainer(container[thisView]);
    } else {
        uint8_t payStr[ALIPAY_GET_PAY_STR_MAX_LENGTH] = { 0 };
        presenter_->AlipayGetPayCode(payStr);
        alipayPayCode->AlipayRefreshQrcode((const char *)payStr);
    }

    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeSettingView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeSettingView");
    if (container[thisView] == nullptr) {
        alipaySetting = new AlipaySetting(presenter_);
        if (alipaySetting == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeSettingView alipayPayCode null error!");
            return false;
        }
        if (!alipaySetting->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeSettingView  InitView null error!");
            return false;
        }

        container[thisView] = alipaySetting;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeHelpView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeHelpView");
    if (container[thisView] == nullptr) {
        alipayHelp = new AlipayHelp(presenter_);
        if (alipayHelp == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeHelpView alipayHelp null error!");
            return false;
        }
        if (!alipayHelp->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeHelpView  InitView null error!");
            return false;
        }

        container[thisView] = alipayHelp;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeBindAniView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindAniView");
    if (container[thisView] == nullptr) {
        alipayBindAni = new AlipayBindAni(presenter_);
        if (alipayBindAni == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindAniView alipayBindAni null error!");
            return false;
        }
        if (!alipayBindAni->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindAniView  InitView null error!");
            return false;
        }

        container[thisView] = alipayBindAni;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeBindExitView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindExitView");
    if (container[thisView] == nullptr) {
        alipayExitBind = new AlipayExitBind(presenter_);
        if (alipayExitBind == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindExitView alipayExitBind null error!");
            return false;
        }
        if (!alipayExitBind->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindExitView  InitView null error!");
            return false;
        }

        container[thisView] = alipayExitBind;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeBindResView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindResView");
    if (container[thisView] == nullptr) {
        alipayBindRes = new AlipayBindRes(presenter_);
        if (alipayBindRes == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindResView alipayBindRes null error!");
            return false;
        }
        if (!alipayBindRes->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindResView  InitView null error!");
            return false;
        }

        container[thisView] = alipayBindRes;

        AddViewToRootContainer(container[thisView]);
    }
    alipayBindRes->AlipayRefreshBindRes();
    if (presenter_->AlipayGetBindFlag()) {
        AlipayModel::GetInstance()->AlipayStartSwitchTimer(ALIPAY_BIND_SUCCESS_RES_TIMER_LENGTH);
    } else {
        AlipayModel::GetInstance()->AlipayStartSwitchTimer(ALIPAY_BIND_FAILURE_RES_TIMER_LENGTH);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeUnbindView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeUnbindView");
    if (container[thisView] == nullptr) {
        alipayExitBindSetting = new AlipayUnbind(presenter_);
        if (alipayExitBindSetting == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                "AlipayMainView::ChangeUnbindView alipayExitBindSetting null error!");
            return false;
        }
        if (!alipayExitBindSetting->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeUnbindView  InitView null error!");
            return false;
        }

        container[thisView] = alipayExitBindSetting;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeUnbindFinishView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeUnbindFinishView");
    AlipayModel::GetInstance()->AlipayUnbind();
    if (container[thisView] == nullptr) {
        alipayUnbindFinish = new AlipayUnbindFinish(presenter_);
        if (alipayUnbindFinish == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                "AlipayMainView::ChangeUnbindFinishView alipayUnbindFinish null error!");
            return false;
        }
        if (!alipayUnbindFinish->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeUnbindFinishView  InitView null error!");
            return false;
        }

        container[thisView] = alipayUnbindFinish;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeBindView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindView");
    AlipayModel::GetInstance()->AlipayStartBind();
    if (container[thisView] == nullptr) {
        alipayBind = new AlipayBind(presenter_);
        if (alipayBind == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindView alipayBind null error!");
            return false;
        }
        if (!alipayBind->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBindView  InitView null error!");
            return false;
        }

        container[thisView] = alipayBind;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransView");
    if (container[thisView] == nullptr) {
        alipayTransCode = new AlipayTransCode(presenter_);
        if (alipayTransCode == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransView alipayTransCode null error!");
            return false;
        }
        if (!alipayTransCode->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransCode;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransListView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransListView");
    if (container[thisView] == nullptr) {
        alipayTransList = new AlipayTransList(presenter_);
        if (alipayTransList == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransListView alipayTransList null error!");
            return false;
        }
        if (!alipayTransList->InitListView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransListView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransList;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransEnableView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransEnableView");
    if (container[thisView] == nullptr) {
        alipayTransEnable = new AlipayTransEnable(presenter_);
        if (alipayTransEnable == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                "AlipayMainView::ChangeTransEnableView alipayTransEnable null error!");
            return false;
        }
        if (!alipayTransEnable->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransEnableView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransEnable;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransTipsView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransTipsView");
    if (container[thisView] == nullptr) {
        alipayTransTips = new AlipayTransTips(presenter_);
        if (alipayTransTips == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransTipsView alipayTransTips null error!");
            return false;
        }
        if (!alipayTransTips->InitTipsView(presenter_->AlipayGetSwitchIndex())) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransTipsView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransTips;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransResView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransResView");
    if (container[thisView] == nullptr) {
        alipayTransRes = new AlipayTransRes(presenter_);
        if (alipayTransRes == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransResView alipayTransRes null error!");
            return false;
        }
        if (!alipayTransRes->InitResView(presenter_->AlipayGetSwitchIndex())) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransResView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransRes;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeGetTransListView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeGetTransListView");
    if (container[thisView] == nullptr) {
        alipayGetTransList = new AlipayGetTransList(presenter_);
        if (alipayGetTransList == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                "AlipayMainView::ChangeGetTransListView alipayGetTransList null error!");
            return false;
        }
        if (!alipayGetTransList->InitGetView(presenter_->AlipayGetSwitchIndex())) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeGetTransListView  InitView null error!");
            return false;
        }

        container[thisView] = alipayGetTransList;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeTransAniView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransProcessView");
    if (container[thisView] == nullptr) {
        alipayTransAni = new AlipayTransAni(presenter_);
        if (alipayTransAni == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransProcessView alipayTransAni null error!");
            return false;
        }
        if (!alipayTransAni->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeTransProcessView  InitView null error!");
            return false;
        }

        container[thisView] = alipayTransAni;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeBarCodeView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBarView");
    AlipayModel::GetInstance()->AlipayStartSwitchTimer(60000);
    if (container[thisView] == nullptr) {
        alipayBarCode = new AlipayBarCode(presenter_);
        if (alipayBarCode == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBarView alipayPayCode null error!");
            return false;
        }
        if (!alipayBarCode->InitView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeBarView  InitView null error!");
            return false;
        }

        container[thisView] = alipayBarCode;

        AddViewToRootContainer(container[thisView]);
    } else {
        uint8_t barStr[ALIPAY_GET_PAY_STR_MAX_LENGTH] = { 0 };
        presenter_->AlipayGetPayCode(barStr);
        AlipayBarCode::GetInstance()->AlipayRefreshBarcode((const char *)barStr);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

bool AlipayMainView::ChangeListView(int16 thisView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeListView");
    if (container[thisView] == nullptr) {
        alipayList = new AlipayList(presenter_);
        if (alipayList == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeListView alipayList null error!");
            return false;
        }
        if (!alipayList->InitListView()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeListView  InitView null error!");
            return false;
        }

        container[thisView] = alipayList;

        AddViewToRootContainer(container[thisView]);
    }
    alipayWhichInter = thisView;
    alipayWhichMea = thisView;
    return true;
}

struct AlipayViewMapper {
    const int16 viewId;
    bool (AlipayMainView::*func)(int16 thisView);
};

static const AlipayViewMapper ViewMapper[] = {
    {ALIPAY_PAY_VIEW, &AlipayMainView::ChangePayView},
    {ALIPAY_SETTING_VIEW, &AlipayMainView::ChangeSettingView},
    {ALIPAY_PAY_BARCODE_VIEW, &AlipayMainView::ChangeBarCodeView},
    {ALIPAY_LIST_VIEW, &AlipayMainView::ChangeListView},
    {ALIPAY_HELP_VIEW, &AlipayMainView::ChangeHelpView},
    {ALIPAY_BIND_ANI_VIEW, &AlipayMainView::ChangeBindAniView},
    {ALIPAY_BIND_EXIT_VIEW, &AlipayMainView::ChangeBindExitView},
    {ALIPAY_BIND_RES_VIEW, &AlipayMainView::ChangeBindResView},
    {ALIPAY_SETTING_UNBIND_VIEW, &AlipayMainView::ChangeUnbindView},
    {ALIPAY_SETTING_FINISH_VIEW, &AlipayMainView::ChangeUnbindFinishView},
    {ALIPAY_BIND_VIEW, &AlipayMainView::ChangeBindView},
    {ALIPAY_TRANS_VIEW, &AlipayMainView::ChangeTransView},
    {ALIPAY_TRANS_LIST_VIEW, &AlipayMainView::ChangeTransListView},
    {ALIPAY_TRANS_ENABLE_VIEW, &AlipayMainView::ChangeTransEnableView},
    {ALIPAY_TRANS_TIPS_VIEW, &AlipayMainView::ChangeTransTipsView},
    {ALIPAY_TRANS_RES_VIEW, &AlipayMainView::ChangeTransResView},
    {ALIPAY_GET_TRANS_LIST_VIEW, &AlipayMainView::ChangeGetTransListView},
    {ALIPAY_TRANS_ANI_VIEW, &AlipayMainView::ChangeTransAniView},

};

bool AlipayMainView::ClickFunc(int16 interface)
{
    int funcSize = sizeof(ViewMapper) / sizeof(ViewMapper[0]);
    for (int i = 0; i < funcSize; i++) {
        if (interface == ViewMapper[i].viewId) {
            bool jug = (this->*(ViewMapper[i].func))(interface);
            if (!jug) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "MainAlarmView:: ChangeInterFace[%d] false", interface);
                return false;
            }
            return true;
        }
    }
    return false;
}

static const int16 DestroyMapper[] = {
    ALIPAY_TRANS_TIPS_VIEW,
    ALIPAY_TRANS_RES_VIEW,
    ALIPAY_GET_TRANS_LIST_VIEW,
    ALIPAY_TRANS_VIEW,
    ALIPAY_TRANS_LIST_VIEW,
    ALIPAY_SETTING_VIEW,
};

void AlipayMainView::DestroyTempView(int16 hideView)
{
    int viewSize = sizeof(DestroyMapper) / sizeof(DestroyMapper[0]);
    for (int i = 0; i < viewSize; i++) {
        if (hideView == DestroyMapper[i]) {
            GetRootContainer()->Remove(container[hideView]);
            container[hideView] = nullptr;
            break;
        }
    }
    if (hideView == ALIPAY_TRANS_TIPS_VIEW) {
        delete alipayTransTips;
        alipayTransTips = nullptr;
    } else if (hideView == ALIPAY_TRANS_RES_VIEW) {
        delete alipayTransRes;
        alipayTransRes = nullptr;
    } else if (hideView == ALIPAY_GET_TRANS_LIST_VIEW) {
        delete alipayGetTransList;
        alipayGetTransList = nullptr;
    } else if (hideView == ALIPAY_TRANS_VIEW) {
        delete alipayTransCode;
        alipayTransCode = nullptr;
    } else if (hideView == ALIPAY_TRANS_LIST_VIEW) {
        delete alipayTransList;
        alipayTransList = nullptr;
    } else if (hideView == ALIPAY_SETTING_VIEW) {
        delete alipaySetting;
        alipaySetting = nullptr;
    }
}

void AlipayMainView::ChangeView(int16 hideView, int16 showView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeView hideView = %d, showView = %d", hideView,
        showView);
    if (container[hideView] == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeView hideView container is null: %x.",
            container[hideView]);
        return;
    }
    if (hideView > ALIPAY_INTERFACES_MAX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeView Overstep ones bounds");
        return;
    }
    interListen = hideView;
    if (!ClickFunc(showView)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::ChangeView ClickFunc error");
        return;
    }

    container[hideView]->SetVisible(false);
    container[showView]->SetVisible(true);

    AlipayModel::GetInstance()->SetAlipayInterLis(showView);

    Draw();
    DestroyTempView(hideView);
    return;
}
}