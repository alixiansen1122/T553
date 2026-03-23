/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchItemView
 * Create: 2025-06-06
 */

#include "stopwatch/StopwatchItemView.h"

namespace OHOS {
static constexpr uint16_t STOPWATCH_ITEM_BG_XPOS = 6;
static constexpr uint16_t STOPWATCH_ITEM_BG_YPOS = 8;
static constexpr uint16_t STOPWATCH_ITEM_BG_WIDTH = 300;
static constexpr uint16_t STOPWATCH_ITEM_BG_HEIGHT = 44;
static constexpr uint16_t STOPWATCH_ITEM_BG_OPA = 100;
static constexpr uint16_t STOPWATCH_ITEM_NAME_LABEL_XPOS = 33;
static constexpr uint16_t STOPWATCH_ITEM_RECORD_LABEL_XPOS = 154;
static constexpr uint16_t STOPWATCH_ITEM_LABEL_YPOS = 12;
static constexpr uint16_t STOPWATCH_ITEM_NAME_LABEL_WIDTH = 100;
static constexpr uint16_t STOPWATCH_ITEM_RECORD_LABEL_WIDTH = 126;
static constexpr uint16_t STOPWATCH_ITEM_LABEL_HEIGHT = 36;
static constexpr uint16_t STOPWATCH_ITEM_FONT_SIZE = 32;
static constexpr uint32_t STOPWATCH_ITEM_FONT_COLOR = 0xffff8530;

StopwatchItemView::StopwatchItemView()
{
    itemBg_.SetPosition(STOPWATCH_ITEM_BG_XPOS, STOPWATCH_ITEM_BG_YPOS);
    itemBg_.Resize(STOPWATCH_ITEM_BG_WIDTH, STOPWATCH_ITEM_BG_HEIGHT);
    itemBg_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    itemBg_.SetStyle(STYLE_BACKGROUND_OPA, STOPWATCH_ITEM_BG_OPA);
    itemBg_.SetTouchable(false);

    nameLabel_.SetPosition(STOPWATCH_ITEM_NAME_LABEL_XPOS, STOPWATCH_ITEM_LABEL_YPOS);
    nameLabel_.Resize(STOPWATCH_ITEM_NAME_LABEL_WIDTH, STOPWATCH_ITEM_LABEL_HEIGHT);
    nameLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, STOPWATCH_ITEM_FONT_SIZE);
    nameLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    nameLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    nameLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    recordLabel_.SetPosition(STOPWATCH_ITEM_RECORD_LABEL_XPOS, STOPWATCH_ITEM_LABEL_YPOS);
    recordLabel_.Resize(STOPWATCH_ITEM_RECORD_LABEL_WIDTH, STOPWATCH_ITEM_LABEL_HEIGHT);
    recordLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, STOPWATCH_ITEM_FONT_SIZE);
    recordLabel_.SetAlign(TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER);
    recordLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    recordLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    Add(&itemBg_);
    Add(&nameLabel_);
    Add(&recordLabel_);
}

StopwatchItemView::~StopwatchItemView()
{
    Remove(&nameLabel_);
    Remove(&recordLabel_);
    Remove(&itemBg_);
}

void StopwatchItemView::SetItemInfo(const StopwatchItem &itemInfo)
{
    nameLabel_.SetText(("分段" + std::to_string(itemInfo.index)).c_str());
    recordLabel_.SetText(itemInfo.record.c_str());
    if (itemInfo.isLatest) {
        nameLabel_.SetStyle(STYLE_TEXT_COLOR, STOPWATCH_ITEM_FONT_COLOR);
        recordLabel_.SetStyle(STYLE_TEXT_COLOR, STOPWATCH_ITEM_FONT_COLOR);
    } else {
        nameLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        recordLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    }
}
}