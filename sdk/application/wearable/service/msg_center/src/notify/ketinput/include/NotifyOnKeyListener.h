/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: NotifyOnKeyListener.h
 * Author: g30037431
 * Create: 2024-12-12
 */

#ifndef KEY_NOTIFY_INPUT_LISTENER_H
#define KEY_NOTIFY_INPUT_LISTENER_H

#include "components/root_view.h"
#include "events/key_event.h"
#include "ohos_types.h"

namespace OHOS {
class NotifyOnKeyListener : public RootView::OnKeyActListener {
public:
    static NotifyOnKeyListener* GetInstance();
    bool OnKeyAct(UIView& view, const KeyEvent& event) override;

private:
    NotifyOnKeyListener() {}
    ~NotifyOnKeyListener() {}
    void OnKeyPress(void);
    void OnKeyLongPress(const KeyEvent& event);
    void OnKeyRelease(const KeyEvent& event);
    void CallVolumeDipSwitchControl(uint8_t status);
    void PlayerVolumeDipSwitchControl(uint8_t status);
private:
    int32 pressFlag{0};
    int32 longPressFlag{0};
};
}
#endif // KEY_NOTIFY_INPUT_LISTENER_H