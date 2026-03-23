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

#ifndef UI_ROLLER_VIEW_H
#define UI_ROLLER_VIEW_H
#include <string>
#include "components/ui_abstract_scroll.h"

namespace OHOS {

enum RollerViewMode {
    DISPLAY, // Display all items in roller form.
    COVER,   // Display current item at full size.
    TRANSITION,  // used internally.
};

class UIRollerView : public UIAbstractScroll {
public:
    /**
     * @brief  If want to turn off mirroring, It needs to be configured during construction.
     */
    UIRollerView(bool isShowMirrorImg_ = true);
    virtual ~UIRollerView() override;

    /**
     * @brief  Set item size, which is the size of image in DisplayMode.
     */
    void SetItemSize(uint16_t width, uint16_t height);

    /**
     * @brief  Set item padding.
     */
    void SetItemPadding(uint16_t padding);

    /**
     * @brief  Set mirror img opacity.
     */
    void SetMirrorOpacity(uint8_t opa);

    /**
     * @brief  Set sensitivity factor.
     *         Drag distance will be converted to angle, and will multiply by this factor.
     */
    void SetSensitivityFactor(float sensitivity);

    /**
     * @brief  Get sensitivity factor. Default value is 1.0.
     */
    float GetSensitivityFactor();

    /**
     * @brief  Add an item.
     */
    bool AddItem(std::string name, const ImageInfo* src);

    /**
     * @brief  Remove an item.
     */
    void RemoveItem(std::string name);

    /**
     * @brief  Clear all items.
     */
    void ClearAll();

    /**
     * @brief  Get current item name.
     */
    std::string GetCurrentItemName();

    /**
     * @brief  Get current item, which contains 2 imgs.
     */
    UIViewGroup* GetCurrentItem();

    /**
     * @brief  Set current item. must to call it once.
     */
    void SetCurrentItem(std::string name, bool enableAnimator = false);

    /**
     * @brief  Set current mode, choose from [DISPLAY, COVER].
     */
    void SetMode(RollerViewMode mode, bool enableAnimator = false);

    /**
     * @brief  Mode change listener.
     */
    class OnModeChangeListener : public HeapBase {
    public:
        /**
         * @brief  Called when RollerViewMode is changed.
         */
        virtual void OnModeChange(UIRollerView* view, RollerViewMode targetMode) = 0;
        virtual ~OnModeChangeListener() {}
    };

    /**
     * @brief  Get OnModeChangeListener.
     */
    OnModeChangeListener*& GetOnModeChangeListener()
    {
        return onModeChangeListener_;
    }

    /**
     * @brief  Set OnModeChangeListener.
     */
    void SetOnModeChangeListener(OnModeChangeListener* onModeChangeListener)
    {
        onModeChangeListener_ = onModeChangeListener;
    }

    /**
     * @brief  Scroll listener.
     */
    class OnScrollListener : public HeapBase {
    public:
        virtual void OnScroll(UIRollerView* roller, std::string currentItemName, float currentItemAngle) = 0;
        virtual ~OnScrollListener() {}
    };

    /**
     * @brief  Set OnScrollListener.
     */
    void SetOnScrollListener(OnScrollListener* onScrollListener)
    {
        onScrollListener_ = onScrollListener;
    }

    /**
     * @brief  Get GetOnScrollListener.
     */
    OnScrollListener*& GetOnScrollListener()
    {
        return onScrollListener_;
    }

    class OnChildStateListener : public HeapBase {
    public:
        /**
        * @brief Callback function when the status of a child control changes.
        *
        * @param view Object of the child control whose state changes.
        *    This object is a control of the UIViewGroup type.
        *    If roller is in mirror mode, the object will contain two subcomponents.
        *    The head is normal image,tail is mirror image.
        *    If there is no mirroring mode,only have normal images.
        * @param angle Rotation angle of the child control in the current.range: 0-360
        */
        virtual bool StateChange(UIView* view, float angle) = 0;
        virtual ~OnChildStateListener() {}
    };

    /**
     * @brief Listens for the change of the angle status of the child control.
     *
     * @param Object of class OnChildStateListener.
     * @since 1.0
     * @version 1.0
     */
    void SetChildStateListener(OnChildStateListener* listener)
    {
        onChildStateListener_ = listener;
    }

    /**
     * @brief  Get current mode.
     */
    RollerViewMode GetMode();

    /**
     * @brief  Set camera distance. If not set, default distance is 1000.
     */
    void SetCamDistance(int16_t distance);

    /**
     * @brief  Set absolute y coordinate of camera.
     *         Default value is GetY() - GetHeight() / 3.
     */
    void SetCamY(int16_t y);

    /**
     * @brief Obtains the view name.
     * @param view Specifies the view to be obtained.
     */
    static std::string GetRollerItemName(UIView* view);
    bool IsShowMirrorMode()
    {
        return isShowMirrorImg_;
    }

    UIViewType GetViewType() const override
    {
        return UI_ROLLER_VIEW;
    }

    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
#if ENABLE_ROTATE_INPUT
    bool OnRotateStartEvent(const RotateEvent& event) override;
    bool OnRotateEvent(const RotateEvent& event) override;
    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    void SetDirection(uint8_t direction) = delete;
    uint8_t GetDirection() const = delete;
    void SetLoopState(bool loop) = delete;

protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override
    {
        return true;
    }

private:
    class AlignAnimatorCallback : public AnimatorCallback {
    public:
        AlignAnimatorCallback()
            : curTime_(0),
              animTime_(0),
              previousAngle_(0),
              angle_(0)
        {
        }

