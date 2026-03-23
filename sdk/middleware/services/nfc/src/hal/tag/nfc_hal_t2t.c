/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc hal T2T tag operations \n
 * Author: @CompanyNameTag \n
 * Date: 2022-11-15 \n
 */
#include "nfc_hal_t2t.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T2T_WRITE_CMD_LEN           6
#define T2T_ACK_NACK_RSP_LEN        1
#define T2T_NCI_RSP_MAX_LEN         17

#define T2T_READ                    0x30
#define T2T_WRITE                   0xA2
#define T2T_SECTOR_SELECT           0xC2
#define T2T_ACK_RSP                 0x0A
#define T2T_SEC_NUM_RFU             0xFF
#define T2T_SECTOR_SELECT_BYTE_2    0xFF

#define FLOW_CTRL_WAIT_TIMEOUT      50
// consider about the slow response of the main control
#define SECTOR_SELECTT_WAIT_TIMEOUT 20

NfcErrorCode NFC_HAL_T2TPollerProcRead(uint8_t blockNo, uint8_t *readData, uint16_t dataBuffSize)
{
    if (readData == NULL || dataBuffSize < T2T_READ_DATA_LEN) {
        return NFC_ERR_CODE_T2T_INPUT_PARAM_INVALID;
    }
    uint8_t txBuff[] = {T2T_READ, blockNo};
    uint8_t rxBuff[T2T_NCI_RSP_MAX_LEN] = {0};
    uint16_t rxLen = T2T_NCI_RSP_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T2T_READ_DATA_LEN + 1 || rxBuff[T2T_READ_DATA_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T2T] ProcRead ret=0x%x, rxLen = %d, RSP = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T2T_READ_DATA_LEN]);
        return NFC_ERR_CODE_T2T_PROC_READ_ERR;
    }
    if (memcpy_s(readData, dataBuffSize, rxBuff, rxLen - 1) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T2TPollerProcWrite(uint8_t blockNo, const uint8_t *writeData, uint8_t dataLen)
{
    if (writeData == NULL || dataLen != T2T_WRITE_DATA_LEN) {
        return NFC_ERR_CODE_T2T_INPUT_PARAM_INVALID;
    }
    uint8_t txBuff[T2T_WRITE_CMD_LEN] = {0};
    uint16_t idx = 0;
    txBuff[idx++] = T2T_WRITE;
    txBuff[idx++] = blockNo;
    if (memcpy_s(&txBuff[idx], T2T_WRITE_CMD_LEN - idx, writeData, dataLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxBuff[T2T_NCI_RSP_MAX_LEN] = {0};
    uint16_t rxLen = T2T_NCI_RSP_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T2T_ACK_NACK_RSP_LEN + 1 ||
        rxBuff[0] != T2T_ACK_RSP || rxBuff[T2T_ACK_NACK_RSP_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T2T] ProcWrite ret=0x%x, rxLen = %d, RSP = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T2T_ACK_NACK_RSP_LEN]);
        return NFC_ERR_CODE_T2T_PROC_WRITE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T2TPollerProcSelectSector(uint8_t secNo)
{
    if (secNo == T2T_SEC_NUM_RFU) {
        return NFC_ERR_CODE_T2T_INPUT_PARAM_INVALID;
    }
    uint8_t txBuff1[] = {T2T_SECTOR_SELECT, T2T_SECTOR_SELECT_BYTE_2};
    uint8_t rxBuff[T2T_NCI_RSP_MAX_LEN] = {0};
    uint16_t rxLen = T2T_NCI_RSP_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff1, sizeof(txBuff1), NCI_STATIC_RF_CONN, rxBuff, &rxLen);
    if (ret != NFC_OK || rxLen != T2T_ACK_NACK_RSP_LEN + 1 ||
        rxBuff[0] != T2T_ACK_RSP || rxBuff[T2T_ACK_NACK_RSP_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T2T] ProcSelectSector ret=0x%x, rxLen = %d, RSP = %x, status = %x\n",
                     ret, rxLen, rxBuff[0], rxBuff[T2T_ACK_NACK_RSP_LEN]);
        return NFC_ERR_CODE_T2T_PROC_SECTOR_SELECT_PACKET_1_ERR;
    }
    uint8_t txBuff2[] = {secNo, 0x00, 0x00, 0x00};
    ret = NFC_HAL_SendData(txBuff2, sizeof(txBuff2), NCI_STATIC_RF_CONN);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[T2T] ProcSelectSector ret=0x%x\n", ret);
        return NFC_ERR_CODE_T2T_PROC_SECTOR_SELECT_PACKET_2_SEND_ERR;
    }
    uint8_t nciData[HAL_MAX_BUF_LEN] = {0};
    rxLen = HAL_MAX_BUF_LEN;
    ret = NFC_HAL_ReceiveNciPkt(nciData, &rxLen, FLOW_CTRL_WAIT_TIMEOUT);
    if (ret != NFC_OK || rxLen < NCI_HEAD_LEN) {
        PS_PRINT_ERR("[T2T] ProcSelectSector rxLen = %d\n", rxLen);
        return NFC_ERR_CODE_T2T_PROC_SECTOR_SELECT_PACKET_2_RCV_FLOW_CTRL_ERR;
    }
    rxLen = HAL_MAX_BUF_LEN;
    ret = NFC_HAL_ReceiveNciPkt(nciData, &rxLen, SECTOR_SELECTT_WAIT_TIMEOUT);
    // if reader/writer receive any RSP in PAT, it means select sector fail
    if (ret == NFC_OK && rxLen >= NCI_HEAD_LEN) {
        PS_PRINT_ERR("[T2T] ProcSelectSector rxLen = %d, RSP = %x\n", rxLen, nciData[NCI_HEAD_LEN]);
        return NFC_ERR_CODE_T2T_PROC_SECTOR_SELECT_PACKET_2_WAIT_TIMEOUT_ERR;
    }

    return NFC_OK;
}

#ifdef __cplusplus
}
#endif