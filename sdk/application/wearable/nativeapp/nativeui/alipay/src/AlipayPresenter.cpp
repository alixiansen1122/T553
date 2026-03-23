/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay presenter.
 * Author:
 * Create:
 */

#include "alipay/AlipayPresenter.h"
#include "alipay/AlipayMainView.h"
#include "ChangeSliceListener.h"
#include "graphic_service.h"
#include "string"
#include "iostream"
#include "sstream"
#include "errcode.h"
#include "pm_definition.h"
#include "alipay/AlipayList/AlipayListView.h"
#include "alipay/AlipayList/AlipayListItemView.h"
#include "alipay/AlipayTrans/AlipayTransListItemView.h"
#include "alipay/AlipayTrans/AlipayTransListView.h"
#include "NativeRegisterManager.h"

namespace OHOS {

REGIST_MENU(VIEW_MAIN_ALIPAY, AlipayMainView, AlipayPresenter, PNG_APPLIST_ALIPAY_IMAGE, PNG_APPLIST_DEFAULT_IMG, "支付宝");

static AlipayPresenter *g_alipayPresenter = nullptr;

ColorType AlipayPresenter::CyanColor()
{
    return Color::GetColorFromRGB(0x1C, 0xF4, 0xED);
}

AlipayPresenter::AlipayPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter new");
    alipayMode_ = AlipayModel::GetInstance();
    g_alipayPresenter = this;
}

AlipayPresenter::~AlipayPresenter()
{
    g_alipayPresenter = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::~AlipayPresenter");
}

AlipayPresenter *AlipayPresenter::GetInstance()
{
    return g_alipayPresenter;
}

void AlipayPresenter::OnStart()
{
    alipayMode_->AlipayExitLpm();
    if (!alipayMode_->AlipayInitModel()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::OnStart AlipayInitModel fail.");
        NativeAbility::GetInstance().ChangeSliceToApplist();
        return;
    }
}

void AlipayPresenter::OnResume(void)
{
    svr_alipay_create_task();
}

void AlipayPresenter::OnStop(void)
{
    svr_alipay_destroy_task();
    alipayMode_->AlipayDeinitModel();
    alipayMode_->AlipayEnterLpm();
}

bool AlipayPresenter::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        view_->DropView(view);
    }
    if (event.GetDragDirection() == DragEvent::DIRECTION_RIGHT_TO_LEFT) {
        view_->SwitchView(view);
    }
    return true;
}

void AlipayPresenter::ClickExitAlipay(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickExitAlipay start");
    AlipayModel::GetInstance()->AlipayFinishBind();
    uint16 preSlice = NativeAbility::GetInstance().GetPreSliceId();
    NativeAbility::GetInstance().ChangeSlice(preSlice);
    return;
}

void AlipayPresenter::ClickJumpPay(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickPromptWear start");
    view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_PAY_VIEW);
    return;
}

void AlipayPresenter::ClickJumpTrans(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpTrans start");
    int32_t ret = alipayMode_->AlipayGetLocalTranslist();
    if (ret != 0) {
        AlipaySetSwitchIndex(ALIPAY_TRANS_GET_LIST_FIRST);
        view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_GET_TRANS_LIST_VIEW);
    } else if (ret == 0) {
        int32_t get_res = alipayMode_->AlipayGetLastTranscode();
        if (get_res == 0) {
            view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_TRANS_VIEW);
        } else {
            view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_TRANS_LIST_VIEW);
        }
    }
    return;
}

void AlipayPresenter::ClickRefreshList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickRefreshList start");
    alipayMode_->AlipaySendMsg(ALIPAY_MSG_UPDATA_TRANS_LIST);
    view_->ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_TRANS_ANI_VIEW);
    return;
}

void AlipayPresenter::ClickBackList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickBackList start");
    view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickUpdateList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickUpdateList start");
    alipayMode_->AlipaySendMsg(ALIPAY_MSG_UPDATA_TRANS_LIST);
    view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_TRANS_ANI_VIEW);
    return;
}

void AlipayPresenter::ClickTransJumpHelp(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransJumpHelp start");
    view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_HELP_VIEW);
    return;
}

