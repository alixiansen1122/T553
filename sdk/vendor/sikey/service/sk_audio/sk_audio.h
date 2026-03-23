#ifndef __SK_AUDIO_H__
#define __SK_AUDIO_H__

#include <stdint.h>
typedef enum
{
    SK_AUDIO_CMD_PLAY = 0,
    SK_AUDIO_CMD_STOP,
    SK_AUDIO_CMD_PAUSE,
    SK_AUDIO_CMD_RESUME,
    SK_AUDIO_CMD_VOLUME,
    SK_AUDIO_CMD_RECORD,
    SK_AUDIO_CMD_RECORD_STOP,
}sk_audio_cmd_t;

typedef struct
{
    sk_audio_cmd_t cmd;
    uint8_t *data;
}sk_audio_cmd_msg_t;

int sk_audio_task_init(void);

int32_t sk_audio_start_record(const char* filePath, uint32_t record_time);
int32_t sk_audio_stop_record(void);

int32_t sk_audio_player(const char* filePath,uint32_t play_time,uint32_t record_time, uint32_t volme);

int32_t sk_audio_set_volume(uint32_t volme);

int32_t sk_audio_stop(void);
#endif