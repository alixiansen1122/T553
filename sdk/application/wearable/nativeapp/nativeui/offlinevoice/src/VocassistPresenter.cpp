/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */
#include "AppViewIDs.h"
#include "offlinevoice/VocassistPresenter.h"
#include "player/PlayersModel.h"
#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"
#include "phonemenu/PhoneMenuView.h"

namespace OHOS {

REGIST_MENU(VIEW_OFFLINEVOICE, MainVocassistView, VocassistPresenter, PNG_APPLIST_VOCASSIST_IMAGE, PNG_APPLIST_DEFAULT_IMG, "语音助手");

static VocassistPresenter *g_pVocassistPresenter = nullptr;

VocassistPresenter::VocassistPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VocassistPresenter] VocassistPresenter");
    g_pVocassistPresenter = this;
}

VocassistPresenter::~VocassistPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VocassistPresenter] ~VocassistPresenter");
    VocassistModel::GetInstance()->SetHasOneCmd(false);
    g_pVocassistPresenter = nullptr;
}

VocassistPresenter *VocassistPresenter::GetInstance()
{
    return g_pVocassistPresenter;
}

void VocassistPresenter::VocAssistDeal(int flag)
{
    if (VoiceAssistance::GetInstance() == nullptr) {
        return;
    }

    if (VocassistModel::GetInstance()->GetPhoneDetailStatus() == ONE_ROAD_AND_INCOMING &&
        flag != SWITCH_TO_MAKECALL && flag != SWITCH_TO_CUTCALL && flag != SWITCH_TO_UNKNOWN) {  // 来电只支持接听挂断电话
        int replyText = NO_SUPPORT_SCENE;
        VoiceAssistance::GetInstance()->DetermineCause(&replyText);
    } else if (!PlayersModel::GetInstance()->HasPlayerPlayed() && ((PRE_SONG <= flag && flag <= NEXT_SONG) ||
        (flag == PAUSE_PLAYBACK))) {         // 得先播放音乐才能执行停止播放，上下首
        int replyText = NONE_SONG_PLAY;
        VoiceAssistance::GetInstance()->DetermineCause(&replyText);
    } else {                                                    // 其他情况
        if (SWITCH_TO_MAKECALL <= flag && flag <= SWITCH_TO_UNKNOWN) {              // 播放音乐，接挂电话, 未知命令
            VoiceAssistance::GetInstance()->SwitchMedium(flag);
        } else {                                                                        // 未开发的命令
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Undeveloped Commands!!");
            VocassistModel::GetInstance()->SetToggleFlag(ALREADY_CLOSE_VOCASSIST_IMAGE);
        }
    }

    VocassistModel::GetInstance()->SetExitStatus(CONTINUTE_EXIT_MODE1);
    VoiceAssistance::GetInstance()->DelayDisplayTime(DELAY_SIX_SECONDS);        // 开启定时器，太长时间未操作退出语音助手
}

void VocassistPresenter::OnAnimatorStop(UIView& view) // 播放结束监听
{
    UNUSED(view);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VocassistPresenter] OnAnimatorStop!!");
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance::GetInstance() is nullptr!!");
        return;
    }
    int connectFlag = VocassistModel::GetInstance()->GetConnectFlag();
    if (connectFlag == CONNECT_TO_WAVEFORM) {
        VoiceAssistance::GetInstance()->SwitchMedium(WAVE_VIEW);
    } else if (connectFlag == CONNECT_TO_VOCASSIST) {
        int flag = VocassistModel::GetInstance()->GetRegFlag();
        if (flag < SWITCH_TO_MAKECALL || flag >= SWITCH_TO_VOCASSIST) {
            return;
        }
        VoiceAssistance::GetInstance()->SwitchMedium(VOC_VIEW);
        VocAssistDeal(flag);
    }
}

bool VocassistPresenter::OnClick(UIView& view, const ClickEvent& event) // 语音图标点击事件
{
    UNUSED(event);
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance instance is nullptr!!");
        return false;
    }
    if (strcmp(view.GetViewId(), VOCPHOTO_ID) == 0) {
        if (VocassistModel::GetInstance()->GetToggleFlag() == ALREADY_CLOSE_VOCASSIST_IMAGE) {
            VoiceAssistance::GetInstance()->DeleteWordLabel();
            VoiceAssistance::GetInstance()->SwitchMedium(VOC_TO_WAVE_VIEW);
            VocassistModel::GetInstance()->SetToggleFlag(ALREADY_OPEN_VOCASSIST_IMAGE);
            VocassistModel::GetInstance()->SetHasOneCmd(false);     // 点击后可以继续喊命令
            VocassistModel::GetInstance()->SetRegFlag(SWITCH_TO_UNKNOWN);
            VocassistModel::GetInstance()->SetExitStatus(CONTINUTE_EXIT_MODE2);
            VoiceAssistance::GetInstance()->DelayDisplayTime(DELAY_SIX_SECONDS);
            LocalAsrSetHifiMode();
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Onclick VOCPHOTO_ID");
    }
    return true;
}

bool VocassistPresenter::OnDrag(UIView& view, const DragEvent& event)
{
    UNUSED(view);
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT &&
        strcmp(view.GetViewId(), VOC_ID) == 0) { // 语音助手右滑退出
        int phoneStatus = VocassistModel::GetInstance()->GetPhoneDetailStatus();    // 扫描设置此时通话的状态和个数
        if (phoneStatus == ONE_ROAD_AND_INCOMING) {
            NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
        } else {
            NativeAbility::GetInstance().ChangeSliceToApplist();
        }
    }
    return true;
}

void VocassistPresenter::OnResume(void)
{
    ScreenTurnOn(-1);
}

void VocassistPresenter::OnStop(void)
{
    ScreenTurnOff();
}
}
