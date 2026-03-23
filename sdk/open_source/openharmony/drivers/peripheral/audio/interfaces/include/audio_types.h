/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_types.h
 *
 * @brief Defines custom data types used in API declarations for the audio module, including audio ports,
 * adapter descriptors, device descriptors, scene descriptors, sampling attributes, and timestamp.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_TYPES_H
#define AUDIO_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Defines the audio handle.
 */
typedef void *AudioHandle;

/**
 * @brief Enumerates the audio port type.
 */
enum AudioPortDirection {
    PORT_OUT    = 0x1u, /**< Output port */
    PORT_IN     = 0x2u, /**< Input port */
    PORT_OUT_IN = 0x3u, /**< Input/output port, supporting both audio input and output */
};

/**
 * @brief Defines the audio port.
 */
struct AudioPort {
    enum AudioPortDirection dir; /**< Audio port type. For details, see {@link AudioPortDirection} */
    uint32_t portId;             /**< Audio port ID */
    const char *portName;        /**< Audio port name */
};

/**
 * @brief Defines the audio adapter descriptor.
 *
 * An audio adapter is a set of port drivers for a sound card, including the output and input ports.
 * One port corresponds to multiple pins, and each pin belongs to a physical component (such as a
 * speaker or a wired headset).
 */
struct AudioAdapterDescriptor {
    const char *adapterName; /**< Name of the audio adapter */
    uint32_t portNum;        /**< Number of ports supported by an audio adapter */
    struct AudioPort *ports; /**< List of ports supported by an audio adapter */
};

/**
 * @brief Enumerates the pin of an audio adapter.
 */
enum AudioPortPin {
    PIN_NONE              = 0x0u,       /**< Invalid pin */
    PIN_OUT_SPEAKER       = 0x1u,       /**< Speaker output pin */
    PIN_OUT_HEADSET       = 0x2u,       /**< Wired headset pin for output */
    PIN_OUT_LINEOUT       = 0x4u,       /**< Line-out pin */
    PIN_OUT_HDMI          = 0x8u,       /**< HDMI output pin */
    PIN_OUT_BT_SCO        = 0x10u,      /**< BT SCO output pin */
    PIN_OUT_BT_A2DP       = 0x20u,      /**< BT A2DP output pin */
    PIN_OUT_MODEM         = 0x40u,       /**< Modem output pin */
    PIN_OUT_MODEM_HEADSET = 0x80u,       /**< Modem headset output pin */
    PIN_OUT_USB           = 0xA0u,       /**< Usb output pin */
    PIN_OUT_USB_DOGGLE    = 0x100u,      /**< Usb doggle output pin */
    PIN_IN_MIC            = 0x8000001u, /**< Microphone input pin */
    PIN_IN_HS_MIC         = 0x8000002u, /**< Wired headset microphone pin for input */
    PIN_IN_LINEIN         = 0x8000004u, /**< Line-in pin */
    PIN_IN_BT_SCO         = 0x8000008u, /**< BT SCO input pin */
    PIN_IN_BT_A2DP        = 0x8000010u, /**< BT A2DP input pin */
    PIN_IN_MODEM          = 0x8000020u, /**< Modem input pin */
    PIN_IN_MODEM_HEADSET  = 0x8000040u, /**< Modem headset input pin */
    PIN_IN_USB            = 0x8000080u, /**< Usb input pin */
    PIN_IN_USB_DOGGLE     = 0x80000A0u, /**< Usb doggle input pin */
};

/**
 * @brief Defines the audio device descriptor.
 */
struct AudioDeviceDescriptor {
    uint32_t portId;        /**< Audio port ID */
    uint32_t pins; /**< Pins of audio ports (input and output). For details, see {@link AudioPortPin}. */
    const char *desc;       /**< Audio device name */
};

/**
 * @brief Enumerates the audio category.
 */
enum AudioCategory {
    AUDIO_IN_MEDIA = 0x10000000u,     /**< Media */
    AUDIO_IN_MUSIC = 0x10000001u,
    AUDIO_IN_A2DP_MUSIC = 0x10000002u,
    AUDIO_IN_NOTIFICATION_SYSTEM = 0x10000004u,
    AUDIO_IN_NEARLINK_MUSIC = 0x10000008u,
    AUDIO_IN_VOICE_RECORD = 0x10000010u,
    AUDIO_IN_VOICE_RECOGNITION = 0x10000011u,
    AUDIO_IN_FITNESS_VIDEO = 0x10000012u,
    AUDIO_IN_USB_MUSIC = 0x10000014u,
    AUDIO_IN_COMMUNICATION = 0x20000000u, /**< Communications */
    AUDIO_IN_VOICE_CALL= 0x20000001u, 
    AUDIO_IN_BT_VOICE_CALL= 0x20000002u, 
    AUDIO_IN_USB_VOICE_CALL= 0x20000004u, 
    AUDIO_IN_NEARLINK_VOICE_CALL= 0x20000008u, 
    AUDIO_IN_VOICE_ASSISTANT = 0x20000010u, 
    AUDIO_IN_VOICE_CALL_VOLTE = 0x20000020u, 
    AUDIO_IN_LIVE_MIC = 0x40000000u, 
	AUDIO_IN_OTHERS = 0xF0000000,		/**< OTHERS */
};

