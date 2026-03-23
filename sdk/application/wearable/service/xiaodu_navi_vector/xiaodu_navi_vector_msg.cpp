
#include "xiaodu_navi_vector.h"
#include "msg_center_protocol.h"
#include "msg_center_cmd.h"
#include "ohos_init.h"
namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

static const msg_center_cmd_map_t g_msg_center_xiaodu_navi_vector_type_tbl[] = {
#ifdef JS_ENABLE
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_PULL_UP,              msg_center_vector_navi_pull_up},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_SVG,             msg_center_vector_navi_recv_svg},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_SVG,              msg_center_vector_navi_req_svg},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DST_BY_TEST,      msg_center_vector_navi_req_dst_by_text},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DST_BY_PCM,       msg_center_vector_navi_req_dst_by_pcm},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_DST_LIST,        msg_center_vector_navi_recv_dst_list},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_START_FROM_WATCH, msg_center_vector_navi_start_from_watch},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_NAVI_INFO,       msg_center_vector_navi_recv_navi_info},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_START_FROM_PHONE, msg_center_vector_navi_start_from_phone},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI,              msg_center_vector_navi_req_poi},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_POI,             msg_center_vector_navi_recv_poi},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_FINISH_FROM_WATCH,    msg_center_vector_navi_finish_from_watch},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_FINISH_FROM_PHONE,    msg_center_vector_navi_finish_from_phone},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE,     msg_center_vector_navi_req_auth_license},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_AUTH_LICENSE,     msg_center_vector_navi_rec_auth_license},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION,     msg_center_vector_navi_req_geo_location},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEO_LOCATION,     msg_center_vector_navi_rec_geo_location},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT,        msg_center_vector_navi_req_navi_init},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_INIT,        msg_center_vector_navi_rec_navi_init},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START, msg_center_vector_navi_req_route_plan_start},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_START, msg_center_vector_navi_rec_route_plan_start},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC,  msg_center_vector_navi_req_route_plan_succ},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_SUCC,  msg_center_vector_navi_rec_route_plan_succ},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL,  msg_center_vector_navi_req_route_plan_fail},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_FAIL,  msg_center_vector_navi_rec_route_plan_fail},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START,       msg_center_vector_navi_req_navi_start},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_START,       msg_center_vector_navi_rec_navi_start},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY,     msg_center_vector_navi_req_navi_destroy},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_DESTROY,     msg_center_vector_navi_rec_navi_destroy},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE, msg_center_vector_navi_req_road_guide_text_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_TEXT_UPDATE, msg_center_vector_navi_rec_road_guide_text_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE, msg_center_vector_navi_req_road_guide_icon_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_ICON_UPDATE, msg_center_vector_navi_rec_road_guide_icon_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE,     msg_center_vector_navi_req_remain_time_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_TIME_UPDATE,     msg_center_vector_navi_rec_remain_time_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE, msg_center_vector_navi_req_remain_distance_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_DISTANCE_UPDATE, msg_center_vector_navi_rec_remain_distance_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE,    msg_center_vector_navi_req_remain_route_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_ROUTE_UPDATE,    msg_center_vector_navi_rec_remain_route_update},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE,      msg_center_vector_navi_req_gps_status_change},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GPS_STATUS_CHANGE,      msg_center_vector_navi_rec_gps_status_change},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY,          msg_center_vector_navi_req_route_faraway},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_FARAWAY,          msg_center_vector_navi_rec_route_faraway},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING,      msg_center_vector_navi_req_route_plan_yawing},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_YAWING,      msg_center_vector_navi_rec_route_plan_yawing},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE,      msg_center_vector_navi_req_re_route_complete},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_RE_ROUTE_COMPLETE,      msg_center_vector_navi_rec_re_route_complete},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION,      msg_center_vector_navi_req_arrive_destination},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ARRIVE_DSTINATION,      msg_center_vector_navi_rec_arrive_destination},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH,         msg_center_vertor_navi_req_poi_sug_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_POI_SUG_SEARCH,         msg_center_vertor_navi_rec_poi_sug_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH,    msg_center_vector_navi_req_route_riding_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH,    msg_center_vector_navi_rec_route_riding_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH,   msg_center_vector_navi_req_route_walking_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH,   msg_center_vector_navi_rec_route_walking_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH,   msg_center_vector_navi_req_route_transit_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH,   msg_center_vector_navi_rec_route_transit_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH,         msg_center_vector_navi_req_geocode_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEOCODE_SEARCH,         msg_center_vector_navi_rec_geocode_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH,       msg_center_vector_navi_req_regeocode_search},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REGEOCODE_SEARCH,       msg_center_vector_navi_rec_regeocode_search},
    /* 地图私有协议 */
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_PAGE,           msg_center_vector_navi_req_destroy_page},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_DISCONNECTED,            NULL},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED,               NULL},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_FIRST_BATCH,            msg_center_vector_navi_rec_first_batch},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_FROM_WATCH,     msg_center_vector_navi_req_destroy_from_watch},
    { MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_DESTROY_FROM_WATCH,     msg_center_vector_navi_rec_destroy_from_watch},
#endif
};

errcode_t msg_center_xiaodu_navi_vetcor_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    uint8_t tbl_size = sizeof(g_msg_center_xiaodu_navi_vector_type_tbl) / sizeof(g_msg_center_xiaodu_navi_vector_type_tbl[0]);
    uint8_t i;
    for (i = 0; i < tbl_size; i++) {
        msg_center_cmd_map_t *item = (msg_center_cmd_map_t *)&g_msg_center_xiaodu_navi_vector_type_tbl[i];
        if ((item->type == type) && (item->handler != NULL)) {
            item->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

void xiaodu_navi_vetcor_init(void)
{
    msg_center_register_cmd(MSGCENTER_CMD_VECTOR_NAVI, msg_center_xiaodu_navi_vetcor_dispatch);
}

#ifdef JS_ENABLE
APP_FEATURE_INIT_PRI(xiaodu_navi_vetcor_init, LAYER_INIT_LEVEL_4);
#endif
#ifdef __cplusplus
}
#endif
}  // namespace OHOS