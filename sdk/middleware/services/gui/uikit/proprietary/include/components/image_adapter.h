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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file image_adapter.h
 *
 * @brief Defines a image adapter that is used to construct UIViewGroup and UIImageView instances.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef IMAGE_ADAPTER_H
#define IMAGE_ADAPTER_H

#include "components/ui_image_view.h"
#include "components/abstract_adapter.h"
#include "gfx_utils/list.h"

namespace OHOS {
/**
 * @brief Defines a image adapter, which implements UIView and UIImageView instance construction
 *        and supports image data.
 *
 * @since 1.0
 * @version 1.0
 */
class ImageAdapter : public AbstractAdapter {
public:
    /**
     * @brief A constructor used to create a <b>UIView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ImageAdapter();

    /**
     * @brief A destructor used to delete the <b>UIView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~ImageAdapter() override;

    /**
     * @brief Obtains a <b>UIView</b> instance to convert adapter data into another <b>UIView</b> instance.
     *
     * @param inView Indicates the pointer to the reusable instance. If this parameter is not <b>NULL</b>, a reusable
     *        <b>UILabel</b> instance is available. In this case, this function does not need to create a new
     *        <b>UILabel</b> instance, just resusing the instance specified by <b>inView</b> to update the <b>inView</b>
     *        data. If this parameter is <b>NULL</b>, there is no resuable <b>UIView</b> instance. In this case, this
     *        function needs to create a new <b>UILabel</b> instance.
     *
     * @param index Indicates the adapter data index.
     *
     * @return Returns the address of the <b>UIView</b> instance constructed by the adapter.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetView(UIView* inView, int16_t index) override;

    /**
     * @brief Sets the UIView adapter data, which is a string linked list.
     *
     * @param data Indicates the string linked list data.
     * @since 1.0
     * @version 1.0
     */
    void SetData(List<const char*>* data);

    /**
     * @brief Obtains the UIView adapter style.
     *
     * @return Returns the UIView adapter style.
     * @since 1.0
     * @version 1.0
     */
    Style& GetStyle()
    {
        return style_;
    }

    /**
     * @brief Obtains the <b>data</b> size of the UIView adapter.
     *
     * @return Returns the <b>data</b> size of the UIView adapter.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetCount() override;

    /**
     * @brief Sets the width of UIView constructed by the adapter.
     *
     * @return Returns the width of UIView.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width)
    {
        width_ = width;
    }

    /**
     * @brief Sets the height of UIView constructed by the adapter.
     *
     * @return Returns the height of UIView.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height)
    {
        height_ = height;
    }

    /**
     * @brief Sets the width of UIImageView constructed by the adapter.
     *
     * @return Returns the width of UIImageView.
     * @since 1.0
     * @version 1.0
     */
    void SetImageWidth(int16_t width)
    {
        imageWidth_ = width;
    }

    /**
     * @brief Sets the height of UIImageView constructed by the adapter.
     *
     * @return Returns the height of UIImageView.
     * @since 1.0
     * @version 1.0
     */
    void SetImageHeight(int16_t height)
    {
        imageHeight_ = height;
    }

    /**
     * @brief Sets the callback function to be invoked upon a click event.
     *
     * @param clickListener Indicates the pointer to the callback function.
     * @since 1.0
     * @version 1.0
     */
    void SetOnClickListener(UIView::OnClickListener* clickListener)
    {
        clickListener_ = clickListener;
    }

private:
    void ClearData();
    uint16_t imageWidth_;
    uint16_t imageHeight_;
    uint16_t width_;
    uint16_t height_;
    List<const char*> data_;
    Style style_;
    UIView::OnClickListener* clickListener_;
    void SetImageViewInfo(UIImageView& imgView, int16_t index);
};
}
#endif
/**
 * @}
 */
