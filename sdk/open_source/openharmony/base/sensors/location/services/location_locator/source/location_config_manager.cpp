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
#include "location_config_manager.h"

#include <fstream>

#include "common_utils.h"
#include "constant_definition.h"
#include "location_log.h"

namespace OHOS {
namespace Location {
const int UNKNOW_ERROR = -1;
const int MAX_SIZE = 100;
const int STATE_OPEN = 1;
const int STATE_CLOSE = 0;
const int PRIVACY_TYPE_OTHERS = 0;
const int PRIVACY_TYPE_STARTUP = 1;
const int PRIVACY_TYPE_CORE_LOCATION = 2;
const char *LOCATION_PRIVACY_MODE = "persist.location.privacy_mode";

LocationConfigManager *LocationConfigManager::GetInstance()
{
    static LocationConfigManager gLocationConfigManager;
    return &gLocationConfigManager;
}

LocationConfigManager::LocationConfigManager()
{
    mLocationSwitchState = STATE_CLOSE;
    mPrivacyTypeState[PRIVACY_TYPE_OTHERS] = STATE_CLOSE;         // for OTHERS
    mPrivacyTypeState[PRIVACY_TYPE_STARTUP] = STATE_CLOSE;        // for STARTUP
    mPrivacyTypeState[PRIVACY_TYPE_CORE_LOCATION] = STATE_CLOSE;  // for CORE_LOCATION
    MutexAttr attr = {false};
    mutex_ = MutexCreate(&attr);
}

LocationConfigManager::~LocationConfigManager()
{
    MutexDestroy(&mutex_);
}

int LocationConfigManager::Init()
{
    LBSLOGI(LOCATOR, "LocationConfigManager::Init");
    if (!IsExistFile(GetLocationSwitchConfigPath())) {
        CreateFile(GetLocationSwitchConfigPath(), "0");
    }
    return 0;
}

bool LocationConfigManager::IsExistFile(const std::string &filename)
{
    bool bExist = false;
    std::fstream ioFile;
    char path[PATH_MAX + 1] = {0x00};
    if (strlen(filename.c_str()) > PATH_MAX || realpath(filename.c_str(), path) == NULL) {
        return false;
    }
    ioFile.open(path, std::ios::in);
    if (ioFile) {
        bExist = true;
    } else {
        return false;
    }
    ioFile.clear();
    ioFile.close();
    LBSLOGD(LOCATOR, "IsExistFile = %d", bExist ? 1 : 0);
    return bExist;
}

bool LocationConfigManager::CreateFile(const std::string &filename, const std::string &filedata)
{
    LBSLOGD(LOCATOR, "CreateFile");
    std::ofstream outFile;
    outFile.open(filename.c_str());
    if (!outFile) {
        LBSLOGE(LOCATOR, "file open failed");
        return false;
    }
    outFile.flush();
    outFile << filedata << std::endl;
    outFile.clear();
    outFile.close();
    return true;
}

std::string LocationConfigManager::GetLocationSwitchConfigPath()
{
    int userId = 0;
    std::string filePath = LOCATION_DIR + SWITCH_CONFIG_NAME + "_" + std::to_string(userId) + ".conf";
    return filePath;
}

std::string LocationConfigManager::GetPrivacyTypeConfigPath(const int type)
{
    int userId = 0;
    std::string filePath;
    switch (type) {
        case PRIVACY_TYPE_OTHERS: {
            filePath = "others_";
            break;
        }
        case PRIVACY_TYPE_STARTUP: {
            filePath = "startup_";
            break;
        }
        case PRIVACY_TYPE_CORE_LOCATION: {
            filePath = "core_location_";
            break;
        }
        default: {
            filePath = "";
            break;
        }
    }
    return LOCATION_DIR + PRIVACY_CONFIG_NAME + "_" + filePath + std::to_string(userId) + ".conf";
}

int LocationConfigManager::GetLocationSwitchState()
{
    AutoLock lock(mutex_);
    if (!IsExistFile(GetLocationSwitchConfigPath())) {
        CreateFile(GetLocationSwitchConfigPath(), "0");
    }
    std::ifstream fs(GetLocationSwitchConfigPath());
    if (!fs.is_open()) {
        LBSLOGE(LOCATOR, "LocationConfigManager: fs.is_open false, return");
        return -1;
    }
    std::string line;
    while (std::getline(fs, line)) {
        if (line.empty()) {
            break;
        }
        if (line[0] == '0') {
            mLocationSwitchState = STATE_CLOSE;
        } else if (line[0] == '1') {
            mLocationSwitchState = STATE_OPEN;
        }
        break;
    }
    fs.clear();
    fs.close();
    return mLocationSwitchState;
}

bool LocationConfigManager::GetStringParameter(const std::string &type, std::string &value)
{
    char result[MAX_BUFF_SIZE] = {0};
    auto res = GetParameter(type.c_str(), "", result, MAX_BUFF_SIZE);
    if (res <= 0) {
        LBSLOGE(LOCATOR, "get para value failed, res: %d", res);
        return false;
    }
    value = result;
    return true;
}

int LocationConfigManager::GetIntParameter(const std::string &type)
{
    char result[MAX_BUFF_SIZE] = {0};
    std::string value = "";
    auto res = GetParameter(type.c_str(), "", result, MAX_BUFF_SIZE);
    if (res < 0 || strlen(result) == 0) {
        LBSLOGE(LOCATOR, "get para value failed, res: %d", res);
        return UNKNOW_ERROR;
    }
    value = result;
    for (auto ch : value) {
        if (std::isdigit(ch) == 0) {
            LBSLOGE(LOCATOR, "wrong para");
            return UNKNOW_ERROR;
        }
    }
    if (value.size() == 0) {
        return UNKNOW_ERROR;
    }
    return std::stoi(value);
}

int LocationConfigManager::GetParameter(const char *key, const char *def, char *value, uint32_t len)
{
    return 0;
}

int LocationConfigManager::SetParameter(const char *key, const char *value)
{
    return 0;
}

bool LocationConfigManager::GetNlpServiceName(std::string &name)
{
    return 0;
}

bool LocationConfigManager::GetNlpAbilityName(std::string &name)
{
    return 0;
}

int LocationConfigManager::GetSuplMode()
{
    return GetIntParameter(SUPL_MODE_NAME);
}

bool LocationConfigManager::GetAgnssServerAddr(std::string &name)
{
    return GetStringParameter(AGNSS_SERVER_ADDR, name);
}

int LocationConfigManager::GetAgnssServerPort()
{
    return GetIntParameter(AGNSS_SERVER_PORT);
}

int LocationConfigManager::SetLocationSwitchState(int state)
{
    AutoLock lock(mutex_);
    if (!IsExistFile(GetLocationSwitchConfigPath())) {
        CreateFile(GetLocationSwitchConfigPath(), "0");
    }
    std::fstream fs(GetLocationSwitchConfigPath());
    if (state != STATE_CLOSE && state != STATE_OPEN) {
        LBSLOGE(LOCATOR, "LocationConfigManager:SetLocationSwitchState state = %d, return", state);
        return -1;
    }
    if (!fs.is_open()) {
        LBSLOGE(LOCATOR, "LocationConfigManager: fs.is_open false, return");
        return -1;
    }
    std::string content = "1";
    if (state == STATE_CLOSE) {
        content = "0";
    }
    fs.write(content.c_str(), content.length());
    fs.clear();
    fs.close();
    mLocationSwitchState = state;
    return 0;
}

LocationErrCode LocationConfigManager::GetPrivacyTypeState(const int type, bool &isConfirmed)
{
    int status = 0;
    int cacheState = GetCachePrivacyType();
    if (cacheState == DISABLED || cacheState == ENABLED) {
        isConfirmed = (status == 1);
        return ERRCODE_SUCCESS;
    }
#ifdef RDB_SUPPORT
    if (!LocationDataRdbManager::GetLocationEnhanceStatus(status)) {
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
#endif
    isConfirmed = (status == 1);
    return ERRCODE_SUCCESS;
}

int LocationConfigManager::GetCachePrivacyType()
{
    return GetIntParameter(LOCATION_PRIVACY_MODE);
}

bool LocationConfigManager::SetCachePrivacyType(int value)
{
    char valueArray[MAX_SIZE] = {0};
    (void)sprintf_s(valueArray, sizeof(valueArray), "%d", value);
    int res = SetParameter(LOCATION_PRIVACY_MODE, valueArray);
    if (res < 0) {
        LBSLOGE(COMMON_UTILS, "failed, res: %d",  res);
        return false;
    }
    return true;
}

LocationErrCode LocationConfigManager::SetPrivacyTypeState(const int type, bool isConfirmed)
{
    int status = isConfirmed ? 1 : 0;
#ifdef RDB_SUPPORT
    if (!LocationDataRdbManager::SetLocationEnhanceStatus(status)) {
        return ERRCODE_SERVICE_UNAVAILABLE;
    }
#endif
    SetCachePrivacyType(status);
    return ERRCODE_SUCCESS;
}

#ifdef NOTIFICATION_ENABLE
std::string LocationConfigManager::GenerateStartCommand()
{
    nlohmann::json param;
    std::string uiType = "sysDialog/common";
    param["ability.want.params.uiExtensionType"] = uiType;
    std::string cmdData = param.dump();
    LBSLOGD(LOCATOR_GNSS, "cmdData is: %s.", cmdData.c_str());
    return cmdData;
}

void LocationConfigManager::OpenPrivacyDialog()
{
    LBSLOGI(LOCATOR, "ConnectExtension");
    AAFwk::Want want;
    std::string bundleName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    std::string connectStr = GenerateStartCommand();
    ConnectExtensionAbility(want, connectStr);
}

void LocationConfigManager::ConnectExtensionAbility(const AAFwk::Want &want, const std::string &commandStr)
{
    std::string bundleName = "com.ohos.locationdialog";
    std::string abilityName = "LocationPrivacyExtAbility";
    sptr<UIExtensionAbilityConnection> connection(
        new (std::nothrow) UIExtensionAbilityConnection(commandStr, bundleName, abilityName));
    if (connection == nullptr) {
        LBSLOGE(LOCATOR, "connect UIExtensionAbilityConnection fail");
        return;
    }

    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret =
        AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want, connection, nullptr, -1);
    LBSLOGI(LOCATOR, "connect service extension ability result = %d", ret);
    IPCSkeleton::SetCallingIdentity(identity);
    return;
}
#endif
}  // namespace Location
}  // namespace OHOS
