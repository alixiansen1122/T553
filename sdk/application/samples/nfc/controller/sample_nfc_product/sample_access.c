/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc access sample, should enable build macro HAVE_NFC_LISTEN & HAVE_NFC_POLL\n
 * Author: @CompanyNameTag \n
 * Date: 2025-06-25 \n
 */

#include "sample_access.h"
#include "cmsis_os2.h"
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "nfc_utils.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_ACCESS_SAMPLE_TASK_STACK_SIZE               0x800
#define NFC_ACCESS_SAMPLE_TASK_PRIO                     (osPriority_t)(17)

#define NFC_WAIT_NEXT_BUSINESS_TO                       500
#define NCI_DISC_NTF_HEAD                               0x6103
#define NCI_INTF_ACT_NTF_HEAD                           0x6105
#define NCI_INT_ACT_NTF_PROT_IDX                        5
#define NCI_INT_ACT_NTF_TECH_IDX                        6
#define NFC_RF_DISCOV_NTF                               3
#define NFC_RF_INT_ACT_NTF                              5
#define NCI_DISCOV_NTF_MIN_LEN                          8
#define NFC_RF_DISCOV_MORE_NTF                          2
#define NFC_DISCOVERY_TIME                              0x7FFFFFFF

NonContactParam g_nfcAccessNonContactParam = {
    .sensRes = {0x04, 0x00},
    .sak = 0x8,
    .nfcidLen = 0x04,
    .nfcid = {0x00, 0x00, 0x00, 0x00}
};

typedef enum {
    NFC_RW_MODE,
    NFC_HCE_MODE,
} NfcWorkMode;

NfcWorkMode g_nfcAccessWorkMode = NFC_RW_MODE;

static void *NFC_SAMPLE_AccessThread(const char *arg)
{
    UNUSED(arg);
    while (true) {
        NfcErrorCode ret = NFC_HAL_Open();
        if (ret != NFC_OK) {
            PS_PRINT_ERR("nfc open fail, exit nfc process!\r\n");
            return NULL;
        }
        do {
            switch (g_nfcAccessWorkMode) {
#ifdef HAVE_NFC_POLL
                case NFC_RW_MODE:
                    ret = NFC_SAMPLE_AccessRwTaskMain();
                    break;
#endif
#ifdef HAVE_NFC_LISTEN
                case NFC_HCE_MODE:
                    ret = NFC_SAMPLE_AccessHceTaskMain();
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

NonContactParam *NFC_SAMPLE_AccessGetNonContactParam(void)
{
    return &g_nfcAccessNonContactParam;
}

NfcErrorCode NFC_SAMPLE_AccessRecvDiscovNtfAndShow(uint8_t *recvNtf, uint16_t recvLen)
{
    int readNtfFlag, i = 0;
    UNUSED(i);  // 避免打印等级太高导致变量i未使用
    uint8_t recvBuff[HAL_MAX_BUF_LEN] = {0};
    uint16_t len = HAL_MAX_BUF_LEN;

    if (recvLen < NCI_DISCOV_NTF_MIN_LEN) {
        return NFC_ERR_CODE_DISCOV_NTF_PLD_ERR;
    }
    PS_PRINT_INFO("discover idx:%x, protocal:%x, tech:%x",
        i, recvNtf[NCI_INT_ACT_NTF_PROT_IDX], recvNtf[NCI_INT_ACT_NTF_TECH_IDX]);
    i++;
    readNtfFlag = recvNtf[recvLen - 1];

    while (readNtfFlag == NFC_RF_DISCOV_MORE_NTF) {
        NfcErrorCode ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, NCI_READ_TO);
        if (ret != NFC_OK || len < NCI_DISCOV_NTF_MIN_LEN) {
            return NFC_ERR_CODE_DISCOV_NTF_HEAD_I2C_ERR;
        }
        PS_PRINT_INFO("discover idx:%x, protocal:%x, tech:%x",
            i, recvBuff[NCI_INT_ACT_NTF_PROT_IDX], recvBuff[NCI_INT_ACT_NTF_TECH_IDX]);
        i++;
        readNtfFlag = recvBuff[len - 1];
    }

    return NFC_OK;
}

void NFC_SAMPLE_AccessParseActNtf(uint8_t *buff, uint16_t len)
{
    if (buff == NULL || len < NCI_HEAD_LEN) {
        return;
    }
    uint8_t index = NCI_INT_ACT_NTF_PROT_IDX;
    if (buff[index] != 0x80 || buff[index + 1] != 0x00) { // 只解析mxx卡
        return;
    }
    index += 0x5;
    NonContactParam *nfcNonContactParam = NFC_SAMPLE_AccessGetNonContactParam();
    if (memcpy_s(nfcNonContactParam->sensRes, SENS_RES_LEN, &buff[index], SENS_RES_LEN) != EOK) {
        return;
    }
    index += SENS_RES_LEN;
    nfcNonContactParam->nfcidLen = buff[index++];
    if (memcpy_s(nfcNonContactParam->nfcid, NFCID1_MAX_LEN, &buff[index], nfcNonContactParam->nfcidLen) != EOK) {
        return;
    }
    index += nfcNonContactParam->nfcidLen;
    uint8_t sakLen = buff[index++];
    if (sakLen != 0) {
        nfcNonContactParam->sak = buff[index];
    } else {
        nfcNonContactParam->sak = 0x08;
    }
}

void NFC_SAMPLE_AccessProcessRwOnce(void)
{
    PS_PRINT_INFO("start process door card rw!");
    uint8_t recvBuff[HAL_MAX_BUF_LEN] = {0};
    uint16_t len = HAL_MAX_BUF_LEN;
    NfcErrorCode ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, NFC_DISCOVERY_TIME);
    if (ret != NFC_OK || len < NCI_HEAD_LEN) {
        PS_PRINT_ERR("[RW] data recv error[%x] len[%x]\n", ret, len);
        return;
    }
    PS_PRINT_INFO("PollRecvIntfActNtf head:%d, %d, %d\n", recvBuff[0], recvBuff[1], recvBuff[NCI_LEN_POS]);
    if (len < 0) {
        return;
    } else if (UTIL_Combine8bitTo16bit(recvBuff[0], recvBuff[1]) != NCI_DISC_NTF_HEAD &&
               UTIL_Combine8bitTo16bit(recvBuff[0], recvBuff[1]) != NCI_INTF_ACT_NTF_HEAD) {
        NFC_HAL_CleanUpDevice();
        return;
    }

    if (recvBuff[1] == NFC_RF_INT_ACT_NTF) {  // Only one RF technical discovery ntf
        PS_PRINT_INFO("act protocal:%x, tech:%x",
            recvBuff[NCI_INT_ACT_NTF_PROT_IDX], recvBuff[NCI_INT_ACT_NTF_TECH_IDX]);
        NFC_SAMPLE_AccessParseActNtf(recvBuff, len);  // 普通门禁卡一般不存在复合卡的场景，通过该函数解析出非接参数
    } else if (recvBuff[1] == NFC_RF_DISCOV_NTF) {  // More RF technical discovery ntf
        NFC_SAMPLE_AccessRecvDiscovNtfAndShow(recvBuff, len);
    }
}

/* 读卡器模式，读取卡片非接参数并记录 */
NfcErrorCode NFC_SAMPLE_AccessRwTaskMain(void)
{
    PS_PRINT_INFO("Nfc door card RW task main");
    NfcErrorCode ret = NFC_EXAMPLE_RwInit();
    if (ret != NFC_OK) {
        return ret;
    }

    ret = NFC_EXAMPLE_StartPolling();
    if (ret != NFC_OK) {
        return ret;
    }
    NFC_SAMPLE_AccessProcessRwOnce();
    NFC_HAL_CleanUpDevice();
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_HAL_EnterIdleSleep();
}

NfcErrorCode NFC_SAMPLE_AccessNonContactParamCfg(void)
{
    NonContactParam *nfcNonContactParam = NFC_SAMPLE_AccessGetNonContactParam();
    uint8_t recvShortBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;
    // 参数1：nfcid(0x33)，从读卡器模式中获取
    uint8_t cfgDbCmd2[] = {0x20, 0x02, 0x0D, 0x01, 0x33, 0x0A,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    cfgDbCmd2[0x05] = nfcNonContactParam->nfcidLen;
    if (memcpy_s(
        &cfgDbCmd2[0x06], sizeof(cfgDbCmd2) - 0x6, nfcNonContactParam->nfcid, nfcNonContactParam->nfcidLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    cfgDbCmd2[0x02] = nfcNonContactParam->nfcidLen + 0x3;
    NfcErrorCode ret =
        NFC_HAL_SendNciCmdAndRecvRsp(cfgDbCmd2, nfcNonContactParam->nfcidLen + 0x6, recvShortBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    rspLen = HAL_SHORT_BUFF_LEN;
    // 参数1：sak(0x32)，从读卡器模式中获取
    // 参数2：sdd(0x30)，从读卡器模式中获取
    // 参数3：plt config(0x31)，从读卡器模式中获取
    uint8_t cfgDbCmd3[] = {0x20, 0x02, 0x0A, 0x03,
        0x32, 0x01, nfcNonContactParam->sak,
        0x30, 0x01, nfcNonContactParam->sensRes[0],
        0x31, 0x01, nfcNonContactParam->sensRes[1]};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(cfgDbCmd3, sizeof(cfgDbCmd3), recvShortBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }
    return NFC_OK;
}

NfcErrorCode NFC_SAMPLE_AccessInitHceMode(void)
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

    uint8_t recvShortBuff[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    rspLen = HAL_SHORT_BUFF_LEN;
    // 参数1：CON_DISCOVERY_PARAM(0x02) -> 0x00 enable DH-NFCEE
    uint8_t cfgDbCmd[] = {0x20, 0x02, 0x04, 0x01, 0x02, 0x01, 0x00};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(cfgDbCmd, sizeof(cfgDbCmd), recvShortBuff, &rspLen);
    if (ret != NFC_OK) {
        return ret;
    }

    NFC_SAMPLE_AccessNonContactParamCfg();

    uint8_t routeCmd[] = {0x21, 0x01, 0x09, 0x00, 0x01, 0x00, 0x03, 0x00, 0x3B, 0x00, 0x03, 0x03};
    uint16_t idx = sizeof(routeCmd);
    routeCmd[NCI_LEN_POS] = idx - NCI_HEAD_LEN;
    ret = NFC_HAL_SetListenRouteTbl(routeCmd, idx);
    if (ret != NFC_OK) {
        return ret;
    }

    return NFC_OK;
}

void NFC_SAMPLE_AccessProcessHceOnce(void)
{
    // Wait for first business NTF with indicated timeout
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN] = {0};
    uint16_t len = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, NFC_DISCOVERY_TIME);
    if (ret != NFC_OK || len < NCI_HEAD_LEN) {
        return;
    }

    do {
        len = HAL_SHORT_BUFF_LEN;
        ret = NFC_HAL_ReceiveNciPkt(recvBuff, &len, NCI_READ_TO);
    } while (ret == NFC_OK && len > NCI_HEAD_LEN);

    // No more NTF, this sample treats it as business end when RW leave.
    return;
}

/* 卡模拟模式，配置非接参数进行业务（只交互非接参数，不进行数据交互） */
NfcErrorCode NFC_SAMPLE_AccessHceTaskMain(void)
{
    PS_PRINT_INFO("Nfc door card HCE task main");
    NfcErrorCode ret = 0;
    ret = NFC_SAMPLE_AccessInitHceMode(); // 配置NFCC为门禁卡模拟模式
    if (ret != NFC_OK) {
        return ret;
    }

    uint8_t list[] = {NFC_NCI_A_PASSIVE_LISTEN_MODE};  // MXX卡均为A技术卡
    ret = NFC_HAL_StartDiscovery(list, sizeof(list));
    if (ret != NFC_OK) {
        return ret;
    }

    NFC_SAMPLE_AccessProcessHceOnce();
        
    ret = NFC_HAL_Deactivate();
    if (ret != NFC_OK) {
        PS_PRINT_ERR("nfc door card task deactivate fail, ret = %d\n", ret);
    }
    return ret;
}

static void NFC_SAMPLE_AccessEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NfcAccessSampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_ACCESS_SAMPLE_TASK_STACK_SIZE;
    attr.priority = NFC_ACCESS_SAMPLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NFC_SAMPLE_AccessThread, NULL, &attr) == NULL) {
        PS_PRINT_ERR("nfc access sample task create fail!\n");
        return;
    }
}

#ifdef __cplusplus
}
#endif