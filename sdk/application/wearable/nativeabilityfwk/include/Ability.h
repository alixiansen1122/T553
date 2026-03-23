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

#ifndef NATIVEABILITYFWK_ABILITY_H
#define NATIVEABILITYFWK_ABILITY_H
#include <cstdint>
#include "wearable_log.h"
#include "TransitionType.h"
#include "soc_osal.h"
#include "gfx_utils/vector.h"
#include "graphic_mutex.h"
#include "Slice.h"
#include "SliceProxy.h"
#include "SlicePageManager.h"

namespace OHOS {
const static uint16_t gSliceInvalid = 0;
const static uint16_t gPageInvalid = 0;
const static uint16_t gSliceDefaultPriority = 4;  // 页面默认是最低优先级

struct ExtralConfig {
    uint16_t pageId;
    TransitionType type;
    bool enableSlideBack;
};

// 定义默认配置
constexpr ExtralConfig DEFAULT_CONF = {gPageInvalid, TransitionType::TRANSITION_INVALID, false};

class Ability {
public:
    void AddSliceProxy(uint16_t sliceId, SliceProxy* proxy);

    /**
     * @brief Switch to specified page in the current slice, target page will be created, and current slice will
     *         not be recreated. if slices or pages is being switched, this operation will be canceled
     * @param pageId Indicates the specified id of the page in the current slice, it should not be zero
     * @param type Indicates Transition animation type, which is declared in TransitionType.h. The matched
     *        Transitioncallback should have params: enableCurrentSnapshot_ = false enableTargetSnapshot_ = false
     * @param canBack Indicates whether current page will be cached or destroyed. if cached, can back from target page
     * @param data Indicates the data which will be transfered to next page, default value is nullptr,
     *        data can only be native C structures or basic data types
     * @param dataLength Indicates the length of data
     * @return Returns <b>true</b> if the pages is switched successfully, returns <b>false</b> otherwise.
     * @note It is recommended to call this method in the UI thread, and the event will be post into graphic event queue
     */
    bool SwitchPageInSlice(uint16_t pageId, TransitionType type = TransitionType::TRANSITION_INVALID,
                           bool canBack = false, void* data = nullptr, uint16_t dataLength = 0);

    /**
     * @brief Back to previous cached page in the current slice, current page will be destroyed, and previous
     *        cached page will be resumed. if slices or pages is being switched, this operation will be canceled
     * @return Returns <b>true</b> if back to cached page successfully, returns <b>false</b> if no cached page.
     * @note It is recommended to call this method in the UI thread, and the event will be post into graphic event queue
     */
    bool BackToPrevSlicePage();

    /**
     * @brief Back to target cached page in the current slice, current page and the cached pages on the top of target
     *        cached page will be destroyed, target cached page will be resumed. if slices or pages is being
     *        switched, this operation will be canceled
     * @param pageId Indicates the target page id in cached pages, it should not be zero
     * @return Returns <b>true</b> if back to target cached page successfully, returns <b>false</b> otherwise.
     * @note It is recommended to call this method in the UI thread, and the event will be post into graphic event queue
     */
    bool BackToCachedSlicePage(uint16_t pageId);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If pageId is valid or default page exists, matched page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     * @param sliceId Indicates the specified id of the slice
     * @param pageId Indicates the specified id of the slice, if pageId is invalid, default page will be used.
     * @param type Indicates Transition animation type, which is declared in TransitionType.h
     * @param enableSliceBack Indicates whether can return to current slice from target slice by sliding
     */
    void SwitchSlice(uint16_t sliceId, uint16_t pageId = gPageInvalid,
        TransitionType type = TransitionType::TRANSITION_INVALID, bool enableSlideBack = false);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If pageId is valid or default page exists, matched page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     * @param config Indicates the extern config of the slice.
     */
    void SwitchSliceWithData(uint16_t sliceId, void* data, uint16_t dataLength,
        const ExtralConfig& config = DEFAULT_CONF);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If default page is exist, default page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     *        Note: Method SwitchSlice is recommended, this method is reserved for compatibility.
     * @param sliceId Indicates the specified id of the slice
     * @param type Indicates Transition animation type, which is declared in TransitionType.h
     * @param priority Indicates the priority of slice, default value is used currently. if needed, config it in
     *        XXXPresenter::OnResume using method SetCurSlicePriority
     * @param enableSliceBack Indicates whether can return to current slice from target slice by sliding
     */
    void ChangeSlice(uint16_t sliceId, TransitionType type = TransitionType::TRANSITION_INVALID,
        uint16_t priority = gSliceDefaultPriority, bool enableSlideBack = false);

