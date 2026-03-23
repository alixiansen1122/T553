/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

#include "audio_base_type.h"
#include "audio_utils.h"
#include "audio_debug_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t             sampleRateMasks;
    uint32_t             channelMasks;
    uint32_t             numFormat;
    AudioCodecFormat     *formats; /* free by device manager */
    uint32_t             bitWidth;
    uint32_t             channel;
} AudioCapability;

typedef struct {
    AudioStreamType type;
    bool interleaved;
    AudioCodecFormat format;
    uint32_t sampleRate;
    uint32_t channelCount;
    uint16_t sampleFmt;
} AudioStreamConfig;

/**
 * @brief audio policy callback type
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    DEVICE_CALLBACK_DEVICE_CONNECTED = 0, /* device connected */
    DEVICE_CALLBACK_DEVICE_DISCONNECTED,  /* device disconnected */
    DEVICE_CALLBACK_UNKNOMN               /* unknown */
} DeviceCallBackType;


/**
 * @brief audio policy callback struct
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
    int32_t (*OnCallback)(DeviceCallBackType type, uint32_t dataLen, const uint8_t *callbackData);
} AudioDeviceCallback;

/**
 * @brief Init device manager, call only by init service once
 *
 * @return Returns 0 if init successfully; returns -1 otherwise.
 */
int32_t DeviceMgrInit(void);

/**
 * @brief Get available device(device state is connection) count, device type can input or output or all.
 *
 * @param flag Indicates the device type, enum DeviceFlag.
 * @param deviceCount Indicates the return device num.
 * @return Returns 0 if find successfully; returns -1 otherwise.
 */
int32_t DeviceMgrGetAvailableDeviceCount(uint32_t flag, uint32_t *deviceCount);


/**
 * @brief Get available device(device state is connection), device type can input or output or all.
 *
 * @param flag Indicates the device type, enum DeviceFlag.
 * @param deviceCount Indicates the return device num.
 * @param devices Indicates the return device info, malloc by caller.
 * @return Returns 0 if find successfully; returns -1 otherwise.
 */
int32_t DeviceMgrGetAvailableDevices(uint32_t flag, uint32_t deviceCount, AudioDeviceInfo *devices);

/**
 * @brief decide the specified device can offload es stream(es stream is encode stream).
 *
 * @param device Indicates the specified device.
 * @param info Indicates the es stream attribute(samplerate,format,channel).
 * @return Returns 1 if support offload; returns 0 otherwise.
 */
bool DeviceMgrIsSupportOffload(AudioDeviceType device, const AudioOffloadInfo *info);

/**
 * @brief set device connect state, such as bluetooth connected or disconnected.
 *
 * @param device Indicates the specified device.
 * @param state Indicates connect state, enum AudioDeviceConnectState.
 * @return Returns 0 if set successfully; returns -1 otherwise.
 */
int32_t DeviceMgrSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state);

/**
 * @brief get device capability.
 *
 * @param device Indicates the specified device.
 * @param capability return device capability.
 * @return Returns 0 if get successfully; returns -1 otherwise.
 */
int32_t DeviceMgrGetDeviceCapability(AudioDeviceType device, AudioCapability *capability);

/**
 * @brief register callback function, when device state is change, notify policy manager.
 *
 * @param pfnCb Indicates callback function.
 * @return Returns 0 if register successfully; returns -1 otherwise.
 */
int32_t DeviceMgrRegisterDeviceCallback(const AudioDeviceCallback *callback);

/**
 * @brief create audiocapture to capture audio stream.
 *
 * @param device Indicates the input device.
 * @param attr Indicates input stream attribute.
 * @param capture return capture handle.
 * @return Returns 0 if create successfully; returns -1 otherwise.
 */
int32_t DeviceMgrCreateStreamIn(uint32_t device, AudioStreamConfig attr, Handle *inputStreamHandle);

/**
 * @brief destroy audiocapture to stop capture audio stream.
 *
 * @param device Indicates the input device.
 * @param capture Indicates capture handle.
 * @return Returns 0 if destroy successfully; returns -1 otherwise.
 */
int32_t DeviceMgrDestroyStreamIn(uint32_t device, Handle inputStreamHandle);

/**
 * @brief create audiorender to render audio stream.
 *
 * @param device Indicates the output device, maybe multi-device, enum AudioDeviceType.
 * @param attr Indicates output stream attribute.
 * @param render return render handle.
 * @return Returns 0 if create successfully; returns -1 otherwise.
 */
int32_t DeviceMgrCreateStreamOut(uint32_t device, AudioStreamConfig attr, Handle *outputStreamHandle);

/**
 * @brief destroy audiorender to stop render audio stream.
 *
 * @param device Indicates the output device.
 * @param render Indicates render handle.
 * @return Returns 0 if destroy successfully; returns -1 otherwise.
 */
int32_t DeviceMgrDestroyStreamOut(uint32_t device, Handle outputStreamHandle);

int32_t DeviceMgrDumpInfo(AudioDeviceDebugInfo *audioDeviceInfo);

int32_t DeviceMgrInvoke(AudioHaidInvokeId audioHaidInvokeId, char *param, uint32_t *paramlength);

typedef int32_t (*AudioDeviceHaidEventCallback)(int32_t eventType, void* param, void* context);

int32_t DeviceMgrRegCallBack(AudioDeviceHaidEventCallback callBack, void* context);

/**
 * @brief deinit device manager.
 *
 * @return Returns 0.
 */
int32_t DeviceMgrDeInit(void);

#ifdef __cplusplus
}
#endif

#endif  // DEVICE_MANAGER_H_

