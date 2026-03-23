/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio base type
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef AUDIO_BASE_TYPE_H
#define AUDIO_BASE_TYPE_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t AudioSession;

enum {
    AUDIO_SESSION_ID_NONE    = 0,
};

typedef enum {
    OUTPUT_DEVICES_FLAG    = 0x1u,
    INPUT_DEVICES_FLAG     = 0x2u,
} DeviceFlag;

typedef enum {
    UNKNOWN = 0x0,
    IN_FLAG = 0x80000000,
    OUT_BUILTIN_SPEAKER = 0x2,
    OUT_BLUETOOTH_SCO = 0x10,
    OUT_BLUETOOTH_A2DP = 0x80,
    OUT_MODEM = 0x100,
    OUT_MODEM_HEADSET = 0x200,
    IN_BUILTIN_MIC = IN_FLAG | 0x4,
    IN_BLUETOOTH_SCO = IN_FLAG | 0x8,
    IN_BLUETOOTH_A2DP = IN_FLAG | 0x20,
    IN_MODEM = IN_FLAG | 0x40,
    IN_MODEM_HEADSET = IN_FLAG | 0x400,
} AudioDeviceType;

typedef struct {
    DeviceFlag      flag;          /* enum DeviceRole  */
    AudioDeviceType device;        /* device type */
    const char      *name;
} AudioDeviceInfo;

typedef enum {
    AUDIO_DEVICE_CONNECTED        = 0x0u,
    AUDIO_DEVICE_DISCONNECTED     = 0x1u,
} AudioDeviceConnectState;

/**
 * @brief Describes ringer modes.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /**
     * Indicates that the ringer mode is restored to the previous setting.
     * A device will be audible if the previous mode was sound.
     */
    RINGER_MODE_NORMAL = 0,
    /**
     * Indicates that a device is silent and vibrates. For calls, the device will vibrate,
     * and for message notifications, the vibration can be enabled by users.
     */
    RINGER_MODE_VIBRATE,
    /**
     * Indicates that a device is silent and does not vibrate. This mode overrides the vibration mode.
     */
    RINGER_MODE_SILENT,
    /**
     * Invalid value
     */
    RINGER_MODE_INVALID = 0xFFFFFFFF,
} AudioRingMode;

/**
 * @brief Enumerates audio source types.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /** Invalid audio source */
    AUDIO_SOURCE_INVALID = -1,
    /** Default audio source */
    AUDIO_SOURCE_DEFAULT = 0,
    /** Microphone */
    AUDIO_MIC = 1,
    /** Uplink voice */
    AUDIO_VOICE_UPLINK = 2,
    /** Downlink voice */
    AUDIO_VOICE_DOWNLINK = 3,
    /** Voice call */
    AUDIO_VOICE_CALL = 4,
    /** Camcorder */
    AUDIO_CAMCORDER = 5,
    /** Voice recognition */
    AUDIO_VOICE_RECOGNITION = 6,
    /** Voice communication */
    AUDIO_VOICE_COMMUNICATION = 7,
    /** Remote submix */
    AUDIO_REMOTE_SUBMIX = 8,
    /** Unprocessed audio */
    AUDIO_UNPROCESSED = 9,
    /** Voice performance */
    AUDIO_VOICE_PERFORMANCE = 10,
    /** Echo reference */
    AUDIO_ECHO_REFERENCE = 1997,
    /** Radio tuner */
    AUDIO_RADIO_TUNER = 1998,
    /** Hotword */
    AUDIO_HOTWORD = 1999,
    /** Extended remote submix */
    AUDIO_REMOTE_SUBMIX_EXTEND = 10007,
} AudioSourceType;

/**
 * @brief Defines the audio Device Descriptor.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    /** Device name */
    uint8_t *deviceName; // todo: std::string --> uint8_t *
    /** Type of the audio input source */
    AudioSourceType inputSourceType;
    /** Bits 31-24: reserved bits; bits 23-16: mode ID; bits 15-8: device ID; bits 7-0: channel ID */
    uint32_t deviceId;
} AudioDeviceDesc;

