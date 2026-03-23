/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc hal T3T tag operations \n
 * Author: @CompanyNameTag \n
 * Date: 2022-11-15 \n
 */
#include "nfc_hal_t3t.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T3T_SOD_LEN                      1
#define T3T_CMD_ID_LEN                   1
#define T3T_CMD_HEAD_LEN                 (T3T_SOD_LEN + T3T_CMD_ID_LEN)
#define T3T_SERVICE_CODE_SIZE            2
#define T3T_CHECK_RSP_STRUCT_BUFF_LEN    219
#define T3T_RSP_MIN_LEN                  12

#define T3T_CHECK_CMD_HEAD           0x06
#define T3T_UPDATE_CMD_HEAD          0x08

NfcErrorCode NFC_HAL_T3TSetCommCmd(NfcHalT3tCmdParams *cmdParams, uint8_t cmdHead, uint8_t *cmd, uint16_t *cmdLen)
{
    // First byte is reserved for SOD
    uint16_t idx = 1;
    cmd[idx++] = cmdHead;
    if (memcpy_s(&cmd[idx], *cmdLen - idx, cmdParams->nfcid2, NFCID2_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += NFCID2_LEN;
    cmd[idx++] = cmdParams->numberOfServices;
    uint16_t scListLen = cmdParams->numberOfServices * T3T_SERVICE_CODE_SIZE;
    if (memcpy_s(&cmd[idx], *cmdLen - idx, cmdParams->serviceCodeList, scListLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += scListLen;
    cmd[idx++] = cmdParams->numberOfBlock;
    if (memcpy_s(&cmd[idx], *cmdLen - idx, cmdParams->blockList, cmdParams->blockListLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += cmdParams->blockListLen;
    *cmdLen = idx;
    cmd[0] = idx;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T3TGetCheckRsp(uint8_t numberOfBlock, NfcHalT3tCheckRspParams *rspParams, uint8_t *rsp,
                                    uint8_t rspLen)
{
    if (rspLen < T3T_RSP_MIN_LEN) {
        return NFC_ERR_CODE_T3T_CHECK_RSP_DECODE_LEN_ERR;
    }
    uint8_t idx = T3T_CMD_HEAD_LEN;
    if (memcpy_s(rspParams->nfcid2, NFCID2_LEN, &rsp[idx], NFCID2_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += NFCID2_LEN;
    rspParams->flag1 = rsp[idx++];
    rspParams->flag2 = rsp[idx++];
    if (rspParams->flag1 != NFC_OK) {
        return NFC_ERR_CODE_T3T_CHECK_RSP_DECODE_FLAG1_ERR;
    }
    rspParams->numberOfBlock = rsp[idx++];
    if (rspParams->numberOfBlock != numberOfBlock) {
        return NFC_ERR_CODE_T3T_CHECK_RSP_DECODE_BLK_NUM_ERR;
    }
    uint16_t blockLen = rspParams->numberOfBlock * T3T_BLOCK_SIZE;
    if (blockLen + idx > rspLen) {
        return NFC_ERR_CODE_T3T_CHECK_RSP_DECODE_LEN_ERR;
    }
    if (memcpy_s(rspParams->blockData, T3T_BLOCK_BUFF_MAX_LEN, &rsp[idx], blockLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T3TGetUpdateRsp(NfcHalT3tUpdateRspParams *rspParams, uint8_t *rsp, uint8_t rspLen)
{
    if (rspLen < T3T_RSP_MIN_LEN) {
        return NFC_ERR_CODE_T3T_INPUT_PARAM_INVALID;
    }
    uint8_t idx = T3T_CMD_HEAD_LEN;
    if (memcpy_s(rspParams->nfcid2, NFCID2_LEN, &rsp[idx], NFCID2_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += NFCID2_LEN;
    rspParams->flag1 = rsp[idx++];
    rspParams->flag2 = rsp[idx++];
    if (rspParams->flag1 != 0x0) {
        return NFC_ERR_CODE_T3T_UPDATE_RSP_DECODE_FLAG1_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T3TPollCheckCmd(NfcHalT3tCmdParams *cmdParams, NfcHalT3tCheckRspParams *rspParams)
{
    if (cmdParams == NULL || rspParams == NULL || cmdParams->numberOfBlock > T3T_BLOCKS_MAX_NUM) {
        return NFC_ERR_CODE_T3T_INPUT_PARAM_INVALID;
    }

    uint8_t rxBuff[NCI_MAX_PAYLOAD_LEN];
    uint16_t rxLen = NCI_MAX_PAYLOAD_LEN;

    uint8_t txBuff[NCI_MAX_PAYLOAD_LEN];
    uint16_t txLen = NCI_MAX_PAYLOAD_LEN;
    NfcErrorCode ret = NFC_HAL_T3TSetCommCmd(cmdParams, T3T_CHECK_CMD_HEAD, txBuff, &txLen);
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || memcmp(&txBuff[T3T_CMD_HEAD_LEN], &rxBuff[T3T_CMD_HEAD_LEN], NFCID2_LEN) != 0) {
        PS_PRINT_ERR("[T3T] ProcCheck ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T3T_CHECK_CMD_RSP_ERR;
    }

    ret = NFC_HAL_T3TGetCheckRsp(cmdParams->numberOfBlock, rspParams, rxBuff, rxLen);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T3T] ProcCheck ret=0x%x, rxLen = %d\n", ret, rxLen);
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T3TPollUpdateCmd(NfcHalT3tCmdParams *cmdParams, NfcHalT3tUpdateRspParams *rspParams,
                                      const uint8_t *writeData, uint8_t writeDataLen)
{
    if (cmdParams == NULL || rspParams == NULL || writeData == NULL || (writeDataLen % T3T_BLOCK_SIZE != 0) ||
        writeDataLen > T3T_BLOCK_BUFF_MAX_LEN) {
        return NFC_ERR_CODE_T3T_INPUT_PARAM_INVALID;
    }

    uint8_t rxBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rxLen = HAL_SHORT_BUFF_LEN;

    uint8_t txBuff[NCI_MAX_PAYLOAD_LEN];
    uint16_t txLen = NCI_MAX_PAYLOAD_LEN;
    NfcErrorCode ret = NFC_HAL_T3TSetCommCmd(cmdParams, T3T_UPDATE_CMD_HEAD, txBuff, &txLen);
    if (ret != NFC_OK) {
        return ret;
    }
    if (memcpy_s(&txBuff[txLen], NCI_MAX_PAYLOAD_LEN - txLen, writeData, writeDataLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    txBuff[0] += writeDataLen;

    ret = NFC_HAL_TranceiveUnchainedData(txBuff, txLen + writeDataLen, NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || memcmp(&txBuff[T3T_CMD_HEAD_LEN], &rxBuff[T3T_CMD_HEAD_LEN], NFCID2_LEN) != 0) {
        PS_PRINT_ERR("[T3T] ProcUpdate ret=0x%x, rxLen = %d\n", ret, rxLen);
        return NFC_ERR_CODE_T3T_UPDATE_CMD_RSP_ERR;
    }

    ret = NFC_HAL_T3TGetUpdateRsp(rspParams, rxBuff, rxLen);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T3T] ProcUpdate ret=0x%x, rxLen = %d\n", ret, rxLen);
        return ret;
    }

    return NFC_OK;
}

#ifdef __cplusplus
}
#endif
