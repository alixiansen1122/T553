/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay model.
 * Author:
 * Create:
 */

#include "UiConfig.h"
#include "alipay_feature.h"
#include "alipay/AlipayModel.h"
#include "alipay/AlipayMainView.h"
#include "graphic_service.h"

namespace OHOS {
AlipayModel::AlipayModel() : alipayApi(alipay_svr_get_api())
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayModel");
}

AlipayModel::~AlipayModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::~AlipayModel");
}

AlipayModel *AlipayModel::GetInstance(void)
{
    static AlipayModel alipayModel;
    return &alipayModel;
}

void AlipayModel::SetAlipayInterLis(int16 interListen)
{
    alipayInterLis = interListen;
}

int16 AlipayModel::GetAlipayInterLis(void)
{
    return alipayInterLis;
}

void AlipayBindCallBackProc(void *argument)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindCallBackProc in.");
    const alipay_svr_api_t *alipay_api = alipay_svr_get_api();
    int32_t status = 0;
    int32_t ret = alipay_api->get_bind_status(&status);
    if (AlipayMainView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::GetInstance is null, do nothing.");
        return;
    }

    int16_t currentView = AlipayModel::GetInstance()->GetAlipayInterLis();
    uint32_t reFlag = AlipayModel::GetInstance()->AlipayGetRefreshFlag();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
        "AlipayModel::AlipayBindProc get_bind_status fail,ret :%d,status:%u,flag:%u.", ret, status, reFlag);
    if (status == ALIPAY_INTER_STATUS_START_BINDING && currentView == ALIPAY_BIND_VIEW) {
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_BIND_VIEW, ALIPAY_BIND_ANI_VIEW);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindCallBackProc ALIPAY_INTER_STATUS_START_BINDING");
        AlipayModel::GetInstance()->AlipaySetRefreshFlag(1);
        return;
    } else if (ALIPAY_INTER_STATUS_BINDING_OK == status && reFlag == 1) {
        AlipayModel::GetInstance()->AlipaySetRefreshFlag(0);
        AlipayModel::GetInstance()->AlipayFinishBind();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindCallBackProc ALIPAY_INTER_STATUS_BINDING_OK");
        AlipayModel::GetInstance()->AlipaySetBindFlag(true);
        AlipayMainView::GetInstance()->ChangeView(currentView, ALIPAY_BIND_RES_VIEW);
    } else if (ALIPAY_INTER_STATUS_BINDING_FAIL == status && reFlag == 1) {
        AlipayModel::GetInstance()->AlipayStopBindTimer();
        AlipayModel::GetInstance()->AlipaySetBindFlag(false);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayBindCallBackProc ALIPAY_INTER_STATUS_BINDING_FAIL");
        AlipayModel::GetInstance()->AlipaySetRefreshFlag(0);
        AlipayMainView::GetInstance()->ChangeView(currentView, ALIPAY_BIND_RES_VIEW);
    }
}

void AlipayBindTimerCallBack(void *argument)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlipayBindCallBackProc, argument));
}

bool AlipayModel::AlipayStartBind(void)
{
    if (bindTimerId == nullptr) {
        bindTimerId = osTimerNew(AlipayBindTimerCallBack, osTimerPeriodic, (void *)0, nullptr);
        if (bindTimerId == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStartBind Create timer fail.");
            return false;
        }
    }

    int32_t retcode = osTimerStart(bindTimerId, ALIPAY_BIND_STATUS_TIMER_LENGTH);
    if (retcode != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStartBind Start timer fail");
        return false;
    }

    alipayApi->bind_start();
    uint8_t str[ALIPAY_GET_BIND_STR_MAX_LENGTH] = { 0 };
    AlipayGetBindCode(str);
    return true;
}

bool AlipayModel::AlipayFinishBind(void)
{
    AlipayStopBindTimer();
    alipayApi->bind_finish();
    return true;
}

