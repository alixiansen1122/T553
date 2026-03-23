/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersPresenter
 * Created: 2025-06-05
 */
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "wearable_log.h"
#include "common/image_cache_manager.h"
#include "UiConfig.h"
#include "ui_resource_image.h"
#include "player/PlayersModel.h"
#include "graphic_service.h"
#include "phoneservice/PhoneService.h"
#include "main/MainViewSample.h"
#include "phonemenu/PhoneMenuView.h"
#include "bts_avrcp_controller.h"
#include "bts_avrcp_target.h"
#include "player/PlayersPresenter.h"

namespace OHOS {
static PlayersPresenter *g_pPlayersPresenter = nullptr;

PlayersPresenter *PlayersPresenter::GetInstance(void)
{
    return g_pPlayersPresenter;
}

PlayersPresenter::PlayersPresenter()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersPresenter");
    g_pPlayersPresenter = this;
}

PlayersPresenter::~PlayersPresenter()
{
    g_pPlayersPresenter = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~PlayersPresenter");
}

PlayersListGroup::~PlayersListGroup()
{
    if (playersCaseList_ != nullptr) {
        playersCaseList_->Clear();
    }
    delete playersCaseList_;
    playersCaseList_ = nullptr;
}

void PlayersListGroup::AddCase(PlayerCaseInfo testCaseInfo)
{
    if (playersCaseList_ == nullptr) {
        return;
    }
    playersCaseList_->PushBack(testCaseInfo);
}

void PlayersListGroup::SetUpListCase()
{
    int16_t ret;
    if (playersCaseList_ == nullptr) {
        playersCaseList_ = new List<PlayerCaseInfo>();
        if (playersCaseList_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PlayersListGroup playersCaseList_ new fail");
            return;
        }
    }
    playersCaseList_->Clear();
    ret = GetAllFiles(playersCaseList_);
    if (ret == OHOS_FAILURE) {
        return;
    }
}

List<PlayerCaseInfo>& PlayersListGroup::GetCase()
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "playersCaseList_.Size(%d)", playersCaseList_->Size());
    return *playersCaseList_;
}

char *PlayersListGroup::GetCase(int16_t index)
{
    if (index < 0 || playersCaseList_ == nullptr) {
        return nullptr;
    }
    ListNode<PlayerCaseInfo> *node = playersCaseList_->Begin();
    while (node != playersCaseList_->End()) {
        if (index == node->data_.index) {
            return node->data_.sliceId;
        }
        node = node->next_;
    }
    return nullptr;
}

// 读取文件夹下文件名信息
int PlayersListGroup::GetAllFiles(List<PlayerCaseInfo> *playersCaseList)
{
    int ret;
    int i{0};
    struct dirent *direntp;
    struct PlayerCaseInfo playerCaseInfo;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView::GetAllFiles");
    // 打开目录
    DIR *dirp = opendir(APP_MUSIC_PATH);
    // 遍历文件
    if (dirp != nullptr) {
        while ((direntp = readdir(dirp)) != nullptr) {
            ret = memcpy_s(playerCaseInfo.sliceId, DEC_NUM_256, direntp->d_name, DEC_NUM_256);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "memcpy_s err %d.", ret);
                closedir(dirp);
                return OHOS_FAILURE;
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "song Name %s.", playerCaseInfo.sliceId);
            playerCaseInfo.index = i;
            i++;
            playersCaseList->PushBack(playerCaseInfo);
        }
    } else {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView::opendir err!");
        return OHOS_FAILURE;
    }
    // 关闭目录
    closedir(dirp);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlayersView::GetAllFiles end!");
    return OHOS_SUCCESS;
}


// 播放器回调
PlayerCallbackImpl::PlayerCallbackImpl()
{
}

PlayerCallbackImpl::~PlayerCallbackImpl()
{
}

void PlayerCallbackImpl::OnPlaybackComplete()
{
    // 此处实现代码用于处理文件播放完成的事件
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnPlaybackCompleten");
    GraphicService::GetInstance()->PostGraphicEvent([] {
        PlayersView *playersView = PlayersView::GetInstance();
        if (playersView != nullptr) {
            playersView->SetPlaybackProgress(100); // 100:进度100%
        }
    });
    int sleepTime = 20000;
    usleep(sleepTime);
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&PlayersModel::PlaySwitching, PlayersModel::GetInstance()));
}

void PlayerCallbackImpl::OnError(int32_t errorType, int32_t errorCode)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnError test, errorType %d, errorCode %d", errorType, errorCode);
    // 此处实现代码处理错误事件
}

void PlayerCallbackImpl::OnInfo(int type, int extra)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnInfo test, type %d, extra %d.", type, extra);
    // 此处实现代码处理普通事件
}

void PlayerCallbackImpl::OnRewindToComplete()
{
    // 此处实现代码处理进度控制完成的事件
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnRewindToComplete test");
}

