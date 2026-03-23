
#include "msg_center_ota.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif
static const msg_center_cmd_map_t g_msg_center_auto_ota_tbl[] = {
#ifdef SUPPORT_AUTO_OTA_SERVER
    {MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_START, msg_center_ota_rec_upg_start},
    {MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_CHECK, msg_center_ota_rec_upg_check},
    {MSGCENTER_CMD_OTA, MSGCENTER_TYPE_ID_OTA_UPG_CHECK_RESULT, msg_center_ota_rec_upg_check_result},
#endif
};

errcode_t msg_center_auto_ota_type_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_auto_ota_tbl) / sizeof(g_msg_center_auto_ota_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_auto_ota_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void auto_ota_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_OTA, msg_center_auto_ota_type_dispatch);
}

#ifdef SUPPORT_AUTO_OTA_SERVER
APP_FEATURE_INIT_PRI(auto_ota_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS