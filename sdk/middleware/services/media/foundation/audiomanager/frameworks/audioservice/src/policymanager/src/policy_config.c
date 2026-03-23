/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: policy config impl
* Author: Media Software Group
* Create: 2021-02-28
*/

#include "policy_config.h"
#include "policy_default_config.h"
#include "audio_errors.h"
#include "audio_utils.h"

#define LOG_MODULE_NAME "PolicyConfig"

static uint32_t g_defaultPriorityColumn = sizeof(g_defaultPriorityToStrategies[0]) / sizeof(AudioStrategy);
static uint32_t g_defaultPrioritySize = sizeof(g_defaultPriorityToStrategies) / sizeof(AudioStrategy);

static uint32_t g_defaultInputPriorityColumn = sizeof(g_defaultInputPriorityToStrategies[0]) / sizeof(AudioStrategy);
static uint32_t g_defaultInputPrioritySize = sizeof(g_defaultInputPriorityToStrategies) / sizeof(AudioStrategy);


int32_t LoadPolicyConfig(void)
{
    ALOGI("LoadPolicyConfig");
    return AUDIO_SUCCESS;
}

static StrategyConfig *InquireStrategyConfigForInputStream(AudioStreamType streamType)
{
    for (uint32_t i = 0; i < sizeof(g_defaultInputStrategies) / sizeof(StrategyConfig); ++i) {
        ALOGD("InquireStrategyConfigForInputStream index:%u streamType :0x%x supportDeviceCount:%u",
              i, g_defaultInputStrategies[i].streamType, g_defaultInputStrategies[i].supportDeviceCount);
        if (g_defaultInputStrategies[i].streamType == streamType) {
            return &g_defaultInputStrategies[i];
        }
    }
    ALOGI("InquireStrategyConfigForInputStream failed :0x%x", streamType);
    return NULL;
}

static StrategyConfig *InquireStrategyConfigForOutputStream(AudioStreamType streamType)
{
    for (uint32_t i = 0; i < sizeof(g_defaultStrategies) / sizeof(StrategyConfig); ++i) {
        ALOGD("InquireStrategyConfigForOutputStream index:%u streamType :0x%x supportDeviceCount:%u",
              i, g_defaultStrategies[i].streamType, g_defaultStrategies[i].supportDeviceCount);
        if (g_defaultStrategies[i].streamType == streamType) {
            return &g_defaultStrategies[i];
        }
    }
    ALOGI("InquireStrategyConfigForOutputStream failed :0x%x", streamType);
    return NULL;
}

StrategyConfig *InquireStrategyConfigForStream(AudioStreamType streamType, bool isOutput)
{
    ALOGD("streamType:0x%x isOutput:%d", streamType, isOutput);
    if (isOutput) {
        return InquireStrategyConfigForOutputStream(streamType);
    } else {
        return InquireStrategyConfigForInputStream(streamType);
    }
}

static int32_t InquirePriorityForInputStream(AudioStreamType incomingStream, uint32_t *priority)
{
    for (uint32_t i = 0; i < sizeof(g_defaultInputStrategies) / sizeof(StrategyConfig); ++i) {
        if (g_defaultInputStrategies[i].streamType == incomingStream) {
            *priority =  g_defaultInputStrategies[i].priority;
            return AUDIO_SUCCESS;
        }
    }
    ALOGI("InquirePriorityForStream failed :0x%x", incomingStream);
    return AUDIO_ERROR;
}

static int32_t InquirePriorityForOutputStream(AudioStreamType incomingStream, uint32_t *priority)
{
    for (uint32_t i = 0; i < sizeof(g_defaultStrategies) / sizeof(StrategyConfig); ++i) {
        if (g_defaultStrategies[i].streamType == incomingStream) {
            *priority =  g_defaultStrategies[i].priority;
            return AUDIO_SUCCESS;
        }
    }
    ALOGI("InquirePriorityForStream failed :0x%x", incomingStream);
    return AUDIO_ERROR;
}


