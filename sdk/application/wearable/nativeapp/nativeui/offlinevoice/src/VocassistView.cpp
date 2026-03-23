/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */

#include <limits>
#include "ohos_timer.h"
#include "graphic_timer.h"
#include "AppViewIDs.h"
#include "player/PlayersModel.h"
#include "phonemenu/PhoneMenuView.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "offlinevoice/VocassistModel.h"
#include "offlinevoice/VocassistView.h"
#include "offlinevoice/VocassistPresenter.h"

static constexpr uint16_t SWITCH_MAP_NUM = 16;
static constexpr uint16_t ANI_X = 84;
static constexpr uint16_t ANI_Y = 108;
static constexpr uint16_t ANI_SL = 116;

static constexpr uint16_t WAVEFORM_CHANGE_START = 9;
static constexpr uint16_t WAVEFORM_CHANGE_PICTURE_TOTAL = 34;
static constexpr uint16_t WAVEFORM_CHANGE_PICTURE_INTERVAL = 50;

static constexpr uint16_t WAVEFORM_CHANGE_END = 51;
static constexpr uint16_t ACTIVE_VOCASSIST_PICTURE_TOTAL = 20;
static constexpr uint16_t ACTIVE_VOCASSIST_PICTURE_INTERVAL = 100;

static constexpr uint16_t VOCASSIST_XLABEL = 180;
static constexpr uint16_t VOCASSIST_YLABEL = 60;
static constexpr uint16_t VOCASSIST_WIDTH = 80;
static constexpr uint16_t VOCASSIST_HEIGHTH = 80;

static constexpr uint16_t WORD_LENGTH = 60;
static constexpr int16_t ERROR_RETVAL = -1;

namespace OHOS {
GraphicTimer* g_closeVocassistHandle = nullptr;
static VoiceAssistance *g_pVoiceAssistance = nullptr;

enum {
    WITHOUT_OPERATE = PLAYER_RESPONESE_MAX,
    NO_SUPPORT_VOC_IN_CALLING,
    VOC_RESPONSE,
    WITHOUT_INCOMING_PHONE,
    RESPONESE_MAX
};

using viewFunct = int (VoiceAssistance::*)();

using Viewmapper = struct {
    uint16_t viewIndex;
    viewFunct funct;
};

static constexpr Viewmapper g_switchMap[SWITCH_MAP_NUM] = {
    {SWITCH_TO_MAKECALL, &VoiceAssistance::ChangeAcceptPhone},
    {SWITCH_TO_CUTCALL, &VoiceAssistance::ChangeHangupPhone},
    {PRE_SONG, &VoiceAssistance::ChangePreSong},
    {NEXT_SONG, &VoiceAssistance::ChangeNextSong},
    {INCREASE_VOLUME, &VoiceAssistance::ChangeIncreaseVolume},
    {DECREASE_VOLUME, &VoiceAssistance::ChangeDecreaseVolume},
    {SWITCH_TO_PLAYER, &VoiceAssistance::ChangeViewToPlayer},
    {PAUSE_PLAYBACK, &VoiceAssistance::ChangePausePlayback},
    {SWITCH_TO_UNKNOWN, &VoiceAssistance::ResponseWithoutUse},
    {CREATE_CMD_LABEL, &VoiceAssistance::CreateCmdLabel},
    {VOC_VIEW, &VoiceAssistance::VocAssistChange},
    {WAVE_VIEW, &VoiceAssistance::WaveFormChange},
    {VOC_TO_WAVE_VIEW, &VoiceAssistance::WithDetectVocImg},
    {WAVE_TO_VOC_VIEW, &VoiceAssistance::WithoutDetectVocImg}
};

static constexpr char g_resStr[RESPONESE_MAX][WORD_LENGTH]{
    {"好的，马上"},
    {"没有歌曲正在播放"},
    {"已经在播放歌曲"},
    {"歌曲已经暂停"},
    {"歌曲已经停止"},
    {"抱歉，执行失败"},
    {"抱歉当前状态不支持此操作"},
    {"抱歉，我刚才开小差了，没听清你说了什么"},
    {"抱歉，通话时不支持使用语音助手"},
    {"我在"},
    {"当前没有来电"}
};

VoiceAssistance::VoiceAssistance()
{
    g_pVoiceAssistance = this;
}

VoiceAssistance::~VoiceAssistance()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~VoiceAssistance!!");
    RemoveAll();
    g_pVoiceAssistance = nullptr;
    // 切换为其它slice时将定时器删除
    if (g_closeVocassistHandle != nullptr) {
        delete g_closeVocassistHandle;
        g_closeVocassistHandle = nullptr;
    }

    delete voicePhoto;
    voicePhoto = nullptr;
    workFrame->RemoveAll();
    delete wordContext;
    wordContext = nullptr;
    delete workFrame;
    workFrame = nullptr;
}

VoiceAssistance *VoiceAssistance::GetInstance()
{
    return g_pVoiceAssistance;
}

void VoiceAssistance::InitImageAnimatorSource(void)
{
    int ret;
    (void)memset_s(storPicPathBuf, sizeof(storPicPathBuf), 0, sizeof(storPicPathBuf));
    for (int i = 0; i < ANI_TOTAL_NUM; i++) {
        ret = sprintf_s(storPicPathBuf[i], sizeof(storPicPathBuf[i]),
                RES_PATH"/VOICE_ASSISTANT_%02d.bin", i + 1);
        if (ret == ERROR_RETVAL) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Splice failed!!");
            return;
        }
        imageAnimatorInfo[i].imagePath = (const char *)storPicPathBuf[i];
        imageAnimatorInfo[i].pos.x = ANI_X;
        imageAnimatorInfo[i].pos.y = ANI_Y;
        imageAnimatorInfo[i].width = ANI_SL;
        imageAnimatorInfo[i].height = ANI_SL;
        imageAnimatorInfo[i].imageType = IMG_SRC_FILE_PATH;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] VoiceAssistanceInitImageAnimatorSource success!!");
}

void VoiceAssistance::DetermineCause(const int *eventRet)        // 根据能否执行用户指令做出回复
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter DetermineCause");
    if (eventRet == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "EventRet is nullptr!!");
        return;
    }
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance instance is nullptr!!");
        return;
    }
    int temp = *eventRet;
    CreateWordLabel(g_resStr[temp], ERASE_LABEL_TEXT);
    VocassistModel::GetInstance()->SetToggleFlag(ALREADY_CLOSE_VOCASSIST_IMAGE);
}

void VoiceAssistance::InitVocassist(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] InitVocassist");
    voicePhoto = new UIImageAnimatorView();
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance : failed to Create vociePhoto!!");
        return;
    }
    voicePhoto->SetPosition(VOCASSIST_XLABEL, VOCASSIST_YLABEL, VOCASSIST_WIDTH, VOCASSIST_HEIGHTH);
    voicePhoto->SetRepeat(true);
    voicePhoto->SetVisible(true);
    voicePhoto->SetSizeFixed(true);

    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance instance is nullptr!!");
        return;
    }
    VoiceAssistance::GetInstance()->SwitchMedium(WAVE_VIEW);
    voicePhoto->SetViewId(VOCPHOTO_ID);
    voicePhoto->SetTouchable(true);
    onClickListener = static_cast<UIView::OnClickListener*>(VocassistPresenter::GetInstance());
    voicePhoto->SetOnClickListener(onClickListener);

    stoplistener = static_cast<UIImageAnimatorView::AnimatorStopListener*>(VocassistPresenter::GetInstance());
    if (stoplistener == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Failed to Create stoplistener!!");
        return;
    }
    voicePhoto->SetAnimatorStopListener(stoplistener);
}

void VoiceAssistance::SwitchMedium(uint16_t targetView)
{
    if (GraphicService::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GraphicService instance is nullptr!!");
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&VoiceAssistance::VocSwitchApp, this, targetView));
}

static viewFunct FindIndex(uint16_t target)
{
    for (int i = 0; i < SWITCH_MAP_NUM; i++) {
        if (g_switchMap[i].viewIndex == target) {
            return (g_switchMap[i].funct);
        }
    }
    return nullptr;
}

