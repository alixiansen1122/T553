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

#ifndef SIGN_MODE_H
#define SIGN_MODE_H

#include "stdint.h"

namespace OHOS {

class SignMode {
public:
    static SignMode& GetInstance() {
        static SignMode instance;
        return instance;
    }
#ifdef OHOS_DEBUG
    static uint8_t SetSignMode(bool enable);
    static bool IsSignMode();
#endif

private:
    SignMode() = default;
    ~SignMode() = default;
    static bool isSignMode_;
};

} // namespace OHOS

#endif // SIGN_MODE_H