void AlipayPresenter::ClickTranstBackList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTranstBackList start");
    int16_t currentView = alipayMode_->GetAlipayInterLis();
    view_->ChangeView(currentView, ALIPAY_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickTransToTransList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransJumpHelp start");
    int16_t currentView = alipayMode_->GetAlipayInterLis();
    view_->ChangeView(currentView, ALIPAY_TRANS_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickTransToTransCode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransToTransCode start");
    AlipayTransListItemView *alipayTransListItemView = static_cast<AlipayTransListItemView *>(&view);
    uint32_t index = alipayTransListItemView->GetAlipayTransListIndex();
    if (index >= AlipayGetCardSum()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransToTransCode index error:%u", index);
        return;
    }
    AlipaySetCardNum(index);
    int32_t ret = alipayMode_->AlipayCheckTransCard(index);
    if (ret != 0) {
        alipayMode_->AlipaySendMsg(ALIPAY_MSG_UPDATA_SPECIAL_CARD);
        view_->ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_TRANS_ANI_VIEW);
        return;
    }

    ret = alipayMode_->AlipayGetTransCode(index);
    if (ret == 0) {
        view_->ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_TRANS_VIEW);
    } else if (ret == ALIPAY_TRANS_CODE_NET_ERROR) {
        AlipaySetSwitchIndex(ALIPAY_TRANS_RES_CODE);
        view_->ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_TRANS_RES_VIEW);
    } else {
        if (ret == ALIPAY_TRANS_CODE_DATA_LIMIT) {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_SECURE);
        } else if (ret == ALIPAY_TRANS_CODE_NOT_SUPPORT) {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_NOT_SUPPORT);
        } else {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_LIST_UNKNOW);
        }
        view_->ChangeView(ALIPAY_TRANS_LIST_VIEW, ALIPAY_TRANS_TIPS_VIEW);
    }
    return;
}

void AlipayPresenter::ClickRetryTransCode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickRetryTransCode start");
    uint32_t index = AlipayGetCardNum();
    int32_t ret = alipayMode_->AlipayGetTransCode(index);
    if (ret == 0) {
        view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_TRANS_VIEW);
    } else if (ret == ALIPAY_TRANS_CODE_NET_ERROR) {
        AlipaySetSwitchIndex(ALIPAY_TRANS_RES_CODE);
        view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_TRANS_RES_VIEW);
    } else {
        if (ret == ALIPAY_TRANS_CODE_DATA_LIMIT) {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_SECURE);
        } else if (ret == ALIPAY_TRANS_CODE_NOT_SUPPORT) {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_NOT_SUPPORT);
        } else {
            AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_LIST_UNKNOW);
        }
        view_->ChangeView(ALIPAY_GET_TRANS_LIST_VIEW, ALIPAY_TRANS_TIPS_VIEW);
    }
    return;
}

void AlipayPresenter::ClickTransJumpGetList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransJumpGetList start");
    AlipaySetSwitchIndex(ALIPAY_TRANS_GET_LIST_CLICK_DETAIL);
    view_->ChangeView(ALIPAY_TRANS_RES_VIEW, ALIPAY_GET_TRANS_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickTransJumpGetCode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransJumpGetCode start");
    AlipaySetSwitchIndex(ALIPAY_TRANS_GET_LIST_CODE_DETAIL);
    view_->ChangeView(ALIPAY_TRANS_RES_VIEW, ALIPAY_GET_TRANS_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickTransCodeBackList(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickTransCodeBackList start");
    view_->ChangeView(ALIPAY_TRANS_VIEW, ALIPAY_TRANS_LIST_VIEW);
    return;
}

void AlipayPresenter::ClickJumpSetting(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpSetting start");
    view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_SETTING_VIEW);
    return;
}

void AlipayPresenter::ClickJumpHelp(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpHelp start");
    view_->ChangeView(ALIPAY_LIST_VIEW, ALIPAY_HELP_VIEW);
    return;
}

void AlipayPresenter::ClickJumpBindAni(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpBindAni start");
    view_->ChangeView(ALIPAY_BIND_EXIT_VIEW, ALIPAY_BIND_ANI_VIEW);
    return;
}

void AlipayPresenter::ClickJumpUnbind(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpUnbind start");
    view_->ChangeView(ALIPAY_SETTING_VIEW, ALIPAY_SETTING_UNBIND_VIEW);
    return;
}

void AlipayPresenter::ClickJumpBindFinish(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickJumpBindFinish start");
    view_->ChangeView(ALIPAY_SETTING_UNBIND_VIEW, ALIPAY_SETTING_FINISH_VIEW);
    return;
}

void AlipayPresenter::ClickBackUnbind(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickBackUnbind start");
    view_->ChangeView(ALIPAY_SETTING_UNBIND_VIEW, ALIPAY_SETTING_VIEW);
    return;
}

void AlipayPresenter::ClickBackBindCode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickBackBindCode start");
    view_->ChangeView(ALIPAY_SETTING_FINISH_VIEW, ALIPAY_BIND_VIEW);
    return;
}

void AlipayPresenter::ClickRefreshQrcode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickRefreshQrcode start");
    AlipayPayCode *alipayPayCodeView = AlipayPayCode::GetInstance();
    if (alipayPayCodeView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayPresenter::ClickRefreshQrcode AlipayPayCode::GetInstance() nullptr");
        return;
    }
    alipayMode_->AlipayStartSwitchTimer(60000);
    uint8_t str[ALIPAY_GET_PAY_STR_MAX_LENGTH] = {0};
    AlipayGetPayCode(str);
    alipayPayCodeView->AlipayRefreshQrcode((const char *)str);
    return;
}

void AlipayPresenter::ClickRefreshBarcode(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickRefreshBarcode start");
    AlipayBarCode *alipayBarCodeView = AlipayBarCode::GetInstance();
    if (alipayBarCodeView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "AlipayPresenter::ClickRefreshBarcode AlipayBarCode::GetInstance() nullptr");
        return;
    }
    alipayMode_->AlipayStartSwitchTimer(60000);
    uint8_t str[ALIPAY_GET_PAY_STR_MAX_LENGTH] = {0};
    AlipayGetPayCode(str);
    alipayBarCodeView->AlipayRefreshBarcode((const char *)str);

    return;
}

void AlipayPresenter::ClickGetBindStr(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickGetBindStr start");
    uint8_t str[ALIPAY_GET_BIND_STR_MAX_LENGTH] = {0};
    int32_t ret = AlipayGetBindCode(str);
    if (ret == 0) {
        view_->ChangeView(ALIPAY_GET_BIND_FAILURE_VIEW, ALIPAY_BIND_VIEW);
    }
    return;
}

void AlipayPresenter::ClickSwitchBar(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickSwitchBar start");
    view_->ChangeView(ALIPAY_PAY_VIEW, ALIPAY_PAY_BARCODE_VIEW);
    return;
}

void AlipayPresenter::ClickSwitchQr(UIView &view)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickSwitchQR start");
    view_->ChangeView(ALIPAY_PAY_BARCODE_VIEW, ALIPAY_PAY_VIEW);
    return;
}

struct AlipayPresentMapper1 {
    const char *viewId;
    void (AlipayPresenter::*func)(UIView &view);
};

static const AlipayPresentMapper1 PresentMapper[] = {
    {ALIPAY_INTER_EXIT, &AlipayPresenter::ClickExitAlipay},
    {ALIPAY_LIST_PAY_BUTTON, &AlipayPresenter::ClickJumpPay},
    {ALIPAY_PAY_SWITCH_BAR, &AlipayPresenter::ClickSwitchBar},
    {ALIPAY_PAY_SWITCH_QR, &AlipayPresenter::ClickSwitchQr},
    {ALIPAY_LIST_SETTING_BUTTON, &AlipayPresenter::ClickJumpSetting},
    {ALIPAY_LIST_HELP_BUTTON, &AlipayPresenter::ClickJumpHelp},
    {ALIPAY_BIND_CONTINUE, &AlipayPresenter::ClickJumpBindAni},
    {ALIPAY_UNBIND_BUTTON, &AlipayPresenter::ClickJumpUnbind},
    {ALIPAY_SETTING_UNBIND, &AlipayPresenter::ClickJumpBindFinish},
    {ALIPAY_SETTING_CANCEL, &AlipayPresenter::ClickBackUnbind},
    {ALIPAY_SETTING_FINISH, &AlipayPresenter::ClickBackBindCode},
    {ALIPAY_PAY_QRCODE, &AlipayPresenter::ClickRefreshQrcode},
    {ALIPAY_BIND_BUTTON, &AlipayPresenter::ClickGetBindStr},
    {ALIPAY_PAY_BARCODE, &AlipayPresenter::ClickRefreshBarcode},
    {ALIPAY_LIST_TRANS_BUTTON, &AlipayPresenter::ClickJumpTrans},
    {ALIPAY_TRANS_REFRESH_LIST, &AlipayPresenter::ClickRefreshList},
    {ALIPAY_TRANS_GET_LIST_EXIT, &AlipayPresenter::ClickBackList},
    {ALIPAY_TRANS_GET_LIST_FINISH, &AlipayPresenter::ClickUpdateList},
    {ALIPAY_TRANS_GET_LIST_DETAIL, &AlipayPresenter::ClickTransJumpHelp},
    {ALIPAY_TRANS_CERTAIN_BACK_LIST, &AlipayPresenter::ClickTranstBackList},
    {ALIPAY_TRANS_CERTAIN_TO_TRANS, &AlipayPresenter::ClickTransToTransList},
    {ALIPAY_TRANS_LIST_BUTTON, &AlipayPresenter::ClickTransToTransCode},
    {ALIPAY_TRANS_RETRY_TRANS_CODE, &AlipayPresenter::ClickRetryTransCode},
    {ALIPAY_TRANS_LIST_RES_DETAIL, &AlipayPresenter::ClickTransJumpGetList},
    {ALIPAY_TRANS_CODE_RES_DETAIL, &AlipayPresenter::ClickTransJumpGetCode},
    {ALIPAY_TRANS_CODE_BACK_LIST, &AlipayPresenter::ClickTransCodeBackList},
};

