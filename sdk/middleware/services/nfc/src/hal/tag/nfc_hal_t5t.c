/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc hal T5T tag operations \n
 * Author: @CompanyNameTag \n
 * Date: 2022-11-15 \n
 */
#include "nfc_hal_t5t.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T5T_SINGLE_CMD_LEN              11
#define T5T_READ_MULT_CMD_LEN           12
#define T5T_EX_SINGLE_CMD_LEN           12
#define T5T_EX_READ_MULT_CMD_LEN        14
#define T5T_SELECT_CMD_LEN              10
#define T5T_SLPV_CMD_LEN                10

#define T5T_CMD_START_POS               0
#define DEFAULT_NUMBEROFBLOCK           0

#define T5T_READ_ALIKE_RES_BASIC_LEN    1
#define T5T_WRITE_ALIKE_RES_LEN         1
#define T5T_SELECT_RES_LEN              1
#define T5T_MIN_BLOCK_LEN               4

#define CREADITS_NTF_LEN                6
#define CREADITS_NTF_HEADLEN            3
#define CREADITS_NTF_BYTE0              0x60
#define CREADITS_NTF_BYTE1              0x06
#define T5T_RES_FLAG_STATUS_OK          0x00
#define FLOW_CTRL_WAIT_TIMEOUT          50
// The response time of the main control is considered
#define SLPV_WAIT_TIMEOUT               20

#define DEFAULT_REQ_FLAG                0x02
#define REQ_FLAG_OPTION_FLAG            0x40
#define REQ_FLAG_AMS                    0x20
#define REQ_FLAG_SMS                    0x10

enum {
    T5T_SLPV_CMD = 0x02,
    T5T_READ_SINGLE_CMD = 0x20,
    T5T_WRITE_SINGLE_CMD = 0x21,
    T5T_LOCK_SINGLE_CMD = 0x22,
    T5T_READ_MULT_CMD = 0x23,
    T5T_SELECT_CMD = 0x25,
    T5T_EX_READ_SINGLE_CMD = 0x30,
    T5T_EX_WRITE_SINGLE_CMD = 0x31,
    T5T_EX_LOCK_SINGLE_CMD = 0x32,
    T5T_EX_READ_MULT_CMD = 0x33,
};

uint16_t NfcHalT5tCalRxReadLen(uint16_t basicLen, NfcHalT5TRWInfo *rwInfo, uint16_t numberOfBlock);
NfcErrorCode NfcHalT5tCmdBuild(uint8_t *txBuff, NfcHalT5TRWInfo *rwInfo, uint8_t cmdType,
                               const uint8_t *buff, uint16_t numberOfBlock);
uint8_t MakeReqFlagHelper(NfcHalT5TReqFlag *reqFlag);
uint16_t NfcHalT5tCmdLenHelper(uint8_t cmdType);

static inline uint16_t NfcHalT5tCalTxLen(uint16_t maxLen, bool isAms)
{
    return (isAms ? maxLen : maxLen - UID_LEN);
}

static NfcErrorCode NFC_HAL_T5TCheckReadRes(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes, uint16_t blockNum)
{
    if ((rwInfo->reqFlag.isOptionFlag && (readRes->status == NULL || readRes->statusBuffSize < blockNum)) ||
        (readRes->data == NULL || readRes->dataBuffSize < blockNum * rwInfo->blockLen)) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    return NFC_OK;
}

