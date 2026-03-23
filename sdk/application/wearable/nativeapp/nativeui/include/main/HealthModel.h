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

#ifndef HEALTH_MODEL_H
#define HEALTH_MODEL_H

#include <cstdio>
#include "gfx_utils/list.h"
#include "UiServersMsg.h"

namespace OHOS {
#define NUM_1 1
#define NUM_2 2
#define NUM_3 3
#define NUM_4 4
#define NUM_5 5
#define NUM_7 7
#define NUM_16 16
#define NUM_99 99

typedef struct {
    uint32 healthSleep;
    uint32 heat;
} HealthAllData;


extern HealthAllData g_healthData;
#define FILE_NAME_LENGTH 50
#define LIST_HEALTH_MAX 20

class HealthModel {
public:
    HealthModel();
    virtual ~HealthModel();
    static HealthModel *GetInstance(void);
    int SaveGlobalHealthToFile(void);
    int GetHealthGlobalData(DataDistribute type, uint32 *value);
    bool InitHealthData(void);
    bool GetHealthFrontData(HealthMsg *healthMsg);
    void HealthPopFront(void);
    bool FetchedHealthData(HealthMsg *healthMsg);
    void PushHealthData(HealthMsg healthMsg);
// drop down status
    int GetBriScrStatus(void);
    int GetDntDistStatus(void);
    void UpdateBriScrStatus(uint16_t status);
    void UpdateDntDistStatus(uint16_t status);
private:
    bool SetUpListCase(void);
    List<HealthMsg> listHealth;
    uint32 listHealthNum = 0;
    int briScreenStatus{0};
    int dntDisturbStatus{0};
};
}

#endif // HEALTH_MODEL_H
