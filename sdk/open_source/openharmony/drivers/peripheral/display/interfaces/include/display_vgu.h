/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* *
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * This module provides driver functions for the graphics subsystem, including graphics layer management,
 * device control, graphics hardware acceleration, display memory management, and callbacks.
 *
 * @since 3.0
 */

/* *
 * @file display_vgu.h
 *
 * @brief Declares the driver functions for implementing 2D vector hardware acceleration.
 *
 * @since 3.0
 */

#ifndef DISPLAY_VGU_H
#define DISPLAY_VGU_H
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef HDI_VGU_SCALAR_IS_FLOAT
#define HDI_VGU_SCALAR_IS_FLOAT 1

#ifdef HDI_VGU_SCALAR_IS_FLOAT
typedef float VGUScalar;
#else
typedef int32_t VGUScalar;
#endif

#define SURFACE_CLIP_MAX  1
typedef PixelFormat VGUPixelFormat;     /* < Pixel formats */
typedef BlendType VGUBlendType;         /* < Blend types supported by hardware acceleration */

/* *
 * @brief Enumerates data types of paths.
 *
 */
typedef enum {
    VGU_DATA_TYPE_S8 = 0,   /* < signed int(1 byte) */
    VGU_DATA_TYPE_S16,      /* < Signed int(2 bytes) */
    VGU_DATA_TYPE_S32,      /* < Signed int(4 bytes) */
    VGU_DATA_TYPE_F32       /* < Float(4 bytes) */
} VGUPathDataType;

/* *
 * @brief Enumerates supported hardware acceleration capabilities.
 *
 */
typedef enum {
    VGU_CAP_BLIT = 0x01,        /* < Support bit blit */
    VGU_CAP_FILL = 0x02,        /* < Support fill */
    VGU_CAP_PATH = 0x04,        /* < Support path fill and stroke */
    VGU_CAP_BLUR = 0x08,        /* < Support filter blur */
    VGU_CAP_BUTT                /* < Invalid operation */
} VGUCapability;

/* *
 * @brief Enumerates result codes that may return.
 *
 */
typedef enum {
    VGU_SUCCESS = 0,            /* < The operation is successful. */
    VGU_NO_SUPPORT = -1,        /* < This feature is not supported. */
    VGU_OPERATION_FAILED = -2,  /* < The operation failed. */
    VGU_OUT_OF_MEMORY = -3,     /* < The operation ran out of memory. */
    VGU_TIMEOUT = -4,           /* < The operation times out. */
    VGU_INVALID_PARAMETER = -5, /* < One or more parameters are invalid. */
    VGU_BUSY = -6,              /* < The device is busy. */
    VGU_NO_CONTEXT = -7,        /* < There is no context specified. */
} VGUResult;

/* *
 * @brief Enumerates styles for the endpoints of a stroked line.
 *
 */
typedef enum {
    VGU_LINECAP_BUTT = 0,   /* < A line with a squared-off end (default value) */
    VGU_LINECAP_ROUND,      /* < A line with a rounded end */
    VGU_LINECAP_SQUARE      /* < A line with a squared-off end */
} VGULineCap;

/* *
 * @brief Enumerates join types for stroked lines.
 *
 */
typedef enum {
    VGU_LINE_JOIN_MITER = 0,    /* < A join with a sharp corner (default value) */
    VGU_LINE_JOIN_ROUND,        /* < A join with a rounded end */
    VGU_LINE_JOIN_BEVEL,        /* < A join with a squared-off end */
    VGU_LINE_JOIN_BUTT          /* < Invalid definition */
} VGUJointType;

/* *
 * @brief Defines the coordinates of a point.
 *
 */
typedef struct {
    VGUScalar x;    /* < Horizontal coordinate of the point */
    VGUScalar y;    /* < Vertical coordinate of the point */
} VGUPoint;

/* *
 * @brief Defines the information about a rectangle.
 *
 */
typedef struct {
    VGUScalar x; /* < Horizontal coordinate of the start point of the rectangle */
    VGUScalar y; /* < Vertical coordinate of the start point of the rectangle */
    VGUScalar w; /* < Width of the rectangle */
    VGUScalar h; /* < Height of the rectangle */
} VGURect;

/* *
 * @brief Define the filter type for rendering an image.
 *
 */
