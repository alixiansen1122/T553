/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageHours
 * Create: 2025-03-15
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_weather.h"
#include "wearable_log.h"
#include "weather/WeatherPresenter.h"
#include "weather/WeatherView.h"
#include "weather/WeatherPageHours.h"

namespace OHOS {
static constexpr uint16_t ANIMATOR_TIME = 50;

REGIST_SLICE_PAGE(VIEW_WEATHER, WEATHER_PAGES::WEATHER_HOURS_PAGE, WeatherPageHours, false);

WeatherPageHours::WeatherPageHours()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageHours::WeatherPageHours");
}

WeatherPageHours::~WeatherPageHours()
{
    if (weatherSwipeGroup_ != nullptr) {
        weatherSwipeGroup_->RemoveAll();
        delete weatherSwipeGroup_;
        weatherSwipeGroup_ = nullptr;
    }

    for (int i = 0; i < GROUP_24H_COUNT; ++i) {
        if (hoursGroup_[i] != nullptr) {
            hoursGroup_[i]->RemoveAll();
            delete hoursGroup_[i];
            hoursGroup_[i] = nullptr;
        }
    }
    WeatherModel::GetInstance().index_ = 0;
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(WEATHER_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherPageHours::~WeatherPageHours");
}

void WeatherPageHours::OnStart(void *data)
{
    weatherSwipeGroup_ = new UISwipeView(UISwipeView::VERTICAL);
    if (weatherSwipeGroup_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "weatherSwipeGroup_ new fail");
        return;
    }
    weatherSwipeGroup_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    weatherSwipeGroup_->SetViewId("weatherSwipeGroup_");
    weatherSwipeGroup_->SetOnDragListener(this);
    weatherSwipeGroup_->SetOnSwipeListener(this);
    weatherSwipeGroup_->SetDraggable(true);
    weatherSwipeGroup_->SetTouchable(true);
    weatherSwipeGroup_->SetAnimatorTime(ANIMATOR_TIME);
    
    for (int i = 0; i < GROUP_24H_COUNT; i++) {
        hoursGroup_[i] = new WeatherPageHoursGroup();
        if (hoursGroup_[i] == nullptr) {
            return;
        }
        WeatherModel::GetInstance().index_++;
        weatherSwipeGroup_->Add(hoursGroup_[i]);
    }

    AddViewToPageContainer(weatherSwipeGroup_);
}

bool WeatherPageHours::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_RIGHT_TO_LEFT) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_DAYS_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

void WeatherPageHours::OnSwipe(UISwipeView &view)
{
    UNUSED(view);
    return;
}

bool WeatherPageHours::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    return true;
}
}