/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include "broadcast_feature.h"
#include "cores_types.h"
#include "samgr_lite.h"
#include "securec.h"
#include "wearable_log.h"
#include "broadcast_service.h"
#include "broadcast_implement.h"

typedef struct BroadcastFeature {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(BroadcastApi);
    TopicSubscriberInfo topicSubscriberInfo;
} BroadcastFeature;

static const char *GetFeatureName(Feature *feature_);
static void OnInitialize(Feature *feature_, Service *parent, Identity id);
static void OnStop(Feature *feature_, Identity id);
static BOOL OnMessage(Feature *feature_, Request *req);

static int32 SubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);
static int32 UnsubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);
static int32 GetTopicBitmap(IUnknown *broadcastApi, const Topic topic, uint32 *bitmaps, int16 size);
static int32 PublishTopic(IUnknown *broadcastApi, Request *msg);
static int32 HandleTopicMessage(BroadcastFeature *broadcast, Request *request);

static BroadcastFeature broadcastFeature = {
    .GetName = GetFeatureName,
    .OnInitialize = OnInitialize,
    .OnStop = OnStop,
    .OnMessage = OnMessage,
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .SubscribeTopic = SubscribeTopic,
    .UnsubscribeTopic = UnsubscribeTopic,
    .GetTopicBitmap = GetTopicBitmap,
    .PublishTopic = PublishTopic,
    DEFAULT_IUNKNOWN_ENTRY_END
};

void BroadcastFeatureLog(IUnknown *broadcastApi)
{
    BroadcastFeature *broadcast =  GET_OBJECT(broadcastApi, BroadcastFeature, iUnknown);
    TopicSubscriberInfoLog(&(broadcast->topicSubscriberInfo));
}

void InitBroadcastFeature(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "InitBroadcastFeature start!");
    BroadcastFeature *feature = &broadcastFeature;
    InitTopicSubscriberInfoImpl(&(feature->topicSubscriberInfo));
    SAMGR_GetInstance()->RegisterFeature(BROADCAST_SERVICE_NAME, (Feature *)feature);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(BROADCAST_SERVICE_NAME, GET_IUNKNOWN(broadcastFeature));
    SAMGR_GetInstance()->RegisterFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME,
        GET_IUNKNOWN(broadcastFeature));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "InitBroadcastFeature end!");
}

static const char *GetFeatureName(Feature *feature)
{
    (void)feature;
    return BROADCAST_FEATURE_NAME;
}

static void OnInitialize(Feature *feature, Service *parent, Identity identity)
{
    (void)feature;
    (void)parent;
    (void)identity;
    BroadcastFeature *f = (BroadcastFeature *)feature;
    Identity *id = &(f->id);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "BroadcastFeature featureId=%u, queueId=%u, serviceId=%u",
                  id->featureId, id->queueId, id->serviceId);
}

static void OnStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
}

static BOOL OnMessage(Feature *feature, Request *req)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "BroadcastFeature req=0x%p, msgId=%u, len=%u, data=0x%p",
                  req, req->msgId, req->len, req->data);
    HandleTopicMessage((BroadcastFeature *)feature, req);
    return TRUE;
}

static int32 SubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber)
{
    BroadcastFeature *broadcast =  GET_OBJECT(broadcastApi, BroadcastFeature, iUnknown);
    return SubscribeTopicImpl(&(broadcast->topicSubscriberInfo), topic, subscriber);
}

static int32 UnsubscribeTopic(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber)
{
    BroadcastFeature *broadcast =  GET_OBJECT(broadcastApi, BroadcastFeature, iUnknown);
    return UnsubscribeTopicsImpl(&(broadcast->topicSubscriberInfo), topic, subscriber);
}

static int32 GetTopicBitmap(IUnknown *broadcastApi, const Topic topic, uint32 *bitmaps, int16 size)
{
    BroadcastFeature *broadcast =  GET_OBJECT(broadcastApi, BroadcastFeature, iUnknown);
    return GetTopicBitmapsImpl(&(broadcast->topicSubscriberInfo), topic, bitmaps, size);
}

static int32 PublishTopic(IUnknown *broadcastApi, Request *msg)
{
    BroadcastFeature *broadcast =  GET_OBJECT(broadcastApi, BroadcastFeature, iUnknown);
    return PublishTopicMsgImpl(&(broadcast->topicSubscriberInfo), msg);
}

/* 处理其他核发送过来的消息 */
static int32 HandleTopicMessage(BroadcastFeature *broadcast, Request *request)
{
    return HandleTopicMsgImpl(&(broadcast->topicSubscriberInfo), request);
}
