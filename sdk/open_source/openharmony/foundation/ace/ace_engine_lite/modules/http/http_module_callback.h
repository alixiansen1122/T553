/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: http module callback interfaces
 * Author: Software Group
 * Create: 2025-09-1
 */

#ifndef HTTP_MODULE_CALLBACK_H
#define HTTP_MODULE_CALLBACK_H

#include "jsi.h"
#include "http_errcode.h"
#include "http_callback.h"

namespace OHOS {
namespace ACELite {

class HttpModuleCallback final : public HttpCallback {
public:

    static HttpModuleCallback* GetInstance()
    {
        static HttpModuleCallback instance;
        return &instance;
    }

    void CreateCallback(HttpResult* result) override;
    void CreateSubscription(HttpResult* result) override;

    static void ExecuteCallFunc(void *params);
    static void ExecuteSubscription(void *params);
private:
    HttpModuleCallback() = default;
    ~HttpModuleCallback() = default;

    static void SendData(HttpResult* result);
    static void SendStateInfo(HttpResult* result);
};

}  // namespace ACELite
}  // namespace OHOS

#endif // HTTP_MODULE_CALLBACK_H