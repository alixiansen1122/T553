/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature implement
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include "broadcast_service.h"
#include "service_id_define.h"
#include "samgr_lite.h"
#include "wearable_log.h"
#include "broadcast_feature.h"
#include "ohos_init.h"

static const char *GetName(Service *service_);
static BOOL Initialize(Service *service_, Identity id);
static TaskConfig GetTaskConfig(Service *service_);
static BOOL MessageHandle(Service *service_, Request *req);

static BroadcastService broadcastSvr = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
};

static void InitBroadcastService(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "InitBroadcastService start!");
    SAMGR_GetInstance()->RegisterService((Service *)&broadcastSvr);
    InitBroadcastFeature();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "InitBroadcastService end!");
}

static const char *GetName(Service *service_)
{
    (void)service_;
    return BROADCAST_SERVICE_NAME;
}

static BOOL Initialize(Service *service_, Identity id)
{
    (void)service_;
    (void)id;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "BroadcastService featureId=%u, queueId=%u, serviceId=%u",
                  id.featureId, id.queueId, id.serviceId);
    return TRUE;
}

static BOOL MessageHandle(Service *service_, Request *req)
{
    (void)service_;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "BroadcastService req=0x%p, msgId=%u, len=%u, data=0x%p",
                  req, req->msgId, req->len, req->data);
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service_)
{
    (void)service_;
    TaskConfig config = {LEVEL_HIGH, PRI_ABOVE_NORMAL, 0x800, 20, SPECIFIED_TASK};
    return config;
}


SYSEX_SERVICE_INIT_PRI(InitBroadcastService, 0);
