/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT parse source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02， Create file. \n
 */

#ifndef    RIL_INTERFACE_H
#define    RIL_INTERFACE_H

#include "lwip/ip_addr.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#define RIL_OPERATOR_INFO_SIZE   20
#define EXT_UE_MAX_CELL_INFO_NUM    10
#define RIL_SEND_CMD_DATA_SIZE      500
#ifdef CONFIG_FT_DITING
#define UART_CHAN_TEST_MAX_RETURN_SIZE 50
#endif
typedef enum : uint8_t {
    /* AT指令通道 */
    ID_RIL_AT_CMD_EXC   = 0x00,       /* AT指令发送 */
    /* 设备信息 */
    ID_RIL_COMM_GET_SOFTWARE = 0x01,  /* 软件版本号 */
    ID_RIL_COMM_GET_IMEI_REQ,         /* IMEI设备识别码 */
    ID_RIL_COMM_GET_HAVE_CALI,        /* 是否校准 */
    /* SIM卡信息 */
    ID_RIL_SIM_GET_IN_POSITION,       /* 卡是否在位 */
    ID_RIL_SIM_GET_PIN_BLOCK,         /* Pin-block */
    ID_RIL_SIM_GET_PUN_BLOCK,         /* Puk-block */
    ID_RIL_SIM_GET_IMSI,              /* IMSI用户识别码 */
    ID_RIL_SIM_GET_MSISDN,            /* 本机号码显示 */
    /* 信号状态 */
    ID_RIL_RADIO_GET_SIGNAL_INFO,     /* 信号状态 */
    /* 小区信息 */
    ID_RIL_RADIO_GET_CELL_INFO,       /* 当前小区信息 */
    ID_RIL_RADIO_GET_NEIGHBOR_INFO,   /* 邻区信息 */
    /* 驻网状态 */
    ID_RIL_RADIO_GET_REG_STATUS,      /* 是否驻网 */
    ID_RIL_RADIO_GET_OPERATOR_INFO,   /* 显示运营商信息 */
    ID_RIL_RADIO_GET_IMS_REG,         /* 是否IMS驻网 */
    ID_RIL_RADIO_GET_PDP_INFO,        /* 是否PDP驻网显示当前IP地址 */
    ID_RIL_RADIO_SEARCH_START,        /* 手动搜网，显示可选网络 */
    /* 网络速率 */
    ID_RIL_NETWORK_DL_RATE,           /* 网络下行速率 */
    ID_RIL_NETWORK_UL_RATE,           /* 网络上行速率 */
    ID_RIL_NETWORK_UL_TX_STATS,       /* 上行发数状态 */
    ID_RIL_NETWORK_STATE,             /* 上报普通承载激活状态 */
    /* 通话接口 */
    ID_RIL_CALL_SET_AUTO_ANSWER,      /* 自动接听开关 */
    ID_RIL_CALL_SET_LOOP_BACK,        /* 电话语音loopback开关 */
    ID_RIL_CALL_ORIG_REQ,             /* 拨打电话 */
    ID_RIL_CALL_MT_ACCEPT_REQ,        /* 手动接听 */
    ID_RIL_CALL_REL_REQ,              /* 手动挂断 */
    /* SMS */
    ID_RIL_SMS_GET_SMSC,              /* 显示短信中心号码 */
    ID_RIL_SMS_RECEIVE_REGISTER,      /* 收单条短信 */
    ID_RIL_SMS_SEND,                  /* 发单条短信 */
    /* UPDATED */
    ID_RIL_UPDATED_START,             /* 升级开始 */
    ID_RIL_UPDATED_PACKEAGE,          /* 升级数据包 */
    ID_RIL_UPDATED_END,               /* 升级完成 */
    /* 产测 */
    ID_RIL_UART_CHANNEL_TEST,         /* 产测UART通道测试 */
    ID_RIL_SYS_ERR_IND,               /* 死机维测 */
    ID_RIL_HANDLE_MAX,
} ril_handle_cmd_id_t;


typedef enum : uint8_t {
    RIL_RET_SUCCESS = 0x00,
    RIL_RET_FAIL_INVALID_PARAM,
    RIL_RET_FAIL_MEMCPY,
    RIL_RET_FAIL_MALLOC,
    RIL_RET_FAIL_SEND,
    RIL_RET_FAIL_UART_NOT_CONNECT,
    RIL_RET_FAIL_RECIVE_TIMEOUT,
    RIL_RET_FAIL_RECIVE_ERR,
    RIL_RET_FAIL_PROCESS,
} ril_errcode;

