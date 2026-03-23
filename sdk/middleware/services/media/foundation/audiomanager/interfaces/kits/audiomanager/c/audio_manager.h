/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio manager define
* Author: Media Software Group
* Create: 2021-04-21
*/

/**
 * @addtogroup MultiMedia_AudioManager
 * @{
 *
 * @brief Defines the <b>AudioManager</b> class and provides functions for manages audio information.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_manager.h
 *
 * @brief Declares the <b>AudioManager</b> class for Manages audio information,
 * controls audio devices, adjusts the volume, and manages the audio focus.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#include "audio_base_type.h"
#include "audio_debug_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * Called back to notify an application when its audio is interrupted by another application.
 *
 * @param type Indicates the interruption type, which can be {@link AudioInterruptType#INTERRUPT_TYPE_BEGIN}
 * or {@link AudioInterruptType#INTERRUPT_TYPE_END}.
 * @param hint Indicates the interruption prompt, which can be {@link AudioInterruptHintType#INTERRUPT_HINT_PAUSE},
 * {@link AudioInterruptHintType#INTERRUPT_HINT_RESUME}, or {@link AudioInterruptHintType#INTERRUPT_HINT_STOP}.
 * @since 1
 */
typedef struct {
    void (*OnInterrupt)(int32_t type, int32_t hint, void *cookie);
    void *cookie;
} InterruptListener;

/**
 * @brief An audio interruption management mechanism
 * used to resolve audio playback conflicts among multiple applications.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    AudioStreamType streamType;
    AudioSession sessionID;
    InterruptListener *interruptListener;
} AudioInterrupt;

/**
 * @brief audio manager debug info.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /* 所有输入焦点类型 */
    InterruptDebugInfo *inputInterruptInfo;
    uint32_t interruptInfoLength;
    /* 所有输出焦点类型 */
    InterruptDebugInfo *outputInterruptInfo;
    uint32_t outputInterruptInfoLength;
    /* 所有流的信息 */
    StreamDebugInfo *allstreamInfo;
    uint32_t streamInfoLength;
    /* 所有设备的信息 */
    DeviceDebugInfo *allDeviceInfo;
    uint32_t deviceInfoLength;
} AudioManagerDebugInfo;

/**
 * @brief Provides functions for Managing audio information.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerInitialize(void);

/**
 * @brief Creates a session ID.
 *
 * @return Returns the session ID if created; returns {@code 0} otherwise.
 * @since 1.0
 * @version 1.0
 */
AudioSession AudioManagerMakeSessionId(void);

