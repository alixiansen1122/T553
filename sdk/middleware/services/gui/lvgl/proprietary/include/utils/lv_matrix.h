/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_MATRIX_H
#define LV_MATRIX_H

#include <stdbool.h>
#include "lv_conf.h"

#if LV_USE_VGU

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float m[3][3];
} LvMatrix;

typedef struct {
    float m[4][4];
} LvMatrix4;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} LvVector4;

typedef struct {
    float x;
    float y;
    float z;
} LvVector3;

typedef struct {
    float x;
    float y;
} LvVector2;

/**
 * @brief  Init a identity matrix.
 * @param  [in]  mat    Pointer to a matrix object.
 */
void LvInitIdentityMatrix(LvMatrix* mat);

/**
 * @brief  Do matrix multiplication.
 * @param  [in]  res    Pointer to the result matrix object.
 * @param  [in]  mat1    Pointer to the left matrix object.
 * @param  [in]  mat2    Pointer to the right matrix object.
 */
void LvMatrixMultiply(LvMatrix* res, const LvMatrix* mat1, const LvMatrix* mat2);

/**
 * @brief  Check whether the matrix simply translates the target object.
 * @param  [in]  matrix    Pointer to a matrix object.
 * @return Return <b>true</b> if the matrix simply translates the target object; return <b>false</b> otherwise.
 *                Identity matrix also returns true.
 */
bool IsTranslationMatrix(const LvMatrix* matrix);

/**
 * @brief  Produce a rotating matrix.
 */
void LvMatrixRotate(LvMatrix* res, float angle, LvVector2 pivot);

/**
 * @brief  Produce a scaling matrix.
 */
void LvMatrixScale(LvMatrix* res, LvVector2 scale, LvVector2 fixed);

/**
 * @brief  Produce a translating matrix.
 */
void LvMatrixTranslate(LvMatrix* res, LvVector2 trans);

/**
 * @brief  Print a matrix.
 */
void LvPrintMatrix(const LvMatrix* mat);

/**
 * @brief  Init a identity matrix4.
 */
void LvInitIdentityMatrix4(LvMatrix4* mat);

/**
 * @brief  Produce a rotating matrix4.
 */
void LvMatrix4Rotate(LvMatrix4* res, float angle, LvVector3 pivot1, LvVector3 pivot2);

/**
 * @brief  Produce a scaling matrix4.
 */
void LvMatrix4Scale(LvMatrix4* res, LvVector3 scale, LvVector3 fixed);

/**
 * @brief  Produce a translating matrix4.
 */
void LvMatrix4Translate(LvMatrix4* res, LvVector3 trans);

/**
 * @brief  Produce a shearing matrix4.
 */
void LvMatrix4Shear(LvMatrix4* res, LvVector2 shearX, LvVector2 shearY, LvVector2 shearZ);

/**
 * @brief  Do matrix4 mulplication.
 */
void LvMatrix4Multiply(LvMatrix4* res, const LvMatrix4* mat1, const LvMatrix4* mat2);

/**
 * @brief  Convert matrix from matrix4.
 */
void LvGetMatrixFromMatrix4(LvMatrix* res, const LvMatrix4* mat);

/**
 * @brief  Multiply matrix4 by vector4.
 */
void LvMatrix4MultiplyVec4(LvVector4* res, const LvMatrix4* mat, const LvVector4* vec);

/**
 * @brief  Print a matrix4.
 */
void LvPrintMatrix4(const LvMatrix4* mat);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_USE_VGU

#endif // LV_MATRIX_H