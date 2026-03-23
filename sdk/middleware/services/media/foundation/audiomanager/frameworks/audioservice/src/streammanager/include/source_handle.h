/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: source interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef SOURCE_HANDLE_H
#define SOURCE_HANDLE_H

#include <time.h>
#include "audio_debug_info.h"
#include "audio_base_type.h"
#include "stream_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    uint32_t inputSource;
    uint32_t audioFormat;
    int32_t sampleRate;
    int32_t channelCount;
    int32_t bitRate;
    uint32_t streamType;
    uint32_t bitWidth;
} AudioCapturerInfo;

/**
 * @brief 创建source handle 上下文，并构建索引
 *
 * @param streamId 流的索引
 * @param inf 需要的流信息
 * @return 正常返回0，否则返回非0
 */
uintptr_t CreateSourceHandle(int32_t streamId, const AudioStreamInfo *inf,
    uint32_t defaultRouteDevice, VolumeInf volInf);

/**
 * @brief 销毁track handle 上下文
 *
 * @param handle 流的索引
 * @param deviceId 指定的输出设备
 * @return 正常返回0，否则返回非0
 */
int32_t DestroySourceHandle(uintptr_t handle);

/**
 * @brief 销毁track handle 上下文
 *
 * @param handle 流的索引
 * @param cbk 流的回调函数
 * @param caller 回调时通知对象
 * @return 正常返回0，否则返回非0
 */
int32_t SetSourceCallback(uintptr_t handle, CapturerCallback cbk, const void *caller);

/**
 * @brief 指定 streamId 标定的流，输出到 deviceId 标定的设备
 *
 * @param handle 流的索引
 * @param deviceId 指定的输出设备
 * @param info 需要的流信息
 * @return 正常返回0，否则返回非0
 */
int32_t SetSourceInputDevice(uintptr_t handle, int32_t deviceId, const AudioStreamInfo *info);

/**
 * @brief  获取当前设备
 *
 * @param streamId 指定的流
 * @return 当前设备
 */
int32_t GetSourceDeviceId(uintptr_t handle);

/**
 * @brief 获取数据通道ID
 *
 * @param handle 流的索引
 * @param channelId 数据通道ID指针
 * @return 正常返回0，否则返回非0
 */
int32_t GetSourceChannelId(uintptr_t handle, uint32_t *channelId);

/**
 * @brief 连接后端id
 *
 * @param handle 流的索引
 * @param backEndId 数据通道ID指针
 * @return 正常返回0，否则返回非0
 */
int32_t SourceAttachBackend(uintptr_t handle, uint32_t backEndId);

/**
 * @brief 获取输入流时间戳
 *
 * @param handle 流的索引
 * @param timestamp 时间戳数据结构指针
 * @return 正常返回0，否则返回非0
 */
int32_t GetSourceTimestamp(uintptr_t handle, struct timespec *timestamp);

/**
 * @brief handle代表的track开启
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t StartSource(uintptr_t handle);

/**
 * @brief  获取输入音频流数据
 *
 * @param handle 指定的流
 * @param buffer buffer信息
 * @param isBlocking 是否阻塞
 * @return 正常返回0，否则返回非0
 */
int32_t ObtainSourceBuffer(uintptr_t handle, const CapturerBuffer *buffer);

/**
 * @brief 将指定的流停止
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t StopSource(uintptr_t handle);

/**
 * @brief 获取当前流的时间
 * @param handle 指定的流
 * @param timestamp 存放时间
 * @return 正常返回0，否则返回非0
 */
int32_t GetTimestamp(uintptr_t handle, struct timespec *timestamp);

/**
 * @brief  释放输入音频流数据
 *
 * @param handle 指定的流
 * @param buffer 要释放的buffer
 * @param frameSize buffer中携带的framesize
 * @return 当前设备
 */
void ReleaseBuffer(uintptr_t handle, CapturerBuffer *buffer, size_t frameSize);

/**
 * @brief  设置输入音频流是否静音
 *
 * @param handle 指定的流
 * @param mute 值为true，设置静音；其值为 false 时，解除静音
 * @return 正常返回0，否则返回非0
 */

int32_t SetSourceMute(uintptr_t handle, bool mute);

int32_t SetSourceVolume(uintptr_t handle, float volume);

int32_t GetSourceSessionId(uintptr_t handle, AudioSession *sessionId);

AudioStreamType GetSourceStreamType(uintptr_t handle);

int32_t GetSourceStreamId(uintptr_t handle);

int32_t DeactivateCorrespondingSource(uintptr_t handle);

StreamDebugInfo DumpSourceInfo(uintptr_t handle);

int32_t SetSourceParam(uintptr_t handle, const char *keyValueList, size_t len);

int32_t GetSourceParam(uintptr_t handle, char *keyValueList, size_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

