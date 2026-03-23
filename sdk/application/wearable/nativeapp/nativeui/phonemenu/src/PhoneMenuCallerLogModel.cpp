/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogModel
 * Create: 2025-07-12
 */

#include <string>
#include <sys/time.h>
#include "ohos_timer.h"
#include "graphic_timer.h"
#include "samgr_lite.h"
#include "graphic_service.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "NativeAbility.h"
#include "notification_manager.h"
#include "main/VolumeSettingView.h"
#include "main/VolumeSettingPresenter.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallingPage.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS;

static int g_callTimes{0};
static constexpr int NO_NUMBER_STATUS = -1;
static int g_phoneStatus = PHONE_SINGLE_STATUS;
static int g_isCall{NO_NUMBER_STATUS};
static PhoneCallInfo callInfo{};
static int g_retNum{0};
static int g_volumeLocal{0};
GraphicTimer *g_startCounting = nullptr;
static constexpr uint64_t MS_ONE_SECONDS = 1000;
static PhoneCallInfo callList[MULTI_CALL_MAX_NUME]{};
static int g_firstPriority = NO_NUMBER_STATUS;
static int g_secondPriority = NO_NUMBER_STATUS;
static int g_thirdPriority = NO_NUMBER_STATUS;
static int g_fourthPriority = NO_NUMBER_STATUS;
static bool g_isWatchCallView = false;
static constexpr int16 USLEEP_NUM = 5000;
static constexpr int16 ONE_NUM = 1;

static void SetPriorityNumber()
{
    time_t seconds;
    time(&seconds);

    if (g_firstPriority != NO_NUMBER_STATUS) {
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callList[g_firstPriority].unitCalls.number,
                                                                 callList[g_firstPriority].unitCalls.number_len);
        g_callTimes = seconds - callList[g_firstPriority].activeTime;
    } else if (g_secondPriority != NO_NUMBER_STATUS) {
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callList[g_secondPriority].unitCalls.number,
                                                                 callList[g_secondPriority].unitCalls.number_len);
        g_callTimes = seconds - callList[g_secondPriority].activeTime;
    } else if (g_thirdPriority != NO_NUMBER_STATUS) {
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callList[g_thirdPriority].unitCalls.number,
                                                                 callList[g_thirdPriority].unitCalls.number_len);
    } else if (g_fourthPriority != NO_NUMBER_STATUS) {
        PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(callList[g_fourthPriority].unitCalls.number,
                                                                 callList[g_fourthPriority].unitCalls.number_len);
    }
    return;
}

static void DisableCallingTimer()
{
    if (g_startCounting != nullptr) {
        delete g_startCounting;
        g_startCounting = nullptr;
        g_callTimes = 0;
    }
}

static void StatusReset()
{
    PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(false);
    PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(false);
    PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(false);
    DisableCallingTimer();
    if (!PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        PhoneMenuCallerLogModel::GetInstance()->BackToPrePage();
    } else {
        g_isWatchCallView = false;
    }
    return;
}

static void CallTimerCallbackProc(void *data)
{
    g_callTimes++;

    if (PhoneMenuCallingPage::GetInstance() == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallTimerCallbackProc] GetInstance is nullptr");
        return;
    }

    if (PhoneMenuCallingPage::GetInstance()->GetCallingStateLabel() == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP,
                      "[CallTimerCallbackProc] GetInstance()->GetCallingStateLabel is nullptr");
        return;
    }

    if (PhoneMenuCallingPage::GetInstance()->GetCallingState() == PhoneMenuCallingPage::CallingState::SESSION) {
        PhoneMenuCallingPage::GetInstance()->GetCallingStateLabel()->SetText(
            PhoneMenuCallerLogModel::GetInstance()->DisplayCallDuration().c_str());
    }
}

static void CallTimerCallback(void *data)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(CallTimerCallbackProc, data));
}

