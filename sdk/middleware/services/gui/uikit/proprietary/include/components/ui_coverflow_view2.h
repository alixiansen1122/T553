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

#ifndef UI_COVERFLOW_VIEW_2_H
#define UI_COVERFLOW_VIEW_2_H
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_swipe_view.h"
#include "components/ui_transform_group.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UICoverFlowView2 : public UISwipeView {
public:
    explicit UICoverFlowView2(bool supportMirror = true, uint8_t mirrorOpa = 50); // 50:mirror view opaque
    ~UICoverFlowView2() override;

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_COVER_FLOW_2;
    }

    /**
     * @brief Adds a child view.
     *
     * @param view Indicates the pointer to the child view to add.
     * Only UITransform are allowed to be added.
     * Mirror image with UICanvasExt in UITransform are not supported.
     * @since 1.0
     * @version 1.0
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts a new child view behind the current one.
     *
     * @param prevView Indicates the pointer to the current child view, previous to the new child view to insert.
     * @param view Indicates the pointer to the new child view to insert.
     * @since 1.0
     * @version 1.0
     */
    void Insert(UIView* prevView, UIView* insertView) override;

    /**
     * @brief Removes a child view.
     *
     * @param view Indicates the pointer to the child view to remove.
     * @since 1.0
     * @version 1.0
     */
    void Remove(UIView* view) override;

    /**
     * @brief Set page direct interval.
     *
     * @param padding setting padding value. The interval between two images is twice the padding.
     */
    void SetPagePadding(uint16_t padding);

    /**
     * @brief Set rotate angle between the left and right items in static display mode.
     *
     * @param angle setting rotate angle.
     */
    void SetChildMaxRotateAngle(float angle);

    /**
     * @brief Represents a listener for swipe of the coverflow view.
     */
    class OnCoverflowScrollListener : public HeapBase {
    public:
        virtual bool OnScroll(uint16_t distance) = 0;
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
    void ReMeasure(void) override;
    bool CalCurViewInfo();
    void UpdatePerspectiveMatrix();

    using UISwipeView::Add;
    using UISwipeView::Remove;
    Matrix4<float> perspectiveMatrix_;
    OnCoverflowScrollListener* coverflowListener_;
    uint16_t swipeMid_;
    float rotateAngle_;
    bool isFirstRender_;
    uint16_t padding_;
    uint8_t mirrorOpa_;
    bool isShowMirrorImg_;
    Graphic::Vector<UITransformGroup*> mirrorViews_;
};
} // namespace OHOS
#endif // UI_COVERFLOW_VIEW_2_H
