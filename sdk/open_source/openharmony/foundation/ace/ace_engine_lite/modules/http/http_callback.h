/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: http callback interfaces
 * Author: Software Group
 * Create: 2025-09-1
 */

#ifndef HTTP_CALLBACK_H
#define HTTP_CALLBACK_H

#include "jsi.h"
#include "httpclient.h"
#include "http_errcode.h"
#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {

typedef HTTP_REQUEST_TYPE HttpRequestType;
typedef HTTP_ERRCODE HttpErrcode;
typedef HTTPC_RESULT HttpcResult;

enum HttpDataType {
    DATA_TYPE_STRING = 0,
    DATA_TYPE_OBJECT = 1,
    DATA_TYPE_ARRAY_BUFFER = 2
};

enum HttpEventId {
    HTTP_EVENT_RECEIVE = 0,
    HTTP_EVENT_PROGRESS = 1,
    HTTP_EVENT_END = 2,
    HTTP_EVENT_MAX = 3
};

struct HttpResult {
    HttpErrcode resCode;
    int responseCode;
    const char *errMsg;
    uint8_t *data;
    int dataLen;
    int receiveSize;
    int totalSize;
    bool isMore;
    bool isFinish;
    bool isOnce;
    HttpDataType datatype;
    HttpEventId eventId;
    JSIValue context;
    JSIValue callback;
    void *client;
};

class HttpCallback : public MemoryHeap {
public:
    virtual ~HttpCallback() {}
    virtual void CreateCallback(HttpResult* result) {}
    virtual void CreateSubscription(HttpResult* result){}
};

}  // namespace ACELite
}  // namespace OHOS

#endif