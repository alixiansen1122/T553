/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallerLogModel
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_CALLER_LOG_MODEL_H
#define PHONE_MENU_CALLER_LOG_MODEL_H

#include "string"
#include "AppViewIDs.h"
#include "message.h"
#include "msg_center.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuSync.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { CHANGE_STATUS = 0, SCO_STATUS, ERROR_MESSAGE } MessageType;

enum PhoneStatus { PHONE_MULTI_STATUS = 0, PHONE_SINGLE_STATUS };

bool MulitplePhoneTextReplaces();
bool FindCallListInfoAndSetPriorityNumber();
void PhoneScoStatausCallback(hfp_sco_connect_state_t state);
void PhoneChangeStatusCallback(const PhoneCallInfo *phoneCall);
void PhoneErrorCallback(PhoneServiceErrorType type, int volume);
void UsingPhoneScoStatausCallback(hfp_sco_connect_state_t state);
void UsingPhoneChangeStatusCallback(const PhoneCallInfo *phoneCall);
void UsingPhoneErrorCallback(PhoneServiceErrorType type);

namespace OHOS {
class VolumeSettingView;
void HonrsViewChange(void);
VolumeSettingView *GetvolumeSetting(void);
class PhoneMenuCallerLogModel {
public:
    void InitPhoneService();
    static PhoneMenuCallerLogModel *GetInstance(void);
    std::string GetPhoneNumber(void);
    void SetoperateNumber(const char *number, char number_len);
    bool GetPhoneStatus();
    void SetMakePhoneStatus(bool status);
    bool GetVolumeScreenStatus();
    void SetVolumeScreenStatus(bool status);
    bool GetCallStatus();
    void SetCallStatus(bool status);
    bool GetWaitingStatus();
    void SetWaitingStatus(bool status);
    bool GetMulitplePhoneStatus();
    void SetMulitplePhoneStatus(bool status);
    std::string GetDialingStatus(void);
    void SetDialingStatus(const char *text);
    std::string InitDateHourTime();
    std::string &DisplayCallDuration();
    int CurrCallNum();
    bool GetNotifyFlag(void);
    void SetNotifyFlag(bool flag);
    bool IsMulitplePhoneStatus(void);
    std::list<Contacts> &GetContacts();
    bool GetMicMute()
    {
        return isMicMute_;
    }
    void SetMicMute(bool isMicMute)
    {
        isMicMute_ = isMicMute;
    }
    void SaveCurPage();
    void BackToPrePage();

private:
    PhoneMenuCallerLogModel();
    PhoneMenuCallerLogModel(const PhoneMenuCallerLogModel &);
    PhoneMenuCallerLogModel &operator=(const PhoneMenuCallerLogModel &);
    virtual ~PhoneMenuCallerLogModel();

    std::string dialingStatus_;
    std::string timers_;
    std::string callDuration_;
    std::string phoneNumber_;
    bool mulitplePhoneStatus_{false};
    bool waitingStatus_{false};
    bool volumeStatus_{false};
    bool phoneStatus_{false};
    bool callStatus_{false};
    bool notifyFlag_{false};
    bool isMicMute_{false};
    uint16_t preSliceId_{VIEW_MAIN_SAMPLE};
    uint16_t prePageId_{0};
};
}
#ifdef __cplusplus
}
#endif
#endif