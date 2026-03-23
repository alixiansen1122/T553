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

#ifndef NATIVEABILITYFWK_ABILITY_SLICE_H
#define NATIVEABILITYFWK_ABILITY_SLICE_H
#include <typeinfo>
#include "Slice.h"
#include "Presenter.h"
#include "View.h"
#include "SlicePageManager.h"

namespace OHOS {
enum class SliceState : uint8_t {
    UNINITIALIZED,
    START,
    RESUME,
    PAUSE,
    STOP
};
class AbilitySlice : public Slice {
public:
    AbilitySlice(ViewBase* viewBase, PresenterBase* presenterBase, uint32_t targetId)
        : sliceId_(targetId & SLICE_MASK), view_(viewBase), presenter_(presenterBase),
          slicePageMgr_(targetId), state_(SliceState::UNINITIALIZED) {}

    void OnStart(void* data) override;

    void OnResume() override;

    void OnPause() override;

    void OnStop() override;

    UIViewGroup* GetSliceContainer() override;

    bool ChangeSlicePage(uint16_t pageId, void* data, TransitionType type, bool canBack);

    bool BackToPrevSlicePage();

    bool BackToCachedSlicePage(uint16_t pageId);

    SlicePageManager* GetSlicePageManger()
    {
        return &slicePageMgr_;
    }
private:
    uint16_t sliceId_;
    ViewBase* view_;
    PresenterBase* presenter_;
    SlicePageManager slicePageMgr_;
    SliceState state_;
};
} // namespace OHOS

#endif // NATIVEABILITY_ABILITY_SLICE_H
