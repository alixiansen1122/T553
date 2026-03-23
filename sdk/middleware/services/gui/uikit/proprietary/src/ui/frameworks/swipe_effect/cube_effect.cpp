/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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
#include "securec.h"
#include "common/screen.h"
#include "cube_effect.h"

namespace OHOS {
namespace {
static constexpr int16_t EASE_RANGE = 100;
static constexpr int16_t CAMERA_DISTANCE = 500;
static constexpr int16_t ROTATE_RANGE_MAX = 90;
}

void CubeEffect::StopEffect()
{
    if (preLeftCard != nullptr) {
        preLeftCard->ResetTransParameter();
        preLeftCard = nullptr;
        container_->UIViewGroup::Remove(&leftImg_);
        leftImg_.ResetTransParameter();
    }

    if (preRightCard != nullptr) {
        preRightCard->ResetTransParameter();
        preRightCard = nullptr;
        container_->UIViewGroup::Remove(&rightImg_);
        rightImg_.ResetTransParameter();
    }

    CardSwipe::StopEffect();
}

void CubeEffect::TransformCard(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    int16_t width = Screen::GetInstance().GetWidth();

    float angle = ROTATE_RANGE_MAX * MATH_ABS(xOffset) / (float)width;

    uint16_t leftW = leftCard->GetWidth();
    Vector3<float> leftRotateStart = {leftW + 1, 0, 0};
    Vector3<float> leftRotateEnd = {leftW + 1, 1, 0};
    Vector3<float> rightRotateStart = {-1, 0, 0};
    Vector3<float> rightRotateEnd = {-1, 1, 0};
    if (xOffset > 0) {
        leftCard->Rotate((angle - ROTATE_RANGE_MAX), leftRotateStart, leftRotateEnd);
        rightCard->Rotate(angle, rightRotateStart, rightRotateEnd);
    } else if (xOffset < 0) {
        leftCard->Rotate(-angle, leftRotateStart, leftRotateEnd);
        rightCard->Rotate((ROTATE_RANGE_MAX - angle), rightRotateStart, rightRotateEnd);
    } else {
        leftCard->ResetTransParameter();
        rightCard->ResetTransParameter();
    }
}

void CubeEffect::TransformMirrorCard(UIView& leftCard, UIView& rightCard, int16_t xOffset)
{
    int16_t width = Screen::GetInstance().GetWidth();
    int16_t height = Screen::GetInstance().GetHeight();
    float angle = ROTATE_RANGE_MAX * MATH_ABS(xOffset) / (float)width;

    Matrix4<float> perspectiveMatrix;
    perspectiveMatrix[2][2] = 0;            // 2 : index
    perspectiveMatrix[2][3] = -1.0f / CAMERA_DISTANCE; // 2 3 : index 1000:cam distance

    int16_t leftRigth = leftCard.GetOrigRect().GetRight();
    int16_t leftBottom = leftCard.GetOrigRect().GetBottom();
    Matrix4<float> leftMatrix = Matrix4<float>::Rotate(180.0f, {0, leftBottom, 0}, {1.0, leftBottom, 0});
    Vector3<float> leftRotateStart = {leftRigth, 0, 0};
    Vector3<float> leftRotateEnd = {leftRigth, 1, 0};
    Matrix4<float> leftTranslateFromCamera = Matrix4<float>::Translate(Vector3<float>(-leftRigth, -(height >> 1), 0));
    Matrix4<float> leftTranslateToCamera = Matrix4<float>::Translate(Vector3<float>(leftRigth, (height >> 1), 0));

    int16_t rightBottom = rightCard.GetOrigRect().GetBottom();
    Matrix4<float> rightMatrix = Matrix4<float>::Rotate(180.0f, {0, rightBottom, 0}, {1.0, rightBottom, 0});
    Vector3<float> rightRotateStart = {0, 0, 0};
    Vector3<float> rightRotateEnd = {0, 1, 0};
    Matrix4<float> rightTranslateFromCamera = Matrix4<float>::Translate(Vector3<float>(0, -(height >> 1), 0));
    Matrix4<float> rightTranslateToCamera = Matrix4<float>::Translate(Vector3<float>(0, (height >> 1), 0));
    if (xOffset > 0) {
        leftMatrix = Matrix4<float>::Rotate((angle - ROTATE_RANGE_MAX), leftRotateStart, leftRotateEnd) * leftMatrix;
        leftMatrix = leftTranslateToCamera * (perspectiveMatrix * leftTranslateFromCamera) * leftMatrix;
        leftMatrix = Matrix4<float>::Translate({-(width - xOffset), 0, 0}) * leftMatrix;

        rightMatrix = Matrix4<float>::Rotate(angle, rightRotateStart, rightRotateEnd) * rightMatrix;
        rightMatrix = rightTranslateToCamera * (perspectiveMatrix * rightTranslateFromCamera) * rightMatrix;
        rightMatrix = Matrix4<float>::Translate({xOffset, 0, 0}) * rightMatrix;
    } else {
        leftMatrix = Matrix4<float>::Rotate(-angle, leftRotateStart, leftRotateEnd) * leftMatrix;
        leftMatrix = leftTranslateToCamera * (perspectiveMatrix * leftTranslateFromCamera) * leftMatrix;
        leftMatrix = Matrix4<float>::Translate({xOffset, 0, 0}) * leftMatrix;

        rightMatrix =
            Matrix4<float>::Rotate((ROTATE_RANGE_MAX - angle), rightRotateStart, rightRotateEnd) * rightMatrix;
        rightMatrix = rightTranslateToCamera * (perspectiveMatrix * rightTranslateFromCamera) * rightMatrix;
        rightMatrix = Matrix4<float>::Translate({width + xOffset, 0, 0}) * rightMatrix;
    }
    TransformMap tempTrans;
    tempTrans.SetMatrix(leftMatrix);
    leftCard.SetTransformMap(tempTrans);

    tempTrans.SetMatrix(rightMatrix);
    rightCard.SetTransformMap(tempTrans);
}

void CubeEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    if (leftCard == nullptr || rightCard == nullptr) {
        return;
    }

