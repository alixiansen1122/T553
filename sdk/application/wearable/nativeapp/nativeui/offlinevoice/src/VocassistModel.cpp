/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */
#include "phoneservice/PhoneService.h"
#include "main/MainViewSample.h"
#include "offlinevoice/VocassistView.h"
#include "offlinevoice/VocassistModel.h"
#include "AppViewIDs.h"

#include "NativeAbility.h"
#include "graphic_service.h"

#ifdef __cplusplus
extern "C" {
#endif
static constexpr int CMD_LENGTH = 21;
static const char g_voiceWordName[SMART_VOICE_INVALID_CMD_ID + 1][CMD_LENGTH] = { "接听电话",
                                                                                  "拒接电话",
                                                                                  "上一首",
                                                                                  "下一首",
                                                                                  "调大音量",
                                                                                  "调小音量",
                                                                                  "开始播放",
                                                                                  "停止播放",
                                                                                  "无法识别" };
#ifdef __cplusplus
}
#endif

namespace OHOS {
static void RecognVoiceEvent(const SmartVoiceCmdParam data)
{
    SmartVoiceCmdParam *param = nullptr;
    param = (SmartVoiceCmdParam *)&data;
    if (param->cmdId < SMART_VOICE_ACCEPT_CALL_CMD || param->cmdId > SMART_VOICE_INVALID_CMD_ID) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get cmdid failed, funct callback finish!!");
        return;
    }
    if (VocassistModel::GetInstance()->GetPhoneDetailStatus() == ONE_OR_MORE_ROAD_AND_OTHER_STATUS) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Phone is active, voc assistance can not open!!");
        return;
    }

    if (NativeAbility::GetInstance().GetCurSliceId() == VIEW_OFFLINEVOICE) {  // 已经进入语音助手
        if (VoiceAssistance::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance instance is nullptr, funct callback finish!!");
            return;
        }
        if (VocassistModel::GetInstance()->GetToggleFlag() == ALREADY_OPEN_VOCASSIST_IMAGE &&
            (!VocassistModel::GetInstance()->GetHasOneCmd())) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CurSliceId equals VIEW_OFFLINEVOICE");
            VoiceAssistance::GetInstance()->SetCmdContext(g_voiceWordName[param->cmdId]);
            VoiceAssistance::GetInstance()->SwitchMedium(CREATE_CMD_LABEL);
            VocassistModel::GetInstance()->SetHasOneCmd(true);
            VocassistModel::GetInstance()->SetRegFlag(param->cmdId);
            VocassistModel::GetInstance()->SetExitStatus(CONTINUTE_EXIT_MODE2);
            VoiceAssistance::GetInstance()->DelayDisplayTime(500); // 500ms
        }
    }
}

void VoiceFunction(SmartVoiceEventType type, td_void *data, td_s32 size)
{
    int dataSize = sizeof(SmartVoiceCmdParam);
    if (data == nullptr || dataSize != size) {
        return;
    }

    switch (type) {
        case SMART_VOICE_EVENT_VOICE_BEGIN:
                break;
        case SMART_VOICE_EVENT_VOICE_END:
                break;
        case SMART_VOICE_EVENT_VOICE_COMMAND: {
                GraphicService::GetInstance()->PostGraphicEvent(std::bind(RecognVoiceEvent, *(SmartVoiceCmdParam *)data));
                break;
            }
        default:
            break;
    }
}

VocassistModel::VocassistModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistModel] VocassistModel");
}

VocassistModel::~VocassistModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistModel] ~VocassistModel");
}

VocassistModel *VocassistModel::GetInstance()
{
    static VocassistModel vocassistModel;
    return &vocassistModel;
}

void VocassistModel::Init()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistModel] Init");
    LocalAsrInitParams asrpara;
    asrpara.powerMode = ALWAYS_WAKE;
    asrpara.inputMode = INNER_AUDIO_PORT;
    asrpara.enableAEC = DISABLE;
    asrpara.enableNpu = DISABLE;
    LocalAsrInit(&asrpara);
    LocalAsrSetCallback(VoiceFunction);
}

void VocassistModel::UnInit()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistModel] UnInit");
}

int VocassistModel::GetPhoneDetailStatus()     // 设置来电时的状态
{
    int curCallNum = GetCurrCallNum();
    int curCallState = GetCurrCallState();
    if (curCallNum != 0) {
        if (curCallNum == 1 && curCallState == HFP_HF_CALL_STATE_INCOMING) {
            return ONE_ROAD_AND_INCOMING;
        } else {
            return ONE_OR_MORE_ROAD_AND_OTHER_STATUS;
        }
    } else {
        return OTHER_STATUS;
    }
}

int VocassistModel::GetToggleFlag()
{
    return toggleFg;
}

void VocassistModel::SetToggleFlag(int val)
{
    toggleFg = val;
}

int VocassistModel::GetRegFlag()
{
    return regFlag;
}

void VocassistModel::SetRegFlag(int val)
{
    regFlag = val;
}

int VocassistModel::GetExitStatus()
{
    return exitStatus;
}

void VocassistModel::SetExitStatus(int val)
{
    exitStatus = val;
}

int VocassistModel::GetConnectFlag()
{
    return imgConnectFlag;
}

void VocassistModel::SetConnectFlag(int flag)
{
    imgConnectFlag = flag;
}

bool VocassistModel::GetHasOneCmd()
{
    return hasOneCmd;
}

void VocassistModel::SetHasOneCmd(bool fg)
{
    hasOneCmd = fg;
}
}