// 音频中断
PlayerInterruptListener::PlayerInterruptListener()
{
}

PlayerInterruptListener::~PlayerInterruptListener()
{
}

void PlayerInterruptListener::SetOwner(std::shared_ptr<Player> player)
{
    player_ = player;
}

std::shared_ptr<Player> PlayerInterruptListener::GetPlayer()
{
    return player_;
}

void PlayerInterruptListener::RestFlag()
{
    isPaused_ = false;
    isDelayed_ = false;
    isResumed_ = false;
    isBeginStopped_ = false;
    isEndStopped_ = false;
}

void PlayerInterruptListener::SetPaused(bool value)
{
    isPaused_ = value;
}

void PlayerInterruptListener::SetResumed(bool value)
{
    isResumed_ = value;
}

bool PlayerInterruptListener::IsPaused()
{
    return isPaused_;
}

bool PlayerInterruptListener::IsInterruptedPaused()
{
    return isInterruptedPause_;
}

bool PlayerInterruptListener::IsDelayed()
{
    return isDelayed_;
}

bool PlayerInterruptListener::IsResumed()
{
    return isResumed_;
}

bool PlayerInterruptListener::IsBeginStopped()
{
    return isBeginStopped_;
}

bool PlayerInterruptListener::IsEndStopped()
{
    return isEndStopped_;
}

static void PlayerPostGraphicEvent(void (*fun)(void))
{
    GraphicService::GetInstance()->PostGraphicEvent(*fun);
}

static void Pause(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::PausePlayer] start");
    int32_t state;
    int32_t ret;
    std::shared_ptr<PlayerInterruptListener> PlayerIntrptListener =
        OHOS::PlayersModel::GetInstance()->GetPlayerIntrptListener();
    if (PlayerIntrptListener == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener] Pause PlayerIntrptListener is null");
        return;
    }

    std::shared_ptr<Player> player = PlayerIntrptListener->GetPlayer();
    if (player.get() == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::PausePlayer] player is nullptr");
        return;
    }

    player->GetPlayerState(state);
    if (state == PLAYER_STARTED) {
        ret = player->Pause();
        if (ret == 0) {
            PlayerIntrptListener->RestFlag();
            PlayerIntrptListener->SetPaused(true);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::PausePlayer]");
        }
    }
}

void PlayerInterruptListener::PausePlayer(void)
{
    Pause();
}

static void Resume(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::Resume] start");
    int32_t state;
    int32_t ret;
    std::shared_ptr<PlayerInterruptListener> PlayerIntrptListener =
        OHOS::PlayersModel::GetInstance()->GetPlayerIntrptListener();
    if (PlayerIntrptListener == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener] Pause PlayerIntrptListener is null");
        return;
    }

    std::shared_ptr<Player> player = PlayerIntrptListener->GetPlayer();
    if (player.get() == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::ResumePlayer] player is nullptr");
        return;
    }

    player->GetPlayerState(state);
    if (state == PLAYER_PAUSED && PlayerIntrptListener->IsPaused()) {
        ret = player->Play();
        if (ret == 0) {
            PlayerIntrptListener->RestFlag();
            PlayerIntrptListener->SetResumed(true);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::ResumePlayer]");
        }
    }
}

void PlayerInterruptListener::ResumePlayer(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::ResumePlayer] start");
    int sleepTime = 500000;
    usleep(sleepTime); // 确保界面切换完成
    if (NativeAbility::GetInstance().GetCurTargetId() ==
        (VIEW_PHONE_MENU | static_cast<uint32_t>(PhoneMenuPages::PHONE_MENU_CALLING_PAGE) << PAGE_OFFSET) ||
        (GetCurrCallState() != HFP_HF_CALL_STATE_FINISHED && GetScoConnectState() == HFP_SCO_STATE_CONNECTED)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::ResumePlayer] this is in calling");
        return; // 通话界面，禁止音乐恢复播放
    }
    Resume();
}

static void Stop(void)
{
    PlayersModel::GetInstance()->PlayerStop();
    PlayersModel::GetInstance()->ClearActivateAudioInterrupt();
}

void PlayerInterruptListener::StopPlayer(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::StopPlayer] start");
    PlayerPostGraphicEvent(Stop);
}

void PlayerInterruptListener::OnInterrupt(int32_t type, int32_t hint)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[PlayerInterruptListener::] OnInterrupt go in, type %d, hint %d.", type, hint);

    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
        isInterruptedPause_ = true;
        PausePlayer();
    }

    if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
        isInterruptedPause_ = false;
        ResumePlayer();
    }

    if (hint == INTERRUPT_HINT_STOP) {
        RestFlag();
        if (type == INTERRUPT_TYPE_BEGIN) {
            isBeginStopped_ = true;
            StopPlayer();
        } else if (type == INTERRUPT_TYPE_END) {
            isEndStopped_ = true;
        }
    }
}
} // namespace OHOS
