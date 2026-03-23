/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved. \n
 * Description: nfc hal T1T tag operations \n
 * Author: @CompanyNameTag \n
 * Date: 2022-11-11 \n
 */
#include "nfc_hal_t1t.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T1T_READ_RES_LEN                2
#define T1T_WRITE_E_RES_LEN             2
#define T1T_WRITE_NE_RES_LEN            2
#define T1T_RSEG_RES_LEN                129
#define T1T_READ8_RES_LEN               9
#define T1T_WRITE_E8_RES_LEN            9
#define T1T_WRITE_NE8_RES_LEN           9

#define T1T_READ_SHORT_DATA_MAX_LEN     16
#define T1T_READ_LONG_DATA_MAX_LEN      132
#define T1T_STATIC_MEM_CMD_LEN          7
#define T1T_STATIC_MEM_CMD_ADDR_POS     1
#define T1T_DYNAMIC_MEM_CMD_LEN         14
#define T1T_DYNAMIC_MEM_CMD_ADDR_POS    1

enum {
    T1T_RALL_CMD = 0x00,
    T1T_READ_CMD = 0x01,
    T1T_READ8_CMD = 0x02,
    T1T_RSEG_CMD = 0x10,
    T1T_WRITE_NE_CMD = 0x1A,
    T1T_WRITE_NE8_CMD = 0x1B,
    T1T_WRITE_E_CMD = 0x53,
    T1T_WRITE_E8_CMD = 0x54,
    T1T_RID_CMD = 0x78,
};

