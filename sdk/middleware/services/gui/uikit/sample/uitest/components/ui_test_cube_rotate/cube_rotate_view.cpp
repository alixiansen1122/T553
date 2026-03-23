/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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

#include "cube_rotate_view.h"
#include "common/image_cache_manager.h"

namespace OHOS {
static constexpr int16_t IMG_LEFT = 147;
static constexpr int16_t IMG_TOP = 110;
static constexpr int16_t IMG_RIGHT = 306;
static constexpr int16_t IMG_BOTTOM = 269;
static constexpr int16_t IMG_WIDTH = 160;
static constexpr int16_t IMG_HEIGHT = 160;
static constexpr int16_t IMG_SIZE = 160;
static constexpr float CENTER_X = 227.0;
static constexpr float CENTER_Y = 190.0;
static constexpr int ROTATE_DELTA = 1;
static constexpr int16_t CAM_DISTANCE = 1000;
// absolute position in the display rect
static const Vector2<float> CAM_POSITION = {CENTER_X, CENTER_Y};

static Vector3<float> CalPlaneNormal(Vector3<float> a, Vector3<float> b)
{
    float x = a.y_ * b.z_ - b.y_ * a.z_;
    float y = a.z_ * b.x_ - a.x_ * b.z_;
    float z = a.x_ * b.y_ - a.y_ * b.x_;
    return Vector3<float>(x, y, z);
}

void CubeAnimatorCallback::Callback(UIView *view)
{
    CubeRotateView* cubView = dynamic_cast<CubeRotateView*>(view);
    cubView->RotatePlanes(angle_, {IMG_LEFT, IMG_TOP, 0.0},
        {IMG_RIGHT, IMG_BOTTOM, -IMG_SIZE});
    angle_ += ROTATE_DELTA;
    if (angle_ == CIRCLE_IN_DEGREE) {
        angle_ = 0;
    }
}

CubeRotateView::CubeRotateView()
{
    if (CAM_DISTANCE != 0) {
        Matrix4<float> translateFromCamera = Matrix4<float>::Translate({-CAM_POSITION.x_, -CAM_POSITION.y_, 0});
        Matrix4<float> translateToCamera = Matrix4<float>::Translate({CAM_POSITION.x_, CAM_POSITION.y_, 0});
        perspectiveMatrix_[2][2] = 0; // 2 : index
        perspectiveMatrix_[2][3] = -1.0f / CAM_DISTANCE; // 2 3 : index
        perspectiveMatrix_ = translateToCamera * (perspectiveMatrix_ * translateFromCamera);
    }
}

CubeRotateView::~CubeRotateView()
{
    ClearAll();
}

void CubeRotateView::InitPlanes()
{
    // front plane
    CubeInfo info;
    info.img = new UIImageView();
    info.n = {0.0, 0.0, 1.0};
    cubePlane_.PushBack(info);

    // back plane
    info.img = new UIImageView();
    info.img->SetVisible(false);
    info.n = {0.0, 0.0, -1.0};
    info.matrix = Matrix4<float>::Translate({0.0, 0.0, -IMG_SIZE}) *
        Matrix4<float>::Rotate(SEMICIRCLE_IN_DEGREE, {CENTER_X, 0.0, 0.0}, {CENTER_X, 1.0, 0.0});
    cubePlane_.PushBack(info);

    // left plane
    info.img = new UIImageView();
    info.img->SetVisible(false);
    info.n = {-1.0, 0.0, 0.0};
    info.matrix =  Matrix4<float>::Translate({0.0, 0.0, -IMG_SIZE}) *
        Matrix4<float>::Rotate(-QUARTER_IN_DEGREE, {IMG_LEFT, 0.0, 0.0}, {IMG_LEFT, 1.0, 0.0});
    cubePlane_.PushBack(info);

    // right plane
    info.img = new UIImageView();
    info.img->SetVisible(false);
    info.n = {1.0, 0.0, 0.0};
    info.matrix =  Matrix4<float>::Translate({0.0, 0.0, -IMG_SIZE}) *
        Matrix4<float>::Rotate(QUARTER_IN_DEGREE, {IMG_RIGHT, 0.0, 0.0}, {IMG_RIGHT, 1.0, 0.0});
    cubePlane_.PushBack(info);

    // top plane
    info.img = new UIImageView();
    info.img->SetVisible(false);
    info.n = {0.0, -1.0, 0.0};
    info.matrix = Matrix4<float>::Translate({0.0, 0.0, -IMG_SIZE}) *
        Matrix4<float>::Rotate(QUARTER_IN_DEGREE, {0.0, IMG_TOP, 0.0}, {1.0, IMG_TOP, 0.0});
    cubePlane_.PushBack(info);

    // bottom plane
    info.img = new UIImageView();
    info.img->SetVisible(false);
    info.n = {0.0, 1.0, 0.0};
    info.matrix = Matrix4<float>::Translate({0.0, 0.0, -IMG_SIZE}) *
        Matrix4<float>::Rotate(-QUARTER_IN_DEGREE, {0.0, IMG_BOTTOM, 0.0}, {1.0, IMG_BOTTOM, 0.0});
    cubePlane_.PushBack(info);
}

void CubeRotateView::ClearAll()
{
    RemoveAll();
    if (animator != nullptr) {
        animator->Stop();
        delete animator;
        animator = nullptr;
    }
    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }
    if (controlButton_ != nullptr) {
        delete controlButton_;
        controlButton_ = nullptr;
    }
    for (int i = 0; i < cubePlane_.Size(); i++) {
        CubeInfo info = cubePlane_[i];
        if (info.img != nullptr) {
            delete info.img;
            info.img = nullptr;
        }
    }
    if (img_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"logo.bin");
        img_ = nullptr;
    }
}

