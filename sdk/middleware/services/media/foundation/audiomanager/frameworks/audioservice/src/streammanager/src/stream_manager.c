/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: stream manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "stream_manager.h"
#include <stdlib.h>
#include <math.h>
#include "securec.h"
#include "policy_manager.h"
#include "source_handle.h"
#include "track_handle.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "stream_config.h"
#ifdef SUPPORT_KV
#include "kvstore_env.h"
#include "kv_store.h"
#include "ohos_errno.h"
#endif
#ifdef SUPPORT_NV
#include "nv.h"
#endif
#include "media_thread_adapt.h"
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
#include "hifi_clock_freq_manager.h"
#endif

#define LOG_MODULE_NAME "StreamMgr"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CONCURRENT_ACTIVE_STREAM_MAX_NUM 10
#define ONE_STREAM_EXCLUSIVE 1
#define MAX_LISTENER_NUM 6
#define GOLDEN_POINT 0.618
#define MAX_VOLUME_POINT 1
#define THOUSAND 1000
#define MAX_FLOAT_TO_CHAR_NUM 10
#define MAX_STREAM_IN_NUM 3
#define MEDIA_TYPE_VOLUME "media_type_volume"
#define RING_TYPE_VOLUME "ring_type_volume"
#define VOICE_CALL_TYPE_VOLUME "voice_call_type_volume"

typedef struct {
    StreamChangeCallback *listenerPtr;
    int curCapbility;
    int curNum;
} ListenersWrapper;

typedef struct {
    int32_t streamId;
    uintptr_t streamHandle;
    AudioStreamType streamType;
    bool active;
    AudioLinkDirection linkDir;
} StreamSimpleInfo;

typedef struct {
    // 同时存在的流应当不会超过10个
    StreamSimpleInfo simpleInfos[CONCURRENT_ACTIVE_STREAM_MAX_NUM];
    StreamSimpleInfo curInputStreamInfo[MAX_STREAM_IN_NUM];
    float typeVolume[STREAM_TYPE_NUM];
    bool typeMute[STREAM_TYPE_NUM];
    bool masterMute;
    bool micMute;
    AudioRingMode ringMode;

    ListenersWrapper listeners;
    int32_t maxOutStreamId;
    int32_t maxInputStreamId;
} StreamManagerContext;

typedef enum {
    CREATE,
    DESTROY
} CurrentStage;

static StreamManagerContext g_streamMgrContext;
static bool g_inited = false;
static MediaMutexHandle g_mutex = NULL;

static StreamTypeHashId FindStreamTypeHashId(int32_t streamType);

inline static bool IsValidType(AudioStreamType streamType)
{
    for (int32_t i = 0; i < STREAM_TYPE_NUM; i++) {
        if (streamType == typeVolumes[i].streamType) {
            return true;
        }
    }
    return false;
}

static uint16_t GetTypeId(StreamTypeHashId typeHashId)
{
    // high 8 bit mod, low 8 bit typeHashId
    uint16_t typeId = 0;
    typeId = typeId | AUDIO_MOD_FOR_STORAGE;
    typeId = typeId + (uint16_t)typeHashId;
    return typeId;
}

static void GetValueByKey(uint8_t typeId)
{
    uint16_t readLen = 0;
    uint8_t value[MAX_FLOAT_TO_CHAR_NUM] = { 0 };
    // maybe many saving ways, eg kv, nv...
#if defined(SUPPORT_KV)
    UtilsSetEnv("/bin/vs/sd0p0");
    int32_t ret = UtilsGetValue(typeName[typeId], value, MAX_FLOAT_TO_CHAR_NUM);
    if (ret == EC_INVALID || ret == EC_FAILURE) {
        ALOGE("kv get value wrong, ret[%d]", ret);
        g_streamMgrContext.typeVolume[typeId] = GOLDEN_POINT;
    } else {
        ALOGI("kv read typeName[%s], typeVolume[%.3f]",
            typeName[typeId], g_streamMgrContext.typeVolume[typeId]);
        g_streamMgrContext.typeVolume[typeId] = strtof((char *)value, NULL) / THOUSAND;
    }
#elif defined(SUPPORT_NV)
    uint16_t key = GetTypeId(typeId);
    errcode_t nvRet = uapi_nv_read(key, MAX_FLOAT_TO_CHAR_NUM, &readLen, value);
    if (nvRet == ERRCODE_SUCC && readLen == MAX_FLOAT_TO_CHAR_NUM) {
        g_streamMgrContext.typeVolume[typeId] = strtof((char *)value, NULL) / THOUSAND;
        ALOGI("nv read typeName[%s], typeVolume[%.3f]",
            typeName[typeId], g_streamMgrContext.typeVolume[typeId]);
    } else {
        ALOGI("nv get value wrong, ret[%d]", nvRet);
        if (typeId == AUDIO_STREAM_VOICE_CALL_VOLTE_HASH_ID) {
            g_streamMgrContext.typeVolume[typeId] = MAX_VOLUME_POINT;
        } else {
            g_streamMgrContext.typeVolume[typeId] = GOLDEN_POINT;
        }
    }
#else
    ALOGI("not support key value read, just default value");
    g_streamMgrContext.typeVolume[typeId] = GOLDEN_POINT;
#endif
}

