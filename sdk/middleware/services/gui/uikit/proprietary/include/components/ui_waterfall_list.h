/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_WATERFALL_LIST_H
#define UI_WATERFALL_LIST_H

#include "components/ui_view.h"
#include "components/ui_abstract_scroll.h"
#include "components/ui_scroll_view.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UIWaterfallList : public UIScrollView {
public:
    UIWaterfallList();

    ~UIWaterfallList() override {}

    /**
     * @brief Set origin image distance between the center of the two adjacent images
     *        imageDistance = oriImgDistance * globalScale
     *        globalScale: refer to function Scale
     *        LayoutChildren(true) must be invoked mannualy after invoking this method.
     *
     * @param distance Indicates image distance to set when the image in the center region
     */
    void SetImageDistance(uint16_t distance)
    {
        oriImgDistance_ = distance;
    }

    /**
     * @brief Set the size of Image or UITransformGroup.
     *        LayoutChildren(true) must be invoked mannualy after invoking this method.
     *
     * @param size Indicates size to set, when the Image or UITransformGroup is displayed in the center.
     *             It is best to keep child's size as same as this size.
     */
    void SetImageSizeInCenter(uint16_t size)
    {
        imgSizeInCenter_ = size;
    }

    /**
     * @brief Set origin image scale factor, scale strategy will follow the below rules
     *        if globalScale >= 1
     *           scaleFactor = oriScaleFactor
     *        elif globalScale > scaleThreshold
     *           scaleFactor = oriScaleFactor * (globalScale - scaleThreshold) / (1 - scaleThrethold)
     *        else
     *           scaleFactor = 0
     *        img size is inversely proportional to distance * scaleFactor
     *
     *        oriScaleFactor: refer to function SetScaleFactorByDistance
     *        scaleThreshold: refer to function SetScaleThesholdOfZeroFactor
     *        globalScale: refer to function Scale
     *
     * @param factor Indicates scale factor to set, which will decide scale value together with distance
     */
    void SetScaleFactorByDistance(float factor)
    {
        oriScaleFactor_ = factor;
    }

    float GetScaleFactorByDistance()
    {
        return oriScaleFactor_;
    }

    /**
     * @brief Set scale threshold, scale factor will decrease to 0 if global scale equal and less than scaleThreshold,
     *        default value is 0.6, more information can  refer to method SetScaleFactorByDistance
     *
     * @param threshold Indicates factor to set, the scale factor when the image is not in the center
     */
    void SetScaleThesholdOfZeroFactor(float threshold)
    {
        scaleThresholdOfZeroFactor_ = threshold;
    }

    float GetScaleThesholdOfZeroFactor()
    {
        return scaleThresholdOfZeroFactor_;
    }

    /**
     * @brief Adds UITransformGroup or UIImageView whose resize mode will set to be UIImageView::COVER.
     *        LayoutChildren(true) should be invoked mannualy after invoking this method.
     *        Note: View index was set for children inner in order of addition, so you can
     *        get its index by call method GetViewIndex()
     *
     * @param view Indicates the pointer to the child view to add.
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts UITransformGroup or UIImageView whose resize mode will set to be UIImageView::COVER.
     *        LayoutChildren(true) should be invoked mannualy after invoking this method.
     *        Note: View index was set for children inner in order of addition, so you can
     *        get its index by call method GetViewIndex()
     *
     * @param prevView Indicates the pointer to the child view, previous to the new child view.
     * @param view Indicates the pointer to the new child view to insert.
     */
    void Insert(UIView* prevView, UIView* insertView) override;

    bool OnDragEvent(const DragEvent& event) override;

    /**
     * @brief Layout all children views according to the Waterfall shape, it must be invoked after method:
     *        1. Add(UIView* view)
     *        2. Insert(UIView* prevView, UIView* insertView)
     *        3. void SetImageSizeInCenter(uint16_t size)
     *        4. void SetImageDistance(uint16_t distance)
     *
     * @param needInvalidate Specifies whether to refresh the invalidated area after the layout is complete.
     *                       Value <b>true</b> means to refresh the invalidated area after the layout is complete,
     *                       and <b>false</b> means the opposite.
     */
    void LayoutChildren(bool needInvalidate = false) override;

    /**
     * @brief Moves all child views.
     *
     * @param x Indicates the offset distance by which this view group is moved on the x-axis.
     * @param y Indicates the offset distance by which this view group is moved on the y-axis.
     */
    void MoveChildByOffset(int16_t offsetX, int16_t offsetY) override;

    /**
     * @brief Set global scale, and image with centerIndex will be focused
     *
     * @param scale Indicates global scale value both for x- and y- axes.
     */
    void Scale(float scale, int16_t centerIndex = -1);

    float GetGlobalScale()
    {
        return globalScale_;
    }

    /**
     * @brief Get the Index Of View object in this view group
     *
     * @param  index, Indicates the view to search
     * @return Returns the view, nullptr if not exsit
     */
    UIView* GetViewByIndex(uint8_t index);

    /**
     * @brief Get index of focused image, default 0, changed by scale and scroll

     *        When start Scale(scale, index), scale center index will set to be focus index
     * @return Returns index of focused image
     */
    int8_t GetFocusedImgIndex()
    {
        return focusImgIndex_;
    }

    /**
     * @brief Get center position of the special(index) image
     *        Note: the position is not actual position, it is position in regular
     *        hexagon without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @return Returns postion
     */
    Vector2<int16_t> GetCurrentPosition(uint8_t index);

    /**
     * @brief Get the row where the initial grid position of index is
     *
     * @return Returns the row
     */
    int16_t GetInitRow(uint8_t index);

    /**
     * @brief Get the column where the initial grid position of index is
     *
     * @return Returns the row
     */
    int16_t GetInitCol(uint8_t index);

    /**
     * @brief Get the initial grid position of the special(index) image
     *        Note: the position is not actual position, it is position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @return Returns postion
     */
    Vector2<int16_t> GetInitPosition(uint8_t index);

    UIViewType GetViewType() const override
    {
        return UI_WATERFALL_LIST;
    }

    void ReMeasure() override;

    /**
     * @brief Simulate the positions of different views during the entry process of waterfall within a period of time
     *        Note: the position is not actual position, it is position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @param  curTime, current time
     * @param  totalTime, total time
     */
    void EnterMove(float curTime, float totalTime);

    /**
     * @brief Get the rebound distance, which is the external interface of CalculateReboundDistance
     *        Note: the position in caculating is not actual position, it is
     *        position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     */
    Vector2<int16_t> GetReboundDistance();

protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;

    float globalScale_{1.0f};
    float scaleThresholdOfZeroFactor_{0.75f};

private:
    using UIScrollView::Scale;
    void InitParametersAndImgIndex();
    void CalculateAdjustDistanceForRectScreen(Vector2<float>& position, float& imgScaleRatio);
    void AdjustViewByDistance(UIView& view, const Vector2<int16_t>& position);
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void ScaleBaseElement(UIView* view, float scaleX, float scaleY);
    Graphic::Vector<Vector2<int16_t>> imgPoints_{64};
    float imgDistance_{0};
    uint16_t oriImgDistance_{0};
    float oriScaleFactor_{0.23f};
    int16_t imgSizeInCenter_{0};
    int16_t focusImgIndex_{-1};
    float oriImgRadius_{0.0f};
    float imgRadius_{0.0f};
    float layoutWidth_{0.0f};
    float layoutHeight_{0.0f};
    int16_t centerX_{0};
    int16_t centerY_{0};
};
}
#endif // UI_WATERFALL_LIST_H