void AlipaySwitchTimerCallBackProc(void *argument)
{
    UNUSED(argument);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipaySwitchTimerCallBackProc in.");
    if (AlipayMainView::GetInstance() == nullptr) {
        return;
    }
    int16 alipayLis = AlipayModel::GetInstance()->GetAlipayInterLis();
    int16 bindState = AlipayModel::GetInstance()->AlipayGetBindFlag();
    if (alipayLis == ALIPAY_BIND_RES_VIEW && bindState) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ALIPAY_BIND_RES_VIEW bindState true in.");
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_BIND_RES_VIEW, ALIPAY_LIST_VIEW);
    } else if (alipayLis == ALIPAY_BIND_RES_VIEW && (!bindState)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ALIPAY_BIND_RES_VIEW bindState false in.");
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_BIND_RES_VIEW, ALIPAY_BIND_VIEW);
    } else if (alipayLis == ALIPAY_PAY_VIEW) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ALIPAY_PAY_VIEW in.");
        uint8_t payStr[ALIPAY_GET_PAY_STR_MAX_LENGTH] = { 0 };
        AlipayModel::GetInstance()->AlipayGetPayCode(payStr);
        if (AlipayPayCode::GetInstance() != nullptr) {
            AlipayPayCode::GetInstance()->AlipayRefreshQrcode((const char *)payStr);
            AlipayModel::GetInstance()->AlipayStartSwitchTimer(60000);
        }
    } else if (alipayLis == ALIPAY_PAY_BARCODE_VIEW) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ALIPAY_PAY_BARCODE_VIEW in.");
        uint8_t barStr[ALIPAY_GET_PAY_STR_MAX_LENGTH] = { 0 };
        AlipayModel::GetInstance()->AlipayGetPayCode(barStr);
        if (AlipayBarCode::GetInstance() != nullptr) {
            AlipayBarCode::GetInstance()->AlipayRefreshBarcode((const char *)barStr);
            AlipayModel::GetInstance()->AlipayStartSwitchTimer(60000);
        }
    }

    return;
}

void AlipaySwitchTimerCallBack(void *argument)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipaySwitchTimerCallBack in.");
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlipaySwitchTimerCallBackProc, argument));
}

bool AlipayModel::AlipayStartSwitchTimer(uint16_t time)
{
    if (switchTimerId == nullptr) {
        switchTimerId = osTimerNew(AlipaySwitchTimerCallBack, osTimerOnce, (void *)0, nullptr);
        if (switchTimerId == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStartSwitchTimer Create timer fail.");
            return false;
        }
    } else {
        osTimerStop(switchTimerId);
    }

    int32_t retcode = osTimerStart(switchTimerId, time);
    if (retcode != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStartSwitchTimer Start timer fail");
        return false;
    }
    return true;
}

void AlipayModel::AlipayStopSwitchTimer()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStopSwitchTimer stop timer.");
    if (switchTimerId != nullptr) {
        osTimerStop(switchTimerId);
    }
    return;
}

bool AlipayModel::AlipayGetBindstate(void)
{
    bool state = alipayApi->get_bind_state();
    return state;
}

