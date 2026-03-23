/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardEffectItemGroup
 * Create: 2025-04
 */
#ifndef SETTING_CARD_EFFECT_ITEM_GROUP_H
#define SETTING_CARD_EFFECT_ITEM_GROUP_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_image_animator.h"
#include "SettingCardEffectSample.h"

namespace OHOS {
class SettingCardEffectItemGroup : public UIViewGroup {
public:
    explicit SettingCardEffectItemGroup();
    ~SettingCardEffectItemGroup() override;
    virtual void SetItemInfo(const SettingCardEffectSample& leftItemInfo,
        const SettingCardEffectSample& rightItemInfo);
    virtual void SetUpChild();
    CardEffectID GetViewId(const ClickEvent& event);
    void SelectedChange();
    void Reset();
private:
    void InitializeAnimator(UIImageAnimatorView& animator, int x, int y, int size);
    void InitializeIcon(UIImageView& icon, int x, int y, int size);
    void InitializeLabel(UILabel& label, int x, int y, int width, int height);
    bool LoadMultiImages(SettingCardEffectSample& itemInfo, bool right);

    UIImageAnimatorView leftAnimator_;
    UILabel leftLabel_;
    UIImageView leftSelectIcon_;
    UIImageView leftPreviewIcon_;

    UIImageAnimatorView rightAnimator_;
    UILabel rightLabel_;
    UIImageView rightSelectIcon_;
    UIImageView rightPreviewIcon_;

    SettingCardEffectSample leftItemInfo_;
    SettingCardEffectSample rightItemInfo_;
};
}
#endif
