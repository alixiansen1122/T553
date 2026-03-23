/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "sensor_service.h"

#include <ohos_init.h>
#include "feature.h"
#include "iproxy_server.h"
#include "samgr_lite.h"
#include "sensor_service_impl.h"
#include "service.h"

int32_t Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if ((iProxy == NULL) || (req == NULL) || (reply == NULL)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s iProxy or req or reply is NULL",
            SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_ERROR_INVALID_PARAM;
}

static SensorService g_sensorService = {
    .GetName = SENSOR_GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    .GetAllSensors = GetAllSensorsImpl,
    .ActivateSensor = ActivateSensorImpl,
    .DeactivateSensor = DeactivateSensorImpl,
    .SetBatch = SetBatchImpl,
    .SubscribeSensor = SubscribeSensorImpl,
    .UnsubscribeSensor = UnsubscribeSensorImpl,
    .SetMode = SetModeImpl,
    .SetOption = SetOptionImpl,
// ohos-hisi begin
    .GetData = GetSensorDataImpl,
// ohos-hisi end
    IPROXY_END,
};

static void Init(void)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    SAMGR_GetInstance()->RegisterService((Service *)&g_sensorService);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(SENSOR_SERVICE, GET_IUNKNOWN(g_sensorService));
}

SYSEX_SERVICE_INIT(Init);