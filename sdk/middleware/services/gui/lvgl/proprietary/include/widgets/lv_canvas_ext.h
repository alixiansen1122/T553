/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_CANVAS_EXT_H
#define LV_CANVAS_EXT_H

#include "stdint.h"
#include "lvgl.h"
#include "lv_draw_vgu.h"
#include "lv_transform.h"

#if LV_USE_CANVAS_EXT

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_obj_class_t LvCanvasExtClass;

typedef enum {
    /* Stroke only */
    STROKE_STYLE = 1,
    /* Fill only */
    FILL_STYLE,
    /* Stroke and fill */
    STROKE_FILL_STYLE,
} LvPaintStyle;

typedef enum {
    CMD_CLOSE = 0,
    CMD_MOVE_TO = 1 << 1,
    CMD_LINE_TO = 2 << 1,
    CMD_HLINE_TO = 3 << 1,
    CMD_VLINE_TO = 4 << 1,
    CMD_QUARD_TO = 5 << 1,
    CMD_CUBIC_TO = 6 << 1,
    CMD_SQUARD_TO = 7 << 1,
    CMD_SCUBIC_TO = 8 << 1,
    CMD_SCCWARC_TO = 9 << 1,
    CMD_SCWARC_TO = 10 << 1,
    CMD_LCCWARC_TO = 11 << 1,
    CMD_LCWARC_TO = 12 << 1,
} LvPathCmd;

typedef struct {
    uint32_t cmdNum;   // number of commands.
    uint8_t* cmd;      // command.
    uint32_t dataNum;  // number of data.
    float* data;       // data.
} LvPath;

/**
 * @brief Enumerates fill rules.
 */
typedef enum {
    NON_ZERO = 0,
    EVEN_ODD,
} LvFillRule;

/**
 * @brief Enumerates join types for stroke.
 */
typedef enum {
    JOIN_MITER = 0,
    JOIN_ROUND,
    JOIN_BEVEL,
} LvJoinType;

/**
 * @brief Enumerates cap types.
 */
typedef enum {
    /* No cap style */
    CAP_NONE,
    /* Round cap style */
    CAP_ROUND,
    /* Square cap style */
    CAP_SQUARE,
} LvCapType;

/**
 * @brief Enumerates gradient types.
 */
typedef enum {
    GRADIENT_LINEAR,
    GRADIENT_RADIAL,
    GRADIENT_SWEEP,
    GRADIENT_BUTT,
} LvGradientType;

typedef enum {
    SPREAD_PAD,
    SPREAD_REFLECT,
    SPREAD_REPEAT,
} LvGradientSpread;

typedef struct {
    int16_t startX;
    int16_t startY;
    int16_t endX;
    int16_t endY;
} LvLinearGradient;

typedef struct {
    int16_t centerX;
    int16_t centerY;
    int16_t focalX; /* currently not supported */
    int16_t focalY; /* currently not supported */
    int16_t radius;
} LvRadialGradient;

typedef struct {
    int16_t centerX;
    int16_t centerY;
    int16_t startAngle;
} LvSweepGradient;

typedef struct {
    float stop;
    uint32_t color;
} LvColorStop;

typedef struct {
    LvGradientType gradientType;
    LvGradientSpread gradientSpread;
    union {
        LvLinearGradient linearGradient;
        LvRadialGradient radialGradient;
        LvSweepGradient sweepGradient;
    };
    LvColorStop* stops;
    uint16_t stopCount;
} LvGradientAttr;

typedef enum {
    SHADER_COLOR,
    SHADER_GRADIENT,
    SHADER_PATTERN,
} LvShaderType;

typedef enum {
    PATTERN_BUTT,
    PATTERN_FILL,
    PATTERN_PAD,
} LvPatternType;

typedef struct {
    char* src; /* img file path */
    LvPatternType type; /* pattern type */
    uint32_t fillColor; /* fill color is used when type is PATTERN_FILL */
    LvMatrix matrix; /* img transformation matrix, set identity matrix if there is no transformation */
} LvPatternAttr;

typedef struct {
    LvShaderType type;
    union {
        uint32_t color;
        LvGradientAttr gradient;
        LvPatternAttr pattern;
    };
} LvShaderAttr;

typedef struct {
    LvFillRule fillRule;
    LvShaderAttr shader;
} LvFillAttr;

typedef struct {
    LvCapType capType;
    LvJoinType joinType;
    float miterLimit;
    uint16_t strokeWidth;
    LvShaderAttr shader;
} LvStrokeAttr;

typedef struct {
    LvPaintStyle style;
    LvStrokeAttr strokeAttr;
    LvFillAttr fillAttr;
    bool isAntialiased;
} LvPaint;

typedef struct {
    LvPath path;
    LvPaint paint;
    LvMatrix matrix;
    int16_t pathId;
} LvPathInfo;

typedef struct {
    lv_obj_t obj;
    lv_ll_t pathInfos;
    int16_t pathCount;
    bool isPathPending;
    LvTransform* transform;
} LvCanvasExt;

static const int16_t LV_INVALID_PATH_ID = -1;

/**
 * @brief  Create a canvas object.
 * @param  [in]  parent    Pointer to a parent object.
 * @return Pointer to the created object.
 */
