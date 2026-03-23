/*
 * Copyright (c) CompanyNameMagicTag. 2024-2024. All rights reserved.
 * Description: audio recorder
 * Author: Media Software Group
 * Create: 2024-08-22
 */

#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include <stdint.h>
#include "audio_base_type.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* AudioRecorderHandle;

#define MAX_PATH_LEN 256

typedef enum {
    AUDIO_RECORD_STATUS_ERR = 0, /*!< @if Eng error status.
                                        @else   错误状态 @endif */
    AUDIO_RECORD_STATUS_IDLE = 1, /*!< @if Eng idle status.
                                        @else   就绪状态 @endif */
    AUDIO_RECORD_STATUS_INIT = 2, /*!< @if Eng init status.
                                        @else   初始化状态 @endif */
    AUDIO_RECORD_STATUS_PREPARING = 3, /*!< @if Eng Prepareing status.
                                        @else   准备中状态 @endif */
    AUDIO_RECORD_STATUS_PREPARED = 4, /*!< @if Eng Prepared status.
                                        @else   准备状态 @endif */
    AUDIO_RECORD_STATUS_START = 5, /*!< @if Eng start status.
                                        @else   记录开始状态 @endif */
    AUDIO_RECORD_STATUS_STOPPED = 6, /*!< @if Eng stop status.
                                        @else   记录停止状态 @endif */
} AudioRecorderStates;

typedef enum {
    AUDIO_RECORD_EVENT_ERROR, /*!< @if Eng error event.
                                        @else   错误事件 @endif */
    AUDIO_RECORD_STORAGE_REACHE_MAX_VALUE, /*!< @if Eng The storage reaches the maximum value.
                                        @else   录制内存达到最大值事件 @endif */
    AUDIO_RECORD_EVENT_BUTT
} AudioRecorderEvent;

typedef enum {
    AUDIO_RECORD_CURRENT_FILE_REMAIN_TIME, /*!< @if Eng Obtains the remaining recording time of the current file.
                                            @else   获取当前文件剩余录制时间 @endif */
    AUDIO_RECORD_CURRENT_STORAGE_REMAIN_TIME, /*!< @if Eng Obtains the remaining recording time of the current space.
                                            @else   获取当前空间剩余录制时间 @endif */
} RemainTimeType;

typedef struct {
    char fileName[MAX_PATH_LEN];
    uint64_t fileSize;
    uint32_t fileDateYear;
    uint32_t fileDateMonth;
    uint32_t fileDateDay;
    uint32_t fileDateHour;
    uint32_t fileDateMin;
    uint32_t fileDateSec;
    uint32_t fileDuration;
} RecordFileInfo;

