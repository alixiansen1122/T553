/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: NFC NDEF process for T5T \n
 * Author: @CompanyNameTag \n
 * Date: 2023-03-22 \n
 */
#include "nfc_ndef_t5t.h"
#include <stdbool.h>
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_hal_t5t.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T5T_CC_MAX_LEN              8
#define T5T_AREA_SIZE_MULTI         8
#define T5T_ACCESS_READ_ONLY        0x03
#define T5T_MULTI_READ_EN           0x01
#define T5T_LOCK_BLK_EN             0x08
#define T5T_SPEC_FRAME_EN           0x10
#define T5T_NDEF_LRECORD_SIGN       0xFF
#define T5T_NDEF_LRECORD_MIN_LEN    255
#define T5T_NDEF_TL_SHORT_LEN       2
#define T5T_NDEF_TL_LONG_LEN        4
#define T5T_NDEF_LFIELD_LONG_LEN    3
#define T5T_READ_TEMP_BUFF_LEN      (T5T_MAX_BLOCK_LEN * 2)

enum T5TTlvs {
    NDEF_MESSAGE_TLV    = 0x03,
    TERMINATOR_TLV      = 0xFE,
};

typedef enum {
    DETECT_SUCCESS,
    DETECT_NEW_BLOCK,
    DETECT_BYTE_LACK,
    DETECT_FULL_BUFFER,
} T5TNdefDetectResult;

typedef struct {
    uint8_t mappingVer;
    uint8_t accessCon;
    uint32_t t5tAreaSize;
    uint16_t ndefBlNo;
    uint16_t ndefBNo;
    uint16_t ndefLen;
    bool isExtended;
    bool isMultiReadEn;
    bool isLockBlkEn;
    bool isSpecFrameEn;
    bool isDetected;
} T5TNdefInfo;

typedef struct {
    uint32_t detectedLen;
    uint16_t idx;
    uint16_t firstBlNo;
    uint16_t dataLen;   // size of received data
    uint16_t buffLen;   // actual size of rxBuff
    uint8_t *rxBuff;
} T5TNdefDetectParams;

typedef struct {
    uint16_t writtenLen;
    uint16_t headLen;
    uint8_t headBuff[T5T_READ_TEMP_BUFF_LEN];
} T5TNdefWriteParams;

static T5TNdefInfo g_t5tNdefInfo;
static NfcHalT5TRWInfo g_t5tRwInfo;

static NfcErrorCode NFC_NDEF_T5TReadCC(uint16_t *offset, uint8_t *rxBuff, uint16_t buffLen)
{
    NfcHalT5TReadRes readRes = {rxBuff, buffLen, 0, 0, 0, 0};
    NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T5TDetectNdef read first block failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
    }

    uint16_t blkOffset = *offset;
    g_t5tNdefInfo.isExtended = (rxBuff[blkOffset] == T5T_2BYTE_ADDR_MODE_SUPPORT);
    blkOffset++;
    g_t5tNdefInfo.mappingVer = ((rxBuff[blkOffset] & 0xF0) >> 0x4); // mapping version is higher 4 bits
    g_t5tNdefInfo.accessCon = (rxBuff[blkOffset] & 0x0F);
    blkOffset++;
    g_t5tNdefInfo.t5tAreaSize = rxBuff[blkOffset];
    blkOffset++;
    g_t5tNdefInfo.isMultiReadEn = (rxBuff[blkOffset] & T5T_MULTI_READ_EN) != 0;
    g_t5tNdefInfo.isLockBlkEn = (rxBuff[blkOffset] & T5T_LOCK_BLK_EN) != 0;
    g_t5tNdefInfo.isSpecFrameEn = (rxBuff[blkOffset] & T5T_SPEC_FRAME_EN) != 0;
    blkOffset++;
    if (g_t5tNdefInfo.t5tAreaSize == 0) {
        if (g_t5tRwInfo.blockLen < T5T_CC_MAX_LEN) {
            g_t5tRwInfo.blockNo++;
            ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T5TDetectReadCC read second block failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
            }
            blkOffset = 0;
        }
        blkOffset += 2;  // skip 2 bytes for RFU
        g_t5tNdefInfo.t5tAreaSize = UTIL_Combine8bitTo16bit(rxBuff[blkOffset], rxBuff[blkOffset + 1]);
        blkOffset += sizeof(uint16_t);
    }
    g_t5tNdefInfo.t5tAreaSize *= T5T_AREA_SIZE_MULTI;
    g_t5tRwInfo.blockNo += blkOffset / g_t5tRwInfo.blockLen;
    blkOffset = blkOffset % g_t5tRwInfo.blockLen;
    *offset = blkOffset;

    return NFC_OK;
}

static T5TNdefDetectResult NFC_NDEF_T5TSearchNdefTlv(T5TNdefDetectParams *param)
{
    T5TNdefDetectResult result = DETECT_NEW_BLOCK;
    uint16_t tempIdx = param->idx;
    while (tempIdx < param->dataLen && param->rxBuff[tempIdx] != NDEF_MESSAGE_TLV) {
        if (tempIdx + sizeof(uint8_t) >= param->dataLen) {
            result = DETECT_BYTE_LACK;
            break;
        }
        tempIdx++;
        if (param->rxBuff[tempIdx] < T5T_NDEF_LRECORD_MIN_LEN) {
            tempIdx += param->rxBuff[tempIdx] + 1;
        } else {
            if (tempIdx + sizeof(uint16_t) >= param->dataLen) {
                tempIdx--;  // if byte-lack, the next reading always starts at T-Field.
                result = DETECT_BYTE_LACK;
                break;
            }
            tempIdx++;
            tempIdx += UTIL_Combine8bitTo16bit(param->rxBuff[tempIdx], param->rxBuff[tempIdx + 1]) + sizeof(uint16_t);
        }
    }
    if (result == DETECT_BYTE_LACK && param->dataLen + g_t5tRwInfo.blockLen > param->buffLen) {
        result = DETECT_FULL_BUFFER;
    }

    if (result == DETECT_BYTE_LACK) {
        g_t5tRwInfo.blockNo++; // if byte-lack but not full-buffer, do not update position.
    } else {
        param->detectedLen += tempIdx - param->idx;
        param->firstBlNo += tempIdx / g_t5tRwInfo.blockLen;
        g_t5tRwInfo.blockNo = param->firstBlNo;
        if (tempIdx < param->dataLen && param->rxBuff[tempIdx] == NDEF_MESSAGE_TLV) {
            g_t5tNdefInfo.ndefBlNo = g_t5tRwInfo.blockNo;
            g_t5tNdefInfo.ndefBNo = tempIdx % g_t5tRwInfo.blockLen;
            g_t5tNdefInfo.isDetected = true;
            result = DETECT_SUCCESS;
        }
        tempIdx %= g_t5tRwInfo.blockLen;
        param->idx = tempIdx;
    }

    return result;
}

