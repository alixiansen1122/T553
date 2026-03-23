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
 * @file ui_list_ext.h
 *
 * @brief Extend UIListExt ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_UI_LIST_EXT_H
#define GRAPHIC_UI_LIST_EXT_H
#include "graphic_config.h"
#include "components/ui_list.h"
#include "gfx_utils/vector.h"

namespace OHOS {
/**
 * @brief Class to extend UIListExt ability.
 *
 * @since 1.0
 * @version 1.0
 */
class UIListExt : public UIList {
public:
    /**
     * @brief A constructor used to create a <b>UIListExt</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIListExt() {}
    /**
     * @brief A constructor used to create a <b>UIListExt</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    explicit UIListExt(uint8_t direction)
    {
        SetDirection(direction);
    }

    /**
     * @brief A destructor used to delete the <b>UIListExt</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~UIListExt() override {}

    /**
     * @brief Set scale factor witch is used to calculate scale value.
     *        scale = N / scale factor  ( 1 <= N <= scale factor)
     *        A larger scaleFactor value indicates higher scale precision.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetScaleFactor(int16_t value)
    {
        if (value <= 0) {
#ifndef ENABLE_GRAPHIC_LOG
            GRAPHIC_LOGE("Illegal value! Value must bigger than 0");
#endif
            return;
        }
        scaleFactor_ = value;
    }

    /**
     * @brief Sets the spacing between items.
     *
     * @param value Indicates spacing between items.
     */
    void SetItemSpace(uint16_t value)
    {
        space_ = value;
    }

    void ReMeasure() override;

private:
    void ModifyItemSpace(int16_t centerIndex);
    void ModifyMidAdjSpace(int16_t centerIndex, int16_t& prevIndex, int16_t& nextIndex);
    int16_t scaleFactor_ = 400; // 400: default scale factor.
    uint16_t space_ = 0;
    Graphic::Vector<UIView*> items_;
};
} // OHOS
#endif // GRAPHIC_UI_LIST_EXT_H
/**
 * @}
 */
