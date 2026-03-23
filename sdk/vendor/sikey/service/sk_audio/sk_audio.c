#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sk_audio.h"
#include "player_sample_wrapper.h"
#include "audio_capture_wrapper.h"
#include "soc_osal.h"
#include "errcode.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_list.h"
#include "http_api.h"


#define MAX_AUDIO_CMD_QUEUE_NUM 10
#define AUDIO_RECORD_FILE_PATH "/user/dean_record.ogg"

#define MAX_VOLUME "100"
#define MID_VOLUME "80"
#define MIN_VOLUME "60"

#define AUDIO_PLAY_END_PERIOD 700

osal_task *sk_audio_task_id;
unsigned long audio_cmd_queue_id;

osal_timer audio_play_timer={0};

osal_timer audio_record_timer={0};

uint32_t audio_record_time=1000;

void audio_play_end_timer_handler(void)
{
    // const char* filePath = "/user/dean_record.ogg";
    printf("{%s():%d} audio_play_end_timer_handler in\r\n", __FUNCTION__, __LINE__);
    //启动录音操作
    if(audio_record_time>0)
    {
        sk_audio_start_record(AUDIO_RECORD_FILE_PATH,audio_record_time);
    }
}


void audio_record_end_timer_handler(void)
{
    printf("{%s():%d} audio_record_end_timer_handler in\r\n", __FUNCTION__, __LINE__);
    sk_audio_stop_record();
}

int32_t sk_audio_record_stop(void)
{
    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"stop";
    printf("{%s():%d} sk_audio_record_stop in\r\n", __FUNCTION__, __LINE__);
    AudioCaptureSample(argc, (const char **)&argv);
}

