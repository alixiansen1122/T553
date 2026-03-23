
#include "broadcast_implement.h"
#include "securec.h"
#include "message.h"
#include "samgr_lite.h"
#include "thread_adapter.h"
#include "wearable_log.h"
#ifdef BROADCAST_SUPPORT_COMMU_SERVICE
#include "commu_common.h"
#include "commu_feature.h"
#include "commu_service.h"
#endif
#include "broadcast_feature.h"

#define INVALID_INDEX (-1)
#define array_size(x) (sizeof((x)) / sizeof((x)[0]))

/* 当前所有可用topic，不应该出现相同topic在不同位置 !!! 严格按照顺序填写，可以优化获取topic对应的索引 !!! */
static const TopicToCore topicToCoreArray[] = {
    {TOPIC_DEMO_ACORE_MSG, A_CORE_MASK, 1},
    {TOPIC_DEMO_ALL_MSG, A_CORE_MASK, 1},
    {TOPIC_EVENT_ALARM_RING, A_CORE_MASK, 1},
    {TOPIC_EVENT_UTCTIME_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_NEXTDAY_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_WEARSTATE_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_STEPDATA_UPDATE, A_CORE_MASK, 1},
    {TOPIC_EVENT_SHUTDOWN, A_CORE_MASK, 1},
    {TOPIC_EVENT_HEARTRATE_UPDATA, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_INCOMMING, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_PHONE_ACTIVE, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_WATCH_ACTIVE, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_FINISHED, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_PHONE_TO_WATCH, A_CORE_MASK, 1},
    {TOPIC_EVENT_HFP_WATCH_TO_PHONE, A_CORE_MASK, 1},
    {TOPIC_EVENT_BLOODOXYGEN_UPDATA, A_CORE_MASK, 1},
    {TOPIC_EVENT_SPORT_DISTANCE, A_CORE_MASK, 1},
};

/* 所有的topic，其对应的位图 */
static uint32_t topicBitmapArray[BITMAP_ARRAY_SIZE] = {0};
/* 所有的topic，其订阅者信息 */
static Subscriber subscriberArray[MAX_BROADCAST_BITMAP_NUMBER] = {};

/* 获取对应核的个数 */
inline static int8_t GetTopicCoreNum(uint16_t coreMask)
{
    int8_t count = 0;
    while (coreMask != 0) {
        coreMask &= (coreMask -1) ; // 清除最低位的1
        ++count;
    }
    return count;
}

/* 通过topic查找对应索引，当前使用遍历方式。 !!! 后续topic与topicIndex相差10000，可以直接优化!!! */
inline static int16_t GetTopicIndex(const TopicToCore *topicToCore, int16_t size, Topic topic)
{
    for (int16_t i = 0; i < size; ++i) {
        if (topicToCore[i].topic == topic) {
            return i;
        }
    }
    return INVALID_INDEX;
}

/* 通过target，查找器对应的索引。 !!!比较所有的对象，其中identity需要注意，仅比较指针!!! */
inline static int16_t GetSubscriberIndex(const Subscriber* subscribers, int size, const Subscriber* target)
{
    for (int16_t i = 0; i < size; ++i) {
        if (subscribers[i].identity == target->identity &&
            subscribers[i].args == target->args &&
            subscribers[i].Notify == target->Notify) {
            return i;
        }
    }
    return INVALID_INDEX;
}

/* 某个topic，设置其subscriber订阅 */
inline static void SetBitmap(uint32_t *bitmap, int16_t topicIndex, int16_t subIndex)
{
    int16_t offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16_t mod = subIndex % BITMAP_INT32_SIZE;
    bitmap[offset] |= (0x1 << mod);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
}

/* 某个topic，清理其subscriber订阅 */
inline static void ClearBitmap(uint32_t *bitmap, int16_t topicIndex, int16_t subIndex)
{
    int16_t offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16_t mod = subIndex % BITMAP_INT32_SIZE;
    bitmap[offset] &= ~(0x1 << mod);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
}

/* 某个topic，判断某个subscriber是否订阅 */
inline static BOOL GetBitmap(const uint32_t *bitmap, int16_t topicIndex, int16_t subIndex)
{
    int16_t offset = topicIndex * BITMAP_ARRAY_ENTRY_SIZE;
    offset += subIndex / BITMAP_INT32_SIZE;
    int16_t mod = subIndex % BITMAP_INT32_SIZE;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIndex=%u, subIndex=%u, offset=%u, mod=%u, bitmap=0x%x",
        topicIndex, subIndex, offset, mod, bitmap[offset]);
    return bitmap[offset] & (0x1 << mod);
}

