/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef GRAPHIC_LOW_POWER_MANAGER
#define GRAPHIC_LOW_POWER_MANAGER
#include "graphic_config.h"
#include "components/ui_label.h"

namespace OHOS {

class LowPowerManager : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>LowPowerManager</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    LowPowerManager() {}
    /**
     * @brief A destructor used to delete the <b>LowPowerManager</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~LowPowerManager() {}

    static LowPowerManager* GetInstance()
    {
        static LowPowerManager lowPowerManager;
        return &lowPowerManager;
    }

    /**
     * @brief Try to enter low power mode
     *
     */
    void TryToEnterLowPower();

    /**
     * @brief Exit low power mode
     *
     */
    void ExitLowPower();
    /**
     * @brief Check if LiteSurface is in rootView.
     *
     */
    bool IsLiteSurfaceInRootView();

    void SetMaxFrequence(uint32_t freq);

private:
    uint32_t lastExitTime_ = 0;
    uint32_t maxFrequence_ = 60;
};
} // OHOS
#endif // GRAPHIC_LOW_POWER_MANAGER