static int32_t WriteValueByKey(StreamTypeHashId typeHashId,  float typeVolume)
{
    ALOGI("typeName[%s], typeVolume[%.3f]", typeName[typeHashId], typeVolume);
    char typeVolumeStr[MAX_FLOAT_TO_CHAR_NUM] = { 0 };
    int32_t ret = sprintf_s(typeVolumeStr, MAX_FLOAT_TO_CHAR_NUM,
        "%d", (int32_t)(typeVolume * THOUSAND)); // int - str
    if (ret == AUDIO_ERROR) {
        return AUDIO_ERROR;
    }
#if defined(SUPPORT_KV)
    ret = UtilsSetValue(typeName[typeHashId], typeVolumeStr);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
#elif defined(SUPPORT_NV)
    uint16_t key = GetTypeId(typeHashId);
    errcode_t nvRet = uapi_nv_write(key, (uint8_t *)typeVolumeStr, MAX_FLOAT_TO_CHAR_NUM);
    if (nvRet != ERRCODE_SUCC) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
#else
    ALOGW("not support storage typeVolume[%f]", typeVolume);
    return AUDIO_ERROR;
#endif
}

static void InitTypeVolume(void)
{
    for (uint8_t i = 0; i < STREAM_TYPE_NUM; i++) {
        GetValueByKey(i);
    }
}

static int32_t RouteDeviceChange(int32_t deviceId)
{
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        bool valid = g_streamMgrContext.simpleInfos[i].active &&
            g_streamMgrContext.simpleInfos[i].streamId != -1;
        if (!valid) {
            continue;
        }
        // sco/fitness video no need change
        AudioStreamType streamType = g_streamMgrContext.simpleInfos[i].streamType;
        if (streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO ||
            streamType == AUDIO_STREAM_VOICE_CALL_VOLTE ||
            streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI ||
            streamType == AUDIO_STREAM_FITNESS_VIDEO ||
            streamType == AUDIO_STREAM_A2DP_MUSIC) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackOutputDevice(streamHandle, deviceId, NULL);
        CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "set output device failed");
    }
    return AUDIO_SUCCESS;
}

static int32_t PolicyCallBack(PolicyCallBackType type, uint32_t dataLen, const uint8_t *callbackData)
{
    AUDIO_UNUSED(dataLen);
    MediaMutexLock(g_mutex);
    switch (type) {
        case POLICY_ROUTE_DEVICE_CHANGED: {
            int32_t deviceId = *((int32_t*)callbackData);
            if (RouteDeviceChange(deviceId) != AUDIO_SUCCESS) {
                ALOGE("change route device err");
                MediaMutexUnLock(g_mutex);
                return AUDIO_ERROR;
            }
            break;
        }
        default: {
            ALOGE("invalid type");
            MediaMutexUnLock(g_mutex);
            return AUDIO_ERROR;
        }
    }
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static void InitTypeMute(void)
{
    for (int i = 0; i < STREAM_TYPE_NUM; i++) {
        g_streamMgrContext.typeMute[i] = false;
    }
}
static void InitInputStream(void)
{
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        g_streamMgrContext.curInputStreamInfo[i].streamId = -1;
        g_streamMgrContext.curInputStreamInfo[i].active = false;
    }
}
static void InitOutputStream(void)
{
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        g_streamMgrContext.simpleInfos[i].active = false;
        g_streamMgrContext.simpleInfos[i].streamId = -1;
    }
}
int32_t StreamMgrInit(void)
{
    ALOGI("StreamMgrInit IN");
    if (g_mutex == NULL) {
        MediaMutexGlobalLock();
        if (g_mutex == NULL) {
            g_mutex = MediaMutexCreate(NULL);
        }
        MediaMutexGlobalUnLock();
    }

    MediaMutexLock(g_mutex);
    if (g_inited) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_SUCCESS;
    }
    InitTypeVolume();

    const AudioPolicyCallback callback = { PolicyCallBack };
    int32_t ret = PolicyMgrRegisterPolicyCallback(&callback);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    g_streamMgrContext.maxOutStreamId = -1;
    g_streamMgrContext.maxInputStreamId = -1;
    InitInputStream();
    InitOutputStream();

    g_streamMgrContext.listeners.listenerPtr = NULL;
    g_streamMgrContext.listeners.curCapbility = 0;
    g_streamMgrContext.listeners.curNum = 0;
    g_streamMgrContext.ringMode = RINGER_MODE_NORMAL;
    g_streamMgrContext.micMute = false;
    g_streamMgrContext.masterMute = false;
    InitTypeMute();
    g_inited = true;
    MediaMutexUnLock(g_mutex);
    ALOGI("StreamMgrInit SUCCESS");
    return AUDIO_SUCCESS;
}

