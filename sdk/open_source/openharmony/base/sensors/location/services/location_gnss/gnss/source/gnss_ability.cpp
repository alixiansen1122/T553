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

#ifdef FEATURE_GNSS_SUPPORT
#include "gnss_ability.h"

#include "agnss_ni_manager.h"

#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
#include "agnss_event_callback.h"
#endif
#include "common_utils.h"
#include "gnss_event_callback.h"
#include "location_config_manager.h"
#include "location_dumper.h"
#include "location_log.h"

namespace OHOS {
namespace Location {
IMPLEMENT_SINGLE_INSTANCE(GnssAbility);
namespace {
constexpr uint32_t WAIT_MS = 200;

constexpr int NLP_FIX_VALID_TIME = 2;
const int64_t INVALID_TIME = 0;
const int MODE_STANDALONE = 1;
const int MODE_MS_BASED = 2;
const int MODE_MS_ASSISTED = 3;
}

bool GnssAbility::Initialize()
{
    if (inited_) {
        return true;
    }
    MutexAttr attr = { false };
    gnssMutex_ = MutexCreate(&attr);
    nmeaMutex_ = MutexCreate(&attr);
    hdiMutex_ = MutexCreate(&attr);
    statusMutex_ = MutexCreate(&attr);
    if (gnssCallback_ == nullptr) {
        gnssCallback_ = new (std::nothrow) GnssEventCallback();
    }
#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
    if (agnssCallback_ == nullptr) {
        agnssCallback_ = new (std::nothrow) AGnssEventCallback();
    }
#endif
    gnssWorkingStatus_ = GNSS_WORKING_STATUS_NONE;
    SetAbility(GNSS_ABILITY);
#ifdef FEATURE_AGNSS_NI_SUPPORT
    auto agnssNiManager = AGnssNiManager::GetInstance();
    if (agnssNiManager != nullptr) {
        agnssNiManager->SubscribeSaStatusChangeListerner();
    }
#endif
    MonitorNetwork();
    inited_ = true;
    LBSLOGI(LOCATOR_GNSS, "ability constructed.");
    return true;
}

bool GnssAbility::DeInitialize()
{
    if (!inited_) {
        return false;
    }
    MutexDestroy(&gnssMutex_);
    gnssMutex_ = nullptr;
    MutexDestroy(&nmeaMutex_);
    nmeaMutex_ = nullptr;
    MutexDestroy(&hdiMutex_);
    hdiMutex_ = nullptr;
    MutexDestroy(&statusMutex_);
    statusMutex_ = nullptr;

    if (gnssCallback_ != nullptr) {
        delete gnssCallback_ ;
        gnssCallback_ = nullptr;
    }
#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
    if (agnssCallback_ != nullptr) {
        delete agnssCallback_ ;
        agnssCallback_ = nullptr;
    }
#endif
    gnssWorkingStatus_ = GNSS_WORKING_STATUS_NONE;
    inited_ = false;
    return true;
}

LocationErrCode GnssAbility::EnableAbility()
{
#ifdef RDB_SUPPORT
    if (LocationDataRdbManager::QuerySwitchState() != ENABLED) {
        LBSLOGE(LOCATOR_GNSS, "QuerySwitchState is DISABLED");
        return ERRCODE_FAILURE;
    }
#endif
    if (IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been enabled");
        return ERRCODE_FAILURE;
    }
    AutoLock lock(hdiMutex_);
    if (gnssCallback_ == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "gnssCallback_ is nullptr");
        return ERRCODE_FAILURE;
    }
    int32_t ret = IGnssInterface::GetInstance().EnableGnss(gnssCallback_);
    if (ret == ERRCODE_SUCCESS) {
        gnssWorkingStatus_ = GNSS_WORKING_STATUS_ENGINE_ON;
        LBSLOGD(LOCATOR_GNSS, "Successfully enable gnss ");
        return ERRCODE_SUCCESS;
    } else {
        gnssWorkingStatus_ = GNSS_WORKING_STATUS_NONE;
        LBSLOGE(LOCATOR_GNSS, "EnableGnss failed errCode :%d ", ret);
        return ERRCODE_FAILURE;
    }
}

LocationErrCode GnssAbility::DisableAbility()
{
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return ERRCODE_FAILURE;
    }
    int ret = IGnssInterface::GetInstance().DisableGnss();
    if (ret == ERRCODE_SUCCESS) {
        gnssWorkingStatus_ = GNSS_WORKING_STATUS_ENGINE_OFF;
        return ERRCODE_SUCCESS;
    } else {
        LBSLOGE(LOCATOR_GNSS, "DisableGnss failed errCode :%d ", ret);
        return ERRCODE_FAILURE;
    }
}

