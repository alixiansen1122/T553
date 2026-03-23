/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: policy manager interfaces
* Author: Media Software Group
* Create: 2021-04-07
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
 * @file policy_manager.h
 *
 * @brief Declares APIs for operations related to the audio policy manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef POLICY_MANAGER_H
#define POLICY_MANAGER_H

#include "audio_base_type.h"
#include "audio_interrupt_proxy.h"
#include "audio_debug_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_STREAM_NUM 5

/**
 * @brief 音频流信息定义.
 *
 * @see AudioStreamType
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /* 流index */
    int32_t  streamIndex;
    /* 流类型 */
    AudioStreamType streamType;
    /* 流类型音量 */
    float streamTypeVolume;
    AudioLinkDirection audioLinkDir;
} AudioStreamDescriptor;


/**
 * @brief 经音频策略模块决策的流信息定义.
 *
 * @see AudioStreamDescriptor
 * @see AudioDeviceInfo
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /* 当前流信息 */
    AudioStreamDescriptor streamDesc;
    /* 流的混音权重, 独占时 是100% ，混音时最新的流时100% ，其他降低音量的流时50% */
    float mixWeight;
    /* 当前流的路由设备数量 */
    uint32_t currentRoutedDeviceNum;
    /* 当前流的路由设备，如果有多个是AudioDeviceType或 */
    uint32_t currentRoutedDevices;
} AudioRoutedStreamDescriptor;

/**
 * @brief 经音频策略模块决策的流信息定义.
 *
 * @see AudioStreamDescriptor
 * @see AudioDeviceInfo
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /* 当前流对应策略 */
    AudioStrategy  currentStrategy;
    /* 活跃流的数量 (独占时只有一条流且混音权重是1，混音是包括所有流) */
    uint32_t activeStreamNum;
    /* 活跃流的列表 */
    AudioRoutedStreamDescriptor  *activeStreams[MAX_STREAM_NUM];
} AudioStreamStrategy;

/**
 * @brief 音频策略管理模块回调函数类型.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /* 路由设备状态改变 */
    POLICY_ROUTE_DEVICE_CHANGED = 0,
    /* 未定义 方便扩展 */
    POLICY_UNKNOMN
} PolicyCallBackType;

/**
 * @brief 音频策略管理模块流状态类型.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    STREAM_NONE = 0x0,
    STREAM_IN_FLAG = 0x800000,
    /* 输出流创建 */
    STREAM_OUT_CREATE = 0x1,
    /* 输出流恢复 */
    STREAM_OUT_RESUME = 0x2,
    /* 输出流销毁 */
    STREAM_OUT_DESTROY = 0x4,
    /* 输入流创建 */
    STREAM_IN_CREATE = STREAM_IN_FLAG | 0x10,
    /* 输入流恢复 */
    STREAM_IN_RESUME = STREAM_IN_FLAG | 0x20,
    /* 输入流销毁 */
    STREAM_IN_DESTROY = STREAM_IN_FLAG | 0x40,
} StreamStatus;

/**
 * @brief 音频策略模块回调函数结构体.
 *
 * @param userData the user data ptr.
 * @param type the call back type.
 * @param data Indicates the call back uint32_t data.
 * @param data Indicates the call back ptr data.
 * @param streamType Indicates the audio stream type.
 * @see PolicyCallBackType
 * @see AudioDeviceInfo
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    int32_t (*OnCallback)(PolicyCallBackType type, uint32_t dataLen, const uint8_t *callbackData);
} AudioPolicyCallback;

/**
 * @brief 初始化策略管理模块.
 *
 * @return Returns <b>0</b> if the policy manager is inited successfully; returns a negative value otherwise.
 */
int32_t PolicyMgrInit(void);

/**
 * @brief 去初始化策略管理模块.
 *
 * @return Returns <b>0</b> if the policy manager is deinited successfully; returns a negative value otherwise.
 */
int32_t PolicyMgrDeInit(void);

/**
 * @brief 查询指定设备是否能路由.
 *
 * @param device Indicates the pointer to the device.
 * @return Returns <b>true</b> if the device Can be routed; returns false otherwise.
 * @see AudioDeviceInfo
 */
bool PolicyMgrCanBeRoutedToThisDevice(AudioDeviceType device, bool isOutput);

/**
 * @brief 查询指定流支持的设备.
 *
 * @param streamType Indicates the Audio Stream Type.
 * @param supportDeviceCount Indicates the pointer to the support Device Count.
 * @param supportDevices Indicates the pointer to the support Devices.
 * @return Returns <b>true</b> if the device Can be routed; returns false otherwise.
 * @see AudioDeviceInfo
 */
bool PolicyMgrGetSupportDevicesForStream(AudioStreamType streamType, uint32_t *supportDeviceCount,
                                         AudioDeviceType **supportDevices);

/**
 * @brief Activates audio interruption.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code SUCCESS} if audio interruption is activated; returns an error code defined
 * in {@link media_errors.h} otherwise.
 * @see #deactivateAudioInterrupt(AudioInterruptProxy)
 * @see AudioInterruptProxy
 * @since 1.0
 * @version 1.0
 */
int32_t PolicyMgrActivateAudioInterrupt(const AudioInterruptProxy *interrupt);

/**
 * @brief Deactivates audio interruption.
 *
 * <p>If an application does not need to receive interruption messages, call this method to
 * deactivate audio interruption for the application.
 *
 * @param interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code SUCCESS} if audio interruption is deactivated; returns an error code defined
 * in {@link media_errors.h} otherwise.
 * @see #activateAudioInterrupt(AudioInterruptProxy)
 * @see AudioInterruptProxy
 * @since 1.0
 * @version 1.0
 */
int32_t PolicyMgrDeactivateAudioInterrupt(const AudioInterruptProxy *interrupt);


/**
 * @brief 根据当前流列表获取对应的流策略.
 *
 * @param streamNum Indicates the num of sream.
 * @param streamDesc Indicates the pointer to the stream info.
 * @return Returns ptr of AudioStreamStrategy if the audio stream strategy is got successfully;
 * returns a nullptr otherwise.
 * @see AudioStreamDescriptor
 * @see AudioStreamStrategy
 */
AudioStreamStrategy *PolicyMgrGetStrategyForStream(AudioSession sessionID, const AudioStreamDescriptor *streamDesc,
                                                   StreamStatus streamStatus);

/**
 * @brief Creates an audio input stream.
 *
 * @param pfnCb Indicates the pointer to the audio policy callback.
 * @param cookie Indicates the pointer to the user data.
 * @return Returns <b>0</b> if the audio policy callback is created successfully; returns a negative value otherwise.
 * @see AudioPolicyCallback
 */
int32_t PolicyMgrRegisterPolicyCallback(const AudioPolicyCallback *callback);

/**
 * @brief for dfx.
 *
 * @param audioInterruptInfo Indicates the pointer to the audio interrupt Info.
 * @return Returns <b>0</b> if the audio interrupt Infois dumped successfully; returns a negative value otherwise.
 */
int32_t PolicyMgrDumpInterruptInfo(AudioInterruptDebugInfo *audioInterruptInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* POLICY_MANAGER_H */
/** @} */