NfcErrorCode NFC_HAL_T1TPollerProcRid(NfcHalT1TRidRes *res)
{
    if (res == NULL) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_STATIC_MEM_CMD_LEN] = {T1T_RID_CMD};
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = sizeof(rxData);
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_RID_RES_LEN + 1 || rxData[T1T_RID_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T1T] ProcRid ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen, rxData[T1T_RID_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_RID_ERR;
    }

    // T1T use frame RF interface, shall ignore the last status byte.
    uint8_t idx = 0;
    res->hr0 = rxData[idx++];
    res->hr1 = rxData[idx++];
    if (memcpy_s(res->uid, T1T_UID_ECHO_LEN, rxData + idx, T1T_UID_ECHO_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcRall(const uint8_t *uid, uint8_t *rxBuff, uint16_t buffSize)
{
    if (uid == NULL || rxBuff == NULL || buffSize < T1T_RALL_RES_LEN) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_STATIC_MEM_CMD_LEN] = {T1T_RALL_CMD};
    if (memcpy_s(&txBuff[T1T_STATIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_LONG_DATA_MAX_LEN] = {0};
    uint16_t rxLen = sizeof(rxData);
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_RALL_RES_LEN + 1 || rxData[T1T_RALL_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T1T] ProcRall ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen, rxData[T1T_RALL_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_RALL_ERR;
    }

    // T1T use frame RF interface, shall ignore the last status byte.
    if (memcpy_s(rxBuff, buffSize, rxData, T1T_RALL_RES_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcRead(const uint8_t *uid, uint8_t addr, uint8_t *data)
{
    if (uid == NULL || data == NULL) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_STATIC_MEM_CMD_LEN] = {T1T_READ_CMD};
    txBuff[T1T_STATIC_MEM_CMD_ADDR_POS] = addr;
    if (memcpy_s(&txBuff[T1T_STATIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_READ_RES_LEN + 1 || rxData[T1T_READ_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T1T] ProcRead ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen, rxData[T1T_READ_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_READ_ERR;
    }

    *data = rxData[T1T_READ_RES_LEN - 1];
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcWriteE(const uint8_t *uid, uint8_t addr, uint8_t data)
{
    if (uid == NULL) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_STATIC_MEM_CMD_LEN] = {T1T_WRITE_E_CMD};
    uint8_t idx = T1T_STATIC_MEM_CMD_ADDR_POS;
    txBuff[idx++] = addr;
    txBuff[idx++] = data;
    if (memcpy_s(&txBuff[T1T_STATIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_WRITE_E_RES_LEN + 1 || rxData[T1T_WRITE_E_RES_LEN] == NCI_RF_FRAME_CORRUPTED ||
        data != rxData[T1T_WRITE_E_RES_LEN - 1]) {
        PS_PRINT_ERR("[T1T] ProcWriteE ret=0x%x, rxLen = %d, status = 0x%x, received_data = 0x%x\n", ret, rxLen,
                     rxData[T1T_WRITE_E_RES_LEN], rxData[T1T_WRITE_E_RES_LEN - 1]);
        return NFC_ERR_CODE_T1T_PROC_WRITE_E_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcWriteNE(const uint8_t *uid, uint8_t addr, uint8_t data)
{
    if (uid == NULL) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_STATIC_MEM_CMD_LEN] = {T1T_WRITE_NE_CMD};
    uint8_t idx = T1T_STATIC_MEM_CMD_ADDR_POS;
    txBuff[idx++] = addr;
    txBuff[idx++] = data;
    if (memcpy_s(&txBuff[T1T_STATIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_WRITE_NE_RES_LEN + 1 || rxData[T1T_WRITE_NE_RES_LEN] == NCI_RF_FRAME_CORRUPTED ||
        data != rxData[T1T_WRITE_NE_RES_LEN - 1]) {
        PS_PRINT_ERR("[T1T] ProcWriteNE ret=0x%x, rxLen = %d\n, status = 0x%x, received_data = 0x%x", ret, rxLen,
                     rxData[T1T_WRITE_NE_RES_LEN], rxData[T1T_WRITE_NE_RES_LEN - 1]);
        return NFC_ERR_CODE_T1T_PROC_WRITE_NE_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcRseg(const uint8_t *uid, uint8_t addrS, uint8_t *rxBuff, uint16_t buffSize)
{
    if (uid == NULL || rxBuff == NULL || buffSize < T1T_RSEG_RES_DATA_LEN) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_DYNAMIC_MEM_CMD_LEN] = {T1T_RSEG_CMD};
    txBuff[T1T_DYNAMIC_MEM_CMD_ADDR_POS] = addrS;
    if (memcpy_s(&txBuff[T1T_DYNAMIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) !=
        EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_LONG_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_LONG_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_RSEG_RES_LEN + 1 || rxData[T1T_RSEG_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T1T] ProcRseg ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen, rxData[T1T_RSEG_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_RSEG_ERR;
    }

    /* T1T use frame RF interface, shall ignore the last status byte.
     * Also ignore the 1st ADDS byte. Just copy data to upper layer. */
    if (memcpy_s(rxBuff, buffSize, rxData + 1, T1T_RSEG_RES_DATA_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcRead8(const uint8_t *uid, uint8_t addr8, uint8_t *rxBuff, uint16_t buffSize)
{
    if (uid == NULL || rxBuff == NULL || buffSize < T1T_READ8_RES_DATA_LEN) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_DYNAMIC_MEM_CMD_LEN] = {T1T_READ8_CMD};
    txBuff[T1T_DYNAMIC_MEM_CMD_ADDR_POS] = addr8;
    if (memcpy_s(&txBuff[T1T_DYNAMIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) !=
        EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_READ8_RES_LEN + 1 || rxData[T1T_READ8_RES_LEN] == NCI_RF_FRAME_CORRUPTED) {
        PS_PRINT_ERR("[T1T] ProcRead8 ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen, rxData[T1T_READ8_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_READ8_ERR;
    }

    /* T1T use frame RF interface, shall ignore the last status byte.
     * Also ignore the 1st ADD8 byte. Just copy data to upper layer. */
    if (memcpy_s(rxBuff, buffSize, rxData + 1, T1T_READ8_RES_DATA_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcWriteE8(const uint8_t *uid, uint8_t addr8, const uint8_t *buff, uint16_t buffLen)
{
    if (uid == NULL || buff == NULL || buffLen != T1T_WRITE_E8_DATA_LEN) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_DYNAMIC_MEM_CMD_LEN] = {T1T_WRITE_E8_CMD};
    uint8_t idx = T1T_DYNAMIC_MEM_CMD_ADDR_POS;
    txBuff[idx++] = addr8;
    if (memcpy_s(&txBuff[idx], T1T_DYNAMIC_MEM_CMD_LEN - idx, buff, T1T_WRITE_E8_DATA_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    if (memcpy_s(&txBuff[T1T_DYNAMIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) !=
        EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_WRITE_E8_RES_LEN + 1 || rxData[T1T_WRITE_E8_RES_LEN] == NCI_RF_FRAME_CORRUPTED ||
        memcmp(buff, &rxData[1], T1T_WRITE_E8_DATA_LEN) != 0) {
        PS_PRINT_ERR("[T1T] ProcWriteE8 ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen,
                     rxData[T1T_WRITE_E8_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_WRITE_E8_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_T1TPollerProcWriteNE8(const uint8_t *uid, uint8_t addr8, const uint8_t *buff, uint16_t buffLen)
{
    if (uid == NULL || buff == NULL || buffLen != T1T_WRITE_NE8_DATA_LEN) {
        return NFC_ERR_CODE_T1T_INPUT_PARAM_INVALID_ERR;
    }

    uint8_t txBuff[T1T_DYNAMIC_MEM_CMD_LEN] = {T1T_WRITE_NE8_CMD};
    uint8_t idx = T1T_DYNAMIC_MEM_CMD_ADDR_POS;
    txBuff[idx++] = addr8;
    if (memcpy_s(&txBuff[idx], T1T_DYNAMIC_MEM_CMD_LEN - idx, buff, T1T_WRITE_NE8_DATA_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    if (memcpy_s(&txBuff[T1T_DYNAMIC_MEM_CMD_LEN - T1T_UID_ECHO_LEN], T1T_UID_ECHO_LEN, uid, T1T_UID_ECHO_LEN) !=
        EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    uint8_t rxData[T1T_READ_SHORT_DATA_MAX_LEN] = {0};
    uint16_t rxLen = T1T_READ_SHORT_DATA_MAX_LEN;
    NfcErrorCode ret = NFC_HAL_TranceiveUnchainedData(txBuff, sizeof(txBuff), NCI_STATIC_RF_CONN, rxData, &rxLen);
    if (ret != NFC_OK || rxLen != T1T_WRITE_NE8_RES_LEN + 1 ||
        rxData[T1T_WRITE_NE8_RES_LEN] == NCI_RF_FRAME_CORRUPTED ||
        memcmp(buff, &rxData[1], T1T_WRITE_NE8_DATA_LEN) != 0) {
        PS_PRINT_ERR("[T1T] ProcWriteNE8 ret=0x%x, rxLen = %d, status = 0x%x\n", ret, rxLen,
                     rxData[T1T_WRITE_NE8_RES_LEN]);
        return NFC_ERR_CODE_T1T_PROC_WRITE_NE8_ERR;
    }

    return NFC_OK;
}

#ifdef __cplusplus
}
#endif