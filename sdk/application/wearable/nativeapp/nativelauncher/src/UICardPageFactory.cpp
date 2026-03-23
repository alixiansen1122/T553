/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UICardPageFactory
 * Author:
 * Create: 2025-06
 */

#include "UICardPageFactory.h"
#include "wearable_log.h"

namespace OHOS {

void UICardPageFactory::RegisterCardPage(const uint16_t id, UICardPageProxyInterface* uiCardPage)
{
    if (uiCardPage == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Can't register watch dial using invalid proxy %p",
            uiCardPage);
        return;
    }
    if (cardPage_.find(id) != cardPage_.end()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Watch dial id:%d has already been registered", id);
        return;
    } else {
        cardPage_.insert({id, uiCardPage});
    }
}

UICardPage* UICardPageFactory::CreateCardPage(uint16_t id)
{
    auto it = cardPage_.find(id);
    if (it != cardPage_.end()) {
        return it->second->CreateUICardPage();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Watch dial id:%d was not registered", id);
        return nullptr;
    }
}
} // namespace OHOS
