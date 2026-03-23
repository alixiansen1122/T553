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

#include "main/ModelListenerSample.h"

#include "wearable_log.h"
namespace OHOS {
ModelListenerSample::ModelListenerSample()
{
    Init();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ModelListenerSample::ModelListenerSample");
}
ModelListenerSample::~ModelListenerSample()
{
    Deinit();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ModelListenerSample::~ModelListenerSample");
}

void ModelListenerSample::Callback()
{
    Notify();
    OnTickEvent();
}
}