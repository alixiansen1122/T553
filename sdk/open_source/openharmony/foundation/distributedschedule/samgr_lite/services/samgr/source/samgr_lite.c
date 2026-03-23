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
#include "samgr_lite_inner.h"
#include <string.h>
#include <stdint.h>
#include <securec.h>
#include <ohos_errno.h>
#include "memory_adapter.h"
#include "time_adapter.h"
#include "thread_adapter.h"
#include "service_impl.h"
#include "service_id_define.h"
#include "dependency.h"

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "Samgr"
#define LOG_DOMAIN (0xD001800U)

#define ASYN_INIT 0
#define SYNC_INIT 1
/* **************************************************************************************************
 * Samgr Lite local Definitions
 * ************************************************************************************************* */
#define BOOT_STEP (0x1U)
#define TO_NEXT_STATUS(status) (BootStatus)((uint8)(status) | BOOT_STEP)
#define WDG_SVC_BOOT_TIME (5000U)
/* **************************************************************************************************
 * Samgr Lite location functions
 * ************************************************************************************************* */
static boolean HasUnInitService(SamgrLiteImpl *manager);
static Vector GetUnInitServices(SamgrLiteImpl *manager);
static int32 BootNextStep(SamgrLiteImpl* manager);
static void InitializeSingleService(ServiceImpl *impl, int32 *remain);
static void AddTaskPool(ServiceImpl* service, TaskConfig* cfg, const char* name);
static TaskPool* GetSpecifiedTaskPool(TaskConfig* config);
static void HandleInitRequest(const Request* request, const Response* response);
static int32 SendBootRequest(int16 msgId, uint32 value, void* data, Handler callback);
/* **************************************************************************************************
 * Samgr Lite location structure and local variable
 * ************************************************************************************************* */
static SamgrLiteImpl g_samgrImpl;
/* **************************************************************************************************
 * Samgr Lite local funtion
 * ************************************************************************************************* */
SamgrLiteImpl *GetImplement(void) {
    return &g_samgrImpl;
}

static boolean HasUnInitService(SamgrLiteImpl *manager) {
    if (manager == NULL) {
        return FALSE;
    }
    int16 i = 0;
    int16 size = VECTOR_Size(&(manager->services));
    for (i = 0; i < size; i++) {
        ServiceImpl *serviceImpl = (ServiceImpl *)VECTOR_At(&(manager->services), i);
        if ((serviceImpl != NULL) && (serviceImpl->inited < SVC_IDLE)) {
            return TRUE;
        }
    }
    return FALSE;
}

static Vector GetUnInitServices(SamgrLiteImpl *manager) {
    Vector serviceList = VECTOR_Make(NULL, NULL);
    if (manager == NULL) {
        return serviceList;
    }

    int16 size = VECTOR_Size(&(manager->services));
    int16 i = 0;
    for (i = 0; i < size; i++) {
        ServiceImpl *serviceImpl = (ServiceImpl *)VECTOR_At(&(manager->services), i);
        if (serviceImpl == NULL || serviceImpl->inited != SVC_INIT) {
            continue;
        }

        SAMGR_LOG_INFO("add service:%s to List \n", serviceImpl->service->GetName(serviceImpl->service));
        VECTOR_Add(&serviceList, serviceImpl);
    }
    return serviceList;
}

static int32 BootNextStep(SamgrLiteImpl* manager) {
    if (manager->status < BOOT_DYNAMIC_WAIT) {
        MUTEX_Lock(manager->mutex);
        manager->status++;
        MUTEX_Unlock(manager->mutex);
    }

    SAMGR_LOG_INFO("boot next step: %d \n", manager->status);
    if (manager->status == BOOT_APP) {
        return SendBootRequest(BOOT_SYS_COMPLETED, 0, NULL, (Handler)SAMGR_Bootstrap);
    } else if(manager->status == BOOT_DYNAMIC) {
        return SendBootRequest(BOOT_APP_COMPLETED, 0, NULL, (Handler)SAMGR_Bootstrap);
    }
    return EC_SUCCESS;
}

