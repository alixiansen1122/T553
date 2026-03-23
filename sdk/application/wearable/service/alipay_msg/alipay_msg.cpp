
#include "alipay_app.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif
static const msg_center_cmd_map_t g_msg_center_alipay_sec_tbl[] = {
#ifdef SUPPORT_ALIPAY_SEC
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_NETWORK_STATUS,            msg_center_alipay_network_status},
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_HOST_INFO,                 msg_center_alipay_hostent_recv},
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_CONNECT,                   msg_center_alipay_connect_recv},
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_TCPDATA_RECV,              msg_center_alipay_tcpdata_recv},
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_CLOSE,                     msg_center_alipay_close},
    { MSGCENTER_CMD_ALIPAY,      MSGCENTER_TYPE_ID_ALIPAY_TIME_SYNC,                 msg_center_alipay_time_sync},
#endif
};

errcode_t msg_center_alipay_sec_type_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_alipay_sec_tbl) / sizeof(g_msg_center_alipay_sec_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_alipay_sec_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void alipay_sec_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_ALIPAY, msg_center_alipay_sec_type_dispatch);
}

#ifdef SUPPORT_ALIPAY_SEC
APP_FEATURE_INIT_PRI(alipay_sec_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS