/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: xiaodu navi
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include <stdio.h>
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "common_def.h"
#include "js_ability.h"

#ifdef __cplusplus
extern "C" {
#endif

errcode_t msg_center_navigation_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t ack = 1; /* 与手机APK侧约定 */

#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(180000); // 180000ms
#endif
#ifdef JS_ENABLE
    StartJsApp("com.vendor.baidu_navi");
#endif

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_navigation_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    char *data = (char *)malloc(payload_len + 1);
    if (data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_navigation_recv: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv navigate info--> %s\r\n", data);
    DmsLiteSendMsgToJS(data);
    free(data);
    data = nullptr;
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif
