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

#ifndef UI_ICOSAHEDRON_VIEW_H
#define UI_ICOSAHEDRON_VIEW_H
#include "components/ui_abstract_scroll.h"
#include "gfx_utils/graphic_math.h"
#include "components_ext/triangular_plane.h"

namespace OHOS {
class UIIcosahedronView : public UIAbstractScroll {
public:
    UIIcosahedronView();
    virtual ~UIIcosahedronView();

    /**
     * @brief Obtains the view type.
     * @return Returns the view type.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_ICOSAHEDRON_VIEW;
    }

    /**
     * @brief Set the edge length of the icosahedron.
     *
     * @param sideLen edge length of body shape.
     */
    void SetSideLength(uint16_t sideLen);

    /**
     * @brief Add a one-sided view to the icosahedron.
     *
     * @param view Single-sided corresponding view.Only supported ui image view.
     */
    void Add(UIView* view) override;

    /**
     * @brief Refresh icosahedral position.physique position will be reset after refreshing
     */
    void RefreshIcosahedron();

    /**
     * @brief Rotates the icosahedral in 3d, from the start position.
     *
     * @param angle Indicates the rotation angle.
     * @param pivotStart Indicates the coordinates of the rotation start pivot.
     * @param pivotEnd Indicates the coordinates of the rotation end pivot.
     */
    bool RotatePlanes(int16_t angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd);

    /**
     * @brief Rotates the icosahedral in 3d, from the current location.
     *
     * @param angle Indicates the rotation angle.
     * @param pivotStart Indicates the coordinates of the rotation start pivot.
     * @param pivotEnd Indicates the coordinates of the rotation end pivot.
     */
    bool Rotate(int16_t angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd);

    /**
     * @brief Set default Image,the default picture will be used if less than 20 faces are set
     * @param src Pointer to a ImageInfo struct, which is the source img for kaleidoscope img.
     *  the size of img must match the size of screen.
     */
    void SetDefaultImage(ImageInfo* src)
    {
        defaultSrc_ = src;
    }

    /**
     * @brief set luminance compensation factor, adjust brightness.
     *
     * @param value Indicates luminance compensation factor.
     *  Multiple of the original base compensation setting.
     */
    void SetLuminanceFactor(float value)
    {
        luminanceFactor_ = value;
    }
    /**
     * @brief Draws this image view.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
protected:
    bool OnDragStartEvent(const DragEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;
    bool DragXInner(int16_t distance) override
    {
        return true;
    }

    bool DragYInner(int16_t distance) override
    {
        return true;
    }

private:
    class inertiaAnimatorCallback : public AnimatorCallback {
    public:
        inertiaAnimatorCallback() : rotateTimes_(0), curtTime_(0), angle_(0)
        {
        }

        virtual ~inertiaAnimatorCallback() {}

        void SetRotateTimes(uint16_t times)
        {
            curtTime_ = 0;
            rotateTimes_ = times;
        }

        void SetRotateAngle(int16_t angle)
        {
            angle_ = angle;
        }

        void SetRotateAxis(const Vector3<float>& rotateStart, const Vector3<float>& rotateEnd)
        {
            rotateStart_ = rotateStart;
            rotateEnd_ = rotateEnd;
        }

        virtual void Callback(UIView* view) override;

    private:
        uint16_t rotateTimes_;
        uint16_t curtTime_;
        int16_t angle_;
        Vector3<float> rotateStart_;
        Vector3<float> rotateEnd_;
    };
    Matrix4<float> CalculateRotateMatrix(TriangularPlane dst);
    uint8_t GetCurrentPlaneOpt(Vector4<float> normal);

    void AdjustScaleAndTranslate(Vector3<float>& scale,
        Vector3<int16_t>& translate, int16_t widgetWidth, int16_t widgetHeight);
    Point startPoint_;
    uint16_t sideLen_;

    TriangularPlane plane_[20]; // 20: plane number
    Matrix4<float> imgMatrix_;
    Matrix4<float> sphereMatrix_;
    Matrix4<float> lastSphereMatrix_;
    bool isUpdateSideLen_ = true;

    Animator inertiaAnimator_;
    inertiaAnimatorCallback inertiaCallback_;
    float lastAngle_;
    ImageInfo* defaultSrc_;
    float luminanceFactor_;
};
} // namespace OHOS
#endif  // UI_ICOSAHEDRON_H
