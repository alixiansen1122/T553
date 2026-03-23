#ifndef BAIDU_MAP_COMPONENT_INTERFACE_H
#define BAIDU_MAP_COMPONENT_INTERFACE_H

#include "baidumap_msg_center.h"
#include "components/ui_view.h"

class MapComponentApi
{
public:

    static MapComponentApi &getInstance();
    MapComponentApi(const MapComponentApi &) = delete;
    MapComponentApi &operator=(const MapComponentApi &) = delete;

    const OHOS::UIView* get_baidumap_root_view();
    virtual void destroy();
    virtual void set_zoom_event_enabled(bool enabled);
    virtual void zoom_in();
    virtual void zoom_out();
    virtual void zoom_by(int amount);
    virtual void zoom_to(int level);
    virtual void set_map_background_color(char * json_color);
    virtual void set_map_overlayer_enabled(bool enabled);
    virtual void set_map_tiles_enabled(bool enabled);
    virtual void set_scroll_gestures_enabled(bool enabled);
    virtual void scroll_by(int pixel_x, int pixel_y);
    // 导航过程、定位成功
    virtual void new_lat_lng(char * json_lat_lng);
    virtual void new_lat_lng_zoom(char * json_lat_lng, int zoom);
    // 导航结束
    virtual void clear_map_overlayer();
    // 画点
    virtual void draw_circle(char * json_circle);
    // 无
    virtual void draw_polygon(char * json_polygon);
    // 导航路线
    virtual void draw_path(char * json_path);
    // 无
    virtual void draw_text(char * json_text);
    // 定位成功定位点
    virtual void draw_marker(char * json_marker);
    // 导航过程箭头
    virtual void draw_navi_marker(char * json_navi_marker);
    // 定位点
    virtual void overlayer_clear_circle();
    // 定位点
    virtual void overlayer_clear_marker();
    // 导航箭头
    virtual void overlayer_clear_navi_marker();
    // 以下调用使用，不用提供js通路
    void RecvMapSvgInfo(Content *info);
private:
    // MapRootView* container_ = nullptr;
    /**
     * @brief 构造函数
     */
    MapComponentApi();
};

#endif