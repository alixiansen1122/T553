/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UICardPageFactory
 * Author:
 * Create: 2025-06
 */

#include "UIWatchDialFactory.h"
#include "wearable_log.h"

namespace OHOS {

void UIWatchDialFactory::RegisterWatchDial(const uint16_t ability, const uint16_t id,
    UIWatchDialProxyInterface* uiWatchDial, const DialInfo* dialInfo)
{
    if (ability >= DIAL_DISPLAY_SUPPORT_MAX || uiWatchDial == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Can't register watch dial using invalid ability or proxy %d-%p",
        ability, uiWatchDial);
        return;
    }

    if ((ability == DIAL_DISPLAY_SUPPORT_NORMAL || ability == DIAL_DISPLAY_SUPPORT_NORMAL_AOD) && dialInfo != nullptr) {
        if (watchDials_[DIAL_DISPLAY_MODE_NORMAL].find(id) != watchDials_[DIAL_DISPLAY_MODE_NORMAL].end() ||
            normalDialInfo_.find(id) != normalDialInfo_.end()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "normal dial(info) id:%d has already been registered", id);
            return;
        } else {
            watchDials_[DIAL_DISPLAY_MODE_NORMAL].insert({id, uiWatchDial});
            normalDialInfo_.insert({id, dialInfo});
        }
    }

    if ((ability == DIAL_DISPLAY_SUPPORT_AOD || ability == DIAL_DISPLAY_SUPPORT_NORMAL_AOD)) {
        if (watchDials_[DIAL_DISPLAY_MODE_AOD].find(id) != watchDials_[DIAL_DISPLAY_MODE_AOD].end()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "aod dial id:%d has already been registered", id);
            return;
        } else {
            watchDials_[DIAL_DISPLAY_MODE_AOD].insert({id, uiWatchDial});
        }
    }
}

const DialInfo* UIWatchDialFactory::GetNormalDialInfo(uint16_t id)
{
    auto it = normalDialInfo_.find(id);
    if (it != normalDialInfo_.end()) {
        return it->second;
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "normal dial info id:%d was not created", id);
        return nullptr;
    }
}

uint8_t UIWatchDialFactory::GetNormalDialInfoNum()
{
    return normalDialInfo_.size();
}

UICardPage* UIWatchDialFactory::CreateNormalDial(uint16_t id)
{
    auto it = watchDials_[DIAL_DISPLAY_MODE_NORMAL].find(id);
    if (it != watchDials_[DIAL_DISPLAY_MODE_NORMAL].end()) {
        return it->second->CreateWatchDialCardPage(id);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "normal dial id:%d was not registered", id);
        return nullptr;
    }
}

UIFragment* UIWatchDialFactory::CreateAodDial(uint16_t id)
{
    auto it = watchDials_[DIAL_DISPLAY_MODE_AOD].find(id);
    if (it != watchDials_[DIAL_DISPLAY_MODE_AOD].end()) {
        return it->second->CreateWatchDialFragment(id);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "aod dial id:%d was not registered", id);
        return nullptr;
    }
}
} // namespace OHOS
