/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: policy config interface
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef POLICY_CONFIG_H
#define POLICY_CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include "policy_manager.h"

/**
 * @brief 流类型{@link #AudioStreamType} 的方向，仅输入、仅输出、既有输入也有输出,
 * 不支持从配置文件读取，不能根据产品形态配置
 *
 */
typedef enum {
    AUDIO_STREAM_OUT    = 0x1u, /**< Output AudioStream */
    AUDIO_STREAM_IN     = 0x2u, /**< Input AudioStream */
    AUDIO_STREAM_OUT_IN = 0x3u, /**< Input/output AudioStream, supporting both audio input and output */
} AudioStreamDirection;

/**
 * @brief 策略配置，根据流类型{@link #AudioStreamType} 配置各类型流的优先级、支持的设备、是否输出到所有支持设备,
 * 支持从配置文件读取.
 *
 * 高优先级 -  低优先级  打断、混音
 * 低优先级 - 高优先级 不播放
 */
typedef struct {
    /* 流类型 */
    AudioStreamType     streamType;
    /* 流类型方向 */
    AudioStreamDirection     streamDir;
    /* 流类型对应优先级 0 - max 越小优先级越高 */
    uint32_t            priority;
    /* 闹钟支持输出到当前所有可用设备 */
    bool                routeAllDevices;
    /* 当前流被打断后是否需要恢复播放 */
    bool                needResume;
    /* 当前流允许路由的设备, 蓝牙通话只支持Speaker、Music 支持蓝牙、Speaker, 下标表示优先级，越小优先级越高 */
    uint32_t            supportDeviceCount;
    AudioDeviceType     *supportDevices;
} StrategyConfig;

int32_t LoadPolicyConfig(void);

StrategyConfig *InquireStrategyConfigForStream(AudioStreamType streamType, bool isOutput);

bool IsNeedResumeForStream(AudioStreamType streamType);

int32_t InquireStrategyForStream(AudioStreamType focusStream, AudioStreamType incomingStream, bool isOutput,
    AudioStrategy *strategy);

#endif  // DEVICE_MANAGER_H_
