/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: CompassView.h
 * Author:
 * Create: 2021-12-21
 */

#ifndef COMPASS_VIEW_H
#define COMPASS_VIEW_H

#include "View.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "ChangeSliceListener.h"
#include "UiConfig.h"
#include "AppGroupView.h"
#include "CompassCalibrationView.h"

namespace OHOS {
static constexpr float VIEW_CENTERS = 227;

class CompassView : public AppGroupView {
public:
    CompassView();
    ~CompassView() override;

    void DirectionSetText(const char *text, const char *angle);
    static CompassView *GetInstance(void);
    void CompassStartRotate(int16_t numb);
    bool InitPage(void);
    bool InitView(void *caller) override;
    bool GetInitViewState(void);
    bool InitCompassImage(void);

private:
    bool LoadCompassImage(UIImageView *view, uint32_t resId);
    void ClearAll(void);
    UILabel *directionLabel{nullptr};
    UILabel *angledLabel{nullptr};
    UIImageView *backGround{nullptr};
    UIImageView *rotateShaft{nullptr};
    UIImageView *triangulation{nullptr};
    const Vector2<float> VIEW_CENTER = {VIEW_CENTERS, VIEW_CENTERS};
};
}
#endif