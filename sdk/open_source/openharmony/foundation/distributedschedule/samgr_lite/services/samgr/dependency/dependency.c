/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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
#include "dependency.h"

static const ServicePriority g_servicePriority[] = {
    { "Bootstrap",  0 },
    { "BROADCAST",  0 },
    { "commu",      0 },
    { "Input",      1 },
    { "MISCS",      1 },
    { "Display",    1 },
    { "SensorMgr",  1 },
    { "CHARGE",     1 },
    { "AppService", 1 },
};

uint32 GetHighPriorityServices(Vector *vec) {
    uint32 len = sizeof(g_servicePriority) / sizeof(ServicePriority);
    uint32 i = 0;
    uint8 priority = 0xFFU;
    int32 num = VECTOR_Num(vec);
    int32 remain = 0;

    for (i = 0; i < len && num > 0; ++i) {
        int16 id = VECTOR_FindByKey(vec, g_servicePriority[i].service);
        if (id == INVALID_INDEX) {
            continue;
        }

        num--;
        if (priority >= g_servicePriority[i].priority) {
            priority = g_servicePriority[i].priority;
            continue;
        }
        remain++;
        VECTOR_Swap(vec, id, NULL);
    }
    return remain;
}

