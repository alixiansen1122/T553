/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UICustomPlanetList
 * Create: 2025-05
 */

#ifndef UI_CUSTOM_PLANET_LIST_H
#define UI_CUSTOM_PLANET_LIST_H

#include "components/ui_planet_list.h"
#include "components/ui_label.h"
#include "components/ui_transform_group.h"

namespace OHOS {
class UICustomPlanetList : public UIPlanetList {
public:
    UICustomPlanetList(uint16_t itemSize, uint16_t imgDistance)
        : UIPlanetList(itemSize, imgDistance)
    {
    }
    ~UICustomPlanetList() override {}
    UICustomPlanetList(const UICustomPlanetList &) = delete;
    UICustomPlanetList &operator=(const UICustomPlanetList &) = delete;
}; // class UICustomPlanetList
} // namespace OHOS

#endif // UI_CUSTOM_PLANET_VIEW_H