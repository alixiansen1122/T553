/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainCompassPresenter.h
 * Author:
 * Create: 2021-12-21
 */
#ifndef COMPASS_MAINPRESENTER_H
#define COMPASS_MAINPRESENTER_H

#include "Presenter.h"
#include "compass/MainCompassView.h"
#include "compass/CompassModel.h"
#include "common/task.h"

namespace OHOS {
class MainCompassView;
class MainCompassPresenter : public Presenter<MainCompassView>, public UIView::OnDragListener, public Task {
public:
    bool OnDrag(UIView& view, const DragEvent& event) override;

    void OnResume() override;

    void OnPause() override;

    void Callback() override;
};
}
#endif