void VoiceAssistance::VocSwitchApp(uint16_t targetView)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Start Operation!!");
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance instance is nullptr!!");
        return;
    }
    viewFunct func = FindIndex(targetView);
    if (func != nullptr) {
        int retVal = (this->*(func))();
        if (SWITCH_TO_MAKECALL <= targetView && targetView <= SWITCH_TO_UNKNOWN) {
            DetermineCause(&retVal);            // 回复文本显示
        }
    }
}

int VoiceAssistance::VocResponse()
{
    return VOC_RESPONSE;
}

int VoiceAssistance::ChangeViewToPlayer()           // 播放音乐
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeViewToPlayer");

    if (!PlayersModel::GetInstance()->HasPlayerPlayed()) {       // 播放过了就不用再初始化
        PlayersListGroup::GetInstance()->SetUpListCase();
        PlayersModel::GetInstance()->PlayerInit();
    }

    int ret = PlayersModel::GetInstance()->PlayerPlay();
    return ret;
}

int VoiceAssistance::ChangePreSong()               // 上一首
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangePreSong!!");

    int ret = PlayersModel::GetInstance()->PreSongOperate();
    if (ret == NORMAL) {
        ret = PlayersModel::GetInstance()->PlayerPlay();
        if (ret != EXECUTE_FUNC_FAILED) {
            return NORMAL;
        }
    }
    return ret;
}

int VoiceAssistance::ChangeNextSong()              // 下一首
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeNextSong!!");

    int ret = PlayersModel::GetInstance()->NextSongOperate();
    if (ret == NORMAL) {
        ret = PlayersModel::GetInstance()->PlayerPlay();
        if (ret != EXECUTE_FUNC_FAILED) {
            return NORMAL;
        }
    }
    return ret;
}

int VoiceAssistance::ChangeStopPlayback()      // 停止播放
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeStopPlaybcak!!");

    int ret = PlayersModel::GetInstance()->PlayerStop();
    return ret;
}

int VoiceAssistance::ChangePausePlayback() // 暂停
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangePausePlayback!!");

    int ret = PlayersModel::GetInstance()->PlayerPause();
    return ret;
}

int VoiceAssistance::ChangeResumePlayback() // 继续播放
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeResumePlayback!!");

    int ret = PlayersModel::GetInstance()->PlayerPlay();
    return ret;
}

int VoiceAssistance::ChangeIncreaseVolume()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeResumePlayback!!");

    int ret = PlayersModel::GetInstance()->IncreaseVolume();
    return ret;
}

int VoiceAssistance::ChangeDecreaseVolume()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeResumePlayback!!");

    int ret = PlayersModel::GetInstance()->DecreaseVolume();
    return ret;
}

int VoiceAssistance::ChangeAcceptPhone()    // 接听电话
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeAcceptPhone!!");

    int phoneStatus = VocassistModel::GetInstance()->GetPhoneDetailStatus();    // 扫描设置此时通话的状态和个数
    if (phoneStatus == ONE_ROAD_AND_INCOMING) {
        int ret = AcceptIncomingCall();
        if (ret == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AcceptPhone Success!!");
            return NORMAL;
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "AcceptPhone Failed!!");
            return EXECUTE_FUNC_FAILED;
        }
    } else {
        return WITHOUT_INCOMING_PHONE;
    }
}

int VoiceAssistance::ChangeHangupPhone()    // 挂断电话
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Enter ChangeHangupPhone!!");

    int phoneStatus = VocassistModel::GetInstance()->GetPhoneDetailStatus();    // 扫描设置此时通话的状态和个数
    if (phoneStatus == ONE_ROAD_AND_INCOMING) {
        int ret = RejectIncomingCall();
        if (ret == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HangupPhone Success!!");
            return NORMAL;
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HangupPhone Failed!!");
            return EXECUTE_FUNC_FAILED;
        }
    } else {
        return WITHOUT_INCOMING_PHONE;
    }
}

std::string VoiceAssistance::GetCmdContext()
{
    return allWord;
}

void VoiceAssistance::SetCmdContext(std::string str)
{
    allWord = str;
}