static NfcErrorCode NFC_HAL_T5TParseReadRes(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes,
                                            uint8_t *frameRfRxBuff, uint16_t numberOfBlock)
{
    // input param has already checked
    uint16_t idx = T5T_READ_ALIKE_RES_BASIC_LEN;
    readRes->dataLen = rwInfo->blockLen * (numberOfBlock + 1);
    if (rwInfo->reqFlag.isOptionFlag) {
        /* If Option Flag enabled, a response will be composed of a RES_FLAG byte and (NB + 1) tuples of
         * Block security status byte and BLEN data bytes. */
        readRes->statusLen = numberOfBlock + 1;
        uint16_t readIndex = 0;
        uint16_t blkIndex = 0;
        while (blkIndex < numberOfBlock + 1) {
            readRes->status[blkIndex++] = frameRfRxBuff[idx++];
            if (memcpy_s(readRes->data + readIndex, readRes->dataBuffSize - readIndex,
                         frameRfRxBuff + idx, rwInfo->blockLen) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            idx += rwInfo->blockLen;
            readIndex += rwInfo->blockLen;
        }
    } else {
        // If Option Flag not enabled, a response will be only composed of a RES_FLAG byte and (NB+1)*BLEN Data bytes.
        if (memcpy_s(readRes->data, readRes->dataBuffSize, frameRfRxBuff + idx, readRes->dataLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TReadSingleBlock(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes)
{
    if (rwInfo == NULL || readRes == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // calculate the length of the data that should be received
    bool isBlkLenKnown = rwInfo->blockLen != 0;
    uint16_t rxHeadLen = T5T_READ_ALIKE_RES_BASIC_LEN + (rwInfo->reqFlag.isOptionFlag ? 1 : 0);
    // For first read without block length info, set rxReadLen for storing head and one block of maximum length
    uint16_t rxReadLen = rxHeadLen + T5T_MAX_BLOCK_LEN;
    if (isBlkLenKnown) {
        // If block length is known, update expected read length.
        rxReadLen = NfcHalT5tCalRxReadLen(T5T_READ_ALIKE_RES_BASIC_LEN, rwInfo, DEFAULT_NUMBEROFBLOCK);
    }

    // check RES buffer
    if ((rwInfo->reqFlag.isOptionFlag && (readRes->status == NULL || readRes->statusBuffSize < 1)) ||
        (readRes->data == NULL || readRes->dataBuffSize + rxHeadLen < rxReadLen)) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T5T_SINGLE_CMD_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_READ_SINGLE_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcReadSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_READ_SINGLE_ERR;
    }

    uint8_t frameRfRxBuff[NCI_MAX_PAYLOAD_LEN] = {0};
    uint16_t rxLen = NCI_MAX_PAYLOAD_LEN;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_SINGLE_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, frameRfRxBuff, &rxLen);
    bool isRxLenInValid = (isBlkLenKnown && rxLen != rxReadLen + 1) || rxLen < rxHeadLen + 1 ||
                          ((rxLen - (rxHeadLen + 1)) % T5T_MIN_BLOCK_LEN) != 0;
    if (ret != NFC_OK || isRxLenInValid || frameRfRxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        frameRfRxBuff[rxLen - 1] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcReadSingle ret=0x%x, rxLen = %d, resflag = %x, errcode = %x, status = %x\n",
                     ret, rxLen, frameRfRxBuff[0], frameRfRxBuff[1],
                     rxLen >= 1 ? frameRfRxBuff[rxLen - 1] : NCI_RF_FRAME_CORRUPTED);
        return NFC_ERR_CODE_T5T_PROC_READ_SINGLE_ERR;
    }

    if (!isBlkLenKnown) {
        rwInfo->blockLen = (rxLen - 1) - rxHeadLen;
    }

    return NFC_HAL_T5TParseReadRes(rwInfo, readRes, frameRfRxBuff, DEFAULT_NUMBEROFBLOCK);
}

NfcErrorCode NFC_HAL_T5TWriteSingleBlock(NfcHalT5TRWInfo *rwInfo, const uint8_t *buff, uint16_t buffLen)
{
    if (rwInfo == NULL || buff == NULL || buffLen != rwInfo->blockLen) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T5T_SINGLE_CMD_LEN + T5T_MAX_BLOCK_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_WRITE_SINGLE_CMD, buff, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcWriteSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_WRITE_SINGLE_ERR;
    }

    uint8_t rxBuff[T5T_WRITE_ALIKE_RES_LEN + 1] = {0};
    uint16_t rxLen = T5T_WRITE_ALIKE_RES_LEN + 1;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_SINGLE_CMD_LEN + buffLen, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T5T_WRITE_ALIKE_RES_LEN + 1 || rxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        rxBuff[T5T_WRITE_ALIKE_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcWriteSingle ret=0x%x, rxLen = %d, resflag = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T5T_WRITE_ALIKE_RES_LEN]);
        return NFC_ERR_CODE_T5T_PROC_WRITE_SINGLE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TLockSingleBlock(NfcHalT5TRWInfo *rwInfo)
{
    if (rwInfo == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T5T_SINGLE_CMD_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_LOCK_SINGLE_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcLockSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_LOCK_SINGLE_ERR;
    }

    uint8_t rxBuff[T5T_WRITE_ALIKE_RES_LEN + 1] = {0};
    uint16_t rxLen = T5T_WRITE_ALIKE_RES_LEN + 1;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_SINGLE_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T5T_WRITE_ALIKE_RES_LEN + 1 || rxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        rxBuff[T5T_WRITE_ALIKE_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcLockSingle ret=0x%x, rxLen = %d, resflag = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T5T_WRITE_ALIKE_RES_LEN]);
        return NFC_ERR_CODE_T5T_PROC_LOCK_SINGLE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TReadMultBlock(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes, uint16_t numberOfBlock)
{
    if (rwInfo == NULL || readRes == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // calculate the length of the data that should be received
    uint16_t rxReadLen = NfcHalT5tCalRxReadLen(T5T_READ_ALIKE_RES_BASIC_LEN, rwInfo, numberOfBlock);
    if (rxReadLen > T5T_MAX_READ_LEN) {
        PS_PRINT_ERR("[T5T] ProcReadMult unsupported readlen = %d, should smaller than %d\n", rxReadLen,
                     T5T_MAX_READ_LEN);
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // check RES buffer
    NfcErrorCode ret = NFC_HAL_T5TCheckReadRes(rwInfo, readRes, numberOfBlock + 1);
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t txBuff[T5T_READ_MULT_CMD_LEN] = {0};
    ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_READ_MULT_CMD, NULL, numberOfBlock);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcReadMult Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_READ_MULT_ERR;
    }

    uint8_t frameRfRxBuff[NCI_MAX_PAYLOAD_LEN] = {0};
    uint16_t rxLen = NCI_MAX_PAYLOAD_LEN;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_READ_MULT_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, frameRfRxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != rxReadLen + 1 || frameRfRxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        frameRfRxBuff[rxReadLen] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcReadMult ret=0x%x, rxLen = %d, resflag = %x, errcode = %x, status = %x\n",
                     ret, rxLen, frameRfRxBuff[0], frameRfRxBuff[1], frameRfRxBuff[rxReadLen]);
        return NFC_ERR_CODE_T5T_PROC_READ_MULT_ERR;
    }

    return NFC_HAL_T5TParseReadRes(rwInfo, readRes, frameRfRxBuff, numberOfBlock);
}

