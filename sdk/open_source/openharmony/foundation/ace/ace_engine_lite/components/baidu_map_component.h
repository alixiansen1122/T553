/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_BAIDU_MAP_COMPONENT_H
#define OHOS_ACELITE_BAIDU_MAP_COMPONENT_H

#include "acelite_config.h"
#ifdef FEATURE_COMPONENT_BAIDUMAP
#include <stdint.h>
#include "jsi.h"
#include "component.h"
#include "baidumap_js_component.h"


namespace OHOS {
namespace ACELite {
struct FunctionMap {
    ACE_DISALLOW_COPY_AND_MOVE(FunctionMap);
    const char *funcName;
    jerry_external_handler_t handler;
};

class BaiduMapComponent final: public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BaiduMapComponent);
    BaiduMapComponent() = delete;
    BaiduMapComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~BaiduMapComponent() override;

    /**
     * SetZoomEventEnabled, enabled Indicates whether to allow scaling events. This function is disabled by default
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue SetZoomEventEnabled(const jerry_value_t func, const jerry_value_t context,
                                        const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ZoomIn ,Map zoomed out one level. Valid only at levels 4 to 18
     */
    static JSValue ZoomIn(const jerry_value_t func, const jerry_value_t context,
                           const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ZoomOut ,Map zoomed in one level. Valid only at levels 4 to 18
     */
    static JSValue ZoomOut(const jerry_value_t func, const jerry_value_t context,
                            const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ZoomTo, Zoom the map to the specified level
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue ZoomTo(const jerry_value_t func, const jerry_value_t context,
                           const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ZoomBy, amount Incremental scaling of a map at a given level
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue ZoomBy(const jerry_value_t func, const jerry_value_t context,
                           const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * SetMapBackgroundColor, Set the placeholder background color before the map is displayed
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue SetMapBackgroundColor(const jerry_value_t func, const jerry_value_t context,
                                          const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * SetMapOverlayerEnabled, Indicates whether to enable the overlay layer. This function is disabled by default
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue SetMapOverlayerEnabled(const jerry_value_t func, const jerry_value_t context,
                                           const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * SetMapTilesEnabled, Indicates whether to enable the map tile layer. This function is enabled by default
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue SetMapTilesEnabled(const jerry_value_t func, const jerry_value_t context,
                                       const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * SetScrollGesturesEnabled, Indicates whether to enable gesture sliding. This function is enabled by default
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue SetScrollGesturesEnabled(const jerry_value_t func, const jerry_value_t context,
                                             const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ScrollBy, Incremental moving map for a given screen pixel. The screen origin is in the upper left corner
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue ScrollBy(const jerry_value_t func, const jerry_value_t context,
                             const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * NewLatLng, Displays the map to the center of the screen based on the longitude and latitude
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue NewLatLng(const jerry_value_t func, const jerry_value_t context,
                              const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * NewLatLngZoom, Displays the map of the specified longitude and latitude at the specified level to the center of the screen
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue NewLatLngZoom(const jerry_value_t func, const jerry_value_t context,
                                  const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * ClearMapOverlayer, Clear all overlays on the map
     */
    static JSValue ClearMapOverlayer(const jerry_value_t func, const jerry_value_t context,
                                      const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * OverlayerClearCircle
     */
    static JSValue OverlayerClearCircle(const jerry_value_t func, const jerry_value_t context,
                                        const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * OverlayerClearMarker
     */
    static JSValue OverlayerClearMarker(const jerry_value_t func, const jerry_value_t context,
                                        const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * OverlayerClearNaviMarker
     */
    static JSValue OverlayerClearNaviMarker(const jerry_value_t func, const jerry_value_t context,
                                            const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawMarker, Construction point
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawMarker(const jerry_value_t func, const jerry_value_t context,
                              const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawNaviMarker
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawNaviMarker(const jerry_value_t func, const jerry_value_t context,
                                  const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawPolygon, Constructing Polygons
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawPolygon(const jerry_value_t func, const jerry_value_t context,
                                const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawPath, structural route
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawPath(const jerry_value_t func, const jerry_value_t context,
                            const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawCircle
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawCircle(const jerry_value_t func, const jerry_value_t context,
                              const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * DrawText, Constructed text
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue DrawText(const jerry_value_t func, const jerry_value_t context,
                             const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * StartListenMyGeoLocation, Initiate location interception
     */
    static JSValue StartListenMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
                                             const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * StopListenMyGeoLocation, Stop location interception
     */
    static JSValue StopListenMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
                                            const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * GetMyGeoStatus, Obtaining the Location Status
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue GetMyGeoStatus(const jerry_value_t func, const jerry_value_t context,
                                   const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * GetMyGeoLocation, Obtains the watch location information
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue GetMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
                                     const jerry_value_t args[], const jerry_length_t argsSize);

    /**
     * Destroy
     *
     * @param thisVal Indicates function execute context
     * @param args Indicates the function arguments
     * @param argsSize Indicates the size of function arguments
     */
    static JSValue Destroy(const jerry_value_t func, const jerry_value_t context,
                           const jerry_value_t args[], const jerry_length_t argsSize);

    static const char * const FUNC_SET_ZOOM_EVENT_ENABLED;
    static const char * const FUNC_ZOOM_IN;
    static const char * const FUNC_ZOOM_OUT;
    static const char * const FUNC_ZOOM_TO;
    static const char * const FUNC_ZOOM_BY;
    static const char * const FUNC_SET_MAP_BACKGROUND_COLOR;
    static const char * const FUNC_SET_MAP_OVERLAYER_ENABLED;
    static const char * const FUNC_SET_MAP_TITLES_ENABLED;
    static const char * const FUNC_SET_SCROLL_GESTURES_ENABLED;
    static const char * const FUNC_SCROLL_BY;
    static const char * const FUNC_NEW_LAT_LNG;
    static const char * const FUNC_NEW_LAT_LNG_ZOOM;
    static const char * const FUNC_CLEAR_MAP_OVERLAYER;
    static const char * const FUNC_DRAW_MARKER;
    static const char * const FUNC_DRAW_POLYGON;
    static const char * const FUNC_DRAW_PATH;
    static const char * const FUNC_DRAW_TEXT;
    static const char * const FUNC_START_LISTEN_MY_GEO_LOCATION;
    static const char * const FUNC_STOP_LISTEN_MY_GEO_LOCATION;
    static const char * const FUNC_GET_MY_GEO_STATUS;
    static const char * const FUNC_GET_MY_GEO_LOCATION;
    static const char * const FUNC_DRAW_CIRCLE;
    static const char * const FUNC_DRAW_NAVI_MARKER;
    static const char * const FUNC_OVERLAYER_CLEAR_CIRCLE;
    static const char * const FUNC_OVERLAYER_CLEAR_MARKER;
    static const char * const FUNC_OVERLAYER_CLEAR_NAVI_MARKER;
    static const char * const FUNC_DESTROY;
    static const FunctionMap funcMap[];

protected:
    UIView *GetComponentRootView() const override;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_BAIDUMAP
#endif // OHOS_ACELITE_BAIDU_MAP_COMPONENT_H