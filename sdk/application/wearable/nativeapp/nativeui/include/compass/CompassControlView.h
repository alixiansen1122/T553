/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: CompassControlView.h
 * Author:
 * Create: 2022-04-02
 */

#ifndef COMPASS_CONTROL_VIEW_H
#define COMPASS_CONTROL_VIEW_H

#include "components/ui_card_page.h"
#include "compass/CompassFragment.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {

class CompassControlView : public UICardPage, public PeriodicUpdateInterface {
public:
    CompassControlView();
    ~CompassControlView() override;
    void PreLoad(void) override;
    void UnLoad(void) override;
    void OnPause(void) override;
    void OnActive(void) override;
    void ScrollBegin(bool isActive) override;
    void ScrollEnd(bool isActive) override;
    void Update(void) override;
    uint32_t GetPeriod(void) override
    {
        return DEFAULT_TASK_PERIOD; // DEFAULT_TASK_PERIOD: ms
    }

private:
    CompassFragment *compassFragment_{nullptr};
};
}
#endif
