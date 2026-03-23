/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: stream operator header
 * Author: media_develop team
 * Create: 20211117
 */

#ifndef OFFLINE_STREAM_OPERATOR_IMPL
#define OFFLINE_STREAM_OPERATOR_IMPL

#include "ioffline_stream_operator.h"
#include "istream_operator_callback.h"

namespace OHOS {
namespace Camera {
class OfflineStreamOperatorImpl : public IOfflineStreamOperator {
public:
    CamRetCode CancelCapture(int captureId) override;
    CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    CamRetCode Release() override;
public:
    explicit OfflineStreamOperatorImpl(OHOS::sptr<IStreamOperatorCallback>& callback);
    ~OfflineStreamOperatorImpl() override;
};
} // namespace Camera
} // namespace OHOS

#endif
