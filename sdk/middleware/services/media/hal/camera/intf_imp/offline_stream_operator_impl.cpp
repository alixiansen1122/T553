/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: stream operator impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "offline_stream_operator_impl.h"
#include "hw_common.h"

using namespace std;
namespace OHOS {
namespace Camera {

OfflineStreamOperatorImpl::OfflineStreamOperatorImpl(OHOS::sptr<IStreamOperatorCallback>& callback)
{
    HW_HAL_UNUSED(callback);
}

OfflineStreamOperatorImpl::~OfflineStreamOperatorImpl()
{
}

CamRetCode OfflineStreamOperatorImpl::CancelCapture(int captureId)
{
    HW_HAL_UNUSED(captureId);
    return NO_ERROR;
}

CamRetCode OfflineStreamOperatorImpl::ReleaseStreams(const std::vector<int> &streamIds)
{
    HW_HAL_UNUSED(streamIds);
    return NO_ERROR;
}

CamRetCode OfflineStreamOperatorImpl::Release()
{
    return NO_ERROR;
}

} // namespace Camera
} // namespace OHOS