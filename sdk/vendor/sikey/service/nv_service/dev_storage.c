#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "sys/time.h"
#include "sk_board.h"
#include "dev_storage.h"
#include "time.h"
#include "common.h"
#include "key_id.h"
#include "stdbool.h"

#define WATCH_STORAGE_DBG(fmt, args...) printf("[%s] " fmt, __FUNCTION__, ##args)

static bool                         nv_mutex_init_flag = false;
static osMutexId_t                  sk_nv_mutex = NULL;
static nv_sk_info_t                 s_watch_nv = {0};
nv_sensor_cali_t                    sensor_cali_data = {0};
model_version_t                     gyro_cali_data = {0};

static void nv_write(UI_NV_SECTION_T sectionId)
{
    uint32_t length = 0;
    void * data = NULL;
    switch (sectionId) {
    case NV_SECTION_SK_DEVICE_INFO:
        length = sizeof(nv_sk_device_info);
        data = &s_watch_nv.dInfo;
        break;
    case NV_SECTION_SENSOR_CALI_INFO:
        length = sizeof(nv_sensor_cali_t);
        data = &sensor_cali_data;
        break;
    case NV_SECTION_GYRO_CALI_INFO:
        length = sizeof(model_version_t);
        data = &gyro_cali_data;
        break;

    default:
        return;
    }

    sk_nv_write(sectionId, length, (uint8_t *)data);
}

static void nv_read(UI_NV_SECTION_T sectionId)
{
    uint32_t length = 0;
    void * data = NULL;

    switch (sectionId) {
    case NV_SECTION_SK_DEVICE_INFO:
        length = sizeof(nv_sk_device_info);
        data = &s_watch_nv.dInfo;
        break;
    case NV_SECTION_SENSOR_CALI_INFO:
        length = sizeof(nv_sensor_cali_t);
        data = &sensor_cali_data;
        break;
    case NV_SECTION_GYRO_CALI_INFO:
        length = sizeof(model_version_t);
        data = &gyro_cali_data;
        break;

    default:
        return;
    }
    sk_nv_read(sectionId, length, (uint8_t *)data);
}

static void sk_nv_mutex_create(void)
{
    if (nv_mutex_init_flag) return;

    sk_nv_mutex = osMutexNew(NULL);
    if (sk_nv_mutex == NULL) {
        WATCH_STORAGE_DBG("create mutex fail\n");
        return;
    }
    nv_mutex_init_flag = true;
    WATCH_STORAGE_DBG("create mutex success\n");
}

static void sk_nv_lock(void)
{
    osMutexAcquire(sk_nv_mutex, osWaitForever);
    WATCH_STORAGE_DBG("nv mutex lock\n");
}

static void sk_nv_unlock(void)
{
    osMutexRelease(sk_nv_mutex);
    WATCH_STORAGE_DBG("nv mutex unlock\n");
}

int32_t watch_storage_set(WATCH_STORAGE_T type, void * data, uint32_t len)
{
    int32_t result = 0;
    bool change_storage = false;

    if (type > ENUM_STORAGE_NV_SPLIT) {
        sk_nv_lock();
        switch (type) {
        case STORAGE_FAC_FLAG:
            s_watch_nv.dInfo.fac_flag = *(uint8_t *)data;
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_MMI_FLAG:
            s_watch_nv.dInfo.mmi_flag = *(uint8_t *)data;
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_HARD_CODE:
            memset(s_watch_nv.dInfo.hc, 0, sizeof(s_watch_nv.dInfo.hc));
            strncpy(s_watch_nv.dInfo.hc, (char *)data, sizeof(s_watch_nv.dInfo.hc));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_BSN:
            memset(s_watch_nv.dInfo.bsn, 0, sizeof(s_watch_nv.dInfo.bsn));
            strncpy(s_watch_nv.dInfo.bsn, (char *)data, sizeof(s_watch_nv.dInfo.bsn));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_PSN:
            memset(s_watch_nv.dInfo.psn, 0, sizeof(s_watch_nv.dInfo.psn));
            strncpy(s_watch_nv.dInfo.psn, (char *)data, sizeof(s_watch_nv.dInfo.psn));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_IMEI:
            memset(s_watch_nv.dInfo.imei, 0, sizeof(s_watch_nv.dInfo.imei));
            strncpy(s_watch_nv.dInfo.imei, (char *)data, sizeof(s_watch_nv.dInfo.imei));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_DATA_CODE:
            memset(s_watch_nv.dInfo.six_axes, 0, sizeof(s_watch_nv.dInfo.six_axes));
            strncpy(s_watch_nv.dInfo.six_axes, (char *)data, sizeof(s_watch_nv.dInfo.six_axes));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_SCRAMBLING_CODE:
            memset(s_watch_nv.dInfo.scrambling_code, 0, sizeof(s_watch_nv.dInfo.scrambling_code));
            strncpy(s_watch_nv.dInfo.scrambling_code, (char *)data, sizeof(s_watch_nv.dInfo.scrambling_code));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        case STORAGE_SENSOR_DATA:
            sensor_cali_data =*(nv_sensor_cali_t *)data;
            nv_write(NV_SECTION_SENSOR_CALI_INFO);
            break;
        case STORAGE_GYRO_DATA:
            gyro_cali_data =*(model_version_t *)data;
            nv_write(NV_SECTION_GYRO_CALI_INFO);
            break;
        case STORAGE_TM_HOUR:
            memset(s_watch_nv.dInfo.tm_hour, 0, sizeof(s_watch_nv.dInfo.tm_hour));
            strncpy(s_watch_nv.dInfo.tm_hour, (char *)data, sizeof(s_watch_nv.dInfo.tm_hour));
            nv_write(NV_SECTION_SK_DEVICE_INFO);
            break;
        default:
            break;
        }
        sk_nv_unlock();
    }
    uapi_nv_flush();
    return result;
}

int32_t watch_storage_get(WATCH_STORAGE_T type, void * data, uint32_t len)
{
    int32_t result = 0;
    if (NULL == data) {
        return -1;
    }

    if (type > ENUM_STORAGE_NV_SPLIT) {
        sk_nv_lock();
        switch (type) {
        case STORAGE_FAC_FLAG:
            *(uint8_t *)data = s_watch_nv.dInfo.fac_flag;
            break;
        case STORAGE_MMI_FLAG:
            *(uint8_t *)data = s_watch_nv.dInfo.mmi_flag;
            break;
        case STORAGE_HARD_CODE:
            memcpy(data, s_watch_nv.dInfo.hc, (len >= sizeof(s_watch_nv.dInfo.hc)? sizeof(s_watch_nv.dInfo.hc) : len));
            break;
        case STORAGE_BSN:
            memcpy(data, s_watch_nv.dInfo.bsn, (len >= sizeof(s_watch_nv.dInfo.bsn)? sizeof(s_watch_nv.dInfo.bsn) : len));
            break;
        case STORAGE_PSN:
            memcpy(data, s_watch_nv.dInfo.psn, (len >= sizeof(s_watch_nv.dInfo.psn)? sizeof(s_watch_nv.dInfo.psn) : len));
            break;
        case STORAGE_IMEI:
            memcpy(data, s_watch_nv.dInfo.imei, (len >= sizeof(s_watch_nv.dInfo.imei)? sizeof(s_watch_nv.dInfo.imei) : len));
            break;
        case STORAGE_DATA_CODE:
            memcpy(data, s_watch_nv.dInfo.six_axes, (len >= sizeof(s_watch_nv.dInfo.six_axes)? sizeof(s_watch_nv.dInfo.six_axes) : len));
            break;
        case STORAGE_SCRAMBLING_CODE:
            memcpy(data, s_watch_nv.dInfo.scrambling_code, (len >= sizeof(s_watch_nv.dInfo.scrambling_code)? sizeof(s_watch_nv.dInfo.scrambling_code) : len));
            break;
        case STORAGE_SENSOR_DATA:
            *(nv_sensor_cali_t *)data = sensor_cali_data;
            break;
        case STORAGE_GYRO_DATA:
            *(model_version_t *)data = gyro_cali_data;
            break;
        case STORAGE_TM_HOUR:
            memcpy(data, s_watch_nv.dInfo.tm_hour, (len >= sizeof(s_watch_nv.dInfo.tm_hour)? sizeof(s_watch_nv.dInfo.tm_hour) : len));
            break;
        default:
            break;
        }
        sk_nv_unlock();
    }

    return result;
}

void sk_nv_init(void)
{
    memset(&s_watch_nv, 0, sizeof(s_watch_nv));
    sk_nv_mutex_create();
    sk_nv_lock();
    nv_read(NV_SECTION_SK_DEVICE_INFO);
    nv_read(NV_SECTION_SENSOR_CALI_INFO);
    nv_read(NV_SECTION_GYRO_CALI_INFO);
    sk_nv_unlock();
}

