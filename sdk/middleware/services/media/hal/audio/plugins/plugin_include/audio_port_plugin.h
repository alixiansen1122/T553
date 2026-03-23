/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio port plugin base
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_PORT_PLUGIN_H
#define AUDIO_PORT_PLUGIN_H

#include "audio_types.h"

/**
 * @brief Enumerates the audio port track type.
 */
enum PortTrackType {
    TRACK_TYPE_INPUT             = 0x1u, /* Input track */
    TRACK_TYPE_OUTPUT            = 0x2u, /* Output track */
    TRACK_TYPE_OUTPUT_INPUT      = 0x4u, /* Output In track */
    TRACK_TYPE_INVALID           = 0xFFFu, /* Output virtual track */
};

/**
 * @brief Enumerates the audio sub port type.
 */
enum SubPortType {
    SUB_INPUT_PORT_I2S            = 0x1u,        /* Input I2S */
    SUB_INPUT_PORT_ADC            = 0x2u,        /* Input ADC */
    SUB_INPUT_PORT_SIF            = 0x4u,        /* Input SIF */
    SUB_INPUT_PORT_HDMIRX         = 0x8u,        /* Input HDMIRX */
    SUB_INPUT_PORT_PDM            = 0x10u,       /* Input PDM */
    SUB_INPUT_PORT_MAD0           = 0x20u,       /* Input MAD0 */
    SUB_INPUT_PORT_BT_SCO         = 0x40u,       /* Input BT SCO */
    SUB_INPUT_PORT_MODEM          = 0x80u,       /* Input MODEM */
    SUB_INPUT_PORT_MODEM_HEADSET  = 0x100u,      /* Input MODEM HEADSET */
    SUB_OUTPUT_PORT_DAC           = 0x10001u,    /* Output DAC */
    SUB_OUTPUT_PORT_SPDIF         = 0x10002u,    /* Output SPDIF */
    SUB_OUTPUT_PORT_HDMITX1       = 0x10004u,    /* Output HDMITX1 */
    SUB_OUTPUT_PORT_ARC           = 0x10008u,    /* Output ARC */
    SUB_OUTPUT_PORT_BT_SCO        = 0x10010u,    /* Output SCO */
    SUB_OUTPUT_PORT_A2DP          = 0x10020u,    /* Output A2DP */
    SUB_INPUT_PORT_A2DP           = 0x10040u,    /* Input A2DP */
    SUB_OUTPUT_PORT_MODEM         = 0x10080u,    /* Output MODEM */
    SUB_OUTPUT_PORT_MODEM_HEADSET = 0x10100u,    /* Output MODEM HEADSET */
    SUB_PORT_ALL                  = 0xFFFFFFFFu, /* Output ALL */
};

enum VqeScene {
    VQE_SCENE_AI_TALK       = 0, /* Input AI to AO */
    VQE_SCENE_AI_RECORD     = 1, /* Input AI bind AENC */
    VQE_SCENE_AO_MUSIC      = 2, /* Output ADEC to AO */
    VQE_SCENE_AO_RINGING    = 3, /* Output ADEC to AO */
};

enum InvokeID {
    INVOKE_ID_GET_CHANNEL_ID    = 0, /* CurrentChannelId. The value type is <b>uint32_t</b>. */
    INVOKE_ID_CONFIG_VQE        = 1, /* Config vqe for usage scenes see {@link VqeScene} */
    INVOKE_ID_SET_EXTRA_PARAMS = 2, /* Set extra params. The format is <i>key=value</i>. Separate by semicolons (;) */
    INVOKE_ID_GET_EXTRA_PARAMS = 3, /* Get extra params. */
};

