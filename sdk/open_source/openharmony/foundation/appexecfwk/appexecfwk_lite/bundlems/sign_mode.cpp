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

#include "sign_mode.h"
#include "appexecfwk_errors.h"

namespace OHOS {

#ifdef OHOS_DEBUG
bool SignMode::isSignMode_ = true;

uint8_t SignMode::SetSignMode(bool enable)
{
    isSignMode_ = enable;
    return ERR_OK;
}

bool SignMode::IsSignMode()
{
    return isSignMode_;
}
#endif
} // namespace OHOS
