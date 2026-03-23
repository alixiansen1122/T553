/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio internal
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_INTERNAL_H
#define AUDIO_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "audio_manager.h"
#include "securec.h"
#include "audio_port_plugin.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#define MODULE_NAME "AudioHal"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define AUDIO_STR_LEN_MAX 64
#define MAX_CHN_NUM_PER_STREAM 2

#define MAX_CAPTURE_INSTANCE_NUM 2
#define MAX_RENDER_INSTANCE_NUM 4

/* support both audio input and output */
#define MAX_ADAPTER_PORTS_NUM 2

#define AUDIO_LEFT_CHN 0
#define AUDIO_RIGHT_CHN 1
#define AUDIO_DEFAULT_VOLUME 1.0f

#define MODULE_ID(deviceid) \
    ((unsigned int)(((deviceid) >> 16)&0xFF))

#define DEVICE(deviceid) \
    ((unsigned int)(((deviceid) >> 8)&0xFF))

#define CHANNEL_ID(deviceid) \
    ((unsigned int)((deviceid)&0xFF))

struct AdapterChnStatus {
    bool used;
    uint32_t chnId;
};

struct AudioHwAdapter {
    struct AudioAdapter common;
    bool portOpened;
    AudioHandle portHandle;
    enum AudioPortDirection portDirection;
    const AudioPortPlugin *portPlugin;
    struct AdapterChnStatus capture[MAX_CAPTURE_INSTANCE_NUM];
    struct AdapterChnStatus render[MAX_RENDER_INSTANCE_NUM];
    struct AudioAdapterDescriptor adapterDescriptor;
    struct AudioPortCapability *capability;
    enum AudioPortPassthroughMode mode;
    struct AudioSampleAttributes lastAttrs;
    MediaHalMutexHandle mutex;
};

struct AudioHwRender {
    struct AudioRender common;
    AudioPortTrack *trackHandle;
    bool opened;
    bool started;
    bool paused;
    bool steroToMono;
    float speed;
    uint32_t chnId;
    RenderCallback renderCallback;
    void *caller;
    struct AudioDeviceDescriptor deviceDesc;
    struct AudioSampleAttributes attrs;
    enum AudioChannelMode mode;
    MediaHalMutexHandle mutex;
};

struct AudioHwCapture {
    struct AudioCapture common;
    AudioPortTrack *trackHandle;
    bool started;
    float gain;
    bool interleaved;
    uint32_t chnId;
    struct AudioDeviceDescriptor deviceDesc;
    enum AudioCategory streamUsage;
    uint64_t frames;
    struct AudioTimeStamp time;
    struct AudioSampleAttributes attrs;
    MediaHalMutexHandle mutex;
};

bool AudioManagerIsValidAdapter(const struct AudioAdapter *adapter);

int32_t AudioAdapterInitAllPorts(struct AudioAdapter *adapter);
int32_t AudioAdapterClosePort(struct AudioAdapter *adapter);

int32_t AudioAdapterCreateRender(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
    const struct AudioSampleAttributes *attrs, struct AudioRender **render);
int32_t AudioAdapterDestroyRender(struct AudioAdapter *adapter, struct AudioRender *render);
int32_t AudioAdapterCreateCapture(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
    const struct AudioSampleAttributes *attrs, struct AudioCapture **capture);
int32_t AudioAdapterDestroyCapture(struct AudioAdapter *adapter, struct AudioCapture *capture);
int32_t AudioAdapterGetPortCapability(struct AudioAdapter *adapter, const struct AudioPort *port,
    struct AudioPortCapability *capability);
int32_t AudioAdapterSetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
    enum AudioPortPassthroughMode mode);
int32_t AudioAdapterGetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
    enum AudioPortPassthroughMode *mode);
int32_t AudioAdapterSetExtraParams(struct AudioAdapter* adapter, enum AudioExtParamKey key,
    const char* condition, const char* value);
int32_t AudioAdapterGetExtraParams(struct AudioAdapter* adapter, enum AudioExtParamKey key,
    const char* condition, char* value, uint32_t* length);
