/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_ARC_PROGRESS_VIEW_H
#define DIAL_ARC_PROGRESS_VIEW_H

#include "main/dial/DialView.h"
#include "components/ui_circle_progress.h"

namespace OHOS {
class DialArcProgressView : public UICircleProgress, public DialView {
public:
    DialArcProgressView();
    ~DialArcProgressView() override;
    void HandleFloatData(float data) override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
private:
    int32_t value_ = -1;
};
}
#endif