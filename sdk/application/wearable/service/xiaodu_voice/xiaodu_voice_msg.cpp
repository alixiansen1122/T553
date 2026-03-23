
#include "xiaodu_voice.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
#include "msg_center_common.h"
#include "wearable_log.h"

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

static const msg_center_cmd_map_t g_msg_center_xiaodu_voice_tbl[] = {
#ifdef JS_ENABLE
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_START,                     msg_center_xiaodu_start},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_RECV_XIAODU_INFO,          msg_center_xiaodu_recv_xiaodu_info},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_RECV_WENXIN_INFO,          msg_center_xiaodu_recv_wenxin_info},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_RECV_TTS,                  msg_center_xiaodu_recv_tts},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_START_RECORD,              msg_center_xiaodu_start_record},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM,           msg_center_xiaodu_send_pcm_stream},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_END_RECORD,                msg_center_xiaodu_send_end_record},
    { MSGCENTER_CMD_XIAODU,      MSGCENTER_TYPE_ID_XIAODU_SEND_JS_EXIT,              msg_center_xiaodu_send_stop_answer},
#endif
};

errcode_t msg_center_xiaodu_voice_type(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_xiaodu_voice_tbl) / sizeof(g_msg_center_xiaodu_voice_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_xiaodu_voice_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void xiaodu_voice_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_XIAODU, msg_center_xiaodu_voice_type);
}

#ifdef JS_ENABLE
APP_FEATURE_INIT_PRI(xiaodu_voice_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS