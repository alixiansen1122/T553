/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: third application message
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef REMOTE_MSG_H
#define REMOTE_MSG_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_phone_msg_type_id {
    MSGCENTER_TYPE_ID_MSG_THIRD_APP = 0x1, // 三方应用消息
    MSGCENTER_TYPE_ID_MSG_SMS,             // 短信
    MSGCENTER_TYPE_ID_MSG_PHONE,           // 电话
    MSGCENTER_TYPE_ID_UPDATE_CONTACTS,
    MSGCENTER_TYPE_ID_LOOKUP_CONTACTS,
    MSGCENTER_TYPE_ID_MSG_MAX,
} msg_center_msg_notify_type_id_t;

errcode_t msg_center_phone_msg_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_update_contacts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_lookup_contacts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* REMOTE_MSG_H */