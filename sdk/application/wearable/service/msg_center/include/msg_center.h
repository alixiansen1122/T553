/*
 * Copyright (c) CompanyNameMagicTag.
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

#ifndef MSG_CENTER_H
#define MSG_CENTER_H

#include <cstdint>
#include "service.h"
#include "iunknown.h"
#include "broadcast_feature.h"
#include "graphic_service.h"
#include "jsi/jsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PRIORITY_ZERO = 0,
    PRIORITY_ONE,
    PRIORITY_TWO,
    PRIORITY_THREE,
    PRIORITY_FOUR,
} Priority;

typedef struct MsgCenterFeatureApi {
    INHERIT_IUNKNOWN;
    int32_t (*SendMsg)(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);
} MsgCenterFeatureApi;

typedef struct MsgCenter {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(MsgCenterFeatureApi);
    Identity identity;
    Subscriber subscriber;
} MsgCenter;

typedef void (*MsgCenterNotifyCb)(const Topic topic, Request* req);
typedef struct {
    Topic topic;
    MsgCenterNotifyCb cb;
} MsgCenterNotifyTable;

void DmsLiteSendMsgToJS(const char *msgBody);
void DuerMapInfo(uint8_t cmd_id, uint8_t type, void* data);
void InitMsgCenter(OHOS::ACELite::JSIValue exports);

struct PriorityMapper {
    uint16_t topicId;
    Priority prId; // 优先级级别
};

void MsgCenterSubscribe();
bool MsgCenterNotifyProc(const Topic topic, Request* req);
void MsgCenterSubscribeTopic(Topic topic);

#ifdef JS_ENABLE
void StartJsApp(char *bundleName);
void StopJsApp(char *bundleName);
#endif

#ifdef __cplusplus
}
#endif

#endif // OHOS_MAIN_SERVICE_H
