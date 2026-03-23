/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialBoxProgressView.h"

namespace OHOS {
DialBoxProgressView::DialBoxProgressView(void)
{
}

DialBoxProgressView::~DialBoxProgressView(void)
{
}

void DialBoxProgressView::HandleFloatData(float data)
{
    float ratio = data;
    if (ratio > 1.0f) {
        ratio = 1.0f;
    } else if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    int32_t value = ratio * (GetRangeMax() - GetRangeMin());
    if (value_ == value) {
        return;
    }
    value_ = value;
    SetValue(value_);
}

void DialBoxProgressView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (foregroundStopCount_ != 0) {
        Rect rect = GetOrigRect();
        foregroundGradient_.startX = rect.GetX();
        foregroundGradient_.startY = rect.GetY();
        foregroundGradient_.endX = rect.GetRight();
        foregroundGradient_.endY = rect.GetBottom();
    }
    UIBoxProgress::OnDraw(gfxDstBuffer, invalidatedArea);
}
}