/* 获取位图最小位的索引 */
inline static int16_t GetLowestBitIndex(int32_t bitmap)
{
    uint32_t index = (bitmap - 1) & (~bitmap); /* eg: bitmap is 01010000, then the index is 00001111 */
    /* 每次一半一半的1加起来 */
    /* 1 : 2个为一个单位，一个单位分半后加起来 */
    index = (index & 0x55555555) + ((index >> 1) & 0x55555555);     /* 0x55555555: 5555 is 0101010101010101 */
    /* 2 : 4个为一个单位，一个单位分半后加起来 */
    index = (index & 0x33333333) + ((index >> 2) & 0x33333333);     /* 0x33333333: 3333 is 0011001100110011 */
    /* 4 : 8个为一个单位，一个单位分半后加起来 */
    index = (index & 0x0F0F0F0F) + ((index >> 4) & 0x0F0F0F0F);     /* 0x0F0F0F0F: 0f0F is 0000111100001111 */
    /* 8 : 16个为一个单位，一个单位分半后加起来 */
    index = (index & 0x00FF00FF) + ((index >> 8) & 0x00FF00FF);     /* 0x00FF00FF: 00FF is 0000000011111111 */
    /* 16 : 32个为一个单位，一个单位分半后加起来 */
    index = (index & 0x0000FFFF) + ((index >> 16) & 0x0000FFFF);    /* 0x0000FFFF: half + half */
    return (int16_t)index;
}

/* 主题需要核内通信 */
inline static BOOL IsInnerCore(CoreMask mask)
{
    return (mask & PLATFORM_CORE_MASK);
}

/* 主题需要核间通信 */
inline static BOOL IsOuterCore(CoreMask mask)
{
    return (mask & ~PLATFORM_CORE_MASK);
}

/* 核内发送本地消息,直接通过identify发送给对方服务 */
static BOOL SendLocalMessage(const Identity *id, Request *req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "id=0x%p, serviceId=%u, featureId=%u, queueId=%u",
                  id, id->serviceId, id->featureId, id->queueId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "req=0x%p, msgId=%u, len=%u, data=0x%p",
                  req, req->msgId, req->len, req->data);
    int32_t ret = SAMGR_SendRequest(id, req, NULL);
    if (ret != OHOS_SUCCESS) {
        if (req->data != NULL) {
            free(req->data);
        }
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "SAMGR_SendRequest failed. ret=%d ", ret);
        return FALSE;
    }
    return TRUE;
}

/* 核内通信，将消息发送给通信服务转发，后者直接调用回调函数 */
static BOOL SendInnerMessage(const TopicSubscriberInfo *topicSubscriberInfo, int16_t topicIdx, Request *req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIdx=%u, req=0x%p", topicIdx, req);
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    Topic topic = topicTable->topics[topicIdx].topic;
    MUTEX_Lock(topicSubscriberInfo->mutex);
    /* 首先查看是否已经存在相同的结构 */
    int16_t offset = topicIdx * BITMAP_ARRAY_ENTRY_SIZE;
    for (int16_t i = 0; i < BITMAP_ARRAY_ENTRY_SIZE; ++i) {
        /* 使用优化的方式，遍历bitmap */
        int32_t bitmap = topicTable->bitmap[offset + i];
        while (bitmap != 0) {
            int16_t subIdx = GetLowestBitIndex(bitmap);
            subIdx += i * BITMAP_INT32_SIZE;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "i=%d, bitmap=0x%x, subIdx=%u, topicIdx=%u",
                          i, bitmap, subIdx, topicIdx);
            bitmap &= (bitmap - 1); /* 1：清楚最低位bit */

            const SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
            const Subscriber *subscriber = &(subscriberTable->subscribers[subIdx]);
            /* 非空类型，说明注册的回调函数，直接调用对应回调函数 */
            if (subscriber->Notify != NULL) {
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "subIdx=%u, topicIdx=%u, Notify=0x%p, args=0x%p",
                              subIdx, topicIdx, subscriber->Notify, subscriber->args);
                subscriber->Notify(subscriber->args, topic, req);
                continue;
            }

            /* 在当前核内给对应服务发送消息 */
            Request tmpReq = {
                .msgId =  req->msgId,
                .len = req->len,
                .msgValue = req->msgValue,
                .data = NULL,
            };
            if (tmpReq.len != 0) {
                tmpReq.data = malloc(tmpReq.len);
                if (tmpReq.data != NULL) {
                    (void)memcpy_s(tmpReq.data, tmpReq.len, req->data, req->len);
                } else {
                    break;
                }
            }
            SendLocalMessage(subscriber->identity, &tmpReq);
        }
    }
    if (req->data != NULL) {
        free(req->data);
    }
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return TRUE;
}

