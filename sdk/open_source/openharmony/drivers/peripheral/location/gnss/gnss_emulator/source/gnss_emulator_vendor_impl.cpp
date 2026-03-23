/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <unordered_map>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/time.h>
#include "location_vendor_interface.h"
#include "location_vendor_lib.h"
#include "location_utils.h"
#include "gnss_utils.h"
#include "location_log.h"
#include "location_thread.h"
#include "securec.h"
#include "gnss_nmea_parse.h"
#include "pm_definition.h"
#include "pm.h"
#include "osal_task.h"

namespace OHOS {
namespace HDI {
namespace Location {
using namespace OHOS::HDI::Location::Gnss::V2_0;
namespace {

#define COLDSTART_FIXMODEGPS_NMEAALL_SAMPLE 0
#define AUTOSTART_FIXMODEALL_NMEADEF_SAMPLE 1

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif
#define MSG_CONTENT_IDX 2
#define MSG_TYPE_BINRY 0
#define MSG_TYPE_ASCII 1
#define LISTEN_DATA_MAX_LINES 1000
#define LISTEN_TIME_THR 1000
#define GNSS_TASK_CREATE_OK_WAIT_TIMEOUT          1000 // ms
#define GNSS_TASK_CREATE_TICK_TIME                50   // ms

std::string g_gnssNmeaFile = "/user/gnss_nmea.log";

typedef struct {
    bool gnssEnabled;
    bool gnssStarted;
    bool gnssRunning;
    GnssCallbackStruct *gnssCallback;
    GnssInfo gnssInfo;
    GnssLocation location;
    GnssSatelliteStatus satStatus;
    ThreadId listenGnssDataThreadId;
#ifdef SUPPORT_FILE
    std::ifstream gnssNmeaFp;
#endif
} GnssServiceContext;

static GnssServiceContext g_gnssServiceCtx = {
    .gnssEnabled = false,
    .gnssStarted = false,
    .gnssRunning = false,
    .gnssCallback = nullptr,
    .gnssInfo = {0},
    .location = {0},
    .satStatus = {0},
    .listenGnssDataThreadId = nullptr,
};

}  // namespace

static void ResetLocationSatelliteStatus(GnssServiceContext *gnssServiceCtx)
{
    int32_t ret = memset_s(&gnssServiceCtx->location, sizeof(GnssLocation), 0, sizeof(GnssLocation));
    if (ret != EOK) {
        LBSLOGW(HDI_GNSS, "memset_s GnssLocation failed:%d", ret);
    }
    ret = memset_s(&gnssServiceCtx->satStatus, sizeof(GnssSatelliteStatus), 0, sizeof(GnssSatelliteStatus));
    if (ret != EOK) {
        LBSLOGW(HDI_GNSS, "memset_s GnssSatelliteStatus failed:%d", ret);
    }
}

static void ResetGnssInfo(GnssServiceContext *gnssServiceCtx)
{
    int32_t ret = memset_s(&gnssServiceCtx->gnssInfo, sizeof(GnssInfo), 0, sizeof(GnssInfo));
    if (ret != EOK) {
        LBSLOGW(HDI_GNSS, "memset_s GnssInfo failed:%d", ret);
    }
}

#ifdef SUPPORT_FILE
static void ListenGnssData(void *argv)
{
    LBSLOGI(HDI_GNSS, "enter ListenGnssData");
    GnssServiceContext *gnssServiceCtx = (GnssServiceContext *)argv;
    if (gnssServiceCtx == nullptr || gnssServiceCtx->gnssCallback == nullptr || !gnssServiceCtx->gnssNmeaFp.good()) {
        LBSLOGE(HDI_GNSS, "ListenGnssData argv invalid");
        return;
    }
    GnssNmeaInit();
    gnssServiceCtx->gnssRunning = true;
    while (gnssServiceCtx->gnssStarted && g_gnssServiceCtx.gnssNmeaFp.good()) {
        std::string nmeaLine;
        while (getline(gnssServiceCtx->gnssNmeaFp, nmeaLine)) {
            if (nmeaLine.empty()) {
                continue;
            }
            const char *nmeaStr = nmeaLine.c_str();
            int32_t nmeaLen = nmeaLine.length();
            GnssNmeaFrameType nmeaType = GnssNmeaGetFrameType(nmeaStr, nmeaLen);
            LBSLOGD(HDI_GNSS, "nmeaType:%d nmeaStr:%s nmeaLen:%d", nmeaType, nmeaStr, nmeaLen);
            if (nmeaType >= GNSS_NMEA_FRAME_MAX) {
                LBSLOGW(HDI_GNSS, "invalid nmeaStr:%s", nmeaStr);
                continue;
            }
            if (nmeaType == GNSS_NMEA_FRAME_GGA) {
                ResetGnssInfo(gnssServiceCtx);
            }
            gnssServiceCtx->gnssCallback->gnssCallback.nmeaUpdate(LocationGnssUtilsGetTimestampMS(), nmeaStr, nmeaLen);
            int32_t ret =
                GnssNmeaDecodeFrame(nmeaStr, nmeaLine.length(), &gnssServiceCtx->gnssInfo);
            if (ret != GNSS_SUCCESS) {
                LBSLOGW(HDI_GNSS, "GnssNmeaDecodeFrame failed:%d", ret);
            }
            if (nmeaType == GNSS_NMEA_FRAME_TH002) {
                ResetLocationSatelliteStatus(gnssServiceCtx);

                ret = LocationGnssUtilsFillGnssLocation(&gnssServiceCtx->gnssInfo, &gnssServiceCtx->location);
                if (ret == GNSS_SUCCESS) {
                    gnssServiceCtx->gnssCallback->gnssCallback.locationUpdate(&gnssServiceCtx->location);
                }
                ret = LocationGnssUtilsFillAllSatelliteStatus(&gnssServiceCtx->gnssInfo, &gnssServiceCtx->satStatus);
                if (ret == GNSS_SUCCESS) {
                    gnssServiceCtx->gnssCallback->gnssCallback.satelliteStatusUpdate(&gnssServiceCtx->satStatus);
                }
                osDelay(LISTEN_TIME_THR);
            }
        }
        gnssServiceCtx->gnssNmeaFp.clear();
        gnssServiceCtx->gnssNmeaFp.seekg(0, std::ios::beg);
    }
    gnssServiceCtx->gnssRunning = false;
    LBSLOGI(HDI_GNSS, "exit ListenGnssData");
}
#else
constexpr uint32_t REPORT_MAX_CNT = 27;
constexpr uint32_t REPORT_MAX_BYTES = 512;
static char g_nmeaData[REPORT_MAX_CNT][REPORT_MAX_BYTES] = {
    "$GNGGA,021707.00,3045.804990,N,10353.991346,E,1,37,1.2,597.4,M,,M,,*6D",
    "$GNGLL,3045.804990,N,10353.991346,E,021707.00,A,A*7E",
    "$GNGSA,A,3,10,12,15,18,23,25,32,,,,,,2.4,1.2,2.1,1*3F",
    "$GNGSA,A,3,67,68,69,77,78,79,82,83,84,,,,2.4,1.2,2.1,2*38",
    "$GNGSA,A,3,3,5,15,24,25,34,36,,,,,,2.4,1.2,2.1,3*34",
    "$GNGSA,A,3,9,11,13,14,16,24,25,26,28,33,38,39,2.4,1.2,2.1,4*05",
    "$GNGSA,A,3,2,3,,,,,,,,,,,2.4,1.2,2.1,5*32",
    "$GPGSV,2,1,07,10,51,332,29,12,18,104,26,15,08,071,22,18,32,179,22,1*6A",
    "$GPGSV,2,2,07,23,68,068,15,25,17,145,35,32,41,285,44,1*5A",
    "$GLGSV,3,1,09,67,29,054,27,68,44,350,43,69,19,298,45,77,24,044,21,1*72",
    "$GLGSV,3,2,09,78,43,117,22,79,15,168,19,82,18,200,27,83,41,242,18,1*75",
    "$GLGSV,3,3,09,84,22,318,18,1*4E",
    "$GBGSV,6,1,22,01,34,119,,02,51,217,,03,55,166,,04,20,108,,1*71",
    "$GBGSV,6,2,22,05,31,245,,09,59,202,18,11,10,191,30,13,75,039,21,1*78",
    "$GBGSV,6,3,22,14,58,018,30,16,79,185,23,21,05,092,,24,50,130,27,1*7C",
    "$GBGSV,6,4,22,25,19,186,35,26,33,059,16,27,05,316,,28,24,276,28,1*7C",
    "$GBGSV,6,5,22,33,50,329,40,38,61,107,28,39,85,183,17,41,11,293,,1*7A",
    "$GBGSV,6,6,22,42,45,061,19,43,19,211,23,1*72",
    "$GQGSV,1,1,03,02,16,127,30,03,20,157,19,04,56,083,,1*52",
    "$GAGSV,2,1,08,03,53,293,29,05,50,030,31,08,10,259,,15,42,304,42,7*7B",
    "$GAGSV,2,2,08,24,18,089,24,25,15,141,36,34,59,213,27,36,18,167,18,7*7A",
    "$GNRMC,021707.00,A,3045.804990,N,10353.991346,E,000.9,159.3,071124,,E,A,V*70",
    "$GNVTG,159.3,T,159.3,M,000.9,N,001.7,K,A*32",
    "$GNZDA,021707.00,07,11,2024,00,00*78",
    "$PNTH000,6,,-91,0,,,,,1,1*11",
    "$PNTH001,1.226969,2.06,1.61,15.72,0,39,108*2E",
    "$PNTH002,021707.00,N,243.524682,3,200,0.166667,12,0.270406*44",
};

static void ListenGnssData(void *argv)
{
    LBSLOGI(HDI_GNSS, "enter ListenGnssData");
    GnssServiceContext *gnssServiceCtx = (GnssServiceContext *)argv;
    if (gnssServiceCtx == nullptr || gnssServiceCtx->gnssCallback == nullptr) {
        LBSLOGE(HDI_GNSS, "ListenGnssData argv invalid");
        return;
    }
    GnssNmeaInit();
    gnssServiceCtx->gnssRunning = true;
    while (gnssServiceCtx->gnssStarted) {
        for (uint32_t i = 0; i < REPORT_MAX_CNT; i++) {
            const char *nmeaStr = g_nmeaData[i];
            int32_t nmeaLen = strlen(g_nmeaData[i]);
            GnssNmeaFrameType nmeaType = GnssNmeaGetFrameType(nmeaStr, nmeaLen);
            LBSLOGD(HDI_GNSS, "nmeaType:%d nmeaStr:%s nmeaLen:%d", nmeaType, nmeaStr, nmeaLen);
            if (nmeaType >= GNSS_NMEA_FRAME_MAX) {
                LBSLOGW(HDI_GNSS, "invalid nmeaStr:%s", nmeaStr);
                continue;
            }
            if (nmeaType == GNSS_NMEA_FRAME_GGA) {
                ResetGnssInfo(gnssServiceCtx);
            }
            gnssServiceCtx->gnssCallback->gnssCallback.nmeaUpdate(LocationGnssUtilsGetTimestampMS(), nmeaStr, nmeaLen);
            int32_t ret =
                GnssNmeaDecodeFrame(nmeaStr, nmeaLen, &gnssServiceCtx->gnssInfo);
            if (ret != GNSS_SUCCESS) {
                LBSLOGW(HDI_GNSS, "GnssNmeaDecodeFrame failed:%d", ret);
            }
            if (nmeaType == GNSS_NMEA_FRAME_TH002) {
                ResetLocationSatelliteStatus(gnssServiceCtx);

                ret = LocationGnssUtilsFillGnssLocation(&gnssServiceCtx->gnssInfo, &gnssServiceCtx->location);
                if (ret == GNSS_SUCCESS) {
                    gnssServiceCtx->gnssCallback->gnssCallback.locationUpdate(&gnssServiceCtx->location);
                }
                ret = LocationGnssUtilsFillAllSatelliteStatus(&gnssServiceCtx->gnssInfo, &gnssServiceCtx->satStatus);
                if (ret == GNSS_SUCCESS) {
                    gnssServiceCtx->gnssCallback->gnssCallback.satelliteStatusUpdate(&gnssServiceCtx->satStatus);
                }
                osDelay(LISTEN_TIME_THR);
            }
        }
    }
    gnssServiceCtx->gnssRunning = false;
    LBSLOGI(HDI_GNSS, "exit ListenGnssData");
}
#endif

static int LocationStopGnss(uint32_t category)
{
    if (!g_gnssServiceCtx.gnssStarted) {
        LBSLOGI(HDI_GNSS, "tiot service has not Started");
        return GNSS_FAILURE;
    }
    g_gnssServiceCtx.gnssStarted = false;
#ifdef SUPPORT_FILE
    if (g_gnssServiceCtx.gnssNmeaFp) {
        g_gnssServiceCtx.gnssNmeaFp.close();
    }
#endif
    uint16_t taskReadyWait = 0;
    while ((taskReadyWait < GNSS_TASK_CREATE_OK_WAIT_TIMEOUT) && g_gnssServiceCtx.gnssRunning) {
        osal_msleep(GNSS_TASK_CREATE_TICK_TIME);
        taskReadyWait += GNSS_TASK_CREATE_TICK_TIME;
    }
    g_gnssServiceCtx.listenGnssDataThreadId = nullptr;
    return GNSS_SUCCESS;
}

static int32_t LocationDisableGnss(void)
{
    if (!g_gnssServiceCtx.gnssEnabled) {
        LBSLOGE(HDI_GNSS, "gnss not enable");
        return GNSS_FAILURE;
    }
    if (g_gnssServiceCtx.gnssStarted) {
        LBSLOGI(HDI_GNSS, "Gnss is Started");
        LocationStopGnss(0);
    }
    uapi_pm_remove_sleep_veto(PM_ID_SYS);
    LBSLOGI(HDI_GNSS, "finish Disable gnss");
    g_gnssServiceCtx.gnssCallback = nullptr;
    g_gnssServiceCtx.gnssEnabled = false;
    return GNSS_SUCCESS;
}

static int32_t LocationEnableGnss(GnssCallbackStruct *callbacks)
{
    if (g_gnssServiceCtx.gnssEnabled) {
        LBSLOGI(HDI_GNSS, "gnss enabled");
        return GNSS_SUCCESS;
    }
    uapi_pm_add_sleep_veto(PM_ID_SYS);
    g_gnssServiceCtx.gnssCallback = callbacks;
    g_gnssServiceCtx.gnssEnabled = true;
    LBSLOGI(HDI_GNSS, "enable gnss success");
    return GNSS_SUCCESS;
}

static int LocationStartGnss(uint32_t category)
{
    LBSLOGI(HDI_GNSS, "enter Start Gnss");

    if (!g_gnssServiceCtx.gnssEnabled) {
        LBSLOGI(HDI_GNSS, "Gnss not enable");
        return GNSS_FAILURE;
    }
    if (g_gnssServiceCtx.gnssStarted) {
        LBSLOGI(HDI_GNSS, "Gnss has Started");
        return GNSS_SUCCESS;
    }
#ifdef SUPPORT_FILE
    g_gnssServiceCtx.gnssNmeaFp.open(g_gnssNmeaFile, std::ifstream::in);

    if (!g_gnssServiceCtx.gnssNmeaFp.good() ||
        g_gnssServiceCtx.gnssNmeaFp.peek() == std::ifstream::traits_type::eof()) {
        g_gnssServiceCtx.gnssNmeaFp.close();
        LBSLOGI(HDI_GNSS, "Gnss open :%s failed", g_gnssNmeaFile.c_str());
        return GNSS_FAILURE;
    }
#endif
    g_gnssServiceCtx.gnssStarted = true;
    ThreadAttr attr = {"ListenGnssDataTask", 0x2000, (uint8_t)osPriorityNormal, 0, 0};
    ThreadId threadId = (ThreadId)ThreadCreate(ListenGnssData, &g_gnssServiceCtx, &attr);
    if (threadId == nullptr) {
        return GNSS_FAILURE;
    }
    g_gnssServiceCtx.listenGnssDataThreadId = threadId;
    LBSLOGI(HDI_GNSS, "Start Gnss success");
    return GNSS_SUCCESS;
}

static int LocationInjectsGnssReferenceInfo(int category, GnssReferenceInfo *info)
{
    return 0;
}

static int LocationSetGnssConfigPara(GnssConfigParameter *para)
{
    return 0;
}

static void LocationRemoveAuxiliaryData(uint16_t flags)
{}

static int LocationInjectExtendedEphemeris(char *data, int length)
{
    return 0;
}

static int LocationGetCachedLocationsSize()
{
    return 0;
}

static void LocationFlushCachedGnssLocations()
{}

static void *LocationGetGnssModuleIface(int iface)
{
    return nullptr;
}

static GnssVendorInterface g_gnssEmulatorVendorInterface = {
    .size = 1,
    .enableGnss = LocationEnableGnss,
    .disableGnss = LocationDisableGnss,
    .startGnss = LocationStartGnss,
    .stopGnss = LocationStopGnss,
    .injectsGnssReferenceInfo = LocationInjectsGnssReferenceInfo,
    .setGnssConfigPara = LocationSetGnssConfigPara,
    .removeAuxiliaryData = LocationRemoveAuxiliaryData,
    .injectExtendedEphemeris = LocationInjectExtendedEphemeris,
    .getCachedLocationsSize = LocationGetCachedLocationsSize,
    .flushCachedGnssLocations = LocationFlushCachedGnssLocations,
    .getGnssModuleIface = LocationGetGnssModuleIface,
};

static GnssVendorInterface *GetGnssInterface()
{
    return &g_gnssEmulatorVendorInterface;
}

static GnssVendorDevice g_gnssEmulatorVendorDevice = {
    .size = 1,
    .getGnssInterface = GetGnssInterface,
};

GnssVendorDevice *LocationVendorGetGnssVendorDevice()
{
    return &g_gnssEmulatorVendorDevice;
}

}  // namespace Location
}  // namespace HDI
}  // namespace OHOS
