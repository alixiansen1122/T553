/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef XIAODU_NAVI_VECTOR_H
#define XIAODU_NAVI_VECTOR_H

#include "errcode.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_vector_navi_type_id {
    MSGCENTER_TYPE_ID_VECTOR_NAVI_PULL_UP = 0x1,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_SVG,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_SVG,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DST_BY_TEST,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DST_BY_PCM,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_DST_LIST,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_START_FROM_WATCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_NAVI_INFO,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_START_FROM_PHONE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_POI,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_FINISH_FROM_WATCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_FINISH_FROM_PHONE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_AUTH_LICENSE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEO_LOCATION,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_INIT,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_START,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_SUCC,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_FAIL,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_START,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_DESTROY,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_TEXT_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_ICON_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_TIME_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_DISTANCE_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_ROUTE_UPDATE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GPS_STATUS_CHANGE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_FARAWAY,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_YAWING,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_RE_ROUTE_COMPLETE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ARRIVE_DSTINATION,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_POI_SUG_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEOCODE_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REGEOCODE_SEARCH,
    /* 地图私有协议 */
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_PAGE,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_DISCONNECTED,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_FIRST_BATCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_FROM_WATCH,
    MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_DESTROY_FROM_WATCH
} msg_center_vector_navi_type_id_t;

errcode_t msg_center_vector_navi_pull_up(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_recv_svg(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_svg(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_dst_by_text(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_dst_by_pcm(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_recv_dst_list(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_start_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_recv_navi_info(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_start_from_phone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_poi(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_recv_poi(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_finish_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_finish_from_phone(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_auth_license(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_auth_license(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_geo_location(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_geo_location(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_navi_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_navi_init(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_plan_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_plan_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_plan_succ(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_plan_succ(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_plan_fail(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_plan_fail(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_navi_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_navi_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_navi_destroy(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_navi_destroy(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_road_guide_text_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_road_guide_text_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_req_road_guide_icon_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_road_guide_icon_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_req_remain_time_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_remain_time_update(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_remain_distance_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_remain_distance_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_req_remain_route_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_remain_route_update(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                            uint16_t usr_len);
errcode_t msg_center_vector_navi_req_gps_status_change(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_gps_status_change(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_faraway(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_faraway(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_plan_yawing(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_plan_yawing(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_re_route_complete(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_re_route_complete(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_arrive_destination(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_arrive_destination(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vertor_navi_req_poi_sug_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vertor_navi_rec_poi_sug_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_riding_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                         uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_riding_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                         uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_walking_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_walking_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len);
errcode_t msg_center_vector_navi_req_route_transit_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_route_transit_search(uint8_t cmd_id, uint8_t type, void *usr_data,
                                                          uint16_t usr_len);
errcode_t msg_center_vector_navi_req_geocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_geocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_regeocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_regeocode_search(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_destroy_page(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_first_batch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_req_destroy_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_vector_navi_rec_destroy_from_watch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

bool is_vector_navi_long_distance(uint8_t type);

static const char *g_vector_navi_str_tb[] = {
    "req_auth_license",
    "rec_auth_license",
    "req_geo_location",
    "rec_geo_location",
    "req_navi_init",
    "rec_navi_init",
    "req_route_plan_start",
    "rec_route_plan_start",
    "req_route_plan_succ",
    "rec_route_plan_succ",
    "req_route_plan_fail",
    "rec_route_plan_fail",
    "req_navi_start",
    "rec_navi_start",
    "req_navi_destroy",
    "rec_navi_destroy",
    "req_road_guide_text_update",
    "rec_road_guide_text_update",
    "req_road_guide_icon_update",
    "rec_road_guide_icon_update",
    "req_remain_time_update",
    "rec_remain_time_update",
    "req_remain_distance_update",
    "rec_remain_distance_update",
    "req_remain_route_update",
    "rec_remain_route_update",
    "req_gps_status_change",
    "rec_gps_status_change",
    "req_route_faraway",
    "rec_route_faraway",
    "req_route_plan_yawing",
    "rec_route_plan_yawing",
    "req_re_route_complete",
    "rec_re_route_complete",
    "req_arrive_destination",
    "rec_arrive_destination",
    "req_poi_sug_search",
    "rec_poi_sug_search",
    "req_route_riding_search",
    "rec_route_riding_search",
    "req_route_walking_search",
    "rec_route_walking_search",
    "req_route_transit_search",
    "rec_route_transit_search",
    "req_geocode_search",
    "rec_geocode_search",
    "req_regeocode_search",
    "rec_regeocode_search",
    "req_destroy_page",
    "null",
    "null",
    "null",
    "req_destroy_from_watch",
    "rec_destroy_from_watch",
};

static inline const char* vector_navi_get_str(uint8_t type)
{
    return g_vector_navi_str_tb[type - MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE];
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* BAIDU_APP_H */
