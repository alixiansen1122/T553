/**
 * mqtt_event.c
 *
 * Unified MQTT outbound publisher — events (/e), activity (/a), GNSS (/g).
 *
 * Consolidated from: mqtt_event.c + mqtt_activity.c + mqtt_gnss.c
 *
 * Implemented events (P2):
 *   boot, gnss-fix, gnss-lost, battery, charging-started/stopped/completed,
 *   shutdown
 * Implemented events (P3):
 *   fence-*, beacon, modules_status, lte_gnss_off, usage, suggest_PS_*,
 *   EPO_download, charging-abnormal, charging-error
 */

#include "mqtt_event.h"
#include "mqtt_service.h"
#include "sh366102.h"
#include "watch_version.h"
#include "dev_storage.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>          /* time(NULL) for UTC timestamp */

/* ------------------------------------------------------------------ */
/*  Platform externals                                                  */
/* ------------------------------------------------------------------ */
extern uint8_t  g_chargestatus;             /* sh366102.c — 0=dis,1=chg,2=full */
extern unsigned char SH366102_Address;      /* sh366102.c — I2C address 0x55   */

/* Forward declarations for sh366102 functions not in the public header */
extern int16_t SH366102_Read_chg_current(void);
extern float   SH366102_ReadTemperature(uint8_t address);

/* ------------------------------------------------------------------ */
/*  Module-private state — Events                                       */
/* ------------------------------------------------------------------ */
#define EVENT_BUF_SIZE        512          /* Must fit usage event (~250B) + margin */
#define BATTERY_DELTA_MV      150u         /* ~5% of 3000 mV range → ~150 mV */
#define CHARGE_STATUS_UNKNOWN 0xFFu

static volatile bool    g_gnss_was_fixed       = false;
static volatile int32_t g_last_lat_e6          = 0;    /* lat × 1,000,000 — atomic on 32-bit MCU */
static volatile int32_t g_last_lon_e6          = 0;    /* lon × 1,000,000 — atomic on 32-bit MCU */
static volatile uint16_t  g_last_batt_mv       = 0;
static volatile uint8_t   g_last_charge_status = CHARGE_STATUS_UNKNOWN;

/* ------------------------------------------------------------------ */
/*  Internal helper: build and publish to /e topic                      */
/* ------------------------------------------------------------------ */
static void publish_event(const char *json)
{
    int len;
    int rc;

    if (json == NULL) {
        return;
    }
    len = (int)strlen(json);
    if (len <= 0) {
        return;
    }
    printf("[MQTT EVT] %s\n", json);
    rc = mqtt_service_publish(MQTT_TOPIC_SUFFIX_EVENT, json, len, 0);
    if (rc != 0) {
        printf("[MQTT EVT] WARNING: publish failed rc=%d (queue full?)\n", rc);
    }
}

/* ------------------------------------------------------------------ */
/*  Boot event                                                          */
/* ------------------------------------------------------------------ */
static void publish_boot_event(void)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    /* Protocol: {"e":"boot","r":"SW,<ver>","ts":<utc>} */
    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"boot\",\"r\":\"SW,%s\",\"ts\":%llu}",
                   VERSION_STRING, ts);
    publish_event(buf);
}

/* ------------------------------------------------------------------ */
/*  Public API — init                                                   */
/* ------------------------------------------------------------------ */
void mqtt_event_init(void)
{
    /* Snapshot initial battery state so the first poll doesn't false-fire */
    g_last_batt_mv       = get_battery_voltage();
    g_last_charge_status = g_chargestatus;
    g_gnss_was_fixed     = false;

    publish_boot_event();
    printf("[MQTT EVT] Event subsystem initialized\n");
}

/* ------------------------------------------------------------------ */
/*  GNSS events                                                         */
/* ------------------------------------------------------------------ */
void mqtt_event_gnss_fix(double lat_deg, double lon_deg)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (!g_gnss_was_fixed) {
        (void)snprintf(buf, sizeof(buf),
                       "{\"e\":\"gnss-fix\","
                       "\"r\":{\"lat\":%.6f,\"lon\":%.6f},"
                       "\"ts\":%llu}",
                       lat_deg, lon_deg, ts);
        publish_event(buf);
        g_gnss_was_fixed = true;
    }
    /* Always update last known position — int32 writes are atomic on Cortex-M */
    g_last_lat_e6 = (int32_t)(lat_deg * 1000000.0);
    g_last_lon_e6 = (int32_t)(lon_deg * 1000000.0);
}

