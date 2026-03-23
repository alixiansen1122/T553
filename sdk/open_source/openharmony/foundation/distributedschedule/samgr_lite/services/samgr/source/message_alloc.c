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
#include "message_inner.h"
#include <ohos_errno.h>
#include "securec.h"
#include "service_id_define.h"
#include "cores_types.h"
#include "samgr_debug.h"

void SAMGR_CopyMsg(Request *dst, Request *src) {
    dst->msgId = src->msgId;
    dst->len = src->len;
    dst->msgValue = src->msgValue;
}

Request* SAMGR_AllocRequestMsg(
    uint16 direct, uint16 coreNum, uint16 serviceId, uint16 len) {
    Request *req;
    uint32 msgLen = sizeof(Request) + len;
    uint8 *msg =  malloc(msgLen);
    if (msg == NULL) {
        SAMGR_LOG_ERROR("SAMGR_AllocRequestMsg %u %u %u failed\n", coreNum, serviceId, len);
        return NULL;
    }
    req = (Request*)msg;

    if (len == 0) {
        req->data = NULL;
    } else {
        req->data = msg + sizeof(Request);
    }
    req->len = len;
    return req;
}

Request *SAMGR_ZeroCopyMsg(Request* msg)
{
    return msg;
}

void SAMGR_FreeRequestMsg(Request *req)
{
    free(req);
}

