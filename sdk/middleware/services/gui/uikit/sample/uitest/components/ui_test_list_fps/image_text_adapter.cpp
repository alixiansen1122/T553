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

#include "image_text_adapter.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "components/ui_transform_button.h"
#include "common/image_cache_manager.h"
#include "securec.h"
#include "graphic_config.h"
#include "ui_list_fps_res.h"

namespace OHOS {
class ImageTextContainer : public UITransformButton {
public:
    ImageTextContainer() : image_(nullptr), label_(nullptr) {}
    ~ImageTextContainer() override
    {
        this->RemoveAll();
        if (image_ != nullptr) {
            delete image_;
            image_ = nullptr;
        }
        if (label_ != nullptr) {
            delete label_;
            label_ = nullptr;
        }
    }
    ImageTextContainer(const ImageTextContainer &) = delete;
    ImageTextContainer &operator=(const ImageTextContainer &) = delete;

    bool InitContainer()
    {
        if (image_ == nullptr) {
            image_ = new UIImageView();
            if (image_ == nullptr) {
                return false;
            }
        }
        Add(image_);

        if (label_ == nullptr) {
            label_ = new UILabel();
            if (label_ == nullptr) {
                Remove(image_);
                delete image_;
                image_ = nullptr;
                return false;
            }
        }
        Add(label_);

        return true;
    }

    UIImageView* GetImageView()
    {
        return image_;
    }

    UILabel* GetLabelView()
    {
        return label_;
    }

private:
    UIImageView* image_;
    UILabel* label_;
};

uint16_t ImageTextAdapter::GetCount()
{
    return nodeList_.Size();
}

void ImageTextAdapter::ClearText()
{
    ListNode<ItemNode*>* node = nodeList_.Begin();
    while (node != nodeList_.End()) {
        if (node->data_) {
            if (node->data_->txt) {
                UIFree(reinterpret_cast<void*>(const_cast<char*>(node->data_->txt)));
                node->data_->txt = nullptr;
            }
            UIFree(reinterpret_cast<void*>(node->data_));
            node->data_ = nullptr;
        }
        node = node->next_;
    }
    nodeList_.Clear();
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(LIST_FPS);
}

void ImageTextAdapter::SetFont(const char* name, uint8_t size)
{
    if (name == nullptr) {
        return;
    }

    if (fontName_ != nullptr) {
        if (strcmp(fontName_, name) == 0) {
            fontSize_ = size;
            return;
        } else {
            UIFree(fontName_);
            fontName_ = nullptr;
        }
    }

    uint32_t nameLen = static_cast<uint32_t>(strlen(name));
    if ((nameLen == 0) || (nameLen > MAX_TEXT_LENGTH)) {
        return;
    }

    fontName_ = static_cast<char*>(UIMalloc(nameLen + 1));
    if (fontName_ == nullptr) {
        return;
    }
    if (memcpy_s(fontName_, nameLen, name, nameLen) != EOK) {
        UIFree(fontName_);
        fontName_ = nullptr;
        return;
    }
    fontName_[nameLen] = '\0';
    fontSize_ = size;
}

void ImageTextAdapter::FreeNode(ItemNode* node)
{
    if (node != nullptr) {
        if (node->txt) {
            UIFree(reinterpret_cast<void*>(const_cast<char*>(node->txt)));
            node->txt = nullptr;
        }
        UIFree(reinterpret_cast<void*>(node));
        node = nullptr;
    }
}

void ImageTextAdapter::SetData(const List<ItemNode*>* data)
{
    if (data == nullptr) {
        return;
    }
    if (!nodeList_.IsEmpty()) {
        ClearText();
    }
    ListNode<ItemNode*>* node = data->Begin();
    while (node != data->End()) {
        ItemNode* tmpNode = static_cast<ItemNode *>(UIMalloc(sizeof(ItemNode)));
        if (tmpNode == nullptr) {
            return;
        }

        tmpNode->resId = node->data_->resId;
        uint32_t len =  strlen(node->data_->txt);
        char *txt = static_cast<char *>(UIMalloc(len + 1));
        if (txt == nullptr) {
            FreeNode(tmpNode);
            return;
        }
        if (memcpy_s(txt, len + 1, node->data_->txt, len) != EOK) {
            UIFree(txt);
            FreeNode(tmpNode);
            return;
        }
        txt[len] = '\0';
        tmpNode->txt = txt;
        tmpNode->imgInfo = nullptr;

        nodeList_.PushBack(tmpNode);
        node = node->next_;
    }
}

UIView* ImageTextAdapter::GetView(UIView* inView, int16_t index)
{
    if (nodeList_.IsEmpty() || index > nodeList_.Size() - 1 || index < 0) {
        return nullptr;
    }

    ListNode<ItemNode*>* node = nodeList_.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }

    ImageTextContainer* newView = nullptr;
    if (inView == nullptr) {
        newView = new ImageTextContainer();
        if (newView == nullptr) {
            return nullptr;
        }
        if (!newView->InitContainer()) {
            delete newView;
            return nullptr;
        }
        newView->Resize(width_, height_);
        newView->SetStateForStyle(UITransformButton::TransformButtonState::PRESSED);
        newView->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        newView->SetStateForStyle(UITransformButton::TransformButtonState::RELEASED);
        newView->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        UIImageView* image = newView->GetImageView();
        image->SetAutoEnable(false);
        image->SetPosition(80, 0, imageWidth_, imageHeight_); // 80: x, 0: y
        image->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
        UILabel* label = newView->GetLabelView();
        label->SetStyle(STYLE_BORDER_WIDTH, 1);
        label->SetPosition(image->GetX() + imageWidth_, 0, width_ - imageWidth_ - image->GetX(), height_);
        label->SetFont(fontName_, fontSize_);
        label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        label->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    } else {
        newView = static_cast<ImageTextContainer*>(inView);
    }

    if (newView != nullptr) {
        UIImageView* image = newView->GetImageView();
        if (node->data_->imgInfo == nullptr) {
            node->data_->imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(node->data_->resId, LIST_FPS);
        }
        image->SetSrc(node->data_->imgInfo);
        UILabel* label = newView->GetLabelView();
        label->SetText(node->data_->txt);
        newView->SetViewIndex(index);
    }

    return newView;
}
} // namespace OHOS
