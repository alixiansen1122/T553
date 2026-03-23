/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature implement
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include <stdio.h>
#include "msg_center.h"
#include "xiaodu_voice.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "wearable_log.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#endif
#include "common_def.h"
#include "player_sample_wrapper.h"
#include "soc_osal.h"
#include "watchdog.h"
#include "js_ability.h"
#include "recording_module.h"
#ifdef CONFIG_BRANDY_BLE_TRANS_OPTIMIZE
#include "bluetooth/BluetoothChangeBle.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TTS_AUDIO_PARA_LEN 100
#define TTS_MEDIA_MAX_PARAM_CNT 10
#define TTS_MEDIA_PARAM_OFFSET 2
#define TASK_PRIORITY_PCM_STREAM    (osPriority_t)(40)
#define PCM_STREAM_MSG_STACK_SIZE   0xC00

osMessageQueueId_t g_pcm_stream_msg_queue_id = nullptr;
static bool g_pcm_stream_end_record_sended_flag = false; /* 标识pcm的end帧是否发送完成，用于异常场景时资源清理 */


static errcode_t pcm_stream_task_init(void)
{
    osMessageQueueAttr_t m_attr = {0};
    m_attr.name = "PCM_STREAM_MSG";
    g_pcm_stream_msg_queue_id = osMessageQueueNew(PCM_STREAM_MSG_QUEUE_LEN, sizeof(pcm_stream_queue_msg), &m_attr);
    if (g_pcm_stream_msg_queue_id == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "create msg queue fail\r\n");
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "create msg queue succ\r\n");
    return ERRCODE_SUCC;
}

static void pcm_stream_del_queue(void)
{
    osStatus_t ret;

    ret = osMessageQueueDelete(g_pcm_stream_msg_queue_id);
    if (ret != osOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "del pcm_stream queue fail, ret = %u\r\n", ret);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "del pcm_stream queue succ.\r\n");
    return;
}

static errcode_t send_pcm_stream_data(pcm_stream_queue_msg msg)
{
    uint8_t retry_cnt = 100;
    uint8_t msg_send_delay = 10;
    errcode_t res;

    while (retry_cnt--) {
        if (msg.msg_id == MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM) {
            res = msg_center_xiaodu_send_pcm_stream(MSGCENTER_CMD_XIAODU, MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM,
                msg.data, msg.len);
        } else if (msg.msg_id == MSGCENTER_TYPE_ID_XIAODU_END_RECORD) {
            res = msg_center_xiaodu_send_end_record(MSGCENTER_CMD_XIAODU, MSGCENTER_TYPE_ID_XIAODU_END_RECORD, msg.data,
                msg.len);
#ifdef CONFIG_BRANDY_BLE_TRANS_OPTIMIZE
            ChangeBleConnectToDefault();
#endif
        }

        if (res != ERRCODE_SUCC) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "send stream fail, retry_cnt: %d, type = %d\n", retry_cnt,
                          msg.msg_id);
            osDelay(msg_send_delay);
        } else {
            break;
        }
    }
    free(msg.data);
    msg.data = nullptr;
    if (res != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "after retry, send stream still fail, retry_cnt: %d, type = %d\n",
                      retry_cnt, msg.msg_id);
        pcm_stream_del_queue();
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t pcm_stream_task_body(void *data)
{
    UNUSED(data);
    pcm_stream_queue_msg msg;
    errcode_t res;
    uint32_t msg_size = sizeof(pcm_stream_queue_msg);

    if (memset_s(&msg, msg_size, 0, msg_size) != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "pcm stream message memset fail\r\n");
        return ERRCODE_FAIL;
    }

    g_pcm_stream_end_record_sended_flag = false;
#ifdef CONFIG_BRANDY_BLE_TRANS_OPTIMIZE
    ChangeBleConnectToMax();
#endif

    for (;;) {
        osStatus_t ret = osMessageQueueGet(g_pcm_stream_msg_queue_id, &msg, nullptr, osWaitForever);
        if (ret != osOK) {
            continue;
        }

        /* pcm帧、end帧合并处理 */
        res = send_pcm_stream_data(msg);
        if (res == ERRCODE_FAIL || (res == ERRCODE_SUCC && msg.msg_id == MSGCENTER_TYPE_ID_XIAODU_END_RECORD)) {
            return res;
        }
    }
}

/*
 异常场景时清除audio资源、pcm队列资源
 可能的异常场景包括：
 1）录音过程中，JS直接退出；
 2）录音完成后，等待APP结果未返回时，JS退出；
 */
void msg_center_xiaodu_audio_pcm_clear_res(void)
{
    OHOS::ACELite::AudioCapturerModule::ExceptionStop();
    if (g_pcm_stream_end_record_sended_flag == false) {
        pcm_stream_queue_msg msg;
        msg.msg_id = MSGCENTER_TYPE_ID_XIAODU_END_RECORD;
        msg.data = 0;
        msg.len = 1;
        if (g_pcm_stream_msg_queue_id != nullptr) {
            osStatus_t ret = osMessageQueuePut(g_pcm_stream_msg_queue_id, &msg, 0, 0);
            if (ret != osOK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "Fatal err, ret = %d\r\n", ret);
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "active send pcm end\r\n");
        }
    }
    return;
}

osMessageQueueId_t GetPcmMsgQueueId(void)
{
    return g_pcm_stream_msg_queue_id;
}

