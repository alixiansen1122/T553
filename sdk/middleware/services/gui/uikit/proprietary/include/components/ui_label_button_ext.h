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
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_label_button_ext.h
 *
 * @brief Extend UILabelButton ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_LABEL_BUTTON_EXT_H
#define GRAPHIC_LITE_UI_LABEL_BUTTON_EXT_H
#include "graphic_config.h"
#include "components/ui_label_button.h"

#if ENABLE_FONT_VECTOR_GLOBAL || ENABLE_SHAPING
namespace OHOS {
/**
 * @brief Class to extend UILabelButton ability.
 *
 * @since 1.0
 * @version 1.0
 */
class UILabelButtonExt : public UILabelButton {
public:
    /**
     * @brief A constructor used to create a <b>UILabelButtonExt</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UILabelButtonExt() {}

    /**
     * @brief A destructor used to delete the <b>UILabelButtonExt</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~UILabelButtonExt() override {}

     /**
     * @brief Obtains the view type.
     * @return Returns the view type.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_LABEL_BUTTON_EXT;
    }

    /**
     * @brief Set the text id
     *
     */
    void SetTextId(uint16_t textId);

    /**
     * @brief Reset text.
     *
     */
    void ForceResetText() override;

protected:
    void InitLabelButtonText() override;
};
}
#endif // ENABLE_FONT_VECTOR_GLOBAL || ENABLE_SHAPING
#endif // GRAPHIC_LITE_UI_LABEL_BUTTON_E_H
/**
 * @}
 */
