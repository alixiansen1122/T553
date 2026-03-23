/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: policy default config
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef POLICY_DEFAULT_CONFIG_H
#define POLICY_DEFAULT_CONFIG_H

#include "audio_base_type.h"
#include "policy_manager.h"
#include "policy_config.h"

#define PRIORITY_0 0
#define PRIORITY_1 1
#define PRIORITY_2 2
#define PRIORITY_3 3
#define PRIORITY_4 4
#define PRIORITY_5 5
#define PRIORITY_6 6
#define PRIORITY_7 7
#define PRIORITY_8 8
#define PRIORITY_9 9
#define PRIORITY_10 10
#define PRIORITY_11 11

#define INPUT_STRATEGY_MAP_COLUMN_SIZE 6
#define OUTPUT_STRATEGY_MAP_COLUMN_SIZE 12

static AudioDeviceType g_ringSupportDevices[] = {
    OUT_BUILTIN_SPEAKER
};

static AudioDeviceType g_fitnessVideoSupportDevices[] = {
    OUT_BUILTIN_SPEAKER
};

static AudioDeviceType g_voiceCallOutputSupportDevices[] = {
    OUT_BUILTIN_SPEAKER, OUT_BLUETOOTH_SCO
};

static AudioDeviceType g_voiceCallInputSupportDevices[] = {
    IN_BUILTIN_MIC, IN_BLUETOOTH_SCO
};

static AudioDeviceType g_voiceCallSpiInputSupportDevices[] = {
    IN_BUILTIN_MIC
};

static AudioDeviceType g_voiceCallSpiOutputSupportDevices[] = {
    OUT_BUILTIN_SPEAKER
};

static AudioDeviceType g_voiceCallVolteOutputSupportDevices[] = {
    OUT_MODEM, OUT_MODEM_HEADSET, OUT_BLUETOOTH_SCO, OUT_BUILTIN_SPEAKER
};

static AudioDeviceType g_voiceCallVolteInputSupportDevices[] = {
    IN_MODEM, IN_MODEM_HEADSET, IN_BLUETOOTH_SCO, IN_BUILTIN_MIC
};

static AudioDeviceType g_commonOutputSupportDevices[] = {
    OUT_BLUETOOTH_A2DP, OUT_BUILTIN_SPEAKER
};

static AudioDeviceType g_commonInputSupportDevices[] = {
    IN_BUILTIN_MIC
};

static AudioDeviceType g_musicInputSupportDevices[] = {
    IN_BLUETOOTH_A2DP
};

static StrategyConfig g_defaultStrategies[] = {
    {
        .streamType = AUDIO_STREAM_ALARM_SYSTEM,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_0,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_ringSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_ringSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_1,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE_SPI,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_1,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallSpiOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallSpiOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_2,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallVolteOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallVolteOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_RING,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_3,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_ringSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_ringSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_TTS,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_4,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_ASSISTANT,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_5,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_ALARM_CLOCK,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_6,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_FITNESS_VIDEO,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_7,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_fitnessVideoSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_fitnessVideoSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_SYSTEM,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_8,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_NOTIFICATION_PROMPT,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_9,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_MUSIC,
        .streamDir = AUDIO_STREAM_OUT,
        .priority = PRIORITY_10,
        .routeAllDevices = false,
        .needResume = true,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_A2DP_MUSIC,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_11,
        .routeAllDevices = false,
        .needResume = true,
        .supportDeviceCount = sizeof(g_commonOutputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonOutputSupportDevices,
    },
};

static StrategyConfig g_defaultInputStrategies[] = {
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_0,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE_SPI,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_0,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallSpiInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallSpiInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_CALL_VOLTE,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_1,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_voiceCallVolteInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_voiceCallVolteInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_ASSISTANT,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_2,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_RECORD,
        .streamDir = AUDIO_STREAM_IN,
        .priority = PRIORITY_3,
        .routeAllDevices = false,
        .needResume = false,
        .supportDeviceCount = sizeof(g_commonInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_VOICE_RECOGNITION,
        .streamDir = AUDIO_STREAM_IN,
        .priority = PRIORITY_4,
        .routeAllDevices = false,
        .needResume = true,
        .supportDeviceCount = sizeof(g_commonInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_commonInputSupportDevices,
    },
    {
        .streamType = AUDIO_STREAM_A2DP_MUSIC,
        .streamDir = AUDIO_STREAM_OUT_IN,
        .priority = PRIORITY_5,
        .routeAllDevices = false,
        .needResume = true,
        .supportDeviceCount = sizeof(g_musicInputSupportDevices) / sizeof(AudioDeviceType),
        .supportDevices = g_musicInputSupportDevices,
    },
};

static AudioStrategy g_defaultPriorityToStrategies[][OUTPUT_STRATEGY_MAP_COLUMN_SIZE] = {
    {STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_REJECT,
     STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_DELAY,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_REJECT,
     STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_DELAY,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_REJECT,    STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,
     STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_DELAY,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_REJECT,
     STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_DELAY,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_REJECT,
     STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_DELAY,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_NONE,
     STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_MIX},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_NONE,      STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_MIX},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_NONE,      STRATEGY_MIX,       STRATEGY_MIX},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_NONE,      STRATEGY_EXCLUSIVE},
    {STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE,
     STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_MIX,       STRATEGY_REJECT,    STRATEGY_NONE},
};

static AudioStrategy g_defaultInputPriorityToStrategies[][INPUT_STRATEGY_MAP_COLUMN_SIZE] = {
    {STRATEGY_NONE,         STRATEGY_EXCLUSIVE, STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE,    STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_REJECT,    STRATEGY_REJECT,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE,    STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_REJECT,    STRATEGY_REJECT,     STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE,    STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_NONE,      STRATEGY_REJECT,     STRATEGY_MIX},
    {STRATEGY_EXCLUSIVE,    STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_NONE,       STRATEGY_DELAY},
    {STRATEGY_EXCLUSIVE,    STRATEGY_EXCLUSIVE, STRATEGY_EXCLUSIVE, STRATEGY_MIX,       STRATEGY_EXCLUSIVE,  STRATEGY_NONE},
};

#endif  // AUDIO_BASE_H
