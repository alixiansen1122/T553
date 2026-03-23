/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio plugin common header
* Author: Media Software Group
* Create: 2021-05-18
*/

#ifndef AUDIO_PORT_COMMON_H
#define AUDIO_PORT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_ai.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sea.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_acodec.h"
#endif

#include "audio_types.h"

#include "audio_port_plugin.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    AUDIO_MOD_AI = 0,
    AUDIO_MOD_ADP,
    AUDIO_MOD_ADEC,
    AUDIO_MOD_AENC,
    AUDIO_MOD_SEA,
    AUDIO_MOD_SOUND
} AudioSDKModuleType;

#ifdef AUDIO_SDK_SUPPORT
typedef struct {
    enum AudioFormat audioFormat;
    uapi_acodec_id codecId;
} AudioFormatMapping;
#endif

#define INVALID_ID (-1)

// convert sample rate to number of samples per frame
// need fix: AI set this parameter based on the audio encoding protocol
#define SAMPLERATE_TO_PTNUMPERFRM 100

#define VOLUME_MIN 0.0f
#define VOLUME_MAX 1.0f
#define MAX_KEY_STR_SIZE 30
#define MAX_KEY_VALUE_SIZE 512

#define GET_AUDIO_POSITION_INTERVAL_US 10000
#define GET_AUDIO_CACHED_DATA_TIMEOUT_MS 1000

void VolumePercentToDb(float volume, int32_t *db);

bool IsRawFormat(enum AudioFormat audioFormat);

#ifdef AUDIO_SDK_SUPPORT

bool AudioConvertAudioFormatToCodecId(enum AudioFormat audioFormat, uapi_acodec_id *codecId);

int32_t AudioSDKInit(void);

int32_t AudioSDKDeInit(void);

int32_t AudioSDKAdpOpen(td_handle *adpHandle);

int32_t AudioSDKAdpClose(td_handle adpHandle);

int32_t AudioSDKAdecOpen(td_handle *adecHandle, struct PortPluginAttr inputAttr);

int32_t AudioSDKAdecClose(td_handle adecHandle);

int32_t AudioSDKSeaOpen(td_handle *seaHandle, struct PortPluginAttr inputAttr);

int32_t AudioSDKSeaClose(td_handle seaHandle);

int32_t AudioSDKAencOpen(td_handle *aencHandle, uapi_aenc_attr aencAttr);

int32_t AudioSDKAencClose(td_handle aencHandle);

int32_t AudioSDKAttach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle);

int32_t AudioSDKDetach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle);

int32_t SaveToggleConfig(uapi_audio_toggle_config *toggleConfig, const char *value, uint32_t valueLen);

int32_t UpdateCurrentPosition(td_handle trackHandle, struct AudioTimeStamp *time);

int32_t GetParamByValue(uint8_t *param, uint32_t paramLen, const char *value,
    uint32_t valueLen, uint32_t configLen);

int32_t SetSeaEnable(td_handle seaHandle, const char *value, uint32_t valueLen);
#endif

int32_t ExtraParamsInvoke(AudioHandle portHandle, struct InvokeAttr *invokeAttr,
    struct AudioPortInvoke invokeList[], uint32_t listSize);
bool AudioPluginIsValidPin(enum AudioPortPin pin, const char *pluginName);

int32_t ParseKeyValueForExtra(const struct InvokeAttr *invokeAttr,
    char *key, int32_t keyLen, char *value, int32_t valueLen);

int32_t IsNeedSetExtraFlag(enum AudioFormat audioFormat, bool *IsSetExtraParam);

int32_t SetExtraData(struct PortPluginAttr *inputAttr, const char *value, uint32_t valueLen);

int32_t GetBuiltinAIAttr(uapi_ai_attr *aiAttr);

int32_t GetBuiltinAOAttr(uapi_snd_attr *sndAttr);

int32_t GetExternAIAttr(uapi_ai_attr *aiAttr);

int32_t GetExternAOAttr(uapi_snd_attr *sndAttr);

void SetAefExtraParam(td_handle soundHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
