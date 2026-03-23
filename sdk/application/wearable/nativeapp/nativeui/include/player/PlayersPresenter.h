/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersPresenter
 * Create: 2025-06-29
 */

#ifndef PLAYERS_PRESENTER_H
#define PLAYERS_PRESENTER_H
#include "gfx_utils/list.h"
#include "ohos_types.h"
#include "components/ui_view.h"
#include "player.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "main/VolumeSettingView.h"
#include "main/VolumeSettingPresenter.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "settings/model/SettingBluetoothModel.h"

namespace OHOS {
using OHOS::Media::Player;
using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;
class VolumeSettingView;
#define DEC_NUM_256 256
static constexpr uint8_t FONT_THIRTY_SIZE = 30;
static constexpr uint8_t FONT_FORTY_SIZE = 40;

struct PlayerCaseInfo {
    char sliceId[DEC_NUM_256];
    int16_t index;
};

class PlayersListGroup {
public:
    PlayersListGroup() {}
    ~PlayersListGroup();
    static PlayersListGroup *GetInstance()
    {
        static PlayersListGroup instance;
        return &instance;
    }
    void SetUpListCase();
    List<PlayerCaseInfo>& GetCase();
    char *GetCase(int16_t index);
    void AddCase(PlayerCaseInfo testCaseInfo);

private:
    List<PlayerCaseInfo> *playersCaseList_{nullptr};
    int GetAllFiles(List<PlayerCaseInfo> *playersCaseList);
};

// 播放器回调
class PlayerCallbackImpl : public OHOS::Media::PlayerCallback {
public:
    PlayerCallbackImpl();
    ~PlayerCallbackImpl();
    void OnPlaybackComplete();
    void OnError(int32_t errorType, int32_t errorCode);
    void OnInfo(int type, int extra);
    void OnRewindToComplete();
private:
};

// 播放器
class PlayerInterruptListener : public InterruptListener {
public:
    PlayerInterruptListener();
    ~PlayerInterruptListener();

    void SetOwner(std::shared_ptr<Player> player);
    void SetPaused(bool value);
    void SetResumed(bool value);
    void RestFlag();
    bool IsPaused();
    bool IsDelayed();
    bool IsResumed();
    bool IsBeginStopped();
    bool IsEndStopped();
    bool IsInterruptedPaused();
    void OnInterrupt(int32_t type, int32_t hint) override;
    void PausePlayer(void);
    void ResumePlayer(void);
    void StopPlayer(void);
    std::shared_ptr<Player> GetPlayer();

private:
    std::shared_ptr<Player> player_{nullptr};
    bool isPaused_{false};
    bool isDelayed_{false};
    bool isResumed_{false};
    bool isBeginStopped_{false};
    bool isEndStopped_{false};
    bool isInterruptedPause_{false};
};

class PlayersPresenter : public UIView::OnClickListener {
public:
    PlayersPresenter();
    ~PlayersPresenter();
    static PlayersPresenter *GetInstance();
private:
};
} // namespace OHOS
#endif
