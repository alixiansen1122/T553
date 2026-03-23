/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionAdapter
 * Create: 2025-04
 */

#ifndef UI_TEST_TRAINSTION_ADAPTER_H
#define UI_TEST_TRAINSTION_ADAPTER_H

#include <list>
#include "ohos_types.h"
#include "components/ui_view_group.h"
#include "components/abstract_adapter.h"
#include "ui_test_transition_model.h"

namespace OHOS {
class UITestTransitionAdapter : public AbstractAdapter {
public:
    explicit UITestTransitionAdapter();
    ~UITestTransitionAdapter() override;

    void SetData(std::list<UITestTransitionSample> &data);

    uint16_t GetCount(void) override;

    UIView* GetView(UIView* inView, int16_t index) override;

    void AddListItem(const UITestTransitionSample &litem, const UITestTransitionSample &ritem);

    void SetItemClickListener(UIView::OnClickListener* itemclicklistener)
    {
        itemClickListener_ = itemclicklistener;
    }
private:
    std::list<std::pair<UITestTransitionSample, UITestTransitionSample>> listData_;
    UIView::OnClickListener* itemClickListener_{nullptr};
};
}

#endif