enum PluginInvokeId {
    PLUGIN_INVOKE_HAID_CREATE,
    PLUGIN_INVOKE_HAID_DESTROY,
    PLUGIN_INVOKE_HAID_SET_ENABLE,
    PLUGIN_INVOKE_HAID_GET_ENABLE,
    PLUGIN_INVOKE_HAID_SET_EVENT_CALL_BACK,
    PLUGIN_INVOKE_HAID_SET_VOLUME,
    PLUGIN_INVOKE_HAID_GET_VOLUME,
    PLUGIN_INVOKE_HAID_SET_HARDWARE_CONFIG,
    PLUGIN_INVOKE_HAID_SET_CONFIG,
    PLUGIN_INVOKE_HAID_GET_CONFIG,
    PLUGIN_INVOKE_HAID_SET_ALL_CONFIG,
    PLUGIN_INVOKE_HAID_GET_ALL_CONFIG,
    PLUGIN_INVOKE_HAID_TONE_PLAY,
    PLUGIN_INVOKE_HAID_TONE_STOP,
    PLUGIN_INVOKE_HAID_SET_SCENE,
    PLUGIN_INVOKE_HAID_GET_SCENE,
    PLUGIN_INVOKE_TWS_SET_MODE,
    PLUGIN_INVOKE_TWS_SET_ROLE,
    PLUGIN_INVOKE_ANC_INIT,
    PLUGIN_INVOKE_ANC_DEINIT,
    PLUGIN_INVOKE_ANC_SET_CONFIG,
    PLUGIN_INVOKE_ANC_GET_CONFIG,
    PLUGIN_INVOKE_HAID_INVALID,
};

struct PluginInvokeAttr {
    const char* value;
    uint32_t *length;
};

struct PluginInvokeNameId {
    char* funcName;
    enum PluginInvokeId invokeId;
};

enum AudioBitWidth {
    /** 8-bit width */
    AUDIO_BIT_WIDTH_8_BIT   = 8,
    /** 16-bit width */
    AUDIO_BIT_WIDTH_16_BIT  = 16,
    /** 24-bit width */
    AUDIO_BIT_WIDTH_24_BIT  = 24,
    /** 32-bit width */
    AUDIO_BIT_WIDTH_32_BIT  = 32,
    /** Invalid value */
    AUDIO_BIT_WIDTH_INVALID,
};

/**
 * @brief Defines the Invoke attr.
 */
struct InvokeAttr {
    void *request;
    uint64_t requestBytes;
    uint64_t replyBytes;
};

/**
 * @brief Defines the audio port attr.
 */
struct PortPluginAttr {
    enum AudioCategory type;
    enum AudioSampleRatesMask sampleRate;    /* Audio sampleRate, see {@link AudioSampleRatesMask} */
    uint32_t channelCount;                   /* Audio channels */
    uint32_t bitWidth;                       /* Audio bit widths */
    enum AudioFormat audioFormat;            /* Audio Format , see {@link AudioFormat} */
    enum AudioChannelMode trackMode;         /* Audio track mode */
    bool isNeedSetExtraParam;
    uint8_t *extraData;
    uint32_t extraDataSize;
};

/**
 * @brief Enumerates the audio plugin type.
 */
typedef enum {
    AUDIO_PLUGIN_PRIMARY   = 0x1u, /* Primary plugin */
    AUDIO_PLUGIN_BLUETOOTH = 0x2u, /* Bluetooth plugin */
    AUDIO_PLUGIN_MODEM     = 0x3u, /* Modem plugin */
    AUDIO_PLUGIN_NONE,
} AudioPluginType;

typedef int32_t (*TrackCallback)(enum AudioCallbackType type, const void *param, uint32_t len, AudioHandle caller);

typedef int32_t (*AudioPortInvokeFunc)(AudioHandle context, char *value, uint32_t len);

struct AudioPortInvoke {
    char *invokeName;
    AudioPortInvokeFunc invokefunc;
    bool isNeedParseKeyValue;
};

