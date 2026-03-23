/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "common/low_power_manager.h"
#include "graphic_service_wrapper.h"
#include "animator/animator_manager.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "components/root_view.h"
#include "hal_tick.h"
#include "gfx_utils/list.h"

namespace OHOS {
static const uint32_t TRY_ENTER_LOW_POWER_INTERVAL = 100; // to avoid switch refresh rate frequently during roate

void LowPowerManager::TryToEnterLowPower()
{
    // this condition can be removed later, switching refresh rate frequently doesn't seen to have any effect
    if (HALTick::GetInstance().GetElapseTime(lastExitTime_) < TRY_ENTER_LOW_POWER_INTERVAL) {
        return;
    }

    if (AnimatorManager::GetInstance()->IsActive() || InputDeviceManager::GetInstance()->IsEventExist()) {
        return;
    }

    if (LiteMGfxEngine::GetInstance()->HasPendingFrame()) {
        return;
    }

    if (IsLiteSurfaceInRootView()) {
        return;
    }

    uint32_t period = TaskManager::GetInstance()->GetMinimumCustomerTaskPeriod();
    int tmp = (period == 0) ? 1 : 1000 / static_cast<int>(period); // 1000: ms
    int freq = (tmp > maxFrequence_) ? maxFrequence_ : tmp;
    SetFrequenceInner(freq);
}

void LowPowerManager::ExitLowPower()
{
    lastExitTime_ = HALTick::GetInstance().GetTime();
    SetFrequenceInner(maxFrequence_);
}

bool LowPowerManager::IsLiteSurfaceInRootView()
{
    LiteMGfxEngine* gfxView = LiteMGfxEngine::GetInstance();
    if (gfxView->GetHardwareLayer() == nullptr) {
        return false;
    }

    UIView* view = dynamic_cast<UIView *>(gfxView->GetHardwareLayer());
    while (view->GetParent() != nullptr) {
        view = view->GetParent();
    }

    return (view == RootView::GetInstance());
}

void LowPowerManager::SetMaxFrequence(uint32_t freq)
{
    maxFrequence_ = freq;
    if (GetFrequence() > maxFrequence_) {
        SetFrequenceInner(maxFrequence_);
    }
}
} // namespace OHOS