typedef enum {
    VGU_FILTER_BILINEAR = 0,    /* < Use bilinear interpolation filter. (Default) */
    VGU_FILTER_NEAREST,         /* < No interpolation filter. */
    VGU_FILTER_LINEAR,          /* < Use linear interpolation filter. */
    VGU_FILTER_BUTT             /* < Invalid operation. */
} VGUFilter;

/* *
 * @brief Define fill rules for graphics.
 *
 */
typedef enum {
    VGU_RULE_WINDING = 0,   /* < Non zero winding fill rule. (Default) */
    VGU_RULE_EVEN_ODD,      /* < Even-odd fill rule */
    VGU_RULE_BUTT           /* < Invalid operation */
} VGUFillRule;

/* *
 * @brief Define fill type of outside the gradient area.
 *
 */
typedef enum {
    VGU_SPREAD_PAD = 0,     /* < The area is filled with closest stop color. (Default) */
    VGU_SPREAD_REFLECT,     /* < The gradient is reflected outside area */
    VGU_SPREAD_REPEAT,      /* < The gradient is repeated outside area */
    VGU_SPREAD_BUTT         /* < Invalid operation */
} VGUFillSpread;

/* *
 * @brief Define fill type of pattern.
 *
 */
typedef enum {
    VGU_WRAP_PATTERN_DISABLE = 0,
    VGU_WRAP_PATTERN_FILL,        /* < The pattern is fill with color */
    VGU_WRAP_PATTERN_PAD,         /* < The pattern is pad with img edge pixel */
    VGU_WRAP_PATTERN_REFLECT,     /* < The pattern is reflected */
    VGU_WRAP_PATTERN_REPEAT,      /* < The pattern is repeated */
} VGUWrapType;

/* *
 * @brief Enumerates commands for drawing a path.
 *
 */
typedef enum {
    VGU_PATH_CMD_CLOSE = 0,              /* < Close the current subpath, coordinates: none */
    VGU_PATH_CMD_MOVE  = 1 << 1,         /* < Move to the first point, coordinates: x0, y0 */
    VGU_PATH_CMD_LINE  = 2 << 1,         /* < Line to the last point, coordinates: x0, y0 */
    VGU_PATH_CMD_HLINE = 3 << 1,         /* < Horizontal line, coordinates: x0 */
    VGU_PATH_CMD_VLINE = 4 << 1,         /* < Vertical line, coordinates: y0 */
    VGU_PATH_CMD_QUARD = 5 << 1,         /* < Quadratic bezier, coordinates: x0, y0, x1, y1 */
    VGU_PATH_CMD_CUBIC = 6 << 1,         /* < Cubic bezier, coordinates: x0, y0, x1, y1, x2, y2 */
    VGU_PATH_CMD_SQUARD = 7 << 1,        /* < Smooth quadratic bezier, coordinates: x1, y1 */
    VGU_PATH_CMD_SCUBIC = 8 << 1,        /* < Smooth cubic bezier, coordinates: x1, y1, x2, y2 */
    VGU_PATH_CMD_SCCWARC = 9 << 1,       /* < Elliptical arc, coordinates: rh, rv, rot, x0, y0 */
    VGU_PATH_CMD_SCWARC = 10 << 1,       /* < Elliptical arc, coordinates: rh, rv, rot, x0, y0 */
    VGU_PATH_CMD_LCCWARC = 11 << 1,      /* < Elliptical arc, coordinates: rh, rv, rot, x0, y0 */
    VGU_PATH_CMD_LCWARC = 12 << 1,       /* < Elliptical arc, coordinates: rh, rv, rot, x0, y0 */
    VGU_PATH_CMD_BUTT = 13 << 1          /* < Invalid operation */
} VGUPathCmd;

/* *
 * @brief Define the simple transform flag.
 *
 */
typedef enum {
    VGU_TRANSFORM_COMPLEX = 0x00,       /* < Complex transform */
    VGU_TRANSFORM_SCALE = 0x01,         /* < Transform scale */
    VGU_TRANSFORM_TRANSLATE = 0x02,     /* < Transform translate */
    VGU_TRANSFORM_ROTATE_90 = 0x04,     /* < Transform rotate 90 */
    VGU_TRANSFORM_ROTATE_180 = 0x08,    /* < Transform rotate 180 */
    VGU_TRANSFORM_ROTATE_270 = 0x10,    /* < Transform rotate 270 */
    VGU_RANSFORM_BUTT                   /* < Invalid operation */
} VGUTransformFlag;

/* *
 * @brief Define the arc type.
 *
 */
