/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans list item view.
 * Author:
 * Create:
 */

#include "wearable_log.h"
#include "alipay/AlipayTrans/AlipayTransListItemView.h"

namespace OHOS {
static constexpr int16_t ALIPAY_TRANS_LIST_ITEM_BUTTON_HEIGHT = 95;
static constexpr int16_t ALIPAY_TRANS_LIST_ITEM_LABEL_X = 53;

static constexpr int16_t ALIPAY_TRANS_LIST_ITEM_LABEL_WIDTH = 347;
static constexpr int16_t ALIPAY_TRANS_LIST_ITEM_LABEL_HEIGHT = 90;

static constexpr int16_t ALIPAY_TRANS_LIST_ITEM_FONT_SIZE = 35;

AlipayTransListItemView::AlipayTransListItemView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListItemView::AlipayTransListItemView()");
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, ALIPAY_TRANS_LIST_ITEM_BUTTON_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetTouchable(true);
    SetUpChild();

    Add(&sLabel);
}

AlipayTransListItemView::~AlipayTransListItemView()
{
    Remove(&sLabel);
}

void AlipayTransListItemView::SetItemInfo(const AlipayTransListItem &itemInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListItemView::SetItemInfo()");
    sItemInfo = itemInfo;
    sLabel.SetText(itemInfo.label);
}

void AlipayTransListItemView::SetUpChild()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListItemView::SetUpChild()");

    sLabel.SetPosition(ALIPAY_TRANS_LIST_ITEM_LABEL_X, 0);
    sLabel.Resize(ALIPAY_TRANS_LIST_ITEM_LABEL_WIDTH, ALIPAY_TRANS_LIST_ITEM_LABEL_HEIGHT);
    sLabel.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ALIPAY_TRANS_LIST_ITEM_FONT_SIZE);
    sLabel.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    sLabel.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sLabel.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    sLabel.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    sLabel.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

uint8_t AlipayTransListItemView::GetAlipayTransListIndex()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListItemView::GetAlipayTransListIndex()");
    return sItemInfo.index;
}
}