/*
* Copyright (c) CompanyNameMagicTag. 2024-2024. All rights reserved.
* Description: display control interfaces
* Author: Software Group
* Create: 2024-04-03
*/

#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <stdint.h>
#include "jsi.h"

namespace OHOS {
namespace ACELite {
class DisplayModule final : public MemoryHeap {
public:
    /**
     * constructor
     */
    DisplayModule() = default;

    /**
     * desconstructor
     */
    ~DisplayModule() = default;

    /**
     * Display interface
     */
    static JSIValue SetKeepScreenOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue SetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue GetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue SetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Display interface
     */
    static JSIValue GetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
};

void InitBrightnessModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS

#endif // DISPLAY_MODULE_H

