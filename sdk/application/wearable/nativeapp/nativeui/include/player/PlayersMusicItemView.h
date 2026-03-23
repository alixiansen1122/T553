/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersMusicItemView
 * Create: 2025-03-23
 */

#ifndef DATE_BUTTON_ITEM_VIEW_H
#define DATE_BUTTON_ITEM_VIEW_H

#include "UiConfig.h"
#include "components/ui_canvas.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "player/PlayersPresenter.h"
#include "main/LoadImg.h"

namespace OHOS {
class PlayersMusicItemView : public UIViewGroup {
public:
    explicit PlayersMusicItemView();
    ~PlayersMusicItemView() override;
    virtual void SetItemInfo(const PlayerCaseInfo itemInfo);
    PlayerCaseInfo GetItem();

private:
    UILabelButton labelButton_;
    PlayerCaseInfo itemInfo_ = {"", 0};
};
}

#endif
