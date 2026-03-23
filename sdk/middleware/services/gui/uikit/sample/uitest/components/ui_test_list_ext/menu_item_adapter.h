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

#ifndef MENU_ITEM_ADAPTER_H
#define MENU_ITEM_ADAPTER_H
#include "menu_item_adapter.h"
#include "components/abstract_adapter.h"
#include "components/ui_image_view.h"
#include "gfx_utils/list.h"
#include "ui_test_list_fps/image_text_adapter.h"

namespace OHOS {
class MenuItemAdapter : public AbstractAdapter {
public:
    MenuItemAdapter() : imageWidth_(0), imageHeight_(0), width_(0), height_(0), fontName_(nullptr), fontSize_(0) {}

    ~MenuItemAdapter() override
    {
        if (fontName_ != nullptr) {
            UIFree(fontName_);
            fontName_ = nullptr;
        }
        ClearText();
    }

    void SetWidth(int16_t width)
    {
        width_ = width;
    }

    void SetHeight(int16_t height)
    {
        height_ = height;
    }

    void SetImageWidth(int16_t width)
    {
        imageWidth_ = width;
    }

    void SetImageHeight(int16_t height)
    {
        imageHeight_ = height;
    }

    UIView* GetView(UIView* inView, int16_t index) override;
    uint16_t GetCount() override;
    void SetData(const List<ItemNode*>* data);
    void SetFont(const char* name, uint8_t size);

private:
    void ClearText(void);
    void FreeNode(ItemNode* node);
    List<ItemNode*> nodeList_;
    uint16_t imageWidth_;
    uint16_t imageHeight_;
    uint16_t width_;
    uint16_t height_;
    char* fontName_;
    uint8_t fontSize_;
};
} // namespace OHOS
#endif // IMAGE_TEXT_ADAPTER_H