NfcErrorCode NFC_HAL_T5TEXReadSingleBlock(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes)
{
    if (rwInfo == NULL || readRes == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // calculate the length of the data that should be received
    uint16_t rxReadLen = NfcHalT5tCalRxReadLen(T5T_READ_ALIKE_RES_BASIC_LEN, rwInfo, DEFAULT_NUMBEROFBLOCK);

    // check RES buffer
    NfcErrorCode ret = NFC_HAL_T5TCheckReadRes(rwInfo, readRes, DEFAULT_NUMBEROFBLOCK + 1);
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t txBuff[T5T_EX_SINGLE_CMD_LEN] = {0};
    ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_EX_READ_SINGLE_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcEXReadSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_EX_READ_SINGLE_ERR;
    }

    uint8_t frameRfRxBuff[NCI_MAX_PAYLOAD_LEN] = {0};
    uint16_t rxLen = NCI_MAX_PAYLOAD_LEN;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_EX_SINGLE_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, frameRfRxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != rxReadLen + 1 || frameRfRxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        frameRfRxBuff[rxReadLen] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcEXReadSingle ret=0x%x, rxLen = %d, resflag = %x, errcode = %x, status = %x\n",
                     ret, rxLen, frameRfRxBuff[0], frameRfRxBuff[1], frameRfRxBuff[rxReadLen]);
        return NFC_ERR_CODE_T5T_PROC_EX_READ_SINGLE_ERR;
    }

    return NFC_HAL_T5TParseReadRes(rwInfo, readRes, frameRfRxBuff, DEFAULT_NUMBEROFBLOCK);
}

