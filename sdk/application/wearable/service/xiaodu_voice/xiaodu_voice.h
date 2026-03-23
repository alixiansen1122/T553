/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef XIAODU_VOICE_H
#define XIAODU_VOICE_H

#include "errcode.h"
#include "cmsis_os2.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PCM_STREAM_MSG_QUEUE_LEN 1024
#define BAIDU_NAVI_BASE_DIR "/user/baidu_navi/"
#define BAIDU_NAVI_MAX_FILE_LEN 256

typedef enum msg_center_xiaodu_type_id {
    MSGCENTER_TYPE_ID_XIAODU_START = 0x1,
    MSGCENTER_TYPE_ID_XIAODU_RECV_XIAODU_INFO,
    MSGCENTER_TYPE_ID_XIAODU_RECV_WENXIN_INFO,
    MSGCENTER_TYPE_ID_XIAODU_RECV_TTS,
    MSGCENTER_TYPE_ID_XIAODU_START_RECORD,
    MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM,
    MSGCENTER_TYPE_ID_XIAODU_END_RECORD,
    MSGCENTER_TYPE_ID_XIAODU_SEND_JS_EXIT,
} msg_center_xiaodu_type_id_t;

typedef struct {
    uint8_t msg_id;
    uint8_t *data;
    uint16_t len;
} pcm_stream_queue_msg;

osMessageQueueId_t GetPcmMsgQueueId(void);
errcode_t pcm_stream_msg_thread(void *data);
errcode_t pcm_stream_queue_init(void);
errcode_t pcm_stream_create_task(void);

errcode_t msg_center_xiaodu_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_recv_xiaodu_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_recv_xiaodu_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_recv_wenxin_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_start_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_send_pcm_stream(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_send_end_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_send_stop_answer(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_xiaodu_recv_tts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

void msg_center_xiaodu_audio_pcm_clear_res(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* BAIDU_APP_H */
