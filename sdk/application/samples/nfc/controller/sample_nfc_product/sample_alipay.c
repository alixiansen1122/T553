/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc alipay sample, should enable build macro HAVE_NFC_POLL \n
 * Author: @CompanyNameTag \n
 * Date: 2025-06-25 \n
 */
#include "sample_alipay.h"
#include "cmsis_os2.h"
#include "nfc_hal_core.h"
#include "securec.h"
#include "nfc_hal_t2t.h"
#include "nfc_utils.h"
#include "nfc_hal_adapter.h"
#include "example_rw.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_ALIPAY_SAMPLE_TASK_STACK_SIZE               0x800
#define NFC_ALIPAY_SAMPLE_TASK_PRIO                     (osPriority_t)(17)

#define NFC_WAIT_NEXT_BUSINESS_TO                       500
#define DATA_BUF_SIZE                                   300
#define NFC_DISCOVERY_TIME                              0x7FFFFFFF
#define ALIPAY_TOKEN_LEN         50
#define T2T_READ_RETRY_TIME      3

static void *NFC_SAMPLE_AlipayThread(const char *arg)
{
    UNUSED(arg);
    while (true) {
        NfcErrorCode ret = NFC_HAL_Open();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc open fail, exit nfc process!\r\n");
            return NULL;
        }
        do {
#ifdef HAVE_NFC_POLL
            ret = NFC_SAMPLE_AlipayStartRw();
#endif
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

NfcErrorCode NFC_SAMPLE_AlipayEncodeData(uint8_t *txBuff, uint16_t txBuffLen, uint8_t *data, uint16_t len,
    uint8_t *flag)
{
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    uint16_t rxLen = T2T_READ_DATA_LEN;
    NfcErrorCode ret = NFC_HAL_T2TPollerProcRead(4, rxBuff, rxLen); // T2T NDEF内容从block 4开始
    if (ret != NFC_OK) {
        return ret;
    }
    // 发送给支付宝pos的帧格式
    uint8_t idx = 0;
    if (rxBuff[0] == 0x01) {
        txBuff[idx++] = rxBuff[0x04];
        *flag = 1;
    }
    txBuff[idx++]= 0x03;
    txBuff[idx++]= 0x00;
    txBuff[idx++] = 0xd1;
    txBuff[idx++] = 0x01;
    txBuff[idx++] = len + 0x03;
    txBuff[idx++] = 0x54;
    txBuff[idx++] = 0x02;
    txBuff[idx++] = 0x65;
    txBuff[idx++] = 0x6e;
    if (memcpy_s((txBuff + idx), txBuffLen - idx, data, len) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += len;
    if (memset_s(txBuff + idx, txBuffLen - idx, 0, 0x4) != EOK) {
        return NFC_ERR_CODE_MEMSET_FAIL;
    }
    return NFC_OK;
}

NfcErrorCode NFC_SAMPLE_AlipaySendData(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        return NFC_ERR_CODE_INVALID_PARAMETER;
    }
    uint8_t flag = 0;
    uint8_t txBuff[HAL_MAX_BUF_LEN] = {0};
    NfcErrorCode ret = NFC_SAMPLE_AlipayEncodeData(txBuff, sizeof(txBuff), data, len, &flag);
    if (ret != NFC_OK) {
        return ret;
    }
    uint8_t writeBlockIndex = (flag == 1 ? 5 : 4); // 根据读取到的内容从block 4或block 5开始写
    uint8_t txLen = (flag == 1 ? len + 10 : len + 9); // 根据读取到的内容帧头长度为9或10
    uint8_t sendLen = 0;
    while (sendLen < txLen) {
        ret = NFC_HAL_T2TPollerProcWrite(writeBlockIndex, txBuff + sendLen, T2T_WRITE_DATA_LEN);
        if (ret != NFC_OK) {
            return ret;
        }
        writeBlockIndex++;
        sendLen += T2T_WRITE_DATA_LEN;
    }
    // 根据读取内容重写block 4或block 5
    if (flag == 1) {
        txBuff[0x02] = len + 0x07;
        ret = NFC_HAL_T2TPollerProcWrite(5, txBuff, T2T_WRITE_DATA_LEN); // 重写block 5
        if (ret != NFC_OK) {
            return ret;
        }
    } else {
        txBuff[0x01] = len + 0x07;
        ret = NFC_HAL_T2TPollerProcWrite(4, txBuff, T2T_WRITE_DATA_LEN); // 重写block 4
        if (ret != NFC_OK) {
            return ret;
        }
    }
    txBuff[txLen] = 0xFE; // last control TLV
    writeBlockIndex--; // 重写最后一个block
    sendLen -= T2T_WRITE_DATA_LEN;
    ret = NFC_HAL_T2TPollerProcWrite(writeBlockIndex, txBuff + sendLen, T2T_WRITE_DATA_LEN);
    if (ret != NFC_OK) {
        return ret;
    }

    PS_PRINT_INFO("[TagOperation] Alipay data send succ!\n");
    return NFC_OK;
}

NfcErrorCode NFC_SAMPLE_AlipayProcessT2T(void)
{
    uint8_t rxBuff[T2T_READ_DATA_LEN] = {0};
    uint16_t rxLen = sizeof(rxBuff);
    NfcErrorCode ret;
    // T2T 循环读取block 0x04-0x4F，支付宝pos上需要读取的内容存放在些block内
    uint8_t retry = 0;
    for (uint8_t i = 0x4; i < 0x50; i += 0x4) {
        for (retry = 0; retry < T2T_READ_RETRY_TIME; retry++) { // T2T交互帧数较多，空口可能出现异常，若有异常最多重试3次
            ret = NFC_HAL_T2TPollerProcRead(i, rxBuff, rxLen);
            if (ret == NFC_OK) {
                PS_PRINT_INFO("read block:%x", i);
                NFC_PrintArray(rxBuff, T2T_READ_DATA_LEN, "[T2T] read");
                break;
            }
        }
        if (retry == T2T_READ_RETRY_TIME) {
            return ret;
        }
    }
    uint8_t txBuff[ALIPAY_TOKEN_LEN] = {0}; // 将读到的数据交给支付宝api接口处理后得到的token，用于后续数据交互
    ret = NFC_SAMPLE_AlipaySendData(txBuff, sizeof(txBuff)); // 注意！实际传入的长度应为token长度
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_SAMPLE_AlipayTagOperation(NfcIntfActInfo *intfActInfo)
{
    if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T2T) {
        PS_PRINT_INFO("T2T\n");
        return NFC_SAMPLE_AlipayProcessT2T();
    }

    PS_PRINT_ERR("[TagOperation]Not T2T protocol, skip!\n");
    return NFC_OK;
}

bool NFC_SAMPLE_AlipayPresenceCheck(NfcIntfActInfo *intfActInfo)
{
    NfcErrorCode ret;

    if (intfActInfo->protocol == NFC_NCI_RF_PROTOCOL_T2T) {
        uint8_t t2tData[T2T_READ_DATA_LEN] = {0};
        ret = NFC_HAL_T2TPollerProcRead(0x0, t2tData, sizeof(t2tData));
        if (ret != NFC_OK) {
            return false;
        }
        return true;
    }

    PS_PRINT_ERR("[PresenceCheck] not T2T protocol, skip!\n");
    return false;
}

void NFC_SAMPLE_AlipayProcessRw(void)
{
    uint8_t recvBuffer[HAL_MAX_BUF_LEN];
    NfcIntfActInfo intfActInfo = {0};
    NfcErrorCode ret = NFC_HAL_PollRecvIntfActNtf(PRIO_FIRST, recvBuffer, sizeof(recvBuffer), NFC_DISCOVERY_TIME);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("error NFC_HAL_PollRecvIntfActNtf, ret:%x", ret);
        return;
    }

    ret = NFC_HAL_PollParseIntfActNtf(&intfActInfo, &recvBuffer[NCI_HEAD_LEN], recvBuffer[NCI_LEN_POS]);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("error NFC_HAL_PollParseIntfActNtf, ret:%x", ret);
        return;
    }

    // According to intfActInfo.protocol, user can use Tag operation APIs to exchange info with remote.
    ret = NFC_SAMPLE_AlipayTagOperation(&intfActInfo);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("error NFC_TagOperation, ret:%x", ret);
        return;
    }

    // Tag presence check
    if (NFC_SAMPLE_AlipayPresenceCheck(&intfActInfo)) {
        PS_PRINT_INFO("[PresenceCheck] card presence check result is true\n");
    } else {
        PS_PRINT_INFO("[PresenceCheck] card presence check result is false\n");
    }
}

NfcErrorCode NFC_SAMPLE_AlipayStartRw(void)
{
    NfcIntfActInfo intfActInfo = {0};

    NfcErrorCode ret = NFC_EXAMPLE_RwInit();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_EXAMPLE_StartPolling();
    if (ret != NFC_OK) {
        return ret;
    }

    NFC_SAMPLE_AlipayProcessRw();
    NFC_HAL_CleanUpDevice();

    // Exit NFC reader mode
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }

    // May choose to enter idle sleep for lowest power mode as "NFC close".
    return NFC_HAL_EnterIdleSleep();
}

static void NFC_SAMPLE_AlipayEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcAlipaySampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_ALIPAY_SAMPLE_TASK_STACK_SIZE;
    attr.priority = NFC_ALIPAY_SAMPLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NFC_SAMPLE_AlipayThread, NULL, &attr) == NULL) {
        PS_PRINT_ERR("nfc alipay sample task create fail!\n");
        return;
    }
}

#ifdef __cplusplus
}
#endif