typedef enum {
    VGU_OPEN_ARC = 0,      /* < ARC open */
    VGU_CHORD_ARC,         /* < Arc chord */
    VGU_PIE_ARC            /* < Arc Pie */
} VGUArcMode;

/* *
 * @brief Define the transform matrix.
 *
 */
typedef struct {
    float m[3][3];          /* < Transform matrix 3x3 */
    VGUTransformFlag flag;  /* < It is used to flag transformation type, as enumerated in {@link VGUTransformFlag} */
} VGUMatrix3;

/* *
 * @brief Compression mode.
 *
 */
typedef enum {
    VGU_COMPRESS_NONE = 0x0,
    VGU_COMPRESS_HFBC,
    VGU_COMPRESS_HFBC_ABYPASS,
    VGU_COMPRESS_HFBC_2X,
    VGU_COMPRESS_HFBC_3X,
    VGU_COMPRESS_HIMC,
    VGU_COMPRESS_MAX,
} VGUCompressMode;

/* *
 * @brief Stores bitmap information for hardware acceleration.
 *
 */
typedef struct {
    VGUPixelFormat pixelFormat;      /* < Pixel format */
    uint32_t width;                  /* < Bitmap width */
    uint32_t height;                 /* < Bitmap height */
    uint32_t stride;                 /* < Bitmap stride */
    uint32_t uvOffset;               /* < Bitmap uvOffset */
    uint64_t memHandle;              /* < memory handle */
    uint32_t color;                  /* < color of the image, valid for format A8 and solid */
    bool isSolid;                    /* < the buffer whether solid */
    VGUCompressMode compressMode;    /* < Compression mode */
} VGUBuffer;

/* *
 * @brief Stores surface information for hardware acceleration.
 *
 */
typedef struct {
    VGUBuffer buffer;                    /* < Bitmap buffer */
    uint32_t clipNum;                    /* < Count of clipRects */
    VGURect clipRects[SURFACE_CLIP_MAX]; /* < Surface clip rect. If it is null, the whole surface will be drawn */
} VGUSurface;

/* *
 * @brief Define how the colors are distributed along the gradient.
 *
 */
typedef struct {
    float stop;     /* < Stop position. The value ranges from 0.0 to 1.0. */
    uint32_t color; /* < Color of the stop */
} VGUColorStop;

/* *
 * @brief Defines a linear gradient.
 *
 */
typedef struct {
    VGUScalar x1; /* < Horizontal coordinate of the start point of the linear gradient */
    VGUScalar y1; /* < Vertical coordinate of the start point of the linear gradient */
    VGUScalar x2; /* < Horizontal coordinate of the end point of the linear gradient */
    VGUScalar y2; /* < Vertical coordinate of the end point of the linear gradient */
} VGULinear;

/* *
 * @brief Defines a radial gradient.
 *
 */
typedef struct {
    VGUScalar x0; /* < Horizontal coordinate of the center of the inner circle */
    VGUScalar y0; /* < Vertical coordinate of the center of the inner circle */
    VGUScalar r0; /* < Radius of the inner circle */
    VGUScalar x1; /* < Horizontal coordinate of the center of the outer circle */
    VGUScalar y1; /* < Vertical coordinate of the center of the outer circle */
    VGUScalar r1; /* < Radius of the outer circle */
} VGURadial;

/* *
 * @brief Define a sweep gradient.
 *
 */
typedef struct {
    VGUScalar cx; /* < The center x coordinate of the circle */
    VGUScalar cy; /* < The center y coordinate of the circle */
    VGUScalar startAngle; /* < Start angle of sweep gradient */
} VGUSweep;

/* *
 * @brief Define HSV adjust info.
 *
 */
typedef struct {
    int16_t hAdj;         /* < Indicates the adjust value of Hue: [-359, 359] */
    int8_t sAdj;          /* < Indicates the adjust value of Saturation: [-100, 100] */
    int8_t vAdj;          /* < Indicates the adjust value of Value: [-100, 100] */
    uint8_t threshold;    /* < Indicates the threshold of mask value: [0, 255] */
} VGUHSVInfo;

/* *
 * @brief Define an image object.
 *
 */