NfcErrorCode NFC_HAL_T5TEXWriteSingleBlock(NfcHalT5TRWInfo *rwInfo, const uint8_t *buff, uint16_t buffLen)
{
    if (rwInfo == NULL || buff == NULL || buffLen != rwInfo->blockLen) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T5T_EX_SINGLE_CMD_LEN + T5T_MAX_BLOCK_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_EX_WRITE_SINGLE_CMD, buff, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcEXWriteSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_EX_WRITE_SINGLE_ERR;
    }

    uint8_t rxBuff[T5T_WRITE_ALIKE_RES_LEN + 1] = {0};
    uint16_t rxLen = T5T_WRITE_ALIKE_RES_LEN + 1;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_EX_SINGLE_CMD_LEN + buffLen, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T5T_WRITE_ALIKE_RES_LEN + 1 || rxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        rxBuff[T5T_WRITE_ALIKE_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcEXWriteSingle ret=0x%x, rxLen = %d, resflag = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T5T_WRITE_ALIKE_RES_LEN]);
        return NFC_ERR_CODE_T5T_PROC_EX_WRITE_SINGLE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TEXLockSingleBlock(NfcHalT5TRWInfo *rwInfo)
{
    if (rwInfo == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T5T_EX_SINGLE_CMD_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_EX_LOCK_SINGLE_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcEXLockSingle Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_EX_LOCK_SINGLE_ERR;
    }

    uint8_t rxBuff[T5T_WRITE_ALIKE_RES_LEN + 1] = {0};
    uint16_t rxLen = T5T_WRITE_ALIKE_RES_LEN + 1;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_EX_SINGLE_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T5T_WRITE_ALIKE_RES_LEN + 1 || rxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        rxBuff[T5T_WRITE_ALIKE_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcEXLockSingle ret=0x%x, rxLen = %d, resflag = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T5T_WRITE_ALIKE_RES_LEN]);
        return NFC_ERR_CODE_T5T_PROC_EX_LOCK_SINGLE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TEXReadMultBlock(NfcHalT5TRWInfo *rwInfo, NfcHalT5TReadRes *readRes, uint16_t numberOfBlock)
{
    if (rwInfo == NULL || readRes == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // calculate the length of the data that should be received
    uint16_t rxReadLen = NfcHalT5tCalRxReadLen(T5T_READ_ALIKE_RES_BASIC_LEN, rwInfo, numberOfBlock);
    if (rxReadLen > T5T_MAX_READ_LEN) {
        PS_PRINT_ERR("[T5T] ProcEXReadMult unsupported readlen = %d, should smaller than %d\n",
                     rxReadLen, T5T_MAX_READ_LEN);
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }

    // check RES buffer
    NfcErrorCode ret = NFC_HAL_T5TCheckReadRes(rwInfo, readRes, numberOfBlock + 1);
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t txBuff[T5T_EX_READ_MULT_CMD_LEN] = {0};
    ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_EX_READ_MULT_CMD, NULL, numberOfBlock);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcEXReadMult Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_EX_READ_MULT_ERR;
    }

    uint8_t frameRfRxBuff[NCI_MAX_PAYLOAD_LEN] = {0};
    uint16_t rxLen = NCI_MAX_PAYLOAD_LEN;
    uint16_t txLen = NfcHalT5tCalTxLen(T5T_EX_READ_MULT_CMD_LEN, rwInfo->reqFlag.isAms);
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, frameRfRxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != rxReadLen + 1 || frameRfRxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        frameRfRxBuff[rxReadLen] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcEXReadMult ret=0x%x, rxLen = %d, resflag = %x, errcode = %x, status = %x\n",
                     ret, rxLen, frameRfRxBuff[0], frameRfRxBuff[1], frameRfRxBuff[rxReadLen]);
        return NFC_ERR_CODE_T5T_PROC_EX_READ_MULT_ERR;
    }

    return NFC_HAL_T5TParseReadRes(rwInfo, readRes, frameRfRxBuff, numberOfBlock);
}

