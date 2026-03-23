/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateMainPage
 * Created: 2025-4
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "components/ui_button.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_date.h"
#include "bts_br_gap.h"
#include "bts_spp.h"
#include "msg_center_device.h"
#include "date/DateModel.h"
#include "date/DatePresenter.h"
#include "date/DateMainPage.h"

static constexpr int16_t DOWNSELECTPOSITION = 100;
static constexpr int16_t DOWNCONTENTLIST_POSITION_X = 39;
static constexpr int16_t DOWNCONTENTLIST_POSITION_Y = 171;
static constexpr int16_t DOWNCONTENTLIST_WIDTH = 376;
static constexpr int16_t DOWNCONTENTLIST_HEIGHT = 260;
static constexpr int16_t BUTTONCONTENTLIST_POSITION_X = 36;
static constexpr int16_t BUTTONCONTENTLIST_POSITION_Y = 97;
static constexpr int16_t BUTTONCONTENTLIST_WIDTH = 410;
static constexpr int16_t BUTTONCONTENTLIST_HEIGHT = 55;
static constexpr int16_t BUTTONCONTENTLIST_SELECT_POSITON = 90;
static constexpr uint16_t DATE_MONTH_FONT = 25;
static constexpr uint16_t DATE_MONTH_POS_X = 117;
static constexpr uint16_t DATE_MONTH_POS_Y = 30;
static constexpr uint16_t DATE_MONTH_WIDTH = 196;
static constexpr uint16_t LIST_POSITION_Y = 155;
static constexpr uint16_t LIST_BLANK_SIZE = 60;
static constexpr uint16_t DATE_MONTH_HEIGHT = 50;
static constexpr uint8_t FONT_FORTY_SIZE = 36;
static constexpr uint8_t MONTH_ADJUST_THRESHOLD = 3;
static constexpr uint8_t MONTHS_IN_YEAR = 12;
static constexpr uint8_t CENTURY = 100;
static constexpr uint8_t ZELLER_MONTH_FACTOR = 13;
static constexpr uint8_t ZELLER_DIVISOR = 5;
static constexpr uint8_t LEAP_YEAR_INTERVAL = 4;
static constexpr uint8_t DAYS_IN_WEEK = 7;
static constexpr uint8_t CENTURY_OFFSET = 5;
static constexpr int16 INDEX_0 = 0;
static constexpr int16 INDEX_1 = 1;
static constexpr int16 INDEX_2 = 2;
static constexpr int16 INDEX_3 = 3;
static constexpr int16 INDEX_4 = 4;
static constexpr int16 INDEX_5 = 5;
static constexpr int16_t DATE_ITEM_WIDTH = 95;
static constexpr int16_t DATE_ITEM_HEIGHT = 60;
static constexpr int16_t DATE_DOWN_LIST_ITEM_WIDTH = 376;
static constexpr int16_t DATE_DOWN_LIST_ITEM_HEIGHT = 170;
static constexpr char *DATE_LIST = "dateList";
static constexpr char *DETAIL_LIST = "detailList";

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_DATE, DATE_PAGES::DATE_MAIN_PAGE, DateMainPage, true);
static DateMainPage *g_pDateMainPage = nullptr;

DateMainPage::DateMainPage()
{
    g_pDateMainPage = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DateMainPage::DateMainPage");
}

DateMainPage::~DateMainPage()
{
    g_pDateMainPage = nullptr;
    if (uiScrollView_ != nullptr) {
        uiScrollView_->RemoveAll();
        delete uiScrollView_;
        uiScrollView_ = nullptr;
    }

    dateList_->ClearAll();
    if (dateList_ != nullptr) {
        delete dateList_;
        dateList_ = nullptr;
    }

    detailList_->ClearAll();
    if (detailList_ != nullptr) {
        delete detailList_;
        detailList_ = nullptr;
    }

    if (titlLabel_ != nullptr) {
        delete titlLabel_;
        titlLabel_ = nullptr;
    }

    if (bgImage_ != nullptr) {
        delete bgImage_;
        bgImage_ = nullptr;
    }

    int reset = DateModel::GetInstance().resetPage_;
    if (reset == PAGE_STATUS_0 || reset == PAGE_STATUS_2) {
        DateModel::GetInstance().selectButton_ = "";
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(DATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DateMainPage::~DateMainPage");
}

DateMainPage *DateMainPage::GetInstance()
{
    return g_pDateMainPage;
}

void DateMainPage::OnStart(void *data)
{
    uiScrollView_ = new UIScrollView();
    if (uiScrollView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "scrollView is nullptr");
        return;
    }
    uiScrollView_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    uiScrollView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    uiScrollView_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    uiScrollView_->SetThrowDrag(true);
    uiScrollView_->SetOnDragListener(this);
    uiScrollView_->SetThrowDrag(true);
    bgImage_ = new UIImageView();
    if (bgImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImage_ is nullptr");
        return;
    }
    bgImage_->SetPosition(0, 0);
    bgImage_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(bgImage_, DATE_IMAGE, IMAGE_DATE_BG1);
    uiScrollView_->Add(bgImage_);

    DateModel::GetInstance().InitDate();
    titlLabel_ = new UILabel();
    if (titlLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "titlLabel_ is nullptr");
        return;
    }
    std::stringstream ss;
    titlLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT, UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
    titlLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, FONT_FORTY_SIZE);
    titlLabel_->SetPosition(DATE_MONTH_POS_X, DATE_MONTH_POS_Y, DATE_MONTH_WIDTH, DATE_MONTH_HEIGHT);
    uiScrollView_->Add(titlLabel_);
    InitList();
    if (DateModel::GetInstance().GetConnectStatus()) {
        errcode_t result;
        uint8_t value = 1;
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SYNC_DATE_REQ, &value, sizeof(value));
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "sync data request result:%d ", result);
#endif
    }
    AddViewToPageContainer(uiScrollView_);
}

