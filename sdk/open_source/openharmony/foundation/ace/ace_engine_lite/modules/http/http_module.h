/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: http control interfaces
 * Author: Software Group
 * Create: 2025-03-28
 */

#ifndef HTTP_MODULE_H
#define HTTP_MODULE_H

#include <stdint.h>
#include <map>
#include <cstring>
#include "cmsis_os2.h"
#include "jsi.h"
#include "httpclient.h"

namespace OHOS {
namespace ACELite {

class HttpModule final : public MemoryHeap {
public:
    HttpModule() = default;
    ~HttpModule() = default;

    static JSIValue CreateHttp(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Request(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue RequestInStream(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Download(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Upload(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Once(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void OnTerminate();

private:
    static HttpRequestType GetRequestType(const char *method);
    static HttpEventId GetEventId(const char *event);
    static char *HeaderJsonToString(char* headerJson);
};
void InitHttpClientModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS

#endif  // HTTP_MODULE_H