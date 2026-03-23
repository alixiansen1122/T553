/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef PLAYER_SINK_TYPE_H
#define PLAYER_SINK_TYPE_H

#include <memory>
#include <vector>
#include "player_define.h"
#if defined(ENABLE_UIKIT)
#include "surface.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#endif

namespace OHOS {
namespace Media {
using namespace std;
const int32_t AV_INVALID_PTS = -1;
const int32_t VIDEO_INVALID_REGION = -1;
const uint32_t MS_SCALE = 1000;
const uint32_t NANOS_PER_MILLISECOND = 1000000;
#define ENABLE_RENDER "enable-render"
const int32_t MAX_PIPELINE_SINK_NUM = 2;

enum SinkState {
    SINK_STATE_IDLE,
    SINK_STATE_INITED,
    SINK_STATE_PREPARED,
    SINK_STATE_STARTED, /* normal play and tplay */
    SINK_STATE_PAUSE,
    SINK_STATE_STOP,
    SINK_STATE_COMPLETE,  /* recieve eos */
    SINK_STATE_ERROR
};

enum SinkType {
    SINK_TYPE_AUDIO,
    SINK_TYPE_VIDEO,
    SINK_TYPE_BUT,
};

enum DataType {
    DATA_TYPE_U8,
    DATA_TYPE_S8,
    DATA_TYPE_U16,
    DATA_TYPE_S16,
    DATA_TYPE_U32,
    DATA_TYPE_S32,
    DATA_TYPE_U64,
    DATA_TYPE_S64,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_PTR,
};

struct AudioSinkAttr {
    int32_t format;
    uint32_t sampleFmt;
    uint32_t sampleRate;
    uint32_t channel;
    float volume;
    int32_t sessionID;
    uint32_t streamType;
    uint32_t bitWidth;
};

struct VideoSinkAttr {
    int32_t width;
    int32_t height;
    float frameRate;
    int32_t format;
#if defined(ENABLE_UIKIT)
    Surface *surface;
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface;
#endif
};

struct SinkAttr {
    SinkType sinkType;
    int32_t trackId;
    union {
        VideoSinkAttr vidAttr;
        AudioSinkAttr audAttr;
    };
};

struct AudioOffloadParam {
    uint32_t streamType;
    uint32_t sampleRate;
    uint32_t format;
    uint32_t channel;
    uint32_t bitWidth;
};

struct AudioSinkStatus {
    uint64_t audFrameCount;
};

struct VideoSinkStatus {
    uint64_t vidFrameCount;
    uint32_t decWidth;
    uint32_t decHeight;
    uint32_t fpsInteger;
    uint32_t fpsDecimal;
};

enum SinkRetCode {
    SINK_SUCCESS = 0,
    SINK_GET_DEVICE_FAILED,
    SINK_LAOD_DEVICE_FAILED,
    SINK_OPEN_STREAM_FAILED,
    SINK_INIT_FAILED,
    SINK_RENDER_FULL,
    SINK_RENDER_DELAY,
    SINK_RENDER_ERROR,
    SINK_RENDER_FATEL,
    SINK_QUE_EMPTY,
    SINK_RENDER_EOS,
    SINK_INVALID_OP,
    SINK_INVALID_PARAM
};

enum EsFrameType {
    ESFRAME_TYPE_NONE,     /* Unknown type */
    ESFRAME_TYPE_AUDIO,    /* Audio decoding */
    ESFRAME_TYPE_VIDEO,    /* Video decoding */
    ESFRAME_TYPE_IMAGE,    /* Image frame */
    ESFRAME_TYPE_SUB,      /* Subtitle frame */
    ESFRAME_TYPE_DATA,     /* Data */
    ESFRAME_INVALID_TYPE   /* Invalid type */
};

struct EsFrameInfo {
    uint8_t *addr;       /* Virtual address */
    uint64_t size;       /* Length of valid data */
    int64_t pts;         /* Output timestamp */
    uint32_t flag;       /* Output flag */
    EsFrameType type;    /* Stream type */
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SINK_H
