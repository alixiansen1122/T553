/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: NFC NDEF process for T4T \n
 * Author: @CompanyNameTag \n
 * Date: 2023-03-22 \n
 */
#include "nfc_ndef_t4t.h"
#include <stdbool.h>
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_hal_t4t.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T4T_CC_2_0_LEN      15
#define T4T_NDEF_NLEN       2

#define NDEF_FILE_NO_ACCESS 0xFF

typedef struct {
    uint8_t mappingVer;
    uint16_t mLe;
    uint16_t mLc;
    uint16_t ndefFileId;
    uint16_t ndefFileSize;
    uint8_t readAccess;
    uint8_t writeAccess;
    uint16_t ndefLen;
    bool isDetected;
} T4TNdefInfo;

static T4TNdefInfo g_t4tNdefInfo;

static NfcErrorCode NFC_NDEF_T4TDetectReadCC(void)
{
    uint8_t rApdudata[T4T_CC_2_0_LEN] = {0};
    NfcHalT4tRApdu rApduInfo = {rApdudata, T4T_CC_2_0_LEN, 0, 0, 0};

    // Select NDEF Tag Application
    NfcErrorCode ret = NFC_HAL_T4TPollerSelectAid(&rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TProcReadCC select ndef aid failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
    }

    // Select CC
    ret = NFC_HAL_T4TPollerSelectFile(T4T_CC_FILE_ID, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TProcReadCC select cc failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
    }

    // Read CC
    ret = NFC_HAL_T4TPollerReadData(0, T4T_CC_2_0_LEN, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TProcReadCC read cc failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_CC;
    }
    if (rApduInfo.dataLen < T4T_MIN_CC_LEN) {
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    uint16_t idx = 2;   // 2 means the location of mapping version
    g_t4tNdefInfo.mappingVer = rApduInfo.data[idx++];
    if (g_t4tNdefInfo.mappingVer != T4T_MAP_VER) {
        return NFC_ERR_CODE_NDEF_PROC_DETECT_WRONG_TAG;
    }
    g_t4tNdefInfo.mLe = UTIL_Combine8bitTo16bit(rApduInfo.data[idx], rApduInfo.data[idx + 1]);
    idx += sizeof(uint16_t);
    g_t4tNdefInfo.mLc = UTIL_Combine8bitTo16bit(rApduInfo.data[idx], rApduInfo.data[idx + 1]);
    idx += sizeof(uint16_t);
    if (rApduInfo.data[idx] != T4T_NDEF_FILE_CTRL_TLV) {
        PS_PRINT_ERR("[NDEF] T4TProcReadCC not NDEF file. T-Field = 0x%x\n", rApduInfo.data[idx]);
        return NFC_ERR_CODE_NDEF_PROC_DETECT_TLV_NOT_FOUND;
    }
    idx += 2;   // 2 means the length of NDEF-File_Ctrl_TLV T-Field & L-Field
    g_t4tNdefInfo.ndefFileId = UTIL_Combine8bitTo16bit(rApduInfo.data[idx], rApduInfo.data[idx + 1]);
    idx += sizeof(uint16_t);
    g_t4tNdefInfo.ndefFileSize = UTIL_Combine8bitTo16bit(rApduInfo.data[idx], rApduInfo.data[idx + 1]);
    idx += sizeof(uint16_t);
    g_t4tNdefInfo.readAccess = rApduInfo.data[idx++];
    g_t4tNdefInfo.writeAccess = rApduInfo.data[idx++];

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T4TDetectReadNLen(void)
{
    uint8_t rApdudata[T4T_NDEF_NLEN] = {0};
    NfcHalT4tRApdu rApduInfo = {rApdudata, T4T_NDEF_NLEN, 0, 0, 0};

    // Select NDEF File
    NfcErrorCode ret = NFC_HAL_T4TPollerSelectFile(g_t4tNdefInfo.ndefFileId, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TProcReadNLen select NDEF file failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_SEARCH_NDEF;
    }

    // Read NLen
    ret = NFC_HAL_T4TPollerReadData(0, T4T_NDEF_NLEN, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TProcReadNLen read NLEN failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_LEN;
    }
    g_t4tNdefInfo.ndefLen = UTIL_Combine8bitTo16bit(rApduInfo.data[0], rApduInfo.data[1]);

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T4TDetectNdef(void)
{
    NfcErrorCode ret = NFC_NDEF_T4TDetectReadCC();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_NDEF_T4TDetectReadNLen();
    if (ret != NFC_OK) {
        return ret;
    }
    g_t4tNdefInfo.isDetected = true;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T4TReadNdef(uint8_t *readBuff, uint16_t *readLen)
{
    NfcHalT4tRApdu rApduInfo = {readBuff, *readLen, 0, 0, 0};
    uint8_t expRspLen = T4T_DATA_MAX_LEN;
    uint16_t idx = 0;
    while (idx < g_t4tNdefInfo.ndefLen) {
        if (g_t4tNdefInfo.ndefLen - idx < expRspLen) {
            expRspLen = UTIL_Byte0(g_t4tNdefInfo.ndefLen - idx);
        }
        NfcErrorCode ret = NFC_HAL_T4TPollerReadData(T4T_NDEF_NLEN + idx, expRspLen, &rApduInfo);
        if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
            PS_PRINT_ERR("[NDEF] T4TReadNdef read NDEF message failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                         ret, rApduInfo.sw1, rApduInfo.sw2);
            return NFC_ERR_CODE_NDEF_PROC_TAG_READ_NDEF_MESSAGE;
        }
        if (memcpy_s(&readBuff[idx], *readLen - idx, rApduInfo.data, rApduInfo.dataLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        idx += rApduInfo.dataLen;
        rApduInfo.data += rApduInfo.dataLen;
        rApduInfo.dataBuffSize -= rApduInfo.dataLen;
    }
    *readLen = g_t4tNdefInfo.ndefLen;

    return NFC_OK;
}

static NfcErrorCode NFC_NDEF_T4TWriteNdef(const uint8_t *writeBuff, uint16_t writeLen)
{
    uint8_t txBuff[T4T_NDEF_NLEN] = {0};
    NfcHalT4tRApdu rApduInfo = {txBuff, T4T_NDEF_NLEN, 0, 0, 0};

    // Reset NLEN Field
    NfcErrorCode ret = NFC_HAL_T4TPollerWriteData(0, txBuff, T4T_NDEF_NLEN, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TWriteNdef reset ndef len failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_RESET_NDEF_LEN;
    }
    uint8_t txLen = T4T_DATA_MAX_LEN;
    if (txLen > g_t4tNdefInfo.mLc) {
        txLen = g_t4tNdefInfo.mLc;
    }
    uint16_t idx = 0;
    while (idx < writeLen) {
        if (writeLen - idx < txLen) {
            txLen = UTIL_Byte0(writeLen - idx);
        }
        ret = NFC_HAL_T4TPollerWriteData(T4T_NDEF_NLEN + idx, &writeBuff[idx], txLen, &rApduInfo);
        if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
            PS_PRINT_ERR("[NDEF] T4TWriteNdef write ndef data failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                         ret, rApduInfo.sw1, rApduInfo.sw2);
            return NFC_ERR_CODE_NDEF_PROC_TAG_WRITE_NDEF_MESSAGE;
        }
        idx += txLen;
    }

    // Update NLEN Field
    idx = 0;
    txBuff[idx++] = UTIL_Byte1(writeLen);
    txBuff[idx++] = UTIL_Byte0(writeLen);
    ret = NFC_HAL_T4TPollerWriteData(0, txBuff, T4T_NDEF_NLEN, &rApduInfo);
    if (ret != NFC_OK || !NFC_HAL_IsT4TSwSucc(rApduInfo.sw1, rApduInfo.sw2)) {
        PS_PRINT_ERR("[NDEF] T4TWriteNdef update ndef len failed. ret=0x%x, sw1=0x%x, sw2=0x%x.\n",
                     ret, rApduInfo.sw1, rApduInfo.sw2);
        return NFC_ERR_CODE_NDEF_PROC_TAG_UPDATE_NDEF_LEN;
    }
    g_t4tNdefInfo.ndefLen = writeLen;

    return NFC_OK;
}

void NFC_NDEF_T4TResetProc(void)
{
    memset_s(&g_t4tNdefInfo, sizeof(g_t4tNdefInfo), 0, sizeof(g_t4tNdefInfo));
}

NfcErrorCode NFC_NDEF_T4TReadProc(uint8_t *readBuff, uint16_t *readLen)
{
    // NDEF Detect
    if (!g_t4tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T4TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Read
    if (g_t4tNdefInfo.ndefLen == 0) {
        return NFC_ERR_CODE_NDEF_PROC_READ_EMPTY_NDEF;
    }
    if (g_t4tNdefInfo.ndefLen > *readLen) {
        return NFC_ERR_CODE_NDEF_PROC_READ_BUFF_SHORTAGE;
    }
    return NFC_NDEF_T4TReadNdef(readBuff, readLen);
}

NfcErrorCode NFC_NDEF_T4TWriteProc(const uint8_t *writeBuff, uint16_t writeLen)
{
    // NDEF Detect
    if (!g_t4tNdefInfo.isDetected) {
        NfcErrorCode ret = NFC_NDEF_T4TDetectNdef();
        if (ret != NFC_OK) {
            return ret;
        }
    }

    // NDEF Write
    if (g_t4tNdefInfo.writeAccess == NDEF_FILE_NO_ACCESS) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_TLV_READ_ONLY;
    }
    if (g_t4tNdefInfo.ndefFileSize < writeLen + T4T_NDEF_NLEN) {
        return NFC_ERR_CODE_NDEF_PROC_WRITE_LEN_OVERFLOW;
    }
    return NFC_NDEF_T4TWriteNdef(writeBuff, writeLen);
}

#ifdef __cplusplus
}
#endif