static void InitializeSingleService(ServiceImpl *impl, int32 *remain) {
    impl->inited = SVC_WAIT;
    if (impl->taskPool == NULL) {
        Request request = {SYNC_INIT, 0, *(uint32*)remain, impl};
        HandleInitRequest(&request, NULL);
    } else {
        Identity identity = {impl->serviceId, INVALID_INDEX, impl->taskPool->queueId};
        Request request = {ASYN_INIT, 0, *(uint32*)remain, impl};

        uint32 *ref = NULL;
        (void)SAMGR_SendSharedDirectRequest(&identity, &request, NULL, &ref, HandleInitRequest);
    }
}

static void InitializeServices(Vector *services, int32 *remain) {
    int16 size = VECTOR_Size(services);
    SamgrLiteImpl *samgr = GetImplement();

    int16 i = 0;
    for (i = 0; i < size; ++i) {
        ServiceImpl *serviceImpl = (ServiceImpl *)VECTOR_At(services, i);
        if (serviceImpl == NULL) {
            continue;
        }
        TaskConfig config = serviceImpl->service->GetTaskConfig(serviceImpl->service);
        const char *name = serviceImpl->service->GetName(serviceImpl->service);
        if (config.taskFlags != NO_TASK) {
            AddTaskPool(serviceImpl, &config, name);
        }
        InitializeSingleService(serviceImpl, remain);
    }

    MUTEX_Lock(samgr->mutex);
    for (i = 0; i < size; ++i) {
        ServiceImpl *serviceImpl = (ServiceImpl *)VECTOR_At(services, i);
        if (serviceImpl == NULL) {
            continue;
        }
        const char *name = serviceImpl->service->GetName(serviceImpl->service);
        SAMGR_StartTaskPool(serviceImpl->taskPool, name);
    }
    MUTEX_Unlock(samgr->mutex);
}

static void AddTaskPool(ServiceImpl* service, TaskConfig* cfg, const char* name) {
    if (cfg->priority < PRI_LOW || cfg->priority >= PRI_BUTT) {
        cfg->priority = PRI_LOW;
    }

    switch (cfg->taskFlags) {
        case SHARED_TASK: {
                int pos = (int)cfg->priority / PROPERTY_STEP;
                SamgrLiteImpl* samgr = GetImplement();
                if (samgr->sharedPool[pos] == NULL) {
                    TaskConfig shareCfg = DEFAULT_TASK_CFG(pos);
                    samgr->sharedPool[pos] = SAMGR_CreateFixedTaskPool(&shareCfg, name, DEFAULT_SIZE);
                }
                service->taskPool = samgr->sharedPool[pos];
                if (SAMGR_ReferenceTaskPool(service->taskPool) == NULL) {
                    samgr->sharedPool[pos] = NULL;
                }
            }
            break;

        case SPECIFIED_TASK:
            service->taskPool = GetSpecifiedTaskPool(cfg);
            if (service->taskPool != NULL) {
                break;
            }
            // fallthrough
        case SINGLE_TASK:
            service->taskPool = SAMGR_CreateFixedTaskPool(cfg, name, SINGLE_SIZE);
            break;
        default:
            SAMGR_LOG_INFO("service maybe config NO_TASK \n");
            break;
    }
}

static TaskPool* GetSpecifiedTaskPool(TaskConfig* config) {
    SamgrLiteImpl* samgr = GetImplement();
    Vector* services = &(samgr->services);
    int16 serviceNum = VECTOR_Size(services);
    int i;
    for (i = 0; i < serviceNum; ++i) {
        ServiceImpl *impl = VECTOR_At(services, i);
        if (impl == NULL) {
            continue;
        }

        TaskConfig cfg = impl->service->GetTaskConfig(impl->service);
        if (memcmp(&cfg, config, sizeof(TaskConfig)) != 0) {
            continue;
        }

        if (impl->taskPool == NULL) {
            break;
        }

        TaskPool *taskPool = SAMGR_ReferenceTaskPool(impl->taskPool);
        if (taskPool != NULL) {
            return taskPool;
        }
    }
    return NULL;
}

