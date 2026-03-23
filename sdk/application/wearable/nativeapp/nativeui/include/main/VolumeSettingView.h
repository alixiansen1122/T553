/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: VolemeSettingView view
 * Create: 2021-09-5
 */

#ifndef VOLUME_SETTING_VIEW_H
#define VOLUME_SETTING_VIEW_H

#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "components/ui_view_group.h"
#include "components/ui_slider.h"
#include "player/PlayersView.h"
#include "wearable_log.h"
#include "main/VolumeSettingPresenter.h"

namespace OHOS {
#define VOLUME_MIN 0
#define VOLUME_MAX 100

class VolumeSettingView : public UIViewGroup {
public:
    VolumeSettingView();
    ~VolumeSettingView();
    int16_t sounding{0};

    UISlider *GetProgress(void);
    bool SetUpVolume(void);
    UIButton *GetIncreaseButton(void);
    UIButton *GetDecreaseButton(void);
    VolumeSliderEventListener *GetVolumeEventListener(void);
private:
    VolumeSliderEventListener *listener{nullptr};
    UISlider *progress{nullptr};
    UIButton *increaseButton{nullptr};
    UIButton *decreaseButton{nullptr};
    int ShowVolume();
};
} // namespace OHOS
#endif