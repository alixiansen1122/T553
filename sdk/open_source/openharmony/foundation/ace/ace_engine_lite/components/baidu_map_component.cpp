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
#include "acelite_config.h"
#ifdef FEATURE_COMPONENT_BAIDUMAP
#include "baidu_map_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"
#include "soc_vau_api.h"

namespace OHOS {
namespace ACELite {
// API-method
const char * const BaiduMapComponent::FUNC_SET_ZOOM_EVENT_ENABLED = "set_zoom_event_enabled";
const char * const BaiduMapComponent::FUNC_ZOOM_IN = "zoom_in";
const char * const BaiduMapComponent::FUNC_ZOOM_OUT = "zoom_out";
const char * const BaiduMapComponent::FUNC_ZOOM_TO = "zoom_to";
const char * const BaiduMapComponent::FUNC_ZOOM_BY = "zoom_by";
const char * const BaiduMapComponent::FUNC_SET_MAP_BACKGROUND_COLOR = "set_map_background_color";
const char * const BaiduMapComponent::FUNC_SET_MAP_OVERLAYER_ENABLED = "set_map_overlayer_enabled";
const char * const BaiduMapComponent::FUNC_SET_MAP_TITLES_ENABLED = "set_map_tiles_enabled";
const char * const BaiduMapComponent::FUNC_SET_SCROLL_GESTURES_ENABLED = "set_scroll_gestures_enabled";
const char * const BaiduMapComponent::FUNC_SCROLL_BY = "scroll_by";
const char * const BaiduMapComponent::FUNC_NEW_LAT_LNG = "new_lat_lng";
const char * const BaiduMapComponent::FUNC_NEW_LAT_LNG_ZOOM = "new_lat_lng_zoom";
const char * const BaiduMapComponent::FUNC_CLEAR_MAP_OVERLAYER = "clear_map_overlayer";
const char * const BaiduMapComponent::FUNC_DRAW_MARKER = "draw_marker";
const char * const BaiduMapComponent::FUNC_DRAW_POLYGON = "draw_polygon";
const char * const BaiduMapComponent::FUNC_DRAW_PATH = "draw_path";
const char * const BaiduMapComponent::FUNC_DRAW_TEXT = "draw_text";
const char * const BaiduMapComponent::FUNC_START_LISTEN_MY_GEO_LOCATION = "start_listen_my_geo_location";
const char * const BaiduMapComponent::FUNC_STOP_LISTEN_MY_GEO_LOCATION = "stop_listen_my_geo_location";
const char * const BaiduMapComponent::FUNC_GET_MY_GEO_STATUS = "get_my_geo_status";
const char * const BaiduMapComponent::FUNC_GET_MY_GEO_LOCATION = "get_my_geo_location";
const char * const BaiduMapComponent::FUNC_DRAW_CIRCLE = "draw_circle";
const char * const BaiduMapComponent::FUNC_DRAW_NAVI_MARKER = "draw_navi_marker";
const char * const BaiduMapComponent::FUNC_OVERLAYER_CLEAR_CIRCLE = "overlayer_clear_circle";
const char * const BaiduMapComponent::FUNC_OVERLAYER_CLEAR_MARKER = "overlayer_clear_marker";
const char * const BaiduMapComponent::FUNC_OVERLAYER_CLEAR_NAVI_MARKER = "overlayer_clear_navi_marker";
const char * const BaiduMapComponent::FUNC_DESTROY = "destroy";

const FunctionMap BaiduMapComponent::funcMap[] = {
    {FUNC_SET_ZOOM_EVENT_ENABLED, SetZoomEventEnabled},
    {FUNC_ZOOM_IN, ZoomIn},
    {FUNC_ZOOM_OUT, ZoomOut},
    {FUNC_ZOOM_TO, ZoomTo},
    {FUNC_ZOOM_BY, ZoomBy},
    {FUNC_SET_MAP_BACKGROUND_COLOR, SetMapBackgroundColor},
    {FUNC_SET_MAP_OVERLAYER_ENABLED, SetMapOverlayerEnabled},
    {FUNC_SET_MAP_TITLES_ENABLED, SetMapTilesEnabled},
    {FUNC_SET_SCROLL_GESTURES_ENABLED, SetScrollGesturesEnabled},
    {FUNC_SCROLL_BY, ScrollBy},
    {FUNC_NEW_LAT_LNG, NewLatLng},
    {FUNC_NEW_LAT_LNG_ZOOM, NewLatLngZoom},
    {FUNC_CLEAR_MAP_OVERLAYER, ClearMapOverlayer},
    {FUNC_DRAW_MARKER, DrawMarker},
    {FUNC_DRAW_POLYGON, DrawPolygon},
    {FUNC_DRAW_PATH, DrawPath},
    {FUNC_DRAW_TEXT, DrawText},
    {FUNC_START_LISTEN_MY_GEO_LOCATION, StartListenMyGeoLocation},
    {FUNC_STOP_LISTEN_MY_GEO_LOCATION, StopListenMyGeoLocation},
    {FUNC_GET_MY_GEO_STATUS, GetMyGeoStatus},
    {FUNC_GET_MY_GEO_LOCATION, GetMyGeoLocation},
    {FUNC_DRAW_CIRCLE, DrawCircle},
    {FUNC_DRAW_NAVI_MARKER, DrawNaviMarker},
    {FUNC_OVERLAYER_CLEAR_CIRCLE, OverlayerClearCircle},
    {FUNC_OVERLAYER_CLEAR_MARKER, OverlayerClearMarker},
    {FUNC_OVERLAYER_CLEAR_NAVI_MARKER, OverlayerClearNaviMarker},
    {FUNC_DESTROY, Destroy}
};
BaiduMapComponent::BaiduMapComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager)
    : Component(options, children, styleManager)
{
    HILOG_INFO(HILOG_MODULE_ACE,"BaiduMapComponent");
    SetComponentName(K_BAIDUMAP);
    uint16_t funcMapLength = sizeof(funcMap) / sizeof(funcMap[0]);
    for (uint16_t index = 0; index < funcMapLength; index++) {
        RegisterNamedFunction(funcMap[index].funcName, funcMap[index].handler);
    }
    uapi_vau_set_cubic_threshold(1.0f);
}

BaiduMapComponent::~BaiduMapComponent()
{
    HILOG_INFO(HILOG_MODULE_ACE,"BaiduMapComponent destructor");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().destroy();
    uapi_vau_set_cubic_threshold(0.125f);
#endif
}

UIView *BaiduMapComponent::GetComponentRootView() const
{
#ifdef SUPPORT_BAIDU
    return const_cast<UIView *>(MapComponentApi::getInstance().get_baidumap_root_view());
#else
    return nullptr;
#endif
}

JSValue BaiduMapComponent::SetZoomEventEnabled(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"SetZoomEventEnabled");
#ifdef SUPPORT_BAIDU
    bool enabled = BoolOf(args[0]);
    MapComponentApi::getInstance().set_zoom_event_enabled(enabled);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ZoomIn(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ZoomIn");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().zoom_in();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ZoomOut(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ZoomOut");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().zoom_out();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ZoomTo(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ZoomTo");
#ifdef SUPPORT_BAIDU
    int amount = IntegerOf(args[0]);
    MapComponentApi::getInstance().zoom_to(amount);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ZoomBy(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ZoomBy");
#ifdef SUPPORT_BAIDU
    int amount = IntegerOf(args[0]);
    MapComponentApi::getInstance().zoom_by(amount);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::SetMapBackgroundColor(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"SetMapBackgroundColor");
#ifdef SUPPORT_BAIDU
    char *json_color = MallocStringOf(args[0]);
    MapComponentApi::getInstance().set_map_background_color(json_color);
    ACE_FREE(json_color);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::SetMapOverlayerEnabled(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"SetMapOverlayerEnabled");
#ifdef SUPPORT_BAIDU
    bool enabled = BoolOf(args[0]);
    MapComponentApi::getInstance().set_map_overlayer_enabled(enabled);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::SetMapTilesEnabled(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"SetMapTilesEnabled");
#ifdef SUPPORT_BAIDU
    bool enabled = BoolOf(args[0]);
    MapComponentApi::getInstance().set_map_tiles_enabled(enabled);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::SetScrollGesturesEnabled(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"SetScrollGesturesEnabled");
#ifdef SUPPORT_BAIDU
    bool enabled = BoolOf(args[0]);
    MapComponentApi::getInstance().set_scroll_gestures_enabled(enabled);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ScrollBy(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ScrollBy");
#ifdef SUPPORT_BAIDU
    int pixel_x = IntegerOf(args[0]);
    int pixel_y = IntegerOf(args[1]);
    MapComponentApi::getInstance().scroll_by(pixel_x,pixel_y);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::NewLatLng(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"NewLatLng");
#ifdef SUPPORT_BAIDU
    char *json_lat_lng = MallocStringOf(args[0]);
    MapComponentApi::getInstance().new_lat_lng(json_lat_lng);
    ACE_FREE(json_lat_lng);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::NewLatLngZoom(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"NewLatLngZoom");
#ifdef SUPPORT_BAIDU
    char *json_lat_lng = MallocStringOf(args[0]);
    int zoom = IntegerOf(args[1]);
    MapComponentApi::getInstance().new_lat_lng_zoom(json_lat_lng,zoom);
    ACE_FREE(json_lat_lng);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::ClearMapOverlayer(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"ClearMapOverlayer");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().clear_map_overlayer();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawMarker(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawMarker");
#ifdef SUPPORT_BAIDU
    char *json_marker = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_marker(json_marker);
    ACE_FREE(json_marker);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawNaviMarker(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawNaviMarker");
#ifdef SUPPORT_BAIDU
    char *json_navi_marker = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_navi_marker(json_navi_marker);
    ACE_FREE(json_navi_marker);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawPolygon(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawPolygon");
#ifdef SUPPORT_BAIDU
    char *json_polygon = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_polygon(json_polygon);
    ACE_FREE(json_polygon);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawPath(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawPath");
#ifdef SUPPORT_BAIDU
    char *json_path = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_path(json_path);
    ACE_FREE(json_path);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawCircle(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawCircle");
#ifdef SUPPORT_BAIDU
    char *json_circle = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_circle(json_circle);
    ACE_FREE(json_circle);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::DrawText(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"DrawText");
#ifdef SUPPORT_BAIDU
    char *json_text = MallocStringOf(args[0]);
    MapComponentApi::getInstance().draw_text(json_text);
    ACE_FREE(json_text);
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::StartListenMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"StartListenMyGeoLocation");
    return UNDEFINED;
}

JSValue BaiduMapComponent::StopListenMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"StopListenMyGeoLocation");
    return UNDEFINED;
}

JSValue BaiduMapComponent::GetMyGeoStatus(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"GetMyGeoStatus");
    return UNDEFINED;
}

JSValue BaiduMapComponent::GetMyGeoLocation(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"GetMyGeoLocation");
    return UNDEFINED;
}

JSValue BaiduMapComponent::OverlayerClearCircle(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"OverlayerClearCircle");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().overlayer_clear_circle();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::OverlayerClearMarker(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"OverlayerClearMarker");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().overlayer_clear_marker();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::OverlayerClearNaviMarker(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"OverlayerClearNaviMarker");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().overlayer_clear_navi_marker();
#endif
    return UNDEFINED;
}

JSValue BaiduMapComponent::Destroy(const jerry_value_t func, const jerry_value_t context,
    const jerry_value_t args[], const jerry_length_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE,"Destroy");
#ifdef SUPPORT_BAIDU
    MapComponentApi::getInstance().destroy();
#endif
    return UNDEFINED;
}

} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_BAIDUMAP