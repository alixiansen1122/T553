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

#include "ace_log.h"
#include "baidumap.h"
#include "baidumap_js_location.h"
#include "baidumap_js_component.h"
#include "xiaodu_navi_vector.h"
#include "xiaodu_voice.h"
#include "msg_center_protocol.h"
#include "js_async_work.h"
#include "adapter.h"

#include "gfx_utils/heap_base.h"

typedef struct : public OHOS::HeapBase {
    char *msgBody;
    uint8_t type;
    bool flag;
} BaiduMapAsyncMsgParams;

typedef struct : public OHOS::HeapBase {
    double lon;
    double lat;
} BaiduMapJSCoordinate;

constexpr char LON[] = "lon";
constexpr char LAT[] = "lat";
// BT_CONNECT
constexpr char CONNECT[] = "connectcallback";
constexpr char DISCONNECT[] = "disconnectcallback";

namespace OHOS {
namespace ACELite {

static int32_t DmsLiteCopyStr(char **destStr, const char *srcStr);

static void ExecuteSendMsgToJS(void *data);

static bool CheckIsJsValueFunction(OHOS::ACELite::JSIValue jsValue);

// poi_nearby_search
OHOS::ACELite::JSIValue poi_successCallback = nullptr;
OHOS::ACELite::JSIValue poi_failCallback = nullptr;
OHOS::ACELite::JSIValue poi_callbackContext = nullptr;
// route_riding_search
OHOS::ACELite::JSIValue route_riding_successCallback = nullptr;
OHOS::ACELite::JSIValue route_riding_failCallback = nullptr;
OHOS::ACELite::JSIValue route_riding_callbackContext = nullptr;
// route_walking_search
OHOS::ACELite::JSIValue route_walking_successCallback = nullptr;
OHOS::ACELite::JSIValue route_walking_failCallback = nullptr;
OHOS::ACELite::JSIValue route_walking_callbackContext = nullptr;
// route_transit_search
OHOS::ACELite::JSIValue route_transit_successCallback = nullptr;
OHOS::ACELite::JSIValue route_transit_failCallback = nullptr;
OHOS::ACELite::JSIValue route_transit_callbackContext = nullptr;
// auth_license
OHOS::ACELite::JSIValue auth_successCallback = nullptr;
OHOS::ACELite::JSIValue auth_failCallback = nullptr;
OHOS::ACELite::JSIValue auth_callbackContext = nullptr;
// naviInit
OHOS::ACELite::JSIValue nav_successCallback = nullptr;
OHOS::ACELite::JSIValue nav_failCallback = nullptr;
OHOS::ACELite::JSIValue nav_callbackContext = nullptr;
// getGeolocation
OHOS::ACELite::JSIValue geo_location_Callback = nullptr;
OHOS::ACELite::JSIValue geo_location_callbackContext = nullptr;
// onRoutePlanStart
OHOS::ACELite::JSIValue route_plan_start_Callback = nullptr;
OHOS::ACELite::JSIValue route_plan_start_callbackContext = nullptr;
// onRoutePlanSuccess
OHOS::ACELite::JSIValue route_plan_success_Callback = nullptr;
OHOS::ACELite::JSIValue route_plan_success_callbackContext = nullptr;
// onRoutePlanFail
OHOS::ACELite::JSIValue route_plan_fail_Callback = nullptr;
OHOS::ACELite::JSIValue route_plan_fail_callbackContext = nullptr;
// onNaviStart
OHOS::ACELite::JSIValue navi_start_Callback = nullptr;
OHOS::ACELite::JSIValue navi_start_callbackContext = nullptr;
// onNaviDestroy
OHOS::ACELite::JSIValue navi_destroy_Callback = nullptr;
OHOS::ACELite::JSIValue navi_destroy_callbackContext = nullptr;
// onRoadGuideTextUpdate
OHOS::ACELite::JSIValue road_guide_text_update_Callback = nullptr;
OHOS::ACELite::JSIValue road_guide_text_update_callbackContext = nullptr;
// onRouteGuideIconUpdate
OHOS::ACELite::JSIValue road_guide_icon_update_Callback = nullptr;
OHOS::ACELite::JSIValue road_guide_icon_update_callbackContext = nullptr;
// onRemainTimeUpdate
OHOS::ACELite::JSIValue remain_time_Callback = nullptr;
OHOS::ACELite::JSIValue remain_time_callbackContext = nullptr;
// onRemainDistanceUpdate
OHOS::ACELite::JSIValue remain_distance_Callback = nullptr;
OHOS::ACELite::JSIValue remain_distance_callbackContext = nullptr;
// onRemainRouteUpdate
OHOS::ACELite::JSIValue remain_route_Callback = nullptr;
OHOS::ACELite::JSIValue remain_route_callbackContext = nullptr;
// onGpsStatusChange
OHOS::ACELite::JSIValue gps_status_Callback = nullptr;
OHOS::ACELite::JSIValue gps_status_callbackContext = nullptr;
// onRouteFarAway
OHOS::ACELite::JSIValue route_far_Callback = nullptr;
OHOS::ACELite::JSIValue route_far_callbackContext = nullptr;
// onRoutePlanYawing
OHOS::ACELite::JSIValue route_plan_yawing_Callback = nullptr;
OHOS::ACELite::JSIValue route_plan_yawing_callbackContext = nullptr;
// onReRouteComplete
OHOS::ACELite::JSIValue reroute_Callback = nullptr;
OHOS::ACELite::JSIValue reroute_callbackContext = nullptr;
// onArriveDest
OHOS::ACELite::JSIValue arrivedest_Callback = nullptr;
OHOS::ACELite::JSIValue arrivedest_callbackContext = nullptr;
// poi_sug_search
OHOS::ACELite::JSIValue poi_sug_successCallback = nullptr;
OHOS::ACELite::JSIValue poi_sug_failCallback = nullptr;
OHOS::ACELite::JSIValue poi_sug_callbackContext = nullptr;
// geocode_search
OHOS::ACELite::JSIValue geocode_search_successCallback = nullptr;
OHOS::ACELite::JSIValue geocode_search_failCallback = nullptr;
OHOS::ACELite::JSIValue geocode_search_callbackContext = nullptr;
// regeocode_search
OHOS::ACELite::JSIValue regeocode_search_successCallback = nullptr;
OHOS::ACELite::JSIValue regeocode_search_failCallback = nullptr;
OHOS::ACELite::JSIValue regeocode_search_callbackContext = nullptr;
// BT_connect
OHOS::ACELite::JSIValue bt_connect_successCallback = nullptr;
OHOS::ACELite::JSIValue bt_connect_failCallback = nullptr;
OHOS::ACELite::JSIValue bt_connect_callbackContext = nullptr;

static bool CheckIsJsValueDefined(OHOS::ACELite::JSIValue jsValue)
{
    if (jsValue == nullptr) {
        return false;
    }

    if (OHOS::ACELite::JSI::ValueIsUndefined(jsValue)) {
        return false;
    }

    return true;
}

static int32_t DmsLiteCopyStr(char **destStr, const char *srcStr)
{
    if (destStr == nullptr || srcStr == nullptr) {
        return -1;
    }

    uint32_t length = strlen(srcStr) + 1;
    *destStr = static_cast<char *>(AdapterMalloc(length));
    if (*destStr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for destination string");
        return -1;
    }

    if (memset_s(*destStr, length, 0, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to set memory for destination string");
        AdapterFree(*destStr);
        return -1;
    }
    if (memcpy_s(*destStr, length, srcStr, length) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy source string to destination string");
        AdapterFree(*destStr);
        return -1;
    }

    return 0;
}

static void ExecuteSendMsgToJS(void *data)
{
    BaiduMapAsyncMsgParams *params = static_cast<BaiduMapAsyncMsgParams *>(data);
    if (params == nullptr) {
        return;
    }

    OHOS::ACELite::JSIValue success = nullptr;
    OHOS::ACELite::JSIValue fail = nullptr;
    OHOS::ACELite::JSIValue context = nullptr;
    uint8_t type = params->type;
    bool flag = params->flag;
    switch (type) {
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_RECV_POI:
            success = poi_successCallback;
            fail = poi_failCallback;
            context = poi_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_RIDING_SEARCH:
            success = route_riding_successCallback;
            fail = route_riding_failCallback;
            context = route_riding_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_WALKING_SEARCH:
            success = route_walking_successCallback;
            fail = route_walking_failCallback;
            context = route_walking_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_TRANSIT_SEARCH:
            success = route_transit_successCallback;
            fail = route_transit_failCallback;
            context = route_transit_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_POI_SUG_SEARCH:
            success = poi_sug_successCallback;
            fail = poi_sug_failCallback;
            context = poi_sug_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_AUTH_LICENSE:
            success = auth_successCallback;
            fail = auth_failCallback;
            context = auth_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_INIT:
            success = nav_successCallback;
            fail = nav_failCallback;
            context = nav_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEO_LOCATION:
            success = geo_location_Callback;
            context = geo_location_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_START:
            success = route_plan_start_Callback;
            context = route_plan_start_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_SUCC:
            success = route_plan_success_Callback;
            context = route_plan_success_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_FAIL:
            success = route_plan_fail_Callback;
            context = route_plan_fail_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_START:
            success = navi_start_Callback;
            context = navi_start_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_NAVI_DESTROY:
            success = navi_destroy_Callback;
            context = navi_destroy_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_TEXT_UPDATE:
            success = road_guide_text_update_Callback;
            context = road_guide_text_update_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROAD_GUIDE_ICON_UPDATE:
            success = road_guide_icon_update_Callback;
            context = road_guide_icon_update_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_TIME_UPDATE:
            success = remain_time_Callback;
            context = remain_time_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_DISTANCE_UPDATE:
            success = remain_distance_Callback;
            context = remain_distance_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REMAIN_ROUTE_UPDATE:
            success = remain_route_Callback;
            context = remain_route_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GPS_STATUS_CHANGE:
            success = gps_status_Callback;
            context = gps_status_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_FARAWAY:
            success = route_far_Callback;
            context = route_far_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ROUTE_PLAN_YAWING:
            success = route_plan_yawing_Callback;
            context = route_plan_yawing_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_RE_ROUTE_COMPLETE:
            success = reroute_Callback;
            context = reroute_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_ARRIVE_DSTINATION:
            success = arrivedest_Callback;
            context = arrivedest_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_GEOCODE_SEARCH:
            success = geocode_search_successCallback;
            fail = geocode_search_failCallback;
            context = geocode_search_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REC_REGEOCODE_SEARCH:
            success = regeocode_search_successCallback;
            fail = regeocode_search_failCallback;
            context = regeocode_search_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED:
            success = bt_connect_successCallback;
            context = bt_connect_callbackContext;
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_DISCONNECTED:
            success = bt_connect_failCallback;
            context = bt_connect_callbackContext;
            break;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid command type");
            break;
    }

    char *msgBody = params->msgBody;
    delete params;
    params = nullptr;
    OHOS::ACELite::JSIValue dataInfo = OHOS::ACELite::JSI::CreateObject();
    OHOS::ACELite::JSI::SetBooleanProperty(dataInfo, "isFileType", false);
    OHOS::ACELite::JSI::SetBooleanProperty(dataInfo, "isRegister", false);
    OHOS::ACELite::JSI::SetStringProperty(dataInfo, "message", msgBody);
    HILOG_INFO(HILOG_MODULE_ACE, "ExecuteSendMsgToJS type = %d flag = %d", type, flag);
    if (!flag) {
        if ((!CheckIsJsValueFunction(fail)) || (!CheckIsJsValueDefined(context))) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ExecuteSendMsgToJS Callback or Context is nullptr!");
            OHOS::ACELite::JSI::ReleaseValue(dataInfo);
            AdapterFree(msgBody);
            return;
        }
        OHOS::ACELite::JSIValue failArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
        OHOS::ACELite::JSI::CallFunction(fail, context, failArgv, OHOS::ACELite::ARGC_ONE);
        OHOS::ACELite::JSI::ReleaseValue(dataInfo);
    } else {
        if ((!CheckIsJsValueDefined(context)) || (!CheckIsJsValueFunction(success))) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ExecuteSendMsgToJS Callback or Context is nullptr!");
            OHOS::ACELite::JSI::ReleaseValue(dataInfo);
            AdapterFree(msgBody);
            return;
        }
        OHOS::ACELite::JSIValue successArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};
        OHOS::ACELite::JSI::CallFunction(success, context, successArgv, OHOS::ACELite::ARGC_ONE);
        OHOS::ACELite::JSI::ReleaseValue(dataInfo);
    }
    AdapterFree(msgBody);