int32_t AlipayModel::AlipayGetBindCode(uint8_t *bindStr)
{
    int32_t bindStrLen = ALIPAY_GET_BIND_STR_MAX_LENGTH;
    int32_t ret = alipayApi->get_bind_str(bindStr, &bindStrLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetBindCode Get bind str.%s-%d", (char *)bindStr, ret);
    return ret;
}

int32_t AlipayModel::AlipayGetPayCode(uint8_t *payStr)
{
    uint32_t payStrLen = ALIPAY_GET_PAY_STR_MAX_LENGTH;
    int32_t ret = alipayApi->get_pay_str(payStr, &payStrLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetPayCode Get pay str:%s.", (char *)payStr);
    return ret;
}

int32_t AlipayModel::AlipayGetNickName(uint8_t *nickName)
{
    uint32_t nickNameLen = ALIPAY_GET_NICKNAME_MAX_LENGTH;
    int32_t ret = alipayApi->get_nickname(nickName, &nickNameLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetNickName Get nickstr:%s.", (char *)nickName);
    return ret;
}

int32_t AlipayModel::AlipayGetIdName(uint8_t *idStr)
{
    uint32_t idStrLen = ALIPAY_GET_ID_STR_MAX_LENGTH;
    int32_t ret = alipayApi->get_logon_id(idStr, &idStrLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetIdName Get Idname:%s.", (char *)idStr);
    return ret;
}

int32_t AlipayModel::AlipayGetAidCode(int8_t *aidStr)
{
    uint32_t aidStrLen = ALIPAY_GET_AID_STR_MAX_LENGTH;
    int32_t ret = alipayApi->get_aid_str(aidStr, &aidStrLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetAidCode Get aidstr:%s.", (char *)aidStr);
    return ret;
}

void AlipayModel::AlipayUnbind(void)
{
    int32_t ret = alipayApi->remove_bind();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetAidCode unbind ret:%u.", ret);
    return;
}

void AlipayModel::AlipayStopBindTimer(void)
{
    if (bindTimerId == nullptr) {
        return;
    }
    int32_t ret = osTimerStop(bindTimerId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayStopBindTimer in,%u.", ret);
    return;
}

void AlipayModel::AlipayEnterLpm(void)
{
    alipayApi->enter_lpm();
    return;
}

void AlipayModel::AlipayExitLpm(void)
{
    alipayApi->exit_lpm();
    return;
}

int32_t AlipayModel::AlipayGetLastTranscode()
{
    memset_s(transCode_, ALIPAY_TRANS_CODE_MAX_LEN, 0, ALIPAY_TRANS_CODE_MAX_LEN);
    memset_s(cardTitle_, ALIPAY_TRANS_CARD_MEMBER_LEN, 0, ALIPAY_TRANS_CARD_MEMBER_LEN);
    uint32_t len = ALIPAY_TRANS_CODE_MAX_LEN;

    int32_t ret = alipayApi->get_last_transcode(cardTitle_, transCode_, &len);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetLastTranscode in:ret:%u,len:%u.", ret, len);
    return ret;
}

int32_t AlipayModel::AlipayGetLocalTranslist()
{
    cardInfo_ = AlipayTransGetCardList();
    memset_s(cardInfo_, sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH, 0,
        sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH);
    uint32_t localListSize = sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH;
    int32_t ret = alipayApi->get_local_translist(cardInfo_, &localListSize, &cardSum_);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetLastTranscode in:%u.", ret);
    return ret;
}

int32_t AlipayModel::AlipayGetOnlineTranslist()
{
    cardInfo_ = AlipayTransGetCardList();
    mc_alipay_trans_card_info_t *onLineList = nullptr;

    onLineList = (mc_alipay_trans_card_info_t *)malloc(sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH);
    if (onLineList == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetOnlineTranslist alloc fail.");
        return -1;
    }
    memset_s(onLineList, sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH, 0,
        sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH);

    uint32_t onListSize = sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH;
    uint32_t onLineNum = 0;
    int32_t ret = alipayApi->get_online_translist(onLineList, &onListSize, &onLineNum);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayGetOnlineTranslist in,ret:%u,num:%u.", ret, onLineNum);

    if (ret == 0) {
        memset_s(cardInfo_, sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH, 0,
            sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH);
        memcpy_s(cardInfo_, sizeof(mc_alipay_trans_card_info_t) * ALIPAY_TRANS_CARD_MAX_LENGTH, onLineList, onListSize);
        cardSum_ = onLineNum;
    }
    free(onLineList);
    onLineList = nullptr;
    return ret;
}

int32_t AlipayModel::AlipayUpdateTransCard(uint32_t index)
{
    int32_t ret = alipayApi->update_trans_cardinfo(&cardInfo_[index]);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayUpdateTransCard in:%u.", ret);
    return ret;
}

int32_t AlipayModel::AlipayCheckTransCard(uint32_t index)
{
    mc_alipay_tans_card_status_t cardStatus = { 0 };
    int32_t ret = alipayApi->check_trans_cartstatus(&cardInfo_[index], &cardStatus);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayCheckTransCard in:%u.", ret);
    if (cardStatus.card_exits && cardStatus.remain_use_count >= ALIPAY_TRANS_CARD_MIN_VALID_NUM) {
        return 0;
    }
    return -1;
}

int32_t AlipayModel::AlipayGetTransCode(uint32_t index)
{
    cardInfo_ = AlipayTransGetCardList();
    memset_s(transCode_, ALIPAY_TRANS_CODE_MAX_LEN, 0, ALIPAY_TRANS_CODE_MAX_LEN);
    memset_s(cardTitle_, ALIPAY_TRANS_CARD_MEMBER_LEN, 0, ALIPAY_TRANS_CARD_MEMBER_LEN);
    transCodeLen_ = ALIPAY_TRANS_CODE_MAX_LEN;
    int32_t ret = alipayApi->get_transcode(&cardInfo_[index], transCode_, &transCodeLen_);
    if (ret == 0) {
        memcpy_s(cardTitle_, ALIPAY_TRANS_CARD_MEMBER_LEN, cardInfo_[index].title, ALIPAY_TRANS_CARD_MEMBER_LEN);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayCheckTransCard in:%u.", ret);
    return ret;
}

void AlipayTransGetListsCallBackProc(void *argument)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransGetListsCallBackProc in.");
    if (AlipayMainView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::GetInstance is null, do nothing.");
        return;
    }

    int32_t ret = AlipayModel::GetInstance()->AlipayGetOnlineTranslist();
    if (ret != 0) {
        return;
    }
    int16_t currentView = AlipayModel::GetInstance()->GetAlipayInterLis();
    if (currentView != ALIPAY_TRANS_ENABLE_VIEW) {
        return;
    }
    if (AlipayModel::GetInstance()->AlipayGetCardSum() == 0) {
        return;
    }
    AlipayModel::GetInstance()->AlipayTransStopGetListsTimer();
    AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ENABLE_VIEW, ALIPAY_TRANS_LIST_VIEW);
    return;
}

void AlipayTransGetListsTimerCallBack(void *argument)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlipayTransGetListsCallBackProc, argument));
}

bool AlipayModel::AlipayTransStartGetListsTimer(void)
{
    if (transTimerId == nullptr) {
        transTimerId = osTimerNew(AlipayTransGetListsTimerCallBack, osTimerPeriodic, (void *)0, nullptr);
        if (transTimerId == nullptr) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayTransStartGetLists Create timer fail.");
            return false;
        }
    }

    int32_t retcode = osTimerStart(transTimerId, ALIPAY_TRANS_STATUS_TIMER_LENGTH);
    if (retcode != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayTransStartGetLists Start timer fail");
        return false;
    }
    return true;
}
void AlipayModel::AlipayTransStopGetListsTimer(void)
{
    if (transTimerId == nullptr) {
        return;
    }
    int32_t ret = osTimerStop(transTimerId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayTransStopGetListsTimer in,%u.", ret);
    return;
}

void AlipayMsgTransListCallBackProcFunc(uint32_t actionRes)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMsgTransListCallBackProcFunc in.");
    if (AlipayMainView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::GetInstance is null, do nothing.");
        return;
    }

    if (actionRes == 0) {
        uint32_t cardSum = AlipayModel::GetInstance()->AlipayTransGetCardNum();
        AlipayModel::GetInstance()->AlipaySetCardSum(cardSum);
        if (cardSum == 0) {
            AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_ENABLE_VIEW);
        } else {
            AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_LIST_VIEW);
        }
    } else if (actionRes == ALIPAY_TRANS_CARD_LIST_NET_ERROR) {
        AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_RES_LIST);
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_RES_VIEW);
    } else {
        if (actionRes == ALIPAY_TRANS_CARD_LIST_OWN_TOO_MANY_CARD) {
            AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_TO_MUCH);
        } else {
            AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_UNKNOW);
        }
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_TIPS_VIEW);
    }
    return;
}

