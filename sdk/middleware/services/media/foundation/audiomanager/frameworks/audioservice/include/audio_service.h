/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio service interface
* Author: Media Software Group
* Create: 2021-02-28
*/

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines custom types needed for audio policy manager-related APIs and provides functions, for example,
 * to load drivers, access a driver adapter, and control audio streams.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_service.h
 *
 * @brief Declares APIs for operations related to the audio service.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVICE_H

#include "audio_base_type.h"
#include "audio_interrupt_proxy.h"
#include "audio_debug_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief 初始化AudioService.
 *
 * @return Returns <b>0</b> if the Audio Service is inited successfully; returns a negative value otherwise.
 */
int32_t AudioServiceInit(void);

/**
 * @brief 去初始化AudioService.
 *
 * @return Returns <b>0</b> if the Audio Service is deinited successfully; returns a negative value otherwise.
 */
int32_t AudioServiceDeInit(void);

/**
 * @brief 获取音频 session ID.
 *
 * @return Returns the session ID if created; returns {@code -1} otherwise.
 * @since 1.0
 * @version 1.0
 */
AudioSession AudioServiceAcquireAudioSessionId(void);

/**
 * @brief 根据流类型设置音量.
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
bool AudioServiceSetVolume(AudioStreamType streamType, int32_t volume);

/**
 * @brief 获取指定类型音频流的音量.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the volume level of the type of streams.
 * @see #getMaxVolume(AudioStreamType)
 * @see #getMinVolume(AudioStreamType)
 * @see #setVolume(AudioStreamType, int)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetVolume(AudioStreamType streamType);

/**
 * @brief 获取指定类型音频流的最小音量.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the minimum valid volume level for the type of streams.
 * @see #getVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetMinVolume(AudioStreamType streamType);

/**
 * @brief 获取指定类型音频流的最大音量.
 *
 * @param streamType Indicates the stream type.
 * @return Returns the maximum valid volume level for the type of streams.
 * @see #getVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetMaxVolume(AudioStreamType streamType);

/**
 * @brief 静音指定类型音频流.
 *
 * @param streamType Indicates the type of streams to mute.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceMute(AudioStreamType streamType);

/**
 * @brief 接触指定类型音频流的静音.
 *
 * @param streamType Indicates the type of streams to unmute.
 * @return Returns {@code true} if the streams are unmuted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceUnMute(AudioStreamType streamType);

/**
 * @brief 检查指定类型音频流的静音状态.
 *
 * @param streamType Indicates the stream type.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceIsMute(AudioStreamType streamType);

/**
 * @brief 全局静音或者解除全局静音.
 *
 * @param isMute Indicates whether to mute global audio streams. Value {@code true} means global audio streams
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceSetMasterMute(bool isMute);

/**
 * @brief 检查是否全局静音.
 *
 * @return Returns {@code true} if all audio streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceIsMasterMute(void);


/**
 * @brief 静音microphone或者解除microphone静音.
 *
 * @param isMute Indicates whether to mute a microphone. Value {@code true} means that the microphone
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceSetMicrophoneMute(bool isMute);

/**
 * @brief 检查microphone 是否静音.
 *
 * @return Returns {@code true} if the microphone is muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceIsMicrophoneMute(void);

/**
 * @brief 设置铃声模式.
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
bool AudioServiceSetRingerMode(AudioRingMode mode);

/**
 * @brief 获取当前铃声模式.
 *
 * This function must be called after {@link SetAudioSource} but before {@link Prepare}.
 *
 * @return Returns the ringer mode, which can be {@link AudioRingMode#RINGER_MODE_NORMAL},
 * {@link AudioRingMode#RINGER_MODE_SILENT}, or {@link AudioRingMode#RINGER_MODE_VIBRATE}.
 * @see #setRingerMode(AudioRingMode)
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetRingerMode(void);


/**
 * @brief 获取输入、输出或者所有的设备的数量.
 *
 * @param flag Indicates input or output devices, or all audio devices.
 * @param deviceCount Indicates input or output devices count, or all audio devices count.
 * @return Returns {@code AUDIO_SUCCESS} if audio device count is successful; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetDeviceCount(DeviceFlag flag, uint32_t *deviceCount);


/**
 * @brief 获取输入、输出或者所有的设备信息.
 *
 * @param flag Indicates input or output devices, or all audio devices.
 * @param devices Indicates input or output devices information, or all audio devices information.
 * @return Returns {@code AUDIO_SUCCESS} if audio device count is successful; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceGetDevices(DeviceFlag flag, uint32_t deviceCount, AudioDeviceInfo *devices);

/**
 * @brief 设备连接状态设置.
 *
 * state changes of an audio device, for example, when it becomes connected or
 * disconnected.
 *
 * @param device Indicates the device to set.
 * @param state Indicates the device connect state.
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state);


/**
 * @brief 检查是否支持offload模式.
 *
 * Check if hw offload is possible for given format, stream type, sample rate,
 * bit rate, duration, video and streaming or offload property is enabled
 *
 * @param offloadInfo Indicates the offload info .
 * @since 1.0
 * @version 1.0
 */
bool AudioServiceIsOffloadSupported(const AudioOffloadInfo *offloadInfo);


/**
 * @brief 激活音频中断.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code AUDIO_SUCCESS} if audio interruption is activated; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @see #deactivateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceActivateAudioInterrupt(const AudioInterruptProxy *interrupt);

/**
 * @brief 去激活音频中断.
 *
 * <p>If an application does not need to receive interruption messages, call this method to
 * deactivate audio interruption for the application.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code AUDIO_SUCCESS} if audio interruption is deactivated; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @see #activateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceDeactivateAudioInterrupt(const AudioInterruptProxy *interrupt);

/**
 * @brief dfx.
 *
 * <p>If an application does not need to receive interruption messages, call this method to
 * deactivate audio interruption for the application.
 *
 * @param isInput Indicates the interruption is input or output.
 * @return Returns interrupt nums if audio interruption is deactivated; returns
 * {@code AUDIO_ERROR} otherwise.
 * @see InterruptDebugInfo
 * @since 1.0
 * @version 1.0
 */
int32_t AudioServiceDumpInfo(AudioServiceDebugInfo *audioServiceDebugInfo);

int32_t AudioServiceInvoke(AudioHaidInvokeId audioHaidInvokeId, char *param, uint32_t *paramlength);

typedef int32_t (*AudioServiceHaidEventCallback)(int32_t eventType, void *param, void *context);

int32_t AudioServiceRegCallBack(AudioServiceHaidEventCallback callBack, void *context);

int32_t AudioServiceSetParam(AudioLinkDirection direction, const char *param, uint32_t len);

int32_t AudioServiceGetParam(AudioLinkDirection direction, char *param, uint32_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_SERVICE_H */
/** @} */
