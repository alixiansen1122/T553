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

#ifndef LOCATOR_SKELETON_H
#define LOCATOR_SKELETON_H

#include <map>
#include <string>
#include "single_instance.h"
#include "feature.h"
#include "iproxy_client.h"
#include "iunknown.h"
#include "locator_service_interface.h"

namespace OHOS {
namespace Location {

class LocatorFeature : public Feature {
DECLARE_SINGLE_INSTANCE(LocatorFeature);
public:
    bool Initialize();
    Identity *GetIdentity();

    static LocationErrCode IsLocationEnabled(bool &isEnabled);
    static LocationErrCode EnableAbility(bool enable);
    static LocationErrCode StartLocating(const RequestConfig *requestConfig,
        ILocatorCallback *callback);
    static LocationErrCode StopLocating(ILocatorCallback *callback);
    static LocationErrCode RegisterGnssStatusCallback(IGnssStatusCallback *callback);
    static LocationErrCode UnregisterGnssStatusCallback(IGnssStatusCallback *callback);
    static LocationErrCode RegisterNmeaMessageCallback(INmeaMessageCallback *callback);
    static LocationErrCode UnregisterNmeaMessageCallback(INmeaMessageCallback *callback);
    static LocationErrCode QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes);
    static LocationErrCode IsLocationPrivacyConfirmed(const int type, bool &isConfirmed);
    static LocationErrCode SetLocationPrivacyConfirmStatus(const int type, bool isConfirmed);
    static LocationErrCode SendCommand(const LocationCommand *commands);

private:
    static const char *GetFeatureName(Feature *feature);
    static void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);
    static void OnFeatureStop(Feature *feature, Identity identity);
    static BOOL OnFeatureMessage(Feature *feature, Request *request);

private:
    Identity identity_;
};

typedef struct {
    INHERIT_IUNKNOWNENTRY(LocatorInterface);
    LocatorFeature *locator;
} LocatorFeatureImpl;

} // namespace Location
} // namespace OHOS
#endif // LOCATOR_SKELETON_H
