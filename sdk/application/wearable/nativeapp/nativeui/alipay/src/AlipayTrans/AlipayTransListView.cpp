/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay trans list view.
 * Author:
 * Create:
 */

#include <string>
#include "alipay/AlipayTrans/AlipayTransListView.h"
#include "alipay/AlipayTrans/AlipayTransListItem.h"
#include "alipay/AlipayModel.h"
#include "cmsis_os2.h"
#include "common_def.h"

namespace OHOS {
static constexpr int16_t ALIPAY_TRANS_LIST_POSITION_X = 33;
static constexpr int16_t ALIPAY_TRANS_LIST_POSITION_Y = 59;
static constexpr int16_t ALIPAY_TRANS_LIST_SCROLLBLANKSIZE = 50;
static constexpr int16 MAX_SCR_DIS = 900;
static constexpr int16 LIST_POS = 20;
static constexpr int16 SNUM_2 = 2;

static const int16 REFRESH_BUTTON_FONT = 38;
static const int16 REFRESH_BUTTON_X = 189;
static const int16 REFRESH_BUTTON_Y = 400;
static const int16 REFRESH_BUTTON_WIDTH = 76;
static const int16 REFRESH_BUTTON_HEIGHT = 53;
static const char *REFRESH_BUTTON = (char *)"刷新";

AlipayTransList::AlipayTransList(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AlipayTransList");
    presenter_ = presenter;
}

AlipayTransList::~AlipayTransList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::~AlipayTransList");
    RemoveAll();

    if (listAdapter != nullptr) {
        delete listAdapter;
        listAdapter = nullptr;
    }
    if (contentList != nullptr) {
        delete contentList;
        contentList = nullptr;
    }
    if (buttonRefresh != nullptr) {
        delete buttonRefresh;
        buttonRefresh = nullptr;
    }
}

bool AlipayTransList::AlipayInitList()
{
    listAdapter = new AlipayTransListAdapter();
    if (listAdapter == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AlipayInitList listAdapter is nullptr");
        return false;
    }
    listAdapter->SetItemClickListener(presenter_);
    InitAlipayTransListItem();
    contentList = new UIList();
    if (contentList == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AlipayInitList contentList is nullptr");
        return false;
    }
    contentList->SetPosition(ALIPAY_TRANS_LIST_POSITION_X, ALIPAY_TRANS_LIST_POSITION_Y);
    contentList->Resize(HORIZONTAL_RESOLUTION - ALIPAY_TRANS_LIST_POSITION_X, VERTICAL_RESOLUTION - 115);
    contentList->SetScrollBlankSize(ALIPAY_TRANS_LIST_SCROLLBLANKSIZE);
    contentList->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2); // middle position
    contentList->SetDraggable(true);
    contentList->SetThrowDrag(false);
    contentList->EnableAutoAlign(true);
    contentList->SetMaxScrollDistance(MAX_SCR_DIS);
    contentList->SetYScrollBarVisible(true);
    AddAlipayTransListItemToList();
    Add(contentList);
    contentList->SetAdapter(listAdapter);
    return true;
}

void AlipayTransList::InitAlipayTransListItem()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransListModel::InitAlipayTransListItem()");
    uint8_t cardCount = (uint8_t)min(presenter_->AlipayGetCardSum(), ALIPAY_TRANS_LIST_ITEM_LEN);
    for (uint8_t i = 0; i < cardCount; i++) {
        alipayTransLists[i].index = i;
        alipayTransLists[i].viewId = ALIPAY_TRANS_LIST_BUTTON;
        alipayTransLists[i].label = presenter_->AlipayGetCardTitle(i);
    }
    listLen = cardCount;
}

bool AlipayTransList::AlipayInitButton()
{
    buttonRefresh = new UILabelButton();
    if (buttonRefresh == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AlipayInitButton new label fail");
        return false;
    }
    buttonRefresh->SetPosition(REFRESH_BUTTON_X, REFRESH_BUTTON_Y, REFRESH_BUTTON_WIDTH, REFRESH_BUTTON_HEIGHT);
    buttonRefresh->SetText(REFRESH_BUTTON);
    buttonRefresh->SetFont(DEFAULT_VECTOR_FONT_FILENAME, REFRESH_BUTTON_FONT);

    buttonRefresh->SetViewId(ALIPAY_TRANS_REFRESH_LIST);

    buttonRefresh->SetTextColor(Color::Blue());
    buttonRefresh->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    buttonRefresh->SetStyleForState(STYLE_BORDER_COLOR, 0xff0b1928, UIButton::PRESSED);

    buttonRefresh->SetOnClickListener(presenter_);
    Add(buttonRefresh);

    return true;
}


bool AlipayTransList::InitListView()
{
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);

    if (!AlipayInitList()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransList::InitView AlipayInitList() false");
        return false;
    }

    if (!AlipayInitButton()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayTransList::InitView AlipayInitButton() false");
        return false;
    }

    return true;
}

void AlipayTransList::AddAlipayTransListItemToList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AddAlipayTransListItemToList");
    for (uint8_t i = 0; i < listLen; i++) {
        listAdapter->AddListItem(alipayTransLists[i]);
    }
}

void AlipayTransList::AlipayTransListUpdate(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayTransList::AlipayTransListUpdate");
    listAdapter->ClearItem();
    InitAlipayTransListItem();
    AddAlipayTransListItemToList();
    contentList->ScrollTo(0);
    contentList->RefreshList();
}
}