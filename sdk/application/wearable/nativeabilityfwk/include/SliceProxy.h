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

#ifndef SLICE_PROXY_H
#define SLICE_PROXY_H
#include "Slice.h"

namespace OHOS {
class SliceProxy {
public:
    virtual Slice* CreateSlice(uint32_t targetId) = 0;
    SliceProxy() = default;
    virtual ~SliceProxy() = default;
    SliceProxy(const SliceProxy&) = delete;
    SliceProxy& operator=(const SliceProxy&) = delete;
    SliceProxy(SliceProxy&&) = delete;
    SliceProxy& operator=(SliceProxy&&) = delete;
};
} // namespace OHOS
#endif // SLICE_PROXY_H
