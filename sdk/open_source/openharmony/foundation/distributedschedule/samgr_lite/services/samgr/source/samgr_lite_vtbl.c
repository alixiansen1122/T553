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
#include "samgr_lite_inner.h"
#include <service_registry.h>
#include <string.h>
#include "service_impl.h"
#include "memory_adapter.h"
#include "time_adapter.h"
#include "service_id_define.h"
#include <securec.h>

/* **************************************************************************************************
 * Samgr Lite public interfaces
 * ************************************************************************************************* */
static ServiceImpl* GetService(const char* name);
static BOOL IsInvalidService(Service *service);
static BOOL RegisterService(Service* service);
static Service* UnregisterService(const char* name);
static BOOL RegisterFeature(const char* serviceName, Feature* feature);
static Feature* UnregisterFeature(const char* serviceName, const char* featureName);
static BOOL RegisterFeatureApi(const char* serviceName, const char* feature, IUnknown* publicApi);
static IUnknown* UnregisterFeatureApi(const char* serviceName, const char* feature);
static BOOL RegisterDefaultFeatureApi(const char* serviceName, IUnknown* publicApi);
static IUnknown* UnregisterDefaultFeatureApi(const char* serviceName);
static IUnknown* GetDefaultFeatureApi(const char* serviceName);
static IUnknown* GetFeatureApi(const char* serviceName, const char* feature);
static int32 AddSystemCapability(const char *sysCap);
static BOOL HasSystemCapability(const char *sysCap);
static int32 GetSystemAvailableCapabilities(
    char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN], int32 *size);
static const char *GetServiceName(const ServiceImpl *serviceImpl);
static IUnknown *GetDefaultFeatureApiById(uint16 serviceId);
/* **************************************************************************************************
 * Samgr Lite location functions
 * ************************************************************************************************* */
static ServiceImpl* GetService(const char* name) {
    if (name == NULL) {
        return NULL;
    }

    SamgrLiteImpl* manager = GetImplement();
    MUTEX_Lock(manager->mutex);
    Vector* services = &(manager->services);
    short pos = VECTOR_FindByKey(services, (void*)name);
    if (pos < 0) {
        MUTEX_Unlock(manager->mutex);
        return NULL;
    }
    ServiceImpl* serviceImpl = (ServiceImpl*)VECTOR_At(services, pos);
    MUTEX_Unlock(manager->mutex);
    return serviceImpl;
}

static BOOL IsInvalidService(Service *service) {
    return (service == NULL || service->GetName == NULL || service->Initialize == NULL ||
            service->GetTaskConfig == NULL || service->MessageHandle == NULL);
}

static BOOL RegisterService(Service* service) {
    if (IsInvalidService(service)) {
        return FALSE;
    }

    SamgrLiteImpl* samgr = GetImplement();
    MUTEX_Lock(samgr->mutex);
    int16 pos = VECTOR_FindByKey(&(samgr->services), (void*)service->GetName(service));
    if (pos >= 0) {
        MUTEX_Unlock(samgr->mutex);
        return FALSE;
    }

    if (VECTOR_Num(&(samgr->services)) >= MAX_SERVICE_NUM) {
        MUTEX_Unlock(samgr->mutex);
        return FALSE;
    }

    ServiceImpl* serviceImpl = SAMGR_CreateServiceImpl(service, samgr->status);
    if (serviceImpl == NULL) {
        MUTEX_Unlock(samgr->mutex);
        return FALSE;
    }
    serviceImpl->serviceId = VECTOR_Add(&(samgr->services), serviceImpl);
    MUTEX_Unlock(samgr->mutex);

    if (serviceImpl->serviceId == INVALID_INDEX) {
        SAMGR_LOG_ERROR("RegisterService %s Failed\n",
            service->GetName(service));
        SAMGR_Free(serviceImpl);
        return FALSE;
    }
    return TRUE;
}

static Service* UnregisterService(const char* name) {
    if (name == NULL) {
        return NULL;
    }

    SamgrLiteImpl* samgr = GetImplement();
    MUTEX_Lock(samgr->mutex);
    Vector* services = &(samgr->services);
    int16 pos = VECTOR_FindByKey(services, (void*)name);
    ServiceImpl* serviceImpl = (ServiceImpl*)VECTOR_At(services, pos);
    if (serviceImpl == NULL || serviceImpl->defaultApi != NULL ||
        VECTOR_Num(&serviceImpl->features) > 0) {
        MUTEX_Unlock(samgr->mutex);
        return NULL;
    }

    MUTEX_Unlock(samgr->mutex);
    DEFAULT_StopService(serviceImpl);
    MUTEX_Lock(samgr->mutex);

    VECTOR_Swap(services, pos, NULL);
    Service* service = serviceImpl->service;
    VECTOR_Clear(&serviceImpl->features);
    SAMGR_Free(serviceImpl->defaultApi);
    serviceImpl->inited = SVC_DEL;

    SAMGR_ReleaseTaskPool(serviceImpl->taskPool);
    MUTEX_Unlock(samgr->mutex);
    SAMGR_Free(serviceImpl);
    return service;
}

static BOOL RegisterFeature(const char* serviceName, Feature* feature) {
    if (IsInvalidFeature(feature)) {
        return FALSE;
    }

    ServiceImpl* serviceImpl = GetService(serviceName);
    if (serviceImpl == NULL || serviceImpl->inited != SVC_INIT) {
        return FALSE;
    }

    if (DEFAULT_GetFeature(serviceImpl, feature->GetName(feature)) != NULL) {
        return FALSE;
    }

    int16 featureId = DEFAULT_AddFeature(serviceImpl, feature);
    return (featureId >= 0);
}