int32_t sk_audio_record_sample(const char* filePath)
{
    if (filePath == NULL) {
        wstp_print("{%s():%d} Error: filePath is NULL.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    // 固定参数数量：原argv[0]到argv[4] 加上动态的 argv[5] (filePath)
    int32_t argc = 6;
    const char* argv[6];
    int32_t ret = ERRCODE_FAIL;

    // 设置固定参数值
    argv[0] = "xx";      // 固定参数，原argv[0]
    argv[1] = "opus";     // 固定参数，原argv[1]，音频格式
    argv[2] = "16000";   // 固定参数，原argv[2]，采样率
    argv[3] = "1";       // 固定参数，原argv[3]，声道数
    argv[4] = "16";      // 固定参数，原argv[4]，位深

    // 为文件路径参数分配内存并复制
    argv[5] = (char*)malloc(strlen(filePath) + 1);
    if (argv[5] == NULL) {
        wstp_print("{%s():%d} Memory allocation failed for file path.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
    strcpy((char*)argv[5], filePath);

    // 打印所有参数值以供调试
    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s.\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    // 调用录音采样函数
    ret = AudioCaptureSample(argc, argv);

    // 释放为文件路径分配的内存
    if (argv[5] != NULL) {
        free((void*)argv[5]);
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

int32_t sk_audio_start_record(const char* filePath, uint32_t record_time)
{
    if(record_time<=0)
    {
        printf("record_time %d is invalid.\n", record_time);
        return ERRCODE_FAIL;
    }
    FILE *file = fopen(filePath, "r");
    if (file != NULL)
    {
        fclose(file); // 先关闭文件
        if (remove(filePath) == 0) {
            printf("delete file %s success.\n", filePath);
        } else {
            printf("delete file %s fail.", filePath);
        }
    }
    else
    {
        printf("file %s not exist.\n", filePath);
    }
    sk_audio_cmd_msg_t audio_cmd_msg = {SK_AUDIO_CMD_RECORD, NULL};
    audio_cmd_msg.data = (uint8_t *)filePath;
    uint32_t cmd_len = sizeof(sk_audio_cmd_msg_t);
    if (osal_msg_queue_write_copy(audio_cmd_queue_id, (void *)&audio_cmd_msg,
                                            cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("{%s():%d} sk_audio_record in sucess! filePath:%s record_time:%d.\r\n", __FUNCTION__, __LINE__,filePath,record_time);
        audio_record_time=record_time;
        //开始录音计时
        osal_timer_mod(&audio_record_timer,record_time);
        return ERRCODE_SUCC;
    }
    else
    {
        printf("{%s():%d} sk_audio_record in fail! filePath:%s record_time:%d.\r\n", __FUNCTION__, __LINE__,filePath,record_time);
        return ERRCODE_FAIL;
    }
}

int32_t sk_audio_stop_record()
{
    sk_audio_cmd_msg_t audio_cmd_msg = {SK_AUDIO_CMD_RECORD_STOP, NULL};
    uint32_t cmd_len = sizeof(sk_audio_cmd_msg_t);
    if (osal_msg_queue_write_copy(audio_cmd_queue_id, (void *)&audio_cmd_msg,
                                            cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("{%s():%d} sk_audio_record_stop in sucess!\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_SUCC;
    }
    else
    {
        printf("{%s():%d} sk_audio_record_stop in fail!\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
}

int32_t sk_audio_player_stop(void)
{

    printf("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"stop";
    int32_t ret = ERRCODE_FAIL;
    // 调用播放器样例函数
    ret = PlayerSample(argc, (const char **)&argv);

    printf("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

int32_t sk_audio_player_sample(const char* filePath)
{
    if (filePath == NULL) { // 检查文件路径有效性
        printf("{%s():%d} Error: filePath is NULL.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    printf("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    // 固定参数数量：原argv[0], argv[2], argv[3] 加上动态的 argv[1] (filePath)
    int32_t argc = 4; 
    const char* argv[4]; // 构建参数数组
    int32_t ret = ERRCODE_FAIL;

    // 设置固定参数值
    argv[0] = "xx";    // 固定参数，原argv[0]
    argv[2] = "1";     // 固定参数，原argv[2]
    argv[3] = "0";     // 固定参数，原argv[3]

    // 为文件路径参数分配内存并复制
    argv[1] = (char*)malloc(strlen(filePath) + 1); // 加1用于字符串结束符'\0'
    if (argv[1] == NULL) {
        printf("{%s():%d} Memory allocation failed for file path.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
    strcpy((char*)argv[1], filePath); // 复制文件路径字符串

    // 打印所有参数值以供调试
    for (int32_t i = 0; i < argc; i++) {
        printf("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    // 调用播放器样例函数
    ret = PlayerSample(argc, argv);

    // 释放为文件路径分配的内存
    if (argv[1] != NULL) {
        free((void*)argv[1]);
    }

    printf("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

int32_t sk_audio_set_volume(uint32_t volme)
{
    if (volme > 3 || volme < 1) {
        printf("{%s():%d} Error: volme is %d.\r\n", __FUNCTION__, __LINE__,volme);
        return ERRCODE_FAIL;
    }

    printf("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    // 固定参数数量：原argv[0], argv[2], argv[3] 加上动态的 argv[1] (filePath)
    int32_t argc = 2; 
    const char* argv[2]; // 构建参数数组
    int32_t ret = ERRCODE_FAIL;

    // 设置固定参数值
    argv[0] = "setvolume"; 

    switch (volme)
    {
    case 1:
        argv[1] = MIN_VOLUME;
        break;
    case 2:
        argv[1] = MID_VOLUME;
        break;
    case 3:
        argv[1] = MAX_VOLUME;
        break;
    default:
        break;
    }

    // 打印所有参数值以供调试
    for (int32_t i = 0; i < argc; i++) {
        printf("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    // 调用播放器样例函数
    ret = PlayerSample(argc, argv);

    printf("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

int32_t sk_audio_player(const char* filePath,uint32_t play_time,uint32_t record_time, uint32_t volme)
{
    sk_audio_cmd_msg_t audio_cmd_msg = {SK_AUDIO_CMD_PLAY, NULL};
    audio_cmd_msg.data = (uint8_t *)filePath;
    uint32_t cmd_len = sizeof(sk_audio_cmd_msg_t);

    sk_audio_stop();
    osDelay(1000);
    sk_audio_set_volume(volme);

    if (osal_msg_queue_write_copy(audio_cmd_queue_id, (void *)&audio_cmd_msg,
                                            cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("{%s():%d} sk_audio_player in sucess! filePath:%s play_time:%d record_time:%d.\r\n", __FUNCTION__, __LINE__,filePath,play_time,record_time);
        audio_record_time=record_time;
        //开始播放计时
        if(play_time>0)
        {
            osal_timer_mod(&audio_play_timer,play_time+AUDIO_PLAY_END_PERIOD);
        }
        return ERRCODE_SUCC;
    }
    else
    {
        printf("{%s():%d} sk_audio_player in fail! filePath:%s play_time:%d record_time:%d.\r\n", __FUNCTION__, __LINE__,filePath,play_time,record_time);
        return ERRCODE_FAIL;
    }
}

int32_t sk_audio_stop(void)
{
    sk_audio_cmd_msg_t audio_cmd_msg = {SK_AUDIO_CMD_STOP, NULL};
    uint32_t cmd_len = sizeof(sk_audio_cmd_msg_t);

    if (osal_msg_queue_write_copy(audio_cmd_queue_id, (void *)&audio_cmd_msg,
                                            cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
        printf("{%s():%d} sk_audio_player stop sucess!\r\n", __FUNCTION__, __LINE__);
        osal_timer_stop(&audio_play_timer);
        return ERRCODE_SUCC;
    }
    else
    {
        printf("{%s():%d} sk_audio_player stop fail!\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
}

static int  sk_audio_task_entry(void *data)
{
	unused(data);
    sk_audio_cmd_msg_t audio_cmd_msg = {0};
    unsigned int cmd_len = sizeof(sk_audio_cmd_msg_t);
    while(1)
    {
        if (osal_msg_queue_read_copy(audio_cmd_queue_id, (void *)&audio_cmd_msg,
                                            &cmd_len, OSAL_MSGQ_WAIT_FOREVER) == OSAL_SUCCESS) {
            switch(audio_cmd_msg.cmd)
            {
                case SK_AUDIO_CMD_PLAY:
                    if (audio_cmd_msg.data != NULL)
                    {
                        sk_audio_player_sample((const char *)audio_cmd_msg.data);
                        if (audio_cmd_msg.data)
                            free(audio_cmd_msg.data);
                    }
                    break;
                case SK_AUDIO_CMD_STOP:
                    sk_audio_player_stop();
                    break;
                case SK_AUDIO_CMD_PAUSE:
                    break;
                case SK_AUDIO_CMD_RESUME:
                    break;
                case SK_AUDIO_CMD_VOLUME:
                    break;
                case SK_AUDIO_CMD_RECORD:
                    if(audio_cmd_msg.data!=NULL)
                    {
                        sk_audio_record_sample((const char *)audio_cmd_msg.data);
                    }
                    break;
                case SK_AUDIO_CMD_RECORD_STOP:
                    sk_audio_record_stop();
                    //启动websocket或者http post动作
                    http_upload_audio(AUDIO_RECORD_FILE_PATH);
                    break;
                default:
                    break;
            }
        }
    }
}

int sk_audio_task_init(void)
{
    int32_t ret = 0;
    const char audio_cmd_queue_name[] = "msgqueue_sk_audio";
    //播放完成计时
	audio_play_timer.handler = audio_play_end_timer_handler;
    audio_play_timer.interval = 1000*30;
	ret=osal_timer_init(&audio_play_timer);
    if(ret!=OSAL_SUCCESS)
    {
        printf("osal_timer_init failed\n");
    }

    //录音完成计时
    audio_record_timer.handler = audio_record_end_timer_handler;
    audio_record_timer.interval = audio_record_time;
	ret=osal_timer_init(&audio_record_timer);
    if(ret!=OSAL_SUCCESS)
    {
        printf("osal_timer_init failed\n");
    }

	osThreadAttr_t threadAttr={0};
	memset(&threadAttr, 0, sizeof(threadAttr));
    threadAttr.name = "sk_audio_task";
    threadAttr.stack_size = 0x2000;
    threadAttr.priority = 17;
    threadAttr.stack_mem = memalign(16, threadAttr.stack_size);

    sk_audio_task_id = osThreadNew(sk_audio_task_entry, NULL, &threadAttr);
    if ( sk_audio_task_id ==  NULL) {
        printf("osal_kthread_create sk_audio_task_id failed\r\n");
        ret = ERRCODE_FAIL;
    }

    ret = osal_msg_queue_create(audio_cmd_queue_name, MAX_AUDIO_CMD_QUEUE_NUM,
                                &audio_cmd_queue_id,
                                0, sizeof(sk_audio_cmd_msg_t));
    if (ret != ERRCODE_SUCC) {
        printf("Create msgqueue %s failed: %d\n", audio_cmd_queue_name, ret);
        ret = ERRCODE_FAIL;
    }
    else
    {
        printf("Create msgqueue %s, queue_id: %d, sucess: %d\n", audio_cmd_queue_name, audio_cmd_queue_id, ret);
    }

	return 0;
}