static void HandleInitRequest(const Request* request, const Response* response) {
    ServiceImpl* service = (ServiceImpl*)request->data;
    if (service == NULL) {
        SAMGR_LOG_ERROR("Init service Request:<%d,%d>, Response:<%p,%d>!",
            request->msgId, request->msgValue, response->data, response->len);
        return;
    }
    DEFAULT_Initialize(service);
    uint64 endTime = SAMGR_GetProcessTime();
    SamgrLiteImpl* manager = GetImplement();
    MutexId mutex = manager->mutex;

    MUTEX_Lock(mutex);
    boolean isFinished = FALSE;

    service->ops.timestamp = endTime;
    service->inited = SVC_IDLE;
    isFinished = !HasUnInitService(manager);

    if (isFinished) {
        SAMGR_LOG_INFO("current boot stage remain %d services \n", request->msgValue);
        ((request->msgValue) > 0) ? BootServices(manager) : BootNextStep(manager);
    }
    MUTEX_Unlock(mutex);
}

static int32 SendBootRequest(int16 msgId, uint32 value, void* data, Handler callback) {
    Request request = {.msgId = msgId, .len = 0, .msgValue = value, .data = data};
    if (data != NULL) {
        uintptr_t *copy = (uintptr_t *)SAMGR_Malloc(sizeof(uintptr_t));
        *copy = (uintptr_t)data;
        request.data = (void *)copy;
        request.len = sizeof(uintptr_t);
    }
    SamgrLiteImpl* samgr = GetImplement();
    int16 index = VECTOR_FindByKey(&(samgr->services), (void*)BOOTSTRAP_SERVICE);
    if (index == INVALID_INDEX) {
        if (callback != NULL) {
            MUTEX_Unlock(samgr->mutex);
            callback(&request, NULL);
            MUTEX_Lock(samgr->mutex);
        }
        return EC_SUCCESS;
    }
    Identity id = DEFAULT_GetFeatureId(VECTOR_At(&(samgr->services), index), NULL);
    SAMGR_LOG_INFO("In SendBootRequest,msgId:%d \n", request.msgId);
    return SAMGR_SendRequest(&id, &request, callback);
}

int32 BootServices(SamgrLiteImpl* manager) {
    Vector vector = GetUnInitServices(manager);
    int32 remain = GetHighPriorityServices(&vector);
	int16 size = VECTOR_Num(&vector);
    if (size == 0) {
        return EC_NOSERVICE;
    }
	InitializeServices(&vector, &remain);
    VECTOR_Clear(&vector);
    return EC_SUCCESS;
}

SamgrLite *SAMGR_GetInstance(void) {
    if (g_samgrImpl.mutex == NULL) {
        SAMGR_Init(&g_samgrImpl);
    }

//xx00    if (!osal_mutex_is_locked(&(g_samgrImpl.mutex))) {
//xx00        SAMGR_Init(&g_samgrImpl);
//xx00    }
	return &(GetImplement()->vtbl);
}

void SAMGR_Bootstrap(void) {
    SAMGR_LOG_INFO("SAMGR_Bootstrap start \n");
    SamgrLiteImpl* samgr = GetImplement();
    if (samgr->mutex == NULL) {
        return;
    }

    WDT_Reset(WDG_SVC_BOOT_TIME);
    MUTEX_Lock(samgr->mutex);
    samgr->status = TO_NEXT_STATUS(samgr->status);
    SAMGR_LOG_INFO("current boot status: %d \n", samgr->status);
    if ((samgr->status >= BOOT_DYNAMIC) && (!HasUnInitService(samgr))) {
        MUTEX_Unlock(samgr->mutex);
        return;
    }

    BootServices(samgr);
    MUTEX_Unlock(samgr->mutex);
}

ServiceImpl* SAMGR_GetServiceByID(int16 serviceId) {
    SamgrLiteImpl* manager = GetImplement();
    MUTEX_Lock(manager->mutex);
    ServiceImpl* serviceImpl = (ServiceImpl*)VECTOR_At(&(manager->services), serviceId);
    MUTEX_Unlock(manager->mutex);
    return serviceImpl;
}
