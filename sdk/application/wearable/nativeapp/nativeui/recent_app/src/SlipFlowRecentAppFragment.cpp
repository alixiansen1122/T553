/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: SlipFlowRecentAppFragment.cpp
 * Author:
 * Create: 2025-07-17
 */

#include "recent_app/SlipFlowRecentAppFragment.h"
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "Slice.h"
#include "RecentManager.h"
#include "NativeAbility.h"
#include "msg_center_customer.h"

namespace OHOS {
void SlipFlowRecentAppFragment::OnCreateView(void* data)
{
    LoadRecentAppContent();
}

void SlipFlowRecentAppFragment::OnDestroyView()
{
    if (slipFlowContainer_ != nullptr) {
        UIViewGroup* child = dynamic_cast<UIViewGroup*>(slipFlowContainer_->GetChildrenHead());
        while (child != nullptr) {
            UIView* node = child->GetChildrenHead();
            child = dynamic_cast<UIViewGroup*>(child->GetNextSibling());
            delete node;
        }
        delete slipFlowContainer_;
    }
    if (noRecentAppLabel_ != nullptr) {
        delete noRecentAppLabel_;
    }
    if (slipUpNoticeLabel_ != nullptr) {
        delete slipUpNoticeLabel_;
    }
}

void SlipFlowRecentAppFragment::LoadRecentAppContent()
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

void SlipFlowRecentAppFragment::InitRecentApps()
{
    if (slipFlowContainer_ == nullptr) {
        slipFlowContainer_ = new UISlipflowView();
    }
    if (slipFlowContainer_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new UISlipflowView fail");
        return;
    }
    if (slipUpNoticeLabel_ == nullptr) {
        slipUpNoticeLabel_ = new UILabel();
    }
    if (slipUpNoticeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new UILabel fail");
        return;
    }

    fragmentView_.Add(slipFlowContainer_);
    slipFlowContainer_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    slipFlowContainer_->SetThrowDrag(true);
    slipFlowContainer_->SetOnClickListener(this);
    slipFlowContainer_->SetOnSlipflowListener(this);
    AddRecentAppToSlipFlow();

    slipUpNoticeLabel_->SetPosition(127, 400, 200, 50); // 127 400 200 50: label position
    slipUpNoticeLabel_->SetTextColor(Color::White());
    slipUpNoticeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    slipUpNoticeLabel_->SetText("继续上滑删除");
    slipUpNoticeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30 : font size
    slipUpNoticeLabel_->SetVisible(false);
    fragmentView_.Add(slipUpNoticeLabel_);
}

void SlipFlowRecentAppFragment::AddImageToSlipFlow(int index)
{
    UIImageView* image = new UIImageView();
    image->SetViewId(appNames_[index].c_str());
    image->SetSrc(appImgInfos_[index]);
    image->Resize(250, 250); // 250: page size
    image->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    image->SetAutoEnable(false);
    slipFlowContainer_->Add(image);
    image->LayoutCenterOfParent();
}

void SlipFlowRecentAppFragment::AddRecentAppToSlipFlow()
{
    const List<RecentAppInfo*>& recent = RecentManager::GetInstance()->GetRecentApps();
    ListNode<RecentAppInfo*>* node = recent.Begin();
    while (node != recent.End()) {
        RecentAppInfo* appInfo = node->data_;
        ImageInfo* imgInfo = appInfo->snapshot;
        std::string name;
        if (appInfo->isNativeSlice) {
            name = RECENT_ITEM_NATIVE_PREFIX + std::to_string(appInfo->targetId & SLICE_MASK);
        } else {
            name = RECENT_ITEM_JS_PREFIX + appInfo->bundleName;
        }
        if (imgInfo->data == nullptr) {
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
        }
        appNames_.PushBack(name);
        appImgInfos_.PushBack(imgInfo);
        node = node->next_;
    }
    int index = appNames_.Size() - 1;
    while (index >= 0) {
        AddImageToSlipFlow(index);
        index--;
    }
}

bool SlipFlowRecentAppFragment::OnClick(UIView &view, const ClickEvent &event)
{
    std::string itemName(slipFlowContainer_->GetCurrentView()->GetViewId());
    std::string prefix(itemName.begin(), itemName.begin() + RECENT_ITEM_PREFIX_LEN);
    std::string name(itemName.begin() + RECENT_ITEM_PREFIX_LEN, itemName.end());
    UIViewGroup* currentItemGroup = dynamic_cast<UIViewGroup *>(slipFlowContainer_->GetCurrentView());
    if (currentItemGroup == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "currentItemGroup is nullptr!");
        return false;
    }
    uint32_t sliceId = 0;
    bool isNativeSlice = true;
    if (prefix == RECENT_ITEM_NATIVE_PREFIX) {
        sliceId = static_cast<uint32_t>(std::stoul(name));
    } else {
        isNativeSlice = false;
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

void SlipFlowRecentAppFragment::OnScrollUpStart()
{
    slipUpNoticeLabel_->SetVisible(true);
}

void SlipFlowRecentAppFragment::OnScrollUpEnd()
{
    slipUpNoticeLabel_->SetVisible(false);
}

bool SlipFlowRecentAppFragment::OnRemove(UIView* view)
{
    if (view == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Remove view is nullptr!");
        return false;
    }
    std::string itemName(view->GetViewId());
    std::string prefix(itemName.begin(), itemName.begin() + RECENT_ITEM_PREFIX_LEN);
    std::string name(itemName.begin() + RECENT_ITEM_PREFIX_LEN, itemName.end());
    uint32_t sliceId = 0;
    bool isNativeSlice = true;
    if (prefix == RECENT_ITEM_NATIVE_PREFIX) {
        sliceId = static_cast<uint32_t>(std::stoul(name));
    } else {
        isNativeSlice = false;
    }
    if (view->GetViewId() != nullptr) {
        if (isNativeSlice) {
            RecentManager::GetInstance()->RemoveRecentApp(sliceId);
        } else {
            RecentManager::GetInstance()->RemoveRecentApp(name.c_str());
        }
    }
    delete view;
    view = nullptr;
    return true;
}
}