bool GnssAbility::IsGnssEnabled()
{
    return (gnssWorkingStatus_ != GNSS_WORKING_STATUS_ENGINE_OFF &&
        gnssWorkingStatus_ != GNSS_WORKING_STATUS_NONE);
}

void GnssAbility::RestGnssWorkStatus()
{
    AutoLock lock(statusMutex_);
    gnssWorkingStatus_ = GNSS_WORKING_STATUS_NONE;
}

LocationErrCode GnssAbility::StartAbility()
{
    LBSLOGE(LOCATOR_GNSS, "StartGnss ");

#ifdef RDB_SUPPORT
    if (LocationDataRdbManager::QuerySwitchState() != ENABLED) {
        LBSLOGE(LOCATOR_GNSS, "QuerySwitchState is DISABLED");
        return ERRCODE_FAILURE;
    }
#endif
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return ERRCODE_FAILURE;
    }
    if (gnssWorkingStatus_ == GNSS_WORKING_STATUS_SESSION_BEGIN) {
        LBSLOGD(LOCATOR_GNSS, "GNSS navigation started");
        return ERRCODE_SUCCESS;
    }

    int ret = IGnssInterface::GetInstance().StartGnss(GNSS_START_TYPE_NORMAL);
    if (ret == ERRCODE_SUCCESS) {
        gnssWorkingStatus_ = GNSS_WORKING_STATUS_SESSION_BEGIN;
        return ERRCODE_SUCCESS;
    } else {
        LBSLOGE(LOCATOR_GNSS, "StartGnss failed errCode :%d ", ret);
        return ERRCODE_FAILURE;
    }
}

LocationErrCode GnssAbility::StopAbility()
{
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return ERRCODE_FAILURE;
    }
    if (gnssWorkingStatus_ != GNSS_WORKING_STATUS_SESSION_BEGIN) {
        LBSLOGD(LOCATOR_GNSS, "GNSS navigation not started");
        return ERRCODE_FAILURE;
    }

    int ret = IGnssInterface::GetInstance().StopGnss(GNSS_START_TYPE_NORMAL);
    if (ret == ERRCODE_SUCCESS) {
        gnssWorkingStatus_ = GNSS_WORKING_STATUS_SESSION_END;
        return ERRCODE_SUCCESS;
    } else {
        LBSLOGE(LOCATOR_GNSS, "StopGnss failed errCode :%d ", ret);
        return ERRCODE_FAILURE;
    }
}

