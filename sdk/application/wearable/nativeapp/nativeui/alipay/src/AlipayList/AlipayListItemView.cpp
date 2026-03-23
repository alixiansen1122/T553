/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list item view.
 * Author:
 * Create:
 */

#include "wearable_log.h"
#include "alipay/AlipayList/AlipayListItemView.h"

namespace OHOS {
static constexpr int16 ALIPAY_LIST_ICON_X = 20;
static constexpr int16 NUM_22 = 22;
static constexpr int16_t ALIPAY_LIST_ICON_WIDTH = 64;
static constexpr int16_t ALIPAY_LIST_ICON_HEIGHT = 64;
static constexpr int16_t ALIPAY_LIST_ITEM_BUTTON_HEIGHT = 100;
static constexpr int16_t ALIPAY_LIST_ITEM_LABEL_WIDTH = 122;
static constexpr int16_t ALIPAY_LIST_ITEM_LABEL_HEIGHT = 100;
static constexpr int16_t ALIPAY_LIST_ITEM_FONT_SIZE = 40;
static constexpr int16_t ALIPAY_LISTS_RESIZE_SIZE = 100;
static constexpr int16_t ALIPAY_LIST_SICON_Y = 15;
static constexpr int16 NUMS_2 = 2;
static constexpr int16 NUMS_50 = 50;

AlipayListItemView::AlipayListItemView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListItemView::AlipayListItemView()");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, ALIPAY_LIST_ITEM_BUTTON_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetTouchable(true);
    SetUpChild();

    Add(&sIcon);
    Add(&sLabel);
    Add(&sNextImage);
}

AlipayListItemView::~AlipayListItemView()
{
    Remove(&sIcon);
    Remove(&sLabel);
    Remove(&sNextImage);
}

void AlipayListItemView::SetItemInfo(const AlipayListItem &itemInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListItemView::SetItemInfo()");
    sItemInfo = itemInfo;
    sIcon.SetSrc(itemInfo.icon);
    sLabel.SetText(itemInfo.label);
    sNextImage.SetVisible(itemInfo.hasSecondMenu);
}

void AlipayListItemView::SetUpChild()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListItemView::SetUpChild()");
    sIcon.SetPosition(ALIPAY_LIST_ICON_X, ALIPAY_LIST_SICON_Y);
    sIcon.Resize(ALIPAY_LIST_ICON_WIDTH, ALIPAY_LIST_ICON_HEIGHT);

    sIcon.SetAutoEnable(false);
    sIcon.SetResizeMode(UIImageView::FILL);
    sIcon.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);

    sLabel.SetPosition(ALIPAY_LIST_ITEM_LABEL_WIDTH, 0);
    sLabel.Resize(ALIPAY_LIST_ITEM_LABEL_WIDTH, ALIPAY_LIST_ITEM_LABEL_HEIGHT);
    sLabel.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ALIPAY_LIST_ITEM_FONT_SIZE);
    sLabel.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    sLabel.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sLabel.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    sLabel.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    sLabel.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    sNextImage.SetSrc(PNG_SETTING_NEXT_IMAGE);
    sNextImage.SetPosition(LA_HORIZONTAL_RESOLUTION - NUMS_50, (SETTING_ITEM_HEIGHT - sNextImage.GetHeight()) / NUMS_2);
    sNextImage.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
}

}
