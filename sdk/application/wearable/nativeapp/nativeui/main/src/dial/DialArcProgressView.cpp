/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "main/dial/DialArcProgressView.h"

namespace OHOS {
DialArcProgressView::DialArcProgressView(void)
{
}

DialArcProgressView::~DialArcProgressView(void)
{
}

void DialArcProgressView::HandleFloatData(float data)
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

void DialArcProgressView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (foregroundStopCount_ != 0) {
        Rect rect = GetOrigRect();
        foregroundGradient_.startX = rect.GetX();
        foregroundGradient_.startY = rect.GetY();
        foregroundGradient_.endX = rect.GetRight();
        foregroundGradient_.endY = rect.GetBottom();
    }
    UICircleProgress::OnDraw(gfxDstBuffer, invalidatedArea);
}
}