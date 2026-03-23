/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef SVR_BROADCAST_FEATURE_H
#define SVR_BROADCAST_FEATURE_H

#include "ohos_types.h"
#include "message.h"
#include "iunknown.h"
#include "feature.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef BROADCAST_FEATURE_NAME
#define BROADCAST_FEATURE_NAME "BROADCAST"
#endif

/* topic类型 */
typedef uint16 Topic;

typedef enum {
    TOPIC_START_NUM = 10000,
    TOPIC_DEMO_ACORE_MSG = TOPIC_START_NUM,
    TOPIC_DEMO_ALL_MSG,
    TOPIC_EVENT_ALARM_RING,
    TOPIC_EVENT_UTCTIME_UPDATE,
    TOPIC_EVENT_NEXTDAY_UPDATE,
    TOPIC_EVENT_WEARSTATE_UPDATE,
    TOPIC_EVENT_STEPDATA_UPDATE,
    TOPIC_EVENT_SHUTDOWN,
    TOPIC_EVENT_HEARTRATE_UPDATA,
    TOPIC_EVENT_HFP_INCOMMING,
    TOPIC_EVENT_HFP_PHONE_ACTIVE,
    TOPIC_EVENT_HFP_WATCH_ACTIVE,
    TOPIC_EVENT_HFP_FINISHED,
    TOPIC_EVENT_HFP_PHONE_TO_WATCH,
    TOPIC_EVENT_HFP_WATCH_TO_PHONE,
    TOPIC_EVENT_BLOODOXYGEN_UPDATA,
    TOPIC_EVENT_SPORT_DISTANCE,
    TOPIC_EVENT_CALENDAR_NOTIFY,
    TOPIC_END_NUM = 20000,
} TopicNum;

/* topic数量 */
#ifndef MAX_BROACCAST_TOPIC_NUMBER
#define MAX_BROACCAST_TOPIC_NUMBER 32
#endif

/* 每个topic 对应的bitmap长度 */
#ifndef MAX_BROADCAST_BITMAP_NUMBER
#define MAX_BROADCAST_BITMAP_NUMBER  64
#endif
#if (MAX_BROADCAST_BITMAP_NUMBER > 255)
#warning Not support subscriber more than 255!
#define MAX_BROADCAST_BITMAP_NUMBER  255
#endif
/* int32类型对应的位图长度 */
#define BITMAP_INT32_SIZE       32
/* 计算每个topic，bitmap需要多少int32. (采用四字节int32，相比int8，遍历时有优势) */
#define BITMAP_ARRAY_ENTRY_SIZE ((((MAX_BROADCAST_BITMAP_NUMBER) + 31) & ~31) >> 5)
/* 计算所有topic，对应int32数组长 */
#define BITMAP_ARRAY_SIZE       (BITMAP_ARRAY_ENTRY_SIZE * MAX_BROACCAST_TOPIC_NUMBER)

/**
 * @brief 订阅者信息
 */
typedef struct Subscriber Subscriber;
struct Subscriber {
    const Identity *identity; /* 唯一确定订阅者，有OS系统时，需要设置。 无OS时，可以不设置 */
    void *args; /* 回调函数时参数 */
    /**
     * @brief 信息回调函数。无OS、或者无服务的场景，必须设置该参数，需要服务处理该消息时，不能设置
     * @param subscriber Indicates <b>this</b> pointer of the subscriber.
     * @param topic Indicates the pointer to the topic to be processed.
     * @param origin publicTopic时发布的信息.
     * @since 1.0
     * @version 1.0
     */
    void (*Notify)(void *args, const Topic topic, Request* request);
};

/* 广播服务对外提供的接口 */
typedef struct BroadcastApi {
    INHERIT_IUNKNOWN; /* 继承IUNKNOWN，对外提供接口 */
    /**
     * @brief 订阅topic事件通知.
     *
     * 订阅topic事件通知. \n
     *
     * @param topic 当前要订阅的topic信息.
     * @param subscriber 接收通知的订阅者信息.
     * @return EC_SUCCESS is success, others is fail.
     */
    int32 (*SubscribeTopic)(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);

    /**
     * @brief 注销topic和订阅者的订阅关系.
     *
     * 注销topic和订阅者的订阅关系. \n
     *
     * @param topic 要注销订阅关系的topic信息.
     * @param subscriber 要注销订阅关系的订阅者信息.
     * @return EC_SUCCESS is success, others is fail.
     */
    int32 (*UnsubscribeTopic)(IUnknown *broadcastApi, const Topic topic, const Subscriber* subscriber);

    /**
     * @brief 获取topic的订阅关系.
     *
     * 获取topic的订阅关系. \n
     *
     * @param topic 需要查询的topic信息.
     * @param bitmaps [出参]当前的topic的订阅关系
     * @param size 出参bitmaps的uint32的个数
     * @return EC_SUCCESS is success, others is fail.
     */
    int32 (*GetTopicBitmap)(IUnknown *broadcastApi, const Topic topic, uint32 *bitmaps, int16 size);

    /**
     * @brief 广播topic消息.     *
     * @param data 经过共享内存，分配的request信息。其中msg_id对应topic
     * @return EC_SUCCESS is success, others is fail.
     */
    int32 (*PublishTopic)(IUnknown *broadcastApi, Request* request);
} BroadcastApi;

void BroadcastFeatureLog(IUnknown *broadcastApi);
void InitBroadcastFeature(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* SVR_BROADCAST_FEATURE_H */
