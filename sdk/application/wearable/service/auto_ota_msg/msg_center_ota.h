/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_OTA_H
#define MSG_CENTER_OTA_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_ota_type_id {
    MSGCENTER_TYPE_ID_OTA_UPG_START = 0x1,
    MSGCENTER_TYPE_ID_OTA_UPG_CHECK = 0x2,
    MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT = 0x3,
} msg_center_ota_type_id_t;

errcode_t msg_center_ota_rec_upg_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_ota_rec_upg_check(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_ota_rec_upg_check_result(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
void msg_center_ota_upgrade_start(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_OTA_H */
