/*
 * Copyright (c) CompanyNameMagicTag
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

#ifndef DISPLAY_LAYER_INTERNAL_H
#define DISPLAY_LAYER_INTERNAL_H
#include <sys/types.h>
#include "display_common.h"
#include "display_type.h"
#include "soc_dpu_api.h"
#include "graphic_hardware_config.h"

#define CHECK_DEVID_VALID(devId, ret) do { \
    if ((devId) >= DEV_ID_NUM) { \
        DISP_LOGE("devid is invalid\n"); \
        return (ret); \
    } \
} while (0)

#define CHECK_GRAPHIC_LAYERID_VALID(layerId, ret) do { \
    if ((layerId) >= GRA_LAYER_MAX) { \
        DISP_LOGE("graphic layer id is invalid\n"); \
        return (ret); \
    } \
} while (0)

#define CHECK_LAYER_OPENED(devId, layerId, ret) do { \
    LayerPrivate *priv = NULL; \
    priv = GetLayerInstance((devId), (layerId)); \
    if (priv->fd == -1) { \
        DISP_LOGE("layer is not opened\n"); \
        return (ret); \
    } \
} while (0)

/* default layer width */
#define DEFAULT_WIDTH            RESOLUTION_WIDTH
/* default layer height */
#define DEFAULT_HEIGHT           RESOLUTION_HEIGHT
/* Maximum number of display devices supported */
#define DEV_ID_NUM 1
/* Background color */
#define COLOR_RGB_BLUE           0x0000FF
#define BACKGROUD_COLOR          0x00
/* Hdmi frame rate */
#define FRAME_RATE               60
/* Byte align number */
#define BYTE_ALIGN_NUM           2
/* Video layer buffer len */
#define VIDEO_LAYER_BUFF_LEN     3
/* Vo device number */
#define VO_LAYER_ID              0

/* Rotate enable */
#define VO_ROTATE_ENABLE         1

/* SyncInfo symbol */
#define SYNM                0
#define INTFB               0
#define IOP                 1
#define HMID                1
#define IDV                 0
#define IHS                 0
#define IVS                 0
#define BVACT               1
#define BVBB                1
#define BVFB                1

typedef struct {
    /* premultiply enable */
    bool preMul;
    /* max width */
    int32_t maxWidth;
    /* max height */
    int32_t maxHeight;
   /* vo scale enable */
    bool voScale;
} LayerCap;

typedef struct {
    /* file handler */
    int32_t fd;
    /* layer type */
    LayerType type;
    /* layer name */
    char *layerName;
    /* layer pixel format */
    PixelFormat pixFmt;
    /* flush region */
    IRect flushRect;
    /* layer width */
    int32_t width;
    /* layer height */
    int32_t height;
    /* layer canvas */
    ext_dpu_surface canvas;
    /* layer canvas virtual address */
    void *canvas_vir;
    /* layer capability */
    LayerCap cap;
} LayerPrivate;

enum LayerId {
    GRA_LAYER_0,
    GRA_LAYER_1,
    GRA_LAYER_2,
    GRA_LAYER_MAX,
};
#endif /* DISPLAY_LAYER_INTERNAL_H */
