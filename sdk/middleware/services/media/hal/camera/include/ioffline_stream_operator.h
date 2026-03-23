/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: ioffine stream operator
* Author: Media Software Group
* Create: 2022-12-20
*/

/**
 * @file ioffline_stream_operator.h
 *
 * @brief Declares APIs for offline stream operations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H
#define HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H

#include "types.h"

namespace OHOS {
namespace Camera {
enum {
    CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE = 0,
    CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS,
    CMD_OFFLINE_STREAM_OPERATOR_RELEASE,
};

class IOfflineStreamOperator {
public:
    virtual ~IOfflineStreamOperator() {}

    /**
     * @brief Cancels a capture request.
     *
     * @param captureId Indicates the ID of the capture request to cancel.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined
     * in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode CancelCapture(int captureId) = 0;

    /**
     * @brief Releases offline streams.
     *
     * @param streamIds Indicates the IDs of the offline streams to release.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link
     * CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) = 0;

    /**
     * @brief Releases all offline streams.
     *
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link
     * CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode Release() = 0;
};
} // namespace Camera
} // namespace OHOS
#endif // HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H
