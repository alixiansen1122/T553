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

#ifndef UI_SPHERE_VIEW_H
#define UI_SPHERE_VIEW_H
#include "components/ui_scroll_view.h"
#include "components/ui_transform_group.h"
#include "components/ui_image_view.h"
namespace OHOS {
class UISphereView : public UIScrollView {
public:
    UISphereView(uint8_t numberOfRow);
    virtual ~UISphereView() override;

    UIViewType GetViewType() const override
    {
        return UI_SPHERE_VIEW;
    }

    /**
     * @brief Adds a child view.
     *
     * @param view Indicates the pointer to the child view to add.
     * @since 1.0
     * @version 1.0
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts a new child view behind the current one.
     *
     * @param prevView Indicates the pointer to the current child view, previous to the new child view to insert.
     * @param insertView Indicates the pointer to the new child view to insert.
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
     * @brief Sets the number of rows to be displayed.
     *
     * @param numberOfRow Number of rows.
     * @since 1.0
     * @version 1.0
     */
    void SetNumberOfRow(uint8_t numberOfRow);

    /**
     * @brief Get the number of rows to be displayed.
     *
     * @return Number of rows.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetNumberOfRow()
    {
        return numberOfRow_;
    }

    class ChildStateListener : public HeapBase {
    public:
        /**
        * @brief Callback function when the status of a child control changes.
        *
        * @param view Object of the child control whose state changes.
        * @param rowAngle Rotation angle of the child control in the current row direction.
        * @param colAngle Rotation angle of the child control in the current column direction.
        * @since 1.0
        * @version 1.0
        */
        virtual bool StateChange(UIView& view, float rowAngle, float colAngle)
        {
            return false;
        }

        virtual ~ChildStateListener() {}
    };

    /**
     * @brief Listens for the change of the angle status of the child control.
     *
     * @param Object of class ChildStateListener.
     * @since 1.0
     * @version 1.0
     */
    void SetChildStateListener(ChildStateListener* listener)
    {
        listener_ = listener;
    }

    /**
     * @brief Updates the angle of the current spherical rotation.
     * @param angle Indicates The angle of movement.
     * @since 1.0
     * @version 1.0
     */
    void UpdateItemAngle(float angle);

    /**
     * @brief Sets the height for the view.
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

    /**
     * @brief Sets the width for the view.
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief  Set sensitivity factor.
     *         Drag distance will be converted to angle, and will multiply by this factor.
     */
    void SetSensitivityFactor(float sensitivity)
    {
        sensitivityFactor_ = sensitivity;
    }

    /**
     * @brief  Get sensitivity factor. Default value is 1.0.
     */
    float GetSensitivityFactor()
    {
        return sensitivityFactor_;
    }

    void SetSphereCameraPosition(const Vector2<float>& position);
    void SetSphereCameraDistance(int16_t distance);
#if ENABLE_ROTATE_INPUT
    bool OnRotateStartEvent(const RotateEvent& event) override;
    bool OnRotateEvent(const RotateEvent& event) override;
    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override
    {
        return true;
    }
    void StopAnimator() override;
private:
    class DragAnimatorCallback : public AnimatorCallback {
    public:
        DragAnimatorCallback()
            : curTime_(0),
              animTime_(0),
              previousAngle_(0),
              angle_(0)
        {
        }

        virtual ~DragAnimatorCallback() {}

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

    float ConvertDistanceToAngle(int16_t distance);
    bool InitChildExtraMsg();
    void ReMeasure(void) override;
    void CalculateCameraMatrix();
    void TransformItemGroup();

    Animator dragAnim_;
    ChildStateListener* listener_ = nullptr;
    DragAnimatorCallback dragCallback_;
    uint8_t numberOfRow_ = 1;
    bool needRefreshSphere_ = false;
    int16_t offsetRotate_ = 0;
    float perimeter_ = 0.0f;
    float sensitivityFactor_ = 1.0f;
    int16_t cameraDistance_ = -2000;  // -2000 : default distance
    Vector2<float> cameraPosition_;
    Matrix4<float> perspectiveMatrix_;
    /* calculate swipe anim period: angle offset / ANGLE_COEFFICIENT. Minimum swipe anim period is MIN_DRAG_TIMES. */
    static constexpr uint8_t ANGLE_COEFFICIENT = 5;
};
}
#endif // UI_SPHERE_VIEW_H