/**
 * @brief Enumerates audio codec formats.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /** Default format */
    AUDIO_DEFAULT    = 0,
    /** PCM */
    PCM              = 1,
    /** Advanced Audio Coding Low Complexity (AAC-LC) */
    AAC_LC           = 2,
    /** High-Efficiency Advanced Audio Coding (AAC-HE), previously known as AAC+ or aacPlus v1 */
    AAC_HE_V1        = 3,
    /** AAC++ or aacPlus v2 */
    AAC_HE_V2        = 4,
    /** Advanced Audio Coding Low Delay (AAC-LD) */
    AAC_LD           = 5,
    /** Advanced Audio Coding Enhanced Low Delay (AAC-ELD) */
    AAC_ELD          = 6,
    /** G711 a-law*/
    G711A            = 7,
    /** G711 u-law */
    G711U            = 8,
    /** G726 */
    G726             = 9,
    /** Opus */
    OPUS             = 10,
    /** Flac */
    FLAC             = 11,
    /** Vorbis */
    VORBIS           = 12,
    /** Ape */
    APE              = 13,
    /** Mp3 */
    MP3              = 14,
    /** mSBC */
    mSBC             = 15,
    /** silk */
    SILK             = 16,
    /** SBC */
    SBC              = 17,
    /** AMR_WB */
    AMR_WB           = 18,
    /** Invalid value */
    FORMAT_INVALID,
} AudioCodecFormat;

/**
 * @brief 音频流类型定义.
 *
 * @since 1.0
 * @version 1.0
 */
 // 0xFFFF FFFFu  高8位表示基础类型分类   低8位 子类
typedef enum {
    /** Default audio stream type */
    TYPE_DEFAULT = -1,
    /** Media */
    TYPE_MEDIA = 0,
    /** Voice call */
    TYPE_VOICE_COMMUNICATION = 1,

    AUDIO_STREAM_NONE                       = 0x0u,             /**< None */
    AUDIO_STREAM_ALARM                      = 0x00010000u,      /**< 报警类 */
    AUDIO_STREAM_ALARM_SYSTEM               = 0x00010001u,      /**< 设备报警: 手表温度过高报警、车机上发动机故障报警、转向系统报警、刹车系统故障报警等
                                                                    健康报警:心率过高、环境噪声报警 */
    AUDIO_STREAM_ALARM_CLOCK                = 0x00010002u,      /**< 闹钟 */
    AUDIO_STREAM_RING                       = 0x00020000u,      /**< 来电铃声 */
    AUDIO_STREAM_VOICE_CALL                 = 0x00040000u,      /**< 语音通话 */
    AUDIO_STREAM_VOICE_CALL_VOIP            = 0x00040001u,      /**< 语音通话 */
    AUDIO_STREAM_VOICE_CALL_BT_SCO          = 0x00040002u,      /**< 蓝牙通话 */
    AUDIO_STREAM_VOICE_CALL_VOLTE           = 0x00040003u,      /**< 4G VOLTE通话I2S方式 */
    AUDIO_STREAM_VOICE_CALL_VOLTE_SPI       = 0x00040004u,      /**< 4G VOLTE通话SPI方式 */
    AUDIO_STREAM_VOICE_ASSISTANT            = 0x00080004u,      /**< 语音助手 */
    AUDIO_STREAM_TTS                        = 0x00100000u,      /**< 语音合成 */
    AUDIO_STREAM_NOTIFICATION               = 0x00200000u,      /**< 提示音 */
    AUDIO_STREAM_NOTIFICATION_SYSTEM        = 0x00200001u,      /**< 设备提示音: 蓝牙断开、连接提示音 低电量 LOW POWER 消息提示音 MESSAGE*/
    AUDIO_STREAM_NOTIFICATION_PROMPT        = 0x00200002,       /**< 运动健康提示音 */
    AUDIO_STREAM_MUSIC                      = 0x00400000u,      /**< 媒体音 */
    AUDIO_STREAM_A2DP_MUSIC                 = 0x00400001u,      /**< 蓝牙输入媒体音 */
    AUDIO_STREAM_FITNESS_VIDEO              = 0x00800000u,      /**< 健身视频指导提示音 */
    AUDIO_STREAM_VOICE_RECORD               = 0x01000000u,      /**< 语音采集录制 */
    AUDIO_STREAM_VOICE_RECOGNITION          = 0x02000000u,      /**< 语音识别 */
    AUDIO_STREAM_INVALID                    = 0x7FFFFFFFu       /**< Max 无效 */
} AudioStreamType;

/**
 * Describes audio stream Link Direction.
 *
 * @since 1
 */
typedef enum {
    AUDIO_DOWN_LINK,
    AUDIO_UP_LINK,
    AUDIO_LINK_NONE
} AudioLinkDirection;

/**
 * Describes audio stream flags.
 *
 * @since 1
 */
typedef enum {
    AUDIO_STREAM_FLAG_NONE = 0x0,
    AUDIO_STREAM_FLAG_LOW_LATENCY = 0x1,
    AUDIO_STREAM_FLAG_SECURE = 0x2,
} AudioStreamFlag;