static void InitCallingTimer()
{
    if (g_startCounting != nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogModel::CallerTimer already exists!!");
        return;
    }
    uint32 tick = GetOSTick(MS_ONE_SECONDS);
    g_startCounting = new GraphicTimer(tick, CallTimerCallback, nullptr, true);
    if (g_startCounting == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogModel::CallerTimer new GraphicTimer failed!!");
        return;
    }
    bool retTimer = g_startCounting->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogModel::CallerTimer Start failed!!");
        if (g_startCounting != nullptr) {
            delete g_startCounting;
            g_startCounting = nullptr;
        }
        return;
    }
}

bool FindCallListInfoAndSetPriorityNumber()
{
    g_isCall = NO_NUMBER_STATUS;
    g_firstPriority = NO_NUMBER_STATUS;
    g_secondPriority = NO_NUMBER_STATUS;
    g_thirdPriority = NO_NUMBER_STATUS;
    g_fourthPriority = NO_NUMBER_STATUS;
    (void)memset_s(callList, MULTI_CALL_MAX_NUME * sizeof(PhoneCallInfo), 0,
                   MULTI_CALL_MAX_NUME * sizeof(PhoneCallInfo));
    int ret = GetCallInfoInCallList(callList, MULTI_CALL_MAX_NUME, &g_retNum);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "FindCallListInfoAndSetPriorityNumber Error ret = %d", ret);
        return false;
    }
    if (g_retNum == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "FindCallListInfoAndSetPriorityNumber no call num");
        return false;
    }

    if (g_retNum > 0 && g_retNum <= MULTI_CALL_MAX_NUME) {
        for (int i = 0; i < g_retNum; i++) {
            if (callList[i].lastStatus == HFP_HF_CALL_STATE_ACTIVE) {
                g_firstPriority = i;
                g_isCall = callList[i].lastStatus;
                break;
            } else if (callList[i].lastStatus == HFP_HF_CALL_STATE_HELD) {
                g_secondPriority = i;
                g_isCall = callList[i].lastStatus;
            } else if (callList[i].lastStatus == HFP_HF_CALL_STATE_ALERTING ||
                       callList[i].lastStatus == HFP_HF_CALL_STATE_DIALING) {
                if (g_secondPriority == NO_NUMBER_STATUS) {
                    g_isCall = callList[i].lastStatus;
                }
                g_thirdPriority = i;
            } else if (callList[i].lastStatus == HFP_HF_CALL_STATE_INCOMING ||
                       callList[i].lastStatus == HFP_HF_CALL_STATE_WAITING) {
                if (g_isCall == NO_NUMBER_STATUS && callList[i].lastStatus == HFP_HF_CALL_STATE_INCOMING) {
                    g_isCall = callList[i].lastStatus;
                }
                g_fourthPriority = i;
            }
        }
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FindCallListInfoAndSetPriorityNumber] [%d] [%d] [%d] [%d], g_isCall = %d",
                  g_firstPriority, g_secondPriority, g_thirdPriority, g_fourthPriority, g_isCall);
    SetPriorityNumber();
    return true;
}

static void CallModelResumePlayer(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallModelResumePlayer] [ResumePlayer] start");
    usleep(USLEEP_NUM);
}

static void CallModelPostGraphicEvent(void (*fun)(void))
{
    GraphicService::GetInstance()->PostGraphicEvent(*fun);
}

static int g_callOperate = OPERATE_NO_OPE;

static void ExitMakePhoneSlice(void)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
        (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET)) {
        WEARABLE_LOGD(HILOG_MODULE_AAFWK, "ExitMakePhoneSlice");

        if (GetCurrCallNum() == 0) {
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            CallModelPostGraphicEvent(CallModelResumePlayer);
        } else {
            NativeAbility::GetInstance().ChangeSliceToApplist();
        }
    }
    PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(false);
    DisableCallingTimer();
    if (PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        g_isWatchCallView = false;
    }
}

static void DialingHandle(void)
{
    PhoneMenuCallerLogModel::GetInstance()->SetDialingStatus("正在拨号");
    PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(true);
}

static void AlertingHandle(void)
{
    PhoneMenuCallerLogModel::GetInstance()->SetDialingStatus("正在响铃");
    PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(true);
}

static void IncomingHandle(void)
{
    PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(false);
}

static void WaitingOpreate()
{
    if (!IsCompleteCallFlow()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[UsingPhoneChangeStatusCallback] is not complete call flow");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[WaitingOpreate] WaitingOpreate");
    PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(false);
    PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(true);
    if (!PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        uint32_t targetId = NativeAbility::GetInstance().GetCurSliceId();
        if (targetId == VIEW_PHONE_MENU) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLING_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        } else {
            NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
        }
    } else {
        g_isWatchCallView = true;
    }
}

static void WatchActiveHandle(const PhoneCallInfo *phoneCall)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        NativeAbility::GetInstance().GetCurSliceId() == VIEW_OFFLINEVOICE ||
        PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        time_t seconds;
        time(&seconds);
        g_callTimes = seconds - phoneCall->activeTime;

        InitCallingTimer();
        PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
    }
    if (PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        g_isWatchCallView = true;
    }
}

static void PhoneActiveHandle(void)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        g_isWatchCallView) {
        if (GetCurrCallNum() > ONE_NUM) {
            g_phoneStatus = PHONE_MULTI_STATUS;
            if (GetScoConnectState() == HFP_SCO_STATE_DISCONNECTED) {
                DisableCallingTimer();
            }
        }
        TransCallToMobilePhone();
        PhoneMenuCallerLogModel::GetInstance()->BackToPrePage();
        CallModelPostGraphicEvent(CallModelResumePlayer);
    }
    usleep(USLEEP_NUM);
    g_callOperate = OPERATE_WATCH_ACTIVE;
}

static void OppoActiveHandle(const PhoneCallInfo *phoneCall)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        g_isWatchCallView) {
        if (GetCurrCallNum() > ONE_NUM) {
            g_phoneStatus = PHONE_MULTI_STATUS;
            return;
        }
        time_t seconds;
        time(&seconds);
        g_callTimes = seconds - phoneCall->activeTime;
        InitCallingTimer();
        PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
    }
}

bool MulitplePhoneTextReplaces()
{
    DisableCallingTimer();
    PhoneMenuCallingPage::GetInstance()->GetCallingPhoneNumberLabel()->SetText("正在通话");
    PhoneMenuCallingPage::GetInstance()->GetCallingStateLabel()->SetText("");
    return true;
}

static bool MultiplePhonesViewDisplay()
{
    if (g_phoneStatus == PHONE_MULTI_STATUS) {
        if (NativeAbility::GetInstance().GetCurTargetId() !=
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE))) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[MultiplePhonesViewDisplay]GetCurSliceId no PHONE_MENU_CALLING_PAGE");
            DisableCallingTimer();
            PhoneMenuCallerLogModel::GetInstance()->SetMulitplePhoneStatus(true);
            PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
            if (!PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
                NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
            } else {
                g_isWatchCallView = true;
            }
            return true;
        }
        if (!MulitplePhoneTextReplaces()) {
            return false;
        }
        return true;
    }
    return false;
}

static void NonWatchFinishHandle(int laststatus)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        if (GetCurrCallNum() == 0) {
            PhoneMenuCallerLogModel::GetInstance()->SetNotifyFlag(false);
            StatusReset();
            CallModelPostGraphicEvent(CallModelResumePlayer);
            g_phoneStatus = PHONE_SINGLE_STATUS;
        } else if (g_phoneStatus == PHONE_MULTI_STATUS) {
            MultiplePhonesViewDisplay();
        } else if (FindCallListInfoAndSetPriorityNumber()) {
            if (laststatus == HFP_HF_CALL_STATE_ACTIVE || laststatus == HFP_HF_CALL_STATE_HELD) {
                PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(false);
                PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(false);
            } else if (laststatus == HFP_HF_CALL_STATE_WAITING) {
                PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
                PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(false);
            }
            uint32_t targetId = NativeAbility::GetInstance().GetCurSliceId();
            if (targetId == VIEW_PHONE_MENU) {
                NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLING_PAGE,
                    TransitionType::TRANSITION_INVALID, false);
            } else {
                NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
            }
        }
        if (GetCurrCallNum() == 0) {
            PhoneMenuCallerLogModel::GetInstance()->SetNotifyFlag(false);
        }
    }
    if (GetCurrCallNum() == 0) {
        g_phoneStatus = PHONE_SINGLE_STATUS;
    }
}

