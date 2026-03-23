/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DesktopFactory
 * Author:
 * Create: 2025-07
 */

#include "applist/DesktopFactory.h"
#include "wearable_log.h"

namespace OHOS {

void DesktopFactory::RegisterDesktopStyle(const uint16_t style, DesktopInterface* desktop)
{
    if (desktop == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Can't register desktop using null proxy");
        return;
    }
    if (desktops_.find(style) != desktops_.end()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "desktop style %d has already been registered", style);
        return;
    } else {
        desktops_.insert({style, desktop});
    }
}

UIDesktopFragment* DesktopFactory::CreateDesktopStyle(uint16_t style)
{
    auto it = desktops_.find(style);
    if (it != desktops_.end()) {
        return it->second->CreateDesktop();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Desktop style:%d was not registered", style);
        return nullptr;
    }
}
} // namespace OHOS