#ifdef BROADCAST_SUPPORT_COMMU_SERVICE
/* 核间通信 */
static int32_t SendRemoteMessage(Request *req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "req=0x%p, msgId=%u, len=%u, data=0x%p",
                  req, req->msgId, req->len, req->data);
    CommuApi *comApi = NULL;
    IUnknown *api = SAMGR_GetInstance()->GetFeatureApi(COMMU_SERVICE, COMMU_FEATURE);
    if (api == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "HandleCallback get api failed");
        SAMGR_FreeRequestMsg(req);
        return OHOS_FAILURE;
    }
    int32_t result = api->QueryInterface(api, DEFAULT_VERSION, (void **)&comApi);
    if (result != 0 || comApi == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "HandleCallback get publicApi failed");
        SAMGR_FreeRequestMsg(req);
        return FALSE;
    }
    comApi->SendMsg((IUnknown *)comApi, req);
    comApi->Release((IUnknown *)comApi);
    return TRUE;
}

/* 核间通信，将消息发送给通信服务转发，后者直接调用回调函数 */
static BOOL SendOuterMessage(const TopicSubscriberInfo *topicSubscriberInfo, int16_t topicIdx, Request *req)
{
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    uint16_t coreMask = topicTable->topics[topicIdx].coreMask;
    Topic topic = topicTable->topics[topicIdx].topic;
    BOOL isInnerCore = IsInnerCore(coreMask);
    BOOL isOuterCore = IsOuterCore(coreMask);
    uint16_t copyCount = topicTable->topics[topicIdx].coreCount; /* 发送核间消息时，需要复制信息的次数 */
    if (!isInnerCore && isOuterCore) {
        /* 仅存在核间消息时，可以减少一次复制，此时发送消息后会释放所有消息 */
        copyCount --;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIdx=%u, topic=%u, coreMask=0x%x, copyCount=%u",
                  topicIdx, topic, coreMask, copyCount);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "req=0x%p, msgId=%u, len=%u, data=0x%p",
                  req, req->msgId, req->len, req->data);

    Request *tmpReq = NULL;
    for (int8_t i = 0, j = 0; i < CORES_MAX_NUMBER_PHYSICAL; ++i) {
        /* 当前核/非目标核不处理 */
        uint16_t tmpMask = CORE_MASK(i);
        if (!(tmpMask & coreMask) || IsInnerCore(tmpMask)) {
            continue;
        }
        ++j; /* 核间个数 */
        if (j <= copyCount) {
            tmpReq = SAMGR_AllocRequestMsg(0, i, BROADCAST_SVR_ID, req->len);
            if (tmpReq == NULL) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "SAMGR_AllocRequestMsg failed! i=%u, len=%u", i, req->len);
                break;
            }
            (void)memcpy_s(tmpReq, sizeof(Request) + req->len, req, sizeof(Request) + req->len);
            if (req->len > 0) {
                tmpReq->data = ((char *)tmpReq + sizeof(Request));
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "alloc new message, tmpReq=0x%p", tmpReq);
        } else {
            /* 给核间对应服务发送消息。(共享内存时，消息分配，相当于零拷贝) zero-copy  */
            tmpReq = SAMGR_ZeroCopyMsg(req);
            if (tmpReq == NULL) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "SAMGR_AllocRequestMsg failed! i=%u, len=%u", i, req->len);
                break;
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "alloc new message, tmpReq=0x%p", tmpReq);
        }
        tmpReq->msgId = topic;  /* 消息类型为topic */
        SendRemoteMessage(tmpReq);
    }
    return TRUE;
}
#endif

#ifdef WEARABLE_LOG_DEBUG
void TopicSubscriberInfoLog(TopicSubscriberInfo *topicSubscriberInfo)
{
    TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topics number: %u", topicTable->count);
    for (uint16_t i = 0; i < topicTable->count; ++i) {
        const TopicToCore *core = &(topicTable->topics[i]);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic info: idx=%u, topic=%u, topicMask=0x%x, coreCount=%u",
            i, core->topic, core->coreMask, core->coreCount);
    }

    SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "subscribers number: %u", subscriberTable->count);
    for (uint16_t i = 0; i < subscriberTable->count; ++i) {
        const Subscriber *subscriber = &(subscriberTable->subscribers[i]);
        const Identity *identity = subscriber->identity;
        if (identity != NULL) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "serviceId=%u, featureId=%u, queueId=%u",
                identity->serviceId, identity->featureId, identity->queueId);
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "args=0x%p, notify=0x%p", subscriber->args, subscriber->Notify);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "bitmap num: %u, BITMAP_ARRAY_ENTRY_SIZE=%u",
                  topicTable->count, BITMAP_ARRAY_ENTRY_SIZE);
    for (uint16_t i = 0; i < topicTable->count; ++i) {
        int16_t offset = i * BITMAP_ARRAY_ENTRY_SIZE;
        for (uint16_t j = 0; j < BITMAP_ARRAY_ENTRY_SIZE; ++j) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "idx i=%u, j=%u, 0x%x",
                          i, j, topicTable->bitmap[offset + j]);
        }
    }
}
#endif