/**
 * @if Eng
 * @brief recorder callback function.
 *
 * Sets the callback event reported when the record is successful or abnormal.
 *
 * @param [in] enEvent Indicates the Callback Event Type, see {@link AudioRecorderEvent}.
 * @param [in] data Indicates the Event return value.
 * @param [in] cookie Indicates the Context transferred by the callback function.
 * @return void.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 本地录音回调函数。
 *
 * 设置记录成功或异常时上报的回调事件。
 *
 * @param [in] enEvent 回调事件类型，参考{@link AudioRecorderEvent}。
 * @param [in] data 事件返回值。
 * @param [in] cookie 回调函数传入的Context。
 * @return void.
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef void (*RecorderCallback)(uint32_t enEvent, const void *data, const void *cookie);

/**
 * @if Eng
 * @brief create recorder.
 *
 * @param [out] handle Indicates the recorder instance.
 * @return Returns <b>0</b> if the setting is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 创建本地录音。
 *
 * @param [out] handle 本地录音实例。
 * @return 如果设置成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderCreate(AudioRecorderHandle *handle);

/**
 * @if Eng
 * @brief Setting the Recording Configuration Information.
 *
 * @param [in] handle Indicates the recorder instance.[in]
 * @param [in] config Indicates the Recording collection configuration information.[in]
 * @return Returns <b>0</b> if the setting is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置录音配置信息。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] config 录音采集配置信息。
 * @return 如果设置成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderSetInfo(const AudioRecorderHandle handle, const CapturerInputConfig *config);

/**
 * @if Eng
 * @brief Setting the directory for recording files.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] outPath Indicates the output directory.
 * @param [in] outPathLen Indicates the length of the output directory.
 * @return Returns <b>0</b> if the setting is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief设置录制文件的目录。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] outPath 输出目录。
 * @param [in] outPathLen 输出目录长度。
 * @return 如果设置成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderSetOutputPath(const AudioRecorderHandle handle, const char *outPath, uint32_t outPathLen);

/**
 * @if Eng
 * @brief sets whether to overwrite the earliest recording file when the recording file is full.
 *
 * sets the enable after the AudioRecorderPrepare is be called.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] enable Indicates whether to overwrite the earliest recording file cyclically.
 * @return Returns <b>0</b> Indicates that the setting is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置录音文件满时是否覆盖最早的录音文件。
 *
 * 此函数必须在{@link AudioRecorderPrepare}之后调用。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] enable 是否循环覆盖最早的录像文件。
 * @return 返回 <b>0</b>表示设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderEnableOverwriteFiles(const AudioRecorderHandle handle, bool enable);

/**
 * @if Eng
 * @brief setting the Warning Threshold for Remaining Storage.
 *
 * sets the percent after the AudioRecorderPrepare is be called.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] percent Indicates the percentage of the storage space. the value ranges from 0 to 100.
 * @return Returns <b>0</b> Indicates that the setting is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置剩余存储空间的警告阈值。
 *
 * 此函数必须在{@link AudioRecorderPrepare}之后调用。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] percent 存储空间百分比，取值范围0~100。
 * @return 返回 <b>0</b>表示设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */

errcode_t AudioRecorderSetStorageWarningThreshold(const AudioRecorderHandle handle, uint32_t percent);

