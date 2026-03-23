/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_HEXAGONS_LIST_H
#define UI_HEXAGONS_LIST_H

#include "components/ui_view.h"
#include "components/ui_abstract_scroll.h"
#include "components/ui_scroll_view.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UIHexagonsList : public UIScrollView {
public:
    UIHexagonsList();

    ~UIHexagonsList() override {}

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
     * @brief Layout all children views according to the hexagons shape, it must be invoked after method:
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
     * @param pivot Indicates index of scaling center.
     */
    void Scale(float scale, uint8_t centerIndex);

    float GetGlobalScale()
    {
        return globalScale_;
    }

    /**
     * @brief Get the Index Of View object in this view group
     *
     * @param index, Indicates the view to search
     * @return Returns the view, nullptr if not exsit
     */
    UIView* GetViewByIndex(uint8_t index);

    enum class ReboundMode : uint8_t {
        NEAREST_IMG_ON_SCR_CENTER,
        CENTER_IMG_ON_SCR_CENTER,
        NORMAL, // same with UIScrollView
    };

    void SetReboundMode(ReboundMode mode)
    {
        reboundMode = mode;
    }

    /**
     * @brief Get index of focused image, default 0, changed by scale and scroll
     *        When scroll animator end, the index of image in the center screen will set to be focus index
     *        When start Scale(scale, index), scale center index will set to be focus index
     * @return Returns index of focused image, -1 if rebound mode is NORMAL
     */
    int8_t GetFocusedImgIndex()
    {
        if (reboundMode == ReboundMode::NORMAL) {
            return -1;
        } else {
            return focusImgIndex_;
        }
    }

    /**
     * @brief Get center position of the special(index) image
     *        Note: the position is not actual position, it is position in regular hexagon without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @return Returns postion in regular hexagon
     */
    Vector2<int16_t> GetCurrentPosition(uint8_t index);

    UIViewType GetViewType() const override
    {
        return UI_HEXAGONS_LIST;
    }

    void ReMeasure() override;

protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;
    /**
     * @brief Get center position of the special(index) image when scale and center image index in screen is specified
     *
     * @return Returns position
     */
    Vector2<int16_t> GetPositionWithCondition(uint8_t index, uint8_t centerIndex, float scale);

    float globalScale_{1.0f};
    float scaleThresholdOfZeroFactor_{0.75f};

private:
    using UIScrollView::Scale;
    void InitParametersAndImgIndex();
    bool IsCenterInPolygons(const Vector2<int16_t>& offset = {0, 0});
    Rect GetChildrenRegion();
    void CalculateAdjustDistanceForRectScreen(Vector2<float>& position, float& imgScaleRatio);
    void AdjustViewByDistance(UIView& view, const Vector2<int16_t>& position);
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void CalculateReboundDistanceForNormal(int16_t& dragDistanceX, int16_t& dragDistanceY);
    void CalculateReboundDistanceForNearestOnCenter(int16_t& dragDistanceX, int16_t& dragDistanceY);
    void CalculateNearestPointFromOutterPoint(const Vector2<int16_t>& outerPoint);
    void UpdateNearestPointFromFixedPoint(const Vector2<int16_t>& fixedPoint, uint8_t index);
    void CreatePolygons(uint16_t lap, int16_t angle);
    void ScaleBaseElement(UIView* view, float scaleX, float scaleY);
    Graphic::Vector<Vector2<int16_t>> imgPoints_{64};
    uint16_t maxLap_ = 0;
    uint16_t maxAngleInMaxLap_ = 0;
    Graphic::Vector<uint8_t> polygonPoints_{10};
    float imgDistance_{0};
    uint16_t oriImgDistance_{0};
    float oriScaleFactor_{0.23f};
    int16_t imgSizeInCenter_{0};
    uint32_t minDistancePow_{0xFFFFFFFF};
    uint8_t nearestIndex_{0};
    uint8_t focusImgIndex_{0};
    float oriImgRadius_{0.0f};
    float imgRadius_{0.0f};
    int16_t centerX_{0};
    int16_t centerY_{0};
    ReboundMode reboundMode{ReboundMode::NEAREST_IMG_ON_SCR_CENTER};
};
}
#endif // UI_HEXAGONS_LIST_H
