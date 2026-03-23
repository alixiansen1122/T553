/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: CompassCalibrationView.h
 * Author:
 * Create: 2022-04-02
 */

#ifndef COMPASS_CALIBRATION_VIEW_H
#define COMPASS_CALIBRATION_VIEW_H

#include "components/ui_label.h"
#include "View.h"
#include "ohos_types.h"
#include "AppGroupView.h"
#include "components/ui_image_view.h"

namespace OHOS {
class CompassCalibrationView : public AppGroupView {
public:
    CompassCalibrationView();
    ~CompassCalibrationView() override;

    bool CreateUIViewGroup(void);
    static CompassCalibrationView *GetInstance(void);
    bool InitView(void *caller) override;
private:
    UILabel *labelOne{nullptr};
    UILabel *labelTwo{nullptr};
    UIImageView *imageView{nullptr};

    void ClearAll(void);
};
}
#endif