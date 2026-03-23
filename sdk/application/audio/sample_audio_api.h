/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#ifndef _SAMPLE_AUDIO_API_H_
#define _SAMPLE_AUDIO_API_H_

#include "soc_uapi_adec.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_sea.h"
#include "chip_type.h"
#include "circ_buf.h"
#include "cmsis_os.h"
#include "audio_debug.h"
#include "soc_osal.h"
#include "tcxo.h"

#ifdef BRANDY_PROT
#define sap_printf
#elif (SAP_CHIP_TYPE == socmn1)
#include "ha_usart.h"
#define sap_printf UART_PRINTF
#else
#define sap_printf printf
#endif /* SAP_CHIP_TYPE */

#define EXT_SUCCESS 0
#define EXT_FAILURE (-1)

#define sap_msleep(ms) osDelay(ms) /* osDelay(1) : 1ms */
#define sap_udelay uapi_tcxo_delay_us
#define sap_mdelay uapi_tcxo_delay_ms

#define THREAD_SLEEP_5MS 5
#define THREAD_SLEEP_10MS 10
#define THREAD_HEART_BEAT_DUR 100
#define FILE_READ_LEN 0x400 /* 1K */
#define FILE_READ_LEN_MIN 0x80 /* 128B */

#define FILE_PATH_LEN 256
#define FILE_STREAM_PATH "/user/stream"

#if (SAP_CHIP_TYPE == brandy)
#define PSRAM_BRANDY_BASE_ADDR              0x6c001000 /* brandy/common_config/psram.h */
#define PSRAM_AUDIO_OFFSET                  0x200000 /* brandy/common_config/psram.h */

#define PSRAM_AUDIO_BASE_ADDR               (PSRAM_BRANDY_BASE_ADDR + PSRAM_AUDIO_OFFSET)
#define PSRAM_ADUIO_LENGTH                  0x533380 /* brandy/common_config/psram.h */

#define FPGA_SAVE_DATA_ADDR PSRAM_AUDIO_BASE_ADDR /* 0x6c201000 */
#define FPGA_SAVE_DATA_SIZE 0x100000 /* 1M */

#define FPGA_READ_DATA_ADDR (FPGA_SAVE_DATA_ADDR + FPGA_SAVE_DATA_SIZE) /* 0x6c301000 */
#define FPGA_READ_DATA_SIZE 0x100000 /* 1M */

#define FPGA_READ_PCM_DATA_ADDR (FPGA_READ_DATA_ADDR + FPGA_READ_DATA_SIZE) /* 0x6c401000 */
#define FPGA_READ_PCM_DATA_SIZE 0x100000 /* 1M */

#define FPGA_DUMP_DATA_ADDR (FPGA_READ_PCM_DATA_ADDR + FPGA_READ_PCM_DATA_SIZE) /* 0x6c501000 */
#define FPGA_DUMP_DATA_SIZE \
    (PSRAM_ADUIO_LENGTH - (FPGA_SAVE_DATA_SIZE + FPGA_READ_DATA_SIZE + FPGA_READ_PCM_DATA_SIZE)) /* 0x233380: 2M+ */

#define FPGA_FLASH_ES_DATA_MP3_ADDR FPGA_READ_DATA_ADDR
#define FPGA_FLASH_ES_DATA_SBC_ADDR FPGA_READ_DATA_ADDR
#define FPGA_FLASH_PCM_DATA_ADDR FPGA_READ_PCM_DATA_ADDR

#define FPGA_FLASH_ES_SIZE FPGA_READ_DATA_SIZE
#define FPGA_FLASH_PCM_SIZE FPGA_READ_PCM_DATA_SIZE

#else /* socmn1 */
#define FPGA_SAVE_DATA_ADDR 0x5818c000
#define FPGA_SAVE_DATA_SIZE 0x40000

#define FPGA_FLASH_OFFSET_BASEADDR 0x58122000
#define FPGA_FLASH_ES_DATA_MP3_ADDR FPGA_FLASH_OFFSET_BASEADDR
#define FPGA_FLASH_ES_DATA_SBC_ADDR FPGA_FLASH_OFFSET_BASEADDR
#define FPGA_FLASH_PCM_DATA_ADDR FPGA_FLASH_OFFSET_BASEADDR

#define FPGA_FLASH_ES_SIZE 0x100000
#define FPGA_FLASH_PCM_SIZE 0x80000

#define FPGA_DUMP_DATA_ADDR 0x58122000
#define FPGA_DUMP_DATA_SIZE 0x80000

#endif

#define BITS_NUM_PER_BYTE 8
#define HZ_NUM_PER_KHZ 1000

#define AI_VOLUME_DEFAULT 30
#define BT_A2DP_MTU_SIZE 1005
#define BT_SCO_FRAME_SIZE 57

#if (SAP_CHIP_TYPE == brandy)
#define SND_OUT_PORT_DEFAULT UAPI_SND_OUT_PORT_I2S2 /* Out port for SmartPA SPK output */
#else
#define SND_OUT_PORT_DEFAULT UAPI_SND_OUT_PORT_DAC0
#endif

