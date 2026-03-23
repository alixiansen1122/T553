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

#ifndef ICON_ADAPTER_H
#define ICON_ADAPTER_H

#include "components/abstract_adapter.h"
#include "gfx_utils/list.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

namespace OHOS {
struct IconItemNode {
    char* imagePath;
    char* txt;
};

class IconAdapter : public AbstractAdapter {
public:
    IconAdapter() {}

    ~IconAdapter() override
    {
        if (fontName_ != nullptr) {
            UIFree(fontName_);
            fontName_ = nullptr;
        }
        ClearText();
    }

    void SetItemSize(int16_t width, int16_t height)
    {
        width_ = width;
        height_ = height;
    }

    void SetImageSize(int16_t width, int16_t height)
    {
        imageWidth_ = width;
        imageHeight_ = height;
    }

    void SetImageOffset(int16_t offsetX, int16_t offsetY)
    {
        imageOffsetX_ = offsetX;
        imageOffsetY_ = offsetY;
    }

    UIView* GetView(UIView* inView, int16_t index) override;
    uint16_t GetCount() override;
    void SetData(const List<IconItemNode*>* data);
    void SetFont(const char* name, uint8_t size);

    class ViewContainer : public UIViewGroup {
    public:
        ViewContainer(const ViewContainer &) = delete;
        ViewContainer &operator=(const ViewContainer &) = delete;
        ViewContainer() : image_(nullptr), label_(nullptr) {}
        ~ViewContainer() override
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
        UIImageView* image_ = nullptr;
        UILabel* label_ = nullptr;
    };
private:
    void ClearText(void);
    void FreeNode(IconItemNode* node);
    List<IconItemNode*> nodeList_;
    uint16_t imageWidth_ = 0;
    uint16_t imageHeight_ = 0;
    uint16_t imageOffsetX_ = 0;
    uint16_t imageOffsetY_ = 0;
    uint16_t width_ = 0;
    uint16_t height_ = 0;
    char* fontName_ = nullptr;
    uint8_t fontSize_ = 0;
};
} // namespace OHOS
#endif // ICON_ADAPTER_H