/**
 * @brief Defines the audio scene descriptor.
 */
struct AudioSceneDescriptor {
    /**
     * @brief Describes the audio scene.
     */
    union SceneDesc {
        uint32_t id;                   /**< Audio scene ID */
        const char *desc;              /**< Name of the audio scene */
    } scene;                           /**< The <b>scene</b> object */
    struct AudioDeviceDescriptor desc; /**< Audio device descriptor */
};

/**
 * @brief Enumerates the audio format.
 */
enum AudioFormat {
    AUDIO_FORMAT_PCM_8_BIT  = 0x1u,       /**< 8-bit PCM */
    AUDIO_FORMAT_PCM_16_BIT = 0x2u,       /**< 16-bit PCM */
    AUDIO_FORMAT_PCM_24_BIT = 0x3u,       /**< 24-bit PCM */
    AUDIO_FORMAT_PCM_32_BIT = 0x4u,       /**< 32-bit PCM */
    AUDIO_FORMAT_PCM_32_BIT_FLOAT = 0x5u, /**< 32-bit float PCM */
    AUDIO_FORMAT_AAC_MAIN   = 0x1000001u, /**< AAC main */
    AUDIO_FORMAT_AAC_LC     = 0x1000002u, /**< AAC LC */
    AUDIO_FORMAT_AAC_LD     = 0x1000003u, /**< AAC LD */
    AUDIO_FORMAT_AAC_ELD    = 0x1000004u, /**< AAC ELD */
    AUDIO_FORMAT_AAC_HE_V1  = 0x1000005u, /**< AAC HE_V1 */
    AUDIO_FORMAT_AAC_HE_V2  = 0x1000006u, /**< AAC HE_V2 */
    AUDIO_FORMAT_G711A      = 0x2000001u, /**< G711A */
    AUDIO_FORMAT_G711U      = 0x2000002u, /**< G711u */
    AUDIO_FORMAT_G726       = 0x2000003u, /**< G726 */
    AUDIO_FORMAT_SBC        = 0x3000001u, /**< BT A2DP SBC */
    AUDIO_FORMAT_MP3        = 0x3000002u, /**< MP3 */
    AUDIO_FORMAT_HWA        = 0x3000003u, /**< BT A2DP HWA */
    AUDIO_FORMAT_CVSD       = 0x3000004u, /**< BT HFP CVSD */
    AUDIO_FORMAT_mSBC       = 0x3000005u, /**< BT HFP mSBC */
    AUDIO_FORMAT_OPUS       = 0x3000006u, /**< OPUS */
    AUDIO_FORMAT_FLAC       = 0x3000007u, /**< FLAC */
    AUDIO_FORMAT_VORBIS     = 0x3000008u, /**< VORBIS */
    AUDIO_FORMAT_APE        = 0x3000009u, /**< APE */
    AUDIO_FORMAT_SILK       = 0x300000Au, /**< SILK */
    AUDIO_FORMAT_L2HC       = 0x300000Bu, /**< L2HC */
    AUDIO_FORMAT_AMR_WB     = 0x300000Cu, /**< AMR_WB */
};

/**
 * @brief Enumerates the audio channel mask.
 *
 * A mask describes an audio channel position.
 */
enum AudioChannelMask {
    AUDIO_CHANNEL_FRONT_LEFT  = 0x1,  /**< Front left channel */
    AUDIO_CHANNEL_FRONT_RIGHT = 0x2,  /**< Front right channel */
    AUDIO_CHANNEL_MONO        = 0x1u, /**< Mono channel */
    AUDIO_CHANNEL_STEREO      = 0x3u, /**< Stereo channel, consisting of front left and front right channels */
};

/**
 * @brief Enumerates masks of audio sampling rates.
 */
