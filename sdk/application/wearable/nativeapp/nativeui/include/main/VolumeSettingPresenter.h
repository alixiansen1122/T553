/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: VolumeSettingView view
 * Create: 2021-09-5
 */

#ifndef VOLUME_SETTING_PRESENTER_H
#define VOLUME_SETTING_PRESENTER_H

#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "components/ui_slider.h"
#include "main/VolumeSettingView.h"
#include "player/PlayersView.h"

namespace OHOS {
#define INCREASE_BUTTON "increaseButton"
#define DECREASE_BUTTON "decreaseButton"
#define VOLUME_PROGRESS "progress"
#define VOLUME_VIEW "volumeview"

typedef enum : uint16_t {
    VOLUMEPHONE = 1,
    VOLUMEPLAYERS = 3,
    VOLUMEPHONENOTIFY = 4,
} VolumeFactor;

typedef enum : uint8_t {
    VOLUMEDOWN = 0,
    VOLUMEUP = 1,
    VOLUMEDRAG = 2,
} VolumeChangeStatus;

class VolumeSettingView;
class VolumeSliderEventListener : public UISlider::UISliderEventListener,
                                  public UIView::OnClickListener,
                                  public UIView::OnDragListener {
public:
    VolumeSliderEventListener(VolumeSettingView *volumeSetting);
    ~VolumeSliderEventListener();
    static VolumeSliderEventListener *GetInstance();
    void OnChange(int32_t progress) override;
    void OnRelease(int32_t progress) override;
    void ChangeVolume(VolumeChangeStatus status);
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void SetPlayerCoefficients(int16_t value);
    void BtAndLocalVolumeSet(int16_t sounding);
private:
    int16_t preSound{0};
    int16_t playerCoefficients{0};
    VolumeSettingView *volumeSettingView{nullptr};
};
} // namespace OHOS
#endif