typedef struct {
    enum AudioPortPin audioPin;
    /**
     * @brief Obtains the number of audio frames in the audio buffer.
     *
     * @param trackHandle Indicates the pointer to the audio stream to operate.
     * @param count Indicates the pointer to the number of audio frames in the audio buffer.
     * @return Returns <b>0</b> if the number is obtained successfully; returns a negative value otherwise.
     */
    int32_t (*GetFrameCount)(AudioHandle trackHandle, uint64_t *count);

    /**
     * @brief Obtains the estimated latency of the audio device driver.
     *
     * @param trackHandle Indicates the pointer to the port plugin.
     * @param latencyMs Indicates the pointer to the latency (in milliseconds) to be obtained.
     * @return Returns <b>0</b> if the latency is obtained successfully; returns a negative value otherwise.
     */
    int32_t (*GetLatency)(AudioHandle trackHandle, uint32_t *latencyMs);

    /**
     * @brief Enable audio Port Plugin track.
     *
     * @param trackHandle Indicates the audio port track.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*EnableTrack)(AudioHandle trackHandle);

    /**
     * @brief Pause audio Port Plugin Track.
     *
     * @param trackHandle Indicates the pointer to the port trackHandle.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*PauseTrack)(AudioHandle trackHandle);

    /**
     * @brief Resume audio Port Plugin Track.
     *
     * @param trackHandle Indicates the pointer to the port trackHandle.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*ResumeTrack)(AudioHandle trackHandle);

    /**
     * @brief Flush audio Port Plugin Track.
     *
     * @param trackHandle Indicates the pointer to the port trackHandle.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*FlushTrack)(AudioHandle trackHandle);

    /**
     * @brief AcquireFrame from audio Port Plugin track.
     *
     * @param trackHandle Indicates the audio port track.
     * @param buffer Indicates the pointer to the audio buffer for storing the input data.
     * @param requestBytes Indicates the required audio buffer size, in bytes.
     * @param replyBytes Indicates the pointer to the actual length (in bytes) of the audio data to read.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*AcquireFrame)(AudioHandle trackHandle, uint8_t *buffer,
                            uint64_t requestBytes, uint64_t *replyBytes);

    /**
     * @brief SendFrame to audio Port Plugin track.
     *
     * @param trackHandle Indicates the audio port track.
     * @param buffer Indicates the pointer to the audio buffer for storing the input data.
     * @param requestBytes Indicates the required audio buffer size, in bytes.
     * @param replyBytes Indicates the pointer to the actual length (in bytes) of the audio data to read.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*SendFrame)(AudioHandle trackHandle, const uint8_t *buffer,
                         uint64_t requestBytes, uint64_t *replyBytes);

    /**
     * @brief get audio Port Plugin Position.
     *
     * @param trackHandle Indicates the audio port track.
     * @param frames Indicates the pointer to the last number of input audio frames.
     * @param time Indicates the pointer to the timestamp associated to the frame.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*GetPosition)(AudioHandle trackHandle,
                           uint64_t *frames, struct AudioTimeStamp *time);

    /**
     * @brief Disable audio Port Plugin track.
     *
     * @param trackHandle Indicates the audio port track.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*DisableTrack)(AudioHandle trackHandle);

    /**
     * @brief Set the mute operation for the audio Port Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @param mute Specifies whether to mute the port plugin track.
     * Value <b>true</b> means to mute the port plugin track, and <b>false</b> means the opposite.
     * @return Returns <b>0</b> if set mute operation successfully; returns a negative value otherwise.
     */
    int32_t (*SetMute)(AudioHandle trackHandle, bool mute);

    /**
     * @brief Obtains the mute operation for the audio Port Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @param mute Indicates whether the port plugin track is muted.
     * Value <b>true</b> means the port plugin track is muted, and <b>false</b> means the opposite.
     * @return Returns <b>0</b> if get mute operation successfully; returns a negative value otherwise.
     */
    int32_t (*GetMute)(AudioHandle trackHandle, bool *mute);

    /**
     * @brief set the volume operation for the audio Port Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @param volume Indicates the audio port plugin track volume to set. The value ranges from 0.0 to 100.0.
     * @return Returns <b>0</b> if set volume operation successfully; returns a negative value otherwise.
     */
    int32_t (*SetVolume)(AudioHandle trackHandle, float volume);

    /**
     * @brief Obtains the volume operation for the audio Port Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @param volume Indicates the audio port plugin track volume. The value ranges from 0.0 to 100.0.
     * @return Returns <b>0</b> if get volume operation successfully; returns a negative value otherwise.
     */
    int32_t (*GetVolume)(AudioHandle trackHandle, float *volume);

    /**
     * @brief get audio Port Plugin Position.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @param frames Indicates the pointer to the last number of input audio frames.
     * @param time Indicates the pointer to the timestamp associated to the frame.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*Invoke)(AudioHandle trackHandle,
                      enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

     /**
     * @brief Register the callback function operation for the audio Port Plugin track.
     *
     * @param trackHandle Indicates the audio port track.
     * @param callback Indicates the pointer to the callback function.
     * @param caller Indicates the pointer to the caller.
     * @return Returns <b>0</b> if the callback function is registered successfully; returns a negative value otherwise.
     */
    int32_t (*RegCallback)(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller);
} AudioPortTrack;

