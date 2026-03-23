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

#include "menu_item_adapter.h"
#include "components/ui_menu_item.h"
#include "common/image_cache_manager.h"
#include "graphic_config.h"
#include "ui_test_list_ext.h"
#include "securec.h"
#include "ui_test_list_fps/ui_list_fps_res.h"

namespace OHOS {
uint16_t MenuItemAdapter::GetCount()
{
    return nodeList_.Size();
}

void MenuItemAdapter::ClearText()
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

void MenuItemAdapter::SetFont(const char* name, uint8_t size)
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

void MenuItemAdapter::FreeNode(ItemNode* node)
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

void MenuItemAdapter::SetData(const List<ItemNode*>* data)
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
        char *txt = static_cast<char*>(UIMalloc(len + 1));
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

UIView* MenuItemAdapter::GetView(UIView* inView, int16_t index)
{
    if (nodeList_.IsEmpty() || index > nodeList_.Size() - 1 || index < 0) {
        return nullptr;
    }

    ListNode<ItemNode*>* node = nodeList_.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }
    UIMenuItem* item = nullptr;
    if (inView == nullptr) {
        item = new UIMenuItem();
        if (item == nullptr) {
            return nullptr;
        }
        item->Resize(width_, height_);
        item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        item->SetImgResizeMode(UIImageView::ImageResizeMode::CENTER);
        item->SetFont(fontName_, fontSize_);
        item->SetScaleAlignMode(UIMenuItem::ScaleAlignMode::ALIGN_ITEM_CENTER);
        item->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_MARQUEE);
    } else {
        item = static_cast<UIMenuItem*>(inView);
    }

    if (item != nullptr) {
        if (node->data_->imgInfo == nullptr) {
            node->data_->imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(node->data_->resId, LIST_FPS);
        }
        item->SetImageSrc(node->data_->imgInfo);
        item->SetText(node->data_->txt);
        item->SetAlign(TEXT_ALIGNMENT_CENTER);
        item->SetViewIndex(index);
    }
    return item;
}
} // namespace OHOS
