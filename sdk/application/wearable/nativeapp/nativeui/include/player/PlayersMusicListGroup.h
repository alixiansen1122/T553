/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersMusicListGroup
 * Create: 2025-06-29
 */

#ifndef PLAYER_MUSIC_LIST_GROUP_H
#define PLAYER_MUSIC_LIST_GROUP_H

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
class PlayersMusicListGroup : public UIViewGroup,
                    public UIView::OnClickListener,
                    public UIView::OnDragListener {
public:
    PlayersMusicListGroup();
    ~PlayersMusicListGroup() override;
    static PlayersMusicListGroup *GetInstance(void);
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView &view, const DragEvent &event) override;
    void RefreshMusicList();
    void BtnOnListData(UIView& view);

private:
    UISimpleList *musicList_{nullptr};
    char *sliceId_{nullptr};
    int16_t index_{0};
};
}
#endif