    return;
}

static bool CheckIsJsValueFunction(OHOS::ACELite::JSIValue jsValue)
{
    if (!CheckIsJsValueDefined(jsValue)) {
        return false;
    }

    return OHOS::ACELite::JSI::ValueIsFunction(jsValue);
}

static void ReleaseJsValue(OHOS::ACELite::JSIValue &jsValue)
{
    if (CheckIsJsValueDefined(jsValue)) {
        OHOS::ACELite::JSI::ReleaseValue(jsValue);
        jsValue = OHOS::ACELite::JSI::CreateUndefined();
    }
    return;
}

static void ReleaseMsgJsValue(uint8_t type)
{
    HILOG_INFO(HILOG_MODULE_ACE, "ReleaseMsgJsValue");
    switch (type) {
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI:
            ReleaseJsValue(poi_callbackContext);
            ReleaseJsValue(poi_successCallback);
            ReleaseJsValue(poi_failCallback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE:
            ReleaseJsValue(auth_callbackContext);
            ReleaseJsValue(auth_successCallback);
            ReleaseJsValue(auth_failCallback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT:
            ReleaseJsValue(nav_callbackContext);
            ReleaseJsValue(nav_successCallback);
            ReleaseJsValue(nav_failCallback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START:
            ReleaseJsValue(route_plan_start_callbackContext);
            ReleaseJsValue(route_plan_start_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH:
            ReleaseJsValue(poi_sug_callbackContext);
            ReleaseJsValue(poi_sug_successCallback);
            ReleaseJsValue(poi_sug_failCallback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC:
            ReleaseJsValue(route_plan_success_callbackContext);
            ReleaseJsValue(route_plan_success_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION:
            ReleaseJsValue(geo_location_callbackContext);
            ReleaseJsValue(geo_location_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL:
            ReleaseJsValue(route_plan_fail_callbackContext);
            ReleaseJsValue(route_plan_fail_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START:
            ReleaseJsValue(navi_start_callbackContext);
            ReleaseJsValue(navi_start_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY:
            ReleaseJsValue(navi_destroy_callbackContext);
            ReleaseJsValue(navi_destroy_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE:
            ReleaseJsValue(road_guide_text_update_callbackContext);
            ReleaseJsValue(road_guide_text_update_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE:
            ReleaseJsValue(road_guide_icon_update_callbackContext);
            ReleaseJsValue(road_guide_icon_update_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE:
            ReleaseJsValue(remain_time_callbackContext);
            ReleaseJsValue(remain_time_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE:
            ReleaseJsValue(remain_distance_callbackContext);
            ReleaseJsValue(remain_distance_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE:
            ReleaseJsValue(remain_route_callbackContext);
            ReleaseJsValue(remain_route_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE:
            ReleaseJsValue(gps_status_callbackContext);
            ReleaseJsValue(gps_status_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY:
            ReleaseJsValue(route_far_callbackContext);
            ReleaseJsValue(route_far_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING:
            ReleaseJsValue(route_plan_yawing_callbackContext);
            ReleaseJsValue(route_plan_yawing_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE:
            ReleaseJsValue(reroute_callbackContext);
            ReleaseJsValue(reroute_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION:
            ReleaseJsValue(arrivedest_callbackContext);
            ReleaseJsValue(arrivedest_Callback);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH:
            ReleaseJsValue(geocode_search_successCallback);
            ReleaseJsValue(geocode_search_failCallback);
            ReleaseJsValue(geocode_search_callbackContext);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH:
            ReleaseJsValue(regeocode_search_successCallback);
            ReleaseJsValue(regeocode_search_failCallback);
            ReleaseJsValue(regeocode_search_callbackContext);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH:
            ReleaseJsValue(route_riding_successCallback);
            ReleaseJsValue(route_riding_failCallback);
            ReleaseJsValue(route_riding_callbackContext);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH:
            ReleaseJsValue(route_walking_successCallback);
            ReleaseJsValue(route_walking_failCallback);
            ReleaseJsValue(route_walking_callbackContext);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH:
            ReleaseJsValue(route_transit_successCallback);
            ReleaseJsValue(route_transit_failCallback);
            ReleaseJsValue(route_transit_callbackContext);
            break;
        case MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED:
            ReleaseJsValue(bt_connect_successCallback);
            ReleaseJsValue(bt_connect_failCallback);
            ReleaseJsValue(bt_connect_callbackContext);
            break;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "ReleaseMsgJsValue Invalid command type");
            break;
    }
    return;
}

bool setCallbackFunction(OHOS::ACELite::JSIValue callback, uint8_t type, bool isSuccess)
{
    if (!OHOS::ACELite::JSI::ValueIsUndefined(callback)) {
        if (!OHOS::ACELite::JSI::ValueIsFunction(callback)) {
            HILOG_INFO(HILOG_MODULE_ACE, "callback is not a function will release function");
            ReleaseJsValue(callback);
            callback = nullptr;
            return false;
        } else {
            switch (type) {
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI:
                    if (isSuccess) {
                        poi_successCallback = callback;
                    } else {
                        poi_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE:
                    if (isSuccess) {
                        auth_successCallback = callback;
                    } else {
                        auth_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT:
                    if (isSuccess) {
                        nav_successCallback = callback;
                    } else {
                        nav_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH:
                    if (isSuccess) {
                        poi_sug_successCallback = callback;
                    } else {
                        poi_sug_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION:
                    geo_location_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START:
                    route_plan_start_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC:
                    route_plan_success_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL:
                    route_plan_fail_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START:
                    navi_start_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY:
                    navi_destroy_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE:
                    road_guide_text_update_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE:
                    road_guide_icon_update_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE:
                    remain_time_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE:
                    remain_distance_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE:
                    remain_route_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE:
                    gps_status_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY:
                    route_far_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING:
                    route_plan_yawing_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE:
                    reroute_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION:
                    arrivedest_Callback = callback;
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH:
                    if (isSuccess) {
                        geocode_search_successCallback = callback;
                    } else {
                        geocode_search_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH:
                    if (isSuccess) {
                        regeocode_search_successCallback = callback;
                    } else {
                        regeocode_search_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH:
                    if (isSuccess) {
                        route_riding_successCallback = callback;
                    } else {
                        route_riding_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH:
                    if (isSuccess) {
                        route_walking_successCallback = callback;
                    } else {
                        route_walking_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH:
                    if (isSuccess) {
                        route_transit_successCallback = callback;
                    } else {
                        route_transit_failCallback = callback;
                    }
                    break;
                case MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED:
                    if (isSuccess) {
                        bt_connect_successCallback = callback;
                    } else {
                        bt_connect_failCallback = callback;
                    }
                    break;
                default:
                    HILOG_ERROR(HILOG_MODULE_ACE, "setCallbackFunction Invalid command type");
                    return false;
            }
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "callback value is undefined");
        return false;
    }

    return true;
}

/**
 * MsgCenter interface
 */

JSIValue BaiduMapComponentModule::poi_nearby_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "poi_nearby_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "poi_nearby_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI);
    poi_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback = setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_poi(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::route_riding_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "route_riding_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "route_riding_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH);
    route_riding_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH, true);
    bool hasFailCallback =
        setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_route_riding_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_RIDING_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::route_walking_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "route_walking_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "route_walking_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH);
    route_walking_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH, true);
    bool hasFailCallback =
        setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_route_walking_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_WALKING_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::route_transit_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "route_transit_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "route_transit_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH);
    route_transit_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH, true);
    bool hasFailCallback =
        setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_route_transit_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_TRANSIT_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::auth_license(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "auth_license");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "auth_license : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE);
    auth_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_auth_license(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_AUTH_LICENSE, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::getGeolocation(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "getGeolocation");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "getGeolocation : %s", data);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION);
    geo_location_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    bool hasCallback = setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION, true);
    if (hasCallback) {
        msg_center_vector_navi_req_geo_location(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEO_LOCATION, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::naviInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "naviInit");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "naviInit : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT);
    nav_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback = setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_navi_init(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_INIT, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRoutePlanStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRoutePlanStart");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START);
    route_plan_start_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_START, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRoutePlanSuccess(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRoutePlanSuccess");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC);
    route_plan_success_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_SUCC, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRoutePlanFail(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRoutePlanFail");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL);
    route_plan_fail_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_FAIL, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onNaviStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onNaviStart");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START);
    navi_start_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_START, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onNaviDestroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onNaviDestroy");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY);
    navi_destroy_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_NAVI_DESTROY, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRoadGuideTextUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRoadGuideTextUpdate");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE);
    road_guide_text_update_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_TEXT_UPDATE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRouteGuideIconUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRouteGuideIconUpdate");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE);
    road_guide_icon_update_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROAD_GUIDE_ICON_UPDATE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRemainTimeUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRemainTimeUpdate");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE);
    remain_time_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_TIME_UPDATE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRemainDistanceUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRemainDistanceUpdate");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE);
    remain_distance_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_DISTANCE_UPDATE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRemainRouteUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRemainRouteUpdate");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE);
    remain_route_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REMAIN_ROUTE_UPDATE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onGpsStatusChange(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onGpsStatusChange");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE);
    gps_status_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GPS_STATUS_CHANGE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRouteFarAway(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRouteFarAway");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY);
    route_far_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_FARAWAY, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onRoutePlanYawing(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onRoutePlanYawing");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING);
    route_plan_yawing_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING, true);
    // msg_center_vector_navi_req_route_plan_yawing(MSGCENTER_CMD_VECTOR_NAVI,
    // MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ROUTE_PLAN_YAWING,Callback,1);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onReRouteComplete(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onReRouteComplete");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE);
    reroute_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_RE_ROUTE_COMPLETE, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::onArriveDest(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onArriveDest");
    OHOS::ACELite::JSIValue Callback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_CALLBACK);
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION);
    arrivedest_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    setCallbackFunction(Callback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_ARRIVE_DSTINATION, true);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::poi_sug_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "poi_sug_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "poi_sug_search : %s", data);

    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH);
    poi_sug_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vertor_navi_req_poi_sug_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_POI_SUG_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

/**
 * location interface
 */
JSIValue BaiduMapComponentModule::Location_WGS84ToGCJ02(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToGCJ02");
    double lon = OHOS::ACELite::JSI::GetNumberProperty(args[0], LON);
    double lat = OHOS::ACELite::JSI::GetNumberProperty(args[0], LAT);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToGCJ02 befor lon: %lf \s", lon);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToGCJ02 befor lat: %lf \s", lat);
    JSCoordinate params = {0};
    WGS84ToGCJ02(lon, lat, params);

    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToGCJ02 after lon: %lf \s", params.lon);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToGCJ02 after lat: %lf \s", params.lat);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "lon", params.lon);
    JSI::SetNumberProperty(result, "lat", params.lat);
    return result;
}

JSIValue BaiduMapComponentModule::Location_GCJ02ToBD09(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "GCJ02ToBD09");
    double lon = OHOS::ACELite::JSI::GetNumberProperty(args[0], LON);
    double lat = OHOS::ACELite::JSI::GetNumberProperty(args[0], LAT);
    HILOG_INFO(HILOG_MODULE_ACE, "GCJ02ToBD09 befor lon: %lf \s", lon);
    HILOG_INFO(HILOG_MODULE_ACE, "GCJ02ToBD09 befor lat: %lf \s", lat);
    JSCoordinate params = {0};
    GCJ02ToBD09(lon, lat, params);

    HILOG_INFO(HILOG_MODULE_ACE, "GCJ02ToBD09 after lon: %lf \s", params.lon);
    HILOG_INFO(HILOG_MODULE_ACE, "GCJ02ToBD09 after lat: %lf \s", params.lat);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "lon", params.lon);
    JSI::SetNumberProperty(result, "lat", params.lat);
    return result;
}

JSIValue BaiduMapComponentModule::Location_WGS84ToBD09(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToBD09");
    double lon = OHOS::ACELite::JSI::GetNumberProperty(args[0], LON);
    double lat = OHOS::ACELite::JSI::GetNumberProperty(args[0], LAT);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToBD09 befor lon: %lf \s", lon);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToBD09 befor lat: %lf \s", lat);
    JSCoordinate params = {0};
    WGS84ToBD09(lon, lat, params);

    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToBD09 after lon: %lf \s", params.lon);
    HILOG_INFO(HILOG_MODULE_ACE, "WGS84ToBD09 after lat: %lf \s", params.lat);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "lon", params.lon);
    JSI::SetNumberProperty(result, "lat", params.lat);
    return result;
}

JSIValue BaiduMapComponentModule::Location_BD09ToGCJ02(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "BD09ToGCJ02");
    double lon = OHOS::ACELite::JSI::GetNumberProperty(args[0], LON);
    double lat = OHOS::ACELite::JSI::GetNumberProperty(args[0], LAT);
    HILOG_INFO(HILOG_MODULE_ACE, "BD09ToGCJ02 befor lon: %lf \s", lon);
    HILOG_INFO(HILOG_MODULE_ACE, "BD09ToGCJ02 befor lat: %lf \s", lat);
    JSCoordinate params = {0};
    BD09ToGCJ02(lon, lat, params);

    HILOG_INFO(HILOG_MODULE_ACE, "BD09ToGCJ02 after lon: %lf \s", params.lon);
    HILOG_INFO(HILOG_MODULE_ACE, "BD09ToGCJ02 after lat: %lf \s", params.lat);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "lon", params.lon);
    JSI::SetNumberProperty(result, "lat", params.lat);
    return result;
}

/**
 * geocode interface
 */
JSIValue BaiduMapComponentModule::Geocode_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "Geocode_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "Geocode_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH);
    geocode_search_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_geocode_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_GEOCODE_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

JSIValue BaiduMapComponentModule::Regeocode_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "Regeocode_search");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "Regeocode_search : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH);
    regeocode_search_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue successCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_SUCCESS);
    OHOS::ACELite::JSIValue failCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CB_FAIL);
    bool hasSuccessCallback =
        setCallbackFunction(successCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH, true);
    bool hasFailCallback = setCallbackFunction(failCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH, false);
    if (hasSuccessCallback || hasFailCallback) {
        msg_center_vector_navi_req_regeocode_search(
            MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_REGEOCODE_SEARCH, data, strlen(data));
    }
    ace_free(data);
    return JSI::CreateBoolean(true);
}

/**
 * destroy page interface
 */
JSIValue BaiduMapComponentModule::Destroy_page(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "Destroy_page");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "Destroy_page : %s", data);
    msg_center_vector_navi_req_destroy_page(
        MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_PAGE, data, strlen(data));
    ace_free(data);
    return JSI::CreateBoolean(true);
}

/**
 * BT connect interface
 */
JSIValue BaiduMapComponentModule::onBTConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "onBTConnect");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "onBTConnect : %s", data);
    // success Callback & failed Callback
    ReleaseMsgJsValue(MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED);
    bt_connect_callbackContext = OHOS::ACELite::JSI::AcquireValue(thisVal);
    OHOS::ACELite::JSIValue connectCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], CONNECT);
    OHOS::ACELite::JSIValue disconnectCallback = OHOS::ACELite::JSI::GetNamedProperty(args[0], DISCONNECT);
    setCallbackFunction(connectCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED, true);
    setCallbackFunction(disconnectCallback, MSGCENTER_TYPE_ID_VECTOR_NAVI_BT_CONNECTED, false);
    ace_free(data);
    return JSI::CreateBoolean(true);
}

/**
 * exit Navigation
 */
JSIValue BaiduMapComponentModule::NaviDestroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "exitNavigation");
    char *data = JSI::JsonStringify(args[0]);
    HILOG_INFO(HILOG_MODULE_ACE, "exitNavigation : %s", data);
    msg_center_vector_navi_req_destroy_from_watch(
        MSGCENTER_CMD_VECTOR_NAVI, MSGCENTER_TYPE_ID_VECTOR_NAVI_REQ_DESTROY_FROM_WATCH, data, strlen(data));
    ace_free(data);
    return JSI::CreateBoolean(true);
}

