/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainCompassView
 * Author:
 * Create: 2021-12-23
 */

#ifndef MAIN_COMPASS_VIEW_H
#define MAIN_COMPASS_VIEW_H

#include "View.h"
#include "compass/CompassFragment.h"

namespace OHOS {
class MainCompassPresenter;
class MainCompassView : public View<MainCompassPresenter> {
public:
    MainCompassView() : compassFragment_(nullptr) {}
    void OnStart() override;
    void OnStop() override;

private:
    friend class MainCompassPresenter;
    CompassFragment *compassFragment_;
};
} // namespace OHOS
#endif