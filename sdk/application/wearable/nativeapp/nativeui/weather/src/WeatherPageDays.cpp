/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageDays
 * Create: 2025-03-23
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_weather.h"
#include "wearable_log.h"
#include "weather/WeatherModel.h"
#include "weather/WeatherPresenter.h"
#include "weather/WeatherView.h"
#include "weather/WeatherPageDays.h"

namespace OHOS {
static constexpr uint8_t WEATHER_ITEM_FONT_SIZE = 25;
static constexpr int16_t WEATHER_RESIZE_SIZE_WIDTH = 200;
static constexpr int16_t WEATHER_RESIZE_SIZE_HEIGHT = 50;
static constexpr int16_t WEATHER_TITLE_POSITION_X = 130;
static constexpr int16_t WEATHER_LIST_POSITION_X = 32;
static constexpr int16_t WEATHER_LIST_POSITION_Y = 70;
static constexpr int16_t WEATHER_LIST_WIDTH = 390;
static constexpr int16_t WEATHER_LIST_HEIGHT = 318;
static constexpr int16_t WEATHER_LIST_SELECT_POSITION = 80;
static constexpr int16_t LIST_POS = 20;
static constexpr int16_t WEATHER_LABEL_FRONT_ONE = 20;
static constexpr int16_t WEATHER_LABEL_FRONT = 40;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_X = 199;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_Y = 429;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_WIDTH = 56;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_HEIGHT = 10;
static constexpr int16_t WEATHER_ITEM_BUTTON_WIDTH = 390;
static constexpr int16_t WEATHER_ITEM_BUTTON_HEIGHT = 83;

REGIST_SLICE_PAGE(VIEW_WEATHER, WEATHER_PAGES::WEATHER_DAYS_PAGE, WeatherPageDays, false);

WeatherPageDays::WeatherPageDays()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::WeatherPageDays");
}

WeatherPageDays::~WeatherPageDays()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (contentList_ != nullptr) {
        contentList_->ClearAll();
        delete contentList_;
        contentList_ = nullptr;
    }

    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (pageIdxImg_ != nullptr) {
        delete pageIdxImg_;
        pageIdxImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(WEATHER_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::~WeatherPageDays");
}

void WeatherPageDays::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::group_ is nullptr");
        return;
    }
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group_->SetOnDragListener(this);

    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::title_ is nullptr");
        return;
    }
    title_->SetPosition(WEATHER_TITLE_POSITION_X, WEATHER_LABEL_FRONT_ONE);
    title_->SetText("7日天气预报");
    title_->Resize(WEATHER_RESIZE_SIZE_WIDTH, WEATHER_RESIZE_SIZE_HEIGHT);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    title_->SetWidth(title_->GetTextWidth());
    int pageWidth = 454;
    int half = 2;
    title_->SetX((pageWidth - title_->GetTextWidth()) / half);

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OnStart bgImg_ new fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    bgImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadImageOfDayNight(bgImg_, IMAGE_WEATHER_BG1, IMAGE_WEATHER_BG2);
    InitScrollView();
    InitScrollData();
    AddViewToPageContainer(group_);
}

void WeatherPageDays::InitScrollView()
{
    contentList_ = new UISimpleList();
    if (contentList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::contentList_ is nullptr");
        return;
    }

    contentList_->SetPosition(WEATHER_LIST_POSITION_X, WEATHER_LIST_POSITION_Y);
    contentList_->Resize(WEATHER_LIST_WIDTH, WEATHER_LIST_HEIGHT);
    contentList_->SetSelectPosition(WEATHER_LIST_SELECT_POSITION);
    contentList_->SetDraggable(true);
    contentList_->SetThrowDrag(true);
    contentList_->EnableAutoAlign(true);
    contentList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    contentList_->SetElastic(true);
    pageIdxImg_ = new UIImageView();
    if (pageIdxImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::pageIdxImg_ new image fail");
        return;
    }
    pageIdxImg_->SetPosition(WEATHER_IMAGEPAGEKEY_X, WEATHER_IMAGEPAGEKEY_Y, WEATHER_IMAGEPAGEKEY_WIDTH,
                             WEATHER_IMAGEPAGEKEY_HEIGHT);
    pageIdxImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(pageIdxImg_, WEATHER_IMAGE, IMAGE_WEATHER_PAGE3);
    group_->Add(bgImg_);
    group_->Add(title_);
    group_->Add(contentList_);
    group_->Add(pageIdxImg_);
}

bool WeatherPageDays::InitScrollData()
{
    WeatherItem *itemInfo = WeatherModel::GetInstance().GetWeatherlistItems();
    RefreshWeathersList(itemInfo, WeatherModel::GetInstance().GetWeatherListNum());
    return true;
}

static UIView* CreateViewCb(uint8_t type)
{
    WeatherItemView* item = new WeatherItemView();
    if (item == nullptr) {
        return nullptr;
    }

    item->SetPosition(0, 0, WEATHER_ITEM_BUTTON_WIDTH, WEATHER_ITEM_BUTTON_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    WeatherItemView* item = dynamic_cast<WeatherItemView*>(view);
    if (view == nullptr || data == nullptr || item == nullptr) {
        return;
    }
    WeatherItem* content = static_cast<WeatherItem *>(data);
    item->SetItemInfo(*content);
}


void WeatherPageDays::RefreshWeathersList(WeatherItem *item, uint8_t count)
{
    if (item == nullptr) {
        return;
    }
    if (contentList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherPageDays::contentList_ fail");
        return;
    }
    contentList_->ScrollTo(0);
    for (uint8_t i = 0; i < count; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.data = reinterpret_cast<void *>(&item[i]);
        contentList_->AddContent(content);
    }
    contentList_->RefreshList();
}

bool WeatherPageDays::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_HOURS_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void WeatherPageDays::OnSwipe(UISwipeView &view)
{
    UNUSED(view);
    return;
}

bool WeatherPageDays::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    return true;
}
}