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
 * @file ui_map_view.h
 *
 * @brief Defines the class to extend canvas/paint ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_MAP_VIEW_H
#define GRAPHIC_LITE_UI_MAP_VIEW_H
#include "components_ext/map_svg_parser.h"
#include "components/ui_view_group.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
struct POIInfo {
    ImageDetail* img = nullptr;
    TextDetail* text[2] = {nullptr, nullptr};
    uint8_t tag = 0;
    uint16_t rank = 0;
    uint16_t id = 0;
    Rect rect;
    Point orignPos = {0, 0};
    bool visible = false;
};

struct RoadInfo {
    TextDetail* text = nullptr;
    Rect rect;
    Point orignPos = {0, 0};
    bool visible = false;
};

class OnPOIClickListener : public HeapBase {
public:
    /* *
     * @brief Called when a POI is clicked.
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnClick(uint32_t index)
    {
        return false;
    }

    /* *
     * @brief A destructor used to delete the <b>OnPOIClickListener</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~OnPOIClickListener() {}
};

class UIMapView : public UIViewGroup, public UIView::OnClickListener {
public:
    UIMapView(const UIMapView &) = delete;
    UIMapView &operator=(const UIMapView &) = delete;

    /* *
     * @brief A constructor used to create an <b>UIMapView</b> instance.
     *
     */
    UIMapView();

    /* *
     * @brief A destructor used to delete the <b>UIMapView</b> instance.
     *
     */
    ~UIMapView() override;

    /* *
     * @brief Get current view type.
     *
     */
    UIViewType GetViewType() const override
    {
        return UI_MAP_VIEW;
    }

    /* *
     * @brief Set SVG src with file path.
     *
     */
    bool SetSVG(const char* path);

    /* *
     * @brief Set SVG src with file buffer.
     *
     */
    bool SetSVG(uint8_t* data, uint32_t length);

    /* *
     * @brief Set POI on cick listener.
     *
     */
    void SetOnPOIClickListener(OnPOIClickListener* listener);

    /* *
     * @brief Enable cache. It will cache this tile immediately when enable is true,
     *        and free cache buffer immediately. We need to enable cache when setting svg,
     *        or after scale map finished, and disable cache before scale map.
     *
     */
    void EnableCache(bool enable);

    /* *
     * @brief Used to scale map base the target point.
     *
     */
    void ScaleMap(float scale, const Vector2<float>& pivot);

    /* *
     * @brief Hide the POI by tag.
     *
     */
    void HidePOI(uint16_t tag, bool isHide = true);
    void SetX(int16_t x) override;
    void SetY(int16_t y) override;

private:
    POIInfo* pois_ = nullptr;
    RoadInfo* roads_ = nullptr;
    OnPOIClickListener* poiListener_ = nullptr;
    bool cacheEnable_ = false;
    bool needUpdate_ = false;
    ImageInfo cacheInfo_ = {{0}};
    MapSVGParser parser_;
    PathDetail* pathArray_ = nullptr;
    uint8_t pathCount_ = 0;
    RoadDetail* roadArray_ = nullptr;
    uint8_t roadCount_ = 0;
    POIDetail* poiArray_ = nullptr;
    uint8_t poiCount_ = 0;
    char* fontName_ = DEFAULT_VECTOR_FONT_FILENAME;
    uint8_t fontId_ = 0xff;
    uint8_t fontSize_ = 0;
    uint32_t poiMask_ = 0;
    float scale_ = 1.0f;
    bool isSetSVG_ = false;

    void InitMapInfo();
    void DrawPath(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawText(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, TextDetail& info, Point offset = {0, 0},
        bool isPOIText = false);
    Rect DrawImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, ImageInfo& info, MapRect& rect);
    void DrawPOIs(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void DrawRoads(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    bool RenderMapToCache(void);
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void HandlePOICollision();
    void HandleRoadCollision();
    bool InitPOIInfo(int num);
    bool InitRoadInfo(int num);
    void ClearSVG();
};
#endif
}
#endif
/**
 * @}
 */