/**
 * stop answer
 */
JSIValue BaiduMapComponentModule::StopAnswer(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "StopAnswer");
    uint8_t data = (uint8_t)JSI::GetNumberProperty(args[0], "data");
    HILOG_INFO(HILOG_MODULE_ACE, "StopAnswer : %d", data);
    uint32_t ret = msg_center_xiaodu_send_stop_answer(
            MSGCENTER_CMD_XIAODU, MSGCENTER_TYPE_ID_XIAODU_SEND_JS_EXIT, &data, sizeof(data));
    return (ret == ERRCODE_SUCC) ? JSI::CreateBoolean(true) : JSI::CreateBoolean(false);
}

JSIValue BaiduMapComponentModule::MapInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "MapInit");
    // poi_nearby_search
    poi_successCallback = nullptr;
    poi_failCallback = nullptr;
    poi_callbackContext = nullptr;
    // route_riding_search
    route_riding_successCallback = nullptr;
    route_riding_failCallback = nullptr;
    route_riding_callbackContext = nullptr;
    // route_walking_search
    route_walking_successCallback = nullptr;
    route_walking_failCallback = nullptr;
    route_walking_callbackContext = nullptr;
    // route_transit_search
    route_transit_successCallback = nullptr;
    route_transit_failCallback = nullptr;
    route_transit_callbackContext = nullptr;
    // auth_license
    auth_successCallback = nullptr;
    auth_failCallback = nullptr;
    auth_callbackContext = nullptr;
    // naviInit
    nav_successCallback = nullptr;
    nav_failCallback = nullptr;
    nav_callbackContext = nullptr;
    // getGeolocation
    geo_location_Callback = nullptr;
    geo_location_callbackContext = nullptr;
    // onRoutePlanStart
    route_plan_start_Callback = nullptr;
    route_plan_start_callbackContext = nullptr;
    // onRoutePlanSuccess
    route_plan_success_Callback = nullptr;
    route_plan_success_callbackContext = nullptr;
    // onRoutePlanFail
    route_plan_fail_Callback = nullptr;
    route_plan_fail_callbackContext = nullptr;
    // onNaviStart
    navi_start_Callback = nullptr;
    navi_start_callbackContext = nullptr;
    // onNaviDestroy
    navi_destroy_Callback = nullptr;
    navi_destroy_callbackContext = nullptr;
    // onRoadGuideTextUpdate
    road_guide_text_update_Callback = nullptr;
    road_guide_text_update_callbackContext = nullptr;
    // onRouteGuideIconUpdate
    road_guide_icon_update_Callback = nullptr;
    road_guide_icon_update_callbackContext = nullptr;
    // onRemainTimeUpdate
    remain_time_Callback = nullptr;
    remain_time_callbackContext = nullptr;
    // onRemainDistanceUpdate
    remain_distance_Callback = nullptr;
    remain_distance_callbackContext = nullptr;
    // onRemainRouteUpdate
    remain_route_Callback = nullptr;
    remain_route_callbackContext = nullptr;
    // onGpsStatusChange
    gps_status_Callback = nullptr;
    gps_status_callbackContext = nullptr;
    // onRouteFarAway
    route_far_Callback = nullptr;
    route_far_callbackContext = nullptr;
    // onRoutePlanYawing
    route_plan_yawing_Callback = nullptr;
    route_plan_yawing_callbackContext = nullptr;
    // onReRouteComplete
    reroute_Callback = nullptr;
    reroute_callbackContext = nullptr;
    // onArriveDest
    arrivedest_Callback = nullptr;
    arrivedest_callbackContext = nullptr;
    // poi_sug_search
    poi_sug_successCallback = nullptr;
    poi_sug_failCallback = nullptr;
    poi_sug_callbackContext = nullptr;
    // geocode_search
    geocode_search_successCallback = nullptr;
    geocode_search_failCallback = nullptr;
    geocode_search_callbackContext = nullptr;
    // regeocode_search
    regeocode_search_successCallback = nullptr;
    regeocode_search_failCallback = nullptr;
    regeocode_search_callbackContext = nullptr;
    // BT_connect
    bt_connect_successCallback = nullptr;
    bt_connect_failCallback = nullptr;
    bt_connect_callbackContext = nullptr;
    return JSI::CreateBoolean(true);
}

