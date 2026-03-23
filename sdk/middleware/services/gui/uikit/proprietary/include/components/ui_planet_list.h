/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UIPlanetList
 * Create: 2025-05
 */

#ifndef UI_PLANET_LIST_H
#define UI_PLANET_LIST_H

#include <map>
#include "animator/animator.h"
#include "animator/easing_equation.h"
#include "components/ui_abstract_scroll.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view.h"
#include "components/ui_label.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UIPlanetList : public UIAbstractScroll {
public:
    UIPlanetList(uint16_t itemSize, uint16_t imgDistance);
    ~UIPlanetList();

    void AddItemVIew(UIView* view);
    void AddLabel(UIView* view);
    void MoveChildTo(int16_t posX, int16_t posY);
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    bool OnRotateStartEvent(const RotateEvent& event) override;
    bool OnRotateEvent(const RotateEvent& event) override;
    bool OnRotateEndEvent(const RotateEvent& event) override;
    void LayoutChildren(bool needInvalidate = false) override;
    void MoveChildByOffset(int16_t offsetX, int16_t offsetY) override;
    void SetItemClickListener(UIView::OnClickListener* itemClickListener);
    void AddClickListenerToAppItems();
    void RestoreItemState();

    UIViewType GetViewType() const override
    {
        return UI_PLANET_LIST;
    }

    void SetImageDistance(uint16_t distance)
    {
        imgDistance_ = distance;
    }

    uint16_t GetImageDistance()
    {
        return imgDistance_;
    }

    void SetImageSize(uint16_t size)
    {
        itemSize_ = size;
    }

    uint16_t GetImageSize()
    {
        return itemSize_;
    }

    void SetLastOffset(int16_t x, int16_t y)
    {
        lastQuitOffset_.x_ = x;
        lastQuitOffset_.y_ = y;
    }

    void GetLastOffset(int16_t& x, int16_t& y);

protected:
    enum SwipeEdge {
        LEFT = 1,
        RIGHT = 1 << 1,
        UP = 1 << 2,
        DOWN = 1 << 3
    };

    class RotateAnimatorCallback : public AnimatorCallback {
    public:
        RotateAnimatorCallback()
            : startTimes_(0),
              endTimes_(0),
              startValueX_(0),
              endValueX_(0),
              startValueY_(0),
              endValueY_(0)
        {
        }

        virtual ~RotateAnimatorCallback() {}

        void SetDragTimes(uint16_t times)
        {
            endTimes_ = times;
        }

        void SetDragStartValue(int16_t startValueX, int16_t startValueY)
        {
            startValueX_ = startValueX;
            startValueY_ = startValueY;
        }

        void SetDragEndValue(int16_t endValueX, int16_t endValueY)
        {
            endValueX_ = endValueX;
            endValueY_ = endValueY;
        }

        void ResetCallback()
        {
            startTimes_ = 0;
            endTimes_ = 0;
            startValueX_ = 0;
            endValueX_ = 0;
            startValueY_ = 0;
            endValueY_ = 0;
        }

        virtual void Callback(UIView* view);

        uint16_t startTimes_;
        uint16_t endTimes_;
        int16_t startValueX_;
        int16_t endValueX_;
        int16_t startValueY_;
        int16_t endValueY_;
    };

    class ScaleAnimatorCallback : public AnimatorCallback {
    public:
        ScaleAnimatorCallback()
            : curtTime_(0),
              dragTimes_(0),
              startImgDistance_(0),
              endImgDistance_(0),
              startItemSize_(0),
              endItemSize_(0)
        {
        }

        virtual ~ScaleAnimatorCallback() {}

        void SetDragTimes(uint16_t times)
        {
            dragTimes_ = times;
        }

        void SetDragStartValue(int16_t startImgDistance, int16_t startItemSize)
        {
            startImgDistance_ = startImgDistance;
            startItemSize_ = startItemSize;
        }

        void SetDragEndValue(int16_t endImgDistance, int16_t endItemSize)
        {
            endImgDistance_ = endImgDistance;
            endItemSize_ = endItemSize;
        }

        void ResetCallback()
        {
            curtTime_ = 0;
            dragTimes_ = 0;
            startImgDistance_ = 0;
            endImgDistance_ = 0;
            startItemSize_ = 0;
            endItemSize_ = 0;
        }

        virtual void Callback(UIView* view);

        uint16_t curtTime_;
        uint16_t dragTimes_;
        int16_t startImgDistance_;
        int16_t endImgDistance_;
        int16_t startItemSize_;
        int16_t endItemSize_;
    };

    class PlanetAnimator : public Animator {
    public:
        PlanetAnimator() {}
        PlanetAnimator(AnimatorCallback* callback, UIView* view, uint32_t time, bool repeat)
            : Animator(callback, view, time, repeat)
        {}
        virtual ~PlanetAnimator() {}
    };

private:
    UIPlanetList();
    void FillActiveView();
    float GetPivotByOffset(int16_t offset);
    bool OnPressEvent(const PressEvent& event) override;
    float GetScaleFactor(int16_t dstCoordinate, int16_t& distance);
    Vector2<float> GetPivot(UIView* view, int16_t dstX, int16_t dstY);
    float ResizeItem(UIView* view, int16_t positionX, int16_t positionY);
    void ScaleView(UIView* view, float scaleFactor, Vector2<float> pivot);
    int16_t CalculateOffsetAfterMove(int16_t posDiff);
    bool AutoAlignAnimator(Vector2<int16_t>& oriPos);
    bool InertialAnimator(const DragEvent &event, Vector2<int16_t>& movedPos);
    void ReCalculateDragEnd(int16_t& offsetX, int16_t& offsetY);
    void StopAnimator() override;
    Vector2<int16_t> GetHeadItemCurPosition();
    void ReAlignWhenReachEdge();
    void InitBorderLimits();
    void NeedRebound();
    void Showlabels();
    void ScaleMiddleSize();
    void AdjustScaleParameters(float scale, int16_t& dstImgDis, int16_t& dstItemSize);
    UIView* GetViewByIndex(List<UIView*> list, uint16_t index);
    void UpdateOriHeadPos();
    void RecordAndRestoreOffset(bool isRecord);
    bool PlayMoveAnimator(Vector2<int16_t> oriPos, int16_t dragTimesFactor);
    bool PlayScaleAnimator(float scale);
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;

    Vector2<int16_t> singleDragDistance_;
    TimeType dragStartTime_;
    int16_t imgDistance_;
    int16_t oriImgDistance_;
    int16_t itemSize_;
    int16_t oriItemSize_;
    EasingFunc easingFunc_;
    List<UIView*> appLists_;
    List<UIView*> labelLists_;
    bool isReCalculateDragEnd_;
    uint16_t reAlignDirection_;
    Vector2<int16_t> dragStartPos_;
    OnClickListener* itemClickListener_;
    std::map<SwipeEdge, int16_t> dragEndLimits_;
    std::map<SwipeEdge, int16_t> reboundLimits_;
    PlanetAnimator pltMoveAnimator_;
    RotateAnimatorCallback moveCallback_;
    PlanetAnimator pltScaleAnimator_;
    ScaleAnimatorCallback scaleCallback_;
    Vector2<int16_t> oriHeadItemPos_;
    Vector2<int16_t> lastQuitOffset_{0, 0};
    float globalScale_{1.0f};
    float startScale_{0.0f};
}; // class UIPlanetList
} // namespace OHOS

#endif // UI_PLANET_LIST_H
