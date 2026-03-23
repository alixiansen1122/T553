/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherItemView
 * Create: 2025-03-23
 */

#ifndef WEATHER_ITEM_VIEW_H
#define WEATHER_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_canvas.h"
#include "UiConfig.h"
#include "WeatherModel.h"

namespace OHOS {
class WeatherItemView : public UIViewGroup {
public:
    explicit WeatherItemView();
    ~WeatherItemView() override;
    virtual void SetItemInfo(const WeatherItem itemInfo);

private:
    UILabel labelDay_;
    UIImageView icon_;
    UILabel labelTmp_;
    UICanvas chartLine_;
    UIImageView bgImg_;
};
}

#endif