/* sound local output default pcm format */
#define SND_OUT_SAMPLE_RATE_DEF     UAPI_AUDIO_SAMPLE_RATE_48K
#define SND_OUT_BIT_DEPTH_DEF       UAPI_AUDIO_BIT_DEPTH_16
#define SND_OUT_CHANNEL_DEF         UAPI_AUDIO_CHANNEL_2
#define SND_OUT_I2S_MST_DEF         TD_TRUE /* TD_TRUE:master;TD_FALSE:slave */
#define SND_OUT_I2S_DATA_EDGE_DEF   TD_TRUE /* TD_TRUE:Rising;TD_FALSE:Falling */
#define SND_OUT_FRAME_MODE_WORD_LEN UAPI_AUDIO_BIT_DEPTH_16 /* 16:for pcm16/i2s32, 32:for pcm32/i2s64 */

/* I2S for CAT1 default i2s attr */
#define SIO_CAT1_I2S_SAMPLE_RATE_DEF UAPI_AUDIO_SAMPLE_RATE_16K
#define SIO_CAT1_I2S_CHANNEL_DEF     UAPI_AUDIO_CHANNEL_2
#define SIO_CAT1_I2S_PCM_DELAY_DEF   UAPI_AUDIO_I2S_PCM_0_DELAY
#define SIO_CAT1_I2S_MST_DEF         TD_TRUE /* TD_TRUE:master;TD_FALSE:slave */
#define SIO_CAT1_I2S_DATA_EDGE_DEF   TD_TRUE /* TD_TRUE:Rising;TD_FALSE:Falling */
#define SIO_CAT1_FRAME_MODE_WORD_LEN UAPI_AUDIO_BIT_DEPTH_16 /* 16:for pcm16/i2s32, 32:for pcm32/i2s64 */

#define sap_min(a, b) ((a) < (b) ? (a) : (b))
#define sap_max(a, b) ((a) > (b) ? (a) : (b))

#define sap_unused(var) \
    do { \
        (void)(var); \
    } while (0)

#define clear_obj(obj) ((*(obj)) = (__typeof__(*(obj))) { 0 })

typedef enum {
    GUI_AEF_AO,
    GUI_AEF_SEA,
    GUI_AEF_HAID,
    GUI_AEF_MAX,
} gui_aef_type;

typedef struct {
    gui_aef_type type;
    td_handle handle;
} gui_aef_str;

typedef struct {
    td_u8 *buf;
    td_u32 size;
    td_u32 read;
    td_u32 write;
    circ_buf cb;
} data_info;

static inline td_void data_info_init(data_info *d, td_u32 buf, td_u32 size)
{
    d->buf = (td_u8 *)(td_uintptr_t)buf;
    d->size = size;
    d->read = 0;
    d->write = 0;
    circ_buf_init(&d->cb, &d->write, &d->read, d->buf, d->size);
}

typedef struct {
    const td_char *file;
    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;
} sample_acodec_arg;

typedef struct {
    td_handle *player;               /* cast_player input adp */
    const sample_acodec_arg *in_arg; /* cast player input es attr */
    td_handle *snd;                  /* cast_player output track */
    const uapi_snd_attr *snd_attr; /* cast player output pcm attr */
    uapi_snd_aef_profile aef_profile;
} sample_escast_arg;

td_s32 sample_ao(td_s32 argc, td_char *argv[]);
td_s32 sample_ai(td_s32 argc, td_char *argv[]);
td_s32 sample_encode(td_s32 argc, td_char *argv[]);
td_s32 sample_decode(td_s32 argc, td_char *argv[]);

td_s32 sample_esplay_open(const sample_acodec_arg *acodec_arg, const td_handle *player,
                          uapi_snd_aef_profile aef_profile);
td_void sample_esplay_close(td_void);

td_s32 sample_escast_open(sample_escast_arg *escast_arg);
td_void sample_escast_close(td_void);

td_s32 sample_ao_player_open(sample_escast_arg *escast_arg, uapi_snd snd_id);
td_void sample_ao_player_close(td_void);

td_s32 sample_cast_aenc_play(td_s32 argc, td_char *argv[]);
td_s32 sample_encode_play(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_aenc(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_sea_aenc(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_ao(td_s32 argc, td_char *argv[]);
td_s32 sample_sea(td_s32 argc, td_char *argv[]);
td_s32 sample_gui_aef(td_s32 argc, td_char *argv[]);
#if defined(SAP_HAID_SUPPORT)
td_s32 sample_haid(td_s32 argc, td_char *argv[]);
td_void sample_gui_haid_register_param(td_void *params);
td_s32 sample_spk_calib(td_s32 argc, td_char *argv[]);
#endif
#if defined(SAP_DPM_SUPPORT)
td_s32 sample_dpm(td_s32 argc, td_char *argv[]);
#endif /* SAP_DPM_SUPPORT */
td_s32 sample_lpwk(td_s32 argc, td_char *argv[]);

td_s32 sample_phone_application(td_s32 argc, td_char *argv[]);
td_s32 sample_phone_protocol(td_s32 argc, td_char *argv[]);

td_s32 sample_dump(td_s32 argc, td_char *argv[]);

td_s32 sample_proc(td_s32 argc, td_char *argv[]);

td_u32 sample_get_vector_size(td_void);

td_void sample_gui_aef_add_handle(td_u8 type, td_handle handle);
td_void sample_gui_aef_delete_handle(td_handle handle);

const td_u16 *sample_get_vector_16k_1ch_16bit(td_void);

#endif
