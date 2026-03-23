/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_ROTATE_IMG_VIEW_H
#define DIAL_ROTATE_IMG_VIEW_H

#include <cstdint>
#include "main/dial/DialView.h"
#include "components/ui_image_view.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {

class DialRotateImgView : public UIImageView, public DialView {
public:
    DialRotateImgView();
    ~DialRotateImgView() override;

    void HandleFloatData(float data) override;

    void SetRotatePivot(const Vector2<float>& pivot);
    void SetRotateStart(float rotateStart);
    void SetRotateEnd(float rotateEnd);

private:
    Vector2<float> rotatePivot_ = {0.0f, 0.0f};
    float rotateStart_ = 0.0f;
    float rotateEnd_ = 360.0f;
    bool isRotateSet_ = false;
    float rotate_ = 0.0f;
};
}
#endif
