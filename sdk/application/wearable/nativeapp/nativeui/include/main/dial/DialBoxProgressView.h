/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_BOX_PROGRESS_VIEW_H
#define DIAL_BOX_PROGRESS_VIEW_H

#include "main/dial/DialView.h"
#include "components/ui_box_progress.h"

namespace OHOS {
class DialBoxProgressView : public UIBoxProgress, public DialView {
public:
    DialBoxProgressView();
    ~DialBoxProgressView() override;
    void HandleFloatData(float data) override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
private:
    int32_t value_ = -1;
};
}
#endif