/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: baidumap sdk wrapper
 * Author: SoftwarePlatform Group
 * Create: 2024-03-02
 */

#ifndef BAIDUMAPCOMPONENT_MODULE_H
#define BAIDUMAPCOMPONENT_MODULE_H

#include <stdint.h>
#include "jsi.h"

namespace OHOS {
namespace ACELite {
class BaiduMapComponentModule final : public MemoryHeap {
public:
    /**
     * constructor
     */
    BaiduMapComponentModule() = default;

    /**
     * desconstructor
     */
    ~BaiduMapComponentModule() = default;

    /**
     * MsgCenter interface
     */
    static JSIValue poi_nearby_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue route_riding_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue route_walking_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue route_transit_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue auth_license(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue getGeolocation(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue naviInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRoutePlanStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRoutePlanSuccess(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRoutePlanFail(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onNaviStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onNaviDestroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRoadGuideTextUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRouteGuideIconUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRemainTimeUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRemainDistanceUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRemainRouteUpdate(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onGpsStatusChange(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRouteFarAway(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onRoutePlanYawing(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onReRouteComplete(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onArriveDest(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue poi_sug_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * location interface
     */
    static JSIValue Location_WGS84ToGCJ02(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Location_GCJ02ToBD09(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Location_WGS84ToBD09(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Location_BD09ToGCJ02(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * geocode interface
     */
    static JSIValue Geocode_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Regeocode_search(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * destroy page interface
     */
    static JSIValue Destroy_page(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * bt connect interface
     */
    static JSIValue onBTConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * exit navigation
     */
    static JSIValue NaviDestroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * stop answer
     */
    static JSIValue StopAnswer(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
	
	/**
     * mapInit
     */
    static JSIValue MapInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
	
	/**
     * destroy
     */
    static void OnDestroy();
};

void InitBaiduMapComponentModule(JSIValue exports);
}  // namespace ACELite
}  // namespace OHOS
int32_t BaidumapSendMsgToJS(const char *msgBody, uint8_t type);
int32_t BaidumapFailSendMsgToJS(const char *msgBody, uint8_t type);
#endif