bool CubeRotateView::InitView()
{
    InitPlanes();
    img_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"logo.bin");
    if (img_ == nullptr) {
        return false;
    }

    TransformMap tempTrans;
    for (int i = 0; i < cubePlane_.Size(); i++) {
        CubeInfo& info = cubePlane_[i];
        if (info.img == nullptr) {
            ClearAll();
            return false;
        }

        info.img->SetSrc(img_);
        info.img->SetPosition(IMG_LEFT, IMG_TOP, IMG_WIDTH, IMG_HEIGHT);
        info.img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
        info.img->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full); // R,G,B are all set to 200.
        tempTrans.SetMatrix(perspectiveMatrix_ * info.matrix);

        info.img->SetTransformMap(tempTrans);
        Add(info.img);
    }

    controlButton_ = new UILabelButton();
    if (controlButton_ == nullptr) {
        ClearAll();
        return false;
    }
    controlButton_->SetPosition(177, 350, 100, 50); // 177, 350, 100, 50: left, right, width, height
    controlButton_->SetText("Start");
    controlButton_->SetDragParentInstead(false);
    controlButton_->SetOnClickListener(this);
    Add(controlButton_);

    callBack_ = new CubeAnimatorCallback(this);
    if (callBack_ == nullptr) {
        ClearAll();
        return false;
    }
    animator = new Animator(callBack_, this, 0, true);
    if (animator == nullptr) {
        ClearAll();
        return false;
    }
    return true;
}

void CubeRotateView::RotatePlanes(int angle, const Vector3<float>& rotateStart, const Vector3<float>& rotateEnd)
{
    TransformMap tempTrans;
    Matrix4<float> planMatrix;
    matrix_ =  Matrix4<float>::Rotate(angle, rotateStart, rotateEnd) * lastPhaseMatrix_;
    bool isVisible = false;
    Vector4<float> center;

    for (int i = 0; i < cubePlane_.Size(); i++) {
        CubeInfo& info = cubePlane_[i];
        Vector4<float> normal = matrix_ * Vector4<float>(info.n.x_, info.n.y_, info.n.z_, 0);
        planMatrix =  matrix_ * info.matrix;
        if (CAM_DISTANCE > 0) {
            center = {CENTER_X, CENTER_Y, 0, 1};
            center = planMatrix * center;
            Vector3<float> cameraLine = {CAM_POSITION.x_ - center.x_, CAM_POSITION.y_ - center.y_,
                CAM_DISTANCE - center.z_};
            isVisible = ((normal.x_ * cameraLine.x_ +  normal.y_ * cameraLine.y_ + normal.z_ * cameraLine.z_) > 0);
        } else {
            isVisible = (normal.z_ > 0);
        }

        if (isVisible) {
            tempTrans.SetMatrix(perspectiveMatrix_ * planMatrix);
            info.img->SetTransformMap(tempTrans);
            info.img->SetVisible(true);
        } else {
            info.img->SetVisible(false);
        }
    }
}

bool CubeRotateView::OnClick(UIView& view, const ClickEvent& event)
{
    if (animator->GetState() == Animator::STOP) {
        controlButton_->SetText("Stop");
        animator->Start();
    } else if (animator->GetState() == Animator::START) {
        animator->Stop();
        controlButton_->SetText("Start");
    }
    return true;
}

bool CubeRotateView::OnDragStartEvent(const DragEvent& event)
{
    if (animator->GetState() == Animator::START) {
        animator->Stop();
        controlButton_->SetText("Start");
    }
    callBack_->angle_ = 0;
    lastPhaseMatrix_ = matrix_;
    startPoint_ = event.GetLastPoint();

    return true;
}

bool CubeRotateView::OnDragEvent(const DragEvent& event)
{
    Point point = event.GetCurrentPos();

    Vector3<float> curr = Vector3<float>(point.x - startPoint_.x, point.y - startPoint_.y, 0);
    Vector3<float> normal = CalPlaneNormal(Vector3<float>(0, 0, 1), curr);
    int x = (point.x - startPoint_.x);
    int y = (point.y - startPoint_.y);
    float len = Sqrt(x * x + y * y);
    float angle = RADIAN_TO_ANGLE * len / IMG_WIDTH;
    const Vector3<float> center = {CENTER_X, CENTER_Y, -IMG_SIZE / 2.0f};
    RotatePlanes(angle, center, {center.x_ + normal.x_, center.y_ + normal.y_, center.z_ + normal.z_});
    return true;
}

bool CubeRotateView::OnDragEndEvent(const DragEvent& event)
{
    lastPhaseMatrix_ = matrix_;
    return true;
}
}