/*
 * Copyright (c) CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "NativeAbility.h"

#include "NativeLauncher.h"
#include "ability_info.h"
#include "ability_state.h"
#include "graphic_service_wrapper.h"
#include "graphic_service.h"
#include "notification_manager.h"
#include "PageTransitionMgr.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef JS_ENABLE
int InstallNativeAbility(const AbilityInfo *abilityInfo, const OHOS::SliteAbility *ability);
#endif

#ifdef __cplusplus
}
#endif

namespace OHOS {
NativeAbility &NativeAbility::GetInstance(void)
{
    static NativeAbility instance;
    return instance;
}

#ifdef JS_ENABLE
void NativeAbility::InstallAbility(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeAbility InstallAbility");
    // 注册nativeability到AMS
    InstallNativeAbility(nullptr, this);

    // 初始化Launcher
    NativeLauncher::GetInstance().InitNativeLauncher();
    HideGraphicLogo();
    return;
}
#endif

void NativeAbility::OnStart(const Want &want) {
    UNUSED(want);
    nativeAbilityState_ = STATE_INITIAL;
}

void NativeAbility::OnInactive() {
    nativeAbilityState_ = STATE_INACTIVE;
}

void NativeAbility::ResumeSliceWithJS()
{
    ResumeSlice();
    nativeAbilityState_ = STATE_ACTIVE;
#ifdef JS_ENABLE
    Want want; // initialize want which is not used actually
    SliteAbility::OnActive(want);
#endif
}

void NativeAbility::ResumeAbility(uint32_t targetId)
{
    GraphicService::GetInstance()->SetNativeUIRunning(true);
    NotificationManager::GetInstance()->SetNativeUIRunning(true);
    if ((targetId & SLICE_MASK) != VIEW_AOD) {
        allSlices_.Clear(); // clear slice history when jump from js to native
    } else {
        allSlices_.PopBack(); // clear invalid slice id
    }
    StartSlice(targetId);
    if (nextSlice_.slice == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeAbility ResumeAbility %d failed", targetId);
        return;
    }
    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded()) {
        ResumeSliceWithJS();
    } else {
        PageTransitionMgr::GetInstance().SaveTargetSource(nextSlice_.slice->GetSliceContainer());
        PageTransitionMgr::GetInstance().StartTransAnim(std::bind(&NativeAbility::ResumeSliceWithJS, this));
    }
}

void NativeAbility::OnActive(const Want &want)
{
    uint32_t targetId = gSliceInvalid;
    if (targetIdFromJS_ != gSliceInvalid) {
        targetId = targetIdFromJS_;
        targetIdFromJS_ = gSliceInvalid;
    } else if (GetPreTargetId() != gSliceInvalid) {
        targetId = GetPreTargetId(); // for forcestop or ternimate js
    } else if (GetCurTargetId() != gSliceInvalid) {
        targetId = GetCurTargetId();
    }
    targetId = (targetId == gSliceInvalid) ? defaultTargetId_ : targetId;

#ifdef JS_ENABLE
    if ((GetState() == STATE_ACTIVE)) {
        SliteAbility::OnActive(want);
        return;
    }
#endif
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeAbility OnActive");
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&NativeAbility::ResumeAbility, this,
        targetId));
}

void NativeAbility::PauseAbility()
{
    PauseSlice();
    PageTransitionMgr::GetInstance().SaveCurrentSourceIfNecessary(curSlice_.slice->GetSliceContainer());
    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded() ||
        PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled()) {
        StopSlice();
    }
    nativeAbilityState_ = STATE_BACKGROUND;
    GraphicService::GetInstance()->SetNativeUIRunning(false);
    NotificationManager::GetInstance()->SetNativeUIRunning(false);
#ifdef JS_ENABLE
    SliteAbility::OnBackground();
#endif
}

void NativeAbility::OnBackground()
{
#ifdef JS_ENABLE
    if (GetState() == STATE_BACKGROUND) {
        return;
    }
#endif
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeAbility::OnBackground");
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&NativeAbility::PauseAbility, this));
}

void NativeAbility::StopAbility()
{
    if (PageTransitionMgr::GetInstance().IsTransAnimNeeded() &&
        !PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled()) {
        StopSlice();
    }
    if (allSlices_.Size() > 0 && allSlices_[allSlices_.Size() - 1] == VIEW_AOD) {
        allSlices_.PopBack();
    }
    allSlices_.PushBack(gSliceInvalid);
    nativeAbilityState_ = STATE_UNINITIALIZED;
#ifdef JS_ENABLE
    SliteAbility::OnStop();
#endif
}

void NativeAbility::OnStop()
{
#ifdef JS_ENABLE
    if (GetState() == STATE_UNINITIALIZED) {
        return;
    }
#endif
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeAbility::OnStop");
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&NativeAbility::StopAbility, this));
}

int NativeAbility::GetNativeAbilityState() const
{
    return nativeAbilityState_;
}
}
