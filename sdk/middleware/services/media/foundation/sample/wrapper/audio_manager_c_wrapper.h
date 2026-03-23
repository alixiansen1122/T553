/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: audio manager c wrapper header
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#ifndef AUDIO_MANAGER_C_WRAPPER_H
#define AUDIO_MANAGER_C_WRAPPER_H

#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct {
    AudioCodecFormat audioFormat;
    uint32_t sampleRate;
    uint32_t channelCount;
} AudioFormatConfig;

bool AudioManagerInit(void);

AudioSession AudioManagerMakeSessionId(void);

int32_t AudioManagerSetDeviceConnectionState(AudioDeviceType deviceType, AudioDeviceConnectState connectState);

int32_t AudioManagerActivateInterrupt(AudioStreamType streamType, AudioSession sessionId);

int32_t AudioManagerDeactivateInterrupt(void);

int32_t AudioManagerSetVolume(AudioStreamType streamType, int32_t volume);

int32_t AudioManagerGetVolume(AudioStreamType streamType);

int32_t AudioManagerSetSeaEnable(bool enable);

int32_t AudioStreamInInit(CapturerInputConfig captureConfig);

int32_t AudioStreamInStart(void);

int32_t AudioStreamInObtainBuffer(uint8_t *data, uint32_t *size);

int32_t AudioStreamInGetPosition(int64_t *pos);

int32_t AudioStreamInSetBitRate(uint32_t bitrate);

int32_t AudioStreamInStop(void);

int32_t AudioStreamInDeinit(void);

int32_t AudioStreamOutInit(AudioRendererConfig renderConfig);

int32_t AudioStreamOutSetMute(bool mute);

int32_t AudioStreamOutPlay(void);

int32_t AudioStreamOutStreamWrite(const uint8_t *data, uint32_t size);

int32_t AudioStreamOutStop(void);

int32_t AudioStreamOutDeinit(void);

/**
 * @brief Sets audio effect algorithm parameters.
 *
 * @param effectType Audio effect algorithm type, For details,
    see the {uapi_aef_type} enumeration in the {soc_uapi_aef.h} file.
 * @param param Indicates Algorithm parameter pointer.
 * @param length Indicates Algorithm parameter length.
 * @return Returns {0} If the audio effect algorithm parameters are set successfully; returns {-1}
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerA2dpSinkSetAefParam(int32_t effectType, const char *param, uint32_t length);

/**
 * @brief Sets the audio effect algorithm switch of the A2DP sink channel.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 * @param effectType Audio effect algorithm type, For details,
    see the {uapi_aef_type} enumeration in the {soc_uapi_aef.h} file.
 * @param enable
 * @return Returns {0} If the audio effect is set successfully; returns {-1}
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerA2dpSinkSetAefEnable(int32_t effectType, bool enable);

int32_t AudioManagerSetSeaParam(const uint8_t *seaParam, uint16_t len);

int32_t AudioManagerGetSeaParam(uint8_t *seaParam, uint16_t len);

int32_t AudioManagerSetAefParam(int32_t effectType, const uint8_t *aefParam, uint16_t len);

int32_t AudioManagerGetAefParam(int32_t effectType, uint8_t *aefParam, uint16_t len);

int32_t AudioManagerA2dpSinkSetParam(AudioFormatConfig config);

int32_t AudioManagerA2dpSinkCreate(void);

int32_t AudioManagerA2dpSinkDestroy(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MANAGER_C_WRAPPER_H */