static void WatchFinishHandle(void)
{
    if (NativeAbility::GetInstance().GetCurTargetId() ==
            (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        if (GetCurrCallNum() == 0) {
            PhoneMenuCallerLogModel::GetInstance()->SetNotifyFlag(false);
            StatusReset();
        } else {
            StatusReset();
            for (int i = GetCurrCallNum(); i > 0; i--) {
                FinishCall();
            }
        }
    }
    g_phoneStatus = PHONE_SINGLE_STATUS;
}

static void ChangeStatusScreendisplay(int lastStatus)
{
    if (GetScoConnectState() != HFP_SCO_STATE_CONNECTED) {
        if (lastStatus == HFP_HF_CALL_STATE_ACTIVE) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] STATE_ACTIVE");
            StatusReset();
            return;
        } else if (lastStatus == HFP_HF_CALL_STATE_INCOMING) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] STATE_INCOMING");
        } else if (GetCurrCallNum() == ONE_NUM &&
                   (lastStatus == HFP_HF_CALL_STATE_DIALING || lastStatus == HFP_HF_CALL_STATE_ALERTING)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] STATE_ALERTING");
        } else if (GetCurrCallNum() > ONE_NUM) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] GetCurrCallNum MULITE");
            DisableCallingTimer();
            return;
        }
    }
    if (!IsCompleteCallFlow()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] is not complete call flow");
        return;
    }
    if (MultiplePhonesViewDisplay()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ChangeStatusScreendisplay] MultiplePhonesViewDisplay");
        return;
    }
    if (g_callOperate != OPERATE_PHONE_ACTIVE && (!PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag())) {
        uint32_t targetId = NativeAbility::GetInstance().GetCurSliceId();
        if (targetId == VIEW_PHONE_MENU) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLING_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        } else {
            NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
        }
    }
}

void UsingPhoneChangeStatusCallback(const PhoneCallInfo *phoneCall)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[UsingPhoneChangeStatusCallback] operate:%d, callNUm:%d, call flow:%d",
                  phoneCall->operate, GetCurrCallNum(), IsCompleteCallFlow());

    if (phoneCall->operate == OPERATE_WATCH_FINISH && GetCurrCallNum() == 0) {
        g_callOperate = OPERATE_WATCH_FINISH;
        WatchFinishHandle();
        return;
    }
    PhoneMenuCallerLogModel::GetInstance()->SetoperateNumber(phoneCall->unitCalls.number,
                                                             phoneCall->unitCalls.number_len);
    g_callOperate = phoneCall->operate;

    switch (g_callOperate) {
        case OPERATE_DIALING:
            DialingHandle();
            break;
        case OPERATE_ALERTING:
            AlertingHandle();
            break;
        case OPERATE_INCOMING:
            IncomingHandle();
            break;
        case OPERATE_WAITING:
            WaitingOpreate();
            return;
        case OPERATE_WATCH_ACTIVE:
            WatchActiveHandle(phoneCall);
            break;
        case OPERATE_PHONE_ACTIVE:
            PhoneActiveHandle();
            return;
        case OPERATE_PEER_ACTIVE:
            OppoActiveHandle(phoneCall);
            break;
        case OPERATE_PHONE_HOLD:
            return;
        case OPERATE_WATCH_FINISH:
            WatchFinishHandle();
            return;
        case OPERATE_WATCH_REJECT:
        case OPERATE_NON_WATCH_FINISH:
            NonWatchFinishHandle(phoneCall->lastStatus);
            return;
        default:
            break;
    }
    ChangeStatusScreendisplay(phoneCall->lastStatus);
}

