/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: stream manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include "audio_base_type.h"
#include "audio_debug_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    EventError,
    EventCreated,
    EventDestroied,
    EventMax = 0x1FFFFFFF
} EventType;

typedef struct {
    int32_t (*OnEvent)(uintptr_t appData, EventType event,
        uint32_t data1, uint32_t data2, uintptr_t eventData);
    uintptr_t instance;
} StreamChangeCallback;

/**
 * @brief 模块初始化
 *
 * 获取类型音量，启动内部线程
 *
 * @return 正常返回0，否则返回非0
 */
int32_t StreamMgrInit(void);

/**
 * @brief 模块去初始化
 *
 * @return 正常返回0，否则返回非0
 */
int32_t StreamMgrDeInit(void);

/**
 * @brief 设置回调函数，来观察流的变化
 *
 * @param callback 观察流的回调函数
 * @param instance 上层的观察实例
 * @return 正常返回0，否则返回非0
 */
int32_t StreamMgrSetCallback(const StreamChangeCallback *callback);

/**
 * @brief 通知 StreamManager 创建新的流，StreamManager 在内部构建对内的 TrackHandle
 *
 * 创建流和销毁流都会触发内部的策略更新
 *
 * @param inf 创建流需要的信息
 * @return 返回streamId，正常大于等于0，异常小于0
 */
uintptr_t StreamMgrCreateStreamOut(const AudioStreamInfo *inf);

/**
 * @brief 通知 StreamManager 销毁track
 *
 * 创建流和销毁流都会触发内部的策略更新
 *
 * @param inf 之前创建的track
 * @return 正常销毁返回0，否则非0
 */
int32_t StreamMgrDestroyStreamOut(uintptr_t handle);

/**
 * @brief 通知 StreamManager 创建新的流
 *
 * 创建流和销毁流都会触发内部的策略更新
 *
 * @param inf 创建流需要的信息
 * @return 正常创建返回TrackHandle，否则NULL
 */
uintptr_t StreamMgrCreateStreamIn(const AudioStreamInfo *inf);

/**
 * @brief 通知 StreamManager 销毁track
 *
 * 创建流和销毁流都会触发内部的策略更新
 *
 * @param inf 之前创建的track
 * @return 正常销毁返回0，否则非0
 */
int32_t StreamMgrDestroyStreamIn(uintptr_t handle);

/**
 * @brief  设置指定类型的音量
 *
 * 类型音量会持久化，方便重启后的音量设定
 *
 * @param streamType 指定的类型
 * @return 正常返回0，否则返回非0
 */
int32_t StreamMgrSetTypeVolume(int32_t streamType, float typeVolume);

/**
 * @brief  获取指定类型的音量
 *
 * @param streamType 指定的类型
 * @return 指定类型音量，-1表示无效
 */
bool StreamMgrGetTypeVolume(int32_t streamType, float *typeVolume);

/**
 * @brief  mute指定类型流
 *
 * 流音量不会持久化
 *
 * @param streamId 指定的流
 * @return 正常返回0，否则返回非0
 */
int32_t StreamMgrSetMute(AudioStreamType streamType, bool mute);

/**
 * @brief  指定类型流是否mute
 *
 * @param streamId 指定的流
 * @return true表示mute，flase表示没有mute
 */
bool StreamMgrIsMute(AudioStreamType streamType);

/**
 * @brief  全局mute
 *
 * @param streamId 指定的流
 * @return 正常返回0，否则返回非0
 */
bool StreamMgrSetMasterMute(bool isMute);

/**
 * @brief  全局是否mute
 *
 * @return true表示全局mute，flase表示没有mute
 */
bool StreamMgrIsMasterMute(void);

/**
 * @brief 设置铃声模式
 *
 * @param mode 铃声模式，NORMAL、VIBRATE、SILENT
 * @return true表示设置正常，false不正常
 */
bool StreamMgrSetRingerMode(int mode);

/**
 * @brief 获取当前铃声模式
 *
 * @param mode 铃声模式，NORMAL、VIBRATE、SILENT
 * @return true表示设置正常，false不正常
 */
int32_t StreamMgrGetRingerMode(void);

/**
 * @brief 设置麦克风是否mute
 *
 * @param isMute 是否mute
 * @return true表示设置正常，false不正常
 */
bool StreamMgrSetMicrophoneMute(bool isMute);

/**
 * @brief 获取当前麦克风是否mute
 *
 * @return true表示mute，false不mute
 */
bool StreamMgrIsMicrophoneMute(void);

int32_t StreamMgrDumpInfo(AudioStreamDebugInfo *audioStreamInfo);

int32_t StreamManagerDeactivateCorrespondingStream(AudioSession sessionID, AudioStreamType streamType);

int32_t StreamManagerSetParam(AudioLinkDirection direction, const char *param, uint32_t len);

int32_t StreamManagerGetParam(AudioLinkDirection direction, char *param, uint32_t len);

bool StreamMgrIsCreateStreamByType(AudioStreamType streamType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* STREAM_MANAGER_H */
