/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: NotifyOnKeyListener.cpp
 * Author:g30037431
 * Create: 2024-12-14
 */

#include "NotifyOnKeyListener.h"
#include "common/key_code.h"
#include "AppViewIDs.h"
#include "View.h"
#include "key/KeyInputListener.h"
#include "main/VolumeSettingView.h"
#include "main/VolumeSettingPresenter.h"
#include "main/VolumeSettingView.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "dock/input_device.h"
#include "notification_manager.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "applist/ApplistModel.h"

namespace OHOS {
static bool IsNotifyCallStatus()
{
    uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
    if (NotificationManager::GetInstance()->HasNotifyShowing() &&
        (notifyId == TOPIC_EVENT_HFP_WATCH_ACTIVE || notifyId == TOPIC_EVENT_HFP_PHONE_TO_WATCH)) {
        return true;
    } else {
        return false;
    }
}

static void PressAlarmNotify()
{
    AlarmClockModel::GetInstance()->AlarmGetClockNum();
    AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
    NotificationManager::GetInstance()->StopNotify();
}

NotifyOnKeyListener *NotifyOnKeyListener::GetInstance()
{
    static NotifyOnKeyListener instance;
    return &instance;
}

void NotifyOnKeyListener::OnKeyPress(void)
{
    pressFlag = 1;  // Short press the logo
    return;
}

void NotifyOnKeyListener::OnKeyLongPress(const KeyEvent &event)
{
    longPressFlag = 1;  // Long press the logo

    uint16 keyID = event.GetKeyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) { // Power button
        NativeAbility::GetInstance().ChangeSlice(VIEW_REBOOT); // Switch to shutdown/restart card
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) {  // Function buttons
        // Switch to View System Version Card
    }
    return;
}

void NotifyOnKeyListener::CallVolumeDipSwitchControl(uint8_t status)
{
    VolumeSliderEventListener *volumeSliderEventListener = nullptr;
    if (NotificationManager::GetInstance()->GetVolumeScreenStatus()) {
        VolumeSettingView *settingView = GetvolumeSetting();
        if (settingView == nullptr) {
            return;
        }
        volumeSliderEventListener = settingView->GetVolumeEventListener();
        if (volumeSliderEventListener == nullptr) {
            return;
        }
        volumeSliderEventListener->ChangeVolume(static_cast<VolumeChangeStatus>(status));
    } else {
        HonrsViewChange();
        NotificationManager::GetInstance()->SetVolumeScreenStatus(true);
    }
    return;
}

void NotifyOnKeyListener::OnKeyRelease(const KeyEvent &event)
{
    uint16_t currentViewId = NativeAbility::GetInstance().GetCurSliceId();
    uint16 keyID = event.GetKeyId();
    uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) {  // Power button
        if (longPressFlag == 0 && pressFlag == 1 &&
            NotificationManager::GetInstance()->HasNotifyShowing()) {        // Short press the power button
            if (NotificationManager::GetInstance()->GetWatchCallStatus()) {  // Call notify
                CallVolumeDipSwitchControl(VOLUMEUP); // Volume+
            } else if (notifyId == TOPIC_EVENT_ALARM_RING) {  // Main dial
                PressAlarmNotify();  // When currently on the alarm notification page, process the alarm first before switching pages
            }
        }
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) {  // Function buttons
        if (longPressFlag == 0 && pressFlag == 1) {                             // Short press the function key
            if (NotificationManager::GetInstance()->GetWatchCallStatus()) {     // Volume card
                CallVolumeDipSwitchControl(VOLUMEDOWN);  // Volume-
            } else {
                // Switch to user configured shortcut card
            }
        }
    }

    pressFlag = 0;
    longPressFlag = 0;
    return;
}

bool NotifyOnKeyListener::OnKeyAct(UIView &view, const KeyEvent &event)
{
    UNUSED(view);
    uint8 state = event.GetState();

    switch (state) {
        case InputDevice::STATE_PRESS:  // Short press
            OnKeyPress();
            break;
#ifdef __LITEOS_M__
        case InputDevice::STATE_LONG_PRESS:  // Long press
            OnKeyLongPress(event);
            break;
#endif
        case InputDevice::STATE_RELEASE:  // Release
            OnKeyRelease(event);
            break;
        default:
            break;
    }
    return true;
}
}  // namespace OHOS
