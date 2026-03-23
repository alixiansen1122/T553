/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherMainPage
 * Created: 2025-4
 */

#ifndef WEATHER_MAIN_PAGE_H
#define WEATHER_MAIN_PAGE_H

#include <sys/time.h>
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_list.h"
#include "WeatherPresenter.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "WeatherView.h"

namespace OHOS {
static constexpr int16_t LABEL_ADDRESS = 0;
static constexpr int16_t LABEL_TEMP = 1;
static constexpr int16_t LABEL_AIR = 2;
static constexpr int16_t LABEL_HUMIDITY = 3;
static constexpr int16_t LABEL_UPDATETIME = 4;
static constexpr int16_t LABEL_CURRENT_TEMP = 5;
static constexpr int16_t LABEL_COUNT = 6;

class WeatherPresenter;
class WeatherMainPage : public SlicePage<WeatherPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    WeatherMainPage();
    ~WeatherMainPage() override;
    static WeatherMainPage *GetInstance();
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    std::string FormatTime(const std::string& inputStr);
    void InitWeatherImage1();
    void InitWeatherImage2();
    void InitWeatherLabel1();
    void InitWeatherLabel2();
    uint8_t GetConnectStatus();
    void ReloadPage();
private:
    bool flag_ = false;
    int curWeatherCode_ = 0;
    CurWeather cur_{nullptr};
    UIViewGroup *group_ = nullptr;
    UILabel *title_[LABEL_COUNT]{nullptr};
    UILabel *labelTitle_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *addressImg_{nullptr};
    UIImageView *airQualityAndHumidityBgImg_{nullptr};
    UIImageView *weatherImg_{nullptr};
    UIImageView *airQulityImgIcon_{nullptr};
    UIImageView *humidityImgIcon_{nullptr};
    UIImageView *pageIdxImg_{nullptr};
};
}
#endif // SETTING_MAIN_PAGE_H