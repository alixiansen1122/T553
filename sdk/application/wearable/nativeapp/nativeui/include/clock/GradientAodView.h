/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GradientAodView
 * Create: 2025-04
 */

#ifndef GRADIENT_AOD_VIEW_H
#define GRADIENT_AOD_VIEW_H

#include "components/ui_card_page.h"
#include "GradientDateView.h"
#include "components/ui_fragment.h"
#include "common/periodicupdate_interface.h"

namespace OHOS {

class GradientAodView : public UIFragment, public PeriodicUpdateInterface {
public:
    GradientAodView();
    ~GradientAodView() override;
    GradientAodView(const GradientAodView &) = delete;
    static GradientAodView* GetInstance(void);
    GradientAodView &operator=(const GradientAodView &) = delete;
    void Update(void) override;
    uint32_t GetPeriod(void) override
    {
        return 1000; // 1000: ms
    }

private:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    GradientDateView* gradientDateView_{nullptr};
}; // class GradientAodView

} // namespace OHOS

#endif // GRADIENT_AOD_VIEW_H