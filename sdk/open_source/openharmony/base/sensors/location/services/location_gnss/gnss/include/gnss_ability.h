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

#ifndef GNSS_ABILITY_H
#define GNSS_ABILITY_H
#ifdef FEATURE_GNSS_SUPPORT
#include "single_instance.h"
#include "i_gnss_status_callback.h"
#include "i_locator_callback.h"
#include "i_nmea_message_callback.h"
#include <v2_0/ignss_interface.h>
#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
#include <v2_0/ia_gnss_interface.h>
#endif

#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
#include "agnss_event_callback.h"
#endif
#include "location_thread.h"
#include "common_utils.h"
#include "constant_definition.h"
#include "i_gnss_status_callback.h"
#include "i_nmea_message_callback.h"
#include "subability_common.h"

#ifdef TIME_SERVICE_ENABLE
#include "time_manager.h"
#endif
#ifdef NET_MANAGER_ENABLE
#include "net_conn_observer.h"
#include "net_conn_client.h"
#include "net_specifier.h"
#endif

namespace OHOS {
namespace Location {
#ifdef __aarch64__
#define VENDOR_GNSS_ADAPTER_SO_PATH "/system/lib64/vendorGnssAdapter.so"
#else
#define VENDOR_GNSS_ADAPTER_SO_PATH "/system/lib/vendorGnssAdapter.so"
#endif

using HDI::Location::Gnss::V2_0::IGnssInterface;
using HDI::Location::Gnss::V2_0::IGnssCallback;
using HDI::Location::Gnss::V2_0::GNSS_START_TYPE_NORMAL;
using HDI::Location::Gnss::V2_0::GNSS_WORKING_STATUS_NONE;
using HDI::Location::Gnss::V2_0::GNSS_WORKING_STATUS_SESSION_BEGIN;
using HDI::Location::Gnss::V2_0::GNSS_WORKING_STATUS_SESSION_END;
using HDI::Location::Gnss::V2_0::GNSS_WORKING_STATUS_ENGINE_ON;
using HDI::Location::Gnss::V2_0::GNSS_WORKING_STATUS_ENGINE_OFF;
using HDI::Location::Gnss::V2_0::GnssAuxiliaryDataType;
using HDI::Location::Gnss::V2_0::GnssWorkingMode;
using HDI::Location::Gnss::V2_0::GnssConfigPara;
using HDI::Location::Gnss::V2_0::GnssRefInfoType;
using HDI::Location::Gnss::V2_0::GnssRefInfo;
using HDI::Location::Gnss::V2_0::GnssLocationValidity;
#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
using HDI::Location::Agnss::V2_0::IAGnssInterface;
using HDI::Location::Agnss::V2_0::IAGnssCallback;
using HDI::Location::Agnss::V2_0::AGNSS_TYPE_SUPL;
using HDI::Location::Agnss::V2_0::AGnssServerInfo;
#endif

class GnssAbility : public SubAbility {
DECLARE_SINGLE_INSTANCE(GnssAbility);
public:
    bool Initialize() override;
    bool DeInitialize() override;

    LocationErrCode StartAbility() override;
    LocationErrCode StopAbility() override;
    LocationErrCode EnableAbility() override;
    LocationErrCode DisableAbility() override;

    LocationErrCode SendLocationRequest(WorkRecord &workrecord) override;
 
    LocationErrCode RefrashRequirements();
    LocationErrCode RegisterGnssStatusCallback(IGnssStatusCallback *callback, pid_t uid);
    LocationErrCode UnregisterGnssStatusCallback(IGnssStatusCallback *callback);
    LocationErrCode RegisterNmeaMessageCallback(INmeaMessageCallback *callback, pid_t uid);
    LocationErrCode UnregisterNmeaMessageCallback(INmeaMessageCallback *callback);
    LocationErrCode SendCommand(const LocationCommand *commands);
    void ReportGnssSessionStatus(int status);
    void ReportNmea(int64_t timestamp, const std::string &nmea);
    void ReportSv(const std::shared_ptr<SatelliteStatus> &sv);

    void RequestRecord(WorkRecord &workRecord, bool isAdded)  override;

#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
    void SetAgnssServer();
    void SetAgnssCallback();
    void SetSetId(const SubscriberSetId& id);
    void SetSetIdImpl(const SubscriberSetId &id);
    void SetRefInfo(const AGnssRefInfo& refInfo);
    void SetRefInfoImpl(const AGnssRefInfo &refInfo);
#endif
    bool CheckIfHdiConnected();
    void RestGnssWorkStatus();

    LocationErrCode QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes);
    LocationErrCode SendNetworkLocation(const std::shared_ptr<Location>& location);
    LocationErrCode InjectLocation();
    LocationErrCode InjectTime();
    LocationErrCode UpdateNtpTime(int64_t ntpTime, int64_t elapsedTime);
    void MonitorNetwork();

private:
    static void SaDumpInfo(std::string& result);
    bool IsGnssEnabled();
    bool CheckIfGnssConnecting();
    bool GetCommandFlags(const LocationCommand *commands, GnssAuxiliaryDataType& flags);
    LocationErrCode SetPositionMode();

    bool inited_ = false;
    int gnssWorkingStatus_ = 0;
    MutexId gnssMutex_;
    MutexId nmeaMutex_;
    MutexId hdiMutex_;
    MutexId statusMutex_;
    std::vector<IGnssStatusCallback*> gnssStatusCallback_;
    std::vector<INmeaMessageCallback*> nmeaCallback_;
    IGnssCallback* gnssCallback_;
    Location nlpLocation_;
#ifdef TIME_SERVICE_ENABLE
    TimeManager ntpTime_;
#endif

#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
    IAGnssCallback* agnssCallback_;
#endif
};
} // namespace Location
} // namespace OHOS
#endif // FEATURE_GNSS_SUPPORT
#endif // GNSS_ABILITY_H