bool AlipayPresenter::ClickFuncId(UIView &view)
{
    if (view.GetViewId() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickFuncId buttonName nullptr error!");
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::ClickFuncId buttonName = %s", view.GetViewId());
    for (uint16 i = 0; i < sizeof(PresentMapper) / sizeof(PresentMapper[0]); i++) {
        if (strcmp(view.GetViewId(), PresentMapper[i].viewId) == 0) {
            (this->*(PresentMapper[i].func))(view);
            return true;
        }
    }
    return false;
}

bool AlipayPresenter::OnClick(UIView &view, const ClickEvent &event)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayPresenter::OnClick %s", view.GetViewId());
    UNUSED(event);

    ClickFuncId(view);
    return true;
}

int32_t AlipayPresenter::AlipayGetPayCode(uint8_t *payStr)
{
    return alipayMode_->AlipayGetPayCode(payStr);
}

int32_t AlipayPresenter::AlipayGetNickName(uint8_t *nickName)
{
    return alipayMode_->AlipayGetNickName(nickName);
}

int32_t AlipayPresenter::AlipayGetBindCode(uint8_t *bindStr)
{
    return alipayMode_->AlipayGetBindCode(bindStr);
}

int32_t AlipayPresenter::AlipayGetIdName(uint8_t *idStr)
{
    return alipayMode_->AlipayGetIdName(idStr);
}

int32_t AlipayPresenter::AlipayGetAidCode(int8_t *aidStr)
{
    return alipayMode_->AlipayGetAidCode(aidStr);
}

bool AlipayPresenter::AlipayGetBindFlag(void)
{
    return alipayMode_->AlipayGetBindFlag();
}

void AlipayPresenter::AlipaySetBindFlag(bool bindFlag)
{
    alipayMode_->AlipaySetBindFlag(bindFlag);
}

uint32_t AlipayPresenter::AlipayGetSwitchIndex(void)
{
    return alipayMode_->AlipayGetSwitchIndex();
}
void AlipayPresenter::AlipaySetSwitchIndex(uint32_t switchIndex)
{
    alipayMode_->AlipaySetSwitchIndex(switchIndex);
}

uint32_t AlipayPresenter::AlipayGetCardNum(void)
{
    return alipayMode_->AlipayGetCardNum();
}

void AlipayPresenter::AlipaySetCardNum(uint32_t cardNum)
{
    alipayMode_->AlipaySetCardNum(cardNum);
}

uint32_t AlipayPresenter::AlipayGetCardSum(void)
{
    return alipayMode_->AlipayGetCardSum();
}

void AlipayPresenter::AlipaySetCardSum(uint32_t cardSum)
{
    alipayMode_->AlipaySetCardSum(cardSum);
}

char* AlipayPresenter::AlipayGetCardTitle(uint32_t index)
{
    return alipayMode_->AlipayGetCardTitle(index);
}

uint8_t* AlipayPresenter::AlipayGetTransCode(void)
{
    return alipayMode_->AlipayGetTransCode();
}

uint32_t AlipayPresenter::AlipayGetTransCodeLen(void)
{
    return alipayMode_->AlipayGetTransCodeLen();
}

char* AlipayPresenter::AlipayGetCardTitle(void)
{
    return alipayMode_->AlipayGetCardTitle();
}

}