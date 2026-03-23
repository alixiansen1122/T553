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

#ifndef SLICE_H
#define SLICE_H
#include "components/ui_view_group.h"

namespace OHOS {
extern const uint16_t SLICE_MASK;
extern const uint32_t PAGE_OFFSET;
class Slice {
public:
    Slice() = default;
    virtual ~Slice() = default;
    /**
     * @brief Called when the Slice is started and before transition animator starts if exits.
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnStart And View::OnStart
     */
    virtual void OnStart(void* data) = 0;
    /**
     * @brief Called when the slice will be visible and after transition animator ends if exits
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnResume
     */
    virtual void OnResume() = 0;
    /**
     * @brief Called when another slice will be started and before transition animator starts if exits
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnPause
     */
    virtual void OnPause() = 0;
    /**
     * @brief Called when the slice will be invisible and another slice will be visible
     *
     * @brief Called when the slice maybe become invisible
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnStop and View::OnStop
     */
    virtual void OnStop() = 0;
    virtual UIViewGroup* GetSliceContainer() = 0;

    Slice(const Slice &) = delete;
    Slice &operator=(const Slice &) = delete;
    Slice(Slice &&) = delete;
    Slice &operator=(Slice &&) = delete;
};
} // namespace OHOS

#endif // SLICE_H
