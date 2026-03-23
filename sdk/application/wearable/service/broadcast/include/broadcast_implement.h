/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature implement api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef SVR_BROADCAST_IMPLEMENT_H
#define SVR_BROADCAST_IMPLEMENT_H

#include "ohos_types.h"
#include "cores_types.h"
#include "broadcast_feature.h"
#include "broadcast_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CORE_MASK(core) (0x1 << (core))
#define PLATFORM_CORE_MASK (0x1 << PLATFORM_CORE)
typedef enum CoreMask {
    A_CORE_MASK = 0x1 << PLATFORM_CORE,
} CoreMask;

typedef struct TopicToCore {
    Topic topic;        /* 对应topic，从10000开始 */
    uint8_t coreMask;     /* 对应topic，需要发往的核 */
    uint8_t coreCount;    /* 需要发往的核个数 */
} TopicToCore;

typedef struct TopicToCoreTable {
    uint16_t count;
    uint16_t capSize;
    const TopicToCore *topics;
    uint32 *bitmap;
} TopicToCoreTable;

typedef struct SubscriberTable {
    uint16_t count;
    uint16_t capSize;
    Subscriber *subscribers;
} SubscriberTable;

typedef struct TopicSubscriberInfo {
    TopicToCoreTable topicTable;
    SubscriberTable subscriberTable;
    MutexId mutex;
} TopicSubscriberInfo;

void TopicSubscriberInfoLog(TopicSubscriberInfo *topicSubscriberInfo);
int32_t InitTopicSubscriberInfoImpl(TopicSubscriberInfo *topicSubscriberInfo);
int32_t SubscribeTopicImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber* subscriber);
int32_t UnsubscribeTopicsImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber* subscriber);
int32_t GetTopicBitmapsImpl(const TopicSubscriberInfo *topicSubscriberInfo, Topic topic, uint32 *bitmaps, int16 size);
int32_t PublishTopicImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req);
int32_t HandleTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req);
int32_t PublishTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* SVR_BROADCAST_IMPLEMENT_H */