LocationErrCode GnssAbility::SendLocationRequest(WorkRecord &workrecord)
{
    LocationRequest(workrecord);
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::RefrashRequirements()
{
    HandleRefrashRequirements();
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::RegisterGnssStatusCallback(IGnssStatusCallback *callback, pid_t uid)
{
    if (callback == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "register an invalid gnssStatus callback");
        return ERRCODE_INVALID_PARAMS;
    }
    AutoLock lock(gnssMutex_);
    gnssStatusCallback_.push_back(callback);
    LBSLOGD(LOCATOR_GNSS, "after uid:%d register, gnssStatusCallback size:%s",
        uid, std::to_string(gnssStatusCallback_.size()).c_str());
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::UnregisterGnssStatusCallback(IGnssStatusCallback *callback)
{
    if (callback == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "unregister an invalid gnssStatus callback");
        return ERRCODE_INVALID_PARAMS;
    }
    AutoLock lock(gnssMutex_);
    if (gnssStatusCallback_.size() <= 0) {
        LBSLOGE(COUNTRY_CODE, "gnssStatusCallback_ size <= 0");
        return ERRCODE_SUCCESS;
    }
    size_t i = 0;
    for (; i < gnssStatusCallback_.size(); i++) {
        if (gnssStatusCallback_[i] == callback) {
            break;
        }
    }
    if (i >= gnssStatusCallback_.size()) {
        LBSLOGD(LOCATOR_GNSS, "gnssStatus callback is not in vector");
        return ERRCODE_SUCCESS;
    }
    gnssStatusCallback_.erase(gnssStatusCallback_.begin() + i);
    LBSLOGD(LOCATOR_GNSS, "after unregister, gnssStatus callback size:%s",
        std::to_string(gnssStatusCallback_.size()).c_str());
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::RegisterNmeaMessageCallback(INmeaMessageCallback *callback, pid_t uid)
{
    if (callback == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "register an invalid nmea callback");
        return ERRCODE_INVALID_PARAMS;
    }
    AutoLock lock(nmeaMutex_);
    nmeaCallback_.push_back(callback);
    LBSLOGD(LOCATOR_GNSS, "after uid:%d register, nmeaCallback size:%s",
        uid, std::to_string(nmeaCallback_.size()).c_str());
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::UnregisterNmeaMessageCallback(INmeaMessageCallback *callback)
{
    if (callback == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "unregister an invalid nmea callback");
        return ERRCODE_INVALID_PARAMS;
    }
    AutoLock lock(nmeaMutex_);
    if (nmeaCallback_.size() <= 0) {
        LBSLOGE(LOCATOR_GNSS, "nmeaCallback_ size <= 0");
        return ERRCODE_SUCCESS;
    }
    size_t i = 0;
    for (; i < nmeaCallback_.size(); i++) {
        if (nmeaCallback_[i] == callback) {
            break;
        }
    }
    if (i >= nmeaCallback_.size()) {
        LBSLOGD(LOCATOR_GNSS, "nmea callback is not in vector");
        return ERRCODE_SUCCESS;
    }
    nmeaCallback_.erase(nmeaCallback_.begin() + i);
    LBSLOGD(LOCATOR_GNSS, "after unregister, nmea callback size:%s",
        std::to_string(nmeaCallback_.size()).c_str());
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::QuerySupportCoordinateSystemType(
        std::vector<LocationCoordinateSystemType>& coordinateSystemTypes)
{
    coordinateSystemTypes.push_back(WGS84);
    return ERRCODE_SUCCESS;
}

void GnssAbility::RequestRecord(WorkRecord &workRecord, bool isAdded)
{
    LBSLOGD(LOCATOR_GNSS, "enter RequestRecord");
    if (isAdded) {
        EnableAbility();
#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
        SetAgnssServer();
#endif
        StartAbility();
    } else {
        // GNSS will stop only if all requests have stopped
        if (GetRequestNum() == 0) {
            StopAbility();
        }
    }
}

bool GnssAbility::GetCommandFlags(const LocationCommand *commands, GnssAuxiliaryDataType& flags)
{
    std::string cmd = commands->command;
    if (cmd == "delete_auxiliary_data_ephemeris") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_EPHEMERIS;
    } else if (cmd == "delete_auxiliary_data_almanac") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_ALMANAC;
    } else if (cmd == "delete_auxiliary_data_position") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_POSITION;
    } else if (cmd == "delete_auxiliary_data_time") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_TIME;
    } else if (cmd == "delete_auxiliary_data_iono") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_IONO;
    } else if (cmd == "delete_auxiliary_data_utc") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_UTC;
    } else if (cmd == "delete_auxiliary_data_health") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_HEALTH;
    } else if (cmd == "delete_auxiliary_data_svdir") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_SVDIR;
    } else if (cmd == "delete_auxiliary_data_svsteer") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_SVSTEER;
    } else if (cmd == "delete_auxiliary_data_sadata") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_SADATA;
    } else if (cmd == "delete_auxiliary_data_rti") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_RTI;
    } else if (cmd == "delete_auxiliary_data_celldb_info") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_CELLDB_INFO;
    } else if (cmd == "delete_auxiliary_data_all") {
        flags = GnssAuxiliaryDataType::GNSS_AUXILIARY_DATA_ALL;
    } else {
        LBSLOGE(LOCATOR_GNSS, "unknow command %s", cmd.c_str());
        return false;
    }
    return true;
}

LocationErrCode GnssAbility::SendCommand(const LocationCommand *commands)
{
    GnssAuxiliaryDataType flags;
    bool result = GetCommandFlags(commands, flags);
    LBSLOGI(LOCATOR_GNSS, "GetCommandFlags,flags = %d", flags);
    if (result) {
        IGnssInterface::GetInstance().DeleteAuxiliaryData(flags);
    }
    return ERRCODE_NOT_SUPPORTED;
}

