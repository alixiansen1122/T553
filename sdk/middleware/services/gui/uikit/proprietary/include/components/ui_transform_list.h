/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef GRAPHIC_UI_TRANSFORM_LIST_H
#define GRAPHIC_UI_TRANSFORM_LIST_H
#include "graphic_config.h"
#include "components/ui_list_nested.h"
#include "components/ui_transform_group.h"
#include "gfx_utils/vector.h"

namespace OHOS {
class UITransformList : public UIListNested {
public:
    /**
     * @brief A constructor used to create a <b>UITransformList</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UITransformList() : transformFunc_(nullptr), space_(5) {} // 5: default item space

    /**
     * @brief A constructor used to create a <b>UITransformList</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    explicit UITransformList(uint8_t direction)
    {
        SetDirection(direction);
    }

    /**
     * @brief A destructor used to delete the <b>UITransformList</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~UITransformList() override {}

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_TRANSFORM_LIST;
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

    /**
     * @brief Customized transformation function type.
     *
     * @param list Indicates current transformlist obj.
     * @param item Indicates current item obj.
     * @return Returned value is not longer used. It was used to decide whether the item is in the middle.
     */
    using TransformFunc = bool (*)(UITransformList* list, UIView* item);

    /**
     * @brief Setting a custom transformation function.
     *
     * @param func Indicates Transformation function point.
     */
    void SetItemTransformFunc(TransformFunc func);

protected:
    void ModifyItemSpace(int16_t centerIndex);
    void ReMeasure() override;

private:
    void ModifyMidAdjSpace(int16_t centerIndex, int16_t& prevIndex, int16_t& nextIndex);
    TransformFunc transformFunc_;
    uint16_t space_;
    Graphic::Vector<UIView*> items_;
};
} // namespace OHOS
#endif // GRAPHIC_UI_TRANSFORM_LIST_H