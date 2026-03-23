/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionItemView
 * Create: 2025-04
 */

#ifndef UI_TEST_TRAINSTION_ITEM_VIEW_H_
#define UI_TEST_TRAINSTION_ITEM_VIEW_H_

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_image_animator.h"
#include "ui_test_transition_sample.h"


namespace OHOS {
class UITestTransitionItemView : public UIViewGroup {
public:
    explicit UITestTransitionItemView();
    ~UITestTransitionItemView() override;
    virtual void SetItemInfo(const UITestTransitionSample& leftItemInfo, const UITestTransitionSample& rightItemInfo);
    virtual void SetUpChild();
    uint16_t GetClickViewId(const ClickEvent& event);
    void SelectedChange();
    void Reset();
    bool LoadMultiImages(UITestTransitionSample& itemInfo, bool right);
private:
    UILabel sLabel_;
    UILabel rightLabel_;

    UIImageAnimatorView sAnimator_;
    UIImageAnimatorView rightAnimator_;
    
    UIImageView leftPreviewIcon_;
    UIImageView rightPreviewIcon_;

    UITestTransitionSample leftItemInfo_;
    UITestTransitionSample rightItemInfo_;
};
}

#endif