typedef struct {
    char numeric[RIL_OPERATOR_INFO_SIZE];
    char name_short[RIL_OPERATOR_INFO_SIZE];
    char name_long[RIL_OPERATOR_INFO_SIZE];
} ril_operator_info;

typedef struct {
    ip_addr_t    ip_addr[2];
    uint8_t      ip_num;
} ril_ip_addr_info;

typedef struct {
    int16_t rsrp;
    int16_t rsrq;
    int16_t rssi;
    int16_t snr;
} soc_ue_cell_radio_info;

typedef struct {
    uint8_t version;
    bool primary_cell;
    bool valid;
    uint16_t pci;
    uint32_t earfcn;
    int16_t rsrp;
    int16_t rsrq;
    int16_t rssi;
    int16_t snr;
    int16_t srxlev;
} soc_ue_cell_info;

typedef struct {
    uint8_t neighbor_cell_num;
    soc_ue_cell_info cell_info[EXT_UE_MAX_CELL_INFO_NUM];
} soc_ue_neighbor_info;

typedef enum : uint8_t {
    RIL_USER_LEVEL_NULL,
    RIL_USER_LEVEL_ERROR,
    RIL_USER_LEVEL_WARN,
    RIL_USER_LEVEL_INFO,
    RIL_USER_LEVEL_ALL,
} ril_user_log_level;

typedef struct {
    uint32_t                 download_type;
    uint32_t                 sn;
    uint32_t                 len;
    uint8_t                  crc; /* Length: 1 LengthField: crc_length */
    uint8_t                  *data; /* Length: 512 LengthField: len */
} ril_upadated_package_t;

typedef int32_t (*respond_callback_t)(uint8_t* data, size_t len);

void ril_init(void);
int32_t ril_process_msg_handle(uint16_t cmd_id, uint8_t *data, uint16_t size);
/* AT指令通道 */
int32_t ril_at_cmd_exc(const char *str);
void ril_register_at_reply_callback(respond_callback_t rsp_callback);
int32_t ril_at_cmd_sync_get_info(const char *at_str, uint8_t *data, uint16_t size, uint32_t timeout);
int32_t ril_at_sync_multiple_get_info(const char *at_str, uint8_t *data, uint32_t size, uint32_t times,
    uint32_t timeout);
/* 设备信息 */
int32_t ril_get_software_version(char *ver, size_t size);
int32_t ril_get_imei(char* imei, size_t len);
int32_t ril_get_if_ue_calibrated(bool *calibrate);
/* SIM卡信息 */
int32_t ril_get_is_usim_in_position(bool *status);
int32_t ril_get_is_usim_pin_block(bool *status);
int32_t ril_get_is_usim_puk_block(bool *status);
int32_t ril_get_imsi(char* imsi, size_t len);
int32_t ril_get_usim_msisdn(char* msisdn, size_t len);
/* 信号状态 */
int32_t ril_get_radio_signal_info(soc_ue_cell_radio_info *signal_info);
/* 小区信息 */
int32_t ril_get_ue_registration_status(soc_ue_cell_info *reg_status);
int32_t ril_get_ue_neighbor_cell_info(soc_ue_neighbor_info *neighbor_info);
/* 驻网状态 */
int32_t ril_get_radio_reg_status(uint8_t *status);
int32_t ril_get_radio_operator_info(ril_operator_info *operator_info);
int32_t ril_get_radio_ims_reg(bool *status);
int32_t ril_get_ue_addr(ril_ip_addr_info *ip_addr_info);
int32_t ril_get_radio_search_start(void);
int32_t ril_get_network_ul_tx_stats(bool *status);
/* 通话接口 */
int32_t ril_set_auto_answer(uint8_t time_s);
int32_t ril_set_call_orig_req(const char* call_str);
int32_t ril_set_call_mt_accept_req(void);
int32_t ril_set_call_rel_req(void);
/* SMS接口 */
int32_t ril_get_sms_smsc(char* smsc, size_t len);
int32_t ril_ue_send_short_message(const char* number, const char *sms_data, uint16_t sms_len);
int32_t ril_ue_read_short_message(int8_t sms_index, uint8_t *sms_data, int16_t read_max_length);
/* 设置VoLTE日志级别接口 */
int32_t ril_set_user_log_level(ril_user_log_level level);

/* 升级 */
int32_t ril_systerm_updated_start(void);
int32_t ril_systerm_updated_end(void);
int32_t ril_systerm_updated_download_package(ril_upadated_package_t  *upadated_package);
void ril_register_sys_err_report_callback(respond_callback_t rsp_callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
