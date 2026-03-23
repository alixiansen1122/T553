/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: CompassPresenter.h
 * Author:
 * Create: 2021-12-21
 */
#ifndef COMPASS_PRESENTER_H
#define COMPASS_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"
#include "components/ui_image_view.h"
#include "ohos_types.h"
#include "UiConfig.h"
#include "AppViewIDs.h"
#include "CompassView.h"

namespace OHOS {
typedef struct {
    uint16_t beginAngle;
    uint16_t endAngle;
    const char *dir;
} CompassItem;

class CompassView;
class CompassPresenter {
public:
    CompassPresenter();
    virtual ~CompassPresenter();

    static CompassPresenter *GetInstance(void);
    void DisplayAngleDir(uint16_t angle);
    void DetermineToRotate(void);
    void InScopeRotate(void);
    void TimeCreateRandomNum(void);
    void DisplayAngleRight(uint16_t angle);

private:
    int16_t lastRotateAngle{0};
    int16_t currentRotateAngle{0};
    int16_t angleDiff{0};
    bool getAngleStatus{false};
    bool initDisplayStatus{true};
};
}
#endif