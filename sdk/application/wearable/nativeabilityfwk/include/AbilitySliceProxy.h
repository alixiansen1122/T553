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

#ifndef NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H
#define NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H

#include "AbilitySlice.h"
#include "wearable_log.h"
#include "Presenter.h"

namespace OHOS {
template <class V, class P>
class AbilitySliceProxy : public SliceProxy {
public:
    Slice* CreateSlice(uint32_t targetId) override
    {
        V* v = new V();
        if (v == nullptr) {
            return nullptr;
        }
        P* p = new P();
        if (p == nullptr) {
            delete v;
            return nullptr;
        }
        return new AbilitySlice(v, p, targetId);
    }
};
} // namespace OHOS
#endif // NATIVEABILITYFWK_ABILITY_SLICE_PROXY_H