/**
 * @if Eng
 * @brief setting the Maximum Duration for Splitting a File.
 *
 * This function must be called after {@link AudioRecorderPrepare}.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] duration Indicates the total duration for splitting a file.
 * @return Returns <b>0</b> Indicates that the setting is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置文件分割的最大时长。
 *
 * 此函数必须在{@link AudioRecorderPrepare}之后调用。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] duration 表示拆分文件的总时长。
 * @return 返回 <b>0</b>表示设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderSetMaxDuration(const AudioRecorderHandle handle, uint32_t duration);

/**
 * @if Eng
 * @brief setting the Maximum size for Splitting a File.
 *
 * This function must be called after {@link AudioRecorderPrepare}.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] size Indicates the total size for splitting a file.
 * @return Returns <b>0</b> Indicates that the setting is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置分割文件的最大大小。
 *
 * 此函数必须在{@link AudioRecorderPrepare}之后调用。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] size 表示分割文件的总大小。
 * @return 返回 <b>0</b>表示设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderSetMaxFileSize(const AudioRecorderHandle handle, uint64_t size);

/**
 * @if Eng
 * @brief Registers the recorder callback function.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] callback Indicates the callback function pointer.
 * @param [in] cookie Indicates the Context transferred by the callback function.
 * @return Returns <b>0</b> Indicates that the registration is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 注册本地录音回调函数。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] callback 回调函数指针。
 * @param [in] cookie 回调函数传入的Context。
 * @return 返回 <b>0</b>表示注册成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderSetCallback(const AudioRecorderHandle handle, RecorderCallback callback, const void *cookie);

/**
 * @if Eng
 * @brief Record the current timestamp position of the audio file.
 *
 * If only recording the current dot position is required, no parameters are needed;
 * simply pass in the parameters NULL and a length of 0.
 * @param [in] handle Indicates the recorder instance.
 * @param [in] buffer Indicates the input parameter.
 * @param [in] buf_len Indicates the Length of input parameter.
 * @return Returns <b>0</b> Indicates that the record is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 记录当前音频文件打点位置。
 * 如果只是记录当前的打点位置，
 * 则不需要参数，直接传入参数NULL和长度0。
 * @param [in] handle 本地录音实例。
 * @param [in] buffer 预留的可能需要的其他参数.
 * @param [in] bufLen 预留的可能需要的其他参数长度.
 *
 * @return 返回 <b>0</b>表示记录成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderWriteMarks(const AudioRecorderHandle handle, const uint8_t *buffer, const int32_t bufLen);

/**
 * @if Eng
 * @brief get remaining recording time.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [in] type Indicates the type of the obtained time. see {@link RemainTimeType}.
 * @param [out] time Indicates the time of acquisition.
 * @return Returns <b>0</b> if the playback is paused; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取剩余录制时间。
 *
 * @param [in] handle 本地录音实例。
 * @param [in] type 获取的时间类型参见{@link RemainTimeType}。
 * @param [out] time 表示获取的时间。
 * @return 如果暂停播放，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderGetRemainingTime(const AudioRecorderHandle handle, uint32_t type, uint32_t *time);

/**
 * @if Eng
 * @brief get recorded file info.
 *
 * @param [in] handle Indicates the recorder instance.
 * @param [out] info Indicates the info of the record file. see {@link RecordFileInfo}.[out]
 * @return Returns <b>0</b> if get file info correctly; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取录音文件信息
 *
 * @param [in] handle 本地录音实例。
 * @param [out] info 获取的录音文件信息{@link RecordFileInfo}。
 * @return 如果正确获取录音文件信息，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderGetFileInfo(const AudioRecorderHandle handle, RecordFileInfo *info);

/**
 * @if Eng
 * @brief prepare the recording environment and create the recording file.
 *
 * This function must be called after {@link AudioRecorderSetOutputPath} and before {@link AudioRecorderSetInfo}.
 *
 * @param [in] handle Indicates the recorder instance.
 * @return Returns <b>0</b> if the repared the recording environment and created the recording file;
 * otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 准备录音环境，创建录音文件。
 *
 * 该函数必须在{@link AudioRecorderSetOutputPath}之后调用,在{@link AudioRecorderSetInfo}之前调用。
 *
 * @param [in] handle 本地录音实例。
 * @return如果准备了录音环境并创建了录音文件，则返回<b>0</b>；
 * 否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderPrepare(const AudioRecorderHandle handle);

/**
 * @if Eng
 * @brief start recording.
 *
 * This function must be called after {@link AudioRecorderPrepare}.
 *
 * @param [in] handle Indicates the recorder instance.
 * @return Returns <b>0</b> if the record started; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开始录制。
 *
 * 此函数必须在{@link AudioRecorderPrepare}之后调用。
 *
 * @param [in] handle 本地录音实例。
 * @return如果记录已开始，则返回<b>0</b>；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderStart(const AudioRecorderHandle handle);

/**
 * @if Eng
 * @brief stop recording.
 *
 * @param [in] handle Indicates the recorder instance.
 * @return Returns <b>0</b> if the record is stopped; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止录制。
 *
 * @param [in] handle 本地录音实例。
 * @return如果记录停止，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderStop(const AudioRecorderHandle handle);

/**
 * @if Eng
 * @brief destroy recorder.
 *
 * @param [in] handle Indicates the recorder instance.
 * @return Returns <b>0</b> Indicates that the handle is deleted successfully; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 销毁本地录音。
 *
 * @param [in] handle 本地录音实例。
 * @return Returns <b>0</b>表示删除句柄成功，否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioRecorderDestroy(const AudioRecorderHandle handle);

#ifdef __cplusplus
}
#endif

#endif