NfcErrorCode NFC_HAL_T5TSelect(NfcHalT5TRWInfo *rwInfo)
{
    if (rwInfo == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }
    uint8_t txBuff[T5T_SELECT_CMD_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_SELECT_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcSelect Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_SELECT_ERR;
    }

    uint8_t rxBuff[T5T_SELECT_RES_LEN + 1] = {0};
    uint16_t rxLen = T5T_SELECT_RES_LEN + 1;
    ret = NFC_HAL_TranceiveUnchainedData(txBuff, T5T_SELECT_CMD_LEN, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T5T_SELECT_RES_LEN + 1 || rxBuff[0] != T5T_RES_FLAG_STATUS_OK ||
        rxBuff[T5T_SELECT_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T5T] ProcSelect ret=0x%x, rxLen = %d, resflag = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T5T_SELECT_RES_LEN]);
        return NFC_ERR_CODE_T5T_PROC_SELECT_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T5TSlpV(NfcHalT5TRWInfo *rwInfo)
{
    if (rwInfo == NULL) {
        return NFC_ERR_CODE_T5T_INPUT_PARAM_INVALID_ERR;
    }
    uint8_t txBuff[T5T_SLPV_CMD_LEN] = {0};
    NfcErrorCode ret = NfcHalT5tCmdBuild(txBuff, rwInfo, T5T_SLPV_CMD, NULL, DEFAULT_NUMBEROFBLOCK);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcSlpv Cmd build error, ret=0x%x\n", ret);
        return NFC_ERR_CODE_T5T_PROC_SLPV_ERR;
    }

    uint8_t rxBuff[HAL_SHORT_BUFF_LEN] = {0};
    uint16_t rxLen = HAL_SHORT_BUFF_LEN;
    ret = NFC_HAL_SendData(txBuff, T5T_SLPV_CMD_LEN, NCI_STATIC_RF_CONN);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T5T] ProcSLPV send data ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_DATA_SND_ERR;
    }
    // recv data flow
    rxLen = HAL_SHORT_BUFF_LEN;
    ret = NFC_HAL_ReceiveNciPkt(rxBuff, &rxLen, FLOW_CTRL_WAIT_TIMEOUT);
    if (ret != NFC_OK || rxLen != CREADITS_NTF_LEN || rxBuff[0] != CREADITS_NTF_BYTE0 ||
        rxBuff[1] != CREADITS_NTF_BYTE1) {
        PS_PRINT_ERR("[T5T] ProcSLPV receive credits rxLen=%d, rxBuff head:0x%2x%2x\n", rxLen, rxBuff[0], rxBuff[1]);
        return NFC_ERR_CODE_T5T_PROC_SLPV_ERR;
    }
    // should not recv data
    rxLen = HAL_SHORT_BUFF_LEN;
    ret = NFC_HAL_ReceiveNciPkt(rxBuff, &rxLen, SLPV_WAIT_TIMEOUT);
    if (ret == NFC_OK && rxLen > 0) {
        PS_PRINT_ERR("[T5T] ProcSLPV receive data retLen = %d\n", rxLen);
        return NFC_ERR_CODE_T5T_PROC_SLPV_ERR;
    }
    return NFC_OK;
}

uint16_t NfcHalT5tCalRxReadLen(uint16_t basicLen, NfcHalT5TRWInfo *rwInfo, uint16_t numberOfBlock)
{
    uint16_t rxReadLen = basicLen;
    if (rwInfo->reqFlag.isOptionFlag) {
        rxReadLen += (rwInfo->blockLen + 1) * (numberOfBlock + 1);
    } else {
        rxReadLen += (rwInfo->blockLen) * (numberOfBlock + 1);
    }
    return rxReadLen;
}