    void ChangeSliceToApplist();

    /**
     * @brief Get SlicePageManager of currrent slice. you can Use SlicePageManager to get page info in the slice.
     * @return Returns <b>nullptr</b> if slices is being switched, or js ability is running
     */
    SlicePageManager* GetCurSlicePageManager();

    void SetDefaultSliceId(uint16_t defaultSliceId, uint16_t pageId = gPageInvalid)
    {
        defaultTargetId_ = defaultSliceId | (static_cast<uint32_t>(pageId) << PAGE_OFFSET);
    }

    /**
     * @brief Get current slice id
     * @return Returns <b>0</b> if js ability is running, detail refer to comments of variable allSlices_
     */
    uint16_t GetCurSliceId(void)
    {
        if (allSlices_.IsEmpty()) {
            return gSliceInvalid;
        }
        return static_cast<uint16_t>(allSlices_[allSlices_.Size() - 1] & SLICE_MASK);
    }

    /**
     * @brief Get prevoius slice id in the slice vector, detail refer to comments of variable allSlices_
     */
    uint16_t GetPreSliceId(void)
    {
        if (allSlices_.Size() < 2) {
            return gSliceInvalid;
        }
        return static_cast<uint16_t>(allSlices_[allSlices_.Size() - 2] & SLICE_MASK);
    }

    void UpdateCurSlicePageId(uint16_t pageId);

    uint32_t GetCurTargetId(void)
    {
        if (allSlices_.IsEmpty()) {
            return gSliceInvalid;
        }

        return allSlices_[allSlices_.Size() - 1];
    }

    uint32_t GetPreTargetId(void)
    {
        if (allSlices_.Size() < 2) { // 2, minimum count if contains previous target
            return gSliceInvalid;
        }

        return allSlices_[allSlices_.Size() - 2]; // 2, index offset
    }

    void SetCurSlicePriority(uint16_t priority)
    {
        curSlicePriority_ = priority;
    }

    uint16_t GetCurSlicePriority(void)
    {
        return curSlicePriority_;
    }

    void ResumeSlice();
    void PauseSlice();
    void StopSliceTransition();
    void BackToPreSlice();
    void EnterAod();
    void ExitAod(uint32_t targetId);

protected:
    // targetId consists of slice id and page id
    void StartSlice(uint32_t targetId, void* data = nullptr);
    void ResumeSliceWithoutJS();
    void StopSlice();
    Ability() = default;
    virtual ~Ability() = default;
    struct ProxyStruct {
        uint16_t sliceId;
        SliceProxy* proxy;
    };

    struct SliceStruct {
        uint16_t sliceId = 0;
        Slice* slice = nullptr;
    };

    // 预加载的所有slices
    Graphic::Vector<ProxyStruct> allSlicesProxy_{16};

    /** Processing rule of allSlices_, its item value consists of slice and page. slice | page << 16.
     *    1. When switch slice A to main slice, clear slice vector, and then push back main slice into slice vector
     *    2. When switch slice A to slice B (not main), push back slice B into slice vector
     *    3. When switch js to slice A, clear slice vector, and then push back slice A into slice vector
     *    4. When switch slice A to js, push back 0 into slice vector. You Can get slice A by method GetPreSliceId()
     */
    Graphic::Vector<uint32_t> allSlices_;

    SliceStruct curSlice_;
    SliceStruct nextSlice_;
    SliceStruct prevAodSlice_;
    uint32_t targetIdFromJS_ = 0;
    uint32_t defaultTargetId_ = gSliceInvalid;
    uint16_t curSlicePriority_ = gSliceDefaultPriority;
    void PostChangeSlice(uint32_t targetId, TransitionType type, void* data);
    Ability::ProxyStruct* Find(const uint16_t sliceId);
};
}
#endif // NATIVEABILITYFWK_ABILITY_H