typedef struct {
    VGUBuffer buffer;       /* < Image buffer */
    VGUMatrix3 matrix;      /* < Matrix to transform the image. If it is null, the matrix is identity */
    VGURect rect;           /* < Specifies the rectangle of image. If it is null, the rect is full buffer. */
    VGURect dstRect;        /* < Specifies the dst rectangle of image. If it is null, dst rect is full screen. */
    uint8_t globalAlpha;    /* < Transparency value of the drawing */
    VGUFilter filter;       /* < Specifies the filter type. */
    bool enCompressed;      /* < Specifies the compressed status. */
    bool premultiplied;     /* < Specifies the premultiplied status. */
    int32_t fence;          /* < Indicates the fd of a sync file. */
    VGUBlendType blend;     /* < Blend type, how the image is drawn onto an existing surface */
    bool enColorize;        /* < Colorize enable. */
} VGUImage;

/* *
 * @brief Defines an image pattern.
 *
 */
typedef struct {
    VGUImage *image;    /* < Pointer to the image object */
    VGUWrapType wrapx;  /* < Wrap the image horizontally. */
    VGUWrapType wrapy;  /* < Wrap the image vertically. */
} VGUPattern;

/* *
 * @brief Enumerates light types.
 *
 */
typedef enum {
    VGU_LIGHT_CIRCLE_RADIAL = 0, // 圆形区域，径向衰减
    VGU_LIGHT_CIRCLE_CONST,      // 圆形区域，径向不变
    VGU_LIGHT_FAN_RADIAL,        // 扇形区域，径向衰减
    VGU_LIGHT_FAN_CONST,         // 扇形区域，径向不变
    VGU_LIGHT_FAN_SWEEP_RADIAL,  // 扇形区域，径向/角度衰减
    VGU_LIGHT_FAN_SWEEP_CONST,   // 扇形区域，径向不变，角度衰减
    VGU_LIGHT_BUTT
} VGULightType;

/* *
 * @brief Defines a light style.
 *
 */
typedef struct {
    VGULightType lightMode;   // 光照模式
    VGUScalar cx;             // 圆心的X坐标
    VGUScalar cy;             // 圆心的Y坐标
    VGUScalar radius;         // 圆的半径
    VGUScalar speAmp;         // 高光反射的强度，值越大，高光越亮
    VGUScalar ambAmp;         // 环境光的强度，基础照明，影响整体亮度
    VGUScalar startAngle;     // 扇形起始角度
    VGUScalar endAngle;       // 扇形结束角度
    bool isSymmetrical;       // 是否对称
} VGULightStyle;

/* *
 * @brief Enumerates gradient types.
 *
 */
typedef enum {
    VGU_GRADIENT_LINEAR = 0,    /* < Linear gradient */
    VGU_GRADIENT_RADIAL,        /* < Radial gradient */
    VGU_GRADIENT_SWEEP,         /* < Sweep gradient */
    VGU_GRADIENT_BUTT           /* < Invalid operation */
} VGUGradientType;

/* *
 * @brief Define a gradient object.
 *
 */
typedef struct {
    VGUColorStop *colorStops;   /* < Color stops */
    uint16_t stopCount;         /* < Color stop count */
    union {
        VGULinear linear;       /* < Linear gradient object */
        VGURadial radial;       /* < Radial gradient object */
        VGUSweep sweep;         /* < Sweep gradient object */
    };
    VGUGradientType type;       /* < Gradient type */
    VGUFillSpread spread;       /* < Gradient spread mode */
} VGUGradient;

/* *
 * @brief Enumerates paint types.
 *
 */
typedef enum {
    VGU_PAINT_GRADIENT,
    VGU_PAINT_PATTERN,
    VGU_PAINT_COLOR,
    VGU_PAINT_COLOR_ALPHA_FF,
    VGU_PAINT_BUTT
} VGUPaintType;

/* *
 * @brief Defines the paint style when filling or stroking a path.
 *
 */
typedef struct {
    union {
        VGUGradient *gradient;  /* < Gradient object */
        VGUPattern *pattern;    /* < Pattern object */
        uint32_t color;         /* < Color solid ARGB8888 */
    };
    VGUPaintType type;          /* < Enable paint style to color */
} VGUPaintStyle;

/* *
 * @brief Defines path filling attributes.
 *
 */
typedef struct {
    VGUFillRule rule;     /* < The fill rule */
    VGUPaintStyle *style; /* < paint style for fill */
} VGUFillAttr;

/* *
 * @brief Defines path stroking attributes.
 *
 */
typedef struct {
    VGULineCap cap;     /* < Line cap style */
    VGUJointType join;  /* < Join type */
    float miterLimit;   /* < Miter limit */
    float width;        /* < Line width */
    VGUPaintStyle *style; /* < paint style for stroke */
} VGUStrokeAttr;

