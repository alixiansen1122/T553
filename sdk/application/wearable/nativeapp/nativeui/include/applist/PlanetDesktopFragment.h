/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlanetDesktop
 * Created: 2025-07-29
 */

#ifndef PLANET_DESKTOP_FRAGMENT_H
#define PLANET_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "components/ui_custom_planet_list.h"

namespace OHOS {
class PlanetItemView;
class PlanetDesktopFragment : public UIDesktopFragment {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    UICustomPlanetList* planetList_{nullptr};
    List<PlanetItemView*> pltItemList_;
    List<UILabel*> pltLabelList_;
};

class PlanetItemView : public UIImageView {
public:
    explicit PlanetItemView() {}
    ~PlanetItemView() {}
    void SetItemInfo(const AppItem &itemInfo);
    AppViewId GetViewId();
    const char *GetAppUid();
    const char *GetAppLabel();

private:
    PlanetItemView(const PlanetItemView &) = delete;
    PlanetItemView &operator = (const PlanetItemView &) = delete;
    AppItem itemInfo_ = { VIEW_INVALID, nullptr, nullptr, nullptr, nullptr };
};
}

#endif // PLANET_DESKTOP_FRAGMENT_H
