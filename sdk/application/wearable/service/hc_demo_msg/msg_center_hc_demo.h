/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_HC_DEMO_H
#define MSG_CENTER_HC_DEMO_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_hc_type_id {
    MSGCENTER_TYPE_ID_HC_CARD_CIRCULATION = 0x1,
    MSGCENTER_TYPE_ID_HC_TRANSFER_STATE_RECV = 0x2,
    MSGCENTER_TYPE_ID_HC_TRANSFER_STATE_SEND = 0x3,
    MSGCENTER_TYPE_ID_HC_TRANS_JS_PKG_DONE = 0x4,
    MSGCENTER_TYPE_ID_HC_START_JS = 0x5,
    MSGCENTER_TYPE_ID_HC_SYNC_CALENDAR_EVENT = 0x6,
    MSGCENTER_TYPE_ID_HC_REQ_CALENDAR_EVENT = 0x7,
    MSGCENTER_TYPE_ID_HC_DIAL_SHARE_SEND = 0x8,
    MSGCENTER_TYPE_ID_HC_DIAL_SHARE_RECV = 0x9,
} msg_center_hc_type_id_t;

typedef enum msg_center_hc_transfer_state {
    MSGCENTER_HC_TRANSFER_STATE_UNINSTALL = 0x0,
    MSGCENTER_HC_TRANSFER_STATE_INSTALLED = 0x1,
    MSGCENTER_HC_TRANSFER_STATE_ONLY_STARTED = 0x2,
    MSGCENTER_HC_TRANSFER_STATE_STARTED_AND_SYNC = 0x3,
    MSGCENTER_HC_TRANSFER_STATE_ERROR = 0x4,
} msg_center_hc_transfer_state_t;

errcode_t msg_center_hc_card_circulation(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_transfer_state_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_start_js(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_sync_calendar_event(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_req_calendar_event(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_dial_share_send(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_dial_share_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_hc_js_status_notify(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_HC_DEMO_H */
