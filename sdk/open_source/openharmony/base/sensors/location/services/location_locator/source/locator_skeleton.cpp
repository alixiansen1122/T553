/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#include "locator_skeleton.h"
#include "locator_ability.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "securec.h"

namespace OHOS {
namespace Location {
LocatorFeatureImpl g_locatorImpl = {
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .IsLocationEnabled = LocatorFeature::IsLocationEnabled,
    .EnableAbility = LocatorFeature::EnableAbility,
    .StartLocating = LocatorFeature::StartLocating,
    .StopLocating = LocatorFeature::StopLocating,
    .RegisterGnssStatusCallback = LocatorFeature::RegisterGnssStatusCallback,
    .UnregisterGnssStatusCallback = LocatorFeature::UnregisterGnssStatusCallback,
    .RegisterNmeaMessageCallback = LocatorFeature::RegisterNmeaMessageCallback,
    .UnregisterNmeaMessageCallback = LocatorFeature::UnregisterNmeaMessageCallback,
    .QuerySupportCoordinateSystemType = LocatorFeature::QuerySupportCoordinateSystemType,
    .IsLocationPrivacyConfirmed = LocatorFeature::IsLocationPrivacyConfirmed,
    .SetLocationPrivacyConfirmStatus = LocatorFeature::SetLocationPrivacyConfirmStatus,
    .SendCommand = LocatorFeature::SendCommand,
    DEFAULT_IUNKNOWN_ENTRY_END
};

static void Init()
{
    SamgrLite *samgrLite = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(samgrLite, "LocatorFeature", "get samgr error");
    bool inited = LocatorFeature::GetInstance().Initialize();
    CHECK_FAILED_RETURN("LocatorFeature", inited, true, "LocatorFeature init error");
    BOOL result = samgrLite->RegisterFeature(LOCATOR_SERVICE, &LocatorFeature::GetInstance());
    if (result == FALSE) {
        LBSLOGE(LOCATOR_STANDARD, "[Locator] register feature failure");
        return;
    }
    g_locatorImpl.locator = &LocatorFeature::GetInstance();
    auto publicApi = GET_IUNKNOWN(g_locatorImpl);
    CHECK_NULLPTR_RETURN(publicApi, "LocatorFeatureLite", "publicApi is nullptr");
    BOOL apiResult = samgrLite->RegisterFeatureApi(LOCATOR_SERVICE, LOCATOR_FEATURE, publicApi);
    LBSLOGI(LOCATOR_STANDARD, "Locator feature init %s", apiResult ? "success" : "failure");
}

SYSEX_FEATURE_INIT(Init);

IMPLEMENT_SINGLE_INSTANCE(LocatorFeature);

bool LocatorFeature::Initialize()
{
    this->Feature::GetName = LocatorFeature::GetFeatureName;
    this->Feature::OnInitialize = LocatorFeature::OnFeatureInitialize;
    this->Feature::OnStop = LocatorFeature::OnFeatureStop;
    this->Feature::OnMessage = LocatorFeature::OnFeatureMessage;
    return true;
}

const char *LocatorFeature::GetFeatureName(Feature *feature)
{
    (void) feature;
    return LOCATOR_FEATURE;
}

Identity *LocatorFeature::GetIdentity()
{
    return &identity_;
}

void LocatorFeature::OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    LBSLOGI(LOCATOR_STANDARD, "Locator feature init");
    CHECK_NULLPTR_RETURN(feature, "LocatorFeature", "initialize fail");
    (static_cast<LocatorFeature *>(feature))->identity_ = identity;
    bool ret = LocatorAbility::GetInstance().Initialize();
    LBSLOGI(LOCATOR_STANDARD, "LocatorAbility initialize %s", ret ? "success" : "fail");
}

void LocatorFeature::OnFeatureStop(Feature *feature, Identity identity)
{
    (void) feature;
    (void) identity;
    bool ret = LocatorAbility::GetInstance().DeInitialize();
    LBSLOGI(LOCATOR_STANDARD, "LocatorAbility DeInitialize %s", ret ? "success" : "fail");
}

BOOL LocatorFeature::OnFeatureMessage(Feature *feature, Request *request)
{
    LBSLOGI(LOCATOR_STANDARD, "OnFeatureMessage");
    if (feature == nullptr || request == nullptr) {
        return FALSE;
    }
    return TRUE;
}

LocationErrCode LocatorFeature::IsLocationEnabled(bool &isEnabled)
{
    return LocatorAbility::GetInstance().IsLocationEnabled(isEnabled);
}

LocationErrCode LocatorFeature::EnableAbility(bool enable)
{
    return LocatorAbility::GetInstance().EnableAbility(enable);
}

LocationErrCode LocatorFeature::StartLocating(const RequestConfig *requestConfig,
    ILocatorCallback *callback)
{
    AppIdentity identity;
    return LocatorAbility::GetInstance().StartLocating(requestConfig, callback, identity);
}

LocationErrCode LocatorFeature::StopLocating(ILocatorCallback *callback)
{
    return LocatorAbility::GetInstance().StopLocating(callback);
}

LocationErrCode LocatorFeature::RegisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    return LocatorAbility::GetInstance().RegisterGnssStatusCallback(callback, 0);
}

LocationErrCode LocatorFeature::UnregisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    return LocatorAbility::GetInstance().UnregisterGnssStatusCallback(callback);
}

LocationErrCode LocatorFeature::RegisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    return LocatorAbility::GetInstance().RegisterNmeaMessageCallback(callback, 0);
}

LocationErrCode LocatorFeature::UnregisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    return LocatorAbility::GetInstance().UnregisterNmeaMessageCallback(callback);
}

LocationErrCode LocatorFeature::QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes)
{
    return LocatorAbility::GetInstance().QuerySupportCoordinateSystemType(coordinateSystemTypes);
}

LocationErrCode LocatorFeature::IsLocationPrivacyConfirmed(const int type, bool &isConfirmed)
{
    return ERRCODE_NOT_SUPPORTED;
}

LocationErrCode LocatorFeature::SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed)
{
    return ERRCODE_NOT_SUPPORTED;
}

LocationErrCode LocatorFeature::SendCommand(const LocationCommand *commands)
{
    return LocatorAbility::GetInstance().SendCommand(commands);
}

} // namespace Location
} // namespace OHOS