int32_t StreamMgrDeInit(void)
{
    // 如果有线程，这里销毁，其他资源也要释放
    MediaMutexLock(g_mutex);
    if (!g_inited) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    g_inited = false;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

static int32_t CheckUpdateListener(void)
{
    bool err = g_streamMgrContext.listeners.curCapbility == 0 &&
        g_streamMgrContext.listeners.listenerPtr != NULL;
    if (err) {
        return AUDIO_ERROR;
    }

    if (g_streamMgrContext.listeners.curCapbility == 0) {
        g_streamMgrContext.listeners.listenerPtr =
            (StreamChangeCallback*)malloc(MAX_LISTENER_NUM * sizeof(StreamChangeCallback));
        if (g_streamMgrContext.listeners.listenerPtr == NULL) {
            return AUDIO_ERROR;
        }
        g_streamMgrContext.listeners.curCapbility = MAX_LISTENER_NUM;
    }
    if (g_streamMgrContext.listeners.curNum + 1 == g_streamMgrContext.listeners.curCapbility) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamMgrSetCallback(const StreamChangeCallback *callback)
{
    CHK_NULL_RETURN(callback, AUDIO_ERROR, "StreamChangeCallback is NULL");
    MediaMutexLock(g_mutex);
    if (CheckUpdateListener() != AUDIO_SUCCESS) {
        ALOGE("check listener update fail");
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    int curIndex = g_streamMgrContext.listeners.curNum;
    g_streamMgrContext.listeners.listenerPtr[curIndex] = *callback;
    g_streamMgrContext.listeners.curNum++;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

static bool AnalyzeStrategyForDevice(const AudioStreamStrategy *strategy, int32_t newStreamId,
    uint32_t *defaultRouteDevice)
{
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        if (id == newStreamId) {
            *defaultRouteDevice = strategy->activeStreams[i]->currentRoutedDevices;
            return true;
        }
    }
    *defaultRouteDevice = 0;
    ALOGE("stream[%d] not find", newStreamId);
    return false;
}

static int32_t CheckTrackState(const AudioStreamStrategy *strategy, int32_t newStreamId)
{
    // 1. 如果不是独占情况，那只能是混音，在混音策略下，不必改变流的状态
    if (strategy->activeStreamNum > ONE_STREAM_EXCLUSIVE) {
        return AUDIO_SUCCESS;
    }
    // 2.1 如果是独占，独占肯定是新流独占，否则报错
    if (newStreamId != strategy->activeStreams[0]->streamDesc.streamIndex) {
        ALOGE("exclusive stream must be new stream[%d]", newStreamId);
        return AUDIO_ERROR;
    }
    // 2.2 如果是独占，独占的也是新流，那就检查当前老流中还running的，如果不符合那就pause
    //     此时，新流还未入档，g_streamMgrContext 里面记录的都还是老流
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (g_streamMgrContext.simpleInfos[i].active) {
            uintptr_t handle = g_streamMgrContext.simpleInfos[i].streamHandle;
            if (GetState(handle) == RUNNING && GetStreamType(handle) != AUDIO_STREAM_A2DP_MUSIC) {
                CHK_FAILED_RETURN(PauseTrack(handle), AUDIO_SUCCESS, AUDIO_ERROR, "pause failed");
            }
        }
    }
    return AUDIO_SUCCESS;
}

static AudioStreamStrategy *GetStrategyFromPolicyManager(const AudioStreamInfo *inf,
    uint32_t streamId, StreamStatus status)
{
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    float typeVol = g_streamMgrContext.typeVolume[typeHashId];
    AudioStreamDescriptor newStreamDesc = { streamId, inf->streamType, typeVol, inf->linkDir };
    ALOGI("streamId[%d] sessionId[%d] typeVol[%f]", streamId, inf->sessionID, typeVol);
    return PolicyMgrGetStrategyForStream(inf->sessionID, &newStreamDesc, status);
}

static void RecordOneStream(const AudioStreamInfo *inf, int32_t newStreamId, uintptr_t handle, bool isOutput)
{
    int i;
    int32_t streamNum;
    StreamSimpleInfo *curStreamInfo = NULL;
    streamNum = isOutput ? CONCURRENT_ACTIVE_STREAM_MAX_NUM : MAX_STREAM_IN_NUM;
    curStreamInfo = isOutput ? g_streamMgrContext.simpleInfos : g_streamMgrContext.curInputStreamInfo;
    for (i = 0; i < streamNum; i++) {
        bool isNotUsed = !curStreamInfo[i].active &&
                    curStreamInfo[i].streamId == -1;
        if (isNotUsed) {
            curStreamInfo[i].streamId = newStreamId;
            curStreamInfo[i].streamHandle = handle;
            curStreamInfo[i].streamType = inf->streamType;
            curStreamInfo[i].active = true;
            curStreamInfo[i].linkDir = inf->linkDir;
            break;
        }
    }

    if (i == streamNum) {
        ALOGW("there no free record for id[%d]", newStreamId);
    }

    return;
}

static void DeleteOneStream(int32_t streamId, bool isOutput)
{
    int i;
    int32_t streamNum;
    StreamSimpleInfo *curStreamInfo = NULL;
    streamNum = isOutput ? CONCURRENT_ACTIVE_STREAM_MAX_NUM : MAX_STREAM_IN_NUM;
    curStreamInfo = isOutput ? g_streamMgrContext.simpleInfos : g_streamMgrContext.curInputStreamInfo;
    for (i = 0; i < streamNum; i++) {
        bool isMatch = curStreamInfo[i].streamId == streamId &&
                    curStreamInfo[i].active;
        if (isMatch) {
            curStreamInfo[i].streamId = -1;
            curStreamInfo[i].active = false;
            break;
        }
    }

    if (i == streamNum) {
        ALOGW("there no match record for id[%d]", streamId);
    }

    return;
}

static void NotifyListenersStreamCreated(int newStreamId, AudioSession sessionId)
{
    // 可能有多个观察者，例如 AudioService，AS 想要了解流的变化
    int32_t listernerNum = g_streamMgrContext.listeners.curNum;
    for (int i = 0; i < listernerNum; i++) {
        uintptr_t upperInstance = g_streamMgrContext.listeners.listenerPtr[i].instance;
        if (g_streamMgrContext.listeners.listenerPtr[i].OnEvent != NULL) {
            g_streamMgrContext.listeners.listenerPtr[i].OnEvent(upperInstance,
                EventCreated, (uint32_t)newStreamId, sessionId, (uintptr_t)NULL);
        }
    }
    return;
}

static void NotifyListenersStreamDestroy(int newStreamId, AudioSession sessionId)
{
    // 可能有多个观察者，例如 AudioService，AS 想要了解流的变化
    int32_t listernerNum = g_streamMgrContext.listeners.curNum;
    for (int i = 0; i < listernerNum; i++) {
        uintptr_t upperInstance = g_streamMgrContext.listeners.listenerPtr[i].instance;
        if (g_streamMgrContext.listeners.listenerPtr[i].OnEvent != NULL) {
            g_streamMgrContext.listeners.listenerPtr[i].OnEvent(upperInstance,
                EventDestroied, (uint32_t)newStreamId, sessionId, (uintptr_t)NULL);
        }
    }
    return;
}

static bool GetMixWeightForNewStream(const AudioStreamStrategy *strategy, int32_t newStreamId,
    float *mixWeight)
{
    // 已经保证非NULL
    // 1.如果是独占，看master，mixWeight 是 1
    // 2.如果是混音，看slave，通过newStreamId找到对应的
    // 3.其他，这是不合理的
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        if (id == newStreamId) {
            *mixWeight = strategy->activeStreams[i]->mixWeight;
            return true;
        }
    }
    return false;
}

static bool IsAlreadyRecorded(int32_t streamId, uintptr_t *streamHandle, AudioStreamType *streamType)
{
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!g_streamMgrContext.simpleInfos[i].active) {
            continue;
        }
        if (streamId == g_streamMgrContext.simpleInfos[i].streamId) {
            *streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
            *streamType = g_streamMgrContext.simpleInfos[i].streamType;
            return true;
        }
    }
    return false;
}

