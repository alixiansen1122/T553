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

#include "locator.h"
#include "locator_impl.h"
#include "location_log.h"

namespace OHOS {
namespace Location {

Locator &Locator::GetInstance()
{
    static Locator instance;
    return instance;
}

Locator::Locator()
{
    LocatorImpl::GetInstance().Init();
    LBSLOGD(LOCATOR_STANDARD, "ctor()");
}

Locator::~Locator()
{
    LocatorImpl::GetInstance().DeInit();
    LBSLOGD(LOCATOR_STANDARD, "dctor()");
}

LocationErrCode Locator::IsLocationEnabled(bool &isEnabled)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().IsLocationEnabled(isEnabled);
}

LocationErrCode Locator::EnableAbility(bool enable)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().EnableAbility(enable);
}

LocationErrCode Locator::StartLocating(const RequestConfig *requestConfig, ILocatorCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().StartLocating(requestConfig, callback);
}

LocationErrCode Locator::StopLocating(ILocatorCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().StopLocating(callback);
}

LocationErrCode Locator::RegisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().RegisterGnssStatusCallback(callback);
}

LocationErrCode Locator::UnregisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().UnregisterGnssStatusCallback(callback);
}

LocationErrCode Locator::RegisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().RegisterNmeaMessageCallback(callback);
}

LocationErrCode Locator::UnregisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().UnregisterNmeaMessageCallback(callback);
}

LocationErrCode Locator::QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().QuerySupportCoordinateSystemType(coordinateSystemTypes);
}

LocationErrCode Locator::IsLocationPrivacyConfirmed(const int type, bool &isConfirmed)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().IsLocationPrivacyConfirmed(type, isConfirmed);
}

LocationErrCode Locator::SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().SetLocationPrivacyConfirmStatus(type, isConfirmed);
}

LocationErrCode Locator::SendCommand(const LocationCommand *commands)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    return LocatorImpl::GetInstance().SendCommand(commands);
}

}  // namespace Location
}  // namespace OHOS
