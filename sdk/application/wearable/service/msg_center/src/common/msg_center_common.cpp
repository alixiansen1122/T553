/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */
#include <stdio.h>
#include "securec.h"
#include "bts_br_gap.h"
#include "bts_spp.h"
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"


#ifdef __cplusplus
extern "C" {
#endif

errcode_t msg_center_ping_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t ping_result = 0;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_ping_from_phone\r\n");

    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_PHONE) {
        ping_result = is_spp_connected(0) == true ? 1 : 0;
    } else if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        ping_result = msg_center_get_ble_conn_state();
    } else {
        ping_result = 2; /* 2用来表示异常值 */
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "ble or spp not connected!\r\n");
    }

    /* ping req ack */
    ret = msg_center_send_data(cmd_id, type + 1, &ping_result, sizeof(ping_result));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send_ping_ack fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif