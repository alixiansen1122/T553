/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateButtonItemView
 * Create: 2025-03-23
 */

#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_date.h"
#include "wearable_log.h"
#include "date/DateButtonItemView.h"

namespace OHOS {
static constexpr int16_t DATE_ITEM_BUTTON_POSITION_X = 0;
static constexpr int16_t DATE_ITEM_BUTTON_POSITION_Y = 0;
static constexpr int16_t DATE_ITEM_BUTTON_WIDTH = 80;
static constexpr int16_t DATE_ITEM_BUTTON_HEIGHT = 55;
static constexpr int16 DATE_ITEM_FONT_SIZE = 36;

DateButtonItemView::DateButtonItemView()
{
    labelButton_.SetPosition(DATE_ITEM_BUTTON_POSITION_X, DATE_ITEM_BUTTON_POSITION_Y);
    labelButton_.Resize(DATE_ITEM_BUTTON_WIDTH, DATE_ITEM_BUTTON_HEIGHT);
    labelButton_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT_SIZE);
    labelButton_.SetAlign(TEXT_ALIGNMENT_CENTER);
    labelButton_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelButton_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    labelButton_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    Add(&labelButton_);
}

DateButtonItemView::~DateButtonItemView()
{
    Remove(&labelButton_);
}

void DateButtonItemView::SetItemInfo(const DateButtonItem itemInfo)
{
    itemInfo_ = itemInfo;
    labelButton_.SetText(itemInfo_.buttonDay);
    int imageSrc1;
    int imageSrc2;
    if (itemInfo_.isFirst) {
        if (itemInfo_.isSelect) {
            imageSrc1 = IMAGE_DATE_RED;
            labelButton_.SetTextColor(Color::White());
        } else {
            imageSrc1 = IMAGE_DATE_WHITE;
            labelButton_.SetTextColor(Color::GetColorFromRGBA(0xe0, 0x2d, 0x2f, 0xff));
        }
        imageSrc2 = IMAGE_DATE_RED;
    } else {
        if (itemInfo_.isSelect) {
            imageSrc1 = IMAGE_DATE_SELECT;
        } else {
            imageSrc1 = IMAGE_DATE_GRAY;
        }
        imageSrc2 = IMAGE_DATE_SELECT;
        labelButton_.SetTextColor(Color::White());
    }
    LOADIMG::LoadBtnImage(&labelButton_, DATE_IMAGE, imageSrc1, imageSrc2);
}

DateButtonItem DateButtonItemView::GetItem()
{
    return itemInfo_;
}
}