static int32_t UpdateActiveStreamMixWeight(const AudioStreamStrategy *strategy, CurrentStage stage)
{
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        ALOGI("mix num[%u] i[%d] id[%d]", strategy->activeStreamNum, i, id);
        uintptr_t streamHandle;
        AudioStreamType streamType;
        if (!IsAlreadyRecorded(id, &streamHandle, &streamType)) {
            if (stage == CREATE) {
                continue; // 创建阶段, active stream 可能有新流，新流不在记录中
            } else {
                return AUDIO_ERROR; // 销毁阶段, active stream 必须在记录中
            }
        }
        float mixWeightSet = strategy->activeStreams[i]->mixWeight;
        float curMixWeight = 0;
        GetStreamMixWeight(streamHandle, &curMixWeight);
        if (fabs(curMixWeight - mixWeightSet) >= COMPARE_THRESHOLD) {
            // 先更新value
            SetStreamMixWeight(streamHandle, mixWeightSet);
            // 再作用到hal
            StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
            if (typeHashId == AUDIO_STREAM_TYPE_HASH_ID_MAX) {
                ALOGE("streamtype hash id is inValid");
                return AUDIO_ERROR;
            }
            int32_t ret = SetTrackVolume(streamHandle, g_streamMgrContext.typeVolume[typeHashId],
                mixWeightSet, NOT_SET_STREAM_VOLUME);
            // 解决多线程竞争，概率出现老流未启动设置混音失败，此时新流也会失败退出，
            // 老流会在启动的时候设置音量，此处设置失败不应该直接向上返回错误，
            // 后续要考虑下老流设置音箱的时机，此处是不是只记录音量信息即可
            if (ret == AUDIO_BAD_STATE) {
                ALOGW("current state is not right!");
                return AUDIO_SUCCESS;
            }
            if (ret != AUDIO_SUCCESS) {
                ALOGW("update stream volume fail when new stream create: %d", ret);
                return AUDIO_ERROR;
            }
        }
    }
    return AUDIO_SUCCESS;
}

// 1.创建流下更新和销毁流下更新，这两个情况是不同的，
// 例如，创建流时，如果是独占策略，肯定是新流是独占
// 销毁流时，如果是独占策略，是老流独占
// 2.strategy 信息无法完整描述记录中流的情况
// 例如，当前有两个流在混音，ring流过来，要独占，那么策略返回的
// 只会有ring流信息，此时发生在ring流创建的时候；
static int32_t UpdateTrackMixWeightWhenCreate(const AudioStreamStrategy *strategy)
{
    // 独占策略下，strategy只会返回新流情况，无需更新老流权重，直接返回即可
    if (strategy->currentStrategy == STRATEGY_EXCLUSIVE) {
        return AUDIO_SUCCESS;
    }

    // 到此，必须是混音策略，其他策略是非法的
    if (strategy->currentStrategy != STRATEGY_MIX) {
        return AUDIO_ERROR;
    }
    return UpdateActiveStreamMixWeight(strategy, CREATE);
}

static int32_t UpdateTrackMixWeightWhenDestroy(const AudioStreamStrategy *strategy)
{
    // 只有混音情况下，才需要对老流的权重进行更新
    if (strategy->currentStrategy == STRATEGY_NONE) {
        return AUDIO_SUCCESS;
    }

    return UpdateActiveStreamMixWeight(strategy, DESTROY);
}

static bool IsCurrentStreamMute(AudioStreamType streamType)
{
    // 当前该类型状态已经是设置状态，无需往下走
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    return g_streamMgrContext.typeMute[typeHashId];
}

static bool IsAllStreamDestroyed(AudioStreamType streamType)
{
    if (streamType != AUDIO_STREAM_VOICE_CALL_BT_SCO &&
        streamType != AUDIO_STREAM_VOICE_CALL_VOIP) {
        return true;
    }
    for (int32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.simpleInfos[i];
        if (curStreamInfo.streamType != streamType) {
            continue;
        }
        if (curStreamInfo.active) {
            return false;
        }
    }
    for (int32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (curStreamInfo.streamType != streamType) {
            continue;
        }
        if (curStreamInfo.active) {
            return false;
        }
    }
    return true;
}

