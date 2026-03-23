/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc hal T4T tag operations \n
 * Author: @CompanyNameTag \n
 * Date: 2022-11-15 \n
 */
#include "nfc_hal_t4t.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T4T_APDU_MAX_LEN                             255
#define T4T_CAPDU_PROLOGUE_LEN                       4
#define T4T_CAPDU_LC_LEN                             1
#define T4T_CAPDU_AID_LEN                            7
#define T4T_CAPDU_FID_LEN                            2
#define T4T_RAPDU_SW1SW2_LEN                         2

#define T4T_CLA                                      0x00
#define T4T_INS_SELECT                               0xA4
#define T4T_INS_READBINARY                           0xB0
#define T4T_INS_UPDATEBINARY                         0xD6
#define T4T_P1_SELECT_BY_AID                         0x04
#define T4T_P1_SELECT_BY_FID                         0x00
#define T4T_P2_SELECT_BY_AID_FIRST_OR_ONLY_OCCURENCE 0x00
#define T4T_P2_SELECT_BY_FID_FIRST_OR_ONLY_OCCURENCE 0x0C
#define T4T_LC_NO_DATA                               0x00

static NfcErrorCode NFC_HAL_T4TPollerParseRAPDU(const uint8_t *rxBuff, uint16_t rxLen, NfcHalT4tRApdu *rApduInfo)
{
    if (rxLen < T4T_RAPDU_SW1SW2_LEN || rApduInfo->data == NULL ||
        rApduInfo->dataBuffSize < rxLen - T4T_RAPDU_SW1SW2_LEN) {
        return NFC_ERR_CODE_T4T_INPUT_PARAM_INVALID;
    }
    uint16_t idx = 0;
    if (memcpy_s(rApduInfo->data, rApduInfo->dataBuffSize, rxBuff, rxLen - T4T_RAPDU_SW1SW2_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    rApduInfo->dataLen = rxLen - T4T_RAPDU_SW1SW2_LEN;
    idx += rApduInfo->dataLen;
    rApduInfo->sw1 = rxBuff[idx++];
    rApduInfo->sw2 = rxBuff[idx++];

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T4TPollerSelectAid(NfcHalT4tRApdu *rApduInfo)
{
    if (rApduInfo == NULL) {
        return NFC_ERR_CODE_T4T_INPUT_PARAM_INVALID;
    }
    uint8_t txBuff[] = {T4T_CLA, T4T_INS_SELECT, T4T_P1_SELECT_BY_AID, T4T_P2_SELECT_BY_AID_FIRST_OR_ONLY_OCCURENCE,
                        T4T_CAPDU_AID_LEN, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, T4T_DATA_MAX_LEN};
    uint8_t rxBuff[T4T_APDU_MAX_LEN] = {0};
    uint16_t rxLen = T4T_APDU_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen < T4T_RAPDU_SW1SW2_LEN) {
        PS_PRINT_ERR("[T4T] SelectAid ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T4T_SELECT_AID_ERR;
    }

    return NFC_HAL_T4TPollerParseRAPDU(rxBuff, rxLen, rApduInfo);
}

NfcErrorCode NFC_HAL_T4TPollerSelectFile(uint16_t fid, NfcHalT4tRApdu *rApduInfo)
{
    if (rApduInfo == NULL) {
        return NFC_ERR_CODE_T4T_INPUT_PARAM_INVALID;
    }
    uint8_t txBuff[] = {T4T_CLA, T4T_INS_SELECT, T4T_P1_SELECT_BY_FID, T4T_P2_SELECT_BY_FID_FIRST_OR_ONLY_OCCURENCE,
                        T4T_CAPDU_FID_LEN, UTIL_Byte1(fid), UTIL_Byte0(fid)};
    uint8_t rxBuff[T4T_RAPDU_SW1SW2_LEN] = {0};
    uint16_t rxLen = T4T_RAPDU_SW1SW2_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T4T_RAPDU_SW1SW2_LEN) {
        PS_PRINT_ERR("[T4T] SelectFid ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T4T_SELECT_FID_ERR;
    }

    return NFC_HAL_T4TPollerParseRAPDU(rxBuff, rxLen, rApduInfo);
}

NfcErrorCode NFC_HAL_T4TPollerReadData(uint16_t offset, uint8_t expRspLen, NfcHalT4tRApdu *rApduInfo)
{
    if (offset > T4T_MAX_OFFEST || rApduInfo == NULL) {
        return NFC_ERR_CODE_T4T_INPUT_PARAM_INVALID;
    }
    if (expRspLen > T4T_DATA_MAX_LEN || expRspLen == 0x00) {
        expRspLen = T4T_DATA_MAX_LEN;
    }
    uint8_t txBuff[] = {T4T_CLA, T4T_INS_READBINARY, UTIL_Byte1(offset), UTIL_Byte0(offset), expRspLen};
    uint8_t rxBuff[T4T_APDU_MAX_LEN] = {0};
    uint16_t rxLen = T4T_APDU_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen < T4T_RAPDU_SW1SW2_LEN) {
        PS_PRINT_ERR("[T4T] ReadData ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T4T_READ_DATA_ERR;
    }

    return NFC_HAL_T4TPollerParseRAPDU(rxBuff, rxLen, rApduInfo);
}

NfcErrorCode NFC_HAL_T4TPollerWriteData(uint16_t offset, const uint8_t *data, uint8_t dataLen,
                                        NfcHalT4tRApdu *rApduInfo)
{
    if (offset > T4T_MAX_OFFEST || data == NULL || rApduInfo == NULL || dataLen == T4T_LC_NO_DATA ||
        dataLen > T4T_DATA_MAX_LEN) {
        return NFC_ERR_CODE_T4T_INPUT_PARAM_INVALID;
    }
    uint16_t idx = 0;
    uint8_t txBuff[T4T_APDU_MAX_LEN] = {0};
    txBuff[idx++] = T4T_CLA;
    txBuff[idx++] = T4T_INS_UPDATEBINARY;
    txBuff[idx++] = UTIL_Byte1(offset);
    txBuff[idx++] = UTIL_Byte0(offset);
    txBuff[idx++] = dataLen;
    if (memcpy_s(&txBuff[idx], T4T_APDU_MAX_LEN - idx, data, dataLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint16_t txLen = T4T_CAPDU_PROLOGUE_LEN + T4T_CAPDU_LC_LEN + dataLen;
    uint8_t rxBuff[T4T_RAPDU_SW1SW2_LEN] = {0};
    uint16_t rxLen = T4T_RAPDU_SW1SW2_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T4T_RAPDU_SW1SW2_LEN) {
        PS_PRINT_ERR("[T4T] WriteData ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T4T_WRITE_DATA_ERR;
    }

    return NFC_HAL_T4TPollerParseRAPDU(rxBuff, rxLen, rApduInfo);
}

#ifdef __cplusplus
}
#endif