void UsingPhoneErrorCallback(PhoneServiceErrorType type)
{
    if (type == ERROR_DIAL) {
    } else if (type == ERROR_BT) {
        WEARABLE_LOGD(HILOG_MODULE_AAFWK, "UsingPhoneErrorCallback BT");
        ExitMakePhoneSlice();
    }
}

static void ScoConnectHandle(void)
{
    if (GetCurrCallNum() == 0) {
        return;
    }

    if (!FindCallListInfoAndSetPriorityNumber()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ScoConnectHandle FindCallListInfoAndSetPriorityNumber false");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoConnectHandle] g_isCall = %d", g_isCall);
    switch (g_isCall) {
        case HFP_HF_CALL_STATE_ACTIVE:
            if (g_callOperate == OPERATE_PHONE_ACTIVE) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoConnectHandle] OPERATE_PHONE_ACTIVE");
                return;
            }
            PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
            InitCallingTimer();
            break;
        case HFP_HF_CALL_STATE_ALERTING:
            PhoneMenuCallerLogModel::GetInstance()->SetDialingStatus("正在响铃");
            PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(true);
            break;
        case HFP_HF_CALL_STATE_DIALING:
            PhoneMenuCallerLogModel::GetInstance()->SetDialingStatus("正在拨号");
            PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(true);
            break;
        case HFP_HF_CALL_STATE_INCOMING:
            PhoneMenuCallerLogModel::GetInstance()->SetMakePhoneStatus(false);
            break;
        case HFP_HF_CALL_STATE_HELD:
            PhoneMenuCallerLogModel::GetInstance()->SetCallStatus(true);
            InitCallingTimer();
            break;
        default:
            break;
    }
    g_isCall = NO_NUMBER_STATUS;
    if (!IsCompleteCallFlow()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoConnectHandle] is not complete call flow");
        return;
    }
    if (MultiplePhonesViewDisplay()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoConnectHandle] MultiplePhonesViewDisplay");
        return;
    }
    if (!PhoneMenuCallerLogModel::GetInstance()->GetNotifyFlag()) {
        uint32_t targetId = NativeAbility::GetInstance().GetCurSliceId();
        if (targetId == VIEW_PHONE_MENU) {
            NativeAbility::GetInstance().SwitchPageInSlice(PhoneMenuPages::PHONE_MENU_CALLING_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        } else {
            NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
        }
    } else {
        g_isWatchCallView = true;
    }
}

static void ScoDisconnectHandle(void)
{
    ExitMakePhoneSlice();
    PhoneMenuCallerLogModel::GetInstance()->SetWaitingStatus(false);
}

void UsingPhoneScoStatausCallback(hfp_sco_connect_state_t state)
{
    WEARABLE_LOGD(HILOG_MODULE_AAFWK, "UsingPhoneScoStatausCallback SOC state: %d", state);

    if (!IsCompleteCallFlow()) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[UsingPhoneScoStatausCallback] is not complete call flow");
        return;
    }

    if (state == HFP_SCO_STATE_CONNECTED) {
        ScoConnectHandle();
    } else if (state == HFP_SCO_STATE_DISCONNECTED) {
        ScoDisconnectHandle();
    }
}

void PhoneChangeStatusCallback(const PhoneCallInfo *phoneCall)
{
    (void)memset_s(&callInfo, sizeof(PhoneCallInfo), 0, sizeof(PhoneCallInfo));
    (void)memcpy_s(&callInfo, sizeof(PhoneCallInfo), phoneCall, sizeof(PhoneCallInfo));
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(UsingPhoneChangeStatusCallback, &callInfo));
}

void PhoneErrorCallback(PhoneServiceErrorType type, int volume)
{
    g_volumeLocal = volume;
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(UsingPhoneErrorCallback, type));
}

void PhoneScoStatausCallback(hfp_sco_connect_state_t state)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(UsingPhoneScoStatausCallback, state));
}

