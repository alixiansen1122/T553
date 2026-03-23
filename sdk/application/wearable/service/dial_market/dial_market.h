/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef DIAL_MARKET_H
#define DIAL_MARKET_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_CHIP_PLAT_LEN 5
#define MAX_CHIP_MODEL_LEN 5
#define MAX_OS_LEN 23
#define MAX_API_LEVEL_LEN 3
#define MAX_WATCH_PROTO_VER_LEN 4
#define MAX_WATCH_INTERNAL_VER 6
#define MAX_RESOLUTION_LEN 8
#define MAX_SUPPORT_PROTOCOL 8
#define MAX_WATCH_CNT 64

typedef enum msg_center_dial_market_type_id {
    MSGCENTER_TYPE_ID_DM_GET_SYS_INFO       = 0x1,
    MSGCENTER_TYPE_ID_DM_GET_DEV_CAP        = 0x2,
    MSGCENTER_TYPE_ID_DM_GET_ONE_WATCH_INFO = 0x3,
    MSGCENTER_TYPE_ID_DM_INSTALL_WATCH      = 0x4,
    MSGCENTER_TYPE_ID_DM_UNINSTALL_WATCH    = 0x5,
    MSGCENTER_TYPE_ID_DM_SET_WATCH          = 0x6,
    MSGCENTER_TYPE_ID_DM_GET_ALL_WATCH_INFO = 0x7,
    MSGCENTER_TYPE_ID_DM_GET_CUR_WATCH_INFO = 0x8,
} msg_center_dial_market_type_id_t;

typedef struct {
    char chip_platform[MAX_CHIP_PLAT_LEN];
    char chip_model[MAX_CHIP_MODEL_LEN];
    char os_version[MAX_OS_LEN];
    char api_level[MAX_API_LEVEL_LEN];
    uint8_t watch_proto_support_cnt;
    char watch_protocol_verion[MAX_SUPPORT_PROTOCOL][MAX_WATCH_PROTO_VER_LEN];
    char resolution[MAX_RESOLUTION_LEN];
} watch_sys_info;

typedef struct {
    uint8_t kaleidoscope;
    uint8_t effect_3D;
    uint8_t interaction;
    uint8_t video;
    uint16_t video_format; /* 按照CodecFormat做位映射形成能力集 */
} watch_dev_cap_info;

typedef struct {
    uint32_t uuid;
    uint8_t install_state;
    uint8_t is_current_watch;
    uint8_t watch_protocol_version[MAX_WATCH_PROTO_VER_LEN];
    uint8_t watch_version[MAX_WATCH_INTERNAL_VER];
} watch_dial_info;

errcode_t msg_center_dial_market_get_sys_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_get_dev_cap(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_get_one_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_install_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_uninstall_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_set_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_get_all_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_dial_market_get_cur_watch_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* DIAL_MARKET_H */