void DateMainPage::ReloadPage()
{
    DateModel::GetInstance().InitDate();
    RefreshListData();
}

void DateMainPage::RefreshListData()
{
    std::string strSelect = DateModel::GetInstance().selectButton_;
    if (strSelect == "") {
        SetDateButtonList(listTop_[0].buttonViewId);
        RefreshDateMsg(listTop_[0].buttonViewId);
    } else {
        for (int i = 0; i < listTop_.Size(); i++) {
            if (listTop_[i].buttonViewId == strSelect) {
                SetDateButtonList(listTop_[i].buttonViewId);
                RefreshDateMsg(listTop_[i].buttonViewId);
                break;
            }
        }
    }
}

void DateMainPage::InitList()
{
    dateList_ = new UISimpleList();
    if (dateList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "dateList_ is nullptr");
        return;
    }
    dateList_->SetViewId(DATE_LIST);
    dateList_->SetPosition(BUTTONCONTENTLIST_POSITION_X, BUTTONCONTENTLIST_POSITION_Y, BUTTONCONTENTLIST_WIDTH,
                                   BUTTONCONTENTLIST_HEIGHT);
    dateList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    dateList_->SetSelectPosition(BUTTONCONTENTLIST_SELECT_POSITON);
    dateList_->SetDraggable(true);
    dateList_->SetThrowDrag(true);
    dateList_->EnableAutoAlign(true);
    dateList_->SetIntercept(true);
    dateList_->ScrollTo(0);
    dateList_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    dateList_->SetDirection(0);
    InitListTopDate();

    detailList_ = new UISimpleList();
    if (detailList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "detailList_ is nullptr");
        return;
    }

    detailList_->SetViewId(DETAIL_LIST);
    detailList_->SetPosition(DOWNCONTENTLIST_POSITION_X, DOWNCONTENTLIST_POSITION_Y);
    detailList_->Resize(DOWNCONTENTLIST_WIDTH, DOWNCONTENTLIST_HEIGHT);
    detailList_->SetSelectPosition(DOWNSELECTPOSITION);
    detailList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    detailList_->SetDraggable(true);
    detailList_->SetThrowDrag(true);
    detailList_->EnableAutoAlign(true);
    detailList_->SetElastic(true);
    detailList_->ScrollTo(0);
    detailList_->SetScrollBlankSize(LIST_BLANK_SIZE);
    SetContentListDate();
    RefreshListData();
    uiScrollView_->Add(detailList_);
    uiScrollView_->Add(dateList_);
}

int CalculateWeekday(int year, int month, int day)
{
    if (month < MONTH_ADJUST_THRESHOLD) {
        month += MONTHS_IN_YEAR;
        year--;
    }
    int lastTwoDigitsOfYear = year % CENTURY;
    int century = year / CENTURY;
    int dayOfWeek =
        (day + ZELLER_MONTH_FACTOR * (month + 1) / ZELLER_DIVISOR + lastTwoDigitsOfYear +
         lastTwoDigitsOfYear / LEAP_YEAR_INTERVAL + century / LEAP_YEAR_INTERVAL + CENTURY_OFFSET * century) %
        DAYS_IN_WEEK;
    return dayOfWeek;
}

