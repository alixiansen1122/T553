/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: UIDesktopFragment.cpp
 * Author:
 * Create: 2025-07-07
 */

#include "applist/UIDesktopFragment.h"
#include "common/image_cache_manager.h"
#include "ability_manager.h"
#include "NativeAbility.h"

namespace OHOS {
void UIDesktopFragment::SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    fragmentView_.SetPosition(x, y, width, height);
}

bool UIDesktopFragment::SwitchView(AppViewId viewId, char *uid, TransitionType type, DesktopStyle style)
{
    if (viewId < VIEW_MAX_INTER_APP) {
        if (viewId == VIEW_SETTING && style == DesktopStyle::APPLIST_STYLE) {
            NativeAbility::GetInstance().ChangeSlice(viewId, type, gSliceDefaultPriority, true);
        } else {
            NativeAbility::GetInstance().ChangeSlice(viewId, type);
        }
        return true;
    } else if (viewId == VIEW_EXTERN_APP) {
#ifdef JS_ENABLE
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UIDesktopFragment::OnClick bundleName = %s", uid);
        Want *want = new Want();
        memset_s(want, sizeof(Want), 0, sizeof(Want));
        ElementName startElement = {};
        startElement.abilityName = (char *)"default";
        startElement.bundleName = uid;
        startElement.deviceId = nullptr;
        SetWantElement(want, startElement);
        if (viewId == VIEW_SETTING && style == DesktopStyle::APPLIST_STYLE) {
            StartAbilityWithAnimation(want, static_cast<uint8_t>(type), true);
        } else {
            StartAbilityWithAnimation(want, static_cast<uint8_t>(type), false);
        }
        ClearWant(want);
        delete want;
        return true;
#endif
    }
    return false;
}
}