void BaiduMapComponentModule::OnDestroy()
{
    HILOG_INFO(HILOG_MODULE_ACE, "BaiduMapComponent OnDestroy\n");

    // poi_nearby_search
    ReleaseJsValue(poi_successCallback);
    ReleaseJsValue(poi_failCallback);
    ReleaseJsValue(poi_callbackContext);
    poi_successCallback = nullptr;
    poi_failCallback = nullptr;
    poi_callbackContext = nullptr;
    // route_riding_search
    ReleaseJsValue(route_riding_successCallback);
    ReleaseJsValue(route_riding_failCallback);
    ReleaseJsValue(route_riding_callbackContext);
    route_riding_successCallback = nullptr;
    route_riding_failCallback = nullptr;
    route_riding_callbackContext = nullptr;
    // route_walking_search
    ReleaseJsValue(route_walking_successCallback);
    ReleaseJsValue(route_walking_failCallback);
    ReleaseJsValue(route_walking_callbackContext);
    route_walking_successCallback = nullptr;
    route_walking_failCallback = nullptr;
    route_walking_callbackContext = nullptr;
    // route_transit_search
    ReleaseJsValue(route_transit_successCallback);
    ReleaseJsValue(route_transit_failCallback);
    ReleaseJsValue(route_transit_callbackContext);
    route_transit_successCallback = nullptr;
    route_transit_failCallback = nullptr;
    route_transit_callbackContext = nullptr;
    // auth_license
    ReleaseJsValue(auth_successCallback);
    ReleaseJsValue(auth_failCallback);
    ReleaseJsValue(auth_callbackContext);
    auth_successCallback = nullptr;
    auth_failCallback = nullptr;
    auth_callbackContext = nullptr;
    // onRoutePlanStart
    ReleaseJsValue(route_plan_start_Callback);
    ReleaseJsValue(route_plan_start_callbackContext);
    route_plan_start_Callback = nullptr;
    route_plan_start_callbackContext = nullptr;
    // onRoutePlanSuccess
    ReleaseJsValue(route_plan_success_Callback);
    ReleaseJsValue(route_plan_success_callbackContext);
    route_plan_success_Callback = nullptr;
    route_plan_success_callbackContext = nullptr;
    // onRoutePlanFail
    ReleaseJsValue(route_plan_fail_Callback);
    ReleaseJsValue(route_plan_fail_callbackContext);
    route_plan_fail_Callback = nullptr;
    route_plan_fail_callbackContext = nullptr;
    // onNaviStart
    ReleaseJsValue(navi_start_Callback);
    ReleaseJsValue(navi_start_callbackContext);
    navi_start_Callback = nullptr;
    navi_start_callbackContext = nullptr;
    // onNaviDestroy
    ReleaseJsValue(navi_destroy_Callback);
    ReleaseJsValue(navi_destroy_callbackContext);
    navi_destroy_Callback = nullptr;
    navi_destroy_callbackContext = nullptr;
    // onRoadGuideTextUpdate
    ReleaseJsValue(road_guide_text_update_Callback);
    ReleaseJsValue(road_guide_text_update_callbackContext);
    road_guide_text_update_Callback = nullptr;
    road_guide_text_update_callbackContext = nullptr;
    // onRouteGuideIconUpdate
    ReleaseJsValue(road_guide_icon_update_Callback);
    ReleaseJsValue(road_guide_icon_update_callbackContext);
    road_guide_icon_update_Callback = nullptr;
    road_guide_icon_update_callbackContext = nullptr;
    // onRemainTimeUpdate
    ReleaseJsValue(remain_time_Callback);
    ReleaseJsValue(remain_time_callbackContext);
    remain_time_Callback = nullptr;
    remain_time_callbackContext = nullptr;
    // onRemainDistanceUpdate
    ReleaseJsValue(remain_distance_Callback);
    ReleaseJsValue(remain_distance_callbackContext);
    remain_distance_Callback = nullptr;
    remain_distance_callbackContext = nullptr;
    // onRemainRouteUpdate
    ReleaseJsValue(remain_route_Callback);
    ReleaseJsValue(remain_route_callbackContext);
    remain_route_Callback = nullptr;
    remain_route_callbackContext = nullptr;
    // onGpsStatusChange
    ReleaseJsValue(gps_status_Callback);
    ReleaseJsValue(gps_status_callbackContext);
    gps_status_Callback = nullptr;
    gps_status_callbackContext = nullptr;
    // onRouteFarAway
    ReleaseJsValue(route_far_Callback);
    ReleaseJsValue(route_far_callbackContext);
    route_far_Callback = nullptr;
    route_far_callbackContext = nullptr;
    // onRoutePlanYawing
    ReleaseJsValue(route_plan_yawing_Callback);
    ReleaseJsValue(route_plan_yawing_callbackContext);
    route_plan_yawing_Callback = nullptr;
    route_plan_yawing_callbackContext = nullptr;
    // onReRouteComplete
    ReleaseJsValue(reroute_Callback);
    ReleaseJsValue(reroute_callbackContext);
    reroute_Callback = nullptr;
    reroute_callbackContext = nullptr;
    // onArriveDest
    ReleaseJsValue(arrivedest_Callback);
    ReleaseJsValue(arrivedest_callbackContext);
    arrivedest_Callback = nullptr;
    arrivedest_callbackContext = nullptr;
    // poi_sug_search
    ReleaseJsValue(poi_sug_successCallback);
    ReleaseJsValue(poi_sug_failCallback);
    ReleaseJsValue(poi_sug_callbackContext);
    poi_sug_successCallback = nullptr;
    poi_sug_failCallback = nullptr;
    poi_sug_callbackContext = nullptr;
    // geocode_search
    ReleaseJsValue(geocode_search_successCallback);
    ReleaseJsValue(geocode_search_failCallback);
    ReleaseJsValue(geocode_search_callbackContext);
    geocode_search_successCallback = nullptr;
    geocode_search_failCallback = nullptr;
    geocode_search_callbackContext = nullptr;
    // regeocode_search
    ReleaseJsValue(regeocode_search_successCallback);
    ReleaseJsValue(regeocode_search_failCallback);
    ReleaseJsValue(regeocode_search_callbackContext);
    regeocode_search_successCallback = nullptr;
    regeocode_search_failCallback = nullptr;
    regeocode_search_callbackContext = nullptr;
}