namespace OHOS {
static constexpr int32 NUM_TEN = 10;
static constexpr int16 NUM_TIME_MIN = 60;
static constexpr int16 NUM_TIME_HOUR = 3600;
static constexpr int32 NUM_TIME_DAY = 86400;
VolumeSettingView *g_volumeSetting = nullptr;

void HonrsViewChange(void)
{
    if (g_volumeSetting == nullptr) {
        g_volumeSetting = new VolumeSettingView();
        if (g_volumeSetting == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CallViewEventListener::g_volumeSetting nullptr err");
            return;
        }
        if (!g_volumeSetting->SetUpVolume()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CallViewEventListener::g_volumeSetting SetUpVolume fail");
            return;
        }
        g_volumeSetting->GetProgress()->SetValue(
            AudioManager::GetInstance().GetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO));
        if (NotificationManager::GetInstance()->screenContainer_ != nullptr) {
            NotificationManager::GetInstance()->screenContainer_->Add(g_volumeSetting);
        }
    }
    VolumeSliderEventListener *volumeSliderEventListener = g_volumeSetting->GetVolumeEventListener();
    if (volumeSliderEventListener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CallViewEventListener::volumeSliderEventListener nullptr err");
        return;
    }

    volumeSliderEventListener->SetPlayerCoefficients(VOLUMEPHONENOTIFY);
    g_volumeSetting->SetVisible(true);
    RootView::GetInstance()->Invalidate();
}

VolumeSettingView *GetvolumeSetting(void)
{
    return g_volumeSetting;
}

PhoneMenuCallerLogModel::PhoneMenuCallerLogModel()
{
    PhoneMenuSync::ReadFileContact(CONTACTS_FILE_PATH);
}

PhoneMenuCallerLogModel::~PhoneMenuCallerLogModel() {}

std::list<Contacts> &PhoneMenuCallerLogModel::GetContacts()
{
    return PhoneMenuSync::GetContacts();
}

void PhoneMenuCallerLogModel::InitPhoneService()
{
    WEARABLE_LOGD(HILOG_MODULE_AAFWK, "PhoneMenuCallerLogModel::[InitPhoneService]::Init start");
    PhoneServiceInit();
    PhoneServiceCb serviceFoution;

    serviceFoution.callChanged = PhoneChangeStatusCallback;
    serviceFoution.phoneServiceErrorCb = PhoneErrorCallback;
    serviceFoution.scoStatusCb = PhoneScoStatausCallback;
    int ret = RegisterCallChangedCb(&serviceFoution);
    WEARABLE_LOGD(HILOG_MODULE_AAFWK, "PhoneMenuCallerLogModel::[InitPhoneService]::Init ret = %d", ret);
}

std::string PhoneMenuCallerLogModel::InitDateHourTime(void)
{
    struct tm pTM;
    time_t seconds;
    time(&seconds);
    localtime_r(&seconds, &pTM);
    timers_.erase();
    if (pTM.tm_min < NUM_TEN) {
        std::string min = "0" + std::to_string(pTM.tm_min);
        timers_ = std::to_string(pTM.tm_hour) + ":" + min;
        return timers_;
    }
    timers_ = std::to_string(pTM.tm_hour) + ":" + std::to_string(pTM.tm_min);
    return timers_;
}

std::string &PhoneMenuCallerLogModel::DisplayCallDuration()
{
    callDuration_.erase();
    std::string hours;
    std::string minute;
    std::string seconds;
    if (g_callTimes >= NUM_TIME_DAY) {
        g_callTimes = 0;
    }
    int hour = g_callTimes / NUM_TIME_HOUR;
    int min = (g_callTimes % NUM_TIME_HOUR) / NUM_TIME_MIN;
    int sec = (g_callTimes % NUM_TIME_HOUR) % NUM_TIME_MIN;
    if (hour >= 0 && hour < NUM_TEN) {
        hours = "0" + std::to_string(hour);
    } else {
        hours = std::to_string(hour);
    }
    if (min >= 0 && min < NUM_TEN) {
        minute = "0" + std::to_string(min);
    } else {
        minute = std::to_string(min);
    }
    if (sec >= 0 && sec < NUM_TEN) {
        seconds = "0" + std::to_string(sec);
    } else {
        seconds = std::to_string(sec);
    }

    if (hour == 0) {
        callDuration_ = "通话中  " + minute + ":" + seconds;
    } else {
        callDuration_ = "通话中  " + hours + ":" + minute + ":" + seconds;
    }
    return callDuration_;
}