int VoiceAssistance::ResponseWithoutUse()
{
    int phoneStatus = VocassistModel::GetInstance()->GetPhoneDetailStatus();    // 扫描设置此时通话的状态和个数
    if (phoneStatus == ONE_OR_MORE_ROAD_AND_OTHER_STATUS) {
        return NO_SUPPORT_VOC_IN_CALLING;
    } else {
        return WITHOUT_OPERATE;
    }
}

int VoiceAssistance::CreateCmdLabel()
{
    std::string temp = GetCmdContext();
    CreateWordLabel(temp, ERASE_LABEL_TEXT);
    return NORMAL;
}

int VoiceAssistance::VocAssistChange() // 语音助手图标
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] VocAssistChange");
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoicePhoto is nullptr, please use function InitView firstly!!");
        return EXECUTE_FUNC_FAILED;
    }
    voicePhoto->SetImageAnimatorSrc(imageAnimatorInfo+WAVEFORM_CHANGE_END,
        ACTIVE_VOCASSIST_PICTURE_TOTAL, ACTIVE_VOCASSIST_PICTURE_INTERVAL);
    voicePhoto->SetRepeat(true);
    voicePhoto->Start();
    return NORMAL;
}

int VoiceAssistance::WaveFormChange() // 波形图标
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] WaveFormChange");
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoicePhoto is nullptr, please use function InitView firstly!!");
        return EXECUTE_FUNC_FAILED;
    }
    voicePhoto->SetImageAnimatorSrc(imageAnimatorInfo+WAVEFORM_CHANGE_START,
        WAVEFORM_CHANGE_PICTURE_TOTAL, WAVEFORM_CHANGE_PICTURE_INTERVAL);
    voicePhoto->SetRepeat(true);
    voicePhoto->Start();
    return NORMAL;
}

void VoiceAssistance::InitWordlabel() // 识别出的命令文本初始化
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] InitWordlabel");
    workFrame = new UIScrollView();
    if (workFrame == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Failed to create workFrame!!");
        return;
    }
    workFrame->SetVerticalScrollState(true);
    workFrame->SetHorizontalScrollState(false);
    workFrame->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    workFrame->SetPosition(WORKFRAME_X_COORDINATE, WORKFRAME_Y_COORDINATE, WORKFRAME_WIDTH, WORKFRAME_HEIGHT);

    wordContext = new UILabel();
    if (wordContext == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Failed to Create wordContext!!");
        return;
    }
    wordContext->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_TEXT_SIZE);
    wordContext->SetPosition(LABEL_TEXT_X_COORDINATE, LABEL_TEXT_Y_COORDINATE,
                             LABEL_TEXT_WIDTH, LABEL_TEXT_HEIGHT);
    wordContext->SetAlign(TEXT_ALIGNMENT_CENTER);
    wordContext->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    workFrame->Add(wordContext);
}

bool VoiceAssistance::InitView(void *caller) // 界面初始化
{
    UNUSED(caller);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] InitView %p", caller);

    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE);

    VocassistModel::GetInstance()->SetToggleFlag(ALREADY_OPEN_VOCASSIST_IMAGE);
    VocassistModel::GetInstance()->SetRegFlag(SWITCH_TO_UNKNOWN);
    VocassistModel::GetInstance()->SetExitStatus(INTERRUPT_EXIT);
    VocassistModel::GetInstance()->SetConnectFlag(CONNECT_TO_VOCASSIST);
    InitImageAnimatorSource();
    InitVocassist();
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoicePhoto is nullptr!!");
        return false;
    }
    Add(voicePhoto);

    InitWordlabel();
    if (workFrame == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WorkFrame is nullptr!!");
        return false;
    }
    Add(workFrame);

    voicePhoto->Start();
    return true;
}

int VoiceAssistance::WithDetectVocImg() // 波形变化的过渡图形
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] WithDetectVocImg!!");
    VocassistModel::GetInstance()->SetConnectFlag(CONNECT_TO_WAVEFORM);
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoicePhoto is nullptr, please use function InitView firstly");
        return EXECUTE_FUNC_FAILED;
    }
    voicePhoto->SetImageAnimatorSrc(imageAnimatorInfo+ENTER_VOCASSIST_TRANSITION_START,
                                    ENTER_VOCASSIST_TRANSITION_TOTAL, ENTER_VOCASSIST_TRANSITION_INTERVAL);
    voicePhoto->SetRepeat(false);
    voicePhoto->Start();

    return NORMAL;
}

