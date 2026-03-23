
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
#include "dial_market.h"

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

static const msg_center_cmd_map_t g_msg_center_dial_market_tbl[] = {
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_GET_SYS_INFO, msg_center_dial_market_get_sys_info},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_GET_DEV_CAP, msg_center_dial_market_get_dev_cap},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_GET_ONE_WATCH_INFO, msg_center_dial_market_get_one_watch_info},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_INSTALL_WATCH, msg_center_dial_market_install_watch},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_UNINSTALL_WATCH, msg_center_dial_market_uninstall_watch},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_SET_WATCH, msg_center_dial_market_set_watch},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_GET_ALL_WATCH_INFO, msg_center_dial_market_get_all_watch_info},
    { MSGCENTER_CMD_DIAL_MARKET, MSGCENTER_TYPE_ID_DM_GET_CUR_WATCH_INFO, msg_center_dial_market_get_cur_watch_info},
};

errcode_t msg_center_dial_market_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_dial_market_tbl) / sizeof(g_msg_center_dial_market_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_dial_market_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void dial_market_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_DIAL_MARKET, msg_center_dial_market_dispatch);
}

APP_FEATURE_INIT_PRI(dial_market_init, LAYER_INIT_LEVEL_4);
#ifdef __cplusplus
}
#endif
}  // namespace OHOS