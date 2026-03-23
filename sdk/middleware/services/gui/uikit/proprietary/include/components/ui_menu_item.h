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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_menu_item.h
 *
 * @brief Extend UIMenuItem ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_MENU_ITEM_H
#define GRAPHIC_LITE_UI_MENU_ITEM_H
#include "graphic_config.h"
#include "components/ui_view_group.h"
#include "components/ui_label_ext.h"
#include "components/ui_image_view.h"

namespace OHOS {
/**
 * @brief Class of ui menu item.
 *
 * @since 1.0
 * @version 1.0
 */
class UIMenuItem : public UIViewGroup {
public:
    UIMenuItem(const UIMenuItem &) = delete;
    UIMenuItem &operator=(const UIMenuItem &) = delete;

    /**
     * @brief Scale align mode.
     */
    enum class ScaleAlignMode {
        ALIGN_TEXT_LEFT,
        ALIGN_IMAGE_LEFT,
        ALIGN_ITEM_CENTER
    };

    /**
     * @brief A constructor used to create a <b>UIMenuItem</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIMenuItem();

    /**
     * @brief A destructor used to delete the <b>UIMenuItem</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~UIMenuItem() override;

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_MENU_ITEM</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_MENU_ITEM;
    }

    /**
     * @brief Sets the image path for menu image.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetImageSrc(const char* imgSrc);

    /**
     * @brief Sets the image info for menu image.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetImageSrc(const ImageInfo* imgSrc);

    /**
     * @brief Sets the text for this item.
     *
     * @param text Indicates the pointer to the text.
     * @since 1.0
     * @version 1.0
     */
    void SetText(const char* text)
    {
        InitUILabelExt();
        label_->SetText(text);
        ReMeasure();
    }
    /**
     * @brief Sets the alignment mode for this text.
     *
     * @param align Indicates the text horizontal alignment mode to set, as defined in {@link UITextLanguageAlignment}.
     * @since 1.0
     * @version 1.0
     */
    void SetAlign(UITextLanguageAlignment align)
    {
        InitUILabelExt();
        label_->SetAlign(align, TEXT_ALIGNMENT_CENTER);
    }

    /**
     * @brief Sets the direction for this text.
     *
     * @param direct Indicates the text direction to set, as defined in {@link UITextLanguageDirect}.
     * @since 1.0
     * @version 1.0
     */
    void SetDirect(UITextLanguageDirect direct)
    {
        InitUILabelExt();
        label_->SetDirect(direct);
    }

    /**
     * @brief Sets the style for text.
     *
     * @param labelStyle Indicates the label style to set.
     * @since 1.0
     * @version 1.0
     */
    void SetLabelStyle(Style& labelStyle)
    {
        InitUILabelExt();
        label_->SetStyle(labelStyle);
    }

    /**
     * @brief Sets a style for text.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @since 1.0
     * @version 1.0
     */
    void SetLabelStyle(uint8_t key, int64_t value)
    {
        InitUILabelExt();
        label_->SetStyle(key, value);
    }

    /**
     * @brief Sets the color for text.
     *
     * @param color Indicates the text color to set.
     * @since 1.0
     * @version 1.0
     */
    void SetTextColor(ColorType color)
    {
        InitUILabelExt();
        label_->SetTextColor(color);
    }

    /**
     * @brief Sets the font for text.
     *
     * @param name Indicates the pointer to the font name.
     * @param size Indicates the font size to set.
     * @since 1.0
     * @version 1.0
     */
    void SetFont(const char* name, uint8_t size)
    {
        InitUILabelExt();
        label_->SetFont(name, size);
    }

    /**
     * @brief Sets the font ID.
     *
     * @param fontId Indicates the font ID composed of the font name and size.
     * @since 1.0
     * @version 1.0
     */
    void SetFontId(uint8_t fontId)
    {
        InitUILabelExt();
        label_->SetFontId(fontId);
    }

    /**
     * @brief Sets the line break mode for this text.
     *
     * @param lineBreakMode Indicates the line break mode to set.
     * @since 1.0
     * @version 1.0
     */
    void SetLineBreakMode(const uint8_t lineBreakMode)
    {
        labelLineBreakMode_ = lineBreakMode;
        needRefresh_ = true;
    }

    /**
     * @brief Set the text id.
     *
     * @param textId Indicates text id.
     * @since 1.0
     * @version 1.0
     */
    void SetTextId(uint16_t textId)
    {
        InitUILabelExt();
        label_->SetTextId(textId);
        ReMeasure();
    }

    /**
     * @brief Set image resize mode.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetImgResizeMode(UIImageView::ImageResizeMode mode)
    {
        resizeMode_ = mode;
        if (image_ != nullptr) {
            image_->SetResizeMode(resizeMode_);
        }
    }

    /**
     * @brief Set image offset(relative to the item left boundary).
     *
     * @since 1.0
     * @version 1.0
     */
    void SetImgOffset(int16_t offset)
    {
        imgOffset_ = offset;
        needRefresh_ = true;
    }

    /**
     * @brief Set text offset (relative to the image right boundary , or if there is no image,
     *        it is relative to the item left boundary).
     *
     * @since 1.0
     * @version 1.0
     */
    void SetTextOffset(int16_t offset)
    {
        textOffset_ = offset;
        needRefresh_ = true;
    }

    /**
     * @brief Set the transform align mode for this item.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetScaleAlignMode(ScaleAlignMode mode)
    {
        transAlignMode_ = mode;
        needRefresh_ = true;
    }

    /**
     * @brief Get the transform center this item.
     *
     * @since 1.0
     * @version 1.0
     */
    Vector2<float> GetScaleCenter();

    void ReMeasure() override;
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    void DumpImgInfo() override;

private:
    void InitUILabelExt();
    void InitUIImageView();

    UIImageView* image_;
    UILabelExt* label_;
    uint8_t labelLineBreakMode_;
    bool needRefresh_;
    int16_t imgOffset_ = 0;
    int16_t textOffset_ = 20; // 20: default text offset.
    ScaleAlignMode transAlignMode_ = ScaleAlignMode::ALIGN_ITEM_CENTER;
    UIImageView::ImageResizeMode resizeMode_ = UIImageView::CONTAIN;
};
}
#endif // GRAPHIC_LITE_UI_MENU_ITEM_H
/**
 * @}
 */