/**
 * @brief Enumerates the audio channel mode.
 *
 * A mask describes an audio channel position.
 */
typedef enum {
    /**
     * Indicates an invalid channel mask.
     */
    AUDIO_CHANNEL_MODE_NONE = 0x0u,
    /**
     * Indicates the mask of the front left output channel.
     */
    AUDIO_CHANNEL_MODE_FRONT_LEFT = 0x1u,
    /**
     * Indicates the mask of the front right output channel.
     */
    AUDIO_CHANNEL_MODE_FRONT_RIGHT = 0x2u,
    /**
     * Indicates the single output channel mask.
     */
    AUDIO_CHANNEL_MODE_MONO = 0x4u,
    /**
     * Indicates the dual output channel mask of
     * {@code AUDIO_CHANNEL_FRONT_LEFT | AUDIO_CHANNEL_RIGHT}.
     */
    AUDIO_CHANNEL_MODE_STEREO = 0x8u,
} AudioChannelMode;

/**
 * @brief Enumerates audio bit widths.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /** 8-bit width */
    BIT_WIDTH_8   = 8,
    /** 16-bit width */
    BIT_WIDTH_16  = 16,
    /** 24-bit width */
    BIT_WIDTH_24  = 24,
    /** 32-bit width */
    BIT_WIDTH_32  = 32,
    /** Invalid value */
    BIT_WIDTH_BUTT,
} AudioBitWidth;

typedef struct {
    AudioStreamType streamType;
    uint32_t      samplerate;
    AudioCodecFormat format;
    uint32_t      channel;
    uint32_t      bitwidth;
} AudioOffloadInfo;

/**
 * @brief 音频策略定义.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /* NA 同级别流策略，或者不存在此种流组合 */
    STRATEGY_NONE       = 0x0u,
    /* 打断、排他，当返回该策略时需要给使用者回调INTERRUPT 的事件并停止接收之前流的数据 */
    STRATEGY_EXCLUSIVE  = 0x1u,
    /* 混音 （第一条流权重100%，每次增加需要混音的流权重 为50%，之前的每条流按比例共享剩下的50%） */
    STRATEGY_MIX        = 0x2u,
    /* 延时，当前流延时获取音频焦点 */
    STRATEGY_DELAY      = 0x3u,
     /* 不播放，当前无法获取音频焦点 */
    STRATEGY_REJECT     = 0x4u
} AudioStrategy;

/**
 * Indicates that the interruption result.
 *
 * @since 1
 */
enum {
    /**
     * Indicates that failed to interrupt.
     *
     * @since 1
     */
    INTERRUPT_FAILED = -1,

    /**
     * Indicates Interrupted successfully, include mix interrupt.
     *
     * @since 1
     */
    INTERRUPT_GRANTED = 0,

    /**
     * Indicates Delayed interrupt.
     *
     * @since 1
     */
    INTERRUPT_DELAYED,
};

/**
 * Indicates that the interruption types.
 *
 * @since 1
 */
enum AudioInterruptType {
    /**
     * Indicates that the interruption starts.
     *
     * @since 1
     */
    INTERRUPT_TYPE_BEGIN = 1,

    /**
     * Indicates that the interruption ends.
     *
     * @since 1
     */
    INTERRUPT_TYPE_END
};

/**
 * Indicates that the interruption hint types.
 *
 * @since 1
 */
enum AudioInterruptHintType {
    /**
     * Indicates a prompt for the application to pause audio playback.
     *
     * @since 1
     */
    INTERRUPT_HINT_PAUSE = 0,

    /**
     * Indicates a prompt for the application to resume audio playback.
     *
     * @since 1
     */
    INTERRUPT_HINT_RESUME,

    /**
     * Indicates a prompt for the application to stop audio playback.
     *
     * @since 1
     */
    INTERRUPT_HINT_STOP
};

/**
 * Provides audio stream information.
 *
 * <p>This class provides initialized parameters of {@code AudioCapturer} and {@code AudioRenderer},
 * including the stream encoding format, sampling rate, audio channel, usage type, session ID,
 * buffer size, clearing flag, audio source, and stream flags.
 *
 * @since 1
 */
typedef struct {
    AudioStreamType streamType;
    AudioSession sessionID;
    AudioCodecFormat format;
    uint32_t sampleRate;
    int32_t bitRate;
    /** Number of audio channels */
    uint32_t channelCount;
    AudioChannelMode channelMask;
    AudioStreamFlag audioStreamFlag;
    AudioLinkDirection linkDir;
    uint16_t sampleFmt;
} AudioStreamInfo;

