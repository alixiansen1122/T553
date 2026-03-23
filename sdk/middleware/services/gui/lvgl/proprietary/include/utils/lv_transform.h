/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_TRANSFORM_H
#define LV_TRANSFORM_H

#include "lv_matrix.h"
#include "misc/lv_area.h"

typedef enum {
    ROTATE,
    SCALE,
    SHEAR,
    TRANSLATE,
    TRANSFORM_MAX,
} LvTransformOp;

#define VERTEX_SIZE 4

typedef struct {
    lv_area_t area;
    float camDistance;
    LvVector2 camPos;
    float angle;
    LvVector3 rotatePivotStart;
    LvVector3 rotatePivotEnd;
    LvVector3 scale;
    LvVector3 scalePivot;
    LvVector3 translate;
    LvVector2 shearX;
    LvVector2 shearY;
    LvVector2 shearZ;
    uint8_t operations[TRANSFORM_MAX];
    LvMatrix4 matrices[TRANSFORM_MAX];
    LvMatrix4 transformMatrix;
    LvMatrix4 origTransformMatrix;
    bool isInternal;
    lv_point_t vertexes[VERTEX_SIZE];
} LvTransform;

/**
 * @brief  Init a transform struct.
 */
void LvInitTransform(LvTransform* transform);

/**
 * @brief  Rotate the base area.
 */
void LvTransformRotate(LvTransform* transform, float angle, LvVector3 pivot1, LvVector3 pivot2);

/**
 * @brief  Scale the base area.
 */
void LvTransformScale(LvTransform* transform, LvVector3 scale, LvVector3 pivot);

/**
 * @brief  Translate the base area.
 */
void LvTransformTranslate(LvTransform* transform, LvVector3 translate);

/**
 * @brief  Shear the base area.
 */
void LvTransformShear(LvTransform* transform, LvVector2 x, LvVector2 y, LvVector2 z);

/**
 * @brief  Set camera distance. Default value is 1000 if not set.
 */
void LvTransformSetCamDistance(LvTransform* transform, float distance);

/**
 * @brief  Set camera position.
 */
void LvTransformSetCamPosition(LvTransform* transform, LvVector2 pos);

/**
 * @brief  Get transform matrix4.
 */
void LvTransformGetTransformMatrix4(LvMatrix4* mat, const LvTransform* transform);

/**
 * @brief  Set a transform matrix4.
 */
void LvTransformSetTransformMatrix(LvTransform* transform, const LvMatrix4* matrix);

/**
 * @brief  Get transformed area.
 */
lv_area_t LvTransformGetTransformedArea(LvTransform* transform);

/**
 * @brief  Set the base area for transformation.
 */
void LvTransformSetBaseArea(LvTransform* transform, lv_area_t area);
#endif