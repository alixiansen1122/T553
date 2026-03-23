/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialRotateImgView.h"

namespace OHOS {
DialRotateImgView::DialRotateImgView(void)
{
    SetAutoEnable(true);
}

DialRotateImgView::~DialRotateImgView(void)
{
}

void DialRotateImgView::SetRotatePivot(const Vector2<float>& pivot)
{
    rotatePivot_ = pivot;
}

void DialRotateImgView::SetRotateStart(float rotateStart)
{
    rotateStart_ = rotateStart;
}

void DialRotateImgView::SetRotateEnd(float rotateEnd)
{
    rotateEnd_ = rotateEnd;
}

void DialRotateImgView::HandleFloatData(float data)
{
    float min = MATH_MIN(rotateStart_, rotateEnd_);
    float max = MATH_MAX(rotateEnd_, rotateStart_);

    float angle = data;
    if (angle < min) {
        angle = min;
    } else if (angle > max) {
        angle = max;
    }
    if (isRotateSet_ && FloatEqual(rotate_, angle)) {
        return;
    }
    rotate_ = angle;
    isRotateSet_ = true;
    Rotate(rotate_, rotatePivot_);
}
}