
#include "app_store.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
namespace OHOS {
#ifdef __cplusplus
extern "C" {
#endif
static const msg_center_cmd_map_t g_appStoreTbl[] = {
    { MSGCENTER_CMD_APP_STORE,  APP_STORE_OPERATION,                 AppStorePkgOperation },
    { MSGCENTER_CMD_APP_STORE,  APP_STORE_GET_PKG_LIST,              AppStoreGetPkgList },
    { MSGCENTER_CMD_APP_STORE,  APP_STORE_GET_BUNDLEINFO,            AppStoreGetBundleInfo },
};

errcode_t AppStoreTypeDispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tblSize = sizeof(g_appStoreTbl) / sizeof(g_appStoreTbl[0]);
    uint8_t i;
    for (i = 0; i < tblSize; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_appStoreTbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void AppStoreInit(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_APP_STORE, AppStoreTypeDispatch);
}

APP_FEATURE_INIT_PRI(AppStoreInit, LAYER_INIT_LEVEL_4);

#ifdef __cplusplus
}
#endif
}  // namespace OHOS