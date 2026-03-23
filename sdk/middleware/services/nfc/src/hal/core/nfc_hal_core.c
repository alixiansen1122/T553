/*
 * Copyright (c) @CompanyNameMagicTag 2021. All rights reserved. \n
 * Description: nfc hal core \n
 * Author: @CompanyNameTag \n
 * Date: 2021-08-20 \n
 */
#include "nfc_hal_core.h"
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NCI_RSP_ONLY_STATUS_LEN   4
#define NCI_STATUS_OK             0

#define POWER_ON                  1
#define POWER_OFF                 0

#define RESET_NTF_TRIG_FROM_DH    0x02

#define NFC_RF_DISCOV_MORE_NTF    2
#define NFC_RF_DISCOV_LAST_NTF    0
#define NFC_RF_DISCOV_LIMIT_NTF   1
#define NFC_RESET_RSP_HEAD        0x40
#define NFC_RESET_CMD             0x00
#define NFC_RF_NTF_MAX            5

#define NCI_RF_DISCOV_REQ_NTF_TYPE_POS              4
#define NCI_RF_DISCOV_REQ_NTF_TECHMODE_POS          7
#define NCI_RF_DISCOV_NTF_PAYLOAD_MIN_LEN           5
#define NCI_T3T_POLL_NTF_MIN_HEAD_LEN               6
#define NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN         7
#define NCI_RF_INTF_ACT_NTF_FIXED_TECH_PARAM_LEN    6
#define NCI_RF_INTF_ACT_NTF_TECH_PARAM_T1T_LEN      11
#define NCI_TECH_F_PARAM_FIXED_LEN                  2
#define NCI_TECH_V_PARAM_FIXED_LEN                  10

#define SENSB_RES_MIN_LEN           11
#define SENSF_RES_MIN_LEN           16

#define NCI_READ_BAD_MSG_TO         20

#define NCI_GID_CORE_NTF            0x60
#define NCI_GID_RF_NTF              0x61
#define NCI_OID_CREDIT_NTF          0x06
#define NFC_OID_RF_DISCOV_NTF       0x03
#define NFC_OID_RF_INT_ACT_NTF      0x05
#define NFC_OID_RF_DEACT_NTF        0x06

enum {
    DEVICE_IDLE     = 0, // NCI RFST IDLE
    DEVICE_DISCOVER = 1, // NCI RFST DISCOVERY, W4_HOST_SELECT, W4_ALL_DISCOVERIES, LISTEN_SLEEP
    DEVICE_ACTIVE   = 2, // NCI RFST POLL_ACTIVE, LISTEN_ACTIVE
    DEVICE_SLEEP    = 3, // device is in idle (no business) low power state
};

typedef struct NfcHalCtxStruc {
    uint32_t readTimeout;  // control packet read timeout
    uint32_t dataTimeout;  // data exchange read timeout
    bool isLowPowerEn;
    uint8_t deviceState;
#ifdef HAVE_NFC_POLL
    uint8_t rfDiscovCnt;
    uint8_t rfDiscovId[NFC_RF_NTF_MAX];
    uint8_t rfDiscovProtocol[NFC_RF_NTF_MAX];
#endif
#ifdef HAVE_NFC_LISTEN
    FuncProcessHciFromNfcee processHciFromNfcee;
#endif
} NfcHalCtx;

static NfcHalCtx g_nfcHalCtx = {
    .readTimeout = NCI_READ_TO,
    .dataTimeout = NCI_READ_DATA_TO,
    .isLowPowerEn = true,
    .deviceState = DEVICE_IDLE,
#ifdef HAVE_NFC_POLL
    .rfDiscovId = {0},
    .rfDiscovProtocol = {0},
#endif
#ifdef HAVE_NFC_LISTEN
    .processHciFromNfcee = NULL
#endif
};

void NFC_HAL_SetCoreParams(NfcHalCoreParams *params)
{
    if (params == NULL) {
        return;
    }
    g_nfcHalCtx.readTimeout = params->readTimeout;
    g_nfcHalCtx.dataTimeout = params->dataTimeout;
#ifdef HAVE_NFC_LISTEN
    g_nfcHalCtx.processHciFromNfcee = params->processHciFromNfcee;
#endif
}

NfcErrorCode NFC_HAL_SendNciCmdAndRecvRsp(uint8_t *buf, uint16_t count, uint8_t *recvRsp, uint16_t *rspLen)
{
    if ((buf == NULL) || (recvRsp == NULL) || (rspLen == NULL)) {
        return NFC_ERR_CODE_NULL_POINTER;
    }
    int32_t ret = HW_NFC_Write(buf, count);
    if (ret < 0) {
        PS_PRINT_ERR("nci write failed, ret = 0x%x\n", ret);
        return NFC_ERR_CODE_WRITE_FAIL;
    }

    ret = HW_NFC_NciRead(recvRsp, *rspLen, g_nfcHalCtx.readTimeout);
    if (ret < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_RECV_RSP_HEAD_I2C_ERROR;
    }
    *rspLen = recvRsp[NCI_LEN_POS] + NCI_HEAD_LEN;

    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciReset(void)
{
    uint8_t buff[HAL_MAX_BUF_LEN] = {0};
    uint8_t resetCmd[] = {0x20, 0x00, 0x01, 0x01};

    int32_t ret = HW_NFC_Write(resetCmd, sizeof(resetCmd));
    g_nfcHalCtx.deviceState = DEVICE_IDLE;
    if (ret < 0) {
        return NFC_ERR_CODE_RST_NCI_WRITE_I2C_ERR;
    }
    /* read response */
    do {
        ret = HW_NFC_NciRead(buff, HAL_MAX_BUF_LEN, g_nfcHalCtx.readTimeout);
        if (ret < NCI_HEAD_LEN) {
            return NFC_ERR_CODE_RST_RSP_HEAD_I2C_ERR;
        }

        if ((buff[0] == NFC_RESET_RSP_HEAD) && (buff[1] == NFC_RESET_CMD)) {
            if (buff[NCI_HEAD_LEN] == 0x00) {
                break;
            } else {
                return NFC_ERR_CODE_RST_RSP_STATUS_ERR;
            }
        } else {
            PS_PRINT_ALERT("NciReset get bad rsp head:0x%2x, 0x%2x, 0x%2x\n", buff[0], buff[1], buff[NCI_LEN_POS]);
            continue;
        }
    } while (ret >= 0);

    /* read NTF */
    ret = HW_NFC_NciRead(buff, HAL_MAX_BUF_LEN, g_nfcHalCtx.readTimeout);
    if (ret < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_RST_NTF_HEAD_I2C_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciInit(void)
{
    uint8_t recvRsp[HAL_MAX_BUF_LEN];
    uint16_t rspLen = HAL_MAX_BUF_LEN;
    uint8_t initCmd[] = {0x20, 0x01, 0x02, 0x00, 0x00};

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(initCmd, sizeof(initCmd), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_INIT_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_INIT_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciSetConfig(uint8_t *cmd, uint16_t len)
{
    if (cmd == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, len, recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CFG_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_CFG_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciDiscovMap(uint8_t *cmd, uint16_t len)
{
    if (cmd == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, len, recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DISCOV_MAP_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK) || (rspLen != NCI_RSP_ONLY_STATUS_LEN)) {
        return NFC_ERR_CODE_DISCOV_MAP_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

void NFC_HAL_CleanUpDevice(void)
{
    int32_t len;
    uint8_t recvBuff[HAL_MAX_BUF_LEN] = {0};
    const uint32_t readCountMax = 0xFFFF;
    uint32_t readCount = 0;

    // read out all NCI msg for clean up
    do {
        readCount++;
        len = HW_NFC_NciRead(recvBuff, HAL_MAX_BUF_LEN, NCI_READ_BAD_MSG_TO);
    } while ((len > 0) && (readCount < readCountMax));
}

NfcErrorCode NFC_HAL_NciStartDiscovery(uint8_t *cmd, uint16_t len)
{
    if (cmd == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    if ((g_nfcHalCtx.deviceState != DEVICE_IDLE) && (g_nfcHalCtx.deviceState != DEVICE_SLEEP)) {
        return NFC_ERR_CODE_DISCOV_STATE_INVALID;
    }
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, len, recvRsp, &rspLen);
    // 1. Once we send discovery cmd, if success, we will be in discovery state.
    // 2. If not, we may already in discovery state or other bad state.
    // 3. In case 2, we still set discovery state in order to make sure later deact succ and then discover succ.
    g_nfcHalCtx.deviceState = DEVICE_DISCOVER;
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DISCOV_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK) || (rspLen != NCI_RSP_ONLY_STATUS_LEN)) {
        // In this case we get unexpected NCI msg, we'd better read out all NCI msg for clean up.
        NFC_HAL_CleanUpDevice();
        return NFC_ERR_CODE_DISCOV_RSP_STATUS_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_StartDiscovery(uint8_t *techModeList, uint8_t num)
{
    if (techModeList == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }
    /* payload's length: 1 byte of the number of configurations and 2 * num bytes of the configurations */
    if (((num << 1) + NCI_HEAD_LEN + 1) > HAL_SHORT_BUFF_LEN) {
        return NFC_ERR_CODE_DISCOV_PARAM_BUFF_OVERFLOW;
    }
    uint8_t rfDiscovCmd[HAL_SHORT_BUFF_LEN] = {0x21, 0x03, 0x03, 0x01, 0x80, 0x01};
    uint8_t idx = NCI_HEAD_LEN;
    rfDiscovCmd[idx++] = num;
    for (uint8_t i = 0; i < num; ++i) {
        rfDiscovCmd[idx++] = techModeList[i];
        rfDiscovCmd[idx++] = 0x01; /* 0x01: RF technology and mode will be executed in every discovery period */
    }
    rfDiscovCmd[NCI_LEN_POS] = 1 + (num << 1);
    return NFC_HAL_NciStartDiscovery(rfDiscovCmd, NCI_HEAD_LEN + 1 + (num << 1));
}

NfcErrorCode NFC_HAL_Deactivate(void)
{
    uint8_t recvBuff[HAL_MAX_BUF_LEN];
    uint16_t recvLen = HAL_MAX_BUF_LEN;

    uint8_t deactCmd[] = {0x21, 0x06, 0x01, 0x00};

    if ((g_nfcHalCtx.deviceState == DEVICE_IDLE) || (g_nfcHalCtx.deviceState == DEVICE_SLEEP)) {
        return NFC_OK;
    }

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(deactCmd, sizeof(deactCmd), recvBuff, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DEACT_RECV_RSP_ERR;
    } else if (recvBuff[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_DEACT_RSP_STATUS_ERR;
    }
    if (g_nfcHalCtx.deviceState == DEVICE_ACTIVE) {
        // Device side may be in other state and can not send correct NTF. But since we already exchanged
        // deact cmd/rsp, we should set device state to be idle.
        g_nfcHalCtx.deviceState = DEVICE_IDLE;
        /* read deactivate NTF */
        if (HW_NFC_NciRead(recvBuff, HAL_MAX_BUF_LEN, g_nfcHalCtx.readTimeout) < NCI_HEAD_LEN) {
            return NFC_ERR_CODE_DEACT_NTF_HEAD_I2C_ERR;
        }
        PS_PRINT_ALERT("deactivate get head:0x%2x, 0x%2x, 0x%2x\n", recvBuff[0], recvBuff[1], recvBuff[NCI_LEN_POS]);
    }
    g_nfcHalCtx.deviceState = DEVICE_IDLE;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_EnterIdleSleep(void)
{
    // 不支持低功耗的时候直接返回
    if (!g_nfcHalCtx.isLowPowerEn) {
        return NFC_OK;
    }
    if (g_nfcHalCtx.deviceState == DEVICE_SLEEP) {
        return NFC_OK;
    }
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t recvLen = HAL_SHORT_BUFF_LEN;

    uint8_t sleepCmd[] = {0x2F, 0x06, 0x01, 0x00};

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(sleepCmd, sizeof(sleepCmd), recvBuff, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_ENTER_IDLE_SLP_RECV_RSP_ERR;
    } else if (recvBuff[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_ENTER_IDLE_SLP_RSP_STATUS_ERR;
    }
    // I2C write depends on DEVICE_SLEEP status to wake up device
    g_nfcHalCtx.deviceState = DEVICE_SLEEP;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_CfgLowPwr(bool status)
{
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t recvLen = HAL_SHORT_BUFF_LEN;

    uint8_t cfgLowPwr[] = {0x2F, 0x00, 0x01, status};

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cfgLowPwr, sizeof(cfgLowPwr), recvBuff, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CFG_LOW_POWER_RECV_ERR;
    } else if (recvBuff[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_CFG_LOW_POWER_STATUS_ERR;
    }
    g_nfcHalCtx.isLowPowerEn = status;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_Open(void)
{
    int32_t ret = HW_NFC_Open();
    if (ret != 0) {
        PS_PRINT_ERR("hw nfc open failed");
        return NFC_ERR_CODE_OPEN_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_Close(void)
{
    int32_t ret = HW_NFC_Close();
    if (ret != 0) {
        PS_PRINT_ERR("hw nfc close failed");
        return NFC_ERR_CODE_CLOSE_FAIL;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_ReceiveNciPkt(uint8_t *buff, uint16_t *len, uint32_t timeout)
{
    if ((buff == NULL) || (len == NULL) || (*len < NCI_HEAD_LEN)) {
        return NFC_ERR_CODE_INVALID_PARAMETER;
    }

    int32_t rdLen = HW_NFC_NciRead(buff, *len, timeout);
    if (rdLen < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_PKT_RECV_I2C_ERR;
    }

    *len = rdLen;

    // if not RF NTF which may affect NCI RFST state, just quit.
    if (buff[0] != NCI_GID_RF_NTF) {
        return NFC_OK;
    }

    if (buff[1] == NFC_OID_RF_INT_ACT_NTF) {
        g_nfcHalCtx.deviceState = DEVICE_ACTIVE;
    } else if (buff[1] == NFC_OID_RF_DEACT_NTF) {
        // if deactivate to idle state (0x00), then device is idle, otherwise set as discovery.
        if (buff[NCI_HEAD_LEN] == 0x00) {
            g_nfcHalCtx.deviceState = DEVICE_IDLE;
        } else {
            g_nfcHalCtx.deviceState = DEVICE_DISCOVER;
        }
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_RecvUnchainedData(uint8_t *recvBuff, uint16_t *buffLen)
{
    if ((recvBuff == NULL) || (buffLen == NULL)) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    // recv data flow ctrl
    uint8_t flowCtrl[HAL_SHORT_BUFF_LEN] = {0};
    int32_t cnt = HW_NFC_NciRead(flowCtrl, HAL_SHORT_BUFF_LEN, g_nfcHalCtx.dataTimeout);
    if (cnt < NCI_HEAD_LEN || flowCtrl[0] != NCI_GID_CORE_NTF || flowCtrl[1] != NCI_OID_CREDIT_NTF) {
        return NFC_ERR_CODE_DATA_FLOW_RECV_ERR;
    }

    uint8_t data[HAL_MAX_BUF_LEN] = {0};
    cnt = HW_NFC_NciRead(data, HAL_MAX_BUF_LEN, g_nfcHalCtx.dataTimeout);
    if (cnt < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_DATA_RECV_ERR;
    }

    if (memcpy_s(recvBuff, *buffLen, data + NCI_HEAD_LEN, data[NCI_LEN_POS]) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    *buffLen = data[NCI_LEN_POS];

    return NFC_OK;
}

NfcErrorCode NFC_HAL_SendData(uint8_t *buf, uint16_t sendLen, uint8_t conn)
{
    if (buf == NULL || sendLen > NCI_MAX_PAYLOAD_LEN) {
        return NFC_ERR_CODE_INVALID_PARAMETER;
    }

    uint16_t i = 0;
    uint8_t data[HAL_MAX_BUF_LEN];

    data[i++] = conn;
    data[i++] = 0;
    data[i] = (uint8_t)sendLen;
    if (memcpy_s(data + NCI_HEAD_LEN, NCI_MAX_PAYLOAD_LEN, buf, sendLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    int32_t ret = HW_NFC_Write(data, NCI_HEAD_LEN + sendLen);
    if (ret < 0) {
        return NFC_ERR_CODE_DATA_SND_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_TranceiveUnchainedData(uint8_t *sendBuff, uint16_t lenIn, uint8_t conn,
                                            uint8_t *recvBuff, uint16_t *lenOut)
{
    if ((sendBuff == NULL) || (recvBuff == NULL) || (lenOut == NULL)) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    NfcErrorCode ret = NFC_HAL_SendData(sendBuff, lenIn, conn);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DATA_SND_ERR;
    }

    // recv data
    ret = NFC_HAL_RecvUnchainedData(recvBuff, lenOut);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_DATA_RECV_ERR;
    }
    return NFC_OK;
}

#ifdef HAVE_NFC_POLL
NfcErrorCode NFC_HAL_NciIsoDepNakPresenceCheck(bool *isPresent, uint16_t timeout)
{
    uint8_t nakPresChkCmd[] = {0x21, 0x10, 0x00};
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t recvLen = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(nakPresChkCmd, sizeof(nakPresChkCmd), recvBuff, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_T4T_NAK_PRES_CHK_RECV_RSP_ERR;
    } else if (recvBuff[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_T4T_NAK_PRES_CHK_RECV_RSP_STATUS_ERR;
    }
    int32_t ntfLen = HW_NFC_NciRead(recvBuff, sizeof(recvBuff), timeout);
    if (ntfLen < NCI_HEAD_LEN + 1) {
        return NFC_ERR_CODE_T4T_NAK_PRES_CHK_RECV_NTF_ERR;
    }
    *isPresent = recvBuff[NCI_HEAD_LEN] == NCI_STATUS_OK ? true : false;

    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciSelect(uint8_t discovId, uint8_t protocol, uint8_t intf, uint8_t *recvNtf, uint16_t len)
{
    if (recvNtf == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }
    uint16_t recvLen = len;
    uint8_t selectCmd[] = {0x21, 0x04, 0x03, discovId, protocol, intf};

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(selectCmd, sizeof(selectCmd), recvNtf, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_SELECT_RSP_I2C_ERR;
    }
    if (recvNtf[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_SELECT_RSP_STATUS_ERR;
    }

    if (HW_NFC_NciRead(recvNtf, len, g_nfcHalCtx.readTimeout) < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_SELECT_NTF_HEAD_I2C_ERR;
    }

    g_nfcHalCtx.deviceState = DEVICE_ACTIVE;
    return NFC_OK;
}

static NfcErrorCode NFC_HAL_RecvDiscovNtf(uint8_t *recvNtf, uint16_t len)
{
    uint16_t i = 0;

    do {
        uint16_t payloadLen = recvNtf[NCI_LEN_POS];
        if (payloadLen < NCI_RF_DISCOV_NTF_PAYLOAD_MIN_LEN) {
            return NFC_ERR_CODE_DISCOV_NTF_PLD_ERR;
        }
        g_nfcHalCtx.rfDiscovId[i] = recvNtf[NCI_HEAD_LEN];
        g_nfcHalCtx.rfDiscovProtocol[i++] = recvNtf[NCI_HEAD_LEN + 1];
        g_nfcHalCtx.rfDiscovCnt = i;

        // check if there is more ntf
        uint8_t readNtfFlag = recvNtf[(NCI_HEAD_LEN + payloadLen) - 1];
        if (readNtfFlag != NFC_RF_DISCOV_MORE_NTF) {
            break;
        }

        if (HW_NFC_NciRead(recvNtf, len, g_nfcHalCtx.readTimeout) < NCI_HEAD_LEN) {
            return NFC_ERR_CODE_DISCOV_NTF_HEAD_I2C_ERR;
        }
    } while (i < NFC_RF_NTF_MAX);

    return NFC_OK;
}

NfcErrorCode NFC_HAL_SelectWithPriority(uint8_t priority, uint8_t *recvNtf, uint16_t len)
{
    if (recvNtf == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }

    uint8_t discovId = g_nfcHalCtx.rfDiscovId[0];
    uint8_t protocol = g_nfcHalCtx.rfDiscovProtocol[0];
    uint8_t intf = NFC_NCI_FRAME_RF_INTERFACE;  // Except isodep/nfcdep, other protocol map to frame rf interface

    if (priority != PRIO_FIRST) {
        // Except prio_first, priority value equals to RF protocol
        protocol = priority;
        uint8_t idx = 0;
        for (; idx < g_nfcHalCtx.rfDiscovCnt; idx++) {
            if (g_nfcHalCtx.rfDiscovProtocol[idx] == protocol) {
                discovId = g_nfcHalCtx.rfDiscovId[idx];
                break;
            }
        }
        if (idx >= g_nfcHalCtx.rfDiscovCnt) {
            return NFC_ERR_CODE_DISCOV_NTF_NO_PROP_PROTOCOL_ERR;
        }
    }

    if (protocol == NFC_NCI_RF_PROTOCOL_ISO_DEP) {
        intf = NFC_NCI_ISODEP_RF_INTERFACE;
    } else if (protocol == NFC_NCI_RF_PROTOCOL_NFC_DEP) {
        intf = NFC_NCI_NFCDEP_RF_INTERFACE;
    }

    return NFC_HAL_NciSelect(discovId, protocol, intf, recvNtf, len);
}

NfcErrorCode NFC_HAL_PollRecvIntfActNtf(uint8_t priority, uint8_t *recvNtf, uint16_t len, uint32_t timeout)
{
    if (recvNtf == NULL) {
        return NFC_ERR_CODE_NULL_POINTER;
    }
    if (priority >= PRIO_PUBLIC_END && priority != PRIO_PROP_A) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    /* read activate NTF */
    if (HW_NFC_NciRead(recvNtf, len, timeout) < NCI_HEAD_LEN) {
        return NFC_ERR_CODE_ACT_NTF_HEAD_I2C_ERR;
    }

    PS_PRINT_ALERT("PollRecvIntfActNtf head:0x%2x, 0x%2x, 0x%2x\n", recvNtf[0], recvNtf[1], recvNtf[NCI_LEN_POS]);
    if (recvNtf[1] == NFC_OID_RF_INT_ACT_NTF) {
        g_nfcHalCtx.deviceState = DEVICE_ACTIVE;
        return NFC_OK;
    } else if (recvNtf[1] == NFC_OID_RF_DISCOV_NTF) {
        NfcErrorCode ret = NFC_HAL_RecvDiscovNtf(recvNtf, len);
        if (ret != NFC_OK) {
            return ret;
        }
        return NFC_HAL_SelectWithPriority(priority, recvNtf, len);
    } else {
        return NFC_ERR_CODE_ACT_NTF_RECV_WRONG_PKT;
    }
}

NfcErrorCode NFC_HAL_ParseIntfActNtfPollA(NfcIntfActInfo *intfActInfo, uint8_t *recvNtf, uint16_t len)
{
    if (len != NCI_RF_INTF_ACT_NTF_FIXED_TECH_PARAM_LEN + NFCID1_CASLVL1_LEN &&
        len != NCI_RF_INTF_ACT_NTF_FIXED_TECH_PARAM_LEN + NFCID1_CASLVL2_LEN &&
        len != NCI_RF_INTF_ACT_NTF_FIXED_TECH_PARAM_LEN + NFCID1_CASLVL3_LEN &&
        len != NCI_RF_INTF_ACT_NTF_TECH_PARAM_T1T_LEN) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    uint16_t idx = 0;
    NfcTechAParams *techA = &intfActInfo->techParams.a;
    techA->sensRes[0] = recvNtf[idx++];
    techA->sensRes[1] = recvNtf[idx++];
    techA->nfcid1Len = recvNtf[idx++];
    if (memcpy_s(techA->nfcid1, NFCID1_MAX_LEN, recvNtf + idx, techA->nfcid1Len) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }
    idx += (techA->nfcid1Len + 1);  // ignore sel_res length field
    techA->selRes = recvNtf[idx++];

    return NFC_OK;
}

NfcErrorCode NFC_HAL_ParseIntfActNtfPollB(NfcIntfActInfo *intfActInfo, uint8_t *recvNtf, uint16_t len)
{
    if (len != SENSB_RES_MIN_LEN + 1 && len != SENSB_RES_MAX_LEN + 1) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    uint16_t idx = 0;
    NfcTechBParams *techB = &intfActInfo->techParams.b;
    techB->sensbResLen = recvNtf[idx++];
    if (memcpy_s(techB->sensbRes, SENSB_RES_MAX_LEN, recvNtf + idx, techB->sensbResLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_ParseIntfActNtfPollF(NfcIntfActInfo *intfActInfo, uint8_t *recvNtf, uint16_t len)
{
    if (len != SENSF_RES_MIN_LEN + NCI_TECH_F_PARAM_FIXED_LEN &&
        len != SENSF_RES_MAX_LEN + NCI_TECH_F_PARAM_FIXED_LEN) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    uint16_t idx = 0;
    NfcTechFParams *techF = &intfActInfo->techParams.f;
    techF->bitRate = recvNtf[idx++];
    techF->sensfResLen = recvNtf[idx++];
    if (memcpy_s(techF->sensfRes, SENSF_RES_MAX_LEN, recvNtf + idx, techF->sensfResLen) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_ParseIntfActNtfPollV(NfcIntfActInfo *intfActInfo, uint8_t *recvNtf, uint16_t len)
{
    if (len != NCI_TECH_V_PARAM_FIXED_LEN) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    uint16_t idx = 0;
    NfcTechVParams *techV = &intfActInfo->techParams.v;
    techV->resFlag = recvNtf[idx++];
    techV->dsfid = recvNtf[idx++];
    if (memcpy_s(techV->uid, UID_LEN, recvNtf + idx, UID_LEN) != EOK) {
        return NFC_ERR_CODE_MEMCPY_FAIL;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_PollParseIntfActNtf(NfcIntfActInfo *intfActInfo, uint8_t *recvNtf, uint16_t len)
{
    if (len < NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN || recvNtf[NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN - 1] == 0 ||
        len < NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN + recvNtf[NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN - 1]) {
        return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
    }

    uint16_t idx = 1;
    intfActInfo->interface = recvNtf[idx++];
    intfActInfo->protocol = recvNtf[idx++];
    intfActInfo->techMode = recvNtf[idx++];
    idx = NCI_RF_INTF_ACT_NTF_FIXED_PART1_LEN;
    PS_PRINT_INFO("interface=%d, protocol=%d, techMod=0x%x, techLen=%d", intfActInfo->interface, intfActInfo->protocol,
                  intfActInfo->techMode, recvNtf[idx - 1]);

    switch (intfActInfo->techMode) {
        case NFC_NCI_A_PASSIVE_POLL_MODE:
            return NFC_HAL_ParseIntfActNtfPollA(intfActInfo, &recvNtf[idx], recvNtf[idx - 1]);
        case NFC_NCI_B_PASSIVE_POLL_MODE:
            return NFC_HAL_ParseIntfActNtfPollB(intfActInfo, &recvNtf[idx], recvNtf[idx - 1]);
        case NFC_NCI_F_PASSIVE_POLL_MODE:
            return NFC_HAL_ParseIntfActNtfPollF(intfActInfo, &recvNtf[idx], recvNtf[idx - 1]);
        case NFC_NCI_V_PASSIVE_POLL_MODE:
            return NFC_HAL_ParseIntfActNtfPollV(intfActInfo, &recvNtf[idx], recvNtf[idx - 1]);
        default:
            PS_PRINT_ERR("IntfActNtf recv unexpected techMode!\n");
            break;
    }
    return NFC_ERR_CODE_ACT_NTF_INVALID_PARAM_ERR;
}

NfcErrorCode NFC_HAL_StartPollField(void)
{
    uint8_t contRxEn[] = {0x2f, 0x23, 0x04, 0x04, 0x00, 0x00, 0x00};
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(contRxEn, sizeof(contRxEn), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CONT_RX_EN_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_CONT_RX_EN_RSP_STATUS_ERR;
    }

    uint8_t contRxRun[] = {0x2f, 0x23, 0x01, 0x06};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(contRxRun, sizeof(contRxRun), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CONT_RX_RUN_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_CONT_RX_RUN_RSP_STATUS_ERR;
    }

    return NFC_OK;
}

NfcErrorCode NFC_HAL_StopPollField(void)
{
    uint8_t contRxStop[] = {0x2f, 0x23, 0x01, 0x07};
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(contRxStop, sizeof(contRxStop), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CONT_RX_STOP_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_CONT_RX_STOP_RSP_STATUS_ERR;
    }

    uint8_t contRxDisable[] = {0x2f, 0x23, 0x04, 0x05, 0x00, 0x00, 0x00};
    ret = NFC_HAL_SendNciCmdAndRecvRsp(contRxDisable, sizeof(contRxDisable), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_CONT_RX_DISABLE_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_CONT_RX_DISABLE_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_T3TPollRecvNtf(NfcHalT3tSensfRes *params, uint16_t timeout)
{
    uint8_t recvNtf[HAL_MAX_BUF_LEN] = {0};
    uint16_t ntfLen = HAL_MAX_BUF_LEN;

    uint8_t offset = NCI_HEAD_LEN;
    int32_t totalLen = HW_NFC_NciRead(recvNtf, ntfLen, timeout);
    if (totalLen < NCI_T3T_POLL_NTF_MIN_HEAD_LEN) {
        return NFC_ERR_CODE_T3T_POLLING_NTF_RECV_ERR;
    } else if (recvNtf[offset++] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_T3T_POLLING_NTF_STATUS_ERR;
    }
    uint8_t ntfFollow = recvNtf[offset++];
    ntfFollow = ntfFollow > T3T_POLL_SENSF_RES_MAX_SIZE ? T3T_POLL_SENSF_RES_MAX_SIZE : ntfFollow;
    uint8_t sensfResLen = 0;
    uint8_t leftLen = totalLen - offset;
    uint8_t idx = 0;
    do {
        sensfResLen = recvNtf[offset++];
        leftLen--;
        if (sensfResLen > leftLen) {
            return NFC_ERR_CODE_T3T_POLLING_NTF_DECODE_ERR;
        }
        if (memcpy_s(&params->resParams[idx], sizeof(NfcHalT3tPollNtfParams), recvNtf + offset, sensfResLen) != EOK) {
            return NFC_ERR_CODE_MEMCPY_FAIL;
        }
        offset += sensfResLen;
        leftLen -= sensfResLen;
        idx++;
    } while (idx < ntfFollow && leftLen >= 1);
    params->resNum = idx;

    return NFC_OK;
}

NfcErrorCode NFC_HAL_NciT3TPolling(NfcHalT3tPollCmdParams *cmdParams, NfcHalT3tSensfRes *ntfParams)
{
    if (cmdParams == NULL || ntfParams == NULL) {
        return NFC_ERR_CODE_T3T_INPUT_PARAM_INVALID;
    }
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    uint8_t cmd[] = {0x21, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00};
    uint16_t idx = NCI_HEAD_LEN;
    cmd[idx++] = UTIL_Byte1(cmdParams->sc);
    cmd[idx++] = UTIL_Byte0(cmdParams->sc);
    cmd[idx++] = cmdParams->rc;
    cmd[idx++] = cmdParams->tsn;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, idx, recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_T3T_POLLING_RECV_RSP_ERR;
    } else if (recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK) {
        return NFC_ERR_CODE_T3T_POLLING_RSP_STATUS_ERR;
    }

    return NFC_HAL_T3TPollRecvNtf(ntfParams, cmdParams->timeout);
}
#endif // HAVE_NFC_POLL

#ifdef HAVE_NFC_LISTEN
static uint8_t NFC_HAL_AppendUniqueArrayItem(uint8_t *array, uint8_t idx, uint8_t val)
{
    bool isDuplicated = false;
    for (uint8_t i = 0; i < idx; ++i) {
        if (array[i] == val) {
            isDuplicated = true;
            break;
        }
    }
    if (!isDuplicated) {
        array[idx++] = val;
    }

    return idx;
}

NfcErrorCode NFC_HAL_NfceeDiscovery(uint8_t *nfceeList, uint8_t size, uint8_t *num)
{
    uint8_t nfceeDiscovCmd[] = {0x22, 0x00, 0x00};
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN] = {0};
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(nfceeDiscovCmd, sizeof(nfceeDiscovCmd), recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_NFCEE_DISCOV_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_NFCEE_DISCOV_RSP_STATUS_ERR;
    }

    *num = 0;
    uint8_t idx = 0;
    uint8_t recvNtf[HAL_SHORT_BUFF_LEN] = {0};
    uint16_t recvLen = HAL_SHORT_BUFF_LEN;
    if (recvRsp[NCI_HEAD_LEN + 1] < size) {
        size = recvRsp[NCI_HEAD_LEN + 1];
    }
    while (idx < size) {
        recvLen = HAL_SHORT_BUFF_LEN;
        ret = NFC_HAL_ReceiveNciPkt(recvNtf, &recvLen, g_nfcHalCtx.readTimeout);
        if (ret != NFC_OK) {
            break;
        }
        nfceeList[idx++] = recvNtf[NCI_HEAD_LEN];
    }

    if (idx == 0) {
        return NFC_ERR_CODE_NFCEE_DISCOV_NOT_FOUND_ERR;
    }
    *num = idx;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_NfceeEnable(uint8_t nfceeId, uint8_t *techModeList, uint8_t size, uint8_t *num)
{
    uint8_t nfceeEnableCmd[] = {0x22, 0x01, 0x02, nfceeId, 0x01};
    uint8_t recvBuff[HAL_SHORT_BUFF_LEN];
    uint16_t recvLen = HAL_SHORT_BUFF_LEN;
    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(nfceeEnableCmd, sizeof(nfceeEnableCmd), recvBuff, &recvLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_NFCEE_ENABLE_RECV_RSP_ERR;
    } else if ((recvBuff[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_NFCEE_ENABLE_RSP_STATUS_ERR;
    }

    *num = 0;
    uint8_t idx = 0;
    bool isNfceeAct = false;
    while (idx < size) {
        recvLen = HAL_SHORT_BUFF_LEN;
        ret = NFC_HAL_ReceiveNciPkt(recvBuff, &recvLen, g_nfcHalCtx.readTimeout);
        if (ret != NFC_OK) {
            break;
        }

        if (recvBuff[0] == 0x61 && recvBuff[1] == 0x0A && recvBuff[NCI_RF_DISCOV_REQ_NTF_TYPE_POS] == 0x00) {
            idx = NFC_HAL_AppendUniqueArrayItem(techModeList, idx, recvBuff[NCI_RF_DISCOV_REQ_NTF_TECHMODE_POS]);
        } else if (recvBuff[0] == 0x01 && recvBuff[1] == 0x00) {
            // Call upper layer callback to process HCI cmds from NFCEE
            if (g_nfcHalCtx.processHciFromNfcee != NULL) {
                g_nfcHalCtx.processHciFromNfcee(recvBuff, recvLen);
            }
        } else if (recvBuff[0] == 0x62 && recvBuff[1] == 0x01 && recvBuff[NCI_LEN_POS] == 0x01 &&
                   recvBuff[NCI_HEAD_LEN] == 0x00) {
            isNfceeAct = true;
        }
    }

    if (!isNfceeAct) {
        return NFC_ERR_CODE_NFCEE_ENABLE_ACT_FAIL;
    }
    if (idx == 0) {
        return NFC_ERR_CODE_NFCEE_ENABLE_RF_DISCOV_REQ_NOT_FOUND_ERR;
    }
    *num = idx;
    return NFC_OK;
}

NfcErrorCode NFC_HAL_SetListenRouteTbl(uint8_t *cmd, uint16_t len)
{
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, len, recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_SET_LISTEN_ROUTE_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_SET_LISTEN_ROUTE_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_RestoreHciRam(uint8_t *cmd, uint16_t len)
{
    uint8_t recvRsp[HAL_SHORT_BUFF_LEN];
    uint16_t rspLen = HAL_SHORT_BUFF_LEN;

    cmd[0] = 0x2F;
    cmd[1] = 0x05;
    cmd[NCI_LEN_POS] = (uint8_t)len;

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, len + NCI_HEAD_LEN, recvRsp, &rspLen);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_PROP_HCI_RAM_RESTORE_RECV_RSP_ERR;
    } else if ((recvRsp[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_PROP_HCI_RAM_RESTORE_RSP_STATUS_ERR;
    }
    return NFC_OK;
}

NfcErrorCode NFC_HAL_GetHciRam(uint8_t nfceeId, uint8_t *buff, uint16_t *len)
{
    uint8_t cmd[] = {0x2F, 0x04, 0x01, nfceeId};

    NfcErrorCode ret = NFC_HAL_SendNciCmdAndRecvRsp(cmd, sizeof(cmd), buff, len);
    if (ret != NFC_OK) {
        return NFC_ERR_CODE_PROP_HCI_RAM_STORE_RECV_RSP_ERR;
    } else if ((buff[NCI_HEAD_LEN] != NCI_STATUS_OK)) {
        return NFC_ERR_CODE_PROP_HCI_RAM_STORE_RSP_STATUS_ERR;
    }
    return NFC_OK;
}
#endif // HAVE_NFC_LISTEN

#ifdef __cplusplus
}
#endif