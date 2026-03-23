/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MessageMainPage
 * Create: 2025-06-29
 */

#ifndef MESSAGE_MAIN_VIEW_H
#define MESSAGE_MAIN_VIEW_H

#include "graphic_types.h"
#include "wearable_log.h"
#include "components/ui_card_page.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_simple_list.h"
#include "components/ui_list.h"
#include "main/MessageDetailsGroup.h"
#include "main/MessageModel.h"
#include "MainModel.h"
#include "main/MessageItemView.h"
namespace OHOS {
class MessageMainPage : public UICardPage,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    MessageMainPage();
    ~MessageMainPage() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    static MessageMainPage *GetInstance(void);
    bool InitView();
    void PreLoad(void) override;
    void VerticalViewInits(void);
    void ConfirmDelete(void);
    void CancelDelete(void);
    void InitMsgList(void);
    void DeleteView(void);
    void NoNotify(void);
    void NewNotify(void);
    void RefreshMsgList(std::vector<MessageItem> &item);
    bool InitScrollData();
    void LoadDetailsView(void);
    void DeleteDetailsView(void);
    UISimpleList *msgList_{nullptr};
private:
    UILabel *noMsgLable_{nullptr};
    UILabel *msgLable_{nullptr};
    UIImageView *noMsgImage_{nullptr};
    UIImageView *deleteImage_{nullptr};
    UIScrollView *deleteGroup_{nullptr};
    UILabel *deleteTips_{nullptr};
    UIImageView *confirm_{nullptr};
    UIImageView *cancel_{nullptr};
    DetailsGroup *detailsGroup_{nullptr};
};
}
#endif
