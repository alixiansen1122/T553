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

#include "Ability.h"
#include "AppViewIDs.h"
#include "graphic_service.h"
#include "abilityms_slite_client.h"
#include "common/input_device_manager.h"
#include "animator/animator.h"
#include "components/root_view.h"
#include "common/screen.h"
#include "common/graphic_utils.h"
#include "PageTransitionMgr.h"
#include "gfx_utils/list.h"
#include "AbilitySlice.h"
#include "settings/model/SettingDesktopModel.h"
#include "NativeAbility.h"
#include "power_display_service.h"

namespace OHOS {
void Ability::AddSliceProxy(const uint16_t sliceId, SliceProxy* proxy)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability AddSliceProxy sliceId = %d", sliceId);
    if (sliceId <= gSliceInvalid || proxy == nullptr) {
        return;
    }

    if (Find(sliceId) != nullptr) {
        return;
    }

    Ability::ProxyStruct proxyPair = {sliceId, proxy};
    allSlicesProxy_.PushBack(proxyPair);
}

void Ability::StartSlice(uint32_t targetId, void* data)
{
    if (targetId <= gSliceInvalid) {
        return;
    }
    uint16_t sliceId = static_cast<uint16_t>(targetId & SLICE_MASK);
    Ability::ProxyStruct* proxyStruct = Find(sliceId);
    if (proxyStruct == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "no slice %d was found", sliceId);
        return;
    }
    InputDeviceManager::GetInstance()->ClearInputDeviceState();

    Slice* newSlice = proxyStruct->proxy->CreateSlice(targetId);
    if (newSlice == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "create slice %u failed", sliceId);
        return;
    }

    if (sliceId == defaultTargetId_) {
        allSlices_.Clear(); // clear slice history when return to main slice
        allSlices_.PushBack(defaultTargetId_);
    } else if (PageTransitionMgr::GetInstance().IsBackTransitionRunning()) {
        allSlices_.PopBack();
    } else {
        allSlices_.PushBack(targetId);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability StartSlice %d page %d size:%d", sliceId,
        targetId >> PAGE_OFFSET, allSlices_.Size());
    nextSlice_.sliceId = sliceId;
    nextSlice_.slice = newSlice;
    newSlice->OnStart(data);
}

Ability::ProxyStruct* Ability::Find(const uint16_t sliceId)
{
    if (sliceId <= gSliceInvalid) {
        return nullptr;
    }

    if (allSlicesProxy_.IsEmpty()) {
        return nullptr;
    }

    for (int i = 0; i < allSlicesProxy_.Size(); i++) {
        if (allSlicesProxy_[i].sliceId == sliceId) {
            return &allSlicesProxy_[i];
        }
    }
    return nullptr;
}

SlicePageManager* Ability::GetCurSlicePageManager()
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning())
    {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "No valid SlicePageManager during slice switching");
        return nullptr;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "No valid SlicePageManager when js is running");
        return nullptr;
    }
    return slice->GetSlicePageManger();
}

void Ability::UpdateCurSlicePageId(uint16_t pageId)
{
    if (allSlices_.IsEmpty()) {
        return;
    }
    uint32_t sliceId = allSlices_[allSlices_.Size() - 1] & SLICE_MASK;
    allSlices_[allSlices_.Size() - 1] = sliceId | (static_cast<uint32_t>(pageId) << PAGE_OFFSET);
}

bool Ability::SwitchPageInSlice(uint16_t pageId, TransitionType type, bool canBack, void* data, uint16_t dataLength)
{
    if (pageId == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page id should be valid but no zero");
        return false;
    }
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "SwitchPageInSlice is called not in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    void* sliceData = static_cast<void*>(UIMalloc(dataLength));
    memcpy_s(sliceData, dataLength, data, dataLength);
    GraphicService::GetInstance()->PostGraphicEvent([slice, pageId, sliceData, type, canBack]() {
        slice->ChangeSlicePage(pageId, sliceData, type, canBack);
        UIFree(sliceData);
    });
    return true;
}

bool Ability::BackToPrevSlicePage()
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToPrevSlicePage is not called  in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    GraphicService::GetInstance()->PostGraphicEvent([slice]() {
        slice->BackToPrevSlicePage();
    });
    return true;
}

bool Ability::BackToCachedSlicePage(uint16_t pageId)
{
    if (PageTransitionMgr::GetInstance().IsTransitionRunning()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched during transition");
        return false;
    }
    if (!GraphicService::IsNativeUITask()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "BackToCachedSlicePage is not called in ui task");
        return false;
    }
    AbilitySlice* slice = dynamic_cast<AbilitySlice*>(curSlice_.slice);
    if (slice == nullptr) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "page can't be switched because current slice is nullptr");
        return false;
    }
    GraphicService::GetInstance()->PostGraphicEvent([slice, pageId]() {
        slice->BackToCachedSlicePage(pageId);
    });
    return true;
}