/**
 * Provides audio stream out config.
 *
 * <p>This class provides initialized parameters of {@code AudioCapturer} and {@code AudioRenderer},
 * including the stream encoding format, sampling rate, audio channel, usage type, session ID,
 * buffer size, clearing flag, audio source, and stream flags.
 *
 * @since 1
 */
typedef struct {
    AudioCodecFormat audioFormat;
    AudioStreamType streamType;
    uint32_t sampleRate;
    uint32_t sessionID;
    uint32_t channelCount;
    AudioLinkDirection linkDir;
    uint16_t sampleFmt;
    AudioBitWidth bitWidth;
} AudioRendererConfig;

typedef struct {
    AudioCodecFormat audioFormat;
    AudioStreamType streamType;
    uint32_t sampleRate;
    uint32_t sessionID;
    uint32_t channelCount;
    AudioLinkDirection linkDir;
    AudioBitWidth bitWidth;
} CapturerInputConfig;

typedef enum {
    AUDIO_EVENT_NONBLOCK_WRITE_COMPLETED,  /**< The non-block write is complete. */
    AUDIO_EVENT_DRAIN_COMPLETED,           /**< The draining is complete. */
    AUDIO_EVENT_FLUSH_COMPLETED,           /**< The flush is complete. */
    AUDIO_EVENT_RENDER_FULL,               /**< The render buffer is full. */
    AUDIO_EVENT_RENDER_EOS,                /**< Render to the end of the stream. */
    AUDIO_EVENT_BUFFER_EMPTY,              /**< The underlying buffer data is empty. */
    AUDIO_EVENT_OUTPUT_DEVICE_CHANGED,     /**< The output device has been changed. */
    AUDIO_EVENT_ERROR_OCCUR,               /**< An error occurs. */
} AudioCallbackEventType;

typedef int32_t (*RendererCallback)(const void *caller, AudioCallbackEventType event, const void *info);

typedef int32_t (*CapturerCallback)(const void *caller, AudioCallbackEventType event, const void *info);

typedef struct {
    uint32_t frameCount;     // number of sample frames corresponding to size;
    uint32_t size;           // input in bytes = frameCount * frameSize
                             // output in bytes >= frameCount * channelCount * BytesPerSample
    uint8_t *data;           // 调用者分配内存
    // 时间戳?
} CapturerBuffer;

typedef enum {
    AUDIO_INVOKE_HAID_CREATE,
    AUDIO_INVOKE_HAID_DESTROY,
    AUDIO_INVOKE_HAID_SET_ENABLE,
    AUDIO_INVOKE_HAID_GET_ENABLE,
    AUDIO_INVOKE_HAID_SET_EVENT_CALL_BACK,
    AUDIO_INVOKE_HAID_SET_VOLUME,
    AUDIO_INVOKE_HAID_GET_VOLUME,
    AUDIO_INVOKE_HAID_SET_HARDWARE_CONFIG,
    AUDIO_INVOKE_HAID_SET_CONFIG,
    AUDIO_INVOKE_HAID_GET_CONFIG,
    AUDIO_INVOKE_HAID_SET_ALL_CONFIG,
    AUDIO_INVOKE_HAID_GET_ALL_CONFIG,
    AUDIO_INVOKE_HAID_TONE_PLAY,
    AUDIO_INVOKE_HAID_TONE_STOP,
    AUDIO_INVOKE_HAID_SET_SCENE,
    AUDIO_INVOKE_HAID_GET_SCENE,
    AUDIO_INVOKE_TWS_SET_MODE,
    AUDIO_INVOKE_TWS_SET_ROLE,
    AUDIO_INVOKE_ANC_INIT,
    AUDIO_INVOKE_ANC_DEINIT,
    AUDIO_INVOKE_ANC_SET_CONFIG,
    AUDIO_INVOKE_ANC_GET_CONFIG,
} AudioHaidInvokeId;

typedef struct {
    bool toggleValid;     /* the config will take effect immediately;
                                       true,  the config will take effect in the toggle_count interrupt; */
    uint32_t toggleCount; /* when toggle_valid is true,
                                      the config will take effect in the toggle_count interrupt. */
} ToggleConfig;

typedef enum {
    TOGGLE_COUNT_TYPE_VOLUME,
    TOGGLE_COUNT_TYPE_NOTIFY,
} ToggleConfigType;

#endif  // AUDIO_BASE_TYPE_H