void mqtt_event_gnss_lost(void)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (g_gnss_was_fixed) {
        (void)snprintf(buf, sizeof(buf),
                       "{\"e\":\"gnss-lost\","
                       "\"r\":{\"lat\":%.6f,\"lon\":%.6f},"
                       "\"ts\":%llu}",
                       (double)g_last_lat_e6 / 1000000.0,
                       (double)g_last_lon_e6 / 1000000.0, ts);
        publish_event(buf);
        g_gnss_was_fixed = false;
    }
}

/* ------------------------------------------------------------------ */
/*  Battery events — common helper                                      */
/* ------------------------------------------------------------------ */
static void publish_battery_event(const char *event_name,
                                   uint16_t mv, uint8_t cap, int ch_ind)
{
    char     buf[EVENT_BUF_SIZE];
    int16_t  cur_ma     = SH366102_Read_chg_current();
    int      bat_temp10 = (int)(SH366102_ReadTemperature(SH366102_Address) * 10.0f);
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"%s\","
                   "\"r\":{\"v\":%u,\"c\":%u,\"ch\":%d,"
                   "\"i\":%d,\"t\":[%d,-1]},"
                   "\"ts\":%llu}",
                   event_name,
                   (unsigned)mv, (unsigned)cap, ch_ind,
                   (int)cur_ma, bat_temp10, ts);
    publish_event(buf);
}

/* ------------------------------------------------------------------ */
/*  Battery events (polling-based)                                      */
/* ------------------------------------------------------------------ */
void mqtt_event_battery_poll(void)
{
    uint16_t cur_mv      = get_battery_voltage();
    uint8_t  cur_cap     = get_battery_level();
    uint8_t  cur_charge  = g_chargestatus;
    uint16_t delta;

    /* --- Charging state transitions --- */
    if (cur_charge != g_last_charge_status) {
        if (cur_charge == 1 && (g_last_charge_status == 0 ||
                                g_last_charge_status == CHARGE_STATUS_UNKNOWN)) {
            publish_battery_event("charging-started", cur_mv, cur_cap, 1);

        } else if (cur_charge == 0 && (g_last_charge_status == 1 ||
                                        g_last_charge_status == 2)) {
            publish_battery_event("charging-stopped", cur_mv, cur_cap, 0);

        } else if (cur_charge == 2) {
            publish_battery_event("charging-completed", cur_mv, cur_cap, 1);
        }

        g_last_charge_status = cur_charge;
    }

    /* --- Significant voltage change (>BATTERY_DELTA_MV) --- */
    delta = (cur_mv > g_last_batt_mv)
            ? (cur_mv - g_last_batt_mv)
            : (g_last_batt_mv - cur_mv);

    if (delta >= BATTERY_DELTA_MV) {
        publish_battery_event("battery", cur_mv, cur_cap,
                              (cur_charge > 0) ? 1 : 0);
        g_last_batt_mv = cur_mv;
    }
}

/* ------------------------------------------------------------------ */
/*  Shutdown event                                                      */
/* ------------------------------------------------------------------ */
void mqtt_event_shutdown(void)
{
    char buf[EVENT_BUF_SIZE];
    uint16_t mv      = get_battery_voltage();
    uint8_t  cap     = get_battery_level();
    int16_t  ma      = SH366102_Read_chg_current();
    float    temp_f  = SH366102_ReadTemperature(SH366102_Address);
    int      temp10  = (int)(temp_f * 10.0f);
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"shutdown\","
                   "\"r\":{\"v\":%u,\"c\":%u,\"ch\":%u,"
                   "\"i\":%d,\"t\":[%d,-1]},"
                   "\"ts\":%llu}",
                   (unsigned)mv, (unsigned)cap,
                   (unsigned)(g_chargestatus > 0 ? 1 : 0),
                   (int)ma, temp10,
                   ts);
    publish_event(buf);
}

/* ================================================================== */
/*  GNSS state getters — used by mqtt_cmd_handler for gnss.status       */
/* ================================================================== */

bool mqtt_event_gnss_is_fixed(void)
{
    return g_gnss_was_fixed;
}

double mqtt_event_gnss_last_lat(void)
{
    return (double)g_last_lat_e6 / 1000000.0;
}

double mqtt_event_gnss_last_lon(void)
{
    return (double)g_last_lon_e6 / 1000000.0;
}

/* ================================================================== */
/*  P3 Events — implementations                                        */
/* ================================================================== */

/* Fence state change event */
void mqtt_event_fence(const char *event_name, double lat_deg, double lon_deg,
                      const char *fence_id, int from_state, int to_state)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (event_name == NULL || fence_id == NULL) {
        return;
    }

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"%s\","
                   "\"from\":%d,\"to\":%d,"
                   "\"lat\":%.6f,\"lon\":%.6f,"
                   "\"fence id\":\"%s\","
                   "\"ts\":%llu}",
                   event_name, from_state, to_state,
                   lat_deg, lon_deg, fence_id, ts);
    publish_event(buf);
}

/* Beacon proximity event */
void mqtt_event_beacon(int type, int action, const char *addr)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (addr == NULL) {
        return;
    }

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"beacon\","
                   "\"type\":%d,\"action\":%d,"
                   "\"addr\":\"%s\","
                   "\"ts\":%llu}",
                   type, action, addr, ts);
    publish_event(buf);
}

/* Module status event (typically BLE channel) */
void mqtt_event_modules_status(int lte_state, int aws_state,
                                int gnss_state, int ble_state)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)ble_state; /* reserved */

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"modules_status\","
                   "\"network\":{\"quality\":%d},"
                   "\"gnss\":{\"quality\":%d},"
                   "\"ts\":%llu}",
                   (lte_state && aws_state) ? 4 : (lte_state ? 2 : 0),
                   gnss_state,
                   ts);
    publish_event(buf);
}

/* Power saving LTE/GNSS off event */
void mqtt_event_lte_gnss_off(void)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"lte_gnss_off\",\"ts\":%llu}", ts);
    publish_event(buf);
}

/* Hourly usage statistics event */
void mqtt_event_usage(int lte_conn, int lte_on_cnt, int lte_off_cnt,
                      int aws_conn, int aws_on_cnt, int aws_off_cnt,
                      int gnss_state, int gnss_on_cnt, int gnss_off_cnt,
                      int bat_drain_pct, int ps_mode)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"usage\","
                   "\"ps\":%d,"
                   "\"sys\":{\"sr\":[0,0,0,0,0]},"
                   "\"network\":{"
                   "\"lte\":{\"l\":%d,\"l1\":%d,\"l0\":%d},"
                   "\"aws\":{\"a\":%d,\"a1\":%d,\"a0\":%d}"
                   "},"
                   "\"gnss\":{\"g\":%d,\"g1\":%d,\"g0\":%d},"
                   "\"bat\":{\"bd\":%d},"
                   "\"ts\":%llu}",
                   ps_mode,
                   lte_conn, lte_on_cnt, lte_off_cnt,
                   aws_conn, aws_on_cnt, aws_off_cnt,
                   gnss_state, gnss_on_cnt, gnss_off_cnt,
                   bat_drain_pct,
                   ts);
    publish_event(buf);
}

/* Power saving suggestion events */
void mqtt_event_suggest_ps(bool enable)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"suggest_PS_%s\",\"ts\":%llu}",
                   enable ? "on" : "off", ts);
    publish_event(buf);
}

/* EPO download event */
void mqtt_event_epo_download(int constellation)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"EPO_download\",\"t\":%d,\"ts\":%llu}",
                   constellation, ts);
    publish_event(buf);
}

/* Charging abnormal event */
void mqtt_event_charging_abnormal(void)
{
    char     buf[EVENT_BUF_SIZE];
    uint16_t mv      = get_battery_voltage();
    uint8_t  cap     = get_battery_level();
    int16_t  ma      = SH366102_Read_chg_current();
    int      temp10  = (int)(SH366102_ReadTemperature(SH366102_Address) * 10.0f);
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"charging-abnormal\","
                   "\"charging_ind\":1,"
                   "\"i\":%d,\"v\":%u,\"c\":%u,"
                   "\"t\":[%d,-1],"
                   "\"ts\":%llu}",
                   (int)ma, (unsigned)mv, (unsigned)cap,
                   temp10, ts);
    publish_event(buf);
}

/* Charging error event */
void mqtt_event_charging_error(uint8_t reg08, uint8_t reg09)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    (void)snprintf(buf, sizeof(buf),
                   "{\"e\":\"charging-error\","
                   "\"reg_value\":%u,"
                   "\"ts\":%llu}",
                   (unsigned)((reg08 << 8) | reg09), ts);
    publish_event(buf);
}

/* ================================================================== */
/*  Activity reporter — merged from mqtt_activity.c                     */
/* ================================================================== */

/* --- Configuration --- */
#define ACTIVITY_STEP_LEN_CM     65u
#define ACTIVITY_D_BUCKETS       4
#define POSTURE_IDX_STATIC       0
#define POSTURE_IDX_WALK         1
#define POSTURE_IDX_RUN          2
#define POSTURE_IDX_SHAKE        3
#define ACTIVITY_SAMPLE_MS       90u
#define BREATH_BPM_INVALID       (-1)
#define ACTIVITY_JSON_BUF        256

/* --- Internal state --- */
static osMutexId_t  g_act_mutex            = NULL;
static uint32_t     g_posture_ticks[ACTIVITY_D_BUCKETS];
static uint64_t     g_period_start_ts      = 0;
static uint32_t     g_step_count_at_start  = 0;
static volatile int g_breath_bpm           = BREATH_BPM_INVALID;
static volatile int g_breath_confidence    = 0;
static volatile bool g_has_data            = false;

static int posture_to_bucket(int64_t cls)
{
    switch (cls) {
    case 0:  return POSTURE_IDX_STATIC;
    case 1:  return POSTURE_IDX_WALK;
    case 2:  return POSTURE_IDX_RUN;
    default: return POSTURE_IDX_SHAKE;
    }
}

static uint32_t read_step_count(void)
{
    uint32_t steps = 0;
    (void)watch_storage_get(STORAGE_STEP_COUNT, &steps, (uint32_t)sizeof(steps));
    return steps;
}

void mqtt_activity_init(void)
{
    osMutexAttr_t attr = {0};
    attr.name = "act_mtx";

    (void)memset(g_posture_ticks, 0, sizeof(g_posture_ticks));
    g_period_start_ts     = 0;
    g_step_count_at_start = read_step_count();
    g_breath_bpm          = BREATH_BPM_INVALID;
    g_breath_confidence   = 0;
    g_has_data            = false;

    g_act_mutex = osMutexNew(&attr);
    if (g_act_mutex == NULL) {
        printf("[ACT] WARNING: Failed to create activity mutex\n");
    }

    printf("[ACT] Activity reporter initialized (step_start=%u)\n",
           (unsigned)g_step_count_at_start);
}

void mqtt_activity_on_posture(int64_t posture_class, uint64_t utc_ts)
{
    int bucket;

    if (g_act_mutex == NULL) {
        return;
    }
    bucket = posture_to_bucket(posture_class);

    (void)osMutexAcquire(g_act_mutex, osWaitForever);

    if (!g_has_data) {
        g_period_start_ts = utc_ts;
        g_has_data        = true;
    }
    g_posture_ticks[bucket]++;

    (void)osMutexRelease(g_act_mutex);
}

void mqtt_activity_set_breath(int bpm, int confidence)
{
    if (g_act_mutex != NULL) {
        (void)osMutexAcquire(g_act_mutex, osWaitForever);
    }
    g_breath_bpm        = bpm;
    g_breath_confidence = confidence;
    if (g_act_mutex != NULL) {
        (void)osMutexRelease(g_act_mutex);
    }
    printf("[ACT] Breath updated: bpm=%d conf=%d\n", bpm, confidence);
}

void mqtt_activity_publish(bool force_send)
{
    char     buf[ACTIVITY_JSON_BUF];
    uint64_t start_ts, end_ts;
    uint32_t ticks[ACTIVITY_D_BUCKETS];
    uint32_t d_s[ACTIVITY_D_BUCKETS];
    int      bpm, conf;
    uint32_t step_now, step_delta;
    uint32_t dist_cm, track_cm;
    int      len;
    int      i;
    unsigned long long now_ts;

    if (g_act_mutex == NULL) {
        return;
    }

    (void)osMutexAcquire(g_act_mutex, osWaitForever);

    if (!g_has_data && !force_send) {
        (void)osMutexRelease(g_act_mutex);
        return;
    }

    /* Snapshot and reset posture accumulators atomically */
    (void)memcpy(ticks, g_posture_ticks, sizeof(ticks));
    (void)memset(g_posture_ticks, 0, sizeof(g_posture_ticks));
    start_ts   = g_period_start_ts;
    bpm        = g_breath_bpm;
    conf       = g_breath_confidence;
    g_breath_bpm = BREATH_BPM_INVALID;
    g_has_data = false;

    (void)osMutexRelease(g_act_mutex);

    /* Wall-clock end timestamp */
    now_ts = (unsigned long long)time(NULL);
    end_ts = now_ts;
    if (start_ts == 0u) {
        start_ts = (end_ts > 60u) ? (end_ts - 60u) : 0u;
    }

    /* Mileage: step delta × step length */
    step_now   = read_step_count();
    step_delta = (step_now >= g_step_count_at_start)
                 ? (step_now - g_step_count_at_start)
                 : step_now;
    dist_cm    = step_delta * ACTIVITY_STEP_LEN_CM;
    track_cm   = dist_cm;
    g_step_count_at_start = step_now;

    /* Convert sample counts to whole seconds */
    for (i = 0; i < ACTIVITY_D_BUCKETS; i++) {
        d_s[i] = (ticks[i] * ACTIVITY_SAMPLE_MS) / 1000u;
    }
    {
        uint32_t d_active = d_s[POSTURE_IDX_WALK]
                          + d_s[POSTURE_IDX_RUN]
                          + d_s[POSTURE_IDX_SHAKE];

        /* ---- Publish 1: d[] posture data ---- */
        len = snprintf(buf, sizeof(buf),
                       "[{\"d\":[%u,%u,%u,%u,%u],\"ts\":%llu}]",
                       (unsigned)d_s[0], (unsigned)d_s[1],
                       (unsigned)d_s[2], (unsigned)d_s[3],
                       (unsigned)d_active,
                       now_ts);
        if (len > 0 && len < (int)sizeof(buf)) {
            printf("[ACT] Publish /a d: %s\n", buf);
            (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_ACTIVITY, buf, len, 0);
        }
    }

    /* ---- Publish 2: m[] mileage data -------------------------------- */
    len = snprintf(buf, sizeof(buf),
                   "[{\"m\":[%llu,%llu,%u,%u],\"ts\":%llu}]",
                   (unsigned long long)start_ts,
                   (unsigned long long)end_ts,
                   (unsigned)dist_cm,
                   (unsigned)track_cm,
                   now_ts);
    if (len > 0 && len < (int)sizeof(buf)) {
        printf("[ACT] Publish /a m: %s\n", buf);
        (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_ACTIVITY, buf, len, 0);
    }

    /* ---- Publish 3: h[] breathing (only if valid) ------------------- */
    if (bpm != BREATH_BPM_INVALID) {
        len = snprintf(buf, sizeof(buf),
                       "[{\"h\":[%d,%d],\"ts\":%llu}]",
                       bpm, conf, now_ts);
        if (len > 0 && len < (int)sizeof(buf)) {
            printf("[ACT] Publish /a h: %s\n", buf);
            (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_ACTIVITY, buf, len, 0);
        }
    }
}

/* ================================================================== */
/*  GNSS track publisher — merged from mqtt_gnss.c                      */
/* ================================================================== */

#define MQTT_GNSS_MAX_POINTS    32
#define GNSS_PB_BUF_SIZE        1024

/* --- Lightweight proto3 varint/zigzag encoder --- */

static int pb_encode_varint(uint8_t *buf, int buf_sz, uint64_t val)
{
    int pos = 0;
    while (val > 0x7F) {
        if (pos >= buf_sz) {
            return -1;
        }
        buf[pos++] = (uint8_t)((val & 0x7F) | 0x80);
        val >>= 7;
    }
    if (pos >= buf_sz) {
        return -1;
    }
    buf[pos++] = (uint8_t)(val & 0x7F);
    return pos;
}

static uint64_t pb_zigzag32(int32_t val)
{
    return (uint64_t)(((uint32_t)val << 1) ^ (uint32_t)(val >> 31));
}