void Ability::SwitchSlice(uint16_t sliceId, uint16_t pageId, TransitionType type, bool enableSlideBack)
{
    ExtralConfig config = {pageId, type, enableSlideBack};
    SwitchSliceWithData(sliceId, nullptr, 0, config);
}

void Ability::SwitchSliceWithData(uint16_t sliceId, void* data, uint16_t dataLength, const ExtralConfig& config)
{
    if (sliceId == VIEW_AOD) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "cannot enter aod by switch slice!");
        return;
    }
    if (Find(sliceId) == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "slice %d is not registered", sliceId);
        return;
    }
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    screen_context_t *sc = power_display_get_screen_context();
    if (sc != nullptr && display_api != nullptr && sc->current_state == SCREEN_ON) {
        display_api->turn_on_screen();
    }
#ifdef JS_ENABLE
    ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
    if (elementName != nullptr && strcmp(elementName->bundleName, "com.huawei.launcher") != 0) {
        OHOS::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName, config.type);
        targetIdFromJS_ = sliceId | (static_cast<uint32_t>(config.pageId) << PAGE_OFFSET);
        FreeElement(elementName);
        return;
    }
    FreeElement(elementName);
#endif
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = sliceId | (static_cast<uint32_t>(config.pageId) << PAGE_OFFSET);
    void* sliceData = nullptr;
    if (dataLength != 0 && data != nullptr) {
        sliceData = static_cast<void*>(UIMalloc(dataLength));
        memcpy_s(sliceData, dataLength, data, dataLength);
    }
    target.sliceInfo.data = sliceData;
    target.sliceInfo.dataLength = dataLength;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(TransitionCaller::CHANGE_SLICE, target,
        config.type, config.enableSlideBack);
    if (!runImmed) {
        printf("pending change slice id:%d type:%d enableSlideBack:%d\n", target.sliceInfo.sliceAndPageId, config.type, config.enableSlideBack);
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&Ability::PostChangeSlice, this,
        target.sliceInfo.sliceAndPageId, config.type, target.sliceInfo.data));
}

void Ability::ChangeSlice(uint16_t sliceId, TransitionType type,
    const uint16_t priority, bool enableSlideBack)
{
    (void)(priority);
    SwitchSlice(sliceId, gPageInvalid, type, enableSlideBack);
}

void Ability::ChangeSliceToApplist()
{
    if (SettingDesktopModel::GetInstance().GetDesktopStyle() == HEXAGONS_STYLE) {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_BACK_TO_HEXAGONS);
    } else if (SettingDesktopModel::GetInstance().GetDesktopStyle() == WATERFALL_STYLE) {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_BACK_TO_WATERFALL);
    } else {
        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST);
    }
}

void Ability::EnterAod()
{
#ifdef JS_ENABLE
    ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
    if (elementName != nullptr && strcmp(elementName->bundleName, "com.huawei.launcher") != 0) {
        OHOS::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName);
        targetIdFromJS_ = VIEW_AOD | (static_cast<uint32_t>(gPageInvalid) << PAGE_OFFSET);
        FreeElement(elementName);
        return;
    }
    FreeElement(elementName);
#endif
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = VIEW_AOD | (static_cast<uint32_t>(gPageInvalid) << PAGE_OFFSET);
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(
        TransitionCaller::CHANGE_SLICE, target, TransitionType::TRANSITION_INVALID, false);
    if (!runImmed) {
        printf("pending change slice id:%d type:%d enableSlideBack:%d\n",
            target.sliceInfo.sliceAndPageId, TransitionType::TRANSITION_INVALID, false);
        return;
    }
    PauseSlice();
    prevAodSlice_ = curSlice_;
    curSlice_.slice = nullptr;
    curSlice_.sliceId = gSliceInvalid;
    ImageCacheManager::GetInstance().EnterAod();
    StartSlice(VIEW_AOD);
    ResumeSlice();
    PageTransitionMgr::GetInstance().EndTransition(true);
}

