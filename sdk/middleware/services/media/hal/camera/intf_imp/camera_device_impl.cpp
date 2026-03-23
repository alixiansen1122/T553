/*
 * Copyright (c) 2020-2021 @CompanyNameMagicTag.
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

#include "camera_device_impl.h"
#include "stream_operator_impl.h"

using namespace std;

namespace OHOS {
namespace Camera {

CameraDeviceImpl::CameraDeviceImpl(const std::string &cameraId, std::shared_ptr<CameraDeviceBase> &deviceBase)
{
    m_cameraId = cameraId;
    m_deviceBase = deviceBase;
}

CameraDeviceImpl::~CameraDeviceImpl()
{
    HAL_LOGI("\n");
}

CamRetCode CameraDeviceImpl::GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IStreamOperator> &streamOperator)
{
    HW_HAL_UNUSED(callback);
    shared_ptr<StreamMng> streamMng = nullptr;
    m_deviceBase->GetStreamMng(streamMng);
    streamOperator = make_shared<StreamOperatorImpl>(streamMng);
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::UpdateSettings(const void *value, int size)
{
    m_deviceBase->UpdateSettings(value, size);
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::SetResultMode(const ResultCallbackMode &mode)
{
    HW_HAL_UNUSED(mode);
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::GetEnabledResults(std::vector<MetaType> &results)
{
    HW_HAL_UNUSED(results);
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::EnableResult(const std::vector<MetaType> &results)
{
    HW_HAL_UNUSED(results);
    return NO_ERROR;
}

CamRetCode CameraDeviceImpl::DisableResult(const std::vector<MetaType> &results)
{
    HW_HAL_UNUSED(results);
    return NO_ERROR;
}

void CameraDeviceImpl::Close()
{
    return;
}

} // namespace Camera
} // namespace OHOS