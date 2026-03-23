#ifndef WATCH_STORAGE_H
#define WATCH_STORAGE_H
#include "stdlib.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_semaphore.h"
#include "common.h"
#include "key_id.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    STORAGE_SYSTEM_VERSION,
    STORAGE_DEVICE_MODEL,
    STORAGE_COMMIT_ID,
    STORAGE_SERVER_MODE,
    STORAGE_SIMPLE,
    STORAGE_LOC_TIMER,
    STORAGE_LANGUAGE,
    STORAGE_GROUP_INFO,
    STORAGE_DISTURB_MODE,
    STORAGE_VIDEO_CALL_LICENSE,
    STORAGE_ALARM_CLOCK,
    STORAGE_PHONEBOOK,
    STORAGE_LATEST_FW_INFO_3321,
    STORAGE_LATEST_FW_INFO_5612,
    STORAGE_CLOUDDRIVE,
    STORAGE_SOS_CONTACTS,
    STORAGE_OTA_UPGRADE_STATE,
    STORAGE_NEMA_LOG,
    STORAGE_LOCAL_LOG,
    STORAGE_MENU_SHOW,
    ENUM_STORAGE_NV_SPLIT, // For split storage and nv

    // NV begin
    STORAGE_FAC_FLAG,
    STORAGE_MMI_FLAG,
    STORAGE_HARD_CODE,
    STORAGE_BSN,
    STORAGE_PSN,
    STORAGE_IMEI,
    STORAGE_DATA_CODE,
    STORAGE_SCRAMBLING_CODE,
    STORAGE_SOFTWARE_VERSION,
    STORAGE_SENSOR_DATA,
    STORAGE_GYRO_DATA,
    STORAGE_TM_HOUR,

    STORAGE_BIND,
    STORAGE_AUTO_REJECT, // false表示自动拒绝，默认自动拒接陌生人
    STORAGE_CID,
    STORAGE_TICKET,
    STORAGE_TIMEZONE,
    STORAGE_CHILD_INFO,
    STORAGE_SPORT_RECORD,
    STORAGE_SPORT_ONEDAY,
    STORAGE_ACTIVITY,

    STORAGE_TOKEN,
    // NV end

    STORAGE_RAT,
    STORAGE_STEP_COUNT,
    STORAGE_GSM_CELLINFO,
    STORAGE_LTE_CELLINFO,
    STORAGE_WIFI,
    STORAGE_WEATHER,
    STORAGE_GPS_LOC,
    STORAGE_CELLID,
    STORAGE_CORR_UVI_CF,
    STORAGE_GPS_EPO,
    STORAGE_WIFI_FLAG,
    STORAGE_GPS_FLAG,
    STORAGE_GPS_TTFF,
    STORAGE_STEP_TIMESTAMP,
    STORAGE_UVI_TIMESTAMP,
    STORAGE_OUTDOORS_TIME,
    STORAGE_LOC_TIMESTAMP,
    STORAGE_OTA_INSTALLED,
    STORAGE_GPS_MANUFACTURER,
    STORAGE_MAX,
} WATCH_STORAGE_T;

#define WATCH_PSN_LEN                           NV_SK_PSN_LEN
#define WATCH_BSN_LEN                           NV_SK_BSN_LEN
#define WATCH_IMEI_LEN                          NV_SK_IMEI_LEN
#define WATCH_HC_LEN                            NV_SK_HC_LEN
#define WATCH_AES_LEN                           NV_SK_AES_LEN

#ifdef __cplusplus
}
#endif

void sk_nv_init(void);
int32_t watch_storage_set(WATCH_STORAGE_T type, void * data, uint32_t len);
int32_t watch_storage_get(WATCH_STORAGE_T type, void * data, uint32_t len);

#endif
