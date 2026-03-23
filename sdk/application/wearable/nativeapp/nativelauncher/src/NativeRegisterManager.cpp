/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: NativeRegisterManager
 * Author:
 * Create: 2024-07
 */

#include "NativeRegisterManager.h"

namespace OHOS {

void NativeRegisterManager::InitAllNativesSlices()
{
    for(uint16_t index = 0; index < slices_.Size(); index++) {
        NativeAbility::GetInstance().AddSliceProxy(slices_[index].id, slices_[index].slice);
        GRAPHIC_LOGD("Register slice %d success.", slices_[index].id);
    }
}

List<AppItem>& NativeRegisterManager::GetNativeAppLists()
{
    return nativeAppLists_;
}

void NativeRegisterManager::RegisterSlice(const uint16_t id, SliceProxy* sliceProxy)
{
    if (sliceProxy == nullptr) {
        GRAPHIC_LOGE("Slice : %d is nullptr, register falied.", id);
        return;
    }
    for(uint16_t index = 0; index < slices_.Size(); index++) {
        if (slices_[index].id == id) {
            GRAPHIC_LOGE("Register slice failed because id %d already registered.", id);
            return;
        }
    }
    SliceMap slice = {id, sliceProxy};
    slices_.PushBack(slice);
}

void NativeRegisterManager::RegisterMenu(const uint16_t id, const char *icon, const char *iconHexagon, const char *label)
{
    AppItem item = {id, icon, iconHexagon, label, nullptr};
    ListNode<AppItem>* appNode = nativeAppLists_.Head();
    while (appNode != nativeAppLists_.End()) {
        if (id == appNode->data_.id) {
            GRAPHIC_LOGE("Register menu failed because id %d already registered.", id);
            return;
        }
        if (id < appNode->data_.id) {
            nativeAppLists_.Insert(appNode, item);
            return;
        }
        appNode = appNode->next_;
    }
    nativeAppLists_.PushBack(item);
}

void NativeRegisterManager::RegisterMenu(const AppItem &item)
{
    ListNode<AppItem>* appNode = nativeAppLists_.Head();
    while (appNode != nativeAppLists_.End()) {
        if (item.id == appNode->data_.id) {
            GRAPHIC_LOGE("Register menu failed because id %d already registered.", item.id);
            return;
        }
        if (item.id < appNode->data_.id) {
            nativeAppLists_.Insert(appNode, item);
            return;
        }
        appNode = appNode->next_;
    }
    nativeAppLists_.PushBack(item);
}

bool NativeRegisterManager::IsSliceIdExist(AppViewId id)
{
    for (uint16_t index = 0; index < slices_.Size(); index++) {
        if (slices_[index].id == static_cast<uint16_t>(id)) {
            return true;
        }
    }
    return false;
}
} // namespace OHOS
