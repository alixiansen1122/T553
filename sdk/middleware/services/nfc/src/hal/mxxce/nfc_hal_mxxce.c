/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc mxx ce operations \n
 * Author: @CompanyNameTag \n
 * Date: 2025-03-05 \n
 */

#include "nfc_hal_mxxce.h"
#include <stdbool.h>
#include "nfc_hal_core.h"
#include "securec.h"
#include "nfc_hal_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif
#define MXX_SECTOR_NUM 16
#define MXX_BLOCK_LEN 16
#define MXX_SECTOR_SIZE 64
#define NFC_PROP_CMD_HEAD 0x2F
#define MXX_DOWNLOAD_CMD_OID 0x0D
#define MXX_UPLOAD_CMD_LEN 0x03
#define MXX_UPLOAD_CMD_OID 0x0E
#define MXX_CMD_DELAY_TIME 50
#define MXX_UID0_DEFAULT_VALUE 0x08
#define MXX_NFCEE_ID 0x12
#define NFC_FIELD_INFO_NTF_LEN 4
#define UNRECOVERABLE_ERROR 0

// 级联帧类型定义
#define MXX_END 0
#define MXX_MORE_DATA 1
#define MXX_DELETE_CARD 2

MxxCeFeatureCtx g_mxxCeCtx = {0};

uint8_t g_fieldOnNtf[NFC_FIELD_INFO_NTF_LEN] = {0x61, 0x07, 0x01, 0x01};
uint8_t g_fieldOffNtf[NFC_FIELD_INFO_NTF_LEN] = {0x61, 0x07, 0x01, 0x00};
uint8_t g_UploadNtfHead[NCI_HEAD_LEN - 1] = {0x6F, 0x0E};
uint8_t g_ResetNtfHead[NCI_HEAD_LEN - 1] = {0x60, 0x00};

MxxCeFeatureCtx *NFC_HAL_GetMxxCeCtx(void)
{
    return &g_mxxCeCtx;
}

bool NFC_HAL_MxxCeIsValidCard(uint8_t index)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    return mxxCeCtx->mxxCardsInfo[index].cardValid;
}

