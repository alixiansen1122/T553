/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersOptionGroup
 * Create: 2025-06-29
 */

#ifndef PLAYER_OPTION_GROUP_H
#define PLAYER_OPTION_GROUP_H

#include "graphic_types.h"
#include "wearable_log.h"
#include "components/ui_card_page.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "components/ui_radio_button.h"
#include "components/ui_card_page.h"
#include "components/ui_circle_progress.h"
#include "components/ui_checkbox.h"

namespace OHOS {
class PlayersOptionGroup : public UIViewGroup,
                    public UIView::OnClickListener,
                    public UIView::OnDragListener {
public:
    PlayersOptionGroup();
    ~PlayersOptionGroup() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void InitButton();
    void InitLabel();
    void InitImage();

private:
    UILabel *watchLabel_{nullptr};
    UILabel *phoneLabel_{nullptr};
    UILabel *bluetoothLabel_{nullptr};
    UIImageView *watchImage_{nullptr};
    UIImageView *phoneImage_{nullptr};
    UIImageView *bluetoothImage_{nullptr};
    UIRadioButton *watchButton_{nullptr};
    UIRadioButton *phoneButton_{nullptr};
    UIButton *bluetoothButton_{nullptr};
    UILabelButton *deleteButton{nullptr};
};
}
#endif
