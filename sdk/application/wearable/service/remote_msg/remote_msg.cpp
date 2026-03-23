/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */

#include <stdio.h>
#include "securec.h"
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#include "securec.h"
#include "msg_center.h"
#include "msg_center_common.h"
#include "ohos_init.h"
#include "phone_contacts.h"
#include "remote_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

static const msg_center_cmd_map_t g_msg_center_phone_msg_tbl[] = {
    {MSGCENTER_CMD_PHONE_MSG, MSGCENTER_TYPE_ID_MSG_THIRD_APP, msg_center_phone_msg_rec},
    {MSGCENTER_CMD_PHONE_MSG, MSGCENTER_TYPE_ID_MSG_SMS, msg_center_phone_msg_rec},
    {MSGCENTER_CMD_PHONE_MSG, MSGCENTER_TYPE_ID_MSG_PHONE, msg_center_phone_msg_rec},
    {MSGCENTER_CMD_PHONE_MSG, MSGCENTER_TYPE_ID_UPDATE_CONTACTS, msg_center_update_contacts},
    {MSGCENTER_CMD_PHONE_MSG, MSGCENTER_TYPE_ID_LOOKUP_CONTACTS, msg_center_lookup_contacts},
};

errcode_t msg_center_phone_msg_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t msg_result = 0;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_phone_msg_rec, cmd_id = %d, type = %d\r\n", cmd_id, type);
    Request request;
    request.msgId = type;
    request.len = payload_len;
    request.data = tlv_payload;
    MsgCenterNotifyProc(TOPIC_DEMO_ALL_MSG, &request);
    ret = msg_center_send_data(cmd_id, type, &msg_result, sizeof(msg_result));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "phone msg_result fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_update_contacts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t msg_result = 0;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "update_contacts, cmd_id = %d, type = %d\r\n", cmd_id, type);
    Request request;
    request.msgId = type;
    request.len = payload_len;
    request.data = tlv_payload;
    updata_contacts(&request);
    ret = msg_center_send_data(cmd_id, type, &msg_result, sizeof(msg_result));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_update_contacts fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_lookup_contacts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    uint8_t msg_result = 0;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "lookup_contacts, cmd_id = %d, type = %d\r\n", cmd_id, type);
    std::string lookupContacts;
    uint8_t result = lookup_contacts(lookupContacts);
    if (!result) {
        ret = msg_center_send_data(cmd_id, type, (void *)lookupContacts.c_str(), lookupContacts.size());
    } else {
        ret = msg_center_send_data(cmd_id, type, (void *)&result, 1);
    }
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_lookup_contacts fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}


errcode_t msg_center_phone_msg_type_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_phone_msg_type_dispatch, cmd_id = %d, type = %d\r\n",
                  cmd_id, type);
    uint8_t tbl_size = sizeof(g_msg_center_phone_msg_tbl) / sizeof(g_msg_center_phone_msg_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_phone_msg_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void phone_msg_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_PHONE_MSG, msg_center_phone_msg_type_dispatch);
}

APP_FEATURE_INIT_PRI(phone_msg_init, LAYER_INIT_LEVEL_4);

#ifdef __cplusplus
}
#endif