/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc raw data exchange sample, should enable build macro HAVE_NFC_LISTEN & HAVE_NFC_POLL\n
 * Author: @CompanyNameTag \n
 * Date: 2025-07-04 \n
 */

#include "sample_rawdata_exchange.h"
#include "cmsis_os2.h"
#include "nfc_hal_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_RAWDATA_EXCHANGE_SAMPLE_TASK_STACK_SIZE     0x800
#define NFC_RAWDATA_EXCHANGE_SAMPLE_TASK_PRIO           (osPriority_t)(17)

#define NFC_WAIT_NEXT_BUSINESS_TO                       500
#define NFC_DISCOVERY_TIME                              0x7FFFFFFF
#define NCI_INTF_ACT_NTF_HEAD                           0x6105

typedef enum {
    NFC_RW_MODE,
    NFC_HCE_MODE,
} NfcWorkMode;

NfcWorkMode g_nfcRawDataExchangeWorkMode = NFC_RW_MODE;

static void *NFC_SAMPLE_RawDataExchangeThread(const char *arg)
{
    UNUSED(arg);
    while (true) {
        NfcErrorCode ret = NFC_HAL_Open();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc open fail, exit nfc process!\r\n");
            return NULL;
        }
        do {
            switch (g_nfcRawDataExchangeWorkMode) {
#ifdef HAVE_NFC_POLL
                case NFC_RW_MODE:
                    ret = NFC_SAMPLE_RawDataExchangeRwTaskMain();
                    break;
#endif
#ifdef HAVE_NFC_LISTEN
                case NFC_HCE_MODE:
                    ret = NFC_SAMPLE_RawDataExchangeHceTaskMain();
                    break;
#endif
                default:
                    break;
            }
            NFC_HAL_Msleep(NFC_WAIT_NEXT_BUSINESS_TO);
        } while (ret == NFC_OK);
        PS_PRINT_ERR("nfc business error = 0x%x\r\n", ret);
        ret = NFC_HAL_Close();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc close fail, exit nfc process!\r\n");
            return NULL;
        }
    }
}

/* 读卡器模式，负责发起数据交互 */
NfcErrorCode NFC_SAMPLE_RawDataExchangeRwTaskMain(void)
{
    PS_PRINT_INFO("Nfc RW task main");
    uint8_t recvBuffer[HAL_MAX_BUF_LEN];
    uint8_t sendBuffer[HAL_MAX_BUF_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    uint16_t rspLen = HAL_MAX_BUF_LEN;
    NfcIntfActInfo intfActInfo = {0};

    NfcErrorCode ret = NFC_EXAMPLE_RwInit();
    if (ret != NFC_OK) {
        return ret;
    }

    // Example reader start discovery with tech A & B & F & V.
    ret = NFC_EXAMPLE_StartPolling();
    if (ret != NFC_OK) {
        return ret;
    }

    // Discovery time = 0xFFFFFFFF is wait-forever
    ret = NFC_HAL_PollRecvIntfActNtf(PRIO_FIRST, recvBuffer, sizeof(recvBuffer), NFC_DISCOVERY_TIME);
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_HAL_PollParseIntfActNtf(&intfActInfo, &recvBuffer[NCI_HEAD_LEN], recvBuffer[NCI_LEN_POS]);
    if (ret != NFC_OK) {
        return ret;
    }

    // Data exchange option: Raw data exchange
    // sendBuffer should be set by User, and recvBuffer should be checked by User.
    // During data exchange period NFC_HAL_SendDataAndRecvData may be executed several times. */
    ret = NFC_HAL_TranceiveUnchainedData(sendBuffer, 0x0A, NCI_STATIC_RF_CONN, recvBuffer, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    // Exit NFC reader mode
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }

    // May choose to enter idle sleep for lowest power mode as "NFC close".
    return NFC_HAL_EnterIdleSleep();
}

/* host卡模拟模式，负责响应读卡器端的数据并回复 */
NfcErrorCode NFC_SAMPLE_RawDataExchangeHceTaskMain(void)
{
    PS_PRINT_INFO("Nfc host card emulation task main");
    NfcErrorCode ret = NFC_EXAMPLE_HceInit();
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t list[] = {NFC_NCI_A_PASSIVE_LISTEN_MODE};
    NFC_HAL_StartDiscovery(list, sizeof(list));

    uint8_t recvBuff[HAL_MAX_BUF_LEN];
    uint16_t rspLen = HAL_MAX_BUF_LEN;
    uint8_t sendBuffer[HAL_MAX_BUF_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    ret = NFC_HAL_ReceiveNciPkt(recvBuff, &rspLen, NFC_DISCOVERY_TIME); // 读空口激活帧
    if (ret != NFC_OK || UTIL_Combine8bitTo16bit(recvBuff[0], recvBuff[1]) != NCI_INTF_ACT_NTF_HEAD) {
        return NFC_ERR_CODE_DATA_RECV_ERR;
    }

    rspLen = HAL_MAX_BUF_LEN;
    ret = NFC_HAL_RecvUnchainedData(recvBuff, &rspLen); // 先收再发
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DATA_RECV_ERR;
    }

    rspLen = HAL_MAX_BUF_LEN;
    ret = NFC_HAL_TranceiveUnchainedData(sendBuffer, 0x10, NCI_STATIC_RF_CONN, recvBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_HAL_Deactivate();
}

static void NFC_SAMPLE_RawDataExchangeEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcRawDataExchangeSampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_RAWDATA_EXCHANGE_SAMPLE_TASK_STACK_SIZE;
    attr.priority = NFC_RAWDATA_EXCHANGE_SAMPLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NFC_SAMPLE_RawDataExchangeThread, NULL, &attr) == NULL) {
        PS_PRINT_ERR("nfc rawdata exchange task create fail!\n");
        return;
    }
}

#ifdef __cplusplus
}
#endif