/* 初始化topic */
int32_t InitTopicSubscriberInfoImpl(TopicSubscriberInfo *topicSubscriberInfo)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    topicSubscriberInfo->mutex = MUTEX_InitValue();
    topicSubscriberInfo->topicTable.count = array_size(topicToCoreArray);
    topicSubscriberInfo->topicTable.capSize = array_size(topicToCoreArray);
    topicSubscriberInfo->topicTable.topics = topicToCoreArray;
    topicSubscriberInfo->topicTable.bitmap = topicBitmapArray;
    topicSubscriberInfo->subscriberTable.count = 0;
    topicSubscriberInfo->subscriberTable.capSize = array_size(subscriberArray);
    topicSubscriberInfo->subscriberTable.subscribers = subscriberArray;
    for (uint16_t i = 0; i < array_size(topicToCoreArray); ++i) {
        int8_t coreCount = GetTopicCoreNum(topicToCoreArray[i].coreMask);
        if (topicToCoreArray[i].coreCount != coreCount) {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "coreNum invalid. topicToCoreArray[i].coreCount=%d, coreCount=%d",
                topicToCoreArray[i].coreCount, coreCount);
        }
    }
#ifdef WEARABLE_LOG_DEBUG
    TopicSubscriberInfoLog(topicSubscriberInfo);
#endif
    return OHOS_SUCCESS;
}

/* 某个topic，注册监听者 */
int32_t SubscribeTopicImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber *subscriber)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);

    int32_t ret = OHOS_FAILURE;
    TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16_t topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not exist! topic=%u", topic);
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (!(topicTable->topics[topicIdx].coreMask & PLATFORM_CORE_MASK)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not send current core! topic=%u, coreMask=0x%x, MASK=0x%x",
            topic, topicTable->topics[topicIdx].coreMask, PLATFORM_CORE_MASK);
        return ret;
    }

    const Identity *identity = subscriber->identity;
    if (identity != NULL) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "serviceId=%u, featureId=%u, queueId=%u",
                      identity->serviceId, identity->featureId, identity->queueId);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "args=0x%p, notify=0x%p", subscriber->args, subscriber->Notify);

    SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    do {
        /* 首先查看是否已经存在相同的结构 */
        int16_t subIdx = GetSubscriberIndex(subscriberTable->subscribers, subscriberTable->count, subscriber);
        if (subIdx == INVALID_INDEX) {
            /* 所有的订阅者结构使用完 */
            if (subscriberTable->count >= subscriberTable->capSize) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "subscriber is enough! count=%u, capSize=%u",
                              subscriberTable->count, subscriberTable->capSize);
                break;
            }
            subscriberTable->subscribers[subscriberTable->count] = *subscriber;
            subIdx = subscriberTable->count;
            subscriberTable->count++;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIdx=%u, subIdx=%u", topicIdx, subIdx);
        if (!GetBitmap(topicTable->bitmap, topicIdx, subIdx)) {
            SetBitmap(topicTable->bitmap, topicIdx, subIdx);
        } else {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "already subscribe");
        }
        ret = OHOS_SUCCESS;
    } while (FALSE);
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return ret;
}