void Ability::ExitAod(uint32_t targetId)
{
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = targetId;
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    bool runImmed = PageTransitionMgr::GetInstance().BeginTransition(
        TransitionCaller::CHANGE_SLICE, target, TransitionType::TRANSITION_INVALID, false);
    if (!runImmed) {
        printf("pending change slice id:%d type:%d enableSlideBack:%d\n",
            target.sliceInfo.sliceAndPageId, TransitionType::TRANSITION_INVALID, false);
        return;
    }
    allSlices_.PopBack();
    PauseSlice();
    StopSlice();
    ImageCacheManager::GetInstance().ExitAod();
    if ((targetId & SLICE_MASK) == prevAodSlice_.sliceId) {  // back to prev slice
        if (prevAodSlice_.slice != nullptr) {
            nextSlice_ = prevAodSlice_;
            prevAodSlice_.slice = nullptr;
            prevAodSlice_.sliceId = gSliceInvalid;
        }
        InputDeviceManager::GetInstance()->ClearInputDeviceState();
    } else {
        if (prevAodSlice_.slice != nullptr) {
            WEARABLE_LOGI(
                WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability StopSlice when exits aod %d", prevAodSlice_.sliceId);
            prevAodSlice_.slice->OnStop();
            prevAodSlice_.sliceId = gSliceInvalid;
            delete prevAodSlice_.slice;
            prevAodSlice_.slice = nullptr;
        }
        StartSlice(targetId);
    }
    ResumeSlice();
    PageTransitionMgr::GetInstance().EndTransition(true);
}

void Ability::PostChangeSlice(uint32_t targetId, TransitionType type, void* data)
{
    if (curSlice_.sliceId == VIEW_AOD) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "cannot exit aod by switch slice!");
        PageTransitionMgr::GetInstance().EndTransition(true);
        return;
    }
    PauseSlice();
    PageTransitionMgr::GetInstance().SaveCurrentSourceIfNecessary(curSlice_.slice->GetSliceContainer());
    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded() ||
        PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled()) {
        StopSlice();
    }
    StartSlice(targetId, data);

    if (!PageTransitionMgr::GetInstance().IsTransAnimNeeded()) {
        ResumeSliceWithoutJS();
    } else {
        PageTransitionMgr::GetInstance().SaveTargetSource(nextSlice_.slice->GetSliceContainer());
        PageTransitionMgr::GetInstance().StartTransAnim(std::bind(&Ability::ResumeSliceWithoutJS, this));
    }
    if (data != nullptr) {
        UIFree(data);
    }
}

void Ability::ResumeSliceWithoutJS()
{
    if (PageTransitionMgr::GetInstance().IsTransAnimNeeded() &&
        !PageTransitionMgr::GetInstance().IsCurrentSnapshotEnabled()) {
        StopSlice();
    }
    ResumeSlice();
    PageTransitionMgr::GetInstance().EndTransition(true);
}

void Ability::ResumeSlice()
{
    // if new slice was created, set is as current slice and resume it
    if (nextSlice_.slice != nullptr) {
        if (curSlice_.slice != nullptr) {
            GRAPHIC_LOGE("Both slice %d and %d is valid during resume", curSlice_.sliceId, nextSlice_.sliceId);
        }
        curSlice_.sliceId = nextSlice_.sliceId;
        curSlice_.slice = nextSlice_.slice;
        nextSlice_.sliceId = gSliceInvalid;
        nextSlice_.slice = nullptr;
    }

    if (curSlice_.slice != nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability ResumeSlice %d", curSlice_.sliceId);
        curSlice_.slice->OnResume();
    } else {
        GRAPHIC_LOGE("No valid slice to resume");
    }
}

void Ability::PauseSlice()
{
    if (curSlice_.slice != nullptr) {
        InputDeviceManager::GetInstance()->ClearInputDeviceState();
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability PauseSlice %d", curSlice_.sliceId);
        curSlice_.slice->OnPause();
    } else {
        GRAPHIC_LOGE("No valid slice to pause");
    }
}

void Ability::StopSlice()
{
    if (curSlice_.slice != nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability StopSlice %d", curSlice_.sliceId);
        curSlice_.slice->OnStop();
        curSlice_.sliceId = gSliceInvalid;
        delete curSlice_.slice;
        curSlice_.slice = nullptr;
        curSlicePriority_ = gSliceDefaultPriority;
    } else {
        GRAPHIC_LOGE("No valid slice to stop");
    }
}

void Ability::StopSliceTransition()
{
    PageTransitionMgr::GetInstance().SyncStopAnimator();
}

void Ability::BackToPreSlice()
{
    uint32_t targetId = GetPreTargetId();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "Ability BackToPreSlice %d", targetId);
    if (targetId == gSliceInvalid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "previous slice is invalid");
        return;
    }
    TransitionTarget target;
    target.sliceInfo.sliceAndPageId = targetId;
    target.sliceInfo.data = nullptr;
    target.sliceInfo.dataLength = 0;
    uint16_t sliceId = static_cast<uint16_t>(targetId & SLICE_MASK);
    uint16_t pageId = static_cast<uint16_t>(targetId >> PAGE_OFFSET);
    PageTransitionMgr::GetInstance().BeginBackTransition(TransitionCaller::CHANGE_SLICE, target);
    SwitchSlice(sliceId, pageId);
}
} // namespace OHOS