lv_obj_t* LvCanvasExtCreate(lv_obj_t* parent);

/**
 * @brief  Begin a path. This method would init a new path in the context.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  paint    Pointer to a paint object.
 * @return Return pathId if success; otherwise, return LV_INVALID_PATH_ID;
 */
int16_t LvCanvasExtBeginPath(lv_obj_t* obj, const LvPaint* paint);

/**
 * @brief  Set path data to the current path in the context. User should call this after beginning a path.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  cmdNum    Size of draw commands.
 * @param  [in]  cmd    Draw commands.
 * @param  [in]  dataNum    Size of path data.
 * @param  [in]  data    Path data.
 */
void LvCanvasExtSetPathData(lv_obj_t* obj, uint32_t cmdNum, const uint8_t* cmd,
    uint32_t dataNum, const float* data);

/**
 * @brief  Set path data of a path object defined by the pathId. User should set path data after beginning a path.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  path    Pointer to LvPath.
 * @param  [in]  pathId    PathId returned by other functions.
 */
void LvCanvasExtUpdatePathData(lv_obj_t* obj, const LvPath* path, int16_t pathId);

/**
 * @brief  Set transformation matrix of a path object defined by the pathId.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  matrix    Transformation matrix.
 * @param  [in]  pathId    PathId returned by other functions.
 */
void LvCanvasExtSetMatrix(lv_obj_t* obj, const LvMatrix* matrix, int16_t pathId);

/**
 * @brief  Set drawing flag of the current pending path.
 * @param  [in]  obj    Pointer to a canvas object.
 */
void LvCanvasExtDrawPath(lv_obj_t* obj);

/**
 * @brief  Clear contents of a canvas object, including paths, paints, and matrices, and will invalidate the view.
 * @param  [in]  obj    Pointer to a canvas object.
 */
void LvCanvasExtClear(lv_obj_t* obj);

/**
 * @brief  Draws a straight line.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  startPoint    Start point of the line.
 * @param  [in]  endPoint    End point of the line.
 * @param  [in]  paint    Pointer to a paint object.
 * @return Return pathId if success; otherwise, return LV_INVALID_PATH_ID;
 */
int16_t LvCanvasExtDrawLine(lv_obj_t* obj, const lv_point_t startPoint,
    const lv_point_t endPoint, const LvPaint* paint);
/**
 * @brief  Draws a cubic Bezier curve.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  startPoint    Start point.
 * @param  [in]  control1    First control point.
 * @param  [in]  control2    Second control point.
 * @param  [in]  endPoint    End point.
 * @param  [in]  paint    Pointer to a paint object.
 * @return Return pathId if success; otherwise, return LV_INVALID_PATH_ID;
 */
int16_t LvCanvasExtDrawCurve(lv_obj_t* obj, const lv_point_t startPoint, const lv_point_t control1,
    const lv_point_t control2, const lv_point_t endPoint, const LvPaint* paint);

/**
 * @brief  Draws an arc.
 * @param  [in]  obj    Pointer to a canvas object.
 * @param  [in]  center    Center of arc.
 * @param  [in]  radius    Radius of arc.
 * @param  [in]  startAngle    Start angle.
 * @param  [in]  endAngle    End angle.
 * @param  [in]  paint    Pointer to a paint object.
 * @return Return pathId if success; otherwise, return LV_INVALID_PATH_ID;
 */
int16_t LvCanvasExtDrawArc(lv_obj_t* obj, const lv_point_t center, uint16_t radius,
    int16_t startAngle, int16_t endAngle, const LvPaint* paint);

/**
 * @brief  User has to call this method to set pattern's src before beginning a patterned path.
 *         This will allocate memory for src of LvPatternAttr struct, and copy the value from the second parameter.
 *         After begins a patterned path with the LvPatternAttr, deallocation of src will be taken care of.
 *         Otherwise, user should call lv_mem_free to release the memory of src manually.
 * @param  [in]  pattern    the pointer of LvPatternAttr.
 * @param  [in]  srcPath    the file path of the img src.
 */
void LvInitPatternSrc(LvPatternAttr* pattern, const char* srcPath);

/**
 * @brief  Rotate obj.
 */
void LvCanvasExtRotate(lv_obj_t* obj, float angle, LvVector3 pivot1, LvVector3 pivot2);

/**
 * @brief  Scale obj.
 */
void LvCanvasExtScale(lv_obj_t* obj, LvVector3 scale, LvVector3 pivot);

/**
 * @brief  Translate obj.
 */
void LvCanvasExtTranslate(lv_obj_t* obj, LvVector3 translate);

/**
 * @brief  Set camera distance.
 */
void LvCanvasExtSetCamDistance(lv_obj_t* obj, float distance);

/**
 * @brief  Set camera position.
 */
void LvCanvasExtSetCamPosition(lv_obj_t* obj, LvVector2 pos);

/**
 * @brief  Set transform.
 */
void LvCanvasExtSetTransform(lv_obj_t* obj, const LvTransform* transform);

/**
 * @brief  Get transform.
 */
LvTransform* LvCanvasExtGetTransform(lv_obj_t* obj);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_USE_CANVAS_EXT
#endif // LV_CANVAS_EXT_H