void InitBaiduMapComponentModule(JSIValue exports)
{
    HILOG_INFO(HILOG_MODULE_ACE, "InitBaiduMapComponentModule");
    /**
     * MsgCenter interface
     */
    JSI::SetModuleAPI(exports, "poi_nearby_search", BaiduMapComponentModule::poi_nearby_search);
    JSI::SetModuleAPI(exports, "route_riding_search", BaiduMapComponentModule::route_riding_search);
    JSI::SetModuleAPI(exports, "route_walking_search", BaiduMapComponentModule::route_walking_search);
    JSI::SetModuleAPI(exports, "route_transit_search", BaiduMapComponentModule::route_transit_search);
    JSI::SetModuleAPI(exports, "auth_license", BaiduMapComponentModule::auth_license);
    JSI::SetModuleAPI(exports, "getGeolocation", BaiduMapComponentModule::getGeolocation);
    JSI::SetModuleAPI(exports, "naviInit", BaiduMapComponentModule::naviInit);
    JSI::SetModuleAPI(exports, "onRoutePlanStart", BaiduMapComponentModule::onRoutePlanStart);
    JSI::SetModuleAPI(exports, "onRoutePlanSuccess", BaiduMapComponentModule::onRoutePlanSuccess);
    JSI::SetModuleAPI(exports, "onRoutePlanFail", BaiduMapComponentModule::onRoutePlanFail);
    JSI::SetModuleAPI(exports, "onNaviStart", BaiduMapComponentModule::onNaviStart);
    JSI::SetModuleAPI(exports, "onNaviDestroy", BaiduMapComponentModule::onNaviDestroy);
    JSI::SetModuleAPI(exports, "onRoadGuideTextUpdate", BaiduMapComponentModule::onRoadGuideTextUpdate);
    JSI::SetModuleAPI(exports, "onRouteGuideIconUpdate", BaiduMapComponentModule::onRouteGuideIconUpdate);
    JSI::SetModuleAPI(exports, "onRemainTimeUpdate", BaiduMapComponentModule::onRemainTimeUpdate);
    JSI::SetModuleAPI(exports, "onRemainDistanceUpdate", BaiduMapComponentModule::onRemainDistanceUpdate);
    JSI::SetModuleAPI(exports, "onRemainRouteUpdate", BaiduMapComponentModule::onRemainRouteUpdate);
    JSI::SetModuleAPI(exports, "onGpsStatusChange", BaiduMapComponentModule::onGpsStatusChange);
    JSI::SetModuleAPI(exports, "onRouteFarAway", BaiduMapComponentModule::onRouteFarAway);
    JSI::SetModuleAPI(exports, "onRoutePlanYawing", BaiduMapComponentModule::onRoutePlanYawing);
    JSI::SetModuleAPI(exports, "onReRouteComplete", BaiduMapComponentModule::onReRouteComplete);
    JSI::SetModuleAPI(exports, "onArriveDest", BaiduMapComponentModule::onArriveDest);
    JSI::SetModuleAPI(exports, "poi_sug_search", BaiduMapComponentModule::poi_sug_search);

    /**
     * location interface
     */
    JSI::SetModuleAPI(exports, "WGS84ToGCJ02", BaiduMapComponentModule::Location_WGS84ToGCJ02);
    JSI::SetModuleAPI(exports, "GCJ02ToBD09", BaiduMapComponentModule::Location_GCJ02ToBD09);
    JSI::SetModuleAPI(exports, "WGS84ToBD09", BaiduMapComponentModule::Location_WGS84ToBD09);
    JSI::SetModuleAPI(exports, "BD09ToGCJ02", BaiduMapComponentModule::Location_BD09ToGCJ02);

    /**
     * geocode interface
     */
    JSI::SetModuleAPI(exports, "geocode_search", BaiduMapComponentModule::Geocode_search);
    JSI::SetModuleAPI(exports, "regeocode_search", BaiduMapComponentModule::Regeocode_search);

    /**
     * destroy page
     */
    JSI::SetModuleAPI(exports, "destroy_page", BaiduMapComponentModule::Destroy_page);

    /**
     * BT connect interface
     */
    JSI::SetModuleAPI(exports, "onBTConnect", BaiduMapComponentModule::onBTConnect);
    /**
     * exit navigation
     */
    JSI::SetModuleAPI(exports, "naviDestroy", BaiduMapComponentModule::NaviDestroy);
    /**
     * stop anwser
     */
    JSI::SetModuleAPI(exports, "stopAnswer", BaiduMapComponentModule::StopAnswer);
	/**
     * mapInit
     */
    JSI::SetModuleAPI(exports, "mapInit", BaiduMapComponentModule::MapInit);
	/**
     * destroy
     */
    JSI::SetOnDestroy(exports, BaiduMapComponentModule::OnDestroy);
}

}  // namespace ACELite
}  // namespace OHOS

