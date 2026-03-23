/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: KeyPresenter.cpp
 * Author:
 * Create: 2021-09-18
 */

#include "key/KeyInputListener.h"
#include "common/key_code.h"
#include "AppViewIDs.h"
#include "View.h"
#include "main/MainViewSample.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "dock/input_device.h"
#include "notification_manager.h"
#include "alarm/AlarmPopUpPresenter.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "settings/model/SettingDesktopModel.h"
#include "quickaccess/FloatAccessView.h"

namespace OHOS {
// 可以弹窗快捷应用的应用页
static uint16_t g_viewIds[] = {
    VIEW_MAIN_SAMPLE,   // 主表盘
    VIEW_APPLIST,       // 菜单页
};

static void PressQuickApp(uint16_t viewId)
{
    if (FloatAccessView::GetInstance()->IsShowView()) {
        FloatAccessView::GetInstance()->ExitView();
    } else {
        int len = sizeof(g_viewIds) / sizeof(uint16_t);
        for (int i = 0 ; i < len; i++) {
            if (viewId == g_viewIds[i]) {
                FloatAccessView::GetInstance()->ShowView();
                return;
            }
        }
    }
}

static bool IsNotifyCallStatus()
{
    uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
    if (NotificationManager::GetInstance()->HasNotifyShowing() &&
        (notifyId == TOPIC_EVENT_HFP_WATCH_ACTIVE ||
         notifyId == TOPIC_EVENT_HFP_PHONE_TO_WATCH)) {
        return true;
    } else {
        return false;
    }
}

static void PressAlarmNotify()
{
    uint16_t notifyId = NotificationManager::GetInstance()->GetShowingNotifyId();
    if (NotificationManager::GetInstance()->HasNotifyShowing() && notifyId == TOPIC_EVENT_ALARM_RING) {
        AlarmClockModel::GetInstance()->AlarmGetClockNum();
        AlarmPopUpPresenter::GetInstance()->DelayAlarmRing();
        NotificationManager::GetInstance()->StopNotify();
    }
}

KeyInputListener* KeyInputListener::GetInstance()
{
    static KeyInputListener instance;
    return &instance;
}

void KeyInputListener::OnKeyPress(void)
{
    pressFlag = 1; // 短按标志
    return;
}

void KeyInputListener::OnKeyLongPress(const KeyEvent& event)
{
    longPressFlag = 1; // 长按标志

    uint16 keyID = event.GetKeyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) { // 电源按键
        // 切换到关机/重启卡片
        NativeAbility::GetInstance().ChangeSlice(VIEW_REBOOT);
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) { // 功能按键
        PressQuickApp(NativeAbility::GetInstance().GetCurSliceId());
    }
    return;
}

void KeyInputListener::CallVolumeDipSwitchControl(uint8_t status)
{
    VolumeSliderEventListener *volumeSliderEventListener = nullptr;
    if (PhoneMenuCallerLogModel::GetInstance()->GetCallStatus()) {
        if (PhoneMenuCallerLogModel::GetInstance()->GetVolumeScreenStatus()) {
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
            PhoneMenuCallerLogModel::GetInstance()->SetVolumeScreenStatus(true);
        }
    }
    if (IsNotifyCallStatus()) {
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
    }
}

void KeyInputListener::OnKeyRelease(const KeyEvent& event)
{
    uint32_t targetId = NativeAbility::GetInstance().GetCurTargetId();
    uint16_t curSlice = static_cast<uint16_t>(targetId & SLICE_MASK);
    uint16_t curPage = static_cast<uint16_t>(targetId >> PAGE_OFFSET);
    uint16 keyID = event.GetKeyId();
    if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER)) { // 电源按键
        if (longPressFlag == 0 && pressFlag == 1) { // 短按电源键
            // 先关闭快捷应用悬浮框再处理事件
            if (FloatAccessView::GetInstance()->IsShowView()) {
                FloatAccessView::GetInstance()->CleanView();
            }
            if ((curSlice == VIEW_PHONE_MENU && curPage == PhoneMenuPages::PHONE_MENU_CALLING_PAGE) ||
                NotificationManager::GetInstance()->GetWatchCallStatus()) { // 音量卡片
                // 音量+
                CallVolumeDipSwitchControl(VOLUMEUP);
            } else if (curSlice != VIEW_MAIN_SAMPLE) { // 主表盘
                PressAlarmNotify(); // 当前为闹钟通知页面时，先处理闹钟再切换页面
                NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE, TransitionType::TRANSITION_ZOOM_OUT);
            } else {
                PressAlarmNotify(); // 当前为闹钟通知页面时，先处理闹钟再切换页面
                if (!MainViewSample::GetInstance()->IsMainClockPage()) { // 当前页面为主表盘的其他页面时，切换回主表盘页面
                    MainViewSample::GetInstance()->SwitchToClockPage();
                } else {
                    if (SettingDesktopModel::GetInstance().GetDesktopStyle() == DesktopStyle::WATERFALL_STYLE) {
                        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST,
                                                                 TransitionType::TRANSITION_ENTER_WATERFALL);
                    } else {
                        NativeAbility::GetInstance().ChangeSlice(VIEW_APPLIST, TransitionType::TRANSITION_ZOOM);
                    }
                }
            }
        }
    } else if (keyID == static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC)) { // 功能按键
        if (longPressFlag == 0 && pressFlag == 1) { // 短按功能键
            if ((curSlice == VIEW_PHONE_MENU && curPage == PhoneMenuPages::PHONE_MENU_CALLING_PAGE) ||
                NotificationManager::GetInstance()->GetWatchCallStatus()) { // 音量卡片
                // 音量-
                CallVolumeDipSwitchControl(VOLUMEDOWN);
            } else if (curSlice != VIEW_RECENT_APP) {
                NativeAbility::GetInstance().ChangeSlice(VIEW_RECENT_APP);
            }
        }
    }

    pressFlag = 0;
    longPressFlag = 0;
    return;
}

bool KeyInputListener::OnKeyAct(UIView& view, const KeyEvent& event)
{
    UNUSED(view);

    uint8 state = event.GetState();
    switch (state) {
        case InputDevice::STATE_PRESS: // 短按
            OnKeyPress();
            break;
#ifdef __LITEOS_M__
        case InputDevice::STATE_LONG_PRESS: // 长按
            OnKeyLongPress(event);
            break;
#endif
        case InputDevice::STATE_RELEASE: // 释放
            OnKeyRelease(event);
            break;
        default:
            break;
    }
    return true;
}
}