bool IsNeedResumeForStream(AudioStreamType streamType)
{
    for (uint32_t i = 0; i < sizeof(g_defaultStrategies) / sizeof(StrategyConfig); ++i) {
        ALOGI("index:%u streamType :0x%x IsNeedResume:%u",
              i, g_defaultStrategies[i].streamType, g_defaultStrategies[i].needResume);
        if (g_defaultStrategies[i].streamType == streamType) {
            return g_defaultStrategies[i].needResume;
        }
    }
    for (uint32_t i = 0; i < sizeof(g_defaultInputStrategies) / sizeof(StrategyConfig); ++i) {
        ALOGI("index:%u streamType :0x%x IsNeedResume:%u",
              i, g_defaultInputStrategies[i].streamType, g_defaultInputStrategies[i].needResume);
        if (g_defaultInputStrategies[i].streamType == streamType) {
            return g_defaultInputStrategies[i].needResume;
        }
    }
    return false;
}

static int32_t InquireStrategyForOutputStream(AudioStreamType focusStream, AudioStreamType incomingStream,
    AudioStrategy *strategy)
{
    int32_t ret;
    uint32_t incomingStreamPriority = 0;
    ret = InquirePriorityForOutputStream(incomingStream, &incomingStreamPriority);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    if (incomingStreamPriority >= g_defaultPriorityColumn) {
        ALOGI("invalid incomingStreamPriority :%u", incomingStreamPriority);
        return AUDIO_ERROR;
    }
    uint32_t focusStreamPriority = 0;
    ret = InquirePriorityForOutputStream(focusStream, &focusStreamPriority);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    uint32_t defaultPriorityRow = g_defaultPrioritySize / g_defaultPriorityColumn;
    if (focusStreamPriority >= defaultPriorityRow) {
        ALOGI("invalid focusStreamPriority :%u", focusStreamPriority);
        return AUDIO_ERROR;
    }
    *strategy = g_defaultPriorityToStrategies[focusStreamPriority][incomingStreamPriority];
    ALOGI("InquireStrategyForStream focusStream:0x%x incomingStream:0x%x strategy:%d",
          focusStream, incomingStream, *strategy);
    return AUDIO_SUCCESS;
}


static int32_t InquireStrategyForInputStream(AudioStreamType focusStream, AudioStreamType incomingStream,
    AudioStrategy *strategy)
{
    int32_t ret;
    CHK_NULL_RETURN(strategy, AUDIO_INVALID_PARAMS, "strategy is NULL");
    ALOGI("InquireStrategyForStream focusStream:0x%x incomingStream:0x%x", focusStream, incomingStream);
    uint32_t incomingStreamPriority = 0;
    ret = InquirePriorityForInputStream(incomingStream, &incomingStreamPriority);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    if (incomingStreamPriority >= g_defaultInputPriorityColumn) {
        ALOGI("invalid incomingStreamPriority :%u", incomingStreamPriority);
        return AUDIO_ERROR;
    }
    uint32_t focusStreamPriority = 0;
    ret = InquirePriorityForInputStream(focusStream, &focusStreamPriority);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    uint32_t defaultPriorityRow = g_defaultInputPrioritySize / g_defaultInputPriorityColumn;
    if (focusStreamPriority >= defaultPriorityRow) {
        ALOGI("invalid focusStreamPriority :%u", focusStreamPriority);
        return AUDIO_ERROR;
    }
    *strategy = g_defaultInputPriorityToStrategies[focusStreamPriority][incomingStreamPriority];
    ALOGI("InquireStrategyForStream focusStream:0x%x incomingStream:0x%x strategy:%d",
          focusStream, incomingStream, *strategy);
    return AUDIO_SUCCESS;
}

int32_t InquireStrategyForStream(AudioStreamType focusStream, AudioStreamType incomingStream, bool isOutput,
    AudioStrategy *strategy)
{
    CHK_NULL_RETURN(strategy, AUDIO_INVALID_PARAMS, "strategy is NULL");
    ALOGI("InquireStrategyForStream focusStream:0x%x incomingStream:0x%x", focusStream, incomingStream);
    if (isOutput) {
        return InquireStrategyForOutputStream(focusStream, incomingStream, strategy);
    } else {
        return InquireStrategyForInputStream(focusStream, incomingStream, strategy);
    }
}
