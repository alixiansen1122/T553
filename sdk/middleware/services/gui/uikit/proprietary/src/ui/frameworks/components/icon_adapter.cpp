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

#include "components/icon_adapter.h"
#include "securec.h"

namespace OHOS {
uint16_t IconAdapter::GetCount()
{
    return nodeList_.Size();
}

void IconAdapter::ClearText()
{
    ListNode<IconItemNode*>* node = nodeList_.Begin();
    while (node != nodeList_.End()) {
        if (node->data_) {
            if (node->data_->txt) {
                UIFree(reinterpret_cast<void*>(node->data_->txt));
                node->data_->txt = nullptr;
            }
            UIFree(reinterpret_cast<void*>(node->data_));
            node->data_ = nullptr;
        }
        node = node->next_;
    }
    nodeList_.Clear();
}

void IconAdapter::SetFont(const char* name, uint8_t size)
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

void IconAdapter::FreeNode(IconItemNode* node)
{
    if (node != nullptr) {
        if (node->txt) {
            UIFree(reinterpret_cast<void*>(node->txt));
            node->txt = nullptr;
        }
        UIFree(reinterpret_cast<void*>(node));
        node = nullptr;
    }
}

void IconAdapter::SetData(const List<IconItemNode*>* data)
{
    if (data == nullptr) {
        return;
    }
    if (!nodeList_.IsEmpty()) {
        ClearText();
    }
    ListNode<IconItemNode*>* node = data->Begin();
    while (node != data->End()) {
        IconItemNode* tmpNode = static_cast<IconItemNode *>(UIMalloc(sizeof(IconItemNode)));
        if (tmpNode == nullptr) {
            return;
        }

        tmpNode->imagePath = node->data_->imagePath;
        uint32_t len =  strlen(node->data_->txt);
        tmpNode->txt = static_cast<char*>(UIMalloc(len + 1));
        if (tmpNode->txt == nullptr) {
            FreeNode(tmpNode);
            return;
        }
        if (memcpy_s(tmpNode->txt, len + 1, node->data_->txt, len) != EOK) {
            FreeNode(tmpNode);
            return;
        }
        tmpNode->txt[len] = '\0';
        nodeList_.PushBack(tmpNode);
        node = node->next_;
    }
}

UIView* IconAdapter::GetView(UIView* inView, int16_t index)
{
    if (nodeList_.IsEmpty() || index > nodeList_.Size() - 1 || index < 0) {
        return nullptr;
    }

    ListNode<IconItemNode*>* node = nodeList_.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }

    ViewContainer* newView = nullptr;
    if (inView == nullptr) {
        newView = new ViewContainer();
        if (newView == nullptr) {
            return nullptr;
        }
        if (!newView->InitContainer()) {
            delete newView;
            return nullptr;
        }
        newView->Resize(width_, height_);
        newView->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        UIImageView* image = newView->GetImageView();
        image->SetAutoEnable(false);
        image->SetPosition(imageOffsetX_, imageOffsetY_, imageWidth_, imageHeight_);
        image->SetResizeMode(UIImageView::ImageResizeMode::CENTER);
        UILabel* label = newView->GetLabelView();
        uint16_t labelWidth = (width_ > (imageWidth_ + image->GetX())) ? (width_ - imageWidth_ - image->GetX()) : 0;
        label->SetPosition(image->GetX() + imageWidth_, imageOffsetY_, labelWidth, height_);
        label->SetFont(fontName_, fontSize_);
        label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        label->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    } else {
        newView = static_cast<ViewContainer*>(inView);
    }

    if (newView != nullptr) {
        UIImageView* image = newView->GetImageView();
        image->SetSrc(node->data_->imagePath);
        UILabel* label = newView->GetLabelView();
        label->SetText(node->data_->txt);
        newView->SetViewIndex(index);
    }

    return newView;
}
} // namespace OHOS