// 刷新List数据
void DateMainPage::RefreshDateMsg(std::string dateMsg)
{
    DateModel::GetInstance().InitDateVec(dateMsg);
    SetContentListDate();  // 将设置的数据写入list
    int year;
    int month;
    int day;
    int ret = sscanf_s(dateMsg.c_str(), "%d-%d-%d", &year, &month, &day);
    if (ret != DATE_UNIT_COUNT) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DateMainPage strDate_ error");
        return;
    }
    std::string weekDay[] = {"星期六", "星期日", "星期一", "星期二", "星期三", "星期四", "星期五"};
    int today = CalculateWeekday(year, month, day);
    std::string titleDay;
    if (dateMsg == listTop_.Front().buttonViewId) {
        titleDay = "今天 ";
    } else {
        titleDay = std::to_string(month) + "月 ";
    }
    titleDay += weekDay[today];
    DateModel::GetInstance().titleName_ = titleDay;
    titlLabel_->SetText(titleDay.c_str());
    detailList_->ScrollTo(0);
    detailList_->RefreshList();
}

static UIView* CreateContetListViewCb(uint8_t type)
{
    DateContentItemView* item = new DateContentItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, DATE_DOWN_LIST_ITEM_WIDTH, DATE_DOWN_LIST_ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_pDateMainPage);
    return item;
}

static void UpdateContentListViewCb(UIView* view, void* data, uint8_t type)
{
    DateContentItemView* item = dynamic_cast<DateContentItemView*>(view);
    if (view == nullptr || data == nullptr || item == nullptr) {
        return;
    }
    DateItem* content = static_cast<DateItem *>(data);
    item->SetItemInfo(*content);
}

void DateMainPage::SetContentListDate()
{
    detailList_->ClearAll();
    int count = DateModel::GetInstance().listDate_.size();
    for (int i = 0; i < count; i++) {
        Contents content;
        content.createFunc = CreateContetListViewCb;
        content.updateFunc = UpdateContentListViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&DateModel::GetInstance().listDate_[i]);
        detailList_->AddContent(content);
    }
    detailList_->RefreshList();
}

static UIView* CreateViewCb(uint8_t type)
{
    DateButtonItemView* item = new DateButtonItemView();
    if (item == nullptr) {
        return nullptr;
    }

    item->SetPosition(0, 0, DATE_ITEM_WIDTH, DATE_ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_pDateMainPage);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    DateButtonItemView* item = dynamic_cast<DateButtonItemView*>(view);
    if (view == nullptr || data == nullptr || item == nullptr) {
        return;
    }
    DateButtonItem* content = static_cast<DateButtonItem *>(data);
    item->SetItemInfo(*content);
}

void DateMainPage::SetDateButtonList(std::string viewId)
{
    dateList_->ClearAll();
    int count = listTop_.Size();
    for (int i = 0;i < count; i++) {
        if (listTop_[i].buttonViewId == viewId) {
            listTop_[i].isSelect = true;
        } else {
            listTop_[i].isSelect = false;
        }
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&listTop_[i]);
        dateList_->AddContent(content);
    }
    dateList_->RefreshList();
}

bool DateMainPage::InitListTopDate()
{
    listTop_.Clear();
    for (int i = 0; i < WEEK_DYAS; ++i) {
        strDate_[i] = GetFutureDate(i);
        strDay_[i] = strDate_[i].substr(strDate_[i].length() - INDEX_2);
        if (i == 0) {
            listTop_.PushBack({strDate_[i].c_str(), strDay_[i].c_str(), true, true});
        } else {
            listTop_.PushBack({strDate_[i].c_str(), strDay_[i].c_str(), false, false});
        }
    }
    return true;
}

std::string DateMainPage::GetFutureDate(int days_ahead)
{
    time_t now = time(nullptr);
    time_t future_time = now + (days_ahead * HOUR_COUNT * NUM_TIME_MIN * NUM_TIME_MIN);
    tm *tm = localtime(&future_time); // 检查返回值长度
    char buffer[11];  // 11:2025-08-14字符串长度
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    return std::string(buffer);
}

bool DateMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        DateModel::GetInstance().resetPage_ = PAGE_STATUS_0;
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool DateMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    if(strcmp(view.GetParent()->GetViewId(), DATE_LIST) == 0) {
        DateButtonItemView *dateButtonItemView = dynamic_cast<DateButtonItemView *>(&view);
        DateButtonItem item = dateButtonItemView->GetItem();
        DateModel::GetInstance().selectButton_ = item.buttonViewId;
        SetDateButtonList(item.buttonViewId);
        RefreshDateMsg(item.buttonViewId);
    } else if (strcmp(view.GetParent()->GetViewId(), DETAIL_LIST) == 0) {
        DateContentItemView *dateItemView = dynamic_cast<DateContentItemView *>(&view);
        bool visiable = dateItemView->GetVisiable();
        if (visiable) {
            return false;
        }
        DateModel::GetInstance().messageIndex_ = dateItemView->GetViewId();
        DateModel::GetInstance().resetPage_ = PAGE_STATUS_1;
        NativeAbility::GetInstance().SwitchPageInSlice(DATE_PAGES::DATE_MESSAGE_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    UNUSED(event);
    return true;
}
}