int32_t BaidumapSendMsgToJS(const char *msgBody, uint8_t type)
{
    BaiduMapAsyncMsgParams *params = new BaiduMapAsyncMsgParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params");
        return -1;
    }
    params->type = type;
    params->flag = true;
    int32_t copyRet = OHOS::ACELite::DmsLiteCopyStr(&params->msgBody, msgBody);

    if (copyRet != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy msgBody");
        delete params;
        params = nullptr;
        return copyRet;
    }

    bool ret =
        OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(OHOS::ACELite::ExecuteSendMsgToJS, static_cast<void *>(params));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to dispatch async work");
        AdapterFree(params->msgBody);
        params->msgBody = nullptr;
        delete params;
        params = nullptr;
        return -1;
    }

    return 0;
}

int32_t BaidumapFailSendMsgToJS(const char *msgBody, uint8_t type)
{
    BaiduMapAsyncMsgParams *params = new BaiduMapAsyncMsgParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params");
        return -1;
    }
    params->type = type;
    params->flag = false;
    int32_t copyRet = OHOS::ACELite::DmsLiteCopyStr(&params->msgBody, msgBody);

    if (copyRet != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to copy msgBody");
        delete params;
        params = nullptr;
        return copyRet;
    }

    bool ret =
        OHOS::ACELite::JsAsyncWork::DispatchAsyncWork(OHOS::ACELite::ExecuteSendMsgToJS, static_cast<void *>(params));
    if (!ret) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to dispatch async work");
        AdapterFree(params->msgBody);
        params->msgBody = nullptr;
        delete params;
        params = nullptr;
        return -1;
    }

    return 0;
}

__attribute__((weak)) void WGS84ToGCJ02(double lon, double lat, JSCoordinate &output)
{}

__attribute__((weak)) void GCJ02ToBD09(double lon, double lat, JSCoordinate &output)
{}

__attribute__((weak)) void WGS84ToBD09(double lon, double lat, JSCoordinate &output)
{}

__attribute__((weak)) void BD09ToGCJ02(double bd_lon, double bd_lat, JSCoordinate &output)
{}
