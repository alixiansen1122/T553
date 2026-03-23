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
#include "msg_center_hc_demo.h"
#include "wearable_log.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "common_def.h"

#ifdef __cplusplus
extern "C" {
#endif

errcode_t msg_center_hc_card_circulation(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_hc_card_circulation\r\n");
    /* notify UI or call UI load */

    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_transfer_state_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    msg_center_hc_transfer_state_t state = MSGCENTER_HC_TRANSFER_STATE_ERROR;
    uint8_t ack = 1;

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    char *bundle_name = (char *)malloc(payload_len + 1);
    if (bundle_name == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_transfer_state_recv: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(bundle_name, payload_len, tlv_payload, payload_len);
    bundle_name[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_transfer_state_recv bundle_name = %s\r\n", bundle_name);

    /* call JSI func to return 0 1 2 3 4 */
    // state = func();

    ret = msg_center_send_data(cmd_id, type + 1, &state, sizeof(state));
    if (ret != ERRCODE_SUCC) {
        free(bundle_name);
        bundle_name = nullptr;
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_transfer_state_send fail, ret = %d\r\n", ret);
        return ERRCODE_FAIL;
    }

    free(bundle_name);
    bundle_name = nullptr;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_transfer_state_send succ\r\n");
    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_start_js(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    msg_center_hc_transfer_state_t state = MSGCENTER_HC_TRANSFER_STATE_ERROR;
    uint8_t ack = 1;

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    char *bundle_name = (char *)malloc(payload_len + 1);
    if (bundle_name == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_start_js: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }
    (void)memcpy_s(bundle_name, payload_len, tlv_payload, payload_len);
    bundle_name[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_start_js bundle_name = %s\r\n", bundle_name);

#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(30000); // 30000ms
#endif
#ifdef JS_ENABLE
    StartJsApp(bundle_name);
#endif

    free(bundle_name);
    bundle_name = nullptr;
    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_sync_calendar_event(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    msg_center_hc_transfer_state_t state = MSGCENTER_HC_TRANSFER_STATE_ERROR;
    uint8_t ack = 1;

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_sync_calendar_event, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len);
    /* call JSI */

    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_req_calendar_event(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    msg_center_hc_transfer_state_t state = MSGCENTER_HC_TRANSFER_STATE_ERROR;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "hc_req_calendar_event, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len);

    /* JSI call this func to app*/
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_js_status_notify(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;
    msg_center_hc_transfer_state_t state = MSGCENTER_HC_TRANSFER_STATE_ERROR;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "js_status_notify, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len);

    /* JSI call this func to app*/
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_dial_share_send(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send_dial_share, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len);
    return ERRCODE_SUCC;
}

errcode_t msg_center_hc_dial_share_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    errcode_t ret = ERRCODE_SUCC;

    char *dial_file = (char *)malloc(payload_len + 1);
    if (dial_file == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dial_share: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(dial_file, payload_len, tlv_payload, payload_len);
    dial_file[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "rec_dial_share, usr_data = %d, usr_len = %d\r\n");
    /* notify UI or call UI load */

    free(dial_file);
    dial_file = nullptr;
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif