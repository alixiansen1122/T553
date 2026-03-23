/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: RollerRecentAppFragment.cpp
 * Author:
 * Create: 2025-07-17
 */

#include "recent_app/RollerRecentAppFragment.h"
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "Slice.h"
#include "RecentManager.h"
#include "NativeAbility.h"
#include "msg_center_customer.h"

namespace OHOS {
static constexpr uint8_t DEFAULT_TEXT_SIZE = 30;

void RollerRecentAppFragment::OnCreateView(void* data)
{
    LoadRecentAppContent();
}

void RollerRecentAppFragment::OnDestroyView()
{
    if (rollerContainer_ != nullptr) {
        delete rollerContainer_;
    }
    if (noRecentAppLabel_ != nullptr) {
        delete noRecentAppLabel_;
    }
    if (delBtn_ != nullptr) {
        delete delBtn_;
    }
}

void RollerRecentAppFragment::LoadRecentAppContent()
{
    uint32_t size = RecentManager::GetInstance()->GetCurrentSize();
    if (size == 0) {
        if (noRecentAppLabel_ == nullptr) {
            noRecentAppLabel_ = new UILabel();
        }
        if (noRecentAppLabel_ != nullptr) {
            fragmentView_.Add(noRecentAppLabel_);
            noRecentAppLabel_->SetText("No Recent App.");
            noRecentAppLabel_->Resize(300, 100); // 300, 100: size
            noRecentAppLabel_->LayoutCenterOfParent();
            noRecentAppLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
                UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        }
        return;
    }

    InitRecentApps();
}

void RollerRecentAppFragment::InitRecentApps()
{
    if (delBtn_ == nullptr) {
        delBtn_ = new UILabelButton();
    }
    if (delBtn_ == nullptr) {
        GRAPHIC_LOGE("new UILabelButton failed!");
        return;
    }

    if (rollerContainer_ == nullptr) {
        rollerContainer_ = new UIRollerView(false);
    }
    if (rollerContainer_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new UIRollerView fail");
        return;
    }

    fragmentView_.Add(rollerContainer_);
    rollerContainer_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rollerContainer_->SetItemSize(250, 250); // 250: item size
    rollerContainer_->SetThrowDrag(true);
    rollerContainer_->SetItemPadding(80); // 80: item padding
    rollerContainer_->SetCamDistance(380); // 380: Camera distance
    rollerContainer_->SetCamY(48); // 48: camY
    rollerContainer_->SetOnClickListener(this);

    fragmentView_.Add(delBtn_);
    delBtn_->Resize(120, 50); // 120，50: width and height
    delBtn_->LayoutCenterOfParent(0, 180); // 180: y offset
    delBtn_->SetText("delete");
    delBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_TEXT_SIZE);
    delBtn_->SetViewId(RECENT_APP_DELETE_BTN_ID);
    delBtn_->SetOnClickListener(this);

    AddRecentAppToRoller();
}

void RollerRecentAppFragment::AddRecentAppToRoller()
{
    const List<RecentAppInfo*>& recent = RecentManager::GetInstance()->GetRecentApps();
    ListNode<RecentAppInfo*>* node = recent.Begin();
    std::string alignItemName = "";
    while (node != recent.End()) {
        RecentAppInfo* appInfo = node->data_;
        ImageInfo* imgInfo = appInfo->snapshot;
        std::string name;
        if (appInfo->isNativeSlice) {
            name = RECENT_ITEM_NATIVE_PREFIX + std::to_string(appInfo->targetId & SLICE_MASK);
        } else {
            name = RECENT_ITEM_JS_PREFIX + appInfo->bundleName;
        }
        if (imgInfo->data != nullptr) {
            rollerContainer_->AddItem(name, imgInfo);
        } else {
            std::string path(appInfo->snapshot->file, appInfo->snapshot->fileLen);
            if (appInfo->snapshot->resId == 0) {
                imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(path);
            } else {
                imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(appInfo->snapshot->resId, path);
            }

            if (imgInfo == nullptr) {
                GRAPHIC_LOGE("Load res failed: %s", path.c_str());
                node = node->next_;
                continue;
            }
            rollerContainer_->AddItem(name, imgInfo);
        }
        node = node->next_;
        if (alignItemName == "") {
            alignItemName = name;
        }
    }
    rollerContainer_->SetCurrentItem(alignItemName);
}

bool RollerRecentAppFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UIViewGroup* currentItemGroup = rollerContainer_->GetCurrentItem();
    if (currentItemGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "currentItemGroup is nullptr!");
        return false;
    }

    std::string itemName = rollerContainer_->GetCurrentItemName();
    std::string prefix(itemName.begin(), itemName.begin() + RECENT_ITEM_PREFIX_LEN);
    std::string name(itemName.begin() + RECENT_ITEM_PREFIX_LEN, itemName.end());

    uint32_t sliceId = 0;
    bool isNativeSlice = true;
    if (prefix == RECENT_ITEM_NATIVE_PREFIX) {
        sliceId = static_cast<uint32_t>(std::stoul(name));
    } else {
        isNativeSlice = false;
    }

    const char* viewId = view.GetViewId();
    if (viewId != nullptr && strcmp(viewId, RECENT_APP_DELETE_BTN_ID) == 0) {
        if (isNativeSlice) {
            RecentManager::GetInstance()->RemoveRecentApp(sliceId);
        } else {
            RecentManager::GetInstance()->RemoveRecentApp(name.c_str());
        }
        rollerContainer_->RemoveItem(itemName);
        return true;
    }

    Point clickPoint = event.GetCurrentPos();
    Rect rect = currentItemGroup->GetChildrenHead()->GetRect();
    if (rect.IsContains(clickPoint)) {
        if (isNativeSlice) {
            NativeAbility::GetInstance().ChangeSlice(sliceId);
        } else {
#ifdef JS_ENABLE
            StartJsApp(const_cast<char*>(name.c_str()));
#endif
        }
    }
    return true;
}
}