typedef int32_t (*PluginHaidEventCallBack)(int32_t eventType, void* param, void* cookie);

typedef struct {
    enum AudioPortDirection audioPortDir;
    uint32_t audioPortPins; // mask of PIN
    /**
     * @brief Obtains the capability set of the port Plugin for the audio adapter.
     *
     * @param ports Indicates the pointer to the port.
     * @param capability Indicates the pointer to the capability set to obtain.
     * @return Returns <b>0</b> if the capability set is successfully obtained; returns a negative value otherwise.
     */
    int32_t (*GetPortPluginCapability)(const struct AudioPort *port, struct AudioPortCapability *capability);

    /**
     * @brief Open audio Port Plugin.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*Open)(AudioHandle *portHandle);

    /**
     * @brief Create audio Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*CreateTrack)(AudioHandle portHandle, enum AudioPortPin audioPortPin,
                           struct PortPluginAttr *portAttr, AudioHandle *trackHandle);

    /**
     * @brief Destroy audio Plugin track.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param trackHandle Indicates the audio port track.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*DestroyTrack)(AudioHandle portHandle, AudioHandle trackHandle);

    /**
     * @brief Close audio Port Plugin.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*Close)(AudioHandle portHandle);

    /**
     * @brief sets the mute operation set for the audio port.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param mute Specifies whether to mute the port. Value <b>true</b> means to mute the port,
     * and <b>false</b> means the opposite.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*SetMute)(AudioHandle portHandle, bool mute);

    /**
     * @brief Obtains the mute operation set for the audio port.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param mute Specifies whether to mute the port. Value <b>true</b> means to mute the port,
     * and <b>false</b> means the opposite.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*GetMute)(AudioHandle portHandle, bool *mute);

    /**
     * @brief stop audio Port Plugin.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param volume Indicates the audio port volume to set. The value ranges from 0.0 to 100.0.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*SetVolume)(AudioHandle portHandle, float volume);

    /**
     * @brief stop audio Port Plugin.
     *
     * @param portHandle Indicates the pointer to the port plugin.
     * @param volume Indicates the audio port volume to get. The value ranges from 0.0 to 1.0.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*GetVolume)(AudioHandle portHandle, float *volume);
    /**
     * @brief function execution.
     *
     * @param trackHandle Indicates the audio port track.
     * @param invokeId Indicates the ID of the function to be executed.
     * @param invokeAttr Indicates Pass the parameters required for executing the function.
     * @return Returns <b>0</b> if the input data is read successfully; returns a negative value otherwise.
     */
    int32_t (*Invoke)(AudioHandle trackHandle,
        enum PluginInvokeId invokeId, struct PluginInvokeAttr *invokeAttr);

    int32_t (*RegCallBack)(AudioHandle trackHandle, PluginHaidEventCallBack callBack, void *cookie);
} AudioPortPlugin;

typedef const AudioPortPlugin *(*GetPortPlugin)(AudioPluginType pluginType);

const AudioPortPlugin *GetPortPluginFuncs(AudioPluginType pluginType);

#endif /* AUDIO_PORT_PLUGIN_BASE_H */
/** @} */