static Feature* UnregisterFeature(const char* serviceName, const char* featureName) {
    ServiceImpl *serviceImpl = GetService(serviceName);
    if (serviceImpl == NULL) {
        return NULL;
    }
    return DEFAULT_DeleteFeature(serviceImpl, featureName);
}

static BOOL RegisterFeatureApi(const char* serviceName, const char* feature, IUnknown* publicApi) {
    if (IsInvalidIUnknown(publicApi)) {
        return FALSE;
    }

    ServiceImpl *serviceImpl = GetService(serviceName);
    if (serviceImpl == NULL) {
        return FALSE;
    }

    if (feature == NULL) {
        if (serviceImpl->defaultApi != NULL) {
            return FALSE;
        }
        serviceImpl->defaultApi = publicApi;
        return TRUE;
    }

    FeatureImpl *featureImpl = DEFAULT_GetFeature(serviceImpl, feature);
    if (featureImpl == NULL) {
        return FALSE;
    }
    return SAMGR_AddInterface(featureImpl, publicApi);
}

static IUnknown* UnregisterFeatureApi(const char* serviceName, const char* feature) {
    ServiceImpl *serviceImpl = GetService(serviceName);
    if (serviceImpl == NULL) {
        return NULL;
    }

    if (feature == NULL) {
        IUnknown *iUnknown = serviceImpl->defaultApi;
        serviceImpl->defaultApi = NULL;
        return iUnknown;
    }
    return SAMGR_DelInterface(DEFAULT_GetFeature(serviceImpl, feature));
}

static BOOL RegisterDefaultFeatureApi(const char* serviceName, IUnknown* publicApi) {
    return RegisterFeatureApi(serviceName, NULL, publicApi);
}

static IUnknown* UnregisterDefaultFeatureApi(const char* serviceName) {
    return UnregisterFeatureApi(serviceName, NULL);
}

static IUnknown* GetDefaultFeatureApi(const char* serviceName) {
    return GetFeatureApi(serviceName, NULL);
}

static IUnknown* GetFeatureApi(const char* serviceName, const char* feature) {
    ServiceImpl* serviceImpl = GetService(serviceName);
    if (serviceImpl == NULL) {
        return SAMGR_FindServiceApi(serviceName, feature);
    }
    FeatureImpl *featureImpl = DEFAULT_GetFeature(serviceImpl, feature);
    if (featureImpl == NULL && feature == NULL) {
        return serviceImpl->defaultApi;
    }
    return SAMGR_GetInterface(featureImpl);
}

static int32 AddSystemCapability(const char *sysCap) {
    if (sysCap == NULL || strlen(sysCap) == 0 || strlen(sysCap) > MAX_SYSCAP_NAME_LEN) {
        return EC_INVALID;
    }
    return SAMGR_RegisterSystemCapabilityApi(sysCap, TRUE);
}

static BOOL HasSystemCapability(const char *sysCap) {
    if (sysCap == NULL || strlen(sysCap) == 0 || strlen(sysCap) > MAX_SYSCAP_NAME_LEN) {
        return FALSE;
    }
    return SAMGR_QuerySystemCapabilityApi(sysCap);
}

static int32 GetSystemAvailableCapabilities(
    char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN], int32 *sysCapNum) {
    if (sysCaps == NULL || sysCapNum == NULL) {
        return EC_INVALID;
    }
    return SAMGR_GetSystemCapabilitiesApi(sysCaps, sysCapNum);
}

static const char *GetServiceName(const ServiceImpl *serviceImpl) {
    if (serviceImpl == NULL) {
        return NULL;
    }
    return serviceImpl->service->GetName(serviceImpl->service);
}

static IUnknown *GetDefaultFeatureApiById(uint16 serviceId) {
    SamgrLiteImpl* samgr = GetImplement();
    MUTEX_Lock(samgr->mutex);
    ServiceImpl* serviceImpl = (ServiceImpl*)VECTOR_At(&(samgr->services), serviceId);
    MUTEX_Unlock(samgr->mutex);
    const char *serviceName = GetServiceName(serviceImpl);
    return GetFeatureApi(serviceName, NULL);
}

void SAMGR_Init(SamgrLiteImpl *samgr) {
    WDT_Start(WDG_SAMGR_INIT_TIME);
    samgr->vtbl.RegisterService = RegisterService;
    samgr->vtbl.UnregisterService = UnregisterService;
    samgr->vtbl.RegisterFeature = RegisterFeature;
    samgr->vtbl.UnregisterFeature = UnregisterFeature;
    samgr->vtbl.RegisterFeatureApi = RegisterFeatureApi;
    samgr->vtbl.UnregisterFeatureApi = UnregisterFeatureApi;
    samgr->vtbl.RegisterDefaultFeatureApi = RegisterDefaultFeatureApi;
    samgr->vtbl.UnregisterDefaultFeatureApi = UnregisterDefaultFeatureApi;
    samgr->vtbl.GetDefaultFeatureApi = GetDefaultFeatureApi;
    samgr->vtbl.GetFeatureApi = GetFeatureApi;
    samgr->vtbl.AddSystemCapability = AddSystemCapability;
    samgr->vtbl.HasSystemCapability = HasSystemCapability;
    samgr->vtbl.GetSystemAvailableCapabilities = GetSystemAvailableCapabilities;
    samgr->vtbl.GetDefaultFeatureApiById = GetDefaultFeatureApiById;
    samgr->status = BOOT_SYS;
    samgr->services = VECTOR_Make((VECTOR_Key)GetServiceName, (VECTOR_Compare)strcmp);
    samgr->mutex = MUTEX_InitValue();
    (void)memset_s(samgr->sharedPool, sizeof(TaskPool *) * MAX_POOL_NUM, 0,
                   sizeof(TaskPool *) * MAX_POOL_NUM);
    WDT_Reset(WDG_SVC_REG_TIME);
}
