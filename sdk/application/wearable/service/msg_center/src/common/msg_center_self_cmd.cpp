#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
#include "msg_center_common.h"
namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

static const msg_center_cmd_map_t g_msg_center_self_cmd_tbl[] = {
#ifdef JS_ENABLE
    { MSGCENTER_CMD_PING,       MSGCENTER_TYPE_ID_PING_REC,           msg_center_ping_rec},
    { MSGCENTER_CMD_PING,       MSGCENTER_TYPE_ID_PING_RSP,           NULL},
#endif
};

errcode_t msg_center_self_cmd_type(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_self_cmd_tbl) / sizeof(g_msg_center_self_cmd_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_self_cmd_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void xiaodu_self_cmd_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_PING, msg_center_self_cmd_type);
}

#ifdef JS_ENABLE
APP_FEATURE_INIT_PRI(xiaodu_self_cmd_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS