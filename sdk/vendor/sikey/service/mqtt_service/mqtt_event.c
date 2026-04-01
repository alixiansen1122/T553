/**
 * mqtt_event.c
 *
 * MQTT event publisher — sends unsolicited device events to the /e topic.
 *
 * Implemented events (P2):
 *   boot              — published once at startup
 *   gnss-fix          — when GNSS acquires first valid fix
 *   gnss-lost         — when fix is subsequently lost
 *   battery           — when voltage changes by >5%
 *   charging-started  — when charger connects
 *   charging-stopped  — when charger disconnects (not full)
 *   charging-completed — when battery reaches full
 *   shutdown          — before poweroff (caller must invoke before power-down)
 *
 * Stubbed events (P3): fence-*, beacon, modules_status, lte_gnss_off,
 *                       usage (hourly), suggest_PS_*, EPO_download
 */

#include "mqtt_event.h"
#include "mqtt_service.h"
#include "sh366102.h"
#include "watch_version.h"
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
/*  Module-private state                                                */
/* ------------------------------------------------------------------ */
#define EVENT_BUF_SIZE        320          /* Increased for i/t[] fields */
#define BATTERY_DELTA_MV      150u         /* ~5% of 3000 mV range → ~150 mV */
#define CHARGE_STATUS_UNKNOWN 0xFFu

static volatile bool    g_gnss_was_fixed       = false;
static volatile uint16_t g_last_batt_mv        = 0;
static volatile uint8_t  g_last_charge_status  = CHARGE_STATUS_UNKNOWN;

/* ------------------------------------------------------------------ */
/*  Internal helper: build and publish to /e topic                      */
/* ------------------------------------------------------------------ */
static void publish_event(const char *json)
{
    int len;

    if (json == NULL) {
        return;
    }
    len = (int)strlen(json);
    if (len <= 0) {
        return;
    }
    printf("[MQTT EVT] %s\n", json);
    (void)mqtt_service_publish(MQTT_TOPIC_SUFFIX_EVENT, json, len, 0);
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
}

void mqtt_event_gnss_lost(void)
{
    char buf[EVENT_BUF_SIZE];
    unsigned long long ts = (unsigned long long)time(NULL);

    if (g_gnss_was_fixed) {
        (void)snprintf(buf, sizeof(buf),
                       "{\"e\":\"gnss-lost\",\"ts\":%llu}", ts);
        publish_event(buf);
        g_gnss_was_fixed = false;
    }
}

/* ------------------------------------------------------------------ */
/*  Battery events (polling-based)                                      */
/* ------------------------------------------------------------------ */
void mqtt_event_battery_poll(void)
{
    uint16_t cur_mv      = get_battery_voltage();
    uint8_t  cur_cap     = get_battery_level();
    uint8_t  cur_charge  = g_chargestatus;
    char     buf[EVENT_BUF_SIZE];
    uint16_t delta;
    unsigned long long ts;
    int16_t  cur_ma;
    int      bat_temp10;

    /* --- Charging state transitions --- */
    if (cur_charge != g_last_charge_status) {
        ts = (unsigned long long)time(NULL);
        /* Read I2C sensors only when we actually need to publish */
        cur_ma     = SH366102_Read_chg_current();
        bat_temp10 = (int)(SH366102_ReadTemperature(SH366102_Address) * 10.0f);

        if (g_last_charge_status == 0 && cur_charge == 1) {
            (void)snprintf(buf, sizeof(buf),
                           "{\"e\":\"charging-started\","
                           "\"r\":{\"v\":%u,\"c\":%u,\"ch\":1,"
                           "\"i\":%d,\"t\":[%d,-1]},"
                           "\"ts\":%llu}",
                           (unsigned)cur_mv, (unsigned)cur_cap,
                           (int)cur_ma, bat_temp10, ts);
            publish_event(buf);

        } else if (g_last_charge_status == 1 && cur_charge == 0) {
            (void)snprintf(buf, sizeof(buf),
                           "{\"e\":\"charging-stopped\","
                           "\"r\":{\"v\":%u,\"c\":%u,\"ch\":0,"
                           "\"i\":%d,\"t\":[%d,-1]},"
                           "\"ts\":%llu}",
                           (unsigned)cur_mv, (unsigned)cur_cap,
                           (int)cur_ma, bat_temp10, ts);
            publish_event(buf);

        } else if (cur_charge == 2) {
            (void)snprintf(buf, sizeof(buf),
                           "{\"e\":\"charging-completed\","
                           "\"r\":{\"v\":%u,\"c\":%u,\"ch\":1,"
                           "\"i\":%d,\"t\":[%d,-1]},"
                           "\"ts\":%llu}",
                           (unsigned)cur_mv, (unsigned)cur_cap,
                           (int)cur_ma, bat_temp10, ts);
            publish_event(buf);

        } else if (g_last_charge_status == 2 && cur_charge == 0) {
            (void)snprintf(buf, sizeof(buf),
                           "{\"e\":\"charging-stopped\","
                           "\"r\":{\"v\":%u,\"c\":%u,\"ch\":0,"
                           "\"i\":%d,\"t\":[%d,-1]},"
                           "\"ts\":%llu}",
                           (unsigned)cur_mv, (unsigned)cur_cap,
                           (int)cur_ma, bat_temp10, ts);
            publish_event(buf);
        }

        g_last_charge_status = cur_charge;
    }

    /* --- Significant voltage change (>BATTERY_DELTA_MV) --- */
    delta = (cur_mv > g_last_batt_mv)
            ? (cur_mv - g_last_batt_mv)
            : (g_last_batt_mv - cur_mv);

    if (delta >= BATTERY_DELTA_MV) {
        ts = (unsigned long long)time(NULL);
        cur_ma     = SH366102_Read_chg_current();
        bat_temp10 = (int)(SH366102_ReadTemperature(SH366102_Address) * 10.0f);
        (void)snprintf(buf, sizeof(buf),
                       "{\"e\":\"battery\","
                       "\"r\":{\"v\":%u,\"c\":%u,\"ch\":%u,"
                       "\"i\":%d,\"t\":[%d,-1]},"
                       "\"ts\":%llu}",
                       (unsigned)cur_mv,
                       (unsigned)cur_cap,
                       (unsigned)(cur_charge > 0 ? 1 : 0),
                       (int)cur_ma, bat_temp10,
                       ts);
        publish_event(buf);
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
