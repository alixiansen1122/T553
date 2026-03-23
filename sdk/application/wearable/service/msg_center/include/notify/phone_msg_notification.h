/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: phone message notification
 * Author:
 * Create: 2025-04-25
 */

#ifndef PHONE_MSG_NOTIFICATION_H
#define PHONE_MSG_NOTIFICATION_H

#include <unordered_map>
#include "components/ui_view_group.h"
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button_ext.h"
#include "components/ui_scroll_view.h"
#include "graphic_timer.h"

struct PhoneMsg {
    std::string appName;
    std::string msgTitle;
    std::string msgContent;
};

namespace OHOS {
class PhoneMsgView {
public:
    static UIViewGroup *CreatePhoneMsgView();
    static void FreePhoneMsgView(void);
    static void UpDatePhoneMsg(std::string appName, std::string titile, std::string content);
    static void StartPhoneMsgTimer(int32_t interval);
    static void DisablePhoneMsgTimer();
    static void ShowDateMsg(std::string appName, std::string title, std::string content);
    static void SetControl();
    static const uint32_t notifyTime = 5000;

private:
    static UIScrollView *phoneMsgView_;
    static UILabel *appNameLabel_;
    static UIImageView *appIcon_;
    static UILabel *titleLabel_;
    static UILabel *contentLabel_;
    static GraphicTimer *phoneMsgNotifyStop_;
    static PhoneMsg phoneMsg_;
    static UILabelButtonExt *cancelButton_;
};

class PhoneMsgViewEventListener : public OHOS::UIView::OnClickListener,
                               public OHOS::UIView::OnLongPressListener,
                               public OHOS::UIView::OnTouchListener,
                               public OHOS::UIView::OnDragListener {
public:
    PhoneMsgViewEventListener();
    virtual ~PhoneMsgViewEventListener();
    static PhoneMsgViewEventListener *GetInstance(void);
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(OHOS::UIView& view, const OHOS::DragEvent& event) override;
    bool OnClick(OHOS::UIView& view, const OHOS::ClickEvent& event) override;
    bool OnPress(OHOS::UIView& view, const OHOS::PressEvent& event) override
    {
        return true;
    }
    bool OnLongPress(OHOS::UIView& view, const OHOS::LongPressEvent& event) override
    {
        return true;
    }
    bool OnRelease(OHOS::UIView& view, const OHOS::ReleaseEvent& event) override
    {
        return true;
    }

    bool OnDragStart(OHOS::UIView& view, const OHOS::DragEvent& event) override
    {
        return true;
    }
};
}; // namespace OHOS

#endif /* PHONE_MSG_NOTIFICATION_H */
