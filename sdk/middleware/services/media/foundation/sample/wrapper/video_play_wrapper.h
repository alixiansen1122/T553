/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: video player wrapper
 * Author: Media Software Group
 * Create: 2023-12-10
 */

#ifndef VIDEO_PLAY_WRAPPER_H
#define VIDEO_PLAY_WRAPPER_H

#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_lite_surface_view.h"
#include "graphic_config.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "player.h"
#include "media_thread_adapt.h"

namespace OHOS {
using namespace::Audio;
using namespace OHOS::Media;
using OHOS::Media::Player;
class MediaVideoPlay {
public:
    MediaVideoPlay(Surface *surface, UILabelButton *button1,
        std::string uri, bool isPureVideo);
    MediaVideoPlay(Surface *surface, int32_t fd, uint64_t offset, bool isPureVideo);
    ~MediaVideoPlay();
    int32_t StartVideoPlay(void);
    int32_t StopVideoPlay(void);
    int32_t PauseVideoPlay(void);
    int32_t ResumeVideoPlay(void);
    int32_t GetDumpInfo(PlayerDebugInfo *playerInfo);
    int32_t SeekVideoPlay(int64_t mSeconds);
    int32_t GetCurrentPosition(int64_t *currentPosition);
    void SetVideoPlayLoop(bool isLoop);
    void SetSyncExitMode(bool isSyncExitMode);
    bool IsSyncExitMode(void);
    bool IsExitCompletely(void);
    Surface *surface_ = nullptr;
    AudioInterrupt interrupt_;
    AudioSession sessionId_ = AUDIO_SESSION_ID_NONE;
    bool isPlaybackCompleted_ = false;
    bool isPlaybackStopped_ = false;
    bool interruptHintStop_ = false;
    shared_ptr<Player> player_ = nullptr;
    MediaMutexHandle mutex_ = nullptr;
    MediaThreadCondHandle cond_ = nullptr;
    MediaThreadIdHandle threadHandle_ = nullptr;
    UILabelButton *button_ = nullptr;
    bool isResume_ = false;
    bool isPause_ = false;
    bool isPlayed_ = false;
    bool isLoop_ = true;
    bool isEntered_ = false;
    bool isExited_ = false;
    bool playError_ = false;
    bool needSendBackgroundFrame_ = true;
    int32_t fd_ = -1;
    uint64_t offset_ = 0;
    std::string uri_ = "";
    bool isPureVideo_ = false;
    static MediaVideoPlay *play;
private:
    bool isSyncExitMode_ = true;
    void ResetVideoPlayState(void);
    static void *VideoPlayThread(void *arg);
    void DestroyVideoPlaySource(void);
};
}
#endif // MEDIA_VIDEO_PLAY_H
