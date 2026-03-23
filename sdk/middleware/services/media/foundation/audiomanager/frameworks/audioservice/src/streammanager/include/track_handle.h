/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: track interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef TRACK_HANDLE_H
#define TRACK_HANDLE_H

#include <time.h>
#include "stream_utils.h"
#include "audio_debug_info.h"
#include "audio_base_type.h"
#include "audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define NOT_SET_STREAM_VOLUME (-1)
#define COMPARE_THRESHOLD (1e-6)

/**
 * @brief 创建track handle 上下文，并构建索引
 *
 * @param streamId 流的索引
 * @param deviceId 指定的输出设备
 * @return 正常返回0，否则返回非0
 */
uintptr_t CreateTrackHandle(int32_t streamId, const AudioStreamInfo *inf, uint32_t defalutOutDevice, VolumeInf volInf);

/**
 * @brief 销毁track handle 上下文
 *
 * @param handle 流的索引
 * @param deviceId 指定的输出设备
 * @return 正常返回0，否则返回非0
 */
int32_t DestroyTrackHandle(uintptr_t handle);

/**
 * @brief 设置回调函数，
 *
 * @param streamId 流的索引
 * @param deviceId 指定的输出设备
 * @return 正常返回0，否则返回非0
 */
int32_t SetCallback(uintptr_t handle, RendererCallback cbk, const void *caller);

/**
 * @brief 指定 streamId 标定的流，输出到 deviceId 标定的设备
 *
 * @param streamId 指定的流
 * @param deviceId 指定的输出设备
 * @return 正常返回0，否则返回非0
 */
int32_t SetTrackOutputDevice(uintptr_t handle, int32_t deviceId, const AudioStreamInfo *info);

/**
 * @brief 获取指定 streamId 的状态
 *
 * @param handle 指定流handle
 * @return 正常返回0，否则返回非0
 */
StreamState GetState(uintptr_t handle);

/**
 * @brief 获取指定handle对应的ID
 *
 * @param handle 指定流handle
 * @return 正常返回0，否则返回非0
 */
int32_t GetHandleId(uintptr_t handle, int32_t *streamId, AudioSession *sessionId);

/**
 * @brief handle代表的track开启
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t StartTrack(uintptr_t handle);

/**
 * @brief handle代表的track暂停
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t PauseTrack(uintptr_t handle);

/**
 * @brief 将指定的流停止
 *
 * @param streamId 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t StopTrack(uintptr_t handle);

/**
 * @brief  设置指定的流音量
 *
 * 流音量不会持久化
 *
 * @param handle 指定流的handle
 * @param typeVolume 流所在类型的类型音量
 * @param mixWeight 指定流当前的混音比重
 * @param streamVolume 设置该流音量，如果设置为NOT_SET_STREAM_VOLUME表示使用当前流音量
 * @return 正常返回0，否则返回非0
 */
int32_t SetTrackVolume(uintptr_t handle, float typeVolume, float mixWeight, float streamVolume);

/**
 * @brief  获取指定的流音量
 *
 * @param streamId 指定的流
 * @return 指定类型音量，-1表示无效
 */
float GetTrackVolume(uintptr_t handle);

/**
 * @brief 设置指定的流速度
 *
 * @param speed 设定的速度
 * @return 正常返回0，否则返回非0
 */
int32_t SetStreamPlayBackSpeed(uintptr_t handle, float speed);

/**
 * @brief  获取当前设备
 *
 * @param streamId 指定的流
 * @return 当前设备
 */
int32_t GetStreamDeviceId(uintptr_t handle);

/**
 * @brief 获取数据通道ID
 *
 * @param streamId 指定的流
 * @param channelId 数据通道ID指针
 * @return 正常返回0，否则返回非0
 */
int32_t GetStreamChannelId(uintptr_t handle, uint32_t *channelId);

/**
 * @brief 连接前端id
 *
 * @param handle 流的索引
 * @param backEndId 数据通道ID指针
 * @return 正常返回0，否则返回非0
 */
int32_t TrackAttachFrontend(uintptr_t handle, uint32_t frontendId);

/**
 * @brief 将数据写给指定的流
 *
 * @param streamId 指定的流
 * @param buffer 数据所在的地址
 * @param bufferSize 数据大小
 * @param replyBytes 实际写入的大小
 * @return 正常返回0，否则返回非0
 */
int32_t StreamWriteTrack(uintptr_t handle, const uint8_t *buffer, uint64_t bufferSize, uint64_t *replyBytes);

/**
 * @brief 将指定的流flush
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t TrackFlush(uintptr_t handle);

/**
 * @brief 切换输出路由设备
 *
 * @param handle 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t ChangeRoute(uintptr_t handle, int32_t deviceId);

/**
 * @brief 获取当前正在播放的音频流的位置
 * @param position 存放位置
 * @return 正常返回0，否则返回非0
 */
int32_t GetStreamRenderPosition(uintptr_t handle, int64_t *position);

/**
 * @brief 获取当前正在播放的音频流的时间
 * @param timestamp 存放时间
 * @return 正常返回0，否则返回非0
 */
int32_t GetStreamTimestamp(const uintptr_t handle, struct timespec *position);

/**
 * @brief ?? 缓存中frame 个数
 * @param timestamp 存放时间
 * @return 正常返回0，否则返回非0
 */
int32_t GetBufferFrameSize(uintptr_t handle);

/**
 * @brief 缓存欠载次数
 * @param timestamp 存放时间
 * @return 正常返回0，否则返回非0
 */
int32_t GetBufferUnderflowTimes(int32_t streamId);

AudioStreamType GetStreamType(uintptr_t handle);

int32_t SetStreamMixWeight(uintptr_t handle, float setMixWeight);

bool GetStreamMixWeight(uintptr_t handle, float *mixWeight);

/**
 * @brief 缓存欠载次数
 * @param handle 指定的流
 * @param mute 是否mute
 * @return 正常返回0，否则返回非0
 */
int32_t SetTrackMute(uintptr_t handle, bool mute);

StreamDebugInfo DumpTrackInfo(uintptr_t handle);

int32_t SetTrackParam(uintptr_t handle, const char *keyValueList, size_t len);

int32_t GetTrackParam(uintptr_t handle, char *keyValueList, size_t len);

int32_t DeactivateCorrespondingTrack(uintptr_t handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // TRACK_HANDLE_H

