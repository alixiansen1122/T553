/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: istream operator callback
* Author: Media Software Group
* Create: 2022-12-20
*/

/**
 * @file istream_operator_callback.h
 *
 * @brief Declares callbacks related to {@link IStreamOperator}. The caller needs to implement these callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_STREAM_OPERATOR_CALLBACK_SERVER_H
#define HDI_STREAM_OPERATOR_CALLBACK_SERVER_H

#include "types.h"

namespace OHOS {
namespace Camera {
enum {
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_STARTED = 0,
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ENDED,
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ERROR,
    CMD_STREAM_OPERATOR_CALLBACK_ON_FRAME_SHUTTER,
};

class IStreamOperatorCallback {
public:
    virtual ~IStreamOperatorCallback() {}

public:
    /**
     * @brief Called when the capture starts.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param streamIds Indicates the IDs of the streams corresponding to the callback.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamIds) = 0;

    /**
     * @brief Called when the capture ends.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param infos Indicates information related to the capture when it ends.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureEnded(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureEndedInfo>> &infos) = 0;

    /**
     * @brief Called when an error occurs during the capture.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param infos Indicates a list of capture error messages.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnCaptureError(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureErrorInfo>> &infos) = 0;

    /**
     * @brief Called when a frame is captured.
     *
     * This callback is enabled by using <b>enableShutterCallback_</b> in the {@link CaptureInfo} parameter
     * of {@link Capture}. When <b>enableShutterCallback_</b> is set to <b>true</b>, this callback is triggered
     * each time a frame is captured.
     *
     * @param captureId Indicates the ID of the capture request corresponding to the callback.
     * @param streamIds Indicates the IDs of the streams corresponding to the callback.
     * @param timestamp Indicates the timestamp when the callback is invoked.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void OnFrameShutter(int32_t captureId,
        const std::vector<int32_t> &streamIds, uint64_t timestamp) = 0;
};
} // namespace Camera
} // namespace OHOS
#endif // HDI_STREAM_OPERATOR_CALLBACK_SERVER_H
