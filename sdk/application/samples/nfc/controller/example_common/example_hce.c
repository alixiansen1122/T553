/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc hce example, EXAMPLEHostCardEmulation should enable build macro HAVE_NFC_LISTEN \n
 * Author: @CompanyNameTag \n
 * Date: 2025-07-02 \n
 */
#include "example_hce.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T4T_NDEF_HEAD_LEN                  2
#define T4T_READ_NDEF_LEN_CMD_RSP_LEN      4
#define T4T_CMD_TYPE                       4
#define T4T_SELECT_CMD_TYPE                5
#define T4T_READ_NDEF_OFFSET               6
#define T4T_READ_LEN                       7
#define T4T_RSP_LEN                        2
#define AID_ADDR_OFFSET                    5
#define NCI_DATA_BEGIN                     3
#define AID_LEN                            7
#define CC_LEN                             15
#define T4T_CMD_CLA                        0x00
#define T4T_SELECT_CMD_INS                 0xA4
#define T4T_READ_CMD_INS                   0xB0
#define T4T_READ_CC_LEN                    0x0F
#define T4T_READ_NDEF_HEAD_LEN             0x02
#define T4T_SELECT_AID_CMD                 0x04
#define T4T_SELECT_CC_AND_NDEF_CMD         0x00
#define T4T_SELECT_CC_DATA                 0xE103
#define T4T_SELECT_NDEF_DATA               0xE104
#define NFC_DISCOVERY_TIME                 0x7FFFFFFF

uint8_t g_ndefArray[] = {
    // 淘宝
    0x03, 0x25,
    0xd4, 0x0f, 0x11, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x61,
    0x6f, 0x62, 0x61, 0x6f, 0x2e, 0x74, 0x61, 0x6f,
    0x62, 0x61, 0x6f, 0x90, 0x00,
};

uint8_t g_sendAcceptBuff[T4T_RSP_LEN] = {0x90, 0x00};
uint8_t g_sendRejectBuff[T4T_RSP_LEN] = {0x6A, 0x82};

NfcErrorCode NFC_EXAMPLE_HostDevSendRsp(uint8_t rspStatus)
{
    if (rspStatus) {
        return NFC_HAL_SendData(g_sendAcceptBuff, T4T_RSP_LEN, NCI_STATIC_RF_CONN);
    } else {
        return NFC_HAL_SendData(g_sendRejectBuff, T4T_RSP_LEN, NCI_STATIC_RF_CONN);
    }
}

NfcErrorCode NFC_EXAMPLE_ProcessHostCardEmuOnce(unsigned long timeout)
{
    uint8_t recvBuff[HAL_MAX_BUF_LEN];
    uint8_t ndefLen = g_ndefArray[1];
    uint8_t sendRspBuff[HAL_MAX_BUF_LEN];
    uint8_t selectAidRspBuff[AID_LEN] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
    uint8_t readCcRspBuff[CC_LEN] = {0x00, 0x0F, 0x20, 0x00, 0xF0, 0x00, 0xF0, 0x04,
                                     0x06, 0xE1, 0x04, 0x00, 0xFF, 0x00, 0xFF};
    int ret = HW_NFC_NciRead(recvBuff, HAL_MAX_BUF_LEN, timeout);
    if (ret < 0) {
        return NFC_ERR_CODE_PKT_RECV_I2C_ERR;
    }
    if (recvBuff[NCI_DATA_BEGIN] != T4T_CMD_CLA) {
        return NFC_ERR_CODE_DISCOV_NTF_PLD_ERR;
    }

    uint16_t recvLen = recvBuff[NCI_LEN_POS];
    if (recvBuff[T4T_CMD_TYPE] == T4T_SELECT_CMD_INS) {
        if (recvBuff[T4T_SELECT_CMD_TYPE] == T4T_SELECT_AID_CMD) {  // Receive select aid cmd and response
            ret = NFC_EXAMPLE_HostDevSendRsp(
                memcmp(recvBuff + NCI_HEAD_LEN + AID_ADDR_OFFSET, selectAidRspBuff, AID_LEN) == 0);
        // Receive select ndef and cc cmd and response
        } else if (recvBuff[T4T_SELECT_CMD_TYPE] == T4T_SELECT_CC_AND_NDEF_CMD) {
            uint16_t selectData = UTIL_Combine8bitTo16bit(
                recvBuff[NCI_HEAD_LEN + recvLen - 2], recvBuff[NCI_HEAD_LEN + recvLen - 1]);
            ret = NFC_EXAMPLE_HostDevSendRsp(selectData == T4T_SELECT_CC_DATA || selectData == T4T_SELECT_NDEF_DATA);
        }
    } else if (recvBuff[T4T_CMD_TYPE] == T4T_READ_CMD_INS) {
        if (recvBuff[T4T_READ_LEN] == T4T_READ_CC_LEN) {  // Receive read cc cmd and response
            if (memcpy_s(sendRspBuff, HAL_MAX_BUF_LEN, readCcRspBuff, CC_LEN) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            if (memcpy_s(sendRspBuff + CC_LEN, HAL_MAX_BUF_LEN - CC_LEN, g_sendAcceptBuff, T4T_RSP_LEN) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            ret = NFC_HAL_SendData(sendRspBuff, CC_LEN + T4T_RSP_LEN, NCI_STATIC_RF_CONN);
        } else if (recvBuff[T4T_READ_LEN] == T4T_READ_NDEF_HEAD_LEN) {  // Receive read ndef lengthcmd and response
            uint8_t ndefLenRsp[T4T_READ_NDEF_LEN_CMD_RSP_LEN] = {0x00, ndefLen - T4T_NDEF_HEAD_LEN, 0x90, 0x00};
            ret = NFC_HAL_SendData(ndefLenRsp, T4T_READ_NDEF_LEN_CMD_RSP_LEN, NCI_STATIC_RF_CONN);
        } else {  // Receive read ndef cmd and response
            uint8_t offset = recvBuff[T4T_READ_NDEF_OFFSET];
            uint8_t sendLen = recvBuff[T4T_READ_LEN];
            if (memcpy_s(sendRspBuff, HAL_MAX_BUF_LEN, g_ndefArray + offset, sendLen) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            if (memcpy_s(sendRspBuff + sendLen, HAL_MAX_BUF_LEN - sendLen, g_sendAcceptBuff, T4T_RSP_LEN) != EOK) {
                return NFC_ERR_CODE_MEMCPY_FAIL;
            }
            ret = NFC_HAL_SendData(sendRspBuff, ndefLen, NCI_STATIC_RF_CONN);
        }
    }
    return ret;
}

NfcErrorCode NFC_EXAMPLE_HceInit(void)
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

    uint8_t recvBuff[HAL_MAX_BUF_LEN];
    uint16_t rspLen = HAL_MAX_BUF_LEN;

    uint8_t cfgT4TCmd[] = {0x20, 0x02, 0x0A, 0x03, 0x30, 0x01, 0x04, 0x31, 0x01, 0x00, 0x32, 0x01, 0x20};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(cfgT4TCmd, sizeof(cfgT4TCmd), recvBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    rspLen = HAL_MAX_BUF_LEN;
    uint8_t setRoutingCmd[] = {0x21, 0x01, 0x09, 0x00, 0x01, 0x00, 0x03, 0x00, 0x3B, 0x00, 0x03, 0x03};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(setRoutingCmd, sizeof(setRoutingCmd), recvBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_EXAMPLE_StartHostCardEmulation(void)
{
    PS_PRINT_INFO("Nfc host card emulation task main");
    NfcErrorCode ret = NFC_EXAMPLE_HceInit();
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t list[] = {NFC_NCI_A_PASSIVE_LISTEN_MODE};
    NFC_HAL_StartDiscovery(list, sizeof(list));

    while (true) {
        ret = NFC_EXAMPLE_ProcessHostCardEmuOnce(NFC_DISCOVERY_TIME);
        if (ret == NFC_ERR_CODE_PKT_RECV_I2C_ERR) {
            break;
        }
    }

    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }

    // May choose to enter idle sleep for lowest power mode as "NFC close".
    return NFC_HAL_EnterIdleSleep();
}

#ifdef __cplusplus
}
#endif
