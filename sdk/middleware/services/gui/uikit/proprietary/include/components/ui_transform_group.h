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

#ifndef GRAPHIC_UI_TRANSFORM_GROUP_H
#define GRAPHIC_UI_TRANSFORM_GROUP_H
#include <unordered_map>
#include "graphic_config.h"
#include "components/ui_view_group.h"
#include "gfx_utils/transform.h"

namespace OHOS {
class UITransformGroup : public UIViewGroup {
public:
    UITransformGroup();
    UITransformGroup(UITransformGroup* view);
    virtual ~UITransformGroup();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_TRANSFORM_GROUP</b>, as defined in {@link UIViewType}.
     */
    UIViewType GetViewType() const override
    {
        return UI_TRANSFORM_GROUP;
    }

    /**
     * @brief Adds a child view.
     *
     * @param view Indicates the pointer to the child view to add.
     * Support add <UILabel> <UILabelExt> <UIImageView> <UICanvasExt>.
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts a new child view behind the current one.
     *
     * @param prevView Indicates the pointer to the current child view, previous to the new child view to insert.
     * @param view Indicates the pointer to the new child view to insert.
     * @since 1.0
     * @version 1.0
     * Support insert <UILabel> <UILabelExt> <UIImageView> <UICanvasExt>.
     */
    void Insert(UIView* prevView, UIView* insertView) override;

    /**
     * @brief Remove a child view.
     * @param view Indicates the pointer to the child view to remove.
     */
    void Remove(UIView* view) override;

    /**
     * @brief Remove all child views.
     */
    void RemoveAll() override;

    /**
     * @brief Transform a child view.
     * @param view Indicates the pointer to the child view to transform.
     * @param transmap Indicates transformation applying to the view. SetTransmapRect will be set during calculation.
     */
    void TransformChild(UIView* view, TransformMap transmap);

    /**
     * @brief Sets all child view style and self style.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @since 1.0
     * @version 1.0
     */
    void SetAllStyle(uint8_t key, int64_t value);

    /**
     * @brief Called when a view is drawn.
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
private:
    void ReMeasure() override;
    void DeleteMirrorView();
    std::unordered_map<UIView*, TransformMap> transformHash_;
};
} // namespace OHOS
#endif // GRAPHIC_UI_TRANSFORM_GROUP_H