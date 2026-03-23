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

#include "components/ui_menu_item.h"
#include "components/root_view.h"
namespace OHOS {

UIMenuItem::UIMenuItem() : image_(nullptr), label_(nullptr),
    labelLineBreakMode_(UILabel::LineBreakMode::LINE_BREAK_ELLIPSIS), needRefresh_(false)
{
}

UIMenuItem::~UIMenuItem()
{
    if (image_ != nullptr) {
        Remove(image_);
        delete image_;
        image_ = nullptr;
    }

    if (label_ != nullptr) {
        Remove(label_);
        delete label_;
        label_ = nullptr;
    }
}

Vector2<float> UIMenuItem::GetScaleCenter()
{
    if (transAlignMode_ == ScaleAlignMode::ALIGN_TEXT_LEFT) {
        return Vector2<float>{label_->GetX(), label_->GetHeight() / 2.0f};
    } else if (transAlignMode_ == ScaleAlignMode::ALIGN_IMAGE_LEFT) {
        return Vector2<float>{image_->GetX(), image_->GetHeight() / 2.0f};
    } else {
        return Vector2<float>{GetWidth() / 2.0f, GetHeight() / 2.0f};
    }
}

void UIMenuItem::ReMeasure()
{
    if (needRefresh_) {
        if (image_ != nullptr) {
            image_->SetPosition(imgOffset_, 0, GetHeight(), GetHeight());
        }
        if (label_ != nullptr) {
            if (image_ != nullptr) {
                label_->SetPosition(image_->GetX() + image_->GetWidth() + textOffset_, 0,
                    GetWidth() - image_->GetWidth(), GetHeight());
            } else {
                label_->SetPosition(textOffset_, 0, GetWidth(), GetHeight());
            }
            label_->SetLineBreakMode(labelLineBreakMode_);
        }
    }
    needRefresh_ = false;

    TransformMap transMap;
    if (image_ != nullptr) {
        transMap.SetMatrix(GetTransformMap().GetOrigTransformMatrix());
        image_->SetTransformMap(transMap);
    }

    if (label_ != nullptr) {
        transMap.SetMatrix(GetTransformMap().GetOrigTransformMatrix());
        label_->SetTransformMap(transMap);
    }
}

void UIMenuItem::SetImageSrc(const char* imgSrc)
{
    InitUIImageView();
    image_->SetSrc(imgSrc);
    ReMeasure();
}

void UIMenuItem::SetImageSrc(const ImageInfo* imgSrc)
{
    InitUIImageView();
    image_->SetSrc(imgSrc);
    ReMeasure();
}

void UIMenuItem::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
}

void UIMenuItem::InitUIImageView()
{
    if (image_ == nullptr) {
        image_ = new UIImageView();
        if (image_ == nullptr) {
            GRAPHIC_LOGE("new UIImageView failed");
            return;
        }
        image_->SetResizeMode(resizeMode_);
        image_->SetAutoEnable(false);
        Add(image_);
        needRefresh_ = true;
    }
}

void UIMenuItem::InitUILabelExt()
{
    if (label_ == nullptr) {
        label_ = new UILabelExt();
        if (label_ == nullptr) {
            GRAPHIC_LOGE("new UILabelExt failed");
            return;
        }
        Add(label_);
        needRefresh_ = true;
    }
}

void UIMenuItem::DumpImgInfo()
{
    if (image_ == nullptr) {
        return;
    }
    printf("view[%p]: viewType = %d\n", this, GetViewType());
    const ImageInfo* info = image_->GetImageInfo();
    PrintImgInfo(info);
}
} // namespace OHOS