int32_t AudioAdapterRegExtraParamObserver(struct AudioAdapter *adapter, ParamCallback callback, void *cookie);

int32_t AudioRenderStart(AudioHandle handle);
int32_t AudioRenderStop(AudioHandle handle);
int32_t AudioRenderPause(AudioHandle handle);
int32_t AudioRenderResume(AudioHandle handle);
int32_t AudioRenderFlush(AudioHandle handle);
int32_t AudioRenderGetFrameSize(AudioHandle handle, uint64_t *size);
int32_t AudioRenderGetFrameCount(AudioHandle handle, uint64_t *count);
int32_t AudioRenderSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs);
int32_t AudioRenderGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs);
int32_t AudioRenderGetCurrentChannelId(AudioHandle handle, uint32_t *channelId);
int32_t AudioRenderSetExtraParams(AudioHandle handle, const char *keyValueList);
int32_t AudioRenderGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth);
int32_t AudioRenderCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
    bool *supported);
int32_t AudioRenderSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene);
int32_t AudioRenderSetMute(AudioHandle handle, bool mute);
int32_t AudioRenderGetMute(AudioHandle handle, bool *mute);
int32_t AudioRenderSetVolume(AudioHandle handle, float volume);
int32_t AudioRenderGetVolume(AudioHandle handle, float *volume);
int32_t AudioRenderGetGainThreshold(AudioHandle handle, float *min, float *max);
int32_t AudioRenderGetGain(AudioHandle handle, float *gain);
int32_t AudioRenderSetGain(AudioHandle handle, float gain);
int32_t AudioRenderGetLatency(struct AudioRender *render, uint32_t *ms);
int32_t AudioRenderRenderFrame(struct AudioRender *render, const void *frame,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t AudioRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioRenderSetRenderSpeed(struct AudioRender *render, float speed);
int32_t AudioRenderGetRenderSpeed(struct AudioRender *render, float *speed);
int32_t AudioRenderSetChannelMode(struct AudioRender *render, enum AudioChannelMode mode);
int32_t AudioRenderGetChannelMode(struct AudioRender *render, enum AudioChannelMode *mode);

int32_t AudioCaptureStart(AudioHandle handle);
int32_t AudioCaptureStop(AudioHandle handle);
int32_t AudioCapturePause(AudioHandle handle);
int32_t AudioCaptureResume(AudioHandle handle);
int32_t AudioCaptureFlush(AudioHandle handle);
int32_t AudioCaptureGetFrameSize(AudioHandle handle, uint64_t *size);
int32_t AudioCaptureGetFrameCount(AudioHandle handle, uint64_t *count);
int32_t AudioCaptureSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs);
int32_t AudioCaptureGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs);
int32_t AudioCaptureGetCurrentChannelId(AudioHandle handle, uint32_t *channelId);
int32_t AudioCaptureSetExtraParams(AudioHandle handle, const char *keyValueList);
int32_t AudioCaptureGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth);
int32_t AudioCaptureCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
    bool *supported);
int32_t AudioCaptureSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene);
int32_t AudioCaptureSetMute(AudioHandle handle, bool mute);
int32_t AudioCaptureGetMute(AudioHandle handle, bool *mute);
int32_t AudioCaptureSetVolume(AudioHandle handle, float volume);
int32_t AudioCaptureGetVolume(AudioHandle handle, float *volume);
int32_t AudioCaptureGetGainThreshold(AudioHandle handle, float *min, float *max);
int32_t AudioCaptureGetGain(AudioHandle handle, float *gain);
int32_t AudioCaptureSetGain(AudioHandle handle, float gain);
int32_t AudioCaptureCaptureFrame(struct AudioCapture *capture, void *frame,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t AudioCaptureGetCapturePosition(struct AudioCapture *capture, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioRenderRegCallback(struct AudioRender *render, RenderCallback callback, void *cookie);
#endif // AUDIO_INTERNAL_H