enum AudioSampleRatesMask {
    AUDIO_SAMPLE_RATE_MASK_8000    = 0x1u,        /**< 8 kHz */
    AUDIO_SAMPLE_RATE_MASK_12000   = 0x2u,        /**< 12 kHz */
    AUDIO_SAMPLE_RATE_MASK_11025   = 0x4u,        /**< 11.025 kHz */
    AUDIO_SAMPLE_RATE_MASK_16000   = 0x8u,        /**< 16 kHz */
    AUDIO_SAMPLE_RATE_MASK_22050   = 0x10u,       /**< 22.050 kHz */
    AUDIO_SAMPLE_RATE_MASK_24000   = 0x20u,       /**< 24 kHz */
    AUDIO_SAMPLE_RATE_MASK_32000   = 0x40u,       /**< 32 kHz */
    AUDIO_SAMPLE_RATE_MASK_44100   = 0x80u,       /**< 44.1 kHz */
    AUDIO_SAMPLE_RATE_MASK_48000   = 0x100u,      /**< 48 kHz */
    AUDIO_SAMPLE_RATE_MASK_64000   = 0x200u,      /**< 64 kHz */
    AUDIO_SAMPLE_RATE_MASK_96000   = 0x400u,      /**< 96 kHz */
    AUDIO_SAMPLE_RATE_MASK_INVALID = 0xFFFFFFFFu, /**< Invalid sampling rate */
};

/**
 * @brief Defines audio sampling attributes.
 */
struct AudioSampleAttributes {
    enum AudioCategory type;   /**< Audio type. For details, see {@link AudioCategory} */
    bool interleaved;          /**< Interleaving flag of audio data */
    enum AudioFormat format;   /**< Audio data format. For details, see {@link AudioFormat}. */
    uint32_t sampleRate;       /**< Audio sampling rate */
    uint32_t channelCount;     /**< Number of audio channels. For example, for the mono channel, the value is 1,
                                * and for the stereo channel, the value is 2.
                                */
    uint32_t bitWidth;
    uint32_t period;           /**< Audio sampling period */ 
    uint32_t frameSize;        /**< Frame size of the audio data */
    bool isBigEndian;          /**< Big endian flag of audio data */
    bool isSignedData;         /**< Signed or unsigned flag of audio data */
    uint32_t startThreshold;   /**< Audio render start threshold. */
    uint32_t stopThreshold;    /**< Audio render stop threshold. */
    uint32_t silenceThreshold; /**< Audio capture buffer threshold. */
    bool isProductTestMode;    /**< Is the current production test mode. */
    uint32_t micType;          /**< Microphone used for data collection. */
};

/**
 * @brief Defines the audio timestamp, which is a substitute for POSIX <b>timespec</b>.
 */
struct AudioTimeStamp {
    int64_t tvSec;  /**< Seconds */
    int64_t tvNSec; /**< Nanoseconds */
};

/**
 * @brief Enumerates the passthrough data transmission mode of an audio port.
 */
enum AudioPortPassthroughMode {
    PORT_PASSTHROUGH_LPCM    = 0x1, /**< Stereo PCM */
    PORT_PASSTHROUGH_RAW     = 0x2, /**< HDMI passthrough */
    PORT_PASSTHROUGH_HBR2LBR = 0x4, /**< Blu-ray next-generation audio output with reduced specifications */
    PORT_PASSTHROUGH_AUTO    = 0x8, /**< Mode automatically matched based on the HDMI EDID */
};

/**
 * @brief Defines the sub-port capability.
 */
struct AudioSubPortCapability {
    uint32_t portId;                    /**< Sub-port ID */
    const char *desc;                   /**< Sub-port name */
    enum AudioPortPassthroughMode mask; /**< Passthrough mode of data transmission. For details,
                                         * see {@link AudioPortPassthroughMode}.
                                         */
};

/**
 * @brief Defines the audio port capability.
 */
struct AudioPortCapability {
    uint32_t deviceType;                     /**< Device type (output or input) */
    uint32_t deviceId;                       /**< Device ID used for device binding */
    bool hardwareMode;                       /**< Whether to support device binding */
    uint32_t formatNum;                      /**< Number of the supported audio formats */
    enum AudioFormat *formats;               /**< Supported audio formats. For details, see {@link AudioFormat}. */
    uint32_t sampleRateMasks;                /**< Supported audio sampling rates (8 kHz, 16 kHz, 32 kHz, and 48 kHz) */
    enum AudioChannelMask channelMasks;      /**< Audio channel layout mask of the device. For details,
                                              * see {@link AudioChannelMask}.
                                              */
    uint32_t channelCount;                   /**< Supported maximum number of audio channels */
    uint32_t subPortsNum;                    /**< Number of supported sub-ports (for output devices only) */
    struct AudioSubPortCapability *subPorts; /**< List of supported sub-ports */
};

/**
 * @brief Enumerates channel modes for audio rendering.
 *
 * @attention The following modes are set for rendering dual-channel audios. Others are not supported.
 */
