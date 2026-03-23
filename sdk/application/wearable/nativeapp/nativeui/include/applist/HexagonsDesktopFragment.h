/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: HexagonsDesktopFragment.h
 * Author:
 * Create: 2025-07-04
 */

#ifndef HEXAGONS_DESKTOP_FRAGMENT_H
#define HEXAGONS_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "components/ui_custom_hexagons_list.h"

namespace OHOS {
class HexagonsItemView;
class HexagonsDesktopFragment : public UIDesktopFragment {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnRotate(UIView& view, const RotateEvent& event) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    UICustomHexagonsList* hexagonsList_{nullptr};
    List<HexagonsItemView*> listHexagonsView_;
};

class HexagonsItemView : public UIImageView {
public:
    explicit HexagonsItemView() {};
    ~HexagonsItemView();
    bool SetItemInfo(const AppItem &itemInfo);
    AppViewId GetViewId();
    const char *GetAppUid();
private:
    HexagonsItemView(const HexagonsItemView &);
    HexagonsItemView &operator = (const HexagonsItemView &);
    AppItem itemInfo_;
};
}

#endif // HEXAGONS_DESKTOP_FRAGMENT_H