NfcErrorCode NFC_HAL_MxxCeModeInit(void)
{
    NfcErrorCode ret = NFC_HAL_NciReset();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_NciInit();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_CfgLowPwr(true);
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t recvShortBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    rspLen = HAL_SHORT_BUFF_LEN;
    // 参数1：SWP_EN(0xA300) -> 0x08 使能SWP，bit3表示mxx nfcee
    // 参数2：RF_FIELD_INFO(0x80) -> 0x01 使能场上报
    uint8_t cfgDbCmd[] = {0x20, 0x02, 0x08, 0x02, 0xA3, 0x00, 0x01, 0x08, 0x80, 0x01, 0x01};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(cfgDbCmd, sizeof(cfgDbCmd), recvShortBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    // NFCEE discovery, get available nfcee list
    uint8_t nfceeList[HAL_SHORT_BUFF_LEN] = {0};
    uint8_t num = 0;
    ret = NFC_HAL_NfceeDiscovery(nfceeList, HAL_SHORT_BUFF_LEN, &num);
    if (ret != NFC_OK) {
        return ret;
    } else if (num == 0) {
        return NFC_ERR_CODE_NFCEE_DISCOV_NOT_FOUND_ERR;
    }

    uint8_t routeCmd[] = {0x21, 0x01, 0x09, 0x00, 0x01, 0x00, 0x03, MXX_NFCEE_ID, 0x3B, 0x00, 0x03, 0x03};
    uint16_t idx = sizeof(routeCmd);
    routeCmd[NCI_LEN_POS] = idx - NCI_HEAD_LEN;
    ret = NFC_HAL_SetListenRouteTbl(routeCmd, idx);
    if (ret != NFC_OK) {
        return ret;
    }

    // NFCEE enable, example card emulation only enable first nfcee
    uint8_t techModeList[HAL_SHORT_BUFF_LEN] = {0};
    num = 0;
    // Handle RF discov request, HCI pipe cmd, and nfcee enable ntf
    ret = NFC_HAL_NfceeEnable(MXX_NFCEE_ID, techModeList, HAL_SHORT_BUFF_LEN, &num);
    if (ret != NFC_OK) {
        return ret;
    } else if (num == 0) {
        return NFC_ERR_CODE_NFCEE_ENABLE_RF_DISCOV_REQ_NOT_FOUND_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_MxxCeStart(void)
{
    uint8_t list[] = {NFC_NCI_A_PASSIVE_LISTEN_MODE};
    return NFC_HAL_StartDiscovery(list, sizeof(list));
}

NfcErrorCode NFC_HAL_MxxCeStop(void)
{
    return NFC_HAL_Deactivate();
}

NfcErrorCode NFC_HAL_MxxCeInitCard(uint8_t index, uint8_t *mem, uint16_t memSize, bool isBlank)
{
    if (index >= MAX_MXX_CARD_NUM || mem == NULL || memSize != MXX_MEM_SIZE) {
        return NFC_ERR_CODE_MXX_PROC_INPUT_PARAM_INVALID;
    }
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    if (isBlank) {
        if (memset_s(mem, memSize, 0, MXX_MEM_SIZE) != EOK) {
            return NFC_ERR_CODE_MEMSET_FAIL;
        }
        uint8_t defaultSectorTrailer[MXX_BLOCK_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x80, 0x69, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        for (uint8_t sector = 0; sector < MXX_SECTOR_NUM; sector++) {
            uint16_t start = sector * MXX_SECTOR_SIZE + MXX_BLOCK_LEN * 0x3; // 每个sector第四个block为尾块
            if (memcpy_s(&mem[start], memSize - start, defaultSectorTrailer, MXX_BLOCK_LEN) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
        }
    }

    mxxCeCtx->mxxCardsInfo[index].cardMem = mem;
    mxxCeCtx->mxxCardsInfo[index].cardValid = true;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_MxxCeSwitchCardByIndex(uint8_t index)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    if (index >= MAX_MXX_CARD_NUM || mxxCeCtx->mxxCardsInfo[index].cardMem == NULL ||
        !(mxxCeCtx->mxxCardsInfo[index].cardValid)) {
        return NFC_ERR_CODE_MXX_CARD_INVALID;
    }
    mxxCeCtx->currentCardIndex = index;
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret;
    uint8_t *start = mxxCeCtx->mxxCardsInfo[index].cardMem;
    uint16_t offset = 0;
    uint16_t remainLen = MXX_MEM_SIZE;
    uint8_t downloadMxxBySectorCmd[HAL_MAX_BUF_LEN] = {0};
    while (remainLen > 0) {
        uint8_t copyLen = remainLen > 0xFC ? 0xFC : remainLen;
        uint16_t i = 0;
        downloadMxxBySectorCmd[i++] = NFC_PROP_CMD_HEAD;
        downloadMxxBySectorCmd[i++] = MXX_DOWNLOAD_CMD_OID;
        downloadMxxBySectorCmd[i++] = copyLen + 3; // 前3字节为级联指示位和两字节offset
        downloadMxxBySectorCmd[i++] = remainLen > 0xFC ? MXX_MORE_DATA : MXX_END;
        downloadMxxBySectorCmd[i++] = (uint8_t)offset;
        downloadMxxBySectorCmd[i++] = (uint8_t)(offset >> 0x8);
        if (memcpy_s(&downloadMxxBySectorCmd[i], HAL_MAX_BUF_LEN - i, start, copyLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        i += copyLen;
        rspLen = HAL_SHORT_BUFF_LEN;
        ret = NFC_HAL_SendNciCmdAndRecvRsp(downloadMxxBySectorCmd, i, recvBuff, &rspLen);
        if (ret != NFC_OK) {
            return ret;
        }
        offset += copyLen;
        start += copyLen;
        remainLen -= copyLen;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_MxxCeUploadNtfProc(uint8_t *data)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    uint8_t index = mxxCeCtx->currentCardIndex;
    if (index >= MAX_MXX_CARD_NUM || mxxCeCtx->mxxCardsInfo[index].cardMem == NULL) {
        return NFC_ERR_CODE_MXX_CARD_INVALID;
    }
    uint8_t copyLen = data[0x02] - 3; // 前3字节为级联指示位和两字节offset
    uint16_t offset = ((uint16_t)data[0x05] << 0x8) | data[0x04];
    if (offset + copyLen > MXX_MEM_SIZE) {
        return NFC_ERR_CODE_MXX_CARD_INVALID;
    }
    if (data[0x03] == MXX_MORE_DATA) {
        if (memcpy_s(&mxxCeCtx->mxxCardsInfo[index].cardMem[offset], MXX_MEM_SIZE - offset,
            &data[0x06], copyLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    } else if (data[0x03] == MXX_END) {
        if (memcpy_s(&mxxCeCtx->mxxCardsInfo[index].cardMem[offset], MXX_MEM_SIZE - offset,
            &data[0x06], copyLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    } else {
        return NFC_ERR_CODE_MXX_PROC_INPUT_PARAM_INVALID;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_MxxCeUploadCurrentCard(void)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    uint8_t curCardIndex = mxxCeCtx->currentCardIndex;
    if (curCardIndex >= MAX_MXX_CARD_NUM || mxxCeCtx->mxxCardsInfo[curCardIndex].cardMem == NULL) {
        return NFC_ERR_CODE_MXX_CARD_INVALID;
    }
    uint8_t recvBuff[HAL_MAX_BUF_LEN];
    uint16_t rspLen = HAL_MAX_BUF_LEN;
    NfcErrorCode ret;
    uint8_t upLoadMxxBySectorCmd[MXX_UPLOAD_CMD_LEN] = {0};
    uint8_t i = 0;
    upLoadMxxBySectorCmd[i++] = NFC_PROP_CMD_HEAD;
    upLoadMxxBySectorCmd[i++] = MXX_UPLOAD_CMD_OID;
    upLoadMxxBySectorCmd[i++] = MXX_UPLOAD_CMD_LEN - NCI_HEAD_LEN;
    
    ret = NFC_HAL_SendNciCmdAndRecvRsp(upLoadMxxBySectorCmd, sizeof(upLoadMxxBySectorCmd), recvBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }
    while (true) {
        rspLen = HAL_MAX_BUF_LEN;
        uint8_t timeout = 100; // 接收等待最多100ms
        ret = NFC_HAL_ReceiveNciPkt(recvBuff, &rspLen, timeout);
        if (ret != NFC_OK || rspLen < NCI_HEAD_LEN) {
            return ret;
        }
        ret = NFC_HAL_MxxCeUploadNtfProc(recvBuff);
        if (ret != NFC_OK) {
            return ret;
        }
        if (recvBuff[0x03] == MXX_END) {
            break;
        }
    }
    return NFC_OK;
}

uint8_t NFC_HAL_MxxCeGetCurrentCardIndex(void)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    return mxxCeCtx->currentCardIndex;
}

NfcErrorCode NFC_HAL_MxxCeRemoveCardByIndex(uint8_t index)
{
    if (index >= MAX_MXX_CARD_NUM) {
        return NFC_ERR_CODE_MXX_PROC_INPUT_PARAM_INVALID;
    }
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    mxxCeCtx->mxxCardsInfo[index].cardMem = NULL;
    mxxCeCtx->mxxCardsInfo[index].cardValid = false;
    if (mxxCeCtx->currentCardIndex == index) {
        mxxCeCtx->currentCardIndex = 0xFF;
        uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
        uint16_t rspLen = HAL_SHORT_BUFF_LEN;
        uint8_t deleteCmd[] = {NFC_PROP_CMD_HEAD, MXX_DOWNLOAD_CMD_OID, 0x01, MXX_DELETE_CARD}; // 清理NFCC存储的卡片信息
        return NFC_HAL_SendNciCmdAndRecvRsp(deleteCmd, sizeof(deleteCmd), recvBuff, &rspLen);
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_MxxCeRemoveAllCards(void)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    for (uint8_t i = 0; i < MAX_MXX_CARD_NUM; i++) {
        mxxCeCtx->mxxCardsInfo[i].cardMem = NULL;
        mxxCeCtx->mxxCardsInfo[i].cardValid = false;
    }
    mxxCeCtx->currentCardIndex = 0xFF;
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;
    uint8_t deleteCmd[] = {NFC_PROP_CMD_HEAD, MXX_DOWNLOAD_CMD_OID, 0x01, MXX_DELETE_CARD}; // 清理NFCC存储的卡片信息
    return NFC_HAL_SendNciCmdAndRecvRsp(deleteCmd, sizeof(deleteCmd), recvBuff, &rspLen);
}

void NFC_HAL_MxxCeRegisterCallback(MxxCeCallback handle)
{
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    mxxCeCtx->eventCallback = handle;
}

NfcErrorCode NFC_HAL_MxxCeMessageProc(uint8_t *data, uint16_t len)
{
    NfcMxxCeEvent evtId = NFC_MXXCE_EVENT_NONE;
    MxxCeFeatureCtx *mxxCeCtx = NFC_HAL_GetMxxCeCtx();
    if (len == NFC_FIELD_INFO_NTF_LEN && (memcmp(data, g_fieldOnNtf, NFC_FIELD_INFO_NTF_LEN) == 0)) {
        evtId = NFC_MXXCE_EVENT_FIELD_ON;
    } else if (len == NFC_FIELD_INFO_NTF_LEN && (memcmp(data, g_fieldOffNtf, NFC_FIELD_INFO_NTF_LEN) == 0)) {
        evtId = NFC_MXXCE_EVENT_FIELD_OFF;
    } else if (memcmp(data, g_UploadNtfHead, NCI_HEAD_LEN - 1) == 0) {
        NFC_HAL_MxxCeUploadNtfProc(data);
        evtId = NFC_MXXCE_EVENT_CARD_UPDATED;
    } else if (memcmp(data, g_ResetNtfHead, NCI_HEAD_LEN - 1) == 0) {
        if (data[0x03] == UNRECOVERABLE_ERROR) {
            NfcErrorCode ret = NFC_HAL_NciReset();
            if (ret != NFC_OK) {
                return ret;
            }
            ret = NFC_HAL_NciInit();
            if (ret != NFC_OK) {
                return ret;
            }
            evtId = NFC_MXXCE_EVENT_RESET;
        }
    } else {
        return NFC_OK;
    } // 其余场景暂不处理
    return mxxCeCtx->eventCallback(evtId);
}

#ifdef __cplusplus
}
#endif