    if (preLeftCard == nullptr) {
        leftImg_.SetStyle(STYLE_IMAGE_OPA, 90); // 90: degree
        rightImg_.SetStyle(STYLE_IMAGE_OPA, 90); // 90: degree
        container_->UIViewGroup::Add(&leftImg_);
        container_->UIViewGroup::Add(&rightImg_);
    }

    int16_t width = Screen::GetInstance().GetWidth();
    int16_t height = Screen::GetInstance().GetHeight();
    int16_t middleWidth = width / 2;
    if (leftCard != preLeftCard && rightCard != preRightCard) {
        leftImg_.SetPosition(0, 0, width, height);
        if (leftCard->GetViewType() == UI_IMAGE_VIEW) {
            leftImg_.SetAutoEnable(false);
            leftImg_.SetResizeMode(UIImageView::FILL);
            leftImg_.SetSrc(static_cast<UIImageView *>(leftCard)->GetImageInfo());
        }
        preLeftCard = leftCard;
        preLeftCard->SetCameraPosition(Vector2<float>(leftCard->GetWidth(), leftCard->GetHeight() / 2)); // 2: half
        preLeftCard->SetCameraDistance(CAMERA_DISTANCE);
        rightImg_.SetPosition(0, 0, width, height);
        if (rightCard->GetViewType() == UI_IMAGE_VIEW) {
            rightImg_.SetAutoEnable(false);
            rightImg_.SetResizeMode(UIImageView::FILL);
            rightImg_.SetSrc(static_cast<UIImageView *>(rightCard)->GetImageInfo());
        }
        preRightCard = rightCard;
        preRightCard->SetCameraPosition(Vector2<float>(0, rightCard->GetHeight() / 2)); // 2: half
        preRightCard->SetCameraDistance(CAMERA_DISTANCE);
    }

    TransformCard(preLeftCard, preRightCard, xOffset);
    TransformMirrorCard(leftImg_, rightImg_, xOffset);
}

void CubeEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void CubeEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}
}