int VoiceAssistance::WithoutDetectVocImg()  // 结束波形变化的过渡图形
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] WithoutDetectVocImg!!");
    VocassistModel::GetInstance()->SetConnectFlag(CONNECT_TO_VOCASSIST);
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoicePhoto is nullptr, please use function InitView firstly!!");
        return EXECUTE_FUNC_FAILED;
    }
    voicePhoto->SetImageAnimatorSrc(imageAnimatorInfo+EXIT_VOCASSIST_TRANSITION_START,
                                    EXIT_VOCASSIST_TRANSITION_TOTAL, EXIT_VOCASSIST_TRANSITION_INTERVAL);
    voicePhoto->SetRepeat(false);
    voicePhoto->Start();

    return NORMAL;
}

void VoiceAssistance::CreateWordLabel(std::string Word, int isclearFlag) // 识别语音后展示文字Label
{
    Point scrollPoint;

    if (isclearFlag == ERASE_LABEL_TEXT) {
        DeleteWordLabel();
    }

    allWord  = allWord + Word;
    if (wordContext == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WorkFrame is nullptr, please use function InitView firstly!!");
        return;
    }
    wordContext->SetText(allWord.c_str()); // 在Label设置识别到的文字
    wordContext->Invalidate();

    // 保持显示得到最后一行
    if (workFrame == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WorkFrame is nullptr, please use function InitView firstly!!");
        return;
    }
    scrollPoint = {workFrame->GetRect().GetX(), workFrame->GetRect().GetY()};
    Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_POINT_BASE_X_COORDINATE),
        static_cast<int16_t>(scrollPoint.y + SCROLL_POINT_START_Y_COORDINATE)};
    Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_POINT_BASE_X_COORDINATE),
        static_cast<int16_t>(scrollPoint.y + SCROLL_POINT_END_Y_COORDINATE)};
    EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, SCROLL_POINT_INTERVAL);

    // 刷新界面
    if (MainVocassistView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateWordLabel MainVocassistView GetInstance() is nullptr!!");
        return;
    }
    if (MainVocassistView::GetInstance()->GetVocassist() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateWordLabel MainVocassistView GetVocassist() is nullptr!!");
        return;
    }
    MainVocassistView::GetInstance()->GetVocassist()->Invalidate();
    RootView::GetInstance()->Invalidate();
    return;
}

void VoiceAssistance::DeleteWordLabel()   // 删除文字
{
    allWord.clear();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] DeleteWordLabel!!");
    if (wordContext == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WorkFrame is nullptr, please use function InitView firstly!!");
        return;
    }
    wordContext->SetText(allWord.c_str());
    if (workFrame == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WorkFrame is nullptr, please use function InitView firstly!!");
        return;
    }
    workFrame->Invalidate();
    return;
}

void VoiceAssistance::CloseVocassist()
{
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CloseVocassist GetInstance() is nullptr!!");
        return;
    }

    if (VocassistModel::GetInstance()->GetToggleFlag() == ALREADY_OPEN_VOCASSIST_IMAGE) {
        VoiceAssistance::GetInstance()->SwitchMedium(WAVE_TO_VOC_VIEW);
        DeleteWordLabel();
        if (MainVocassistView::GetInstance() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CloseVocassist MainVocassistView GetInstance() is nullptr!!");
            return;
        }
        if (MainVocassistView::GetInstance()->GetVocassist() == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateWordLabel MainVocassistView GetVocassist() is nullptr!!");
            return;
        }
        // 刷新界面
        MainVocassistView::GetInstance()->GetVocassist()->Invalidate();
        RootView::GetInstance()->Invalidate();
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Somethings wrongs cause in CloseVocassist!!");
        return;
    }
}