enum AudioChannelMode {
    AUDIO_CHANNEL_NORMAL = 0, /**< Normal mode. No processing is required. */
    AUDIO_CHANNEL_BOTH_LEFT,  /**< Two left channels */
    AUDIO_CHANNEL_BOTH_RIGHT, /**< Two right channels */
    AUDIO_CHANNEL_EXCHANGE,   /**< Data exchange between the left and right channels. The left channel takes the audio
                               * stream of the right channel, and the right channel takes that of the left channel.
                               */
    AUDIO_CHANNEL_MIX,        /**< Mix of streams of the left and right channels */
    AUDIO_CHANNEL_LEFT_MUTE,  /**< Left channel muted. The stream of the right channel is output. */
    AUDIO_CHANNEL_RIGHT_MUTE, /**< Right channel muted. The stream of the left channel is output. */
    AUDIO_CHANNEL_BOTH_MUTE,  /**< Both left and right channels muted */
};

/**
 * @brief Enumerates the execution types of the <b>DrainBuffer</b> function.
 */
enum AudioDrainNotifyType {
    AUDIO_DRAIN_NORMAL_MODE, /**< The <b>DrainBuffer</b> function returns after all data finishes playback. */
    AUDIO_DRAIN_EARLY_MODE,  /**< The <b>DrainBuffer</b> function returns before all the data of the current track
                              * finishes playback to reserve time for a smooth track switch by the audio service.
                              */
};

/**
 * @brief Enumerates callback notification events.
 */
enum AudioCallbackType {
    AUDIO_NONBLOCK_WRITE_COMPELETED, /**< The non-block write is complete. */
    AUDIO_DRAIN_COMPELETED,          /**< The draining is complete. */
    AUDIO_FLUSH_COMPLETED,           /**< The flush is complete. */
    AUDIO_RENDER_FULL,               /**< The render buffer is full.*/
    AUDIO_RENDER_EOS,                /**< Render to the end of the stream. */
    AUDIO_BUFFER_EMPTY,              /**< The underlying buffer data is empty. */
    AUDIO_ERROR_OCCUR,               /**< An error occurs.*/
};

/**
 * @brief Describes a mmap buffer.
 */
struct AudioMmapBufferDescripter {
    void *memoryAddress;                 /**< Pointer to the mmap buffer */
    int32_t memoryFd;                    /**< File descriptor of the mmap buffer */
    int32_t totalBufferFrames;           /**< Total size of the mmap buffer (unit: frame )*/
    int32_t transferFrameSize;           /**< Transfer size (unit: frame) */
    int32_t isShareable;                 /**< Whether the mmap buffer can be shared among processes */
    uint32_t offset;
};

enum AudioExtParamKey {
    AUDIO_EXT_PARAM_KEY_NONE = 0,     /**< Distributed audio extra param key none */
    AUDIO_EXT_PARAM_KEY_VOLUME = 1,   /**< Distributed audio extra param key volume event */
    AUDIO_EXT_PARAM_KEY_FOCUS = 2,    /**< Distributed audio extra param key focus event */
    AUDIO_EXT_PARAM_KEY_BUTTON = 3,   /**< Distributed audio extra param key media button event */
    AUDIO_EXT_PARAM_KEY_EFFECT = 4,   /**< Distributed audio extra param key audio effect event */
    AUDIO_EXT_PARAM_KEY_STATUS = 5,   /**< Distributed audio extra param key device status event */
    AUDIO_EXT_PARAM_HAID_MANAGER = 6,
    AUDIO_EXT_PARAM_KEY_LOWPOWER = 1000, /**< Low power event type */
};

/**
 * @brief Called when an event defined in {@link AudioCallbackType} occurs.
 *
 * @param AudioCallbackType Indicates the occurred event that triggers this callback.
 * @param reserved Indicates the pointer to a reserved field.
 * @param cookie Indicates the pointer to the cookie for data transmission.
 * @return Returns <b>0</b> if the callback is successfully executed; returns a negative value otherwise.
 * @see RegCallback
 */
typedef int32_t (*RenderCallback)(enum AudioCallbackType, void *reserved, void *cookie);

/**
 * @brief Register audio extra param callback that will be invoked during audio param event.
 *
 * @param key Indicates param change event.
 * @param condition Indicates the param condition.
 * @param value Indicates the param value.
 * @param reserved Indicates reserved param.
 * @param cookie Indicates the pointer to the callback parameters;
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 */
typedef int32_t (*ParamCallback)(enum AudioExtParamKey key, const char *condition, const char *value, void *reserved,
    void *cookie);

#endif /* AUDIO_TYPES_H */
