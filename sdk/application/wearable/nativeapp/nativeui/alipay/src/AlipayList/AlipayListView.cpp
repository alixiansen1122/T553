/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay list view.
 * Author:
 * Create:
 */

#include <string>
#include "alipay/AlipayList/AlipayListView.h"
#include "alipay/AlipayList/AlipayListItem.h"
#include "cmsis_os2.h"

namespace OHOS {
static constexpr int16_t ALIPAY_LIST_POSITION_X = 33;
static constexpr int16_t ALIPAY_LIST_POSITION_Y = 59;
static constexpr int16_t ALIPAY_LIST_SCROLLBLANKSIZE = 100;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 LIST_POS = 20;
static constexpr int16 SNUM_2 = 2;

AlipayList::AlipayList(AlipayPresenter *presenter)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayList::AlipayList");
    presenter_ = presenter;
}

AlipayList::~AlipayList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayList::~AlipayList");
    RemoveAll();

    if (listAdapter != nullptr) {
        delete listAdapter;
        listAdapter = nullptr;
    }
    if (contentList != nullptr) {
        delete contentList;
        contentList = nullptr;
    }
}

bool AlipayList::AlipayInitList()
{
    listAdapter = new AlipayListAdapter();
    if (listAdapter == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayList::OnStart::listAdapter is nullptr");
        return false;
    }
    listAdapter->SetItemClickListener(presenter_);
    InitAlipayListItem();
    contentList = new UIList();
    if (contentList == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayList::OnStart::contentList is nullptr");
        return false;
    }
    contentList->SetPosition(ALIPAY_LIST_POSITION_X, ALIPAY_LIST_POSITION_Y);
    contentList->Resize(HORIZONTAL_RESOLUTION - ALIPAY_LIST_POSITION_X, VERTICAL_RESOLUTION - ALIPAY_LIST_POSITION_Y);
    contentList->SetScrollBlankSize(ALIPAY_LIST_SCROLLBLANKSIZE);
    contentList->SetSelectPosition(ALIPAY_LIST_POSITION_Y);
    contentList->SetDraggable(true);
    contentList->SetThrowDrag(false);
    contentList->EnableAutoAlign(true);
    contentList->SetMaxScrollDistance(MAX_SCR_DIS);

    AddAlipayListItemToList();
    Add(contentList);
    contentList->SetAdapter(listAdapter);
    return true;
}

void AlipayList::InitAlipayListItem()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayListModel::InitAlipayListItem()");
    alipayLists[0] = {ALIPAY_LIST_ITEM_PAY, ALIPAY_LIST_PAY_BUTTON, PNG_ALIPAY_LIST_PAY, "付款码", true};
    alipayLists[1] = {ALIPAY_LIST_ITEM_TRANS, ALIPAY_LIST_TRANS_BUTTON, PNG_ALIPAY_LIST_TRANS, "乘车码", true};
    alipayLists[2] = {ALIPAY_LIST_ITEM_SETTING, ALIPAY_LIST_SETTING_BUTTON, PNG_ALIPAY_LIST_SETTING, "设置", true};
    alipayLists[3] = {ALIPAY_LIST_ITEM_HELP, ALIPAY_LIST_HELP_BUTTON, PNG_ALIPAY_LIST_HELP, "帮助", true};
}

bool AlipayList::InitListView()
{
    SetDraggable(true);
    SetTouchable(true);
    SetOnDragListener(presenter_);
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);

    if (!AlipayInitList()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AlipayList::InitView AlipayInitList() false");
        return false;
    }

    return true;
}

void AlipayList::AddAlipayListItemToList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlipayList::AddAlipayListItemToList");
    for (uint8_t i = 0; i < ALIPAY_LIST_ITEM_MAX; i++) {
        listAdapter->AddListItem(alipayLists[i]);
    }
}
}