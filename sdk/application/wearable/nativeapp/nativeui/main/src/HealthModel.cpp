/*
 * Copyright (c) 2020-2021 CompanyNameMagicTag.
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
#include "main/HealthModel.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "securec.h"
#include "message.h"
#include "wearable_log.h"
#include "iostream"
#include <fstream>
#include "graphic_mutex.h"

namespace OHOS {
HealthAllData g_healthData;

static GraphicMutex g_healthDataMutex;
static constexpr const char *HEALTH_DATA_PATH = "/user/res/health.bin";
static constexpr uint16_t LIST_HEALTH_LENGTH = 30;

HealthModel::HealthModel()
{
}

HealthModel::~HealthModel()
{
    listHealth.Clear();
    listHealthNum = 0;
}

HealthModel *HealthModel::GetInstance(void)
{
    static HealthModel healthModel;
    return &healthModel;
}

int HealthModel::SaveGlobalHealthToFile(void)
{
    std::ofstream outHealthFile;
    outHealthFile.open(HEALTH_DATA_PATH, std::ios::out);
    if (outHealthFile.is_open() == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "open HEALTH_DATA_PATH failure!");
        outHealthFile.close();
        return -1;
    } else {
        outHealthFile.write((const char*)&g_healthData, sizeof(HealthAllData));
        if (outHealthFile.fail()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SaveGlobalHealthToFile fail");
            return -1;
        }
        outHealthFile.close();
        return 0;
    }
}

int HealthModel::GetHealthGlobalData(DataDistribute type, uint32_t *value)
{
    switch (type) {
        case HEALTH_SLEEP:
            *value = g_healthData.healthSleep;
            break;
        case BATTERY_LIFE:
            break;
        default:
            return -1;
            break;
    }
    return 0;
}

void HealthModel::PushHealthData(HealthMsg healthMsg)
{
    uint16_t tmp;
    if (listHealthNum >= LIST_HEALTH_MAX) {
        return;
    }
    g_healthDataMutex.Lock();
    listHealthNum++;
    listHealth.PushBack(healthMsg);
    g_healthDataMutex.Unlock();
    switch (healthMsg.type) {
        case HEALTH_SLEEP:
            g_healthData.healthSleep = healthMsg.value;
            break;
        case BATTERY_LIFE:
            break;
        default:
            break;
    }
}

bool HealthModel::FetchedHealthData(HealthMsg *healthMsg)
{
    if (listHealth.IsEmpty()) {
        return false;
    } else {
        g_healthDataMutex.Lock();
        listHealthNum--;
        *healthMsg = listHealth.Front();
        listHealth.PopFront();
        g_healthDataMutex.Unlock();
    }
    return true;
}

bool HealthModel::InitHealthData(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HealthModel::InitHealthData start");
    (void)memset_s(&g_healthData, sizeof(HealthAllData), 0, sizeof(HealthAllData));
    std::ifstream inHealthFile;
    inHealthFile.open(HEALTH_DATA_PATH, std::ios::in);
    if (!inHealthFile.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "open HEALTH_DATA_PATH failure!");
        inHealthFile.close();
        return false;
    } else {
        inHealthFile.read((char*)&g_healthData, sizeof(HealthAllData));
        if (inHealthFile.fail()) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitHealthData fail");
            inHealthFile.close();
            return false;
        }
        inHealthFile.close();
    }
    return true;
}

int HealthModel::GetBriScrStatus(void)
{
    return briScreenStatus;
}

int HealthModel::GetDntDistStatus(void)
{
    return dntDisturbStatus;
}

void HealthModel::UpdateBriScrStatus(uint16_t status)
{
    briScreenStatus = status;
}

void HealthModel::UpdateDntDistStatus(uint16_t status)
{
    dntDisturbStatus = status;
}
}