LocationErrCode GnssAbility::SetPositionMode()
{
    GnssConfigPara para;
    int suplMode = LocationConfigManager::GetInstance()->GetSuplMode();
    if (suplMode == MODE_STANDALONE) {
        para.gnssBasic.gnssMode = GnssWorkingMode::GNSS_WORKING_MODE_STANDALONE;
    } else if (suplMode == MODE_MS_BASED) {
        para.gnssBasic.gnssMode = GnssWorkingMode::GNSS_WORKING_MODE_MS_BASED;
    } else if (suplMode == MODE_MS_ASSISTED) {
        para.gnssBasic.gnssMode = GnssWorkingMode::GNSS_WORKING_MODE_MS_ASSISTED;
    } else {
        LBSLOGE(LOCATOR_GNSS, "unknow mode");
        return ERRCODE_SUCCESS;
    }
    int ret = IGnssInterface::GetInstance().SetGnssConfigPara(para);
    if (ret != ERRCODE_SUCCESS) {
        LBSLOGE(LOCATOR_GNSS, "SetGnssConfigPara failed , ret =%d", ret);
    }
    return ERRCODE_SUCCESS;
}

void GnssAbility::MonitorNetwork()
{
#ifdef NET_MANAGER_ENABLE
    NetManagerStandard::NetSpecifier netSpecifier;
    NetManagerStandard::NetAllCapabilities netAllCapabilities;
    netAllCapabilities.netCaps_.insert(NetManagerStandard::NetCap::NET_CAPABILITY_INTERNET);
    netSpecifier.netCapabilities_ = netAllCapabilities;
    sptr<NetManagerStandard::NetSpecifier> specifier(
        new (std::nothrow) NetManagerStandard::NetSpecifier(netSpecifier));
    if (specifier == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "new operator error.specifier is nullptr");
        return;
    }
    netWorkObserver_ = sptr<NetConnObserver>((new (std::nothrow) NetConnObserver()));
    if (netWorkObserver_ == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "new operator error.netWorkObserver_ is nullptr");
        return;
    }
    int ret = NetManagerStandard::NetConnClient::GetInstance().RegisterNetConnCallback(specifier, netWorkObserver_, 0);
    LBSLOGI(LOCATOR_GNSS, "RegisterNetConnCallback retcode= %d", ret);
#endif
    return;
}

LocationErrCode GnssAbility::InjectTime()
{
#ifdef TIME_SERVICE_ENABLE
    LBSLOGD(LOCATOR_GNSS, "InjectTime");
    int64_t currentTime = ntpTime_.GetCurrentTime();
    if (currentTime == INVALID_TIME) {
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    auto elapsedTime = MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
    if (elapsedTime < 0) {
        LBSLOGE(LOCATOR_GNSS, "get boot time failed");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    auto ntpTimeCheck = NtpTimeCheck::GetInstance();
    if (ntpTimeCheck != nullptr && ntpTimeCheck->CheckNtpTime(currentTime, elapsedTime)) {
        GnssRefInfo refInfo;
        refInfo.type = GnssRefInfoType::GNSS_REF_INFO_TIME;
        refInfo.time.time = currentTime;
        refInfo.time.elapsedRealtime = elapsedTime;
        refInfo.time.uncertaintyOfTime = ntpTimeCheck->GetUncertainty();
        LBSLOGI(LOCATOR_GNSS, "inject ntp time: %s unert %d",
                std::to_string(currentTime).c_str(), ntpTimeCheck->GetUncertainty());
        IGnssInterface::GetInstance().SetGnssReferenceInfo(refInfo);
    }
#endif
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::UpdateNtpTime(int64_t ntpTime, int64_t elapsedTime)
{
#ifdef TIME_SERVICE_ENABLE
    if (ntpTime <= 0 || elapsedTime <= 0) {
        LBSLOGE(LOCATOR_GNSS, "failed to UpdateNtpTime");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    ntpTime_.SetCurrentTime(ntpTime, elapsedTime);
#endif
    return ERRCODE_SUCCESS;
}

LocationErrCode GnssAbility::SendNetworkLocation(const std::shared_ptr<Location>& location)
{
    if (location == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "location is nullptr");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    nlpLocation_ = *location;
    return InjectLocation();
}

LocationErrCode GnssAbility::InjectLocation()
{
    if (nlpLocation_.GetAccuracy() < 1e-9 || nlpLocation_.GetTimeStamp() == 0) {
        LBSLOGW(LOCATOR_GNSS, "nlp locaton acc or timesatmp is invalid");
        return ERRCODE_INVALID_PARAMS;
    }
    int64_t diff = CommonUtils::GetCurrentTimeStamp() - nlpLocation_.GetTimeStamp() / MILLI_PER_SEC;
    if (diff > NLP_FIX_VALID_TIME) {
        LBSLOGI(LOCATOR_GNSS, "nlp locaton is invalid");
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
    GnssRefInfo refInfo;
    refInfo.type = GnssRefInfoType::GNSS_REF_INFO_LOCATION;
    refInfo.gnssLocation.fieldValidity =
        GnssLocationValidity::GNSS_LOCATION_LAT_VALID |
        GnssLocationValidity::GNSS_LOCATION_LONG_VALID |
        GnssLocationValidity::GNSS_LOCATION_HORIZONTAL_ACCURACY_VALID;
    refInfo.gnssLocation.latitude = nlpLocation_.GetLatitude();
    refInfo.gnssLocation.longitude = nlpLocation_.GetLongitude();
    refInfo.gnssLocation.altitude = nlpLocation_.GetAltitude();
    refInfo.gnssLocation.speed = nlpLocation_.GetSpeed();
    refInfo.gnssLocation.bearing = nlpLocation_.GetDirection();
    refInfo.gnssLocation.horizontalAccuracy = nlpLocation_.GetAccuracy();
    refInfo.gnssLocation.verticalAccuracy = nlpLocation_.GetAltitudeAccuracy();
    refInfo.gnssLocation.speedAccuracy = nlpLocation_.GetSpeedAccuracy();
    refInfo.gnssLocation.bearingAccuracy = nlpLocation_.GetDirectionAccuracy();
    refInfo.gnssLocation.timeForFix = nlpLocation_.GetTimeStamp();
    refInfo.gnssLocation.timeSinceBoot = nlpLocation_.GetTimeSinceBoot();
    refInfo.gnssLocation.timeUncertainty = nlpLocation_.GetUncertaintyOfTimeSinceBoot();
    IGnssInterface::GetInstance().SetGnssReferenceInfo(refInfo);
    return ERRCODE_SUCCESS;
}

void GnssAbility::ReportGnssSessionStatus(int status)
{
}

void GnssAbility::ReportNmea(int64_t timestamp, const std::string &nmea)
{
    AutoLock lock(nmeaMutex_);
    for (auto nmeaCallback : nmeaCallback_) {
        nmeaCallback->OnMessageChange(timestamp, nmea);
    }
}

void GnssAbility::ReportSv(const std::shared_ptr<SatelliteStatus> &sv)
{
    AutoLock lock(gnssMutex_);
    for (auto gnssStatusCallback : gnssStatusCallback_) {
        gnssStatusCallback->OnStatusChange(sv);
    }
}

#ifdef HDF_DRIVERS_INTERFACE_AGNSS_ENABLE
void GnssAbility::SetAgnssServer()
{
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return;
    }
    std::string addrName;
    bool result = LocationConfigManager::GetInstance()->GetAgnssServerAddr(addrName);
    if (!result || addrName.empty()) {
        LBSLOGE(LOCATOR_GNSS, "get agnss server address failed!");
        return;
    }
    int port = LocationConfigManager::GetInstance()->GetAgnssServerPort();
    AGnssServerInfo info;
    info.type = AGNSS_TYPE_SUPL;
    info.server = addrName;
    info.port = port;
    IAGnssInterface::GetInstance().SetAgnssServer(info);
}

void GnssAbility::SetAgnssCallback()
{
    LBSLOGD(LOCATOR_GNSS, "enter SetAgnssCallback");
    AutoLock lock(hdiMutex_);
    if (agnssCallback_ == nullptr) {
        LBSLOGE(LOCATOR_GNSS, "agnssInterface or agnssCallback_ is nullptr");
        return;
    }
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return;
    }
    IAGnssInterface::GetInstance().SetAgnssCallback(agnssCallback_);
}

void GnssAbility::SetSetId(const SubscriberSetId& id)
{
    SetSetIdImpl(id);
}

void GnssAbility::SetSetIdImpl(const SubscriberSetId& id)
{
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return;
    }
    IAGnssInterface::GetInstance().SetSubscriberSetId(id);
}

void GnssAbility::SetRefInfo(const AGnssRefInfo& refInfo)
{
    SetRefInfoImpl(refInfo);
}

void GnssAbility::SetRefInfoImpl(const AGnssRefInfo &refInfo)
{
    if (!IsGnssEnabled()) {
        LBSLOGE(LOCATOR_GNSS, "gnss has been disabled");
        return;
    }
    IAGnssInterface::GetInstance().SetAgnssRefInfo(refInfo);
}

#endif

void GnssAbility::SaDumpInfo(std::string& result)
{
    result += "Gnss Location enable status: true";
    result += "\n";
}
} // namespace Location
} // namespace OHOS
#endif