/* 某个topic，清理对应监听者的位图，监听者数组不做处理 */
int32_t UnsubscribeTopicsImpl(TopicSubscriberInfo *topicSubscriberInfo, Topic topic, const Subscriber *subscriber)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);

    int32_t ret = OHOS_FAILURE;
    TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16_t topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not exist! topic=%u", topic);
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (!(topicTable->topics[topicIdx].coreMask & PLATFORM_CORE_MASK)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not send current core! topic=%u, coreMask=0x%x, MASK=0x%x",
            topic, topicTable->topics[topicIdx].coreMask, PLATFORM_CORE_MASK);
        return ret;
    }

    const Identity *identity = subscriber->identity;
    if (identity != NULL) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "serviceId=%u, featureId=%u, queueId=%u",
                      identity->serviceId, identity->featureId, identity->queueId);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "args=0x%p, notify=0x%p", subscriber->args, subscriber->Notify);

    SubscriberTable *subscriberTable = &(topicSubscriberInfo->subscriberTable);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    do {
        /* 首先查看是否已经存在相同的结构 */
        int16_t subIdx = GetSubscriberIndex(subscriberTable->subscribers, subscriberTable->count, subscriber);
        if (subIdx == INVALID_INDEX) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "subscriber not in array!");
            break;
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topicIdx=%u, subIdx=%u", topicIdx, subIdx);
        if (GetBitmap(topicTable->bitmap, topicIdx, subIdx)) {
            /* 仅清零当前topic对应的subscribe，在subscribe数组中，subscriber数据仍然存在 */
            ClearBitmap(topicTable->bitmap, topicIdx, subIdx);
        } else {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "current not subscribe.");
        }
        ret = OHOS_SUCCESS;
    } while (FALSE);
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    return ret;
}

int32_t GetTopicBitmapsImpl(const TopicSubscriberInfo *topicSubscriberInfo, Topic topic, uint32_t *bitmaps, int16_t size)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32_t ret = -1;
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16_t topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, topic);
    if (topicIdx == INVALID_INDEX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not exist! topic=%u", topic);
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic=%u, topicIdx=%u", topic, topicIdx);

    if (size < BITMAP_ARRAY_ENTRY_SIZE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "bitmaps size not enough! size=%u, SIZE=%u",
                      size, BITMAP_ARRAY_ENTRY_SIZE);
        return ret;
    }

    int16_t offset = topicIdx * BITMAP_ARRAY_ENTRY_SIZE;
    int16_t bufSize = size * sizeof(uint32_t);
    int16_t len = BITMAP_ARRAY_ENTRY_SIZE * sizeof(uint32_t);
    MUTEX_Lock(topicSubscriberInfo->mutex);
    (void)memcpy_s(bitmaps, bufSize, &(topicTable->bitmap[offset]), len);
    MUTEX_Unlock(topicSubscriberInfo->mutex);
    ret = BITMAP_ARRAY_ENTRY_SIZE;
    return ret;
}

/* 通信服务发过来的消息， 处理过程中不需要释放。 如果需要转发时，需要复制消息(零拷贝) */
int32_t HandleTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32_t ret = OHOS_FAILURE;
    if (req == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "invalid param! req=0x%p", req);
        return ret;
    }

    /* topic */
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16_t topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, req->msgId);
    if (topicIdx == INVALID_INDEX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not exist! msgId=%u", req->msgId);
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic=%u, topicIdx=%u", req->msgId, topicIdx);

    SendInnerMessage(topicSubscriberInfo, topicIdx, req);
    return OHOS_SUCCESS;
}

/* 其他服务发直接发布消息， 处理消息后需要释放信息。核内零拷贝、核间需要复制消息 */
int32_t PublishTopicMsgImpl(const TopicSubscriberInfo *topicSubscriberInfo, Request *req)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "start! topicSubscriberInfo=0x%p", topicSubscriberInfo);
    int32_t ret = OHOS_FAILURE;
    if (req == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "invalid param! req=0x%p", req);
        return ret;
    }
    const TopicToCoreTable *topicTable = &(topicSubscriberInfo->topicTable);
    int16_t topicIdx = GetTopicIndex(topicTable->topics, topicTable->count, req->msgId);
    if (topicIdx == INVALID_INDEX) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic not exist! msgId=%u", req->msgId);
        if (req->data != NULL) {
            free(req->data);
        }
        return ret;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_BROADCASE_SERVICE, "topic=%u, topicIdx=%u", req->msgId, topicIdx);

    /* 消息将在SendInnerMessage或SendOuterMessage中释放，或发生出去 */
    uint16_t coreMask = topicTable->topics[topicIdx].coreMask;
    BOOL isInnerCore = IsInnerCore(coreMask);
#ifdef BROADCAST_SUPPORT_COMMU_SERVICE
    BOOL isOuterCore = IsOuterCore(coreMask);
    /* 发送核间消息，判断方式，后续可以优化 */
    if (isOuterCore) {
        SendOuterMessage(topicSubscriberInfo, topicIdx, req);
    }
#endif
    /* topic需要核内通信 */
    if (isInnerCore) {
        SendInnerMessage(topicSubscriberInfo, topicIdx, req);
    }
    return OHOS_SUCCESS;
}