NfcErrorCode NfcHalT5tCmdBuild(uint8_t *txBuff, NfcHalT5TRWInfo *rwInfo, uint8_t cmdType,
                               const uint8_t *buff, uint16_t numberOfBlock)
{
    uint8_t idx = T5T_CMD_START_POS;
    uint16_t cmdLen = NfcHalT5tCmdLenHelper(cmdType);
    uint8_t reqFlagRes = MakeReqFlagHelper(&(rwInfo->reqFlag));
    txBuff[idx++] = reqFlagRes;
    txBuff[idx++] = cmdType;
    if (rwInfo->reqFlag.isAms) {
        if (memcpy_s(&txBuff[idx], cmdLen - idx, rwInfo->uid, UID_LEN) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        idx += UID_LEN;
    }
    if (cmdType >= T5T_READ_SINGLE_CMD && cmdType <= T5T_READ_MULT_CMD) {
        txBuff[idx++] = UTIL_Byte0(rwInfo->blockNo);
    } else if (cmdType >= T5T_EX_READ_SINGLE_CMD && cmdType <= T5T_EX_READ_MULT_CMD) {
        txBuff[idx++] = UTIL_Byte1(rwInfo->blockNo);
        txBuff[idx++] = UTIL_Byte0(rwInfo->blockNo);
    }
    if (cmdType == T5T_WRITE_SINGLE_CMD || cmdType == T5T_EX_WRITE_SINGLE_CMD) {
        if (memcpy_s(&txBuff[idx], cmdLen - idx, buff, rwInfo->blockLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
    } else if (cmdType == T5T_READ_MULT_CMD) {
        txBuff[idx++] = UTIL_Byte0(numberOfBlock);
    } else if (cmdType == T5T_EX_READ_MULT_CMD) {
        txBuff[idx++] = UTIL_Byte1(numberOfBlock);
        txBuff[idx++] = UTIL_Byte0(numberOfBlock);
    }
    return NFC_OK;
}

uint8_t MakeReqFlagHelper(NfcHalT5TReqFlag *reqFlag)
{
    uint8_t reqFlagRes = DEFAULT_REQ_FLAG;
    if (reqFlag->isOptionFlag) {
        reqFlagRes |= REQ_FLAG_OPTION_FLAG;
    }
    if (reqFlag->isAms) {
        reqFlagRes |= REQ_FLAG_AMS;
    }
    if (reqFlag->isSms) {
        reqFlagRes |= REQ_FLAG_SMS;
    }
    return reqFlagRes;
}

uint16_t NfcHalT5tCmdLenHelper(uint8_t cmdType)
{
    uint16_t cmdLen = 0;
    switch (cmdType) {
        case T5T_READ_SINGLE_CMD:
        case T5T_LOCK_SINGLE_CMD:
            cmdLen = T5T_SINGLE_CMD_LEN;
            break;
        case T5T_WRITE_SINGLE_CMD:
            cmdLen = T5T_MAX_BLOCK_LEN + T5T_SINGLE_CMD_LEN;
            break;
        case T5T_READ_MULT_CMD:
            cmdLen = T5T_READ_MULT_CMD_LEN;
            break;
        case T5T_EX_READ_SINGLE_CMD:
        case T5T_EX_LOCK_SINGLE_CMD:
            cmdLen = T5T_EX_SINGLE_CMD_LEN;
            break;
        case T5T_EX_WRITE_SINGLE_CMD:
            cmdLen = T5T_EX_SINGLE_CMD_LEN + T5T_MAX_BLOCK_LEN;
            break;
        case T5T_EX_READ_MULT_CMD:
            cmdLen = T5T_EX_READ_MULT_CMD_LEN;
            break;
        case T5T_SELECT_CMD:
            cmdLen = T5T_SELECT_CMD_LEN;
            break;
        case T5T_SLPV_CMD:
            cmdLen = T5T_SLPV_CMD_LEN;
            break;
        default:
            PS_PRINT_ERR("[T5T] ProcBuildCmd unsupported cmd:%xn", cmdType);
    }
    return cmdLen;
}

#ifdef __cplusplus
}
#endif