void PhoneMenuCallerLogModel::SetDialingStatus(const char *text)
{
    dialingStatus_.clear();
    dialingStatus_.append(reinterpret_cast<const char *>(text));
}

std::string PhoneMenuCallerLogModel::GetDialingStatus(void)
{
    return dialingStatus_;
}

void PhoneMenuCallerLogModel::SetNotifyFlag(bool flag)
{
    notifyFlag_ = flag;
}

bool PhoneMenuCallerLogModel::GetNotifyFlag(void)
{
    return notifyFlag_;
}

bool PhoneMenuCallerLogModel::IsMulitplePhoneStatus(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "IsMulitplePhoneStatus phoneStatus_:%d", g_phoneStatus);
    return g_phoneStatus == PHONE_MULTI_STATUS;
}

void PhoneMenuCallerLogModel::SetMulitplePhoneStatus(bool status)
{
    mulitplePhoneStatus_ = status;
}

bool PhoneMenuCallerLogModel::GetMulitplePhoneStatus()
{
    return mulitplePhoneStatus_;
}

bool PhoneMenuCallerLogModel::GetWaitingStatus()
{
    return waitingStatus_;
}
void PhoneMenuCallerLogModel::SetWaitingStatus(bool status)
{
    waitingStatus_ = status;
}

void PhoneMenuCallerLogModel::SetCallStatus(bool status)
{
    callStatus_ = status;
}

bool PhoneMenuCallerLogModel::GetCallStatus()
{
    return callStatus_;
}

void PhoneMenuCallerLogModel::SetVolumeScreenStatus(bool status)
{
    volumeStatus_ = status;
}

bool PhoneMenuCallerLogModel::GetVolumeScreenStatus()
{
    return volumeStatus_;
}

void PhoneMenuCallerLogModel::SetoperateNumber(const char *number, char number_len)
{
    phoneNumber_.clear();
    phoneNumber_.append(number, number_len);
}

std::string PhoneMenuCallerLogModel::GetPhoneNumber(void)
{
    return phoneNumber_;
}

void PhoneMenuCallerLogModel::SetMakePhoneStatus(bool status)
{
    phoneStatus_ = status;
}

bool PhoneMenuCallerLogModel::GetPhoneStatus()
{
    return phoneStatus_;
}

int PhoneMenuCallerLogModel::CurrCallNum(void)
{
    return GetCurrCallNum();
}

PhoneMenuCallerLogModel *PhoneMenuCallerLogModel::GetInstance(void)
{
    static PhoneMenuCallerLogModel callLogModel;
    return &callLogModel;
}

void PhoneMenuCallerLogModel::SaveCurPage()
{
    uint32_t targetId = NativeAbility::GetInstance().GetCurTargetId();
    if (targetId == VIEW_AOD) {
        preSliceId_ = VIEW_MAIN_SAMPLE;
        prePageId_ = 0;
    } else {
        preSliceId_ = static_cast<uint16_t>(targetId & SLICE_MASK);
        prePageId_ = static_cast<uint16_t>(targetId >> PAGE_OFFSET);
    }
    WEARABLE_LOGD(
        WEARABLE_LOG_MODULE_APP, "PhoneMenuCallerLogModel SaveCurPage:%d %d %d", targetId, preSliceId_, prePageId_);
}

void PhoneMenuCallerLogModel::BackToPrePage()
{
    if (prePageId_ != 0) {
        NativeAbility::GetInstance().SwitchSlice(preSliceId_, prePageId_);
    } else {
        NativeAbility::GetInstance().ChangeSlice(preSliceId_);
    }
}
}
#ifdef __cplusplus
}
#endif
