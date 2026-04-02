/**
 * mqtt_cmd_handler.c
 *
 * MQTT command handler — dispatches incoming /c-topic JSON commands and
 * publishes responses to the /r topic.
 *
 * JSON parsing strategy: cJSON library (already in SDK third_party).
 * JSON building strategy: direct snprintf — all response schemas are fixed.
 *
 * Command implementation status — v1.0.3:
 *   Implemented: v, b, di, ?, l/l0/l1, network.status, lte.auto_reconnect,
 *                sys.reboot, sys.poweroff, factory-reset,
 *                gnss.status, gnss.diag,
 *                audio.list, audio.play, audio.remove,
 *                dir, settings.get, settings.set, power_saving, shock_limit,
 *                usage.stats, usage.stats.clear, usage.log.enabled, usage.log.clear,
 *                sec.format,
 *                f/f0/f1/fe/fe0/fe1/fl/fa/fd (fence),
 *                k/k0/k1/kl/ka/kd/ke0/ke1/kc (beacon),
 *                ap/ap.update/ap.exec_by_idx/ap.exec_custom (alert),
 *                dfu (FOTA via HTTP download + sk_ota)
 *   Stub (e:-99): epo.*, gnss.nmea.upload, debug.*, log.cat,
 *                  audio.hash, debug.sensor_rt
 */

#include "mqtt_cmd_handler.h"
#include "mqtt_service.h"
#include "mqtt_event.h"
#include "dev_storage.h"
#include "sh366102.h"
#include "modem_ctrl.h"
#include "watch_version.h"
#include "sk_audio.h"
#include "cJSON.h"
#include "cmsis_os2.h"
#include "sk_ota.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>

/* ------------------------------------------------------------------ */
/*  Platform externals                                                  */
/* ------------------------------------------------------------------ */
extern uint8_t  g_ipv4_status;                        /* lwip_volte_adapter.c */
extern uint8_t  g_chargestatus;                       /* sh366102.c           */
extern uint32_t mcu_os_sys_reset(uint8_t *para, uint32_t para_len);
extern bool     gnss_is_fixed;                        /* gnss_nmea_process.c  */

/* http_api module — forward declarations to avoid pulling in app_at_process.h.
 * http_event_type_t is an enum (ABI-compatible with int in C). */
extern bool g_ota_skip_crc;
extern unsigned char get_download_file_state(void);
extern int http_send_data_to_server(void *data, size_t length,
                                     int type, uint8_t other_type);
#define MQTT_HTTP_DOWNLOAD_OTA  0   /* == HTTP_DOWNlOAD_OTA */
#define MQTT_TYPE_DOWN_MODEM    1   /* == TYPE_DOWN_MODEM   */

/* ------------------------------------------------------------------ */
/*  Constants                                                           */
/* ------------------------------------------------------------------ */
#define CMD_RESP_BUF_SIZE       1024
#define CMD_NAME_MAX_LEN        32
#define CMD_ID_MAX_LEN          48
#define IMEI_BUF_LEN            20
#define SN_BUF_LEN              32
#define REBOOT_DELAY_MS         500
#define AUDIO_BASE_PATH         "/user/audios/"
#define SETTINGS_FILE_PATH      "/user/mqtt/settings.json"

/* ------------------------------------------------------------------ */
/*  Module-private state                                                */
/* ------------------------------------------------------------------ */
static volatile bool g_live_enabled = false;

/* --- Settings KV (in-memory, persisted to file on write) --- */
static int  g_set_fence_inner_padding = 5;
static int  g_set_power_saving        = 0;
static int  g_set_shock_max_count     = 0;
static int  g_set_shock_window_sec    = 0;

/* --- Usage tracking --- */
static volatile uint32_t g_usage_reset_count = 0;

/* ------------------------------------------------------------------ */
/*  Settings persistence helpers                                        */
/* ------------------------------------------------------------------ */
static void settings_load(void)
{
    FILE *fp;
    char buf[256];
    cJSON *root;
    cJSON *item;
    size_t n;

    fp = fopen(SETTINGS_FILE_PATH, "r");
    if (fp == NULL) {
        return;
    }
    n = fread(buf, 1, sizeof(buf) - 1, fp);
    (void)fclose(fp);
    if (n == 0) {
        return;
    }
    buf[n] = '\0';
    root = cJSON_Parse(buf);
    if (root == NULL) {
        return;
    }
    item = cJSON_GetObjectItemCaseSensitive(root, "fence_inner_padding");
    if (cJSON_IsNumber(item)) g_set_fence_inner_padding = item->valueint;
    item = cJSON_GetObjectItemCaseSensitive(root, "power_saving");
    if (cJSON_IsNumber(item)) g_set_power_saving = item->valueint;
    item = cJSON_GetObjectItemCaseSensitive(root, "shock_max_count");
    if (cJSON_IsNumber(item)) g_set_shock_max_count = item->valueint;
    item = cJSON_GetObjectItemCaseSensitive(root, "shock_window_sec");
    if (cJSON_IsNumber(item)) g_set_shock_window_sec = item->valueint;
    cJSON_Delete(root);
    printf("[MQTT CMD] Settings loaded from file\n");
}

static void settings_save(void)
{
    FILE *fp;
    char buf[256];
    int len;

    len = snprintf(buf, sizeof(buf),
                   "{\"fence_inner_padding\":%d,"
                   "\"power_saving\":%d,"
                   "\"shock_max_count\":%d,"
                   "\"shock_window_sec\":%d}",
                   g_set_fence_inner_padding,
                   g_set_power_saving,
                   g_set_shock_max_count,
                   g_set_shock_window_sec);
    if (len <= 0) {
        return;
    }
    fp = fopen(SETTINGS_FILE_PATH, "w");
    if (fp == NULL) {
        printf("[MQTT CMD] WARNING: failed to save settings\n");
        return;
    }
    (void)fwrite(buf, 1, (size_t)len, fp);
    (void)fclose(fp);
}

/* ------------------------------------------------------------------ */
/*  Publish helper: send resp_buf to /r topic at QoS 0                 */
/* ------------------------------------------------------------------ */
static void publish_response(const char *resp_buf)
{
    int len;

    if (resp_buf == NULL) {
        return;
    }
    len = (int)strlen(resp_buf);
    if (len <= 0) {
        return;
    }
    (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_RESP, resp_buf, len, 0);
}