/* *
 * @brief Defines the blit capability.
 *
 */
typedef struct {
    uint32_t maxImageNum;             /* < max number of support input images */
    uint32_t maxSolidImageNum;        /* < max number of support input solid image */
    uint32_t minWidth;                /* < the min width of input images */
    uint32_t maxWidth;                /* < the max width of input images */
    uint32_t minHeight;               /* < the min height of input images */
    uint32_t maxHeight;               /* < the max height of input images */
    uint32_t maxHorizontalRatio;      /* < the maximum horizontal reduction factor */
    uint32_t maxVerticalRatio;        /* < the maximum vertical reduction factor */
    bool supportClip;                 /* < the target surface whether support clip */
} VGUBlitCapability;

/* *
 * @brief Defines driver functions for 2D hardware acceleration.
 */
typedef struct {
    /* *
     * @brief Initializes hardware acceleration.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @see DeinitVgu
     * @since 3.0
     */
    VGUResult (*InitVgu)(void);

    /* *
     * @brief Deinitializes hardware acceleration.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @see InitVgu
     * @since 3.0
     */
    VGUResult (*DeinitVgu)(void);

    /* *
     * @brief Queries hardware acceleration capabilities.
     *
     * @param cap Indicates the capabilities to query, which are defined by <b>VGUCapability</b>.
     *
     * @return Returns a value greater than or equal to 0 if the operation is successful;
     * returns an error code defined in {@link VGUResult} otherwise.
     * @since 3.0
     */
    int32_t (*QueryCapability)(uint32_t *cap);

    /* *
     * @brief Queries hardware acceleration blit capabilities
     *
     * @param cap Indicates the capabilities of blit, which are defined by <b>VGUBlitCapability</b>.
     *
     * @return Returns a value greater than or equal to 0 if the operation is successful;
     * returns an error code defined in {@link VGUResult} otherwise.
     * @since 3.0
     */
    int32_t (*QueryBlitCapability)(VGUBlitCapability *cap);


    /* *
     * @brief start to render in the new display cycle
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*StartRender)(void);

    /* *
     * @brief Blur the target surface.
     *
     * @param target Pointer to the target surface.
     * @param blur Specifies the blur radius.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlur)(VGUSurface *target, float blur);

    /* *
     * @brief Blur the target surface by mask.
     *
     * @param target Pointer to the target surface.
     * @param blur Specifies the blur radius.
     * @param quality Specifies the blur quality.
     * @param maskInfo Specifies the blur mask.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlurExt)(VGUSurface *target, float blur, uint32_t quality, VGUSurface *maskInfo);

    /* *
     * @brief Lighten the target surface.
     *
     * @param target Pointer to the target surface.
     * @param lishtStyle Specifies the light param.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderLighting)(VGUSurface *target, VGULightStyle lishtStyle);

    /* *
     * @brief Blits one or multiple images to target surface.
     *
     * During bit blit, color space conversion (CSC), transform can be implemented.
     * Users can query the number of composite images supported by hardware through the interface
     * <b>QueryCapability<b/>.
     *
     * @param target Pointer to the target surface.
     * @param src Indicates the array of the src image.
     * @param count Indicates the count of the src image.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlit)(VGUSurface *target, const VGUImage *src, uint16_t count);

    /* *
     * @brief Clear a rectangle with a given color to the surface.
     *
     * @param target Pointer to the target surface.
     * @param rect Pointer the rectangle which the target surface will be cleared.
     * if it is nullptr, the whole surface will be cleared.
     * @param color The specified color to clear target surface.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderClearRect)(VGUSurface *target, const VGURect *rect, uint32_t color);

    /* *
     * @brief Cancel the hardware acceleration to render.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderCancel)(void);

    /* *
     * @brief submit the render commands to hardware.
     *
     * @param isSync Indicates hardware acceleration is synchronous or asynchronous.
     * The value <b>true</b> indicates synchronous, and <b>false</b> indicates asynchronous.
     * @param fence Is the fd of sync file, which indicates whether the render has been completed,
     * it is only used for asynchronous render, if not support sync file, the fence will return -1, and the user will
     * use the RenderWaitDone to make sure all the commands have been completed.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderSubmit)(bool isSync, int32_t *fence);

    /* *
     * @brief wait until all the render commands, that have been submitted to hardware, are completed.
     *
     * @param Indicates the timeout duration for hardware acceleration render. The value <b>0</b>
     * indicates no timeout, so the process waits until hardware acceleration is complete.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderWaitDone)(int32_t timeOut);

    /* *
    * @brief Initialize the path object.
    * @param enAlias Enable the anti alias.
    * @param boundBox The bounding box of path.
    *
    * @return Returns <b>greater than 0</b> if the operation is successful; otherwise returns 0.
    * @since 3.0
    */
    uintptr_t (*VGUPathInit)(VGUPathDataType type, bool enAlias);

    /* *
    * @brief Append the path data to the current path.
    *
    * cmd and data will be copied into new memory, so they can be freed immediately.
    *
    * @param path Pointer to the path object.
    * @param num Number of the path commands.
    * @param cmd Pointer to the path commands array.
    * @param dataNum Number of the path data.
    * @param data Pointer to the data array, the type of data is specified by parameter type in <b>VGUPathInit</b>,
    * the data of each command is specified by the comments for VGUPathCmd.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*VGUPathAppendData)(uintptr_t path, uint32_t num, const uint8_t *cmd,
        uint32_t dataNum, const void *data);

    /* *
    * @brief Set the path data to path without memcpy.
    *
    * cmd and data will be used directly during render without memcpy, so they should not be freed before
    * finishing the render process.
    *
    * @param path Pointer to the path object.
    * @param num Number of the path commands.
    * @param cmd Pointer to the path commands array.
    * @param dataNum Number of the path data.
    * @param data Pointer to the data array, the type of data is specified by parameter type in <b>VGUPathInit</b>,
    * the data of each command is specified by the comments for VGUPathCmd.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*VGUPathSetData)(uintptr_t path, uint32_t num, const uint8_t *cmd, uint32_t dataNum,
        const void *data);

    /* *
    * @brief Append subpath to the current path.
    *
    * @param path Pointer to the path handle.
    * @param subpath Pointer to the subpath handle.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*VGUPathAppendPath)(uintptr_t path, uintptr_t subPath);

    /* *
     * @brief render the given path with paint style.
     *
     * @param target Pointer to the target surface.
     * @param path Pointer to the path object.
     * @param matrix Pointer to the transform matrix object. If matrix is null, it will use the identity matrix.
     * @param attr Specifies the path attribute.
     * @param paint Pointer to the fill style which will be painted.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderPath)(VGUSurface *target, uintptr_t path, const VGUMatrix3 *matrix,
        const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr);

    /* *
    * @brief Clear the path object.
    *
    * @param path Indicates the pointer the path object.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*VGUPathClear)(uintptr_t path);

    /* *
    * @brief Clip a img by path.
    *
    * @param target Pointer to the target surface.
    * @param src Pointer to the source img.
    * @param path Pointer to the path object.
    * @param matrix Pointer to a path transformation matrix.
    * @param fillAttr Pointer to the fill attributes.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*ClipImgByPath)(VGUSurface *target, const VGUImage *src,
        uintptr_t path, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr);

    /* *
    * @brief Adjust image HSV value.
    *
    * @param target Pointer to the target surface.
    * @param src Pointer to the source img.
    * @param mask Pointer to the mask img.
    * @param hsvInfo Specifies the hsv adjust info.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*RenderHSV)(VGUSurface *target, const VGUImage *src, const VGUImage *mask, const VGUHSVInfo *hsvInfo);

    /* *
    * @brief Blend rain image on target.
    *
    * @param target Pointer to the target surface.
    * @param rain Pointer to the rain img.
    *
    * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
    * {@link VGUResult} otherwise.
    * @since 3.0
    */
    VGUResult (*RenderRain)(VGUSurface *target, const VGUImage *rain);
} VGUFuncs;

/* *
 * @brief Initializes the hardware acceleration module to obtain the pointer to functions for
 * hardware acceleration operations.
 *
 * @param funcs Indicates the double pointer to the functions for hardware acceleration operations.
 * Memory is allocated automatically when you initiate the hardware acceleration module, so you can simply use
 * the pointer to gain access to the functions.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 *
 * @since 3.0
 */
VGUResult VGUInitialize(VGUFuncs **funcs);

/* *
 * @brief Deinitializes the hardware acceleration module to release the pointer to functions
 * for hardware acceleration operations.
 *
 * @param funcs Indicates the pointer to the functions for hardware acceleration operations.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUUninitialize(VGUFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif
