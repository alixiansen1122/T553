/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "satellite_status.h"
#include "common_utils.h"

namespace OHOS {
namespace Location {
SatelliteStatus::SatelliteStatus()
{
    satellitesNumber_ = 0;
}

SatelliteStatus::SatelliteStatus(SatelliteStatus& satelliteStatus)
{
    satellitesNumber_ = satelliteStatus.GetSatellitesNumber();
    satelliteIds_ = satelliteStatus.GetSatelliteIds();
    carrierToNoiseDensitys_ = satelliteStatus.GetCarrierToNoiseDensitys();
    altitudes_ = satelliteStatus.GetAltitudes();
    azimuths_ = satelliteStatus.GetAzimuths();
    carrierFrequencies_ = satelliteStatus.GetCarrierFrequencies();
    constellationTypes_ = satelliteStatus.GetConstellationTypes();
    additionalInfoList_ = satelliteStatus.GetSatelliteAdditionalInfoList();
}
} // namespace Location
} // namespace OHOS