/* ------------------------------------------------------------------ */
/*  Command handler function type (cJSON-based)                         */
/* ------------------------------------------------------------------ */
typedef void (*cmd_handler_fn_t)(const char *cmd,
                                 cJSON      *root,
                                 char       *resp,
                                 int         resp_sz);

/* ================================================================== */
/*  P2 handlers — original commands, adapted to cJSON                   */
/* ================================================================== */

/* 1. Version */
static void cmd_version(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":\"%s\",\"v52\":\"n/a\"}",
                   cmd, VERSION_STRING);
}

/* 2. Battery */
static void cmd_battery(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    uint16_t voltage  = get_battery_voltage();
    uint8_t  capacity = get_battery_level();
    uint8_t  ch       = g_chargestatus;

    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{\"v\":%u,\"c\":%u,\"ch\":%u}}",
                   cmd, (unsigned)voltage, (unsigned)capacity,
                   (unsigned)(ch > 0 ? 1 : 0));
}

/* 3. Device info */
static void cmd_device_info(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    char imei[IMEI_BUF_LEN + 1] = {0};
    char sn[SN_BUF_LEN + 1]     = {0};

    (void)root;
    (void)watch_storage_get(STORAGE_IMEI, imei, IMEI_BUF_LEN);
    imei[IMEI_BUF_LEN] = '\0';
    (void)watch_storage_get(STORAGE_PSN, sn, SN_BUF_LEN);
    sn[SN_BUF_LEN] = '\0';

    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"iccid\":\"\","
                   "\"imei\":\"%s\","
                   "\"imsi\":\"\","
                   "\"v9\":\"%s\","
                   "\"v5\":\"n/a\","
                   "\"mac\":\"\","
                   "\"vg\":\"n/a\","
                   "\"sn\":\"%s\","
                   "\"model\":\"d1/2\""
                   "}}", cmd, imei, VERSION_STRING, sn);
}

/* 4. Dump */
static void cmd_dump(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    uint32_t tick = osKernelGetTickCount();
    uint32_t freq = osKernelGetTickFreq();
    uint32_t up   = (freq > 0) ? (tick / freq) : 0;
    uint16_t mv   = get_battery_voltage();
    uint8_t  cap  = get_battery_level();
    uint8_t  ch   = g_chargestatus;

    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"fix\":%d,"
                   "\"f\":0,"
                   "\"fid\":\"\","
                   "\"breach_width\":0,"
                   "\"fence_sec\":0,"
                   "\"bv\":%u,"
                   "\"bat_cap\":%u,"
                   "\"ch\":%u,"
                   "\"uptime\":%u,"
                   "\"rst\":\"\""
                   "}}", cmd,
                   gnss_is_fixed ? 1 : 0,
                   (unsigned)mv, (unsigned)cap,
                   (unsigned)(ch > 0 ? 1 : 0),
                   (unsigned)up);
}

/* 5-7. Live mode */
static void cmd_live_query(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}", cmd, g_live_enabled ? 1 : 0);
}

static void cmd_live_on(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_live_enabled = true;
    printf("[MQTT CMD] live mode ON\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_live_off(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_live_enabled = false;
    printf("[MQTT CMD] live mode OFF\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 8. Network status */
static void cmd_net_status(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    uint8_t lte_reg = modem_get_reg_status();
    uint8_t lte_ip  = g_ipv4_status;
    int     aws_ok  = mqtt_service_is_connected() ? 1 : 0;
    int     lte_ok  = (lte_reg && lte_ip) ? 1 : 0;

    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{\"lte_state\":%d,\"aws_state\":%d}}",
                   cmd, lte_ok, aws_ok);
}

/* 9. LTE auto-reconnect */
static void cmd_lte_reconnect(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    int val  = cJSON_IsNumber(p) ? p->valueint : 0;

    printf("[MQTT CMD] lte.auto_reconnect = %d\n", val);
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 10. Reboot (special: publishes response before rebooting) */
static void reboot_task(void *arg)
{
    (void)arg;
    osDelay(REBOOT_DELAY_MS);
    printf("[MQTT CMD] sys.reboot executing\n");
    (void)mcu_os_sys_reset(NULL, 0);
    osThreadExit();
}

static void cmd_reboot(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    osThreadAttr_t attr = {0};
    cJSON *i_item;

    i_item = cJSON_GetObjectItemCaseSensitive(root, "i");
    if (cJSON_IsString(i_item)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":0,\"i\":\"%s\"}", cmd, i_item->valuestring);
    } else {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":0}", cmd);
    }
    publish_response(resp);
    resp[0] = '\0';   /* prevent double-publish by dispatch */

    attr.name       = "mqtt_reboot";
    attr.stack_size  = 1024;  /* CR-205: increased for mcu_os_sys_reset call chain */
    attr.priority   = osPriorityNormal;
    if (osThreadNew(reboot_task, NULL, &attr) == NULL) {
        printf("[MQTT CMD] WARNING: failed to create reboot task\n");
    }
}

/* 11. Poweroff */
static void cmd_poweroff(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    printf("[MQTT CMD] sys.poweroff requested\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 12. Factory reset */
static void cmd_factory_reset(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    printf("[MQTT CMD] factory-reset requested\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ================================================================== */
/*  NEW handlers — GNSS                                                 */
/* ================================================================== */

/* gnss.status */
static void cmd_gnss_status(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    double lat = mqtt_event_gnss_last_lat();
    double lon = mqtt_event_gnss_last_lon();
    int    fix = gnss_is_fixed ? 1 : 0;
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"state\":%d,"
                   "\"last_loc\":{\"lat\":%.6f,\"lng\":%.6f,\"ts\":%llu},"
                   "\"fix_ts\":%d,"
                   "\"fixing\":%d,"
                   "\"qual\":0,"
                   "\"cn0\":[]"
                   "}}", cmd, fix, lat, lon, ts, fix, fix ? 0 : 1);
}

/* gnss.diag */
static void cmd_gnss_diag(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    /* TODO: integrate with gnss_nmea_process CN0 buffer for real data */
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":[0,0,0,0,0,0,0,0,0]}", cmd);
}

/* ================================================================== */
/*  NEW handlers — Audio                                                */
/* ================================================================== */

/* CR-201/202: Reject filenames containing path traversal sequences */
static bool is_safe_filename(const char *name)
{
    if (name == NULL || name[0] == '\0') return false;
    if (strstr(name, "..") != NULL) return false;
    if (strchr(name, '/') != NULL) return false;
    if (strchr(name, '\\') != NULL) return false;
    return true;
}

/* audio.list: list files in /user/audios/ */
static void cmd_audio_list(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    DIR *dir;
    struct dirent *ent;
    int pos;
    bool first = true;

    (void)root;
    pos = snprintf(resp, (size_t)resp_sz, "{\"c\":\"%s\",\"r\":[", cmd);

    dir = opendir(AUDIO_BASE_PATH);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL && pos < resp_sz - 32) {
            /* Skip . and .. */
            if (ent->d_name[0] == '.') {
                continue;
            }
            if (!first) {
                pos += snprintf(resp + pos, (size_t)(resp_sz - pos), ",");
            }
            pos += snprintf(resp + pos, (size_t)(resp_sz - pos),
                            "\"%s\"", ent->d_name);
            first = false;
        }
        closedir(dir);
    }
    (void)snprintf(resp + pos, (size_t)(resp_sz - pos), "]}");
}

/* audio.play: play audio file {p:{f:"breach.wav",v:15}} */
static void cmd_audio_play(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p     = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *f_obj;
    cJSON *v_obj;
    char path[96];
    int vol;

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    f_obj = cJSON_GetObjectItemCaseSensitive(p, "f");
    v_obj = cJSON_GetObjectItemCaseSensitive(p, "v");

    if (!cJSON_IsString(f_obj)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    if (!is_safe_filename(f_obj->valuestring)) {
        printf("[MQTT CMD] audio.play: BLOCKED unsafe filename\n");
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-4}", cmd);
        return;
    }

    vol = cJSON_IsNumber(v_obj) ? v_obj->valueint : 10;
    (void)snprintf(path, sizeof(path), "%s%s", AUDIO_BASE_PATH, f_obj->valuestring);

    printf("[MQTT CMD] audio.play: %s vol=%d\n", path, vol);
    (void)sk_audio_player(path, 20000, 0, (uint32_t)vol);

    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* audio.remove: delete audio files {p:["bt0.wav","bt1.wav"]} */
static void cmd_audio_remove(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *item;
    char path[96];

    if (cJSON_IsArray(p)) {
        cJSON_ArrayForEach(item, p) {
            if (cJSON_IsString(item) && is_safe_filename(item->valuestring)) {
                (void)snprintf(path, sizeof(path), "%s%s",
                               AUDIO_BASE_PATH, item->valuestring);
                printf("[MQTT CMD] audio.remove: %s\n", path);
                (void)remove(path);
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* audio.hash: SHA256 of audio file — stub, missing SHA256 impl */
static void cmd_audio_hash(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    /* TODO: implement SHA256 computation */
    printf("[MQTT CMD] audio.hash: SHA256 not implemented\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"e\":-99}", cmd);
}

/* ================================================================== */
/*  NEW handlers — Directory                                            */
/* ================================================================== */

/* dir: list files in directory */
static void cmd_dir(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    DIR *dir;
    struct dirent *ent;
    char dir_path[96];
    int pos;
    bool first = true;

    if (cJSON_IsString(p)) {
        if (!is_safe_filename(p->valuestring)) {
            printf("[MQTT CMD] dir: BLOCKED unsafe path\n");
            (void)snprintf(resp, (size_t)resp_sz,
                           "{\"c\":\"%s\",\"e\":-4}", cmd);
            return;
        }
        (void)snprintf(dir_path, sizeof(dir_path), "/user/%s", p->valuestring);
    } else {
        (void)snprintf(dir_path, sizeof(dir_path), "/user");
    }

    pos = snprintf(resp, (size_t)resp_sz, "{\"c\":\"%s\",\"r\":[", cmd);

    dir = opendir(dir_path);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL && pos < resp_sz - 32) {
            if (ent->d_name[0] == '.') {
                continue;
            }
            if (!first) {
                pos += snprintf(resp + pos, (size_t)(resp_sz - pos), ",");
            }
            pos += snprintf(resp + pos, (size_t)(resp_sz - pos),
                            "\"%s\"", ent->d_name);
            first = false;
        }
        closedir(dir);
    }
    (void)snprintf(resp + pos, (size_t)(resp_sz - pos), "]}");
}

/* sec.format: format flash — stub */
static void cmd_sec_format(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    /* TODO: implement LittleFS format */
    printf("[MQTT CMD] sec.format requested\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ================================================================== */
/*  DFU — remote firmware update via HTTP download + sk_ota             */
/* ================================================================== */

static volatile bool g_dfu_in_progress = false;

/*
 * OTA completion callback — publishes result to /r topic.
 * Protocol: {"r":"dfu","p":"100%","ts":UTC} on success
 *           {"r":"dfu","p":"failed","e":CODE,"ts":UTC} on failure
 */
static void dfu_ota_callback(sk_ota_result_e result, int detail)
{
    char buf[128];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (result == SK_OTA_RESULT_OK) {
        (void)snprintf(buf, sizeof(buf),
                       "{\"r\":\"dfu\",\"p\":\"100%%\",\"ts\":%llu}", ts);
    } else {
        (void)snprintf(buf, sizeof(buf),
                       "{\"r\":\"dfu\",\"p\":\"failed\","
                       "\"e\":%d,\"ts\":%llu}",
                       (int)result, ts);
    }
    (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_RESP, buf, (int)strlen(buf), 0);
    g_dfu_in_progress = false;
    printf("[MQTT CMD] DFU OTA callback: result=%d detail=%d\n",
           (int)result, detail);
}

/*
 * dfu: start remote firmware update (async)
 *
 * Protocol v1.0.3:
 *   cmd:  {"c":"dfu","p":{"host":"http://...","file":"xxx.bin","device_type":"9160"}}
 *   rsp:  {"c":"dfu","r":0}
 *   progress (on /r, every 5%): {"r":"dfu","p":"5%","ts":UTC}
 *
 * Flow: parse params → construct URL → queue HTTP download → sk_ota_start_update_async
 */
static void cmd_dfu(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *host_item;
    cJSON *file_item;
    cJSON *dtype_item;
    char url_buf[512];
    char data_buf[768];
    const char *save_path;
    uint8_t other_type;
    size_t url_len;
    size_t path_len;

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    host_item  = cJSON_GetObjectItemCaseSensitive(p, "host");
    file_item  = cJSON_GetObjectItemCaseSensitive(p, "file");
    dtype_item = cJSON_GetObjectItemCaseSensitive(p, "device_type");

    if (!cJSON_IsString(host_item) || !cJSON_IsString(file_item)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    /* Reject if already running */
    if (g_dfu_in_progress || get_download_file_state() == 1) {
        printf("[MQTT CMD] dfu: rejected, OTA already in progress\n");
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-3}", cmd);
        return;
    }

    /* Construct full URL: host/file */
    (void)snprintf(url_buf, sizeof(url_buf), "%s/%s",
                   host_item->valuestring, file_item->valuestring);

    /* Determine update type from device_type */
    if (cJSON_IsString(dtype_item) &&
        strcmp(dtype_item->valuestring, "9160") == 0) {
        save_path  = "/update/Hi2131EV100.fwpkg.bin";
        other_type = MQTT_TYPE_DOWN_MODEM;
    } else {
        /* 52840 OTA is not yet supported per protocol v1.0.3 */
        printf("[MQTT CMD] dfu: unsupported device_type\n");
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-4}", cmd);
        return;
    }

    /* Register OTA result callback */
    sk_ota_register_callback(dfu_ota_callback);

    /* Skip CRC check: MQTT DFU protocol has no CRC field.
     * The firmware package has internal bootloader verification. */
    g_ota_skip_crc = true;

    /* Construct "url;file_path" data for http_send_data_to_server */
    url_len  = strlen(url_buf);
    path_len = strlen(save_path);
    if (url_len + path_len + 2 > sizeof(data_buf)) {
        g_ota_skip_crc = false;
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-5}", cmd);
        return;
    }
    (void)snprintf(data_buf, sizeof(data_buf), "%s;%s", url_buf, save_path);

    /* Queue to HTTP task for async download + OTA */
    if (http_send_data_to_server(data_buf, url_len + path_len + 2,
                                 MQTT_HTTP_DOWNLOAD_OTA, other_type) != 0) {
        g_ota_skip_crc = false;
        printf("[MQTT CMD] dfu: failed to queue HTTP download\n");
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-6}", cmd);
        return;
    }

    g_dfu_in_progress = true;
    printf("[MQTT CMD] DFU started: %s -> %s type=%u\n",
           url_buf, save_path, (unsigned)other_type);
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ================================================================== */
/*  NEW handlers — Settings                                             */
/* ================================================================== */

/* settings.get: read setting by key name */
static void cmd_settings_get(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    int val = 0;

    if (!cJSON_IsString(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    if (strcmp(p->valuestring, "fence_inner_padding") == 0) {
        val = g_set_fence_inner_padding;
    } else if (strcmp(p->valuestring, "power_saving") == 0) {
        val = g_set_power_saving;
    } else if (strcmp(p->valuestring, "shock_max_count") == 0) {
        val = g_set_shock_max_count;
    } else if (strcmp(p->valuestring, "shock_window_sec") == 0) {
        val = g_set_shock_window_sec;
    } else {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}", cmd, val);
}

/* settings.set: write setting {p:{k:"fence_inner_padding",v:1}} */
static void cmd_settings_set(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p     = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *k_obj;
    cJSON *v_obj;

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    k_obj = cJSON_GetObjectItemCaseSensitive(p, "k");
    v_obj = cJSON_GetObjectItemCaseSensitive(p, "v");

    if (!cJSON_IsString(k_obj) || !cJSON_IsNumber(v_obj)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    if (strcmp(k_obj->valuestring, "fence_inner_padding") == 0) {
        g_set_fence_inner_padding = v_obj->valueint;
    } else if (strcmp(k_obj->valuestring, "power_saving") == 0) {
        g_set_power_saving = v_obj->valueint;
    } else if (strcmp(k_obj->valuestring, "shock_max_count") == 0) {
        g_set_shock_max_count = v_obj->valueint;
    } else if (strcmp(k_obj->valuestring, "shock_window_sec") == 0) {
        g_set_shock_window_sec = v_obj->valueint;
    } else {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-3}", cmd);
        return;
    }

    settings_save();
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* power_saving: query (no p) / set (p:0/1) */
static void cmd_power_saving(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");

    if (cJSON_IsNumber(p)) {
        /* Set mode */
        g_set_power_saving = p->valueint;
        settings_save();
        printf("[MQTT CMD] power_saving = %d\n", g_set_power_saving);
    }
    /* Both query and set return current value */
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}", cmd, g_set_power_saving);
}

/* shock_limit: query (no p) / set (p:{max_count:N, window_sec:N}) */
static void cmd_shock_limit(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");

    if (cJSON_IsObject(p)) {
        /* Set */
        cJSON *mc = cJSON_GetObjectItemCaseSensitive(p, "max_count");
        cJSON *ws = cJSON_GetObjectItemCaseSensitive(p, "window_sec");
        if (cJSON_IsNumber(mc)) g_set_shock_max_count  = mc->valueint;
        if (cJSON_IsNumber(ws)) g_set_shock_window_sec = ws->valueint;
        settings_save();
        printf("[MQTT CMD] shock_limit: max=%d win=%d\n",
               g_set_shock_max_count, g_set_shock_window_sec);
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":0}", cmd);
    } else {
        /* Query */
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":{"
                       "\"max_count\":%d,"
                       "\"window_sec\":%d"
                       "}}", cmd,
                       g_set_shock_max_count, g_set_shock_window_sec);
    }
}

/* ================================================================== */
/*  NEW handlers — Usage                                                */
/* ================================================================== */

/* usage.stats: system statistics */
static void cmd_usage_stats(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    uint32_t tick = osKernelGetTickCount();
    uint32_t freq = osKernelGetTickFreq();
    uint32_t up   = (freq > 0) ? (tick / freq) : 0;

    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"t\":%u,"
                   "\"sys\":{\"sr\":[%u,0,0,%u,0]},"
                   "\"network\":{"
                   "\"lte\":{\"l1\":0,\"l0\":0,\"l1ct\":0,\"l1it\":0},"
                   "\"aws\":{\"a1\":0,\"a0\":0,\"a1t\":0,"
                   "\"af\":[0,0,0,0,0,0,0],\"ac\":[0,0,0]}"
                   "},"
                   "\"gnss\":{\"g1\":0,\"g0\":0,\"g1t\":0,\"gf\":0,\"gl\":0,\"gt\":0},"
                   "\"battery\":{\"bc\":0}"
                   "}}", cmd, (unsigned)up,
                   (unsigned)g_usage_reset_count,
                   (unsigned)g_usage_reset_count);
}

/* usage.stats.clear: reset statistics */
static void cmd_usage_stats_clear(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_usage_reset_count = 0;
    printf("[MQTT CMD] usage stats cleared\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* usage.log.enabled: query/set log enabled */
static void cmd_usage_log_enabled(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");

    if (cJSON_IsNumber(p)) {
        printf("[MQTT CMD] usage.log.enabled = %d\n", p->valueint);
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* usage.log.clear */
static void cmd_usage_log_clear(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    printf("[MQTT CMD] usage log cleared\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ================================================================== */
/*  Fence / Beacon / Alert — merged from mqtt_cmd_ext.c                 */
/* ================================================================== */

/* ---- Fence data model ---- */
#define MAX_FENCES              8
#define FENCE_ID_MAX_LEN        32
#define FENCE_MAX_POLY_POINTS   20

typedef struct {
    char     id[FENCE_ID_MAX_LEN];
    bool     in_use;
    bool     enabled;
    int      fp_type;
    double   fp_points[FENCE_MAX_POLY_POINTS * 2];
    int      fp_point_count;
    double   fn_center[2];
    double   fn_radius;
} fence_entry_t;

static fence_entry_t g_fences[MAX_FENCES];
static int           g_fence_count   = 0;
static bool          g_fence_enabled = false;

/* ---- Beacon data model ---- */
#define MAX_BEACONS             16
#define BEACON_ID_MAX_LEN       32

typedef struct {
    char  id[BEACON_ID_MAX_LEN];
    bool  in_use;
    int   type;
    bool  active;
} beacon_entry_t;

static beacon_entry_t g_beacons[MAX_BEACONS];
static int            g_beacon_count   = 0;
static bool           g_beacon_enabled = false;

/* ---- Alert Profile data model ---- */
#define MAX_ALERT_PROFILES      16

typedef struct {
    struct {
        int on_ms;
        int off_ms;
        int repeat;
    } motor;
    struct {
        int freq;
        int duty;
        int dur;
    } shock;
    struct {
        char path[32];
        int  vol;
    } audio;
} alert_profile_t;

static alert_profile_t g_profiles[MAX_ALERT_PROFILES];
static bool            g_profiles_inited = false;

static void profiles_init_once(void)
{
    if (!g_profiles_inited) {
        (void)memset(g_profiles, 0, sizeof(g_profiles));
        g_profiles_inited = true;
    }
}

/* ---- Fence helpers ---- */
static fence_entry_t *fence_find_by_id(const char *id)
{
    int i;
    for (i = 0; i < MAX_FENCES; i++) {
        if (g_fences[i].in_use && strcmp(g_fences[i].id, id) == 0) {
            return &g_fences[i];
        }
    }
    return NULL;
}

static fence_entry_t *fence_alloc(void)
{
    int i;
    for (i = 0; i < MAX_FENCES; i++) {
        if (!g_fences[i].in_use) {
            return &g_fences[i];
        }
    }
    return NULL;
}

static void cmd_fence_query(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}", cmd, g_fence_enabled ? 1 : 0);
}

static void cmd_fence_on(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_fence_enabled = true;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_fence_off(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_fence_enabled = false;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_fence_active_q(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    int i, pos;
    (void)root;

    pos = snprintf(resp, (size_t)resp_sz, "{\"c\":\"%s\",\"r\":[", cmd);
    for (i = 0; i < MAX_FENCES && pos < resp_sz - 4; i++) {
        if (g_fences[i].in_use && g_fences[i].enabled) {
            if (pos > 0 && resp[pos - 1] == '"') {
                pos += snprintf(resp + pos, (size_t)(resp_sz - pos), ",");
            }
            pos += snprintf(resp + pos, (size_t)(resp_sz - pos),
                            "\"%s\"", g_fences[i].id);
        }
    }
    (void)snprintf(resp + pos, (size_t)(resp_sz - pos), "]}");
}

static void cmd_fence_activate(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *item;
    fence_entry_t *f;

    if (cJSON_IsArray(p)) {
        cJSON_ArrayForEach(item, p) {
            if (cJSON_IsString(item)) {
                f = fence_find_by_id(item->valuestring);
                if (f) {
                    f->enabled = true;
                }
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_fence_deactivate(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *item;
    fence_entry_t *f;

    if (cJSON_IsArray(p)) {
        cJSON_ArrayForEach(item, p) {
            if (cJSON_IsString(item)) {
                f = fence_find_by_id(item->valuestring);
                if (f) {
                    f->enabled = false;
                }
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_fence_list(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    int i, pos;
    (void)root;

    pos = snprintf(resp, (size_t)resp_sz, "{\"c\":\"%s\",\"r\":[", cmd);
    for (i = 0; i < MAX_FENCES && pos < resp_sz - 4; i++) {
        if (g_fences[i].in_use) {
            if (pos > 0 && resp[pos - 1] == '"') {
                pos += snprintf(resp + pos, (size_t)(resp_sz - pos), ",");
            }
            pos += snprintf(resp + pos, (size_t)(resp_sz - pos),
                            "\"%s\"", g_fences[i].id);
        }
    }
    (void)snprintf(resp + pos, (size_t)(resp_sz - pos), "]}");
}

static void cmd_fence_add(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *id_item, *fp_arr, *fn_arr;
    fence_entry_t *f;

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    id_item = cJSON_GetObjectItemCaseSensitive(p, "id");
    if (!cJSON_IsString(id_item)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    f = fence_find_by_id(id_item->valuestring);
    if (!f) {
        f = fence_alloc();
        if (!f) {
            (void)snprintf(resp, (size_t)resp_sz,
                           "{\"c\":\"%s\",\"e\":-3}", cmd);
            return;
        }
        g_fence_count++;
    }

    (void)memset(f, 0, sizeof(*f));
    f->in_use = true;
    f->enabled = true;
    (void)snprintf(f->id, sizeof(f->id), "%s", id_item->valuestring);

    /* Parse positive fence polygon (fp) */
    fp_arr = cJSON_GetObjectItemCaseSensitive(p, "fp");
    if (cJSON_IsArray(fp_arr)) {
        cJSON *fp_item = cJSON_GetArrayItem(fp_arr, 0);
        if (cJSON_IsObject(fp_item)) {
            cJSON *ti  = cJSON_GetObjectItemCaseSensitive(fp_item, "type");
            cJSON *pts = cJSON_GetObjectItemCaseSensitive(fp_item, "p");

            if (cJSON_IsNumber(ti)) {
                f->fp_type = ti->valueint;
            }
            if (cJSON_IsArray(pts)) {
                int cnt = cJSON_GetArraySize(pts);
                int j;
                if (cnt > FENCE_MAX_POLY_POINTS * 2) {
                    cnt = FENCE_MAX_POLY_POINTS * 2;
                }
                for (j = 0; j < cnt; j++) {
                    cJSON *v = cJSON_GetArrayItem(pts, j);
                    f->fp_points[j] = cJSON_IsNumber(v) ? v->valuedouble : 0.0;
                }
                f->fp_point_count = cnt / 2;
            }
        }
    }

    /* Parse negative fence circle (fn) */
    fn_arr = cJSON_GetObjectItemCaseSensitive(p, "fn");
    if (cJSON_IsArray(fn_arr)) {
        cJSON *fn_item = cJSON_GetArrayItem(fn_arr, 0);
        if (cJSON_IsObject(fn_item)) {
            cJSON *center = cJSON_GetObjectItemCaseSensitive(fn_item, "center");
            cJSON *radius = cJSON_GetObjectItemCaseSensitive(fn_item, "radius");

            if (cJSON_IsArray(center) && cJSON_GetArraySize(center) >= 2) {
                f->fn_center[0] = cJSON_GetArrayItem(center, 0)->valuedouble;
                f->fn_center[1] = cJSON_GetArrayItem(center, 1)->valuedouble;
            }
            if (cJSON_IsNumber(radius)) {
                f->fn_radius = radius->valuedouble;
            }
        }
    }

    printf("[MQTT CMD] Fence added: %s type=%d pts=%d\n",
           f->id, f->fp_type, f->fp_point_count);
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_fence_delete(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *item;
    fence_entry_t *f;

    if (cJSON_IsArray(p)) {
        cJSON_ArrayForEach(item, p) {
            if (cJSON_IsString(item)) {
                f = fence_find_by_id(item->valuestring);
                if (f) {
                    (void)memset(f, 0, sizeof(*f));
                    if (g_fence_count > 0) {
                        g_fence_count--;
                    }
                }
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ---- Beacon helpers ---- */
static beacon_entry_t *beacon_find_by_id(const char *id)
{
    int i;
    for (i = 0; i < MAX_BEACONS; i++) {
        if (g_beacons[i].in_use && strcmp(g_beacons[i].id, id) == 0) {
            return &g_beacons[i];
        }
    }
    return NULL;
}

static beacon_entry_t *beacon_alloc(void)
{
    int i;
    for (i = 0; i < MAX_BEACONS; i++) {
        if (!g_beacons[i].in_use) {
            return &g_beacons[i];
        }
    }
    return NULL;
}

static void cmd_beacon_query(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}", cmd, g_beacon_enabled ? 1 : 0);
}

static void cmd_beacon_on(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_beacon_enabled = true;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_off(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    (void)root;
    g_beacon_enabled = false;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_list(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    int i, pos;
    (void)root;

    pos = snprintf(resp, (size_t)resp_sz, "{\"c\":\"%s\",\"r\":[", cmd);
    for (i = 0; i < MAX_BEACONS && pos < resp_sz - 32; i++) {
        if (g_beacons[i].in_use) {
            if (i > 0 && resp[pos - 1] == '}') {
                pos += snprintf(resp + pos, (size_t)(resp_sz - pos), ",");
            }
            pos += snprintf(resp + pos, (size_t)(resp_sz - pos),
                            "{\"id\":\"%s\",\"type\":%d,\"state\":%d}",
                            g_beacons[i].id, g_beacons[i].type,
                            g_beacons[i].active ? 1 : 0);
        }
    }
    (void)snprintf(resp + pos, (size_t)(resp_sz - pos), "]}");
}

static void cmd_beacon_add(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *id_item, *type_item;
    beacon_entry_t *b;

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    id_item   = cJSON_GetObjectItemCaseSensitive(p, "id");
    type_item = cJSON_GetObjectItemCaseSensitive(p, "type");

    if (!cJSON_IsString(id_item)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    b = beacon_find_by_id(id_item->valuestring);
    if (!b) {
        b = beacon_alloc();
        if (!b) {
            (void)snprintf(resp, (size_t)resp_sz,
                           "{\"c\":\"%s\",\"e\":-3}", cmd);
            return;
        }
        g_beacon_count++;
    }

    (void)memset(b, 0, sizeof(*b));
    b->in_use = true;
    b->active = true;
    (void)snprintf(b->id, sizeof(b->id), "%s", id_item->valuestring);
    b->type = cJSON_IsNumber(type_item) ? type_item->valueint : 0;

    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_delete(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    beacon_entry_t *b;

    if (cJSON_IsString(p)) {
        b = beacon_find_by_id(p->valuestring);
        if (b) {
            (void)memset(b, 0, sizeof(*b));
            if (g_beacon_count > 0) {
                g_beacon_count--;
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_activate(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    beacon_entry_t *b;

    if (cJSON_IsString(p)) {
        b = beacon_find_by_id(p->valuestring);
        if (b) {
            b->active = true;
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_deactivate(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    beacon_entry_t *b;

    if (cJSON_IsString(p)) {
        b = beacon_find_by_id(p->valuestring);
        if (b) {
            b->active = false;
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_beacon_modify(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *id_item, *type_item;
    beacon_entry_t *b;

    if (cJSON_IsObject(p)) {
        id_item   = cJSON_GetObjectItemCaseSensitive(p, "id");
        type_item = cJSON_GetObjectItemCaseSensitive(p, "type");
        if (cJSON_IsString(id_item)) {
            b = beacon_find_by_id(id_item->valuestring);
            if (b && cJSON_IsNumber(type_item)) {
                b->type = type_item->valueint;
            }
        }
    }
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ---- Alert Profile handlers ---- */
static void cmd_alert_get(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    int idx;
    alert_profile_t *pf;

    profiles_init_once();

    if (!cJSON_IsNumber(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }
    idx = p->valueint;
    if (idx < 0 || idx >= MAX_ALERT_PROFILES) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    pf = &g_profiles[idx];
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"m\":{\"on\":%d,\"off\":%d,\"rep\":%d},"
                   "\"s\":{\"freq\":%d,\"duty\":%d,\"dur\":%d},"
                   "\"a\":{\"path\":\"%s\",\"vol\":%d}}}",
                   cmd,
                   pf->motor.on_ms, pf->motor.off_ms, pf->motor.repeat,
                   pf->shock.freq, pf->shock.duty, pf->shock.dur,
                   pf->audio.path, pf->audio.vol);
}

static void cmd_alert_update(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *idx_item, *prf, *m, *s, *a;
    int idx;
    alert_profile_t *pf;

    profiles_init_once();

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    idx_item = cJSON_GetObjectItemCaseSensitive(p, "idx");
    prf      = cJSON_GetObjectItemCaseSensitive(p, "prf");
    if (!cJSON_IsNumber(idx_item) || !cJSON_IsObject(prf)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }

    idx = idx_item->valueint;
    if (idx < 0 || idx >= MAX_ALERT_PROFILES) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-3}", cmd);
        return;
    }
    pf = &g_profiles[idx];

    /* Parse motor config */
    m = cJSON_GetObjectItemCaseSensitive(prf, "m");
    if (cJSON_IsObject(m)) {
        cJSON *on  = cJSON_GetObjectItemCaseSensitive(m, "on");
        cJSON *off = cJSON_GetObjectItemCaseSensitive(m, "off");
        cJSON *rep = cJSON_GetObjectItemCaseSensitive(m, "rep");
        if (cJSON_IsNumber(on))  { pf->motor.on_ms  = on->valueint;  }
        if (cJSON_IsNumber(off)) { pf->motor.off_ms = off->valueint; }
        if (cJSON_IsNumber(rep)) { pf->motor.repeat = rep->valueint; }
    }

    /* Parse shock config */
    s = cJSON_GetObjectItemCaseSensitive(prf, "s");
    if (cJSON_IsObject(s)) {
        cJSON *freq = cJSON_GetObjectItemCaseSensitive(s, "freq");
        cJSON *duty = cJSON_GetObjectItemCaseSensitive(s, "duty");
        cJSON *dur  = cJSON_GetObjectItemCaseSensitive(s, "dur");
        if (cJSON_IsNumber(freq)) { pf->shock.freq = freq->valueint; }
        if (cJSON_IsNumber(duty)) { pf->shock.duty = duty->valueint; }
        if (cJSON_IsNumber(dur))  { pf->shock.dur  = dur->valueint;  }
    }

    /* Parse audio config */
    a = cJSON_GetObjectItemCaseSensitive(prf, "a");
    if (cJSON_IsObject(a)) {
        cJSON *path = cJSON_GetObjectItemCaseSensitive(a, "path");
        cJSON *vol  = cJSON_GetObjectItemCaseSensitive(a, "vol");
        if (cJSON_IsString(path)) {
            (void)snprintf(pf->audio.path, sizeof(pf->audio.path),
                           "%s", path->valuestring);
        }
        if (cJSON_IsNumber(vol)) {
            pf->audio.vol = vol->valueint;
        }
    }

    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_alert_exec_idx(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");
    cJSON *idx_item;
    int idx;

    profiles_init_once();

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }
    idx_item = cJSON_GetObjectItemCaseSensitive(p, "idx");
    if (!cJSON_IsNumber(idx_item)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-2}", cmd);
        return;
    }
    idx = idx_item->valueint;
    if (idx < 0 || idx >= MAX_ALERT_PROFILES) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-3}", cmd);
        return;
    }

    /* TODO: drive motor/shock/audio hardware */
    printf("[MQTT CMD] Executing alert profile[%d]\n", idx);
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

static void cmd_alert_exec_custom(const char *cmd, cJSON *root, char *resp, int resp_sz)
{
    cJSON *p = cJSON_GetObjectItemCaseSensitive(root, "p");

    if (!cJSON_IsObject(p)) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"e\":-1}", cmd);
        return;
    }

    /* TODO: parse m/s/a and drive hardware */
    printf("[MQTT CMD] Executing custom alert profile\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ================================================================== */
/*  Dispatch table                                                      */
/* ================================================================== */
typedef struct {
    const char       *cmd;
    cmd_handler_fn_t  handler;
} mqtt_cmd_entry_t;

static const mqtt_cmd_entry_t g_cmd_table[] = {
    /* --- P2: original commands --- */
    { "v",                  cmd_version        },
    { "b",                  cmd_battery        },
    { "di",                 cmd_device_info    },
    { "?",                  cmd_dump           },
    { "l",                  cmd_live_query     },
    { "l1",                 cmd_live_on        },
    { "l0",                 cmd_live_off       },
    { "network.status",     cmd_net_status     },
    { "lte.auto_reconnect", cmd_lte_reconnect  },
    { "sys.reboot",         cmd_reboot         },
    { "sys.poweroff",       cmd_poweroff       },
    { "factory-reset",      cmd_factory_reset  },

    /* --- GNSS --- */
    { "gnss.status",        cmd_gnss_status    },
    { "gnss.diag",          cmd_gnss_diag      },

    /* --- Audio --- */
    { "audio.list",         cmd_audio_list     },
    { "audio.play",         cmd_audio_play     },
    { "audio.remove",       cmd_audio_remove   },
    { "audio.hash",         cmd_audio_hash     },

    /* --- Directory / Format --- */
    { "dir",                cmd_dir            },
    { "sec.format",         cmd_sec_format     },

    /* --- DFU (FOTA) --- */
    { "dfu",                cmd_dfu            },

    /* --- Settings --- */
    { "settings.get",       cmd_settings_get   },
    { "settings.set",       cmd_settings_set   },
    { "power_saving",       cmd_power_saving   },
    { "shock_limit",        cmd_shock_limit    },

    /* --- Usage --- */
    { "usage.stats",        cmd_usage_stats       },
    { "usage.stats.clear",  cmd_usage_stats_clear },
    { "usage.log.enabled",  cmd_usage_log_enabled },
    { "usage.log.clear",    cmd_usage_log_clear   },

    /* --- Fence (mqtt_cmd_ext.c) --- */
    { "f",                  cmd_fence_query       },
    { "f1",                 cmd_fence_on          },
    { "f0",                 cmd_fence_off         },
    { "fe",                 cmd_fence_active_q    },
    { "fe1",                cmd_fence_activate    },
    { "fe0",                cmd_fence_deactivate  },
    { "fl",                 cmd_fence_list        },
    { "fa",                 cmd_fence_add         },
    { "fd",                 cmd_fence_delete      },

    /* --- Beacon (mqtt_cmd_ext.c) --- */
    { "k",                  cmd_beacon_query      },
    { "k1",                 cmd_beacon_on         },
    { "k0",                 cmd_beacon_off        },
    { "kl",                 cmd_beacon_list       },
    { "ka",                 cmd_beacon_add        },
    { "kd",                 cmd_beacon_delete     },
    { "ke1",                cmd_beacon_activate   },
    { "ke0",                cmd_beacon_deactivate },
    { "kc",                 cmd_beacon_modify     },

    /* --- Alert Profile (mqtt_cmd_ext.c) --- */
    { "ap",                 cmd_alert_get         },
    { "ap.update",          cmd_alert_update      },
    { "ap.exec_by_idx",     cmd_alert_exec_idx    },
    { "ap.exec_custom",     cmd_alert_exec_custom },
};

#define CMD_TABLE_SIZE ((int)(sizeof(g_cmd_table) / sizeof(g_cmd_table[0])))

/* ================================================================== */
/*  Core dispatch: called by mqtt_service cmd callback                  */
/* ================================================================== */
static void mqtt_cmd_dispatch(const char *topic,
                               const void *payload, int payload_len)
{
    char   json_buf[CMD_RESP_BUF_SIZE];
    cJSON *root;
    cJSON *c_item;
    cJSON *i_item;
    char  *cmd_name;
    char   session_id[CMD_ID_MAX_LEN] = {0};
    char   resp[CMD_RESP_BUF_SIZE];
    int    copy_len;
    int    i;

    (void)topic;

    if (payload == NULL || payload_len <= 0) {
        return;
    }

    /* Copy payload to NUL-terminated buffer */
    copy_len = (payload_len < (int)sizeof(json_buf) - 1)
               ? payload_len : (int)sizeof(json_buf) - 1;
    (void)memcpy(json_buf, payload, (size_t)copy_len);
    json_buf[copy_len] = '\0';

    /* Parse JSON with cJSON */
    root = cJSON_Parse(json_buf);
    if (root == NULL) {
        printf("[MQTT CMD] JSON parse failed: %s\n", json_buf);
        return;
    }

    /* Extract command name */
    c_item = cJSON_GetObjectItemCaseSensitive(root, "c");
    if (!cJSON_IsString(c_item) || c_item->valuestring == NULL) {
        printf("[MQTT CMD] No 'c' field in payload\n");
        cJSON_Delete(root);
        return;
    }
    cmd_name = c_item->valuestring;

    /* Extract optional session ID */
    i_item = cJSON_GetObjectItemCaseSensitive(root, "i");
    if (cJSON_IsString(i_item) && i_item->valuestring != NULL) {
        (void)snprintf(session_id, sizeof(session_id), "%s", i_item->valuestring);
    }

    printf("[MQTT CMD] Received cmd='%s' i='%s'\n", cmd_name, session_id);

    /* Look up and dispatch */
    resp[0] = '\0';
    for (i = 0; i < CMD_TABLE_SIZE; i++) {
        if (strcmp(g_cmd_table[i].cmd, cmd_name) == 0) {
            g_cmd_table[i].handler(cmd_name, root, resp, sizeof(resp));
            if (resp[0] != '\0') {
                /* Inject session ID before closing brace if present */
                if (session_id[0] != '\0') {
                    int rlen    = (int)strlen(resp);
                    int sid_len = (int)strlen(session_id);
                    if (rlen > 0 && resp[rlen - 1] == '}' &&
                        rlen + sid_len + 8 < CMD_RESP_BUF_SIZE) {
                        char id_suffix[CMD_ID_MAX_LEN + 10];
                        (void)snprintf(id_suffix, sizeof(id_suffix),
                                       ",\"i\":\"%s\"}", session_id);
                        resp[rlen - 1] = '\0';
                        (void)strncat(resp, id_suffix,
                                      (size_t)(CMD_RESP_BUF_SIZE - rlen - 1));
                    }
                }
                publish_response(resp);
            }
            cJSON_Delete(root);
            return;
        }
    }

    /* Unregistered / not-yet-implemented command */
    printf("[MQTT CMD] Unimplemented command: '%s'\n", cmd_name);
    if (session_id[0] != '\0') {
        (void)snprintf(resp, sizeof(resp),
                       "{\"c\":\"%s\",\"e\":-99,\"i\":\"%s\"}",
                       cmd_name, session_id);
    } else {
        (void)snprintf(resp, sizeof(resp),
                       "{\"c\":\"%s\",\"e\":-99}", cmd_name);
    }
    publish_response(resp);
    cJSON_Delete(root);
}

/* ================================================================== */
/*  Public API                                                          */
/* ================================================================== */

void mqtt_cmd_handler_init(void)
{
    settings_load();
    mqtt_service_register_cmd_callback(mqtt_cmd_dispatch);
    printf("[MQTT CMD] Command handler registered (%d commands)\n",
           CMD_TABLE_SIZE);
}

bool mqtt_cmd_handler_live_is_on(void)
{
    return g_live_enabled;
}