/**
 * @brief Sets the volume level of a specified type of streams.
 *
 * @param streamType Indicates the type of streams for which the volume level is to set.
 * @param volume     Indicates the volume level to set. You can obtain the maximum level value by calling
 *                   {@link #getMaxVolume(AudioStreamType)}, and the minimum by
 *                   {@link #getMinVolume(AudioStreamType)}.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @see #getMaxVolume(AudioStreamType)
 * @see #getMinVolume(AudioStreamType)
 * @see #getVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerSetVolume(AudioStreamType streamType, int32_t volume);

/**
 * @brief Obtains the volume level of a specified type of audio streams.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the volume level of the type of streams.
 * @see #getMaxVolume(AudioStreamType)
 * @see #getMinVolume(AudioStreamType)
 * @see #setVolume(AudioStreamType, int)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerGetVolume(AudioStreamType streamType);

/**
 * @brief Obtains the minimum volume level of a specified type of audio streams.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the minimum valid volume level for the type of streams.
 * @see #getVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerGetMinVolume(AudioStreamType streamType);

/**
 * @brief Obtains the maximum volume level of a specified type of audio streams.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the maximum valid volume level for the type of streams.
 * @see #getVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerGetMaxVolume(AudioStreamType streamType);

/**
 * @brief Mutes a specified type of streams.
 *
 * @param streamType Indicates the type of streams to mute.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerMute(AudioStreamType streamType);

/**
 * @brief Unmutes a specified type of streams.
 *
 * @param streamType Indicates the type of streams to unmute.
 * @return Returns {@code true} if the streams are unmuted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerUnMute(AudioStreamType streamType);

/**
 * @brief Checks whether a specified type of streams are muted.
 *
 * @param streamType Indicates the stream type.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerIsMute(AudioStreamType streamType);

/**
 * @brief Mutes or unmutes global audio streams.
 *
 * @param isMute Indicates whether to mute global audio streams. Value {@code true} means global audio streams
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerSetMasterMute(bool isMute);

/**
 * Checks whether audio streams are muted globally.
 *
 * @return Returns {@code true} if all audio streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerIsMasterMute(void);

/**
 * @brief Mutes or unmutes a microphone.
 *
 * @param isMute Indicates whether to mute a microphone. Value {@code true} means that the microphone
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerSetMicrophoneMute(bool isMute);

/**
 * @brief Checks whether a microphone is muted.
 *
 * @return Returns {@code true} if the microphone is muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerIsMicrophoneMute(void);

/**
 * @brief Sets the ringer mode.
 *
 * <p>In silent mode, a device is silent and does not vibrate. In vibration mode, a device is silent and vibrates.
 * In sound mode, a device is audible and may vibrate (depending on user settings).
 *
 * @param mode Indicates the ringer mode, which can be {@link AudioRingMode#RINGER_MODE_NORMAL},
 *             {@link AudioRingMode#RINGER_MODE_SILENT}, or {@link AudioRingMode#RINGER_MODE_VIBRATE}.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @see #getRingerMode()
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerSetRingerMode(AudioRingMode mode);

/**
 * @brief Obtains the ringer mode.
 *
 * This function must be called after {@link SetAudioSource} but before {@link Prepare}.
 *
 * @return Returns the ringer mode, which can be {@link AudioRingMode#RINGER_MODE_NORMAL},
 * {@link AudioRingMode#RINGER_MODE_SILENT}, or {@link AudioRingMode#RINGER_MODE_VIBRATE}.
 * @see #setRingerMode(AudioRingMode)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerGetRingerMode(void);

/**
 * @brief Obtains information about audio input or output devices, or that about all.
 *
 * @param flag Indicates input or output devices, or all audio devices.
 * @return Returns an array with the same information structure as an {@code AudioDeviceInfo} instance if
 * the information is obtained; returns an array whose length is 0 otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerGetDevices(DeviceFlag flag, AudioDeviceInfo *deviceInfo, uint32_t *deviceCount);

/**
 * @brief Query whether offload is supported based on audio offload information.
 *
 * @param info Indicates audio offload information {@link AudioOffloadInfo}.
 * @return Returns {@code true} if the offload mode is support; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioManagerIsSupportOffload(const AudioOffloadInfo *info);

/**
 * @brief Sets the device connection state.
 *
 * @param device Indicates audio device {@link AudioDeviceType}.
 * @param state Indicates audio device connection state {@link AudioDeviceConnectState}.
 * @return Returns {@code SUCCESS} if set the device connection status successfully; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state);

/**
 * @brief Activates audio interruption.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code INTERRUPT_GRANTED} if audio interruption is activated; returns {@code INTERRUPT_DELAYED}
 * if should be delayed; returns {INTERRUPT_FAILED}
 * if be rejected, and other error code defined in {@link audio_errors.h}.
 * @see #deactivateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerActivateAudioInterrupt(AudioInterrupt interrupt);

/**
 * @brief Deactivates audio interruption.
 *
 * <p>If an application does not need to receive interruption messages, call this method to
 * deactivate audio interruption for the application.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code SUCCESS} if audio interruption is deactivated; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @see #activateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerDeactivateAudioInterrupt(AudioInterrupt interrupt);

int32_t AudioManagerDumpInfo(AudioManagerDebugInfo *audioManagerInfo);

int32_t AudioManagerDeinitialize(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // AUDIO_MANAGER_H