errcode_t pcm_stream_create_task(void)
{
    errcode_t ret = ERRCODE_SUCC;

    ret = pcm_stream_task_init();
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    osThreadAttr_t attr = {"pcm_stream", 0, nullptr, 0, nullptr, PCM_STREAM_MSG_STACK_SIZE, TASK_PRIORITY_PCM_STREAM, 0,
         0 };
    if (osThreadNew((osThreadFunc_t)pcm_stream_task_body, nullptr, &attr) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "create pcm_stream task fail!\r\n");
        pcm_stream_del_queue();
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "create pcm_stream task succ!\r\n");
    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t ack = 1;
    errcode_t ret = ERRCODE_SUCC;

#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    display_api->set_screen_set_keepon_timeout(30000); // 30000ms
#endif

    /* ack */
    ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
#ifdef JS_ENABLE
    if(tlv_payload[0] == 1) {
        StartJsApp("com.vendor.Aidialogue");
    } else {
        StartJsApp("com.vendor.Aidial");
    }

#endif
    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_recv_xiaodu_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_xiaodu_recv_xiaodu_info: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv xiaodu info--> %s\r\n", data);
    DmsLiteSendMsgToJS(data);
    free(data);
    data = nullptr;
    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_recv_wenxin_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_xiaodu_recv_wenxin_info: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(data, payload_len, tlv_payload, payload_len);
    data[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv wenxin info--> %s\r\n", data);
    DmsLiteSendMsgToJS(data);
    free(data);
    data = nullptr;
    return ERRCODE_SUCC;
}

static int32_t convert_para_to_player_para(const uint8_t *inParam, uint16_t paramLen, int32_t *outArgc,
    uint8_t **outArgv)
{
    int ret;
    char *pos;
    uint8_t *strTemp;
    uint32_t paramCnt = 0;

    strTemp = (uint8_t*)osal_kmalloc(paramLen, OSAL_GFP_KERNEL);
    if (strTemp == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    ret = memset_s(strTemp, paramLen, 0, paramLen);
    if (ret != EOK) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} memset_s failed.\r\n", __FUNCTION__, __LINE__);
        osal_kfree(strTemp);
        return ERRCODE_FAIL;
    }

    ret = memcpy_s(strTemp, paramLen, inParam, paramLen);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        osal_kfree(strTemp);
        return ERRCODE_FAIL;
    }

    pos = strtok((char*)strTemp, ",");
    for (uint32_t i = 0; pos != nullptr; i++) {
        outArgv[i] = (uint8_t*)osal_kmalloc(strlen(pos) + 1, OSAL_GFP_KERNEL);
        if (outArgv[i] == nullptr) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
            osal_kfree(strTemp);
            return ERRCODE_FAIL;
        }
        ret = memcpy_s(outArgv[i], strlen(pos) + 1, pos, strlen(pos) + 1);
        if (ret != EOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
            osal_kfree(strTemp);
            return ERRCODE_FAIL;
        }
        paramCnt++;
        if (*outArgc >= TTS_MEDIA_MAX_PARAM_CNT) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} invalid argc: %d(max: 64).\r\n",
                __FUNCTION__, __LINE__, *outArgc);
            osal_kfree(strTemp);
            return ERRCODE_FAIL;
        }
        pos = strtok(nullptr, ",");
    }

    *outArgc = paramCnt;
    osal_kfree(strTemp);
    return ERRCODE_SUCC;
}

static void msg_center_tts_player_start(const uint8_t *data, uint32_t dataLen)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[TTS_MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = convert_para_to_player_para(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    ret = PlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != nullptr) {
            osal_kfree(argv[i]);
            argv[i]  = nullptr;
        }
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__,
        (ret == ERRCODE_SUCC) ? "success" : "failure");
    return;
}

errcode_t msg_center_xiaodu_recv_tts(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
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

    /* 播放tts语音 */
    char *tts_file = (char *)malloc(payload_len + 1);
    if (tts_file == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_xiaodu_recv_tts: malloc failed.\r\n");
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(tts_file, payload_len, tlv_payload, payload_len);
    tts_file[payload_len] = '\0';
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "recv_tts file: %s\r\n", tts_file);

    char buffer[TTS_AUDIO_PARA_LEN] = {0};
    (void)sprintf_s(buffer, TTS_AUDIO_PARA_LEN, "%s,%s,%d,%d", "xx", tts_file, 1, 0);
    msg_center_tts_player_start((const uint8_t *)buffer, strlen(buffer));

    /* 播放完成后需删除文件 */
    if (unlink(tts_file) < 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "del tts_file fail.\r\n");
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "del tts_file succ.\r\n");
    }

    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_start_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t start_flag = 1;
    ret = msg_center_send_data(cmd_id, type, &start_flag, sizeof(start_flag));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}


errcode_t msg_center_xiaodu_send_pcm_stream(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_send_end_record(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send pcm stream end, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len); /* usr_data参考协议msg_center_pcm_end_flag_t */
    pcm_stream_del_queue();
    g_pcm_stream_end_record_sended_flag = true;
    return ERRCODE_SUCC;
}

errcode_t msg_center_xiaodu_send_stop_answer(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    errcode_t ret = ERRCODE_SUCC;

    ret = msg_center_send_data(cmd_id, type, usr_data, usr_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "send js exit msg, usr_data = %d, usr_len = %d\r\n",
                  *(uint8_t *)usr_data, usr_len); /* usr_data: 0 --normal exit, 1 -- abnormal exit */
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
}
#endif