static NfcErrorCode NFC_NDEF_T5TDetectNdefTlvUpdateInfo(T5TNdefDetectResult result, T5TNdefDetectParams *param,
                                                        NfcHalT5TReadRes *readRes, uint8_t *rxBuff, uint16_t buffLen)
{
    if (result == DETECT_NEW_BLOCK) {
        return NFC_OK;
    }
    if (result == DETECT_FULL_BUFFER) {
        uint16_t currentHead = param->dataLen - g_t5tRwInfo.blockLen;
        if (memcpy_s(rxBuff, buffLen, rxBuff + currentHead, g_t5tRwInfo.blockLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        param->dataLen = g_t5tRwInfo.blockLen;
    }
    readRes->data += param->dataLen;
    readRes->dataBuffSize -= param->dataLen;
    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TDetectNdefTlv(uint16_t offset, uint8_t *rxBuff, uint16_t buffLen)
{
    T5TNdefDetectParams param = {0, offset, g_t5tRwInfo.blockNo,
                                 offset == 0 ? 0 : g_t5tRwInfo.blockLen, buffLen, rxBuff};

    while (!g_t5tNdefInfo.isDetected && param.detectedLen < g_t5tNdefInfo.t5tAreaSize) {
        T5TNdefDetectResult result = NFC_NDEF_T5TSearchNdefTlv(&param);
        if (result == DETECT_SUCCESS) {
            break;
        }
        NfcHalT5TReadRes readRes = {rxBuff, buffLen, 0, 0, 0, 0};
        NfcErrorCode ret = NFC_NDEF_T5TDetectNdefTlvUpdateInfo(result, &param, &readRes, rxBuff, buffLen);
        if (ret != NFC_OK) {
            return ret;
        }
        ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T5TDetectReadTLVs read failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_SEARCH_NDEF;
        }
        if (result == DETECT_NEW_BLOCK) {
            param.dataLen = g_t5tRwInfo.blockLen;
        } else {
            param.dataLen += g_t5tRwInfo.blockLen;
        }
    }

    if (!g_t5tNdefInfo.isDetected) {
        PS_PRINT_ERR("[NDEF] T5TDetectReadTLVs NDEF TLV not found.\n");
        return NFC_ERR_CODE_NDEF_PROC_DETECT_TLV_NOT_FOUND;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TDetectNdef(void)
{
    uint8_t rxBuff[T5T_READ_TEMP_BUFF_LEN] = {0};
    uint16_t offset = 0;

    // reading StaticLock & CC
    NfcErrorCode ret = NFC_NDEF_T5TReadCC(&offset, rxBuff, T5T_READ_TEMP_BUFF_LEN);
    if (ret != NFC_OK) {
        return ret;
    }

    // reading TLVs and find NDEF TLV
    return NFC_NDEF_T5TDetectNdefTlv(offset, rxBuff, T5T_READ_TEMP_BUFF_LEN);
}

static NfcErrorCode NFC_NDEF_T5TReadLField(uint16_t *haveReadLen, uint8_t *readBuff, uint16_t readLen)
{
    g_t5tRwInfo.blockNo = g_t5tNdefInfo.ndefBlNo;
    uint16_t byteNo = g_t5tNdefInfo.ndefBNo + 1;
    if (byteNo >= g_t5tRwInfo.blockLen) {
        g_t5tRwInfo.blockNo++;
        byteNo %= g_t5tRwInfo.blockLen;
    }
    uint8_t rxBuff[T5T_READ_TEMP_BUFF_LEN] = {0};
    NfcHalT5TReadRes readRes = {rxBuff, T5T_READ_TEMP_BUFF_LEN, 0, 0, 0, 0};
    NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T5TReadLField read first block failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_LEN;
    }
    uint16_t dataLen = g_t5tRwInfo.blockLen;
    if (rxBuff[byteNo] == T5T_NDEF_LRECORD_SIGN) {
        byteNo++;
        if (byteNo + 1 >= g_t5tRwInfo.blockLen) {
            readRes.data += g_t5tRwInfo.blockLen;
            readRes.dataBuffSize -= g_t5tRwInfo.blockLen;
            g_t5tRwInfo.blockNo++;
            ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T5TReadLField read failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_LEN;
            }
            dataLen += g_t5tRwInfo.blockLen;
        }
        g_t5tNdefInfo.ndefLen = UTIL_Combine8bitTo16bit(rxBuff[byteNo], rxBuff[byteNo + 1]);
        byteNo += sizeof(uint16_t);
    } else {
        g_t5tNdefInfo.ndefLen = rxBuff[byteNo++];
    }
    if (g_t5tNdefInfo.ndefLen == 0) {
        return NFC_ERR_CODE_NDEF_PROC_READ_EMPTY_NDEF;
    }
    if (g_t5tNdefInfo.ndefLen > readLen) {
        PS_PRINT_ERR("[NDEF] T5TReadLField readBuff shortage. ndefLen = %d\n", g_t5tNdefInfo.ndefLen);
        return NFC_ERR_CODE_NDEF_PROC_READ_BUFF_SHORTAGE;
    }
    uint16_t buffLen = (uint16_t)UTIL_Min(g_t5tNdefInfo.ndefLen, dataLen - byteNo);
    if (buffLen > 0 && memcpy_s(readBuff, readLen, &rxBuff[byteNo], buffLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    g_t5tRwInfo.blockNo++;
    *haveReadLen = buffLen;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TReadVField(uint16_t haveReadLen, uint8_t *readBuff, uint16_t readLen)
{
    // Read the rest
    while (haveReadLen < g_t5tNdefInfo.ndefLen) {
        // If not a full block
        if (g_t5tNdefInfo.ndefLen - haveReadLen < g_t5tRwInfo.blockLen) {
            uint8_t rxBuff[T5T_MAX_BLOCK_LEN] = {0};
            uint16_t rxLen = g_t5tNdefInfo.ndefLen - haveReadLen;
            NfcHalT5TReadRes readRes = {rxBuff, T5T_MAX_BLOCK_LEN, 0, 0, 0, 0};
            NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T5TReadVField read last block failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_MESSAGE;
            }
            if (memcpy_s(&readBuff[haveReadLen], readLen - haveReadLen, rxBuff, rxLen) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            haveReadLen += rxLen;
        } else {
            NfcHalT5TReadRes readRes = {readBuff + haveReadLen, readLen - haveReadLen, 0, 0, 0, 0};
            uint16_t readBlockNum = 1;
            NfcErrorCode ret = NFC_OK;
            if (g_t5tNdefInfo.isMultiReadEn) {
                readBlockNum = (g_t5tNdefInfo.ndefLen - haveReadLen) / g_t5tRwInfo.blockLen;
                ret = NFC_HAL_T5TReadMultBlock(&g_t5tRwInfo, &readRes, readBlockNum - 1);
            } else {
                ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
            }
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T5TDetectNdef read failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_MESSAGE;
            }
            g_t5tRwInfo.blockNo += readBlockNum;
            haveReadLen += readRes.dataLen;
        }
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TReadNdef(uint8_t *readBuff, uint16_t *readLen)
{
    uint16_t haveReadLen = 0;
    // reading NDEF L-Field
    NfcErrorCode ret = NFC_NDEF_T5TReadLField(&haveReadLen, readBuff, *readLen);
    if (ret != NFC_OK) {
        return ret;
    }

    // reading NDEF V-Field
    ret = NFC_NDEF_T5TReadVField(haveReadLen, readBuff, *readLen);
    if (ret != NFC_OK) {
        return ret;
    }
    *readLen = g_t5tNdefInfo.ndefLen;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TResetLFieldWriteOperation(T5TNdefWriteParams *params, uint16_t byteNo,
                                                          const uint8_t *writeBuff, uint16_t writeLen)
{
    uint16_t buffLen = (uint16_t)UTIL_Min(writeLen, params->headLen - byteNo);
    if (buffLen > 0 && byteNo % g_t5tRwInfo.blockLen != 0) {
        if (memcpy_s(&params->headBuff[byteNo], T5T_READ_TEMP_BUFF_LEN - byteNo, writeBuff, buffLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    }
    byteNo = 0;
    while (byteNo < params->headLen) {
        NfcErrorCode ret = NFC_HAL_T5TWriteSingleBlock(&g_t5tRwInfo, params->headBuff + byteNo, g_t5tRwInfo.blockLen);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T5TResetLField write block failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
        }
        byteNo += g_t5tRwInfo.blockLen;
        g_t5tRwInfo.blockNo++;
    }
    params->writtenLen = buffLen;
    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TResetLField(T5TNdefWriteParams *params, const uint8_t *writeBuff, uint16_t writeLen)
{
    g_t5tRwInfo.blockNo = g_t5tNdefInfo.ndefBlNo;
    uint16_t byteNo = g_t5tNdefInfo.ndefBNo + 1;
    if (byteNo >= g_t5tRwInfo.blockLen) {
        g_t5tRwInfo.blockNo++;
        byteNo %= g_t5tRwInfo.blockLen;
    }
    if (byteNo != 0) {
        NfcHalT5TReadRes readRes = {params->headBuff, T5T_READ_TEMP_BUFF_LEN, 0, 0, 0, 0};
        NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T5TResetLField read first block failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
        }
    }
    params->headLen = g_t5tRwInfo.blockLen;
    if (writeLen < T5T_NDEF_LRECORD_MIN_LEN) {
        params->headBuff[byteNo++] = 0;
    } else {
        params->headBuff[byteNo++] = 0;
        if (byteNo >= params->headLen) {
            g_t5tRwInfo.blockNo++;
            NfcHalT5TReadRes readRes = {params->headBuff + params->headLen,
                                        T5T_READ_TEMP_BUFF_LEN - params->headLen, 0, 0, 0, 0};
            NfcErrorCode ret = NFC_HAL_T5TReadSingleBlock(&g_t5tRwInfo, &readRes);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T5TResetLField read block failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
            }
            g_t5tRwInfo.blockNo--;
            params->headLen += g_t5tRwInfo.blockLen;
        }
        byteNo += sizeof(uint16_t);
    }

    return NFC_NDEF_T5TResetLFieldWriteOperation(params, byteNo, writeBuff, writeLen);
}

static NfcErrorCode NFC_NDEF_T5TWriteVField(T5TNdefWriteParams *params, const uint8_t *writeBuff, uint16_t writeLen)
{
    // write ndef message
    while (writeLen - params->writtenLen >= g_t5tRwInfo.blockLen) {
        NfcErrorCode ret = NFC_HAL_T5TWriteSingleBlock(&g_t5tRwInfo, &writeBuff[params->writtenLen],
                                                       g_t5tRwInfo.blockLen);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T5TWriteVField write failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_WRITE_NDEF_MESSAGE;
        }
        g_t5tRwInfo.blockNo++;
        params->writtenLen += g_t5tRwInfo.blockLen;
    }

    // write last block with terminator tlv
    uint8_t lastBlkBuff[T5T_MAX_BLOCK_LEN] = {0};
    uint16_t byteNo = writeLen - params->writtenLen;
    if (byteNo > 0) {
        if (memcpy_s(lastBlkBuff, T5T_MAX_BLOCK_LEN, &writeBuff[params->writtenLen], byteNo) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        params->writtenLen += byteNo;
    }
    params->writtenLen += (writeLen < T5T_NDEF_LRECORD_MIN_LEN ? T5T_NDEF_TL_SHORT_LEN : T5T_NDEF_TL_LONG_LEN);
    if (params->writtenLen < g_t5tNdefInfo.t5tAreaSize) {
        lastBlkBuff[byteNo] = TERMINATOR_TLV;
    }
    NfcErrorCode ret = NFC_HAL_T5TWriteSingleBlock(&g_t5tRwInfo, lastBlkBuff, g_t5tRwInfo.blockLen);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T5TWriteVField write last block failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_WRITE_NDEF_MESSAGE;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TUpdateLField(T5TNdefWriteParams *params, uint16_t writeLen)
{
    g_t5tRwInfo.blockNo = g_t5tNdefInfo.ndefBlNo;
    uint16_t byteNo = g_t5tNdefInfo.ndefBNo + 1;
    if (byteNo >= g_t5tRwInfo.blockLen) {
        g_t5tRwInfo.blockNo++;
        byteNo %= g_t5tRwInfo.blockLen;
    }
    if (writeLen < T5T_NDEF_LRECORD_MIN_LEN) {
        params->headBuff[byteNo++] = writeLen;
    } else {
        params->headBuff[byteNo++] = T5T_NDEF_LRECORD_SIGN;
        params->headBuff[byteNo++] = UTIL_Byte1(writeLen);
        params->headBuff[byteNo++] = UTIL_Byte0(writeLen);
    }
    byteNo = 0;
    while (byteNo < params->headLen) {
        NfcErrorCode ret = NFC_HAL_T5TWriteSingleBlock(&g_t5tRwInfo, params->headBuff + byteNo, g_t5tRwInfo.blockLen);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T5TUpdateLField update L-Field failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_UPDATE_NDEF_LEN;
        }
        byteNo += g_t5tRwInfo.blockLen;
        g_t5tRwInfo.blockNo++;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T5TWriteNdef(const uint8_t *writeBuff, uint16_t writeLen)
{
    T5TNdefWriteParams params = {
        .writtenLen = 0,
        .headLen = 0,
        .headBuff = {0},
    };
    NfcErrorCode ret = NFC_NDEF_T5TResetLField(&params, writeBuff, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    ret = NFC_NDEF_T5TWriteVField(&params, writeBuff, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    ret = NFC_NDEF_T5TUpdateLField(&params, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    g_t5tNdefInfo.ndefLen = writeLen;

    return NFC_OK;
}

NfcErrorCode NFC_NDEF_T5TResetProc(const NfcTechVParams *param)
{
    if (param == NULL) {
        return NFC_ERR_CODE_NDEF_PROC_INPUT_PARAM_INVALID;
    }
    memset_s(&g_t5tNdefInfo, sizeof(g_t5tNdefInfo), 0, sizeof(g_t5tNdefInfo));
    memset_s(&g_t5tRwInfo, sizeof(g_t5tRwInfo), 0, sizeof(g_t5tRwInfo));
    if (memcpy_s(g_t5tRwInfo.uid, UID_LEN, param->uid, UID_LEN) != NFC_OK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    g_t5tRwInfo.reqFlag.isAms = true;

    return NFC_OK;
}

NfcErrorCode NFC_NDEF_T5TReadProc(uint8_t *readBuff, uint16_t *readLen)
{
    // NDEF Detect
    if (!g_t5tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T5TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Read
    return NFC_NDEF_T5TReadNdef(readBuff, readLen);
}

NfcErrorCode NFC_NDEF_T5TWriteProc(const uint8_t *writeBuff, uint16_t writeLen)
{
    // NDEF Detect
    if (!g_t5tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T5TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Write
    if (g_t5tNdefInfo.accessCon == T5T_ACCESS_READ_ONLY) {
        PS_PRINT_ERR("[NDEF] WriteProcT5T TLV Read Only.\n");
        return NFC_ERR_CODE_NDEF_PROC_WRITE_TLV_READ_ONLY;
    }
    uint16_t tlvHeadSpace = writeLen < T5T_NDEF_LRECORD_MIN_LEN ? T5T_NDEF_TL_SHORT_LEN : T5T_NDEF_TL_LONG_LEN;
    if (writeLen + tlvHeadSpace > g_t5tNdefInfo.t5tAreaSize) {
        PS_PRINT_ERR("[NDEF] WriteProcT5T writeLen overflow.\n");
        return NFC_ERR_CODE_NDEF_PROC_WRITE_LEN_OVERFLOW;
    }
    return NFC_NDEF_T5TWriteNdef(writeBuff, writeLen);
}

#ifdef __cplusplus
}
#endif