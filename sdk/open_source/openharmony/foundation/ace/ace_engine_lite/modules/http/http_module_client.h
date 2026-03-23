/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: http client interfaces
 * Author: Software Group
 * Create: 2025-10-20
 */

#ifndef HTTP_MODULE_CLIENT_H
#define HTTP_MODULE_CLIENT_H

#include <mutex>
#include "cmsis_os2.h"
#include "http_errcode.h"
#include "httpclient.h"
#include "http_module_callback.h"

namespace OHOS {
namespace ACELite {

enum HttpRequestId {
    HTTP_REQUEST,
    HTTP_REQUESTINSTREAM,
    HTTP_DOWNLOAD,
    HTTP_DOWNLOAD_IMAGE,
    HTTP_UPLOAD,
    HTTP_FINISH,
};

struct HttpParam {
    const char *url;
    const char *header;
    const char *filePath;
    const char *clientCert;
    char *postBuf;
};

struct HttpMessage {
    HttpRequestId requestId;
    HttpRequestType requestType;
    HttpParam param;
    int dataType;
    JSIValue context;
    JSIValue callback;
};

struct HttpRequest {
    JSIValue request;
    JSIValue requestInStream;
    JSIValue download;
    JSIValue upload;
    JSIValue on;
    JSIValue once;
    JSIValue off;
    JSIValue destroy;
};

struct HttpEvent {
    JSIValue callback;
    bool isOnce;
};

// 定义下载任务回调类型（参数为HttpResult*，与原有回调一致）
typedef std::function<void(HttpResult*)> DownloadCallback;

class HttpModuleClient final : public MemoryHeap {
public:
    HttpModuleClient();
    ~HttpModuleClient();

    bool IsRunning(void) const
    {
        return isRunning_ ;
    }

    void InitCallback(HttpCallback* cbkInstance)
    {
        cbkInstance_ = cbkInstance;
    }
    
    void SetCustomCallback(DownloadCallback callback) {
        downloadCallback_ = callback;
    }

    void Request(HttpParam &param, HttpRequestType requestType, int expectDataType, JSIValue context, JSIValue callback);
    void RequestInStream(HttpParam &param, HttpRequestType requestType, int expectDataType, JSIValue context, JSIValue callback);
    void Download(HttpParam &param, JSIValue context, JSIValue callback);
    void DownloadImage(HttpParam &param);
    void Upload(HttpParam &param, JSIValue context, JSIValue callback);

    bool InitClient(void);
    bool CreateHttpThread(void);
    void StopThread(void);
    osStatus_t HttpMessagePut(HttpMessage *message);
    osStatus_t HttpMessageGet(HttpMessage *message);
    HttpErrcode GetErrorCode(int errorCode);

    void SetEventCallback(JSIValue callback, bool isOnce, HttpEventId id);
    JSIValue GetEventCallback(HttpEventId id, bool &isOnce);

    HttpClient client_{};
    HttpClientData clientData_{};
    osMessageQueueId_t httpQueueId_ = nullptr;
    HttpRequest request_;
    HttpEvent httpEvents_[HTTP_EVENT_MAX];
    void ReleaseThread(void);
    void DestroyClient(void);
    void CloseClient(void);
private:
    void ReleaseJSIValue(JSIValue value)
    {
        if (!JSI::ValueIsUndefined(value)) {
            JSI::ReleaseValue(value);
        }
    }

    HttpDataType GetContentType(int expectDataType);
    void CheckSubscription(HttpResult result);

    void ResetClient(void);
    
    static const uint32_t REQ_BUF_SIZE = 2048;
    static const uint32_t RSP_BUF_SIZE = 2048;
    bool isRunning_  = false;
    void *stackMem_ = nullptr;
    osThreadId_t streamId_ = nullptr;
    HttpCallback* cbkInstance_;
    std::mutex clientMutex_;
    // 新增：存储自定义回调
    DownloadCallback downloadCallback_;
};

}  // namespace ACELite
}  // namespace OHOS

#endif  // HTTP_MODULE_CLIENT_H