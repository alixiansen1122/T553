/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc mxx ce example \n
 * Author: @CompanyNameTag \n
 * Date: 2025-03-12 \n
 */

#include "example_mxxce.h"
#include "nfc_hal_core.h"
#include "nfc_hal_mxxce.h"
#include "nfc_hal_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_DISCOVERY_TIME 5000
#define MXX_CARD_NUM 20

uint8_t g_mxxCard[MXX_CARD_NUM][MXX_MEM_SIZE] = {0};

NfcErrorCode MxxceFieldOnEvtHandle(void)
{
    PS_PRINT_INFO("[MXXCE] field on event!\n");
    return NFC_OK;
}

NfcErrorCode MxxceFieldOffEvtHandle(void)
{
    PS_PRINT_INFO("[MXXCE] field off event!\n");
    return NFC_OK;
}

NfcErrorCode MxxceCardUpdateEvtHandle(void)
{
    PS_PRINT_INFO("[MXXCE] card update event!\n");
    return NFC_OK;
}

NfcErrorCode MxxceResetEvtHandle(void)
{
    PS_PRINT_INFO("[MXXCE] reset event!\n");
    return NFC_OK;
}

NfcErrorCode MxxCeEventHandle(NfcMxxCeEvent evtId)
{
    NfcErrorCode ret = NFC_OK;
    switch (evtId) {
        case NFC_MXXCE_EVENT_FIELD_ON:
            ret = MxxceFieldOnEvtHandle();
            break;
        case NFC_MXXCE_EVENT_FIELD_OFF:
            ret = MxxceFieldOffEvtHandle();
            break;
        case NFC_MXXCE_EVENT_CARD_UPDATED:
            ret = MxxceCardUpdateEvtHandle();
            break;
        case NFC_MXXCE_EVENT_RESET:
            ret = MxxceResetEvtHandle();
            break;
        case NFC_MXXCE_EVENT_NONE:
        default:
            break;
    }
    return ret;
}

void NFC_EXAMPLE_ProcessMxxCardEmuOnce(uint32_t timeout)
{
    uint8_t recvBuff[HAL_MAX_BUF_LEN] = {0};
    uint16_t len = HAL_MAX_BUF_LEN;
    NfcErrorCode ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, timeout);
    if (ret != NFC_OK || len < NCI_HEAD_LEN) {
        return;
    }
    ret = NFC_HAL_MxxCeMessageProc(recvBuff, len);
    if (ret != NFC_OK) {
        return;
    }

    uint32_t readTimeout = 1000;
    do {
        len = HAL_MAX_BUF_LEN;
        ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, readTimeout);
        if (ret != NFC_OK || len < NCI_HEAD_LEN) {
            return;
        }
        ret = NFC_HAL_MxxCeMessageProc(recvBuff, len);
        if (ret != NFC_OK) {
            return;
        }
    } while (ret == NFC_OK && len > NCI_HEAD_LEN);

    return;
}

NfcErrorCode NFC_EXAMPLE_StartMxxCardEmulation(void)
{
    for (uint8_t i = 0; i < MXX_CARD_NUM; i++) {
        NFC_HAL_MxxCeInitCard(i, g_mxxCard[i], MXX_MEM_SIZE, true);
    } // 设置卡片0-2为空白卡

    NFC_HAL_MxxCeRegisterCallback(MxxCeEventHandle); // 注册回调函数

    NfcErrorCode ret = NFC_HAL_MxxCeModeInit(); // 配置NFCC为Mxx卡模拟模式
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeSwitchCardByIndex(0); // 切至卡0，并下载卡片信息给NFCC
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeStart();
    if (ret != NFC_OK) {
        return ret;
    }

    NFC_EXAMPLE_ProcessMxxCardEmuOnce(NFC_DISCOVERY_TIME); // 开始业务

    ret = NFC_HAL_MxxCeUploadCurrentCard(); // 要求NFCC上传卡片0信息到host
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeStop(); // 切卡前停止业务
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeSwitchCardByIndex(1); // 切至卡片1
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t currentCardIndex = NFC_HAL_MxxCeGetCurrentCardIndex(); // 当前卡为卡片1
    PS_PRINT_INFO("[MXXCE] current card index[%x]", currentCardIndex);

    ret = NFC_HAL_MxxCeStart(); // 开启卡片1卡模拟业务
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeStop(); // 删除卡片前停止业务
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeRemoveCardByIndex(0); // 删除卡片0
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_MxxCeRemoveAllCards(); // 删除所有卡片
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_HAL_EnterIdleSleep();  // 进入睡眠模式
}

#ifdef __cplusplus
}
#endif