/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup MultiMedia_Player
 * @{
 *
 * @brief Defines the <b>Player</b> class and provides functions related to media playback.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file player.h
 *
 * @brief Declares the <b>Player</b> class, which is used to implement player-related operations.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "source.h"
#include "format.h"
#include "player_debug_info.h"
#include <memory>
#include <vector>

#if defined(ENABLE_UIKIT)
#include "surface.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#endif

namespace OHOS {
namespace Media {
/**
 * @brief Enumerates player seek modes. You can move the current playback position of the media to a given time
 * position using the specified mode.
 *
 * @since 1.0
 * @version 1.0
 */
enum PlayerSeekMode : int32_t {
    /** Moves the media position to the latest synchronization frame located before the given time position. */
    PLAYER_SEEK_PREVIOUS_SYNC = 0,
    /** Moves the media position to the latest synchronization frame located after the given time position. */
    PLAYER_SEEK_NEXT_SYNC,
    /** Moves the media position to the latest synchronization frame located before or after the given time position. */
    PLAYER_SEEK_CLOSEST_SYNC,
    /** Moves the media position to the latest frame located before or after the given time position. */
    PLAYER_SEEK_CLOSEST,
};

/**
 * @brief Enumerates player states.
 *
 * @since 1.0
 * @version 1.0
 */
enum PlayerStates : uint32_t {
    /** Error */
    PLAYER_STATE_ERROR = 0,
    /** Idle */
    PLAYER_IDLE = 1 << 0,
    /** Initialized */
    PLAYER_INITIALIZED = 1 << 1,
    /** Preparing */
    PLAYER_PREPARING = 1 << 2,
    /** Prepared */
    PLAYER_PREPARED = 1 << 3,
    /** Playback started */
    PLAYER_STARTED = 1 << 4,
    /** Playback paused */
    PLAYER_PAUSED = 1 << 5,
    /** Playback stopped */
    PLAYER_STOPPED = 1 << 6,
    /** Playback completed */
    PLAYER_PLAYBACK_COMPLETE = 1 << 7
};

/**
 * @brief Player debug info.
 *
 * @since 1.0
 * @version 1.0
 */
struct PlayerDebugInfo {
    /* Information about the file to be played */
    PlayerFileDebugInfo fileDebugInfo;
    /* video stream information */
    PlayerVideoDebugInfo videoDebugInfo;
    /* audio stream information */
    PlayerAudioDebugInfo audioDebugInfo;
    /* playback control information */
    PlayerControlDebugInfo playControlInfo;
};

/**
 * @brief Provides listeners for events and exception notifications that occur during media playback.
 *
 * @since 1.0
 * @version 1.0
 */
class PlayerCallback {
public:
    enum PlayerInfoType : int32_t {
        PLAYER_INFO_PLAY_START = 0,
        PLAYER_INFO_PLAYING,
        PLAYER_INFO_PLAY_END,
        PLAYER_INFO_SEEK_START,
        PLAYER_INFO_SEEK_END,
        PLAYER_INFO_STOPPED,
        PLAYER_INFO_PAUSED,
        PLAYER_INFO_BUTT,
    };

    enum PlayerErrorType : int32_t {
        PLAYER_ERROR_UNKNOWN = 0,
    };

    enum PlayerErrorCode : int32_t {
        PLAYER_ERROR_CODE_VID_PLAY_FAIL = 0, /* The video fails to be played. */
        PLAYER_ERROR_CODE_AUD_PLAY_FAIL,       /* The audio fails to be played. */
        PLAYER_ERROR_CODE_DEMUX_FAIL,          /* The file fails to be played. */
        PLAYER_ERROR_CODE_TIMEOUT,             /* Operation timeout. For example, reading data timeout. */
        PLAYER_ERROR_CODE_NOT_SUPPORT,         /* The file format is not supportted. */
        PLAYER_ERROR_CODE_ILLEGAL_STATEACTION, /* illegal action at cur state. */
        PLAYER_ERROR_CODE_UNKNOWN,              /* Unknown error. */
        PLAYER_ERROR_CODE_BUTT,
    };

    PlayerCallback() = default;
    virtual ~PlayerCallback() {}

    /**
    * @brief Called when the playback is complete.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual void OnPlaybackComplete() = 0;

    /**
    * @brief Called when a playback error occurs.
    *
    * @param errorType Indicates the error type. For details, see {@link PlayerErrorType}.
    * @param errorCode Indicates the error code.
    * @since 1.0
    * @version 1.0
    */
    virtual void OnError(int32_t errorType, int32_t errorCode) = 0;

    /**
    * @brief Called when playback information is received.
    *
    * @param type Indicates the information type. For details, see {@link PlayerInfoType}.
    * @param extra Indicates the information code.
    * @since 1.0
    * @version 1.0
    */
    virtual void OnInfo(int32_t type, int32_t extra) = 0;

    /**
    * @brief Called when the rewind is complete.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual void OnRewindToComplete() = 0;
    };

    /**
    * @brief Provides functions for playing online movies, offline movies, and streams, for example, playing local
    * movies and advanced audio coding (AAC) streams.
    *
    * @since 1.0
    * @version 1.0
    */
class Player {
public:
    Player();
    ~Player();

    /**
     * @brief Sets the playback source for the player. The corresponding source can be the file descriptor (FD) of the
     * local file, local file URI, network URI, or media stream.
     *
     * @param source Indicates the playback source. Currently, only local file URIs and media streams are supported.
     * For details, see {@link Source}.
     * @return Returns <b>0</b> if the setting is successful; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t SetSource(const Source &source);

    /**
     * @brief Prepares the playback environment and buffers media data.
     *
     * This function must be called after {@link SetSource}.
     *
     * @return Returns <b>0</b> if the playback environment is prepared and media data is buffered;
     * returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Prepare();

    /**
     * @brief Starts or resumes playback.
     *
     * This function must be called after {@link Prepare}. If the player state is <b>Prepared</b>, this function is
     * called to start playback. If the player state is <b>Playback paused</b>, this function is called to resume
     * playback. If the media is playing in an abnormal speed, this function is called to restore the playback speed
     * to normal.
     *
     * @return Returns <b>0</b> if the playback starts or resumes; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Play();

    /**
     * @brief Checks whether the player is playing.
     *
     * @return Returns <b>true</b> if the player is playing; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsPlaying();

    /**
     * @brief Pauses playback.
     *
     * @return Returns <b>0</b> if the playback is paused; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Pause();

    /**
     * @brief Stops playback.
     *
     * @return Returns <b>0</b> if the playback is stopped; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Stop();

    /**
     * @brief Changes the playback position.
     *
     * This function can be used during playback or pause.
     *
     * @param mSeconds Indicates the target playback position, accurate to second.
     * @param mode Indicates the player seek mode. For details, see {@link PlayerSeekMode}.
     * @return Returns <b>0</b> if the playback position is changed; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
    */
    int32_t Rewind(int64_t mSeconds, int32_t mode);

    /**
     * @brief Sets the volume of the player.
     *
     * This function can be used during playback or pause. The value <b>0</b> indicates no sound, and <b>100</b>
     * indicates the original volume.
     *
     * @param leftVolume Indicates the target volume of the left audio channel to set, ranging from 0 to 300.
     * @param rightVolume Indicates the target volume of the right audio channel to set, ranging from 0 to 300.
     * @return Returns <b>0</b> if the setting is successful; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t SetVolume(float leftVolume, float rightVolume);

    /**
     * @brief Sets a surface for video playback.
     *
     * @param surface Indicates the surface to set. For details, see {@link Surface}.
     * @return Returns <b>0</b> if the setting is successful; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
#if defined(ENABLE_UIKIT)
    int32_t SetVideoSurface(Surface *surface);
#elif defined(ENABLE_LVGL)
    int32_t SetVideoSurface(LvSurfaceView *surface);
#endif

    /**
     * @brief Sets loop playback.
     *
     * @param loop Specifies whether to enable loop playback. The value <b>true</b> means to enable loop playback,
     * and <b>false</b> means to disable loop playback.
     * @return Returns <b>0</b> if the setting is successful; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t EnableSingleLooping(bool loop);

    /**
     * @brief Checks whether the player is looping.
     *
     * @return Returns <b>true</b> if the player is looping; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsSingleLooping();

    /**
     * @brief Obtains the player state.
     *
     * @param state Indicates the player state. For details, see {@link PlayerStates}.
     * @return Returns <b>0</b> if the player state is obtained; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t GetPlayerState(int32_t &state) const;

    /**
     * @brief Obtains the playback position, accurate to millisecond.
     *
     * @param time Indicates the playback position.
     * @return Returns <b>0</b> if the playback position is obtained; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
#if defined(_WIN32)
#undef GetCurrentTime
#endif
    int32_t GetCurrentTime(int64_t &time) const;


    /**
     * @brief Obtains the total duration of media files, in milliseconds.
     *
     * @param duration Indicates the total duration of media files.
     * @return Returns <b>0</b> if the total duration is obtained; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t GetDuration(int64_t &durationMs) const;

    /**
     * Sets the playback speed.
     *
     * @param speed Indicates the playback speed to set, which support {-128/-64/-32/-16/-8/-4/-2/1/2/4/8/16/32/64/128}
     * @return Returns {@code 0} if the playback speed is set; returns {@code -1} otherwise.
     */
    int32_t SetPlaybackSpeed(float speed);
    /**
     * Obtains the playback speed.
     *
     * @param speed Indicates the playback speed.
     * @return Returns {@code 0} if the playback speed is set; returns {@code -1} otherwise.
     */
    int32_t GetPlaybackSpeed(float &speed);
    /**
     * Sets the audio type.
     *
     * @param type Indicates the audio type.
     * @return Returns {@code 0} if the setting is successful; returns {@code -1} otherwise.
     */
    int32_t SetAudioStreamType(int32_t type);
    /**
     * Obtains the audio type.
     *
     * @param type Indicates the audio type.
     * @return Returns {@code 0} if the playback speed is set; returns {@code -1} otherwise.
     */
    int32_t GetAudioStreamType(int32_t &type);
    /**
     * @brief Restores the player to the initial state.
     *
     * @return Returns <b>0</b> if the player is restored; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Reset();

    /**
     * @brief Releases player resources.
     *
     * @return Returns <b>0</b> if player resources are released; returns <b>-1</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t Release();

    /**
     * @brief Registers a listener to receive events and exception notifications from the player.
     *
     * @param cb Indicates the listener to register. For details, see {@link PlayerCallback}.
     * @since 1.0
     * @version 1.0
     */
    void SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb);
    /**
     * set parameter through format, extended interface.
     *
     * @param params Indicates the extended-informations. Format see {@link Format}
     * @return Returns {@code 0} if set parameter successfully; returns {@code -1} otherwise.
     * @note not support on current version
     */
    int32_t SetParameter(const Format &params);
    /**
     * get parameter through format, extended interface.
     *
     * @param params Indicates the extended-informations. Format see {@link Format}
     * @return Returns {@code 0} if set parameter successfully; returns {@code -1} otherwise.
     */
    int32_t GetParameter(Format &params);
    /**
     * get the total number of the tracks.
     *
     * @param totalTracks Indicates the total number of the tracks.
     * @return Returns {@code 0} if set parameter successfully; returns {@code -1} otherwise.
     */
    int32_t GetTotalTracks(int32_t &totalTracks);
    /**
     * set audio sessionid.
     *
     * @param sessionId indicates sessionid to be set.
     * @return Returns {@code 0} if set parameter successfully; returns {@code -1} otherwise.
     */
    int32_t SetAudioSessionId(uint32_t sessionId);
    /**
     * get current audio sessionid.
     *
     * @param indicates sessionid.
     * @return Returns {@code 0} if get parameter successfully; returns {@code -1} otherwise.
     */
    int32_t GetAudioSessionId(uint32_t &sessionId);
    /**
     * set output device.
     *
     * @param deviceId indicates device to be set.
     * @return Returns {@code 0} if set parameter successfully; returns {@code -1} otherwise.
     */
    int32_t SetOutputDevice(int32_t deviceId);
    /**
     * get current route device.
     *
     * @param deviceId indicates the output device.
     * @return Returns {@code 0} if get parameter successfully; returns {@code -1} otherwise.
     */
    int32_t GetRoutedDeviceId(int32_t &deviceId);

    int32_t DumpInfo(PlayerDebugInfo *playerInfo);
private:
    class PlayerImpl;
    std::unique_ptr<PlayerImpl> impl_;
};
}  // namespace Media
}  // namespace OHOS
#endif  // PLAYER_H
