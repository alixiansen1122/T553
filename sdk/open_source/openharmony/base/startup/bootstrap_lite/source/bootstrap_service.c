/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "bootstrap_service.h"
#include <ohos_init.h>
#include <cmsis_os.h>
#include "common.h"
#include "samgr_lite.h"
#include "broadcast.h"
#include "service_id_define.h"

#define DEFAULT_INIT_TIME 1000
#define DEFAULT_TIMES 5
enum BootstrapInner {
    BOOT_INIT_TIMEOUT = BOOTSTRAP_BUTT,
    BOOT_INNER_BUTT
};

typedef struct Bootstrap {
    INHERIT_SERVICE;
    Identity identity;
    Broadcast* broadcast;
    osTimerId_t timerId;
    uint16 times;
    void (*timeout)(void);
} Bootstrap;

static const char* GetName(Service* service);
static BOOL Initialize(Service* service, Identity identity);
static TaskConfig GetTaskConfig(Service* service);
static BOOL MessageHandle(Service* service, Request* request);
static void OnInitTimeout(void* args);
static void Init(void)
{
    static Bootstrap bootstrap;
    bootstrap.GetName = GetName;
    bootstrap.Initialize = Initialize;
    bootstrap.MessageHandle = MessageHandle;
    bootstrap.GetTaskConfig = GetTaskConfig;
    bootstrap.timerId = NULL;
    bootstrap.times = 0;
    bootstrap.timeout = NULL;
    SAMGR_GetInstance()->RegisterService((Service*)&bootstrap);
}
SYS_SERVICE_INIT_PRI(Init, 0);

static const char *GetName(Service *service)
{
    (void)service;
    return BOOTSTRAP_SERVICE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    Bootstrap *bootstrap = (Bootstrap *)service;
    bootstrap->identity = identity;
    bootstrap->timerId = osTimerNew(OnInitTimeout, osTimerPeriodic, service, NULL);
    IUnknown* iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(BROADCAST_SERVICE);
    if (iUnknown == NULL) {
        return FALSE;
    }
    iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void**)&bootstrap->broadcast);
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *request)
{
    Bootstrap* bootstrap = (Bootstrap*)service;
    if (request == NULL || request->msgId >= BOOT_INNER_BUTT) {
        SAMGR_LOG_ERROR("Bootstrap MessageHandle param erro\n");
        return FALSE;
    }
    switch (request->msgId) {
        case BOOT_SYS_COMPLETED:
            INIT_MODULE_CALL(app, service);
            INIT_MODULE_CALL(app, feature);
            break;
        case BOOT_APP_COMPLETED:
            break;
        default:
            break;
    }
    Identity identity = {bootstrap->identity.serviceId, bootstrap->identity.featureId, NULL};
    (void)SAMGR_SendResponseByIdentity(&identity, request, NULL);
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    // The bootstrap service uses a stack of 2 KB (0x2000) in size and a queue of 20 elements.
    // You can adjust it according to the actual situation.
    TaskConfig config = {LEVEL_HIGH, PRI_ABOVE_NORMAL, 0x2000, 20, SHARED_TASK};
    return config;
}

static void OnInitTimeout(void* args)
{
    Bootstrap* bootstrap = (Bootstrap*)args;
    if (args == NULL) {
        return;
    }

    Request request = {.msgId = BOOT_INIT_TIMEOUT, .msgValue = 0};

    SAMGR_SendRequest(&bootstrap->identity, &request, NULL);
}