        virtual ~AlignAnimatorCallback() {}

        void SetAnimTimes(uint16_t times)
        {
            animTime_ = times;
        }

        void SetAngle(float angle)
        {
            angle_ = angle;
        }

        void ResetCallback()
        {
            angle_ = 0;
            previousAngle_ = 0;
            animTime_ = 0;
            curTime_ = 0;
        }

        virtual void Callback(UIView* view) override;

        uint16_t curTime_;
        uint16_t animTime_;
        float previousAngle_;
        float angle_;
    };

    class TransitionAnimatorCallback : public AnimatorCallback {
    public:
        TransitionAnimatorCallback() : curTime_(0), animTime_(0),
            startScaleX_(0), startScaleY_(0), endScaleX_(0), endScaleY_(0),
            camPos_({0, 0}), targetCamPos_({0, 0})
        {
        }

        virtual ~TransitionAnimatorCallback() {}

        void SetAnimTimes(uint16_t times)
        {
            animTime_ = times;
        }

        void SetScaleFactor(float startScaleX, float startScaleY, float endScaleX, float endScaleY)
        {
            startScaleX_ = startScaleX;
            startScaleY_ = startScaleY;
            endScaleX_ = endScaleX;
            endScaleY_ = endScaleY;
        }

        void SetCamPosition(Point previousCamPos, Point targetCamPos)
        {
            camPos_ = previousCamPos;
            targetCamPos_ = targetCamPos;
        }

        void ResetCallback()
        {
            curTime_ = 0;
            animTime_ = 0;
            startScaleX_ = 0;
            startScaleY_ = 0;
            endScaleX_ = 0;
            endScaleY_ = 0;
            camPos_ = {0, 0};
            targetCamPos_ = {0, 0};
        }

        virtual void Callback(UIView* view) override;

        uint16_t curTime_;
        uint16_t animTime_;
        float startScaleX_;
        float startScaleY_;
        float endScaleX_;
        float endScaleY_;
        Point camPos_;
        Point targetCamPos_;
    };

    /* calculate swipe anim period: angle offset / ANGLE_COEFFICIENT. Minimum swipe anim period is MIN_DRAG_TIMES. */
    static constexpr uint8_t ANGLE_COEFFICIENT = 5;
    /* the minimum period of mode transition animator. */
    static constexpr uint8_t MIN_TRANSITION_TIMES = 5;
    /* calculate mode transition animator period: scale offset / TRANSITION_COEFFICIENT. */
    static constexpr float TRANSITION_COEFFICIENT = 0.2;
    OnModeChangeListener* onModeChangeListener_ = nullptr;
    OnScrollListener* onScrollListener_ = nullptr;
    OnChildStateListener* onChildStateListener_ = nullptr;
    float ConvertDistanceToAngle(int16_t distance);
    bool IsInCurrentItemRange(float angle);
    void InitImg(UIView* img);
    void DoCalculation();
    void PrepAlign(float angle);
    void SetCamPosInner(Point position);
    void StartTransitionAnim();
    void StopTransitionAnim();
    void ReflectImg(UIView* img, UIView* mirror);
    void ReMeasure(void) override;
    void LayoutItem();
    void TransformByAngle(float angle);
    void TransformItemGroup(UIView* view, float angle);
    void DelItemGroup(UIViewGroup* group);
    void ResetState();
    void StartAlignAnim();
    void StopAlignAnim();
    void MoveHeadOrTail();
    void HandleMode();
    void HandleAlign();
    void UpdateCam();

    Animator alignAnim_;
    AlignAnimatorCallback alignCallback_;

    Animator transitionAnim_;
    TransitionAnimatorCallback transitionCallback_;

    int16_t camY_ = 0;
    bool isCamYSet_ = false;
    RollerViewMode mode_ = DISPLAY;
    RollerViewMode targetMode_ = DISPLAY;
    bool switchModeWithAnim_ = false;
    bool alignWithAnim_ = false;
    UIView* coverPreviousItem_ = nullptr;
    uint16_t itemWidth_ = 1;
    uint16_t itemHeight_ = 1;
    uint8_t mirrorOpa_ = 100;
    float radius_ = 0.0f;
    float perimeter_ = 0.0f;
    float sensitivityFactor_ = 1.0f;
    float anglePerItem_ = 0.0f;
    Point camPosition_ = {0, 0};
    Point displayCamPosition_ = {0, 0};
    uint16_t cameraDistance_ = 1000;
    uint16_t itemPadding_ = 0;
    bool needLayout_ = false;
    UIViewGroup* currentItem_ = nullptr;
#if ENABLE_VIBRATOR
    UIViewGroup* lastItem_ = nullptr;
    bool needVibration_ = false;
#endif
    UIViewGroup* alignItem_ = nullptr;
    bool isShowMirrorImg_;
    float alignAngle_ = 0.0f;
};
} // namespace OHOS
#endif // UI_ROLLER_VIEW_H
