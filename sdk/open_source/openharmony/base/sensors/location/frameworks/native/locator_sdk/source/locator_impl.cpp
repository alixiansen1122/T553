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

#include "locator_impl.h"
#ifdef CMSIS_SUPPORT
#include <cmsis_os2.h>
#else
#include <pthread.h>
#endif
#include "iproxy_client.h"
#include "samgr_lite.h"
#include "common_utils.h"
#include "location_log.h"
#include "locator_service_interface.h"

namespace OHOS {
namespace Location {
constexpr uint32_t WAIT_MS = 1000;
constexpr int32_t RETRY_TIMES = 10;
const unsigned int ERROR_SLEEP_TIMES = 300;

IMPLEMENT_SINGLE_INSTANCE(LocatorImpl);

bool LocatorImpl::Init()
{
    MutexGlobalLock();
    if (inited_) {
        MutexGlobalUnLock();
        return true;
    }
    int32_t retry = RETRY_TIMES;
    while (retry--) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(LOCATOR_SERVICE, LOCATOR_FEATURE);
        if (iUnknown == nullptr) {
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }

        (void)iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&locatorProxy_);
        if (locatorProxy_ == nullptr) {
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }
        MutexAttr attr = { false };
        mutex_ = MutexCreate(&attr);
        CHECK_NULLPTR_RETURN_CODE(mutex_, LOCATOR_STANDARD, "MutexCreate faile", false);
        inited_ = true;
        MutexGlobalUnLock();
        return true;
    }
    MutexGlobalUnLock();
    return false;
}

bool LocatorImpl::DeInit()
{
    MutexGlobalLock();
    if (!inited_) {
        MutexGlobalUnLock();
        return false;
    }
    MutexDestroy(&mutex_);
    mutex_ = nullptr;
    locatorProxy_ = nullptr;
    inited_ = false;
    MutexGlobalUnLock();
    return true;
}

LocationErrCode LocatorImpl::IsLocationEnabled(bool &isEnabled)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->IsLocationEnabled(isEnabled);
}

LocationErrCode LocatorImpl::EnableAbility(bool enable)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->EnableAbility(enable);
}

LocationErrCode LocatorImpl::StartLocating(const RequestConfig *requestConfig,
    ILocatorCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->StartLocating(requestConfig, callback);
}

LocationErrCode LocatorImpl::StopLocating(ILocatorCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->StopLocating(callback);
}

LocationErrCode LocatorImpl::RegisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->RegisterGnssStatusCallback(callback);
}

LocationErrCode LocatorImpl::UnregisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->UnregisterGnssStatusCallback(callback);
}

LocationErrCode LocatorImpl::RegisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->RegisterNmeaMessageCallback(callback);
}

LocationErrCode LocatorImpl::UnregisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->UnregisterNmeaMessageCallback(callback);
}

LocationErrCode LocatorImpl::QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->QuerySupportCoordinateSystemType(coordinateSystemTypes);
}

LocationErrCode LocatorImpl::IsLocationPrivacyConfirmed(const int type, bool &isConfirmed)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->IsLocationPrivacyConfirmed(type, isConfirmed);
}

LocationErrCode LocatorImpl::SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->SetLocationPrivacyConfirmStatus(type, isConfirmed);
}

LocationErrCode LocatorImpl::SendCommand(const LocationCommand *commands)
{
    LBSLOGD(LOCATOR_STANDARD, "enter");
    AutoLock lock(mutex_);
    if (!inited_) {
        LBSLOGE(LOCATOR_STANDARD, "Locator service not inited");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    return locatorProxy_->SendCommand(commands);
}

}  // namespace Location
}  // namespace OHOS