void AlipayMsgTransSpeCardCallBackProcFunc(uint32_t actionRes)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayMsgTransSpeCardCallBackProcFunc in.");
    if (AlipayMainView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayMainView::GetInstance is null, do nothing.");
        return;
    }
    uint32_t index = AlipayModel::GetInstance()->AlipayGetCardNum();
    if (actionRes == 0) {
        int32_t ret = AlipayModel::GetInstance()->AlipayGetTransCode(index);
        if (ret != 0) {
            AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_LIST_VIEW);
        } else {
            AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_VIEW);
        }
    } else if (actionRes == ALIPAY_TRANS_CODE_NET_ERROR) {
        AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_RES_CODE);
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_RES_VIEW);
    } else {
        if (actionRes == ALIPAY_TRANS_CODE_DATA_LIMIT) {
            AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_SECURE);
        } else if (actionRes == ALIPAY_TRANS_CODE_NOT_SUPPORT) {
            AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_NOT_SUPPORT);
        } else {
            AlipayModel::GetInstance()->AlipaySetSwitchIndex(ALIPAY_TRANS_TIPS_LIST_UNKNOW);
        }
        AlipayMainView::GetInstance()->ChangeView(ALIPAY_TRANS_ANI_VIEW, ALIPAY_TRANS_TIPS_VIEW);
    }
    return;
}

