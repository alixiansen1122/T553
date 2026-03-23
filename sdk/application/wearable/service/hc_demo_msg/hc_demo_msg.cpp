
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
#ifdef SUPPORT_HC_DEMO
#include "msg_center_hc_demo.h"
#endif

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif
static const msg_center_cmd_map_t g_msg_center_hc_demo_tbl[] = {
#ifdef SUPPORT_HC_DEMO
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_CARD_CIRCULATION,         msg_center_hc_card_circulation},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_TRANSFER_STATE_RECV,      msg_center_hc_transfer_state_recv},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_TRANSFER_STATE_SEND,      nullptr},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_TRANS_JS_PKG_DONE,        nullptr}, // 暂不实现
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_START_JS,                 msg_center_hc_start_js},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_SYNC_CALENDAR_EVENT,      msg_center_hc_sync_calendar_event},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_REQ_CALENDAR_EVENT,       msg_center_hc_req_calendar_event},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_DIAL_SHARE_SEND,          msg_center_hc_dial_share_send},
    { MSGCENTER_CMD_HC,         MSGCENTER_TYPE_ID_HC_DIAL_SHARE_RECV,          msg_center_hc_dial_share_recv},
#endif
};

errcode_t msg_center_hc_demo_type_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_hc_demo_tbl) / sizeof(g_msg_center_hc_demo_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_hc_demo_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void hc_demo_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_HC, msg_center_hc_demo_type_dispatch);
}

#ifdef SUPPORT_HC_DEMO
APP_FEATURE_INIT_PRI(hc_demo_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS