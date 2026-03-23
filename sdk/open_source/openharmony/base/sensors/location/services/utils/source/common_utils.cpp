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

#include <map>
#include <random>
#include <sys/time.h>
#include <sstream>

#include "common_utils.h"
#include "constant_definition.h"

namespace OHOS {
namespace Location {

// static std::random_device g_randomDevice;
// static std::mt19937 g_gen(g_randomDevice());
// static std::uniform_int_distribution<> g_dis(0, 15);   // random between 0 and 15
// static std::uniform_int_distribution<> g_dis2(8, 11);  // random between 8 and 11
const int64_t SEC_TO_NANO = 1000 * 1000 * 1000;
bool CommonUtils::GetCurrentUserId(int &userId)
{
#ifdef ACCOUNT_MGR_SUPPORT
    std::vector<int> activeIds;
    int ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activeIds);
    if (ret != 0) {
        LBSLOGI(COMMON_UTILS, "GetCurrentUserId failed ret:%d", ret);
        return false;
    }
    if (activeIds.empty()) {
        LBSLOGE(COMMON_UTILS, "QueryActiveOsAccountIds activeIds empty");
        return false;
    }
    userId = activeIds[0];
#else
    userId = 0;
#endif
    return true;
}

bool CommonUtils::DoubleEqual(double a, double b)
{
    if (fabs(a - b) < 1e-6) {
        return true;
    } else {
        return false;
    }
}

double CommonUtils::CalDistance(const double lat1, const double lon1, const double lat2, const double lon2)
{
    double radLat1 = lat1 * PI / DEGREE_PI;
    double radLat2 = lat2 * PI / DEGREE_PI;
    double radLon1 = lon1 * PI / DEGREE_PI;
    double radLon2 = lon2 * PI / DEGREE_PI;

    double latDiff = radLat1 - radLat2;
    double lonDiff = radLon1 - radLon2;
    double temp = sqrt(pow(sin(latDiff / DIS_FROMLL_PARAMETER), DIS_FROMLL_PARAMETER) +
        cos(radLat1) * cos(radLat2) * pow(sin(lonDiff / DIS_FROMLL_PARAMETER), DIS_FROMLL_PARAMETER));
    double disRad = asin(temp) * DIS_FROMLL_PARAMETER;
    double dis = disRad * EARTH_RADIUS;
    return dis;
}

double CommonUtils::DoubleRandom(double min, double max)
{
    double param = 0.0;
    return param;
}

int CommonUtils::IntRandom(int min, int max)
{
    int param = 0;
    return param;
}

int64_t CommonUtils::GetCurrentTime()
{
    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    int64_t second = static_cast<int64_t>(times.tv_sec);
    return second;
}

int64_t CommonUtils::GetCurrentTimeStamp()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    return static_cast<int64_t>(currentTime.tv_sec);
}

std::vector<std::string> CommonUtils::Split(std::string str, std::string pattern)
{
    std::vector<std::string> result;
    str += pattern;
    size_t size = str.size();
    size_t i = 0;
    while (i < size) {
        size_t pos = str.find(pattern, i);
        if (pos != std::string::npos && pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
        i++;
    }
    return result;
}

uint8_t CommonUtils::ConvertStringToDigit(std::string str)
{
    uint8_t res = 0;
    constexpr int bitWidth = 4;
    constexpr int numDiffForHexAlphabet = 10;
    for (auto ch : str) {
        res = res << bitWidth;
        if (ch >= '0' && ch <= '9') {
            res += (ch - '0');
        }
        if (ch >= 'A' && ch <= 'F') {
            res += (ch - 'A' + numDiffForHexAlphabet);
        }
        if (ch >= 'a' && ch <= 'f') {
            res += (ch - 'a' + numDiffForHexAlphabet);
        }
    }
    return res;
}

errno_t CommonUtils::GetMacArray(const std::string& strMac, uint8_t mac[MAC_LEN])
{
    std::vector<std::string> strVec = Split(strMac, ":");
    for (size_t i = 0; i < strVec.size() && i < MAC_LEN; i++) {
        mac[i] = ConvertStringToDigit(strVec[i]);
    }
    return EOK;
}


std::string CommonUtils::GenerateUuid()
{
    std::stringstream ss;
    return ss.str();
}

bool CommonUtils::CheckAppForUser(int32_t uid)
{
    int currentUserId = 0;
    int userId = 0;
    bool ret = GetCurrentUserId(currentUserId);
    if (!ret) {
        return true;
    }
#ifdef ACCOUNT_MGR_SUPPORT
    auto result = AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
    if (result != ERR_OK) {
        return true;
    }
    if (userId != currentUserId) {
        return false;
    }
#endif
    return true;
}

int64_t CommonUtils::GetSinceBootTime()
{
    int result;
    struct timespec ts;
    result = clock_gettime(CLOCK_BOOTTIME, &ts);
    if (result == 0) {
        return ts.tv_sec * SEC_TO_NANO + ts.tv_nsec;
    } else {
        return 0;
    }
}
} // namespace Location
} // namespace OHOS
