/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: KeyPresenter.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef KEY_INPUT_LISTENER_H
#define KEY_INPUT_LISTENER_H

#include "components/root_view.h"
#include "events/key_event.h"
#include "ohos_types.h"

namespace OHOS {
class KeyInputListener : public RootView::OnKeyActListener {
public:
    static KeyInputListener* GetInstance();
    bool OnKeyAct(UIView& view, const KeyEvent& event) override;

private:
    KeyInputListener() {}
    ~KeyInputListener() {}
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
#endif // KEY_INPUT_LISTENER_H