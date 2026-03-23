/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: AppItemView.cpp
 * Author:
 * Create: 2021-09-18
 */

#include "applist/AppItemView.h"
#include "common/image_cache_manager.h"
#include "wearable_log.h"

namespace OHOS {
    static constexpr int16 ICON_X = 80;
    static constexpr int16 APP_ITEM_BUTTON_WIDTH = 110;
    static constexpr int16 APP_ITEM_BUTTON_HEIGHT = 110;
    static constexpr int16 APP_ITEM_LABEL_WIDTH = 220;
    static constexpr int16 APP_ITEM_LABEL_HEIGHT = 110;

    static constexpr int16 NUM_20 = 20;
    static constexpr int16 APP_ITEM_FONT_SIZE = 40;
    static constexpr int16 APP_ITEM_SELECTED_FONT_SIZE = 44;

    AppItemView::AppItemView()
    {
        SetPosition(0, 0, HORIZONTAL_RESOLUTION, APP_ITEM_BUTTON_HEIGHT);
        SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        SetTouchable(true);
        SetUpChild();
        Add(&icon_);
        Add(&label_);
    }

    AppItemView::~AppItemView()
    {
        Remove(&icon_);
        Remove(&label_);
        if (itemInfo_.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
            ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.resId, itemInfo_.resPath);
        }
    }

    void AppItemView::SetUpChild()
    {
        icon_.SetPosition(ICON_X, 0);
        icon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        icon_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        icon_.Resize(APP_ITEM_BUTTON_WIDTH, APP_ITEM_BUTTON_HEIGHT);
        icon_.SetResizeMode(UIImageView::CENTER);
        icon_.SetAutoEnable(false);

        label_.SetPosition(ICON_X + APP_ITEM_BUTTON_WIDTH + NUM_20, 0);
        label_.Resize(APP_ITEM_LABEL_WIDTH, APP_ITEM_LABEL_HEIGHT);
        label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, APP_ITEM_FONT_SIZE);
        label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        label_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
        label_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
        label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    }

    void AppItemView::SetItemInfo(const AppItem &itemInfo)
    {
        itemInfo_ = itemInfo;
        if (itemInfo.label == nullptr) {
            if (itemInfo.labelId != STR_MAX_ID) {
                label_.SetTextId(itemInfo.labelId);
            } else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AppItemView::SetItemInfo label is null and labelId equal STR_MAX_ID!");
            }
        } else {
            label_.SetText(itemInfo.label);
        }
        if (itemInfo.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
            ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemInfo.resId, itemInfo.resPath);
            if (image != nullptr) {
                icon_.SetSrc(image);
            } else {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AppItemView::SetItemInfo image get failed!");
            }
        } else {
            icon_.SetSrc(itemInfo.iconSmall);
        }
    }

    AppViewId AppItemView::GetViewId()
    {
        return static_cast<AppViewId>(itemInfo_.id);
    }

    const char *AppItemView::GetAppUid()
    {
        return itemInfo_.uid;
    }

    void AppItemView::SelectedChange()
    {
        label_.SetTextColor(Color::White());
        icon_.Scale(Vector2<float>{1.1, 1.1}, Vector2<float>{0, 0});
    }

    void AppItemView::Reset()
    {
        label_.SetTextColor(Color::Gray());
        icon_.ResetTransParameter();
    }
}
