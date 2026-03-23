/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay model.
 * Author:
 * Create:
 */

#ifndef ALIPAY_MODEL_H
#define ALIPAY_MODEL_H

#include "ohos_types.h"
#include "list.h"
#include "source.h"
#include "cmsis_os.h"
#include "ChangeSliceListener.h"
#include "alipay_feature.h"

namespace OHOS {
void AlipayBindTimerCallBack(void *argument);
void AlipaySwitchTimerCallBack(void *argument);
void AlipayBindCallBackProc(void *argument);
void AlipaySwitchTimerCallBackProc(void *argument);
void AlipayMsgCallBack(uint32_t msgId, uint32_t actionRes);

#define ALIPAY_BIND_STATUS_TIMER_LENGTH 2000
#define ALIPAY_GET_BIND_STR_MAX_LENGTH 256
#define ALIPAY_GET_PAY_STR_MAX_LENGTH 20
#define ALIPAY_GET_AID_STR_MAX_LENGTH 256
#define ALIPAY_GET_ID_STR_MAX_LENGTH 128
#define ALIPAY_GET_NICKNAME_MAX_LENGTH 128

#define ALIPAY_TRANS_READ_LOCAL_ERR 5
#define ALIPAY_TRANS_READ_BUFF_SHORT 15
#define ALIPAY_TRANS_READ_NETWORK_ERR 7

#define ALIPAY_TRANS_CODE_LIMITED 40
#define ALIPAY_TRANS_CODE_UNSUPPORTED 43
#define ALIPAY_TRANS_CODE_INVALID 54
#define ALIPAY_TRANS_CODE_COMMON_ERR 56
#define ALIPAY_TRANS_NETWORK_ERR 5
#define ALIPAY_TRANS_STATUS_TIMER_LENGTH 10000

class AlipayModel {
public:
    AlipayModel();
    virtual ~AlipayModel();
    static AlipayModel *GetInstance(void);

    bool AlipayInitModel(void);
    void AlipayDeinitModel(void);
    void SetAlipayInterLis(int16 interListen);
    int16 GetAlipayInterLis(void);
    bool AlipayGetBindstate(void);
    int32_t AlipayGetBindCode(uint8_t *bindStr);
    int32_t AlipayGetPayCode(uint8_t *payStr);
    int32_t AlipayGetNickName(uint8_t *nickName);
    int32_t AlipayGetIdName(uint8_t *idStr);
    int32_t AlipayGetAidCode(int8_t *aidStr);
    void AlipayUnbind(void);
    bool AlipayStartBind(void);
    bool AlipayFinishBind(void);
    bool AlipayStartSwitchTimer(uint16_t time);
    void AlipayStopBindTimer(void);
    void AlipayStopSwitchTimer();
    void AlipayEnterLpm();
    void AlipayExitLpm();
    int32_t AlipayGetLastTranscode();
    int32_t AlipayGetLocalTranslist();
    int32_t AlipayGetOnlineTranslist();
    int32_t AlipayUpdateTransCard(uint32_t index);
    int32_t AlipayCheckTransCard(uint32_t index);
    int32_t AlipayGetTransCode(uint32_t index);
    bool AlipayTransStartGetListsTimer(void);
    void AlipayTransStopGetListsTimer(void);
    uint32_t AlipaySendMsg(uint32_t msgId);
    mc_alipay_trans_card_info_t* AlipayTransGetCardList(void);
    int32_t AlipayTransGetCardNum(void);
    bool AlipayGetBindFlag(void);
    void AlipaySetBindFlag(bool bindFlag);
    uint32_t AlipayGetSwitchIndex(void);
    void AlipaySetSwitchIndex(uint32_t switchIndex);
    uint32_t AlipayGetRefreshFlag(void);
    void AlipaySetRefreshFlag(uint32_t refreshFlag);
    uint32_t AlipayGetCardNum(void);
    void AlipaySetCardNum(uint32_t cardNum);
    uint32_t AlipayGetCardSum(void);
    void AlipaySetCardSum(uint32_t cardSum);
    char* AlipayGetCardTitle(uint32_t index);
    uint8_t* AlipayGetTransCode(void);
    uint32_t AlipayGetTransCodeLen(void);
    char* AlipayGetCardTitle(void);
private:
    osTimerId_t bindTimerId = nullptr;
    osTimerId_t switchTimerId = nullptr;
    osTimerId_t transTimerId = nullptr;
    int16 alipayInterLis;
    const alipay_svr_api_t *alipayApi;
    bool bindFlag_;
    uint32_t switchIndex_ = 0;
    uint32_t refreshFlag_ = 0;
    uint32_t cardNum_ = 0;
    uint32_t cardSum_ = 0;
    mc_alipay_trans_card_info_t *cardInfo_ = nullptr;
    uint8_t* transCode_ = nullptr;
    uint32_t transCodeLen_;
    char cardTitle_[ALIPAY_TRANS_CARD_MEMBER_LEN];
};
}

#endif