/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: NFC NDEF process for T2T \n
 * Author: @CompanyNameTag \n
 * Date: 2023-03-22 \n
 */
#include "nfc_ndef_t2t.h"
#include <stdbool.h>
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_hal_t2t.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T2T_MAGIC_NUMBER            0xE1
#define T2T_AREA_BLOCK_SIZE         4
#define T2T_AREA_SIZE_MULTI         8
#define T2T_READ_BLOCK_NUMS         (T2T_READ_DATA_LEN / T2T_AREA_BLOCK_SIZE)

#define T2T_STATLOCK_BYTE_ADDR      10
#define T2T_STATIC_LOCKED           0xFFFF
#define T2T_STATIC_LOCKED_BLK_NUMS  16
#define T2T_ACCESS_READ_ONLY        0x0F

#define T2T_NDEF_LRECORD_SIGN       0xFF
#define T2T_NDEF_LRECORD_MIN_LEN    255
#define T2T_NDEF_TL_SHORT_LEN       2
#define T2T_NDEF_TL_LONG_LEN        4

enum T2TCtrlTlvs {
    NULL_TLV            = 0x00,
    LOCK_CONTROL_TLV    = 0x01,
    MEMORY_CONTROL_TLV  = 0x02,
    NDEF_MESSAGE_TLV    = 0x03,
    TERMINATOR_TLV      = 0xFE,
};

typedef struct {
    uint16_t staticLock;
    uint16_t t2tAreaSize;
    uint8_t mappingVer;
    uint8_t accessCon;
    uint16_t ndefBlNo;
    uint16_t ndefByteNo;
    uint16_t ndefLen;
    bool isDetected;
} T2TNdefInfo;

typedef struct {
    uint16_t blNo;
    uint16_t readLen;
} T2TNdefReadParams;

typedef struct {
    uint16_t blNo;
    uint16_t writtenLen;
    uint8_t headBuff[T2T_READ_DATA_LEN];
} T2TNdefWriteParams;

static T2TNdefInfo g_t2tNdefInfo;