uintptr_t StreamMgrCreateStreamOut(const AudioStreamInfo *inf)
{
    CHK_NULL_RETURN(inf, (uintptr_t)NULL, "input arg inf is NULL");
    CHK_FAILED_RETURN(IsValidType(inf->streamType), true, (uintptr_t)NULL, "invalid stream type");
    MediaMutexLock(g_mutex);
    // 1. streamId 成功后才会更新maxId
    int32_t curMaxId = g_streamMgrContext.maxOutStreamId;
    int32_t newStreamId = ++curMaxId;
    // 2. 获取到策略，包括默认输出设备，混音比重等
    AudioStreamStrategy *strategy = GetStrategyFromPolicyManager(inf, (uint32_t)newStreamId, STREAM_OUT_CREATE);
    if (strategy == NULL || strategy->currentStrategy == STRATEGY_NONE) {
        ALOGE("get strategy fail or invalid strategy when create out stream");
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 3. 从策略中获取默认 device
    uint32_t defaultRouteDevice = 0;
    if (!AnalyzeStrategyForDevice(strategy, newStreamId, &defaultRouteDevice)) {
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 4. 创建 handle context
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    VolumeInf volInf = {IsCurrentStreamMute(inf->streamType), g_streamMgrContext.typeVolume[typeHashId]};
    uintptr_t handle = CreateTrackHandle(newStreamId, inf, defaultRouteDevice, volInf);
    if (handle == 0) {
        ALOGE("create track fail");
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    float mixWgt = 0;
    // 5. 创建成功后，根据策略对老流状态进行审视，以及老流混音比重的更新
    if (CheckTrackState(strategy, newStreamId) != AUDIO_SUCCESS) {
        ALOGE("CheckTrackState fail");
        goto ERR;
    }
    if (UpdateTrackMixWeightWhenCreate(strategy) != AUDIO_SUCCESS) {
        ALOGE("UpdateTrackMixWeight fail");
        goto ERR;
    }
    // 6. 获取到新流的混音比重
    if (!GetMixWeightForNewStream(strategy, newStreamId, &mixWgt)) {
        ALOGE("analysize strategy for mix may be wrong");
        goto ERR;
    }
    // 7. 上面都正常处理了，该流计入管理
    // 要根据新策略信息更新整个流记录，而非只是增加新流，新流的介入对老流肯定是有影响的
    SetStreamMixWeight(handle, mixWgt);
    RecordOneStream(inf, newStreamId, handle, true);
    g_streamMgrContext.maxOutStreamId++;
    // 8. 通知外界流变化-流创建
    NotifyListenersStreamCreated(newStreamId, inf->sessionID);
    MediaMutexUnLock(g_mutex);
    return handle;
ERR:
    MediaMutexUnLock(g_mutex);
    free((void*)handle);
    return (uintptr_t)NULL;
}

int32_t StreamMgrDestroyStreamOut(uintptr_t handle)
{
    CHK_NULL_RETURN((void*)handle, AUDIO_ERROR, "handle is NULL when destroy");
    int32_t streamId;
    AudioSession sessionId;
    int32_t ret = GetHandleId(handle, &streamId, &sessionId);
    CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "get handle failed");
    MediaMutexLock(g_mutex);
    // 告知策略模块即将清掉的流
    AudioStreamInfo streamInfForDestroy = { 0 };
    streamInfForDestroy.streamType = GetStreamType(handle);
    streamInfForDestroy.sessionID = sessionId;
    AudioStreamStrategy *strategy =
        GetStrategyFromPolicyManager(&streamInfForDestroy, (uint32_t)streamId, STREAM_OUT_DESTROY);
    if (strategy == NULL) {
        ALOGE("get strategy fail when destroy out stream");
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    AudioStreamType streamType = GetStreamType(handle);
    AudioDeviceType deviceType = (AudioDeviceType)GetStreamDeviceId(handle);
#endif
    // hal 销毁
    if (DestroyTrackHandle(handle) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    // 更新的话，先把流从管理中删除
    // 然后根据新的策略，把老流的信息该更新了
    // 通知外界流销毁了
    DeleteOneStream(streamId, true);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    if (IsAllStreamDestroyed(streamType)) {
        SetHifiClockFreqByStreamTypeAndDeviceType(streamType, deviceType, false);
    }
#endif
    if (UpdateTrackMixWeightWhenDestroy(strategy) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        ALOGI("UpdateTrackMixWeightWhenDestroy sessionId:%u failed", sessionId);
        return AUDIO_ERROR;
    }
    NotifyListenersStreamDestroy(streamId, sessionId);
    MediaMutexUnLock(g_mutex);
    ALOGI("StreamMgrDestroyStreamOut sessionId:%u SUCCESS", sessionId);
    return AUDIO_SUCCESS;
}

uintptr_t StreamMgrCreateStreamIn(const AudioStreamInfo *inf)
{
    CHK_NULL_RETURN(inf, (uintptr_t)NULL, "input arg inf is NULL");
    MediaMutexLock(g_mutex);
    // 0. streamId 成功后才会更新maxId
    int32_t curMaxId = g_streamMgrContext.maxInputStreamId;
    int32_t newStreamId = ++curMaxId;
    // 1.先通过PM，获取策略和路由
    // a.策略决定要不要继续
    AudioStreamStrategy *strategy = GetStrategyFromPolicyManager(inf, (uint32_t)newStreamId, STREAM_IN_CREATE);
    if (strategy == NULL ||
        strategy->currentStrategy == STRATEGY_REJECT ||
        strategy->currentStrategy == STRATEGY_NONE) {
        ALOGE("this new input stream is invalid, goodbye newer");
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // b.获取路由设备，可能是MIC，也可能是SCO-IN
    // todo 如果出现失败，要不要告知下PM?
    uint32_t defaultRouteDevice = 0;
    if (!AnalyzeStrategyForDevice(strategy, newStreamId, &defaultRouteDevice)) {
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 2. 创建 handle context
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    VolumeInf volInf = { g_streamMgrContext.micMute, 0.0 };
    if (inf->linkDir == AUDIO_DOWN_LINK) {
        volInf.streamMute = false;
        volInf.typeVolume = g_streamMgrContext.typeVolume[typeHashId];
    }
    uintptr_t handle = CreateSourceHandle(newStreamId, inf, defaultRouteDevice, volInf);
    if (handle != 0) {
        // 3. 上面都成功后，记录并增加maxId
        RecordOneStream(inf, newStreamId, handle, false);
        g_streamMgrContext.maxInputStreamId++;
    }
    MediaMutexUnLock(g_mutex);
    return handle;
}

int32_t StreamMgrDestroyStreamIn(uintptr_t handle)
{
    MediaMutexLock(g_mutex);
    int32_t streamId = GetSourceStreamId(handle);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    AudioStreamType streamType = GetSourceStreamType(handle);
    AudioDeviceType deviceType = (AudioDeviceType)GetSourceDeviceId(handle);
#endif
    if (DestroySourceHandle(handle) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    DeleteOneStream(streamId, false);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    if (IsAllStreamDestroyed(streamType)) {
        SetHifiClockFreqByStreamTypeAndDeviceType(streamType, deviceType, false);
    }
#endif
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static bool IsValidVolume(float typeVolume)
{
    return typeVolume >= 0.0 && typeVolume <= 1.0;
}

inline static bool IsValidStream(int32_t streamId, bool active)
{
    return streamId != -1 && active;
}

inline static bool IsRingModeControlStream(AudioStreamType streamType)
{
    return streamType == AUDIO_STREAM_RING || streamType == AUDIO_STREAM_ALARM_CLOCK ||
        streamType == AUDIO_STREAM_NOTIFICATION_SYSTEM;
}

static int32_t StreamMgrSetTypeVolumeInner(int32_t streamType, float typeVolume,
    StreamSimpleInfo simpleInfos[], uint32_t infoSize)
{
    bool findType = false;
    for (uint32_t i = 0; i < infoSize; i++) {
        StreamSimpleInfo *curInfo = &(simpleInfos[i]);
        if (curInfo->streamType != (AudioStreamType)streamType) {
            continue;
        }
        if (!IsValidStream(curInfo->streamId, curInfo->active)) {
            continue;
        }
        findType = true;
        uintptr_t streamHandle = curInfo->streamHandle;
        if (streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
            int32_t ret = SetSourceVolume(streamHandle, typeVolume);
            if (ret == AUDIO_ERROR) {
                return AUDIO_ERROR;
            }
        } else {
            // 获取混音比例，这个是不是也应该放在 trackcontext 更合适，目前先这么实现
            float mixWeight;
            if (!GetStreamMixWeight(streamHandle, &mixWeight)) {
                return AUDIO_ERROR;
            }
            int32_t ret = SetTrackVolume(streamHandle, typeVolume, mixWeight, NOT_SET_STREAM_VOLUME);
            if (ret == AUDIO_ERROR) {
                return AUDIO_ERROR;
            }
        }
    }
    if (!findType) {
        ALOGI("there is no [%X]-type stream in stream infos", streamType);
    }
    return AUDIO_SUCCESS;
}

int32_t StreamMgrSetTypeVolume(int32_t streamType, float typeVolume)
{
    if (!IsValidType(streamType) || !IsValidVolume(typeVolume)) {
        ALOGE("set type volume invalid param");
        return AUDIO_ERROR;
    }
    MediaMutexLock(g_mutex);
    // 找到对应类型的流，分别获取流音量，计算后设置下去
    // 如果当前没有该类型的流，不用报错
    if (streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        StreamMgrSetTypeVolumeInner(streamType, typeVolume,
            g_streamMgrContext.curInputStreamInfo, MAX_STREAM_IN_NUM);
    } else {
        StreamMgrSetTypeVolumeInner(streamType, typeVolume,
            g_streamMgrContext.simpleInfos, CONCURRENT_ACTIVE_STREAM_MAX_NUM);
    }
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    g_streamMgrContext.typeVolume[typeHashId] = typeVolume;
    MediaMutexUnLock(g_mutex);

    int32_t ret = WriteValueByKey(typeHashId, typeVolume);
    if (ret != AUDIO_SUCCESS) {
        ALOGW("set value fail[%d] type[%d] value[%f]", ret, streamType, typeVolume);
    }
    return AUDIO_SUCCESS;
}

static StreamTypeHashId FindStreamTypeHashId(int32_t streamType)
{
    AudioStreamType type = (AudioStreamType)streamType;
    for (int i = 0; i < STREAM_TYPE_NUM; i++) {
        if (type == typeVolumes[i].streamType) {
            return typeVolumes[i].typeId;
        }
    }
    return AUDIO_STREAM_INVALID_TYPE_HASH_ID;
}

bool StreamMgrGetTypeVolume(int32_t streamType, float *typeVolume)
{
    // 校验下参数
    CHK_NULL_RETURN(typeVolume, false, "type volume ptr is NULL");
    if (!IsValidType(streamType)) {
        ALOGE("invalid stream type[%d]", streamType);
        return false;
    }
    // hash 方式来处理
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    *typeVolume = g_streamMgrContext.typeVolume[typeHashId];
    return true;
}

int32_t StreamMgrSetMute(AudioStreamType streamType, bool mute)
{
    // 校验下参数
    if (!IsValidType(streamType)) {
        ALOGE("set mute invalid stream type[%d]", streamType);
        return AUDIO_ERROR;
    }

    MediaMutexLock(g_mutex);
    // 当前该类型状态已经是设置状态，无需往下走
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    if (mute == g_streamMgrContext.typeMute[typeHashId]) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_SUCCESS;
    }

    // 找到对应类型的流，分别获取流音量，计算后设置下去
    bool findType = false;
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        if (g_streamMgrContext.simpleInfos[i].streamType != (AudioStreamType)streamType) {
            continue;
        }
        findType = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        if (SetTrackMute(streamHandle, mute) == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            return AUDIO_ERROR;
        }
    }
    if (!findType) {
        ALOGI("there is no [%d]-type stream in stream infos", streamType);
    }
    // 成功就设置下来
    g_streamMgrContext.typeMute[typeHashId] = mute;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

bool StreamMgrIsMute(AudioStreamType streamType)
{
    // 校验下参数
    if (!IsValidType(streamType)) {
        ALOGE("invalid stream type[%d]", streamType);
        return false;
    }

    // hash 方式来处理
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    MediaMutexLock(g_mutex);
    bool isTypeMute = g_streamMgrContext.typeMute[typeHashId];
    MediaMutexUnLock(g_mutex);
    return isTypeMute;
}

bool StreamMgrSetMasterMute(bool isMute)
{
    MediaMutexLock(g_mutex);
    // 不做阻拦，考虑场景：1.全局mute 2.A类型解mute 3.全局在mute
    // 找到有效的流
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        // 要先判断有效
        if (!g_streamMgrContext.simpleInfos[i].active || g_streamMgrContext.simpleInfos[i].streamId == -1) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackMute(streamHandle, isMute);
        if (ret == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            return false;
        }
    }
    // 成功才会设置下来，全type都要禁
    for (int i = 0; i < STREAM_TYPE_NUM; ++i) {
        g_streamMgrContext.typeMute[i] = isMute;
    }
    g_streamMgrContext.masterMute = isMute;
    MediaMutexUnLock(g_mutex);
    return true;
}

bool StreamMgrIsMasterMute(void)
{
    MediaMutexLock(g_mutex);
    bool isMasterMute = g_streamMgrContext.masterMute;
    MediaMutexUnLock(g_mutex);
    return isMasterMute;
}

int32_t StreamMgrDumpInfo(AudioStreamDebugInfo *audioStreamInfo)
{
    CHK_NULL_RETURN(audioStreamInfo, AUDIO_ERROR, "audioStreamInfo is NULL");
    MediaMutexLock(g_mutex);
    uint32_t index = 0;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        audioStreamInfo->allstreamInfo[index] = DumpTrackInfo(streamHandle);
        index++;
        if (index >= MAX_INST_NUM) {
            break;
        }
    }

    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (index >= MAX_INST_NUM) {
            break;
        }
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        audioStreamInfo->allstreamInfo[index] = DumpSourceInfo(streamHandle);
        index++;
    }
    audioStreamInfo->streamInfoCnt = index;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static bool IsValidRingMode(AudioRingMode ringMode)
{
    return ringMode >= RINGER_MODE_NORMAL && ringMode <= RINGER_MODE_SILENT;
}

static bool SetRingTypeMute(bool setMute)
{
    const AudioStreamType ringControlTypes[] = { AUDIO_STREAM_RING,
                                                 AUDIO_STREAM_ALARM_CLOCK,
                                                 AUDIO_STREAM_NOTIFICATION_SYSTEM };
    size_t size = sizeof(ringControlTypes) / sizeof(ringControlTypes[0]);
    for (size_t i = 0; i < size; i++) {
        StreamTypeHashId typeHashId = FindStreamTypeHashId(ringControlTypes[i]);
        if (typeHashId == AUDIO_STREAM_TYPE_HASH_ID_MAX) {
            return false;
        }
        g_streamMgrContext.typeMute[typeHashId] = setMute;
    }
    return true;
}

bool StreamMgrSetRingerMode(int mode)
{
    AudioRingMode ringMode = (AudioRingMode)mode;
    if (!IsValidRingMode(ringMode)) {
        return false;
    }

    bool setMute = false;
    if (mode == RINGER_MODE_SILENT || mode == RINGER_MODE_VIBRATE) {
        setMute = true;
    }
    if (mode == RINGER_MODE_NORMAL) {
        setMute = false;
    }
    MediaMutexLock(g_mutex);
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId, g_streamMgrContext.simpleInfos[i].active) ||
            !IsRingModeControlStream(g_streamMgrContext.simpleInfos[i].streamType)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackMute(streamHandle, setMute);
        if (ret == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            return false;
        }
    }
    // 成功后设置
    g_streamMgrContext.ringMode = ringMode;
    bool ret = SetRingTypeMute(setMute);
    MediaMutexUnLock(g_mutex);
    return ret;
}

int32_t StreamMgrGetRingerMode(void)
{
    MediaMutexLock(g_mutex);
    AudioRingMode ringMode = g_streamMgrContext.ringMode;
    MediaMutexUnLock(g_mutex);
    return ringMode;
}

bool StreamMgrSetMicrophoneMute(bool isMute)
{
    MediaMutexLock(g_mutex);
    if (isMute == g_streamMgrContext.micMute) {
        MediaMutexUnLock(g_mutex);
        return true;
    }

    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        if (g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_RECORD ||
            ((g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO ||
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_VOLTE ||
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI) &&
            g_streamMgrContext.curInputStreamInfo[i].linkDir == AUDIO_UP_LINK)) {
            uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
            int32_t ret = SetSourceMute(streamHandle, isMute);
            if (ret == AUDIO_ERROR) {
                ALOGE("SetSourceMute failed ret:%d", ret);
                MediaMutexUnLock(g_mutex);
                return false;
            }
            break;
        }
    }
    g_streamMgrContext.micMute = isMute;
    MediaMutexUnLock(g_mutex);
    return true;
}

bool StreamMgrIsMicrophoneMute(void)
{
    MediaMutexLock(g_mutex);
    bool isMute = g_streamMgrContext.micMute;
    MediaMutexUnLock(g_mutex);
    return isMute;
}

static int32_t StreamManagerDeactivateCorrespondingInputStream(AudioSession sessionID)
{
    uint32_t i;
    for (i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        AudioSession sessionIdTemp = AUDIO_SESSION_ID_NONE;
        int32_t ret = GetSourceSessionId(g_streamMgrContext.curInputStreamInfo[i].streamHandle, &sessionIdTemp);
        if (ret != AUDIO_SUCCESS) {
            continue;
        }
        if (sessionIdTemp == sessionID) {
            if (DeactivateCorrespondingSource(streamHandle) != AUDIO_SUCCESS) {
                return AUDIO_ERROR;
            }
            break;
        }
    }
    if (i == MAX_STREAM_IN_NUM) {
        ALOGI("not find match in stream for session[%u]", sessionID);
        return AUDIO_ERROR;
    }

    return AUDIO_SUCCESS;
}

static int32_t StreamManagerDeactivateCorrespondingOutputStream(AudioSession sessionID)
{
    // ?????????а????????? sId????????????ó???Ч????(?????????????У??????)
    uint32_t i;
    for (i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t streamIdTemp = 0;
        AudioSession sessionIdTemp = AUDIO_SESSION_ID_NONE;
        int32_t ret = GetHandleId(streamHandle, &streamIdTemp, &sessionIdTemp);
        CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "get handle failed");
        if (sessionID == sessionIdTemp) {
            if (DeactivateCorrespondingTrack(streamHandle) != AUDIO_SUCCESS) {
                ALOGI("no corresponding track[sessionID(%u)]", sessionID);
                return AUDIO_ERROR;
            }
            break;
        }
    }
    if (i == CONCURRENT_ACTIVE_STREAM_MAX_NUM) {
        ALOGI("not find match out stream for session[%u]", sessionID);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamManagerDeactivateCorrespondingStream(AudioSession sessionID, AudioStreamType streamType)
{
    MediaMutexLock(g_mutex);
    int32_t ret;
    if (streamType == AUDIO_STREAM_VOICE_RECORD) {
        ret = StreamManagerDeactivateCorrespondingInputStream(sessionID);
    } else if (streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO ||
        streamType == AUDIO_STREAM_VOICE_CALL_VOLTE ||
        streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI ||
        streamType == AUDIO_STREAM_A2DP_MUSIC) {
        ret = StreamManagerDeactivateCorrespondingInputStream(sessionID);
        ret = StreamManagerDeactivateCorrespondingOutputStream(sessionID);
    } else if (streamType == AUDIO_STREAM_ALARM_SYSTEM || streamType == AUDIO_STREAM_ALARM_CLOCK ||
               streamType == AUDIO_STREAM_RING || streamType == AUDIO_STREAM_NOTIFICATION_PROMPT ||
               streamType == AUDIO_STREAM_MUSIC) {
        ret = StreamManagerDeactivateCorrespondingOutputStream(sessionID);
    } else {
        ALOGW("should not be here");
        ret = AUDIO_ERROR;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

static int32_t GetStreamOutParamValue(char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
            g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uint32_t deviceId = GetStreamDeviceId(g_streamMgrContext.simpleInfos[i].streamHandle);
        if (deviceId == OUT_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = GetTrackParam(streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get track param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGI("not find match out stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t SetStreamOutParamValue(const char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
            g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uint32_t deviceId = GetStreamDeviceId(g_streamMgrContext.simpleInfos[i].streamHandle);
        if (deviceId == OUT_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackParam(streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set track param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match out stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t GetStreamInParamValue(char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (!IsValidStream(curStreamInfo.streamId, curStreamInfo.active)) {
            continue;
        }
        uint32_t deviceId = GetSourceDeviceId(curStreamInfo.streamHandle);
        if (deviceId == IN_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        int32_t ret = GetSourceParam(curStreamInfo.streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get source param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match in stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t SetStreamInParamValue(const char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (!IsValidStream(curStreamInfo.streamId, curStreamInfo.active)) {
            continue;
        }
        uint32_t deviceId = GetSourceDeviceId(curStreamInfo.streamHandle);
        if (deviceId == IN_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        int32_t ret = SetSourceParam(curStreamInfo.streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set source param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match in stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamManagerSetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    int32_t ret = AUDIO_SUCCESS;
    MediaMutexLock(g_mutex);
    switch (direction) {
        case AUDIO_DOWN_LINK:
            ret = SetStreamOutParamValue(param, len);
            break;
        case AUDIO_UP_LINK:
            ret = SetStreamInParamValue(param, len);
            break;
        default:
            ALOGI("not find match direction!");
            return AUDIO_ERROR;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

int32_t StreamManagerGetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    int32_t ret = AUDIO_SUCCESS;
    MediaMutexLock(g_mutex);
    switch (direction) {
        case AUDIO_DOWN_LINK:
            ret = GetStreamOutParamValue(param, len);
            break;
        case AUDIO_UP_LINK:
            ret = GetStreamInParamValue(param, len);
            break;
        default:
            ALOGE("not find match direction!");
            return AUDIO_ERROR;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

bool StreamMgrIsCreateStreamByType(AudioStreamType streamType)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (curStreamInfo.streamType == streamType &&
            curStreamInfo.streamId != -1) {
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
            StreamSimpleInfo curStreamInfo = g_streamMgrContext.simpleInfos[i];
            if (curStreamInfo.streamType == streamType &&
                curStreamInfo.streamId != -1) {
                isFound = true;
                break;
            }
        }
    }
    return isFound;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

