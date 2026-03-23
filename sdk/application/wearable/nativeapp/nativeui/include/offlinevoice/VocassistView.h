/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */

#ifndef VOICE_ASSISTANCE_H
#define VOICE_ASSISTANCE_H

#include <string>
#include "components/ui_image_animator.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "UiConfig.h"
#include "dfx/event_injector.h"
#include "wearable_log.h"
#include "main/AppGroupView.h"

static constexpr const char VOCPHOTO_ID[]  = "VOCPIC";
static constexpr const char VOC_ID[] = "VOC";

static constexpr uint16_t WORKFRAME_X_COORDINATE = 24;
static constexpr uint16_t WORKFRAME_Y_COORDINATE = 140;
static constexpr uint16_t WORKFRAME_WIDTH = 400;
static constexpr uint16_t WORKFRAME_HEIGHT = 250;

static constexpr uint16_t LABEL_TEXT_SIZE = 35;
static constexpr uint16_t LABEL_TEXT_X_COORDINATE = 80;
static constexpr uint16_t LABEL_TEXT_Y_COORDINATE = 0;
static constexpr uint16_t LABEL_TEXT_WIDTH = 260;
static constexpr uint16_t LABEL_TEXT_HEIGHT = 140;

static constexpr uint16_t ENTER_VOCASSIST_TRANSITION_START = 1;
static constexpr uint16_t ENTER_VOCASSIST_TRANSITION_TOTAL = 9;
static constexpr uint16_t ENTER_VOCASSIST_TRANSITION_INTERVAL = 80;

static constexpr uint16_t EXIT_VOCASSIST_TRANSITION_START = 44;
static constexpr uint16_t EXIT_VOCASSIST_TRANSITION_TOTAL = 8;
static constexpr uint16_t EXIT_VOCASSIST_TRANSITION_INTERVAL = 80;

static constexpr uint16_t SCROLL_POINT_BASE_X_COORDINATE = 50;
static constexpr uint16_t SCROLL_POINT_START_Y_COORDINATE = 200;
static constexpr uint16_t SCROLL_POINT_END_Y_COORDINATE = 0;
static constexpr uint16_t SCROLL_POINT_INTERVAL = 20;

static constexpr uint64_t DELAY_TWO_SECONDS = 2000;
static constexpr uint64_t DELAY_FOUR_SECONDS = 4000;
static constexpr uint64_t DELAY_SIX_SECONDS = 6000;

constexpr int ANI_TOTAL_NUM = 71;
static constexpr int PATH_LENGTH = 60;

namespace OHOS {
    typedef enum {
        ALREADY_OPEN_VOCASSIST_IMAGE = 1,
        ALREADY_CLOSE_VOCASSIST_IMAGE
    } VocAssistStatus;

    typedef enum {
        CONNECT_TO_WAVEFORM = 1,
        CONNECT_TO_VOCASSIST
    } ConnectStatus;

    typedef enum {
        ERASE_LABEL_TEXT = 1,
        NERASE_LABEL_TEXT
    } LabelEraseChoice;

    enum SwitchApp {
        SWITCH_TO_MAKECALL,
        SWITCH_TO_CUTCALL,
        PRE_SONG,
        NEXT_SONG,
        INCREASE_VOLUME,
        DECREASE_VOLUME,
        SWITCH_TO_PLAYER,
        PAUSE_PLAYBACK,
        SWITCH_TO_UNKNOWN,
        SWITCH_TO_VOCASSIST,
    };

    enum SwitchWordOrGraph {
        CREATE_CMD_LABEL = SWITCH_TO_VOCASSIST + 1,
        VOC_VIEW,
        WAVE_VIEW,
        VOC_TO_WAVE_VIEW,
        WAVE_TO_VOC_VIEW
    };

    enum ExitVaStatus {
        INTERRUPT_EXIT,
        CONTINUTE_EXIT_MODE1,   // 退出语音助手标志
        CONTINUTE_EXIT_MODE2,   // 退出波形图标标志
        CONTINUTE_EXIT_MODE3    // 退出到来电界面标志
    };

    enum CurInterface {
        AT_VOCASSIST,
        AT_PLAYER,
        AT_MAX
    };

    class VoiceAssistance : public AppGroupView {
    public:
        VoiceAssistance();
        ~VoiceAssistance() override;
        static VoiceAssistance *GetInstance();
        bool InitView(void *caller) override;               // 初始化所有控件

        int WithDetectVocImg();                            // 语音助手图标转化到波形变换
        int WithoutDetectVocImg();                         // 波形变换转化到语音助手图标
        int WaveFormChange();                          // 波形持续变化
        int VocAssistChange();                         // 语音助手图标持续变化
        void SwitchMedium(uint16_t targetView);
        void VocSwitchApp(uint16_t targetView);
        int VocResponse();              // 小艺回复我在
        int ChangeViewToPlayer();       // 切换到音乐播放界面
        int ChangePreSong();            // 切换到上一首
        int ChangeNextSong();           // 切换到下一首
        int ChangeStopPlayback();      // 停止播放
        int ChangePausePlayback();      // 切换到暂停
        int ChangeResumePlayback();     // 切换到播放
        int ChangeIncreaseVolume();     // 增加音量
        int ChangeDecreaseVolume();     // 减少音量
        int ChangeAcceptPhone();        // 接听电话
        int ChangeHangupPhone();        // 挂断电话
        int CreateCmdLabel();           // 展示命令词
        int ResponseWithoutUse();        // 打开语音助手没有操作
        void DetermineCause(const int *eventRet);                 // 根据场景回复命令词

        void DelayDisplayTime(int32_t waitSec);                      // 延迟展示一段时间
        void SetCmdContext(std::string str);                         // 设置要展示的文字
        void DelayTimeCallbackExtend();
        void DeleteWordLabel();                             // 删除识别到的文字
    private:
        void InitVocassist(void);                           // 初始化语音图标控件
        void InitWordlabel(void);                               // 初始化文本控件
        void InitImageAnimatorSource(void);                     // 初始化图片文件
        void CreateWordLabel(std::string Word, int isclearFlag);         // 显示识别到的文字
        
        void ExitProgress();                                // 退出语音程序
        void CloseVocassist();                           // 波形切换为语音助手图标并清除文字
        std::string GetCmdContext();
        UILabel *wordContext{nullptr};
        UIScrollView *workFrame{nullptr};
        UIImageAnimatorView::AnimatorStopListener* stoplistener{nullptr};
        UIView::OnClickListener* onClickListener{nullptr};
        UIImageAnimatorView *voicePhoto{nullptr};
        char storPicPathBuf[ANI_TOTAL_NUM][PATH_LENGTH];
        ImageAnimatorInfo imageAnimatorInfo[ANI_TOTAL_NUM];
        std::string allWord;                    // 记录显示文字
    };
}
#endif