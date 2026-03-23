/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageItemView
 * Create: 2025-06-29
 */

#ifndef MESSAGE_ITEM_VIEW_H
#define MESSAGE_ITEM_VIEW_H

#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_canvas.h"
#include "UiConfig.h"
#include "main/MessageModel.h"

namespace OHOS {
class MessageItemView : public UIViewGroup,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    explicit MessageItemView();
    ~MessageItemView() override;
    virtual void SetItemInfo(const MessageItem itemInfo);
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void SetIndex(uint16_t index);
    uint16_t GetIndex();
    MessageItem GetItem();
    void UpdateItemStatus();

    int posX;
    int x = 0;
private:
    uint16_t itemIndex_;
    UIViewGroup group_;
    UILabel appName_;
    UIImageView appIcon_;
    UILabel appContact_;
    UILabel appContents_;
    UILabel msgTime_;
    UIImageView bgImg_;
    UIImageView deleteImg_;
    MessageItem msgItemInfo_ = {"", "", "", "", 0, 0, false};
};
}

#endif