static NfcErrorCode NFC_NDEF_T2TDetectReadCC(void)
{
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(0, rxBuff, T2T_READ_DATA_LEN);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T2TDetectReadCC read failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
    }
    uint16_t byteNo = T2T_STATLOCK_BYTE_ADDR;
    g_t2tNdefInfo.staticLock = UTIL_Combine8bitTo16bit(rxBuff[byteNo], rxBuff[byteNo + 1]);
    byteNo += sizeof(uint16_t);
    if (rxBuff[byteNo] != T2T_MAGIC_NUMBER) {  // CC_0
        PS_PRINT_ERR("[NDEF] T2TDetectReadCC CC_0 = 0x%x\n", rxBuff[byteNo]);
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    byteNo++;
    g_t2tNdefInfo.mappingVer = rxBuff[byteNo++];  // CC_1
    g_t2tNdefInfo.t2tAreaSize = (uint16_t)rxBuff[byteNo++] * T2T_AREA_SIZE_MULTI;  // CC_2
    g_t2tNdefInfo.accessCon = rxBuff[byteNo++];  // CC_3

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TDetectTlvs(void)
{
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    uint16_t blNo = T2T_READ_BLOCK_NUMS;
    uint16_t byteNo = 0;
    while (byteNo < g_t2tNdefInfo.t2tAreaSize) {
        NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(blNo, rxBuff, T2T_READ_DATA_LEN);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T2TDetectReadTLVs read failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_SEARCH_NDEF;
        }
        uint16_t idx = byteNo % T2T_AREA_BLOCK_SIZE;
        // If not NDEF TLV, skip to next TLV.
        while (idx < T2T_READ_DATA_LEN - 1 && rxBuff[idx] != NDEF_MESSAGE_TLV) {
            idx++;                      // L-field
            idx += rxBuff[idx] + 1;     // skip to the next TLV
        }
        blNo += idx / T2T_AREA_BLOCK_SIZE;
        byteNo += idx - byteNo % T2T_AREA_BLOCK_SIZE;
        // If find NDEF TLV
        if (idx < T2T_READ_DATA_LEN && rxBuff[idx] == NDEF_MESSAGE_TLV) {
            g_t2tNdefInfo.ndefBlNo = blNo;
            g_t2tNdefInfo.ndefByteNo = byteNo % T2T_AREA_BLOCK_SIZE;
            g_t2tNdefInfo.isDetected = true;
            break;
        }
    }
    if (!g_t2tNdefInfo.isDetected) {
        PS_PRINT_ERR("[NDEF] T2TDetectReadTLVs NDEF TLV not found.\n");
        return NFC_ERR_CODE_NDEF_PROC_DETECT_TLV_NOT_FOUND;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TDetectNdef(void)
{
    // reading StaticLock & CC
    NfcErrorCode ret = NFC_NDEF_T2TDetectReadCC();
    if (ret != NFC_OK) {
        return ret;
    }

    // reading TLVs and find NDEF TLV
    return NFC_NDEF_T2TDetectTlvs();
}

static NfcErrorCode NFC_NDEF_T2TReadLField(T2TNdefReadParams *params, uint8_t *readBuff, uint16_t readLen)
{
    uint16_t byteNo = g_t2tNdefInfo.ndefByteNo;

    // Read the first NDEF block
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(params->blNo, rxBuff, T2T_READ_DATA_LEN);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T2TReadLField read failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_LEN;
    }
    byteNo++;
    if (rxBuff[byteNo] == T2T_NDEF_LRECORD_SIGN) {
        byteNo++;
        g_t2tNdefInfo.ndefLen = UTIL_Combine8bitTo16bit(rxBuff[byteNo], rxBuff[byteNo + 1]);
        byteNo += 2;  // skip the last 2 bytes of 3-byte L-Field
    } else {
        g_t2tNdefInfo.ndefLen = rxBuff[byteNo++];
    }
    if (g_t2tNdefInfo.ndefLen == 0) {
        return NFC_ERR_CODE_NDEF_PROC_READ_EMPTY_NDEF;
    }
    if (g_t2tNdefInfo.ndefLen > readLen) {
        PS_PRINT_ERR("[NDEF] T2TReadLField readBuff shortage. ndefLen = %d\n", g_t2tNdefInfo.ndefLen);
        return NFC_ERR_CODE_NDEF_PROC_READ_BUFF_SHORTAGE;
    }
    uint16_t buffLen = (uint16_t)UTIL_Min(g_t2tNdefInfo.ndefLen, (T2T_READ_DATA_LEN - byteNo));
    if (memcpy_s(readBuff, readLen, &rxBuff[byteNo], buffLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    params->blNo += T2T_READ_BLOCK_NUMS;
    params->readLen = buffLen;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TReadVField(T2TNdefReadParams *params, uint8_t *readBuff, uint16_t readLen)
{
    // Read the rest
    while (params->readLen < g_t2tNdefInfo.ndefLen) {
        // If not a full 4-block
        if (g_t2tNdefInfo.ndefLen - params->readLen < T2T_READ_DATA_LEN) {
            uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
            uint16_t rxLen = g_t2tNdefInfo.ndefLen - params->readLen;
            NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(params->blNo, rxBuff, T2T_READ_DATA_LEN);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T2TReadVField read failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_MESSAGE;
            }
            if (memcpy_s(&readBuff[params->readLen], readLen - params->readLen, rxBuff, rxLen) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            params->readLen += rxLen;
        } else {
            NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(params->blNo, &readBuff[params->readLen], T2T_READ_DATA_LEN);
            if (ret != NFC_OK) {
                PS_PRINT_ERR("[NDEF] T2TReadVField read failed. ret=0x%x\n", ret);
                return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_MESSAGE;
            }
            params->readLen += T2T_READ_DATA_LEN;
            params->blNo += T2T_READ_BLOCK_NUMS;
        }
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TReadNdef(uint8_t *readBuff, uint16_t *readLen)
{
    T2TNdefReadParams params = {
        .blNo = g_t2tNdefInfo.ndefBlNo,
        .readLen = 0,
    };
    NfcErrorCode ret = NFC_NDEF_T2TReadLField(&params, readBuff, *readLen);
    if (ret != NFC_OK) {
        return ret;
    }
    ret = NFC_NDEF_T2TReadVField(&params, readBuff, *readLen);
    if (ret != NFC_OK) {
        return ret;
    }
    *readLen = g_t2tNdefInfo.ndefLen;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TResetLField(T2TNdefWriteParams *params, const uint8_t *writeBuff, uint16_t writeLen)
{
    // If ndef tlv does not begin at the first byte, read the block.
    if (g_t2tNdefInfo.ndefByteNo != 0) {
        NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(params->blNo, params->headBuff, T2T_READ_DATA_LEN);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T2TResetLField first block read failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
        }
    }

    // T-field
    uint16_t byteNo = g_t2tNdefInfo.ndefByteNo;
    params->headBuff[byteNo++] = NDEF_MESSAGE_TLV;

    // L-field reset
    if (writeLen < T2T_NDEF_LRECORD_MIN_LEN) {
        params->headBuff[byteNo++] = 0;
    } else {
        params->headBuff[byteNo++] = 0;
        params->headBuff[byteNo++] = 0;
        params->headBuff[byteNo++] = 0;
    }

    // fill whole block
    uint16_t buffLen = (uint16_t)UTIL_Min(writeLen, T2T_READ_DATA_LEN - byteNo);
    if (byteNo % T2T_AREA_BLOCK_SIZE != 0) {
        if (memcpy_s(&params->headBuff[byteNo], T2T_READ_DATA_LEN - byteNo, writeBuff, buffLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    }

    // start to write
    uint16_t idx = 0;
    while (idx < byteNo) {
        NfcErrorCode ret = NFC_HAL_T2TPollerProcWrite(params->blNo, &params->headBuff[idx], T2T_WRITE_DATA_LEN);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T2TResetLField write failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
        }
        params->blNo++;
        idx += T2T_WRITE_DATA_LEN;
    }
    idx = idx - byteNo;
    params->writtenLen = (uint16_t)UTIL_Min(buffLen, idx);

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TWriteVField(T2TNdefWriteParams *params, const uint8_t *writeBuff, uint16_t writeLen)
{
    // write ndef message
    while (writeLen - params->writtenLen >= T2T_WRITE_DATA_LEN) {
        NfcErrorCode ret = NFC_HAL_T2TPollerProcWrite(params->blNo, &writeBuff[params->writtenLen],
                                                      T2T_WRITE_DATA_LEN);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T2TWriteVField write failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_WRITE_NDEF_MESSAGE;
        }
        params->blNo++;
        params->writtenLen += T2T_WRITE_DATA_LEN;
    }

    // write last block with terminator tlv
    uint8_t lastBlkBuff[T2T_WRITE_DATA_LEN] = {0};
    uint16_t byteNo = writeLen - params->writtenLen;
    if (byteNo > 0) {
        if (memcpy_s(lastBlkBuff, T2T_WRITE_DATA_LEN, &writeBuff[params->writtenLen], byteNo) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        params->writtenLen += byteNo;
    }
    params->writtenLen += (writeLen < T2T_NDEF_LRECORD_MIN_LEN ? T2T_NDEF_TL_SHORT_LEN : T2T_NDEF_TL_LONG_LEN);
    if (params->writtenLen < g_t2tNdefInfo.t2tAreaSize) {
        lastBlkBuff[byteNo] = TERMINATOR_TLV;
    }
    NfcErrorCode ret = NFC_HAL_T2TPollerProcWrite(params->blNo, lastBlkBuff, T2T_WRITE_DATA_LEN);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] T2TWriteVField write last block failed. ret=0x%x\n", ret);
        return NFC_ERR_CODE_NDEF_PROC_TAG_WRITE_NDEF_MESSAGE;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TUpdateLField(T2TNdefWriteParams *params, uint16_t writeLen)
{
    uint16_t blNo = g_t2tNdefInfo.ndefBlNo;
    uint16_t byteNo = g_t2tNdefInfo.ndefByteNo + 1;

    // L-field update
    if (writeLen < T2T_NDEF_LRECORD_MIN_LEN) {
        params->headBuff[byteNo++] = writeLen;
    } else {
        params->headBuff[byteNo++] = T2T_NDEF_LRECORD_SIGN;
        params->headBuff[byteNo++] = UTIL_Byte1(writeLen);
        params->headBuff[byteNo++] = UTIL_Byte0(writeLen);
    }

    // start to write
    uint16_t idx = 0;
    while (idx < byteNo) {
        NfcErrorCode ret = NFC_HAL_T2TPollerProcWrite(blNo, &params->headBuff[idx], T2T_WRITE_DATA_LEN);
        if (ret != NFC_OK) {
            PS_PRINT_ERR("[NDEF] T2TUpdateLField update L-field failed. ret=0x%x\n", ret);
            return NFC_ERR_CODE_NDEF_PROC_TAG_UPDATE_NDEF_LEN;
        }
        blNo++;
        idx += T2T_WRITE_DATA_LEN;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T2TWriteNdef(const uint8_t *writeBuff, uint16_t writeLen)
{
    T2TNdefWriteParams params = {
        .blNo = g_t2tNdefInfo.ndefBlNo,
        .writtenLen = 0,
        .headBuff = {0},
    };
    NfcErrorCode ret = NFC_NDEF_T2TResetLField(&params, writeBuff, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    ret = NFC_NDEF_T2TWriteVField(&params, writeBuff, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    ret = NFC_NDEF_T2TUpdateLField(&params, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }
    g_t2tNdefInfo.ndefLen = writeLen;

    return NFC_OK;
}

void NFC_NDEF_T2TResetProc(void)
{
    memset_s(&g_t2tNdefInfo, sizeof(g_t2tNdefInfo), 0, sizeof(g_t2tNdefInfo));
}

NfcErrorCode NFC_NDEF_T2TReadProc(uint8_t *readBuff, uint16_t *readLen)
{
    // NDEF Detect
    if (!g_t2tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T2TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Read
    NfcErrorCode ret = NFC_NDEF_T2TReadNdef(readBuff, readLen);
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_OK;
}

NfcErrorCode NFC_NDEF_T2TWriteProc(const uint8_t *writeBuff, uint16_t writeLen)
{
    // NDEF Detect
    if (!g_t2tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T2TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Write
    if (g_t2tNdefInfo.accessCon == T2T_ACCESS_READ_ONLY) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_TLV_READ_ONLY;
    }
    if (g_t2tNdefInfo.staticLock == T2T_STATIC_LOCKED) {
        if (g_t2tNdefInfo.ndefBlNo < T2T_STATIC_LOCKED_BLK_NUMS) {
            return NFC_ERR_CODE_NDEF_PROC_WRITE_TLV_READ_ONLY;
        }
    }
    uint16_t tlvHeadSpace = T2T_NDEF_TL_SHORT_LEN;
    if (writeLen >= T2T_NDEF_LRECORD_MIN_LEN) {
        tlvHeadSpace = T2T_NDEF_TL_LONG_LEN;
    }
    if (writeLen + tlvHeadSpace > g_t2tNdefInfo.t2tAreaSize) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_LEN_OVERFLOW;
    }
    NfcErrorCode ret = NFC_NDEF_T2TWriteNdef(writeBuff, writeLen);
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_OK;
}

#ifdef __cplusplus
}
#endif