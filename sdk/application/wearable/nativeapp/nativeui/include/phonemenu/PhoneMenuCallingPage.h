/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallingPage
 * Create: 2025-06-06
 */

#ifndef PHONE_MENU_CALLING_PAGE_H
#define PHONE_MENU_CALLING_PAGE_H

#include "SlicePage.h"
#include "components/ui_view_group.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_image_view.h"
#include "PhoneMenuPresenter.h"

namespace OHOS {
class PhoneMenuCallingPage : public SlicePage<PhoneMenuPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    PhoneMenuCallingPage();
    ~PhoneMenuCallingPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    static PhoneMenuCallingPage *GetInstance();
    UILabel *GetCallingPhoneNumberLabel();
    UILabel *GetCallingStateLabel();
    enum class CallingState : uint8_t {SESSION, INCOMING, OUTGOING, WAITING};
    CallingState GetCallingState();
private:
    CallingState state_;
    void CreateUILabel(void);
    void CreateUIButton(void);
    UIViewGroup *group_{nullptr};
    UIImageView *icon_{nullptr};
    UILabel *phoneNumber_{nullptr};
    UILabel *stateLabel_{nullptr};
    UIButton *answerButton_{nullptr};
    UIButton *hangUpButton_{nullptr};
    UIButton *volumeButton_{nullptr};
};
}  // namespace OHOS
#endif  // PHONE_MENU_CALLING_PAGE_H