static int pb_write_tag(uint8_t *buf, int buf_sz, int field_num, int wire_type)
{
    uint64_t tag = ((uint64_t)(unsigned)field_num << 3) | (uint64_t)(unsigned)wire_type;
    return pb_encode_varint(buf, buf_sz, tag);
}

static int pb_encode_sint32(uint8_t *buf, int buf_sz, int field_num, int32_t val)
{
    int pos = 0;
    int n;

    if (val == 0) {
        return 0;
    }

    n = pb_write_tag(buf + pos, buf_sz - pos, field_num, 0);
    if (n < 0) {
        return -1;
    }
    pos += n;

    n = pb_encode_varint(buf + pos, buf_sz - pos, pb_zigzag32(val));
    if (n < 0) {
        return -1;
    }
    pos += n;

    return pos;
}

static int pb_encode_int32(uint8_t *buf, int buf_sz, int field_num, int32_t val)
{
    int pos = 0;
    int n;

    if (val == 0) {
        return 0;
    }

    n = pb_write_tag(buf + pos, buf_sz - pos, field_num, 0);
    if (n < 0) {
        return -1;
    }
    pos += n;

    if (val < 0) {
        n = pb_encode_varint(buf + pos, buf_sz - pos, (uint64_t)(uint32_t)val);
    } else {
        n = pb_encode_varint(buf + pos, buf_sz - pos, (uint64_t)val);
    }
    if (n < 0) {
        return -1;
    }
    pos += n;

    return pos;
}

static int encode_gnss_point(uint8_t *buf, int buf_sz, const mqtt_gnss_point_t *pt)
{
    int pos = 0;
    int n;

    n = pb_encode_int32(buf + pos, buf_sz - pos, 1, pt->lat);
    if (n < 0) {
        return -1;
    }
    pos += n;

    n = pb_encode_int32(buf + pos, buf_sz - pos, 2, pt->lon);
    if (n < 0) {
        return -1;
    }
    pos += n;

    n = pb_encode_int32(buf + pos, buf_sz - pos, 3, pt->ts);
    if (n < 0) {
        return -1;
    }
    pos += n;

    n = pb_encode_int32(buf + pos, buf_sz - pos, 4, pt->quality);
    if (n < 0) {
        return -1;
    }
    pos += n;

    n = pb_encode_int32(buf + pos, buf_sz - pos, 5, pt->alt);
    if (n < 0) {
        return -1;
    }
    pos += n;

    return pos;
}

int mqtt_gnss_publish(const mqtt_gnss_point_t *points, int count)
{
    uint8_t buf[GNSS_PB_BUF_SIZE];
    uint8_t sub_buf[64];
    int pos = 0;
    int sub_len;
    int n;
    int i;

    if (points == NULL || count <= 0) {
        return -1;
    }
    if (count > MQTT_GNSS_MAX_POINTS) {
        count = MQTT_GNSS_MAX_POINTS;
    }

    for (i = 0; i < count; i++) {
        sub_len = encode_gnss_point(sub_buf, (int)sizeof(sub_buf), &points[i]);
        if (sub_len < 0) {
            printf("[MQTT GNSS] Failed to encode point %d\n", i);
            return -2;
        }

        n = pb_write_tag(buf + pos, GNSS_PB_BUF_SIZE - pos, 1, 2);
        if (n < 0) {
            return -3;
        }
        pos += n;

        n = pb_encode_varint(buf + pos, GNSS_PB_BUF_SIZE - pos, (uint64_t)sub_len);
        if (n < 0) {
            return -3;
        }
        pos += n;

        if (pos + sub_len > GNSS_PB_BUF_SIZE) {
            return -3;
        }
        (void)memcpy(buf + pos, sub_buf, (size_t)sub_len);
        pos += sub_len;
    }

    printf("[MQTT GNSS] Publishing %d points (%d bytes) to /g\n", count, pos);
    return mqtt_service_publish(MQTT_TOPIC_SUFFIX_GPS, buf, pos, 0);
}

int mqtt_gnss_publish_single(int32_t lat, int32_t lon, int32_t ts,
                              int32_t quality, int32_t alt)
{
    mqtt_gnss_point_t pt;
    pt.lat     = lat;
    pt.lon     = lon;
    pt.ts      = ts;
    pt.quality = quality;
    pt.alt     = alt;
    return mqtt_gnss_publish(&pt, 1);
}