static void DeleteTimeTick()
{
    if (g_closeVocassistHandle != nullptr) {
        delete g_closeVocassistHandle;
        g_closeVocassistHandle = nullptr;
    }
}

void VoiceAssistance::DelayTimeCallbackExtend()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceAssistView] Delay time callback extend!!");
    DeleteTimeTick();                       // 删除定时器

    // 太长没操作图标有两种形态转换，第一种是MODE1,当前为语音助手图标时，会推出语音助手；另一种是MODE2,当前为波形图标时，会转化为语音助手图标。
    if (VoiceAssistance::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VoiceAssistance::GetInstance() is nullptr");
        return;
    }

    VoiceAssistance::GetInstance()->ExitProgress(); // 退出语音助手流程

    if (VocassistModel::GetInstance()->GetRegFlag() == SWITCH_TO_VOCASSIST) {
        DeleteWordLabel();
        VoiceAssistance::GetInstance()->SwitchMedium(VOC_TO_WAVE_VIEW);
        VocassistModel::GetInstance()->SetToggleFlag(ALREADY_OPEN_VOCASSIST_IMAGE);
        VocassistModel::GetInstance()->SetHasOneCmd(false);     // 点击后可以继续喊命令
        VocassistModel::GetInstance()->SetRegFlag(SWITCH_TO_UNKNOWN);
        VocassistModel::GetInstance()->SetExitStatus(CONTINUTE_EXIT_MODE2);
        DelayDisplayTime(DELAY_SIX_SECONDS);
    } else {
        VoiceAssistance::GetInstance()->CloseVocassist();                       // 将波形图标转化为语音助手图标
    }
    return;
}

void DelayTimeCallback(void *argument)
{
    UNUSED(argument);
    if (VoiceAssistance::GetInstance() == nullptr) {
        return;
    }
    VoiceAssistance *ptr = VoiceAssistance::GetInstance();
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&VoiceAssistance::DelayTimeCallbackExtend, ptr));
}

void VoiceAssistance::ExitProgress()
{
    int quitStatus =  VocassistModel::GetInstance()->GetExitStatus();
    if (CONTINUTE_EXIT_MODE1 <= quitStatus && quitStatus <= CONTINUTE_EXIT_MODE2) {
        if (quitStatus == CONTINUTE_EXIT_MODE1) {
            int phoneStatus = VocassistModel::GetInstance()->GetPhoneDetailStatus();    // 扫描设置此时通话的状态和个数
            if (phoneStatus == ONE_ROAD_AND_INCOMING) {
                NativeAbility::GetInstance().SwitchSlice(VIEW_PHONE_MENU, PhoneMenuPages::PHONE_MENU_CALLING_PAGE);
                return;
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Too long no operate, now exit vocassist");
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
            return;
        } else if (quitStatus == CONTINUTE_EXIT_MODE2) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Too long no operate, now change model to vocassist label");
            return;
        }
    }
}

void VoiceAssistance::DelayDisplayTime(int32_t waitSec)
{
    if (waitSec < 0 || waitSec > std::numeric_limits<int32_t>::max()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WaitSec beyond range !!");
        return;
    }
    int quitStatus = VocassistModel::GetInstance()->GetExitStatus();
    // 删除定时器，打断退出语音助手的操作
    if (g_closeVocassistHandle != nullptr && (quitStatus == CONTINUTE_EXIT_MODE1 ||
        quitStatus == CONTINUTE_EXIT_MODE2)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Interrupt Timer!!");
        delete g_closeVocassistHandle;
        g_closeVocassistHandle = nullptr;
        VocassistModel::GetInstance()->SetExitStatus(INTERRUPT_EXIT);
    }

    // 创建新的定时器
    uint32 tick = GetOSTick(waitSec);
    g_closeVocassistHandle = new GraphicTimer(tick, DelayTimeCallback, nullptr, false);
    if (g_closeVocassistHandle == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "New timer failed !!");
        return;
    }
    bool retTimer = g_closeVocassistHandle->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Start timer failed !!");
        if (g_closeVocassistHandle != nullptr) {
            delete g_closeVocassistHandle;
            g_closeVocassistHandle = nullptr;
        }
        return;
    }
}
}
