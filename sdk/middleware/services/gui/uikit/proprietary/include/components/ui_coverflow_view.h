/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef UI_COVERFLOW_VIEW_H
#define UI_COVERFLOW_VIEW_H
#include <string>
#include <map>
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_swipe_view.h"

namespace OHOS {
class UICoverflowView : public UISwipeView {
public:
    explicit UICoverflowView(uint8_t mirrorOpa = 30, bool supportMirror = true); // 30: default mirror opacity
    ~UICoverflowView() override;

    /**
     * @brief Add a item to coverflow.
     *
     * @param id Indicates the pointer to the view ID.
     * @param src point item show main image imageinfo.
     */
    void Add(const char* id, const ImageInfo* src);

    /**
     * @brief remove a item from coverflow.
     *
     * @param id Indicates the pointer to the view ID.
     */
    void Remove(const char* id);

    /**
     * @brief  Clear all items.
     */
    void ClearAll();

    /**
     * @brief Set image size.
     *
     * @param width setting image width.
     * @param height setting image height.
     */
    void SetImgSize(uint16_t width, uint16_t height);

    /**
     * @brief Set image direct interval.
     *
     * @param padding setting padding value. The interval between two images is twice the padding.
     */
    void SetImgPadding(uint16_t padding);

    /**
     * @brief Set rotate angle between the left and right items in static display mode.
     *
     * @param angle setting rotate angle.
     */
    void SetRotateAngle(float angle);

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_COVER_FLOW;
    }

    /**
     * @brief Represents a listener for swipe of the coverflow view.
     */
    class OnCoverflowScrollListener : public HeapBase {
    public:
        virtual bool OnScroll() = 0;
        virtual ~OnCoverflowScrollListener() {}
    };

    /**
     * @brief Sets the listener that contains a callback to be invoked upon a swipe event.
     *
     * @param onCoverflowListener Indicates the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnCoverflowListener(OnCoverflowScrollListener* onCoverflowListener)
    {
        coverflowListener_ = onCoverflowListener;
    }

    void SetDirection(uint8_t direction) = delete;
    uint8_t GetDirection() const = delete;
protected:
    bool DragXInner(int16_t distance) override;
    void RefreshCurrentView(int16_t distance, uint8_t dragDirection) override;

private:
    void UpdatePerspectiveMatrix();
    void ReMeasure(void) override;
    void TransformImage(UIImageView* view);
    void DeleteGroup(UIViewGroup* group);
    bool CalCurViewInfo();

    using UISwipeView::Add;
    using UISwipeView::Remove;

    OnCoverflowScrollListener* coverflowListener_;
    std::map<UIView*, Matrix4<float>> map_;
    Matrix4<float> perspectiveMatrix_;

    uint16_t imgWidth_;
    uint16_t imgHeight_;
    uint16_t padding_;
    float rotateAngle_;
    uint8_t mirrorOpa_;
    bool isShowMirrorImg_;
    UIView* lastView_;
    uint16_t swipeMid_;
};
} // namespace OHOS
#endif // UI_COVERFLOW_VIEW_H
