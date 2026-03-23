/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_PROTOCOL_H
#define MSG_CENTER_PROTOCOL_H

#include "errcode.h"
#include "diag_service.h"
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "bts_le_gap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TLV_LEN_BYTE_SHIFT 8
#define TLV_LEN_FLAG_SHIFT 7
#define TLV_NO_CHILD_ONE_BYTE_LEN_MAX 0x7F
#define TLV_LEN_ONE_BYTE 1
#define TLV_LEN_TWO_BYTE 2
#define TLV_LEN_THREE_BYTE 3
#define TLV_LEN_FOUR_BYTE 4
#define TLV_ACTUAL_VALUE_MASK 0x7F
#define TLV_LEN_NO_FATHER_MASK 0x80

#define CRC_PAD_LEN 2

#define NAVI_DST_NAME_LEN 10 /* TODO */
#define NAVI_DST_INFO_LEN 10 /* TODO */

#define MSG_CENTER_SEND_GENERAL_TYPE 0xff

#define FILE_SPILT_MAX_SIZE 970 /* SPP包不能超过990字节 */
#define FILE_NAME_MAX_LEN 100

typedef enum msg_center_pcm_end_flag {
    PCM_END_FLAG_AI_CHAT,     /* AI对话 */
    PCM_END_FLAG_WATCH_FACE,  /* 表盘 */
    PCM_END_FLAG_VECTOR_NAVI, /* 矢量地图 */
} msg_center_pcm_end_flag_t;

typedef enum msg_center_ping_type_id {
    MSGCENTER_TYPE_ID_PING_REC = 0x1,
    MSGCENTER_TYPE_ID_PING_RSP = 0x2,
} msg_center_ping_type_id_t;

typedef enum msg_center_cmd_id {
    MSGCENTER_CMD_XIAODU             = 0x1,
    MSGCENTER_CMD_NAVIGATION         = 0x2,
    MSGCENTER_CMD_VECTOR_NAVI        = 0x3,
    MSGCENTER_CMD_ALIPAY             = 0x4,
    MSGCENTER_CMD_OTA                = 0x5,
    MSGCENTER_CMD_HC                 = 0x6,
    MSGCENTER_CMD_PING               = 0x7,
    MSGCENTER_CMD_WATCH_CONNECT      = 0x8,
    MSGCENTER_CMD_APP_STORE          = 0x9,
    MSGCENTER_CMD_DIAL_MARKET        = 0xa,
    MSGCENTER_CMD_PHONE_MSG          = 0xb, // 增加消息通知;
    MSGCENTER_CMD_DEVICE             = 0xc,
    MSGCENTER_CMD_MAX
} msg_center_cmd_id_t;

/* 错误码定义 */
typedef enum {
    ERR_MSGCENTER_SUCC              = 100000,
    ERR_MSGCENTER_UNKNOWN_ERR       = 100001,
    ERR_MSGCENTER_NONSUPT_SRV       = 100002,
    ERR_MSGCENTER_NONSUPT_CMD       = 100003,
    ERR_MSGCENTER_NO_PERMIT         = 100004,
    ERR_MSGCENTER_SYS_BUSY          = 100005,
    ERR_MSGCENTER_REQ_FMT_ERR       = 100006,
    ERR_MSGCENTER_PARA_ERR          = 100007,
    ERR_MSGCENTER_NO_MEM            = 100008,
    ERR_MSGCENTER_RSP_TIMEOUT       = 100009,
    ERR_MSGCENTER_LOW_BATTERY       = 100010,
    ERR_MSGCENTER_DEV_WRONG         = 100011,
    ERR_MSGCENTER_NO_RPT            = 0x7FFFFFFF, // 表示主动上报或者查询接口不额外上报消息
} msg_center_op_code_t;

typedef errcode_t (*msg_center_pkt_recv_hook)(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
typedef struct {
    uint8_t cmd_id;
    uint8_t type;
    msg_center_pkt_recv_hook handler;
} msg_center_cmd_map_t;

typedef struct {
    uint8_t cmd;
    msg_center_pkt_recv_hook handler;
    struct osal_list_head node;
} msg_center_cmd_t;

typedef struct {
    uint8_t type;
    uint8_t len;
    uint8_t data[0];
} msg_center_pkt_tlv_t;

typedef struct {
    char name[NAVI_DST_NAME_LEN];
    char info[NAVI_DST_INFO_LEN];
} navi_dst_list_t;

// subtype: 是否包括子节点; id: type id
#define msg_center_build_tlv_type(subtype, id)       ((subtype) << 0x7 | (id))

static inline uint8_t msg_center_build_tlv_len(uint16_t usr_len)
{
    if (usr_len <= 0x7F) {
        return (uint8_t)((1U << 0x7) | usr_len);
    } else if (usr_len > 0xFF) {
        return 2; /* fill 2 to lenth field. */
    } else {
        return 1; /* fill 1 to lenth field. */
    }
}

/* 计算tlv结构中length的字节个数 */
static inline uint16_t msg_center_get_tlv_len_num(uint16_t usr_len)
{
    if (usr_len <= 0x7F) {
        return 0;
    } else if (usr_len > 0xFF) {
        return 2; /* Use 2 bytes to identify the length. */
    } else {
        return 1; /* Use 1 byte to identify the length. */
    }
}

/* 计算tlv结构中length的实际长度 */
static inline uint8_t msg_center_get_tlv_len_of_length(uint16_t usr_len)
{
    return msg_center_get_tlv_len_num(usr_len) + 1;
}

static inline void *msg_center_get_tlv_payload(void *usr_data)
{
    msg_center_pkt_tlv_t *tlv = (msg_center_pkt_tlv_t *)usr_data;
    uint8_t offset = 0;

    if ((tlv->len & 0x80) != 0) {
        offset = (uint8_t)sizeof(msg_center_pkt_tlv_t);
    } else {
        offset = (uint8_t)sizeof(msg_center_pkt_tlv_t) + (tlv->len & 0x7F);
    }

    return ((uint8_t *)usr_data + offset);
}

static inline uint8_t msg_center_get_tlv_tl_len(void *usr_data)
{
    msg_center_pkt_tlv_t *tlv = (msg_center_pkt_tlv_t *)usr_data;
    uint8_t offset = 0;

    if ((tlv->len & 0x80) != 0) {
        offset = (uint8_t)sizeof(msg_center_pkt_tlv_t);
    } else {
        offset = (uint8_t)sizeof(msg_center_pkt_tlv_t) + (tlv->len & 0x7F);
    }

    return offset;
}

errcode_t msg_center_pkt_proc(diag_ser_data_t *data);
uint8_t msg_center_get_response_cmd(uint8_t cmd_id);
diag_ser_data_t *msg_center_encap_diag_info(diag_ser_header_t *header, uint16_t tlv_len, uint8_t cmd_id);
int32_t msg_center_fill_tlv(msg_center_pkt_tlv_t *tlv, uint8_t type, uint8_t *value, uint16_t v_len);
errcode_t msg_center_send_data(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
void msg_center_split_file(char *origin_file, int size, char *flist);
void msg_center_combine_file(char *flist, char *target_file_name);
diag_frame_fid_t msg_center_get_trans_channel(void);
void msg_center_set_trans_channel(diag_frame_fid_t select);
void msg_center_set_ble_conn_state(gap_ble_conn_state_t state);
gap_ble_conn_state_t msg_center_get_ble_conn_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_PROTOCOL_H */
