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

#include "components/image_adapter.h"
#include "components/ui_view_group.h"
#include "securec.h"

namespace OHOS {
ImageAdapter::ImageAdapter() : imageWidth_(0), imageHeight_(0), width_(0), height_(0), clickListener_(nullptr)
{
    style_ = StyleDefault::GetBackgroundTransparentStyle();
}

ImageAdapter::~ImageAdapter()
{
    ClearData();
}

void ImageAdapter::ClearData()
{
    ListNode<const char*>* node = data_.Begin();
    while (node != data_.End()) {
        if (node->data_ != nullptr) {
            UIFree(reinterpret_cast<void*>(const_cast<char*>(node->data_)));
            node->data_ = nullptr;
        }
        node = node->next_;
    }
    data_.Clear();
}

void ImageAdapter::SetData(List<const char*>* data)
{
    if (data == nullptr) {
        return;
    }

    if (!data_.IsEmpty()) {
        ClearData();
    }

    ListNode<const char*>* node = data->Begin();
    while (node != data->End()) {
        uint32_t len = strlen(node->data_);
        char* stringData = static_cast<char *>(UIMalloc(len + 1));
        if (stringData == nullptr) {
            return;
        }
        if (memcpy_s(stringData, len + 1, node->data_, len) != EOK) {
            return;
        }
        stringData[len] = '\0';
        data_.PushBack(stringData);
        node = node->next_;
    }
}

UIView* ImageAdapter::GetView(UIView* inView, int16_t index)
{
    UIViewGroup* container = nullptr;
    UIImageView* imageView = nullptr;

    if (data_.IsEmpty() || (index > (data_.Size() - 1)) || (index < 0)) {
        GRAPHIC_LOGE("invalid state data size %u, index %d", data_.Size(), index);
        return nullptr;
    }

    if (inView != nullptr) {
        container = static_cast<UIViewGroup*>(inView);
        imageView = static_cast<UIImageView*>(container->GetChildrenHead());
    } else {
        container = new UIViewGroup();
        if (container == nullptr) {
            GRAPHIC_LOGE("container is null");
            return nullptr;
        }
        imageView = new UIImageView();
        if (imageView == nullptr) {
            delete container;
            container = nullptr;
            GRAPHIC_LOGE("imageView is null");
            return nullptr;
        }
        container->Add(imageView);
    }

    if ((imageView == nullptr) || (container == nullptr)) {
        GRAPHIC_LOGE("container or imageView is null");
        return nullptr;
    }
    container->Resize(width_, height_);
    SetImageViewInfo(*imageView, index);
    return container;
}

void ImageAdapter::SetImageViewInfo(UIImageView& imgView, int16_t index)
{
    ListNode<const char*>* node = data_.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }
    imgView.SetSrc(node->data_);
    imgView.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    imgView.SetAutoEnable(false);
    if (imageWidth_ != 0) {
        imgView.SetWidth(imageWidth_);
    }
    if (imageHeight_ != 0) {
        imgView.SetHeight(imageHeight_);
    }
    imgView.SetResizeMode(UIImageView::ImageResizeMode::FILL);
    imgView.SetViewIndex(index);
    if (clickListener_ != nullptr) {
        imgView.SetOnClickListener(clickListener_);
        imgView.SetTouchable(true);
    }
    imgView.SetPosition(int16_t((width_ - imageWidth_) / 2), // 2: half
        int16_t((height_ - imageHeight_) / 2)); // 2: half
}

uint16_t ImageAdapter::GetCount()
{
    return data_.Size();
}
}
