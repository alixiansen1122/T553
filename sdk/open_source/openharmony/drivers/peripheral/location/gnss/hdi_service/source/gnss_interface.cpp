/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "v2_0/ignss_interface.h"
#include <unordered_map>
#include "location_vendor_interface.h"
#include "location_vendor_lib.h"
#include "gnss_interface_impl.h"
#include "location_utils.h"
#include "location_log.h"
#include "string_utils.h"

namespace OHOS {
namespace HDI {
namespace Location {
namespace Gnss {
namespace V2_0 {

IGnssInterface &IGnssInterface::GetInstance()
{
    static IGnssInterface instance;
    return instance;
}

int32_t IGnssInterface::SetGnssConfigPara(const GnssConfigPara& para)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().SetGnssConfigPara(para);
}

int32_t IGnssInterface::EnableGnss(IGnssCallback* callbackObj)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().EnableGnss(callbackObj);
}

int32_t IGnssInterface::DisableGnss()
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().DisableGnss();
}

int32_t IGnssInterface::StartGnss(GnssStartType type)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().StartGnss(type);
}

int32_t IGnssInterface::StopGnss(GnssStartType type)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().StopGnss(type);
}

int32_t IGnssInterface::SetGnssReferenceInfo(const GnssRefInfo& refInfo)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().SetGnssReferenceInfo(refInfo);
}

int32_t IGnssInterface::DeleteAuxiliaryData(unsigned short data)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);

    return GnssInterfaceImpl::GetInstance().DeleteAuxiliaryData(data);
}

int32_t IGnssInterface::SetPredictGnssData(const std::string& data)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().SetPredictGnssData(data);
}

int32_t IGnssInterface::GetCachedGnssLocationsSize(int32_t& size)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().GetCachedGnssLocationsSize(size);
}

int32_t IGnssInterface::GetCachedGnssLocations()
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().GetCachedGnssLocations();
}

int32_t IGnssInterface::SendNiUserResponse(int32_t gnssNiNotificationId, GnssNiResponseCmd userResponse)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().SendNiUserResponse(gnssNiNotificationId, userResponse);
}

int32_t IGnssInterface::SendNetworkInitiatedMsg(const std::string& msg, int length)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().SendNetworkInitiatedMsg(msg, length);
}

int32_t IGnssInterface::EnableGnssMeasurement(IGnssMeasurementCallback* callbackObj)
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().EnableGnssMeasurement(callbackObj);
}

int32_t IGnssInterface::DisableGnssMeasurement()
{
    LBSLOGI(HDI_GNSS, "%s.", __func__);
    return GnssInterfaceImpl::GetInstance().DisableGnssMeasurement();
}

} // V2_0
} // Gnss
} // Location
} // HDI
} // OHOS
