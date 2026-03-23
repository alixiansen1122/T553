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

/**
 * @addtogroup Broadcast
 * @{
 *
 * @brief Provides data subscription and data push for services.
 *
 * With this module, the Service, Feature, or other modules can broadcast events or data. \n
 * All services that listen to these events or data can receive these broadcasts. \n
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file broadcast_intertface.h
 *
 * @brief Provides the external interfaces and basic type definitions of the broadcast service.
 *
 * The interface and type are used for subscribing to and publishing events and data. \n
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_BROADCAST_H
#define OHOS_BROADCAST_H

#include <ohos_errno.h>
#include "feature.h"
#include "iunknown.h"
#include "message.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Indicates the name of the broadcast service.
 *
 * @since 1.0
 * @version 1.0 */
#define BROADCAST_SERVICE "BRDCST"

/**
 * @brief Indicates the topic of an event or data, which is used to distinguish different types of
 * events or data.
 *
 * @since 1.0
 * @version 1.0
 */
typedef uint32 Topic;

/**
 * @brief Defines the msgId in the origin Request
 */
enum BroadcastMsgID {
    /** declare that the publisher is from the same core or process */
    BRDCST_MSG_ID_LOCAL,
    /** declare that the publisher is from the other core or process */
    BRDCST_MSG_ID_REMOTE,
    BRDCST_MSG_ID_BUTT
};

typedef struct SubscriberInfo SubscriberInfo;
typedef struct Broadcast Broadcast;

struct SubscriberInfo {
    const Identity* identity;
    void *args;
    void (*Notify)(void* args, const Topic* topic, const Request* origin);
};

struct Broadcast {
    INHERIT_IUNKNOWN;
    /**
     * @brief Synchronize subscription relationships to other processes or cores.
     *
     * Synchronize subscription relationships to other processes or cores. \n
     * Note before app service initialized, synchronize current topic's subscription relationships will do nothing . \n
     *
     * @param broadcast Indicates <b>this</b> pointer of the broadcast.
     * @param topic Indicates the pointer to the topic to be processed.
     * @param isSyncAll TRUE mains synchronize all subscription relationships.
     *                  FALSE mains synchronize current topic's subscription relationships.
     * @return EC_SUCCESS is success, others is fail.
     * @since 2.2
     * @version 1.0
     */
    int32 (*Synchronize)(Broadcast *broadcast, const Topic* topic, boolean isSyncAll);

    /**
     * @brief 订阅topic事件通知.
     *
     * 订阅topic事件通知. \n
     *
     * @param topic 当前要订阅的topic信息.
     * @param subscriber 接收通知的订阅者信息.
     * @return EC_SUCCESS is success, others is fail.
     * @since 2.2
     * @version 1.0
     */
    int32 (*Subscribe)(const Topic* topic, const SubscriberInfo* subscriber);

    /**
     * @brief 注销topic和订阅者的订阅关系.
     *
     * 注销topic和订阅者的订阅关系. \n
     *
     * @param topic 要注销订阅关系的topic信息.
     * @param subscriber 要注销订阅关系的订阅者信息.
     * @return EC_SUCCESS is success, others is fail.
     * @since 2.2
     * @version 1.0
     */
    int32 (*Unsubscribe)(const Topic* topic, const SubscriberInfo* subscriber);

    /**
     * @brief 广播topic事件.
     *
     * 广播topic事件. \n
     *
     * @param topic 需要广播的topic信息.
     * @param value topic的数据信息, 在origin的msgValue中. {@link Request}
     * @param data topic的数据信息, 在origin的data中. {@link Request}
     * @param len topic的数据长度, 在origin的len中. {@link Request}
     * @return EC_SUCCESS is success, others is fail.
     * @since 2.2
     * @version 1.0
     */
    int32 (*Publish)(const Topic* topic, uint32 value, uint8* data, int16 len);

    /**
     * @brief 广播topic事件.
     *
     * 广播topic事件. \n
     *
     * @param topic 需要广播的topic信息.
     * @param value topic的数据信息, 在origin的msgValue中. {@link Request}
     * @param data topic的数据信息, 在origin的data中. {@link Request}
     * @param len topic的数据长度, 在origin的len中. {@link Request}
     * @param release data的释放函数
     * @return EC_SUCCESS is success, others is fail.
     * @attention 如果data是动态申请的内存, 同时release为NULL，则data不会被自动释放. 需要生产者保证内存不泄漏.
     * @since 2.2
     * @version 1.0
     */
    int32 (*PublishOutsideRel)(const Topic* topic, uint32 value, uint8* data, int16 len, void (*release)(void*));

    /**
     * @brief 获取topic的订阅关系.
     *
     * 获取topic的订阅关系. \n
     *
     * @param topic 需要查询的topic信息.
     * @param bitmaps [出参]当前的topic的订阅关系
     * @param size 出参bitmaps的uint32的个数
     * @return EC_SUCCESS is success, others is fail.
     * @since 2.2
     * @version 1.0
     */
    int32 (*GetTopicBitmaps)(const Topic* topic, uint32 *bitmaps, int16 size);

    /**
     * @brief 从bitmaps中获取一个订阅者信息.
     *
     * 从bitmaps中获取一个订阅者信息. \n
     *
     * @param bitmaps [出入参]topic的订阅关系,获取成功后会将订阅者从bitmaps的位域中删除
     * @param size bitmaps的uint32的个数
     * @param info [出参]当前获取到的订阅者信息.
     * @return EC_SUCCESS is success, others is fail.
     * @attention 每获取到一个订阅者信息, 就会将订阅者信息从bitmaps中删除.
     * @since 2.2
     * @version 1.0
     */
    int32 (*GetLowestSubscriber)(uint32 *bitmaps, int16 size, SubscriberInfo *info);
};

/**
 * @brief 将topic转换为连续数组的index.
 *
 * 将topic转换为连续数组的index. \n
 * 注意: 此函数为弱函数，由具体的项目来重新实现
 *
 * @param topic 当前topic
 * @return 非负值,topic在数组中的位置; 负数,转换失败.
 * @since 2.2
 * @version 1.0
 */
int32 BRDCST_Topic2Index(const Topic *topic);

/**
 * @brief 将连续数组的index转换为topic.
 *
 * 将连续数组的index转换为topic. \n
 * 注意: 此函数为弱函数，由具体的项目来重新实现
 *
 * @param topic 当前topic
 * @return Topic信息.
 * @since 2.2
 * @version 1.0
 */
Topic BRDCST_Index2Topic(int32 index);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // OHOS_BROADCAST_H
/** @} */