void AlipayMsgCallBack(uint32_t msgId, uint32_t actionRes)
{
    if (msgId == ALIPAY_MSG_UPDATA_TRANS_LIST) {
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlipayMsgTransListCallBackProcFunc, actionRes));
    } else if (msgId == ALIPAY_MSG_UPDATA_SPECIAL_CARD) {
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(&AlipayMsgTransSpeCardCallBackProcFunc, actionRes));
    }

    return;
}

uint32_t AlipayModel::AlipaySendMsg(uint32_t msgId)
{
    uint32_t msg = cardNum_;
    uint16_t msg_len = sizeof(uint32_t);
    errcode_t retcode = svr_alipay_msg_write(msgId, (uint8_t *)&msg, msg_len, false);
    return (uint32_t)retcode;
}

mc_alipay_trans_card_info_t *AlipayModel::AlipayTransGetCardList(void)
{
    mc_alipay_trans_context_t *alipayTransContext = svr_alipay_get_trans_context();
    return alipayTransContext->card_info;
}

int32_t AlipayModel::AlipayTransGetCardNum(void)
{
    mc_alipay_trans_context_t *alipayTransContext = svr_alipay_get_trans_context();
    return alipayTransContext->card_len;
}

bool AlipayModel::AlipayInitModel(void)
{
    transCode_ = (uint8_t *)malloc(ALIPAY_TRANS_CODE_MAX_LEN);
    if (transCode_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayInitModel transCode_ alloc failure.");
        return false;
    }
    uint32_t retCode = svr_alipay_init_trans_context();
    if (retCode != 0) {
        free(transCode_);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayInitModel svr_alipay_init_trans_context failure.");
        return false;
    }
    svr_alipay_register_cb(AlipayMsgCallBack);
    return true;
}

void AlipayModel::AlipayDeinitModel(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayModel::AlipayDeinitModel in.");
    if (transCode_ != nullptr) {
        free(transCode_);
        transCode_ = nullptr;
    }
    svr_alipay_deinit_trans_context();
    return;
}

bool AlipayModel::AlipayGetBindFlag(void)
{
    return bindFlag_;
}
void AlipayModel::AlipaySetBindFlag(bool bindFlag)
{
    bindFlag_ = bindFlag;
}

uint32_t AlipayModel::AlipayGetSwitchIndex(void)
{
    return switchIndex_;
}

void AlipayModel::AlipaySetSwitchIndex(uint32_t switchIndex)
{
    switchIndex_ = switchIndex;
}

uint32_t AlipayModel::AlipayGetRefreshFlag(void)
{
    return refreshFlag_;
}

void AlipayModel::AlipaySetRefreshFlag(uint32_t refreshFlag)
{
    refreshFlag_ = refreshFlag;
}

uint32_t AlipayModel::AlipayGetCardNum(void)
{
    return cardNum_;
}

void AlipayModel::AlipaySetCardNum(uint32_t cardNum)
{
    cardNum_ = cardNum;
}

uint32_t AlipayModel::AlipayGetCardSum(void)
{
    return cardSum_;
}

void AlipayModel::AlipaySetCardSum(uint32_t cardSum)
{
    cardSum_ = cardSum;
}

char* AlipayModel::AlipayGetCardTitle(uint32_t index)
{
    return cardInfo_[index].title;
}

uint8_t* AlipayModel::AlipayGetTransCode(void)
{
    return transCode_;
}

uint32_t AlipayModel::AlipayGetTransCodeLen(void)
{
    return transCodeLen_;
}

char* AlipayModel::AlipayGetCardTitle(void)
{
    return cardTitle_;
}

}