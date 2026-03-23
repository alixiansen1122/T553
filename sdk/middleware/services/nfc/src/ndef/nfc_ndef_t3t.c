/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: NFC NDEF process for T3T \n
 * Author: @CompanyNameTag \n
 * Date: 2023-08-24 \n
 */
#include "nfc_ndef_t3t.h"
#include <stdbool.h>
#include "securec.h"
#include "nfc_hal_core.h"
#include "nfc_hal_t3t.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VALUE_UINT8           255
#define T3T_WRITE_ON              0x0F
#define T3T_WRITE_OFF             0x00
#define T3T_DETECT_SYS_CODE       0x12FC
#define T3T_DETECT_REQ_CODE       0x01
#define T3T_POLL_TSN              3
#define T3T_NDEF_BEGIN_BLOCK_NUM  1
#define T3T_DETECT_CHECK_SUM_LEN  14
#define T3T_ATTR_INFO_RFU_LEN     4
#define T3T_ATTR_INFO_LN_LEN      3

typedef struct {
    uint8_t nfcid2[NFCID2_LEN];
    uint8_t nbr;
    uint8_t nbw;
    uint16_t nMaxB;
    uint8_t writeFlag;
    uint8_t readWriteFlag;
    uint16_t checkSum;
    uint32_t ln;
    uint8_t ver;
    bool isDetected;
} T3TAttrInfo;

typedef struct {
    bool isRead;
    uint16_t totalBlockNum;
    uint8_t blockMaxNumOnce;
    const uint8_t *writeBuff;
    uint16_t writeLen;
    uint8_t *readBuff;
    uint16_t readBuffLen;
} T3TNdefReadWrite;

static T3TAttrInfo g_t3tAttrInfo;

static uint16_t NFC_NDEF_T3TGetCheckSum(uint8_t *nums, uint16_t checkSumLen)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < checkSumLen; ++i) {
        sum += nums[i];
    }

    return sum;
}

static NfcErrorCode NFC_NDEF_T3TUpdateAttrInfo(uint8_t writeFlag, uint32_t ln)
{
    NfcHalT3tCmdParams updateCmd = {
        .numberOfServices = 1,
        .serviceCodeList = {0x09, 0x00}, // service 0x0009
        .numberOfBlock = 1,
        .blockList = {0x80, 0x00}, // block 0:Attribute Information Block
        .blockListLen = 0x02,
    };
    NfcHalT3tUpdateRspParams updateRsp;
    if (memcpy_s(updateCmd.nfcid2, NFCID2_LEN, g_t3tAttrInfo.nfcid2, NFCID2_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t updateAttrInfo[T3T_BLOCK_SIZE];
    (void)memset_s(&updateAttrInfo, T3T_BLOCK_SIZE, 0x00, T3T_BLOCK_SIZE);
    uint8_t idx = 0;
    updateAttrInfo[idx++] = g_t3tAttrInfo.ver;
    updateAttrInfo[idx++] = g_t3tAttrInfo.nbr;
    updateAttrInfo[idx++] = g_t3tAttrInfo.nbw;
    updateAttrInfo[idx++] = UTIL_Byte1(g_t3tAttrInfo.nMaxB);
    updateAttrInfo[idx++] = UTIL_Byte0(g_t3tAttrInfo.nMaxB);
    idx += T3T_ATTR_INFO_RFU_LEN;
    updateAttrInfo[idx++] = writeFlag;
    updateAttrInfo[idx++] = g_t3tAttrInfo.readWriteFlag;
    updateAttrInfo[idx++] = UTIL_Byte2(ln);
    updateAttrInfo[idx++] = UTIL_Byte1(ln);
    updateAttrInfo[idx++] = UTIL_Byte0(ln);
    uint16_t checkSum = NFC_NDEF_T3TGetCheckSum(updateAttrInfo, T3T_DETECT_CHECK_SUM_LEN);
    updateAttrInfo[idx++] = UTIL_Byte1(checkSum);
    updateAttrInfo[idx++] = UTIL_Byte0(checkSum);

    return NFC_HAL_T3TPollUpdateCmd(&updateCmd, &updateRsp, updateAttrInfo, T3T_BLOCK_SIZE);
}

static void NFC_NDEF_T3TEncodeBlockInfo(NfcHalT3tCmdParams *cmd, uint16_t blockIdx, uint8_t blockNum)
{
    cmd->numberOfBlock = blockNum;
    uint8_t blockListIdx = 0;
    uint16_t idx = blockIdx;
    for (uint8_t i = 0; i < blockNum; ++i) {
        if (idx + T3T_NDEF_BEGIN_BLOCK_NUM > MAX_VALUE_UINT8) {
            cmd->blockList[blockListIdx++] = 0x00;
            cmd->blockList[blockListIdx++] = UTIL_Byte1(idx + T3T_NDEF_BEGIN_BLOCK_NUM);
            cmd->blockList[blockListIdx++] = UTIL_Byte0(idx + T3T_NDEF_BEGIN_BLOCK_NUM);
        } else {
            cmd->blockList[blockListIdx++] = 0x80;
            cmd->blockList[blockListIdx++] = idx + T3T_NDEF_BEGIN_BLOCK_NUM;
        }
        idx++;
    }
    cmd->blockListLen = blockListIdx;
}

static NfcErrorCode NFC_NDEF_T3TWriteNdef(const uint8_t *writeBuff, uint16_t writeLen)
{
    uint16_t totalBlockNum = writeLen / T3T_BLOCK_SIZE;
    totalBlockNum = writeLen % T3T_BLOCK_SIZE == 0 ? totalBlockNum : totalBlockNum + 1;
    uint8_t blockMaxNum = UTIL_Min(g_t3tAttrInfo.nbw, T3T_BLOCKS_MAX_NUM);

    NfcErrorCode ret;
    if (totalBlockNum > blockMaxNum) {
        ret = NFC_NDEF_T3TUpdateAttrInfo(T3T_WRITE_ON, g_t3tAttrInfo.ln);
        if (ret != NFC_OK) {
            return ret;
        }
    }

    NfcHalT3tCmdParams cmd = {.numberOfServices = 1, .serviceCodeList = {0x09, 0x00}, };   /* service code 0x0009 */
    if (memcpy_s(cmd.nfcid2, NFCID2_LEN, g_t3tAttrInfo.nfcid2, NFCID2_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    uint16_t blockIdx = 0;
    uint16_t buffOffset = 0;
    do {
        uint8_t currentBlockNum = UTIL_Min(totalBlockNum - blockIdx, blockMaxNum);
        NFC_NDEF_T3TEncodeBlockInfo(&cmd, blockIdx, currentBlockNum);

        NfcHalT3tUpdateRspParams rsp;
        /* 判断是否为最后一个block */
        uint16_t currentWriteLen = currentBlockNum == blockMaxNum ?
                                   currentBlockNum * T3T_BLOCK_SIZE : writeLen - buffOffset;

        ret = NFC_HAL_T3TPollUpdateCmd(&cmd, &rsp, writeBuff + buffOffset, currentWriteLen);
        if (ret != NFC_OK) {
            return ret;
        }

        blockIdx += currentBlockNum;
        buffOffset = blockIdx * T3T_BLOCK_SIZE;
    } while (blockIdx < totalBlockNum);

    return NFC_NDEF_T3TUpdateAttrInfo(T3T_WRITE_OFF, writeLen);
}

static NfcErrorCode NFC_NDEF_T3TReadNdef(uint8_t *readBuff, uint16_t *readLen)
{
    uint8_t blockMaxNum = UTIL_Min(g_t3tAttrInfo.nbr, T3T_BLOCKS_MAX_NUM);
    uint8_t totalBlockNum = g_t3tAttrInfo.ln / T3T_BLOCK_SIZE;
    totalBlockNum = g_t3tAttrInfo.ln % T3T_BLOCK_SIZE == 0 ? totalBlockNum : totalBlockNum + 1;

    NfcHalT3tCmdParams cmd = {.numberOfServices = 1, .serviceCodeList = {0x0B, 0x00}, };  /* service code 0x000B */
    if (memcpy_s(cmd.nfcid2, NFCID2_LEN, g_t3tAttrInfo.nfcid2, NFCID2_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    uint16_t blockIdx = 0;
    uint16_t buffOffset = 0;
    do {
        uint8_t currentBlockNum = UTIL_Min(totalBlockNum - blockIdx, blockMaxNum);
        NFC_NDEF_T3TEncodeBlockInfo(&cmd, blockIdx, currentBlockNum);

        NfcHalT3tCheckRspParams rsp;
        NfcErrorCode ret = NFC_HAL_T3TPollCheckCmd(&cmd, &rsp);
        if (ret != NFC_OK) {
            return ret;
        }
        if (memcpy_s(readBuff + buffOffset, *readLen - buffOffset,
                     rsp.blockData, rsp.numberOfBlock * T3T_BLOCK_SIZE) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }

        blockIdx += currentBlockNum;
        buffOffset = blockIdx * T3T_BLOCK_SIZE;
    } while (blockIdx < totalBlockNum);

    *readLen = g_t3tAttrInfo.ln;
    return NFC_OK;
}

static NfcErrorCode NFC_HAL_T3TDecodeAttrInfo(uint8_t *attrInfoBlk)
{
    uint8_t idx = 0;
    g_t3tAttrInfo.ver = attrInfoBlk[idx++];
    g_t3tAttrInfo.nbr = attrInfoBlk[idx++];
    g_t3tAttrInfo.nbw = attrInfoBlk[idx++];
    g_t3tAttrInfo.nMaxB = UTIL_Combine8bitTo16bit(attrInfoBlk[idx], attrInfoBlk[idx + 1]);
    idx += sizeof(uint16_t);
    idx += T3T_ATTR_INFO_RFU_LEN;
    g_t3tAttrInfo.writeFlag = attrInfoBlk[idx++];
    g_t3tAttrInfo.readWriteFlag = attrInfoBlk[idx++];
    g_t3tAttrInfo.ln = UTIL_Combine8bitTo32bit(0, attrInfoBlk[idx],
                                               attrInfoBlk[idx + 1], attrInfoBlk[idx + 2]); // 1,2:byte1,2
    idx += T3T_ATTR_INFO_LN_LEN;
    g_t3tAttrInfo.checkSum = UTIL_Combine8bitTo16bit(attrInfoBlk[idx], attrInfoBlk[idx + 1]);

    if (NFC_NDEF_T3TGetCheckSum(attrInfoBlk, T3T_DETECT_CHECK_SUM_LEN) != g_t3tAttrInfo.checkSum) {
        return NFC_ERR_CODE_T3T_CHECK_ATTRIBUTE_CHECK_SUM_ERR;
    }

    g_t3tAttrInfo.isDetected = true;
    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T3TDetectNdef(void)
{
    NfcHalT3tPollCmdParams pollCmd = {
        .sc = T3T_DETECT_SYS_CODE,
        .rc = T3T_DETECT_REQ_CODE,
        .tsn = T3T_POLL_TSN,
        .timeout = NCI_READ_DATA_TO,
    };
    NfcHalT3tSensfRes ntf;
    NfcErrorCode ret = NFC_HAL_NciT3TPolling(&pollCmd, &ntf);
    if (ret != NFC_OK) {
        return ret;
    }
    bool isCardExist = false;
    for (uint8_t i = 0; i < ntf.resNum; ++i) {
        if (memcmp(ntf.resParams[i].nfcid2, g_t3tAttrInfo.nfcid2, NFCID2_LEN) == 0) {
            isCardExist = true;
            break;
        }
    }
    if (!isCardExist) {
        return NFC_ERR_CODE_T3T_DETECT_TARGET_CARD_LOST;
    }

    NfcHalT3tCmdParams checkCmd = {
        .numberOfServices = 1,
        .serviceCodeList = {0x0B, 0x00}, // service 0x000B
        .numberOfBlock = 1,
        .blockList = {0x80, 0x00}, // block 0:Attribute Information Block
        .blockListLen = 0x02,
    };
    NfcHalT3tCheckRspParams rsp;
    if (memcpy_s(checkCmd.nfcid2, NFCID2_LEN, g_t3tAttrInfo.nfcid2, NFCID2_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    ret = NFC_HAL_T3TPollCheckCmd(&checkCmd, &rsp);
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_HAL_T3TDecodeAttrInfo(rsp.blockData);
}

NfcErrorCode NFC_NDEF_T3TResetProc(const NfcTechFParams *param)
{
    memset_s(&g_t3tAttrInfo, sizeof(g_t3tAttrInfo), 0, sizeof(g_t3tAttrInfo));

    if (memcpy_s(g_t3tAttrInfo.nfcid2, NFCID2_LEN, param->sensfRes, NFCID2_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_NDEF_T3TReadProc(uint8_t *readBuff, uint16_t *readLen)
{
    // NDEF Detect
    if (!g_t3tAttrInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T3TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Read
    if (g_t3tAttrInfo.ln == 0) {
        return NFC_ERR_CODE_NDEF_PROC_READ_EMPTY_NDEF;
    }
    if (g_t3tAttrInfo.ln > *readLen) {
        return NFC_ERR_CODE_NDEF_PROC_READ_BUFF_SHORTAGE;
    }
    if (g_t3tAttrInfo.writeFlag == T3T_WRITE_ON) {
        return NFC_ERR_CODE_NDEF_PROC_READ_WHILE_WRITE_ON;
    }
    return NFC_NDEF_T3TReadNdef(readBuff, readLen);
}

NfcErrorCode NFC_NDEF_T3TWriteProc(const uint8_t *writeBuff, uint16_t writeLen)
{
    // NDEF Detect
    if (!g_t3tAttrInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T3TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Read
    if (g_t3tAttrInfo.readWriteFlag == 0) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_TLV_READ_ONLY;
    }
    if (g_t3tAttrInfo.nMaxB * T3T_BLOCK_SIZE < writeLen) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_LEN_OVERFLOW;
    }
    return NFC_NDEF_T3TWriteNdef(writeBuff, writeLen);
}

#ifdef __cplusplus
}
#endif
