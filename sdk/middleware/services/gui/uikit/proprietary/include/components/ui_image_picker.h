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
 * @file ui_image_picker.h
 *
 * @brief Defines the attributes and functions of the <b>UIImagePicker</b> class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_IMAGE_PICKER_H
#define UI_IMAGE_PICKER_H

#include "image_adapter.h"
#include "ui_list.h"

namespace OHOS {
/**
 * @brief Defines a image picker. Multiple images can be put into a sliding list for selection.
 *        The selected images are enlarged.
 *
 * @since 1.0
 * @version 1.0
 */
class UIImagePicker : public UIViewGroup {
public:
    /**
     * @brief A constructor used to create a <b>UIImagePicker</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIImagePicker();

    /**
     * @brief A destructor used to delete the <b>UIImagePicker</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIImagePicker();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns the view type. For details, see {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_IMAGE_PICKER;
    }

    virtual bool OnPreDraw(Rect& invalidatedArea) const override
    {
        return false;
    }

    /**
     * @brief Sets image data in the image picker by using a string array.
     *
     * @param value[] Indicates the array of image data path.
     * @param count   Indicates the array size.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetValues(const char* value[], uint16_t count);

    /**
     * @brief Sets selected image data in the image picker by using a string array.
     *
     * @param value[] Indicates the array of selected image data path.
     * @param count   Indicates the array size.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool SetSelectedValues(const char* selectedValue[], uint16_t selectedCount);

    /**
     * @brief Clears all values in the image picker.
     *
     * @since 1.0
     * @version 1.0
     */
    void ClearValues();

    /**
     * @brief Sets the index of the item currently selected in the image picker.
     *
     * @param index Indicates the index to set.
     * @since 1.0
     * @version 1.0
     */
    void SetSelected(uint16_t index);

    /**
     * @brief Obtains the index of the item currently selected in the image picker.
     *
     * @return Returns the index.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetSelected();

    /**
     * @brief Sets the height of each item in the image picker.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetItemHeight(int16_t height);

    /**
     * @brief Sets the width of each item in the image picker.
     *
     * @param height Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetItemWidth(int16_t width);

    /**
     * @brief Sets the width for this component.
     *
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetWidth(int16_t width) override;

    /**
     * @brief Sets the height for this component.
     *
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetHeight(int16_t height) override;

    /**
     * @brief Sets whether a image picker can slide cyclically.
     *
     * @param state Specifies whether the image picker can slide cyclically. Value <b>true</b> indicates that the
     *              image picker can slide cyclically, and value <b>false</b> indicates that the image picker cannot
     *              slide cyclically. The default value is <b>false</b>.
     * @since 1.0
     * @version 1.0
     */
    void SetLoopState(bool state);

    /**
     * @brief Obtains whether the image picker can slide cyclically.
     *
     * @return Returns <b>true</b> if the image picker can slide cyclically; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetLoopState();

    /**
     * @brief Defines the listener used by a image picker. This listener is triggered when an item
     *        is selected after sliding stops.
     *
     * @since 1.0
     * @version 1.0
     */
    class SelectedListener : public HeapBase {
    public:
        /**
         * @brief A constructor used to create a <b>SelectedListener</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        SelectedListener() {}

        /**
         * @brief A destructor used to delete the <b>SelectedListener</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual ~SelectedListener() {}

        /**
         * @brief Called when an item is selected after sliding stops. This function is implemented by applications.
         *
         * @param picker Indicates the image picker instance.
         * @since 1.0
         * @version 1.0
         */
        virtual void OnPickerStoped(UIImagePicker& picker) {}
    };

    /**
     * @brief Register selected listener.
     *
     * @since 1.0
     * @version 1.0
     */
    void RegisterSelectedListener(SelectedListener* pickerListener)
    {
        pickerListener_ = pickerListener;
    }

    /**
     * @brief Refresh the current view
     */
    void Refresh();

private:
    friend class ImagePickerListScrollListener;
    void RefreshList();
    bool RefreshSelected(uint16_t index);
    const char* GetPathFromIndex(List<const char*>& list, uint16_t index);
    void ClearList();

    uint16_t itemsWidth_;
    uint16_t itemsHeight_;
    uint16_t imageWidth_;
    uint16_t imageHeight_;
    uint16_t setSelectedIndex_;
    bool RefreshValues(const char* value[], uint16_t count);
    List<const char*> dataList_;
    List<const char*> selectedDataList_;
    ImageAdapter imageAdapter_;
    UIList list_;
    void* listListener_;
    SelectedListener* pickerListener_;
};
}

#endif
/**
 * @}
 */
