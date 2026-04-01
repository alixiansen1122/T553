/**
 * mqtt_cmd_handler.c
 *
 * MQTT command handler — dispatches incoming /c-topic JSON commands and
 * publishes responses to the /r topic.
 *
 * JSON parsing strategy: lightweight strstr/sscanf — no third-party library.
 * JSON building strategy: direct snprintf — all response schemas are fixed.
 *
 * Command implementation status:
 *   P2  (implemented): v, b, di, ?, l/l0/l1, network.status, lte.auto_reconnect,
 *                       sys.reboot, sys.poweroff, factory-reset
 *   P3  (stub → e:-99): fence, beacon, audio, alert, epo, gnss.*, dfu, settings,
 *                        power_saving, shock_limit, dir, sec.format, usage.*, debug.*
 */

#include "mqtt_cmd_handler.h"
#include "mqtt_service.h"
#include "dev_storage.h"
#include "sh366102.h"
#include "modem_ctrl.h"
#include "watch_version.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  Platform externals                                                  */
/* ------------------------------------------------------------------ */
extern uint8_t g_ipv4_status;                       /* lwip_volte_adapter.c */
extern uint8_t g_chargestatus;                      /* sh366102.c           */
extern uint32_t mcu_os_sys_reset(uint8_t *para, uint32_t para_len); /* app_at_process.c */

/* ------------------------------------------------------------------ */
/*  Module-private state                                                */
/* ------------------------------------------------------------------ */
#define CMD_RESP_BUF_SIZE  512
#define CMD_NAME_MAX_LEN    32
#define CMD_ID_MAX_LEN      48    /* "i" session ID field max length */
#define IMEI_BUF_LEN        20
#define SN_BUF_LEN          32
#define REBOOT_DELAY_MS    500

static volatile bool g_live_enabled = false;

/* ------------------------------------------------------------------ */
/*  Lightweight JSON helpers                                            */
/* ------------------------------------------------------------------ */

/**
 * Extract the string value of the *first* occurrence of "key":"..." in json.
 * Returns true on success, false if the key is not found or out-buffer is too small.
 */
static bool json_get_str(const char *json, const char *key,
                         char *out, int out_sz)
{
    char needle[CMD_NAME_MAX_LEN + 6];  /* "key":" */
    const char *p;
    const char *end;
    int len;

    if (json == NULL || key == NULL || out == NULL || out_sz <= 0) {
        return false;
    }
    (void)snprintf(needle, sizeof(needle), "\"%s\":\"", key);
    p = strstr(json, needle);
    if (p == NULL) {
        return false;
    }
    p += strlen(needle);
    end = strchr(p, '"');
    if (end == NULL) {
        return false;
    }
    len = (int)(end - p);
    if (len < 0 || len >= out_sz) {
        return false;
    }
    (void)memcpy(out, p, (size_t)len);
    out[len] = '\0';
    return true;
}

/**
 * Extract the integer value of "key":N in json.
 * Returns true on success.
 */
static bool json_get_int(const char *json, const char *key, int *out_val)
{
    char needle[CMD_NAME_MAX_LEN + 4];  /* "key": */
    const char *p;

    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }
    (void)snprintf(needle, sizeof(needle), "\"%s\":", key);
    p = strstr(json, needle);
    if (p == NULL) {
        return false;
    }
    p += strlen(needle);
    /* skip optional space */
    while (*p == ' ') {
        p++;
    }
    if (*p == '\0') {
        return false;
    }
    *out_val = (int)strtol(p, NULL, 10);
    return true;
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
/*  Command handler function type                                       */
/* ------------------------------------------------------------------ */
typedef void (*cmd_handler_fn_t)(const char *cmd,
                                 const char *json,
                                 char       *resp,
                                 int         resp_sz);

/* ------------------------------------------------------------------ */
/*  P2 — Implemented command handlers                                   */
/* ------------------------------------------------------------------ */

/* 1. Version: {"c":"v"} → {"c":"v","r":"<ver>","v52":"n/a"} */
static void cmd_version(const char *cmd, const char *json,
                        char *resp, int resp_sz)
{
    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":\"%s\",\"v52\":\"n/a\"}",
                   cmd, VERSION_STRING);
}

/* 2. Battery: {"c":"b"} → {"c":"b","r":{"v":3970,"c":49,"ch":1}} */
static void cmd_battery(const char *cmd, const char *json,
                        char *resp, int resp_sz)
{
    uint16_t voltage   = get_battery_voltage();
    uint8_t  capacity  = get_battery_level();
    uint8_t  ch_state  = g_chargestatus;  /* 0=discharging,1=charging,2=full */

    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{\"v\":%u,\"c\":%u,\"ch\":%u}}",
                   cmd, (unsigned)voltage, (unsigned)capacity,
                   (unsigned)(ch_state > 0 ? 1 : 0));
}

/* 3. Device info: {"c":"di"} → {"c":"di","r":{...}} */
static void cmd_device_info(const char *cmd, const char *json,
                            char *resp, int resp_sz)
{
    char imei[IMEI_BUF_LEN + 1] = {0};
    char sn[SN_BUF_LEN + 1]     = {0};
    int  ret;

    (void)json;
    ret = watch_storage_get(STORAGE_IMEI, imei, IMEI_BUF_LEN);
    if (ret != 0) {
        imei[0] = '\0';
    }
    imei[IMEI_BUF_LEN] = '\0';

    ret = watch_storage_get(STORAGE_PSN, sn, SN_BUF_LEN);
    if (ret != 0) {
        sn[0] = '\0';
    }
    sn[SN_BUF_LEN] = '\0';

    /*
     * Protocol requires 9 fields: iccid, imei, imsi, v9, v5, mac, vg, sn, model
     *
     * Populated now:   imei (STORAGE_IMEI), sn (STORAGE_PSN), v9 (VERSION_STRING)
     * Empty stubs:
     *   iccid  — TODO: read from modem via AT+ICCID or ril_get_iccid()
     *   imsi   — TODO: read from modem via AT+CIMI or ril_get_imsi()
     *   mac    — TODO: read BLE MAC via bt_get_local_addr() if BLE supported
     *   vg     — TODO: read GNSS module FW version via gnss_get_version()
     */
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
                   "}}",
                   cmd, imei, VERSION_STRING, sn);
}


/* 4. Dump: {"c":"?"} → {"c":"?","r":{...}} */
static void cmd_dump(const char *cmd, const char *json,
                     char *resp, int resp_sz)
{
    uint32_t tick_count = osKernelGetTickCount();
    uint32_t tick_freq  = osKernelGetTickFreq();
    uint32_t uptime_sec = (tick_freq > 0) ? (tick_count / tick_freq) : 0;
    uint16_t voltage    = get_battery_voltage();
    uint8_t  capacity   = get_battery_level();
    uint8_t  ch_state   = g_chargestatus;

    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{"
                   "\"fix\":0,"
                   "\"f\":0,"
                   "\"bv\":%u,"
                   "\"bat_cap\":%u,"
                   "\"ch\":%u,"
                   "\"uptime\":%u,"
                   "\"rst\":\"\""
                   "}}",
                   cmd,
                   (unsigned)voltage,
                   (unsigned)capacity,
                   (unsigned)(ch_state > 0 ? 1 : 0),
                   (unsigned)uptime_sec);
}

/* 5. Live query: {"c":"l"} → {"c":"l","r":0/1} */
static void cmd_live_query(const char *cmd, const char *json,
                           char *resp, int resp_sz)
{
    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":%d}",
                   cmd, g_live_enabled ? 1 : 0);
}

/* 6. Live on: {"c":"l1"} → {"c":"l1","r":0} */
static void cmd_live_on(const char *cmd, const char *json,
                        char *resp, int resp_sz)
{
    (void)json;
    g_live_enabled = true;
    printf("[MQTT CMD] live mode ON\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 7. Live off: {"c":"l0"} → {"c":"l0","r":0} */
static void cmd_live_off(const char *cmd, const char *json,
                         char *resp, int resp_sz)
{
    (void)json;
    g_live_enabled = false;
    printf("[MQTT CMD] live mode OFF\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 8. Network status: {"c":"network.status"} → {"c":"network.status","r":{...}} */
static void cmd_net_status(const char *cmd, const char *json,
                           char *resp, int resp_sz)
{
    uint8_t lte_reg = modem_get_reg_status();
    uint8_t lte_ip  = g_ipv4_status;
    int     aws_ok  = mqtt_service_is_connected() ? 1 : 0;
    int     lte_ok  = (lte_reg && lte_ip) ? 1 : 0;

    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":{\"lte_state\":%d,\"aws_state\":%d}}",
                   cmd, lte_ok, aws_ok);
}

/* 9. LTE auto-reconnect: {"c":"lte.auto_reconnect","p":0/1} → {"c":"...","r":0} */
static void cmd_lte_reconnect(const char *cmd, const char *json,
                              char *resp, int resp_sz)
{
    int val = 0;
    (void)json_get_int(json, "p", &val);
    /* Setting stored but enforcement is handled by the network stack */
    printf("[MQTT CMD] lte.auto_reconnect = %d (stored, enforcement TBD)\n", val);
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ------------------------------------------------------------------ */
/*  Reboot helper task — publishes response BEFORE rebooting            */
/* ------------------------------------------------------------------ */
static void reboot_task(void *arg)
{
    (void)arg;
    osDelay(REBOOT_DELAY_MS);
    printf("[MQTT CMD] sys.reboot executing\n");
    (void)mcu_os_sys_reset(NULL, 0);
    /* Should not reach here */
    osThreadExit();
}

/* 10. Reboot: {"c":"sys.reboot"} → {"c":"sys.reboot","r":0}, then reboot */
static void cmd_reboot(const char *cmd, const char *json,
                       char *resp, int resp_sz)
{
    osThreadAttr_t attr = {0};
    char sid[CMD_ID_MAX_LEN];

    /* Extract session ID before publishing (dispatch can't inject for us) */
    if (json_get_str(json, "i", sid, sizeof(sid))) {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":0,\"i\":\"%s\"}", cmd, sid);
    } else {
        (void)snprintf(resp, (size_t)resp_sz,
                       "{\"c\":\"%s\",\"r\":0}", cmd);
    }
    /* Publish response first, then schedule reboot in a separate task */
    publish_response(resp);
    resp[0] = '\0';   /* prevent double-publish by the main router */

    attr.name       = "mqtt_reboot";
    attr.stack_size = 512;
    attr.priority   = osPriorityNormal;
    if (osThreadNew(reboot_task, NULL, &attr) == NULL) {
        printf("[MQTT CMD] WARNING: failed to create reboot task\n");
    }
}

/* 11. Poweroff (stub — poweroff API TBD): {"c":"sys.poweroff"} → {"c":"...","r":0} */
static void cmd_poweroff(const char *cmd, const char *json,
                         char *resp, int resp_sz)
{
    (void)json;
    printf("[MQTT CMD] sys.poweroff requested (implementation TBD)\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* 12. Factory reset (stub — behaviour TBD): {"c":"factory-reset"} → {"c":"...","r":0} */
static void cmd_factory_reset(const char *cmd, const char *json,
                              char *resp, int resp_sz)
{
    (void)json;
    printf("[MQTT CMD] factory-reset requested (implementation TBD)\n");
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"r\":0}", cmd);
}

/* ------------------------------------------------------------------ */
/*  P3 — Generic stub (returns e:-99 = not yet implemented)            */
/* ------------------------------------------------------------------ */
static void cmd_stub(const char *cmd, const char *json,
                     char *resp, int resp_sz)
{
    (void)json;
    (void)snprintf(resp, (size_t)resp_sz,
                   "{\"c\":\"%s\",\"e\":-99}", cmd);
}

/* ------------------------------------------------------------------ */
/*  Dispatch table                                                      */
/* ------------------------------------------------------------------ */
typedef struct {
    const char       *cmd;
    cmd_handler_fn_t  handler;
} mqtt_cmd_entry_t;

static const mqtt_cmd_entry_t g_cmd_table[] = {
    /* --- P2: implemented --- */
    { "v",                  cmd_version       },
    { "b",                  cmd_battery       },
    { "di",                 cmd_device_info   },
    { "?",                  cmd_dump          },
    { "l",                  cmd_live_query    },
    { "l1",                 cmd_live_on       },
    { "l0",                 cmd_live_off      },
    { "network.status",     cmd_net_status    },
    { "lte.auto_reconnect", cmd_lte_reconnect },
    { "sys.reboot",         cmd_reboot        },
    { "sys.poweroff",       cmd_poweroff      },
    { "factory-reset",      cmd_factory_reset },

    /* --- P3: fence --- */
    { "f",   cmd_stub }, { "f1",  cmd_stub }, { "f0",  cmd_stub },
    { "fe",  cmd_stub }, { "fe1", cmd_stub }, { "fe0", cmd_stub },
    { "fl",  cmd_stub }, { "fa",  cmd_stub }, { "fd",  cmd_stub },

    /* --- P3: beacon --- */
    { "k",   cmd_stub }, { "k1",  cmd_stub }, { "k0",  cmd_stub },
    { "kl",  cmd_stub }, { "ka",  cmd_stub }, { "kd",  cmd_stub },
    { "ke1", cmd_stub }, { "ke0", cmd_stub }, { "kc",  cmd_stub },

    /* --- P3: audio --- */
    { "audio.list",       cmd_stub }, { "audio.play",   cmd_stub },
    { "audio.remove",     cmd_stub }, { "audio.hash",   cmd_stub },

    /* --- P3: alert profiles --- */
    { "ap",               cmd_stub }, { "ap.update",    cmd_stub },
    { "ap.exec_by_idx",   cmd_stub }, { "ap.exec_custom", cmd_stub },

    /* --- P3: EPO --- */
    { "epo.enable",       cmd_stub }, { "epo.status",   cmd_stub },
    { "epo.reset",        cmd_stub },

    /* --- P3: GNSS --- */
    { "gnss.status",      cmd_stub }, { "gnss.diag",    cmd_stub },
    { "gnss.nmea.upload", cmd_stub },

    /* --- P3: FOTA --- */
    { "dfu",              cmd_stub },

    /* --- P3: settings --- */
    { "settings.get",     cmd_stub }, { "settings.set", cmd_stub },
    { "power_saving",     cmd_stub }, { "shock_limit",  cmd_stub },

    /* --- P3: other --- */
    { "dir",              cmd_stub }, { "sec.format",   cmd_stub },
    { "usage.stats",      cmd_stub }, { "usage.stats.clear",   cmd_stub },
    { "usage.log.enabled",cmd_stub }, { "usage.log.clear",     cmd_stub },

    /* --- P3: debug --- */
    { "debug.event_gnss", cmd_stub }, { "debug.file",   cmd_stub },
    { "debug.set",        cmd_stub }, { "log.cat",      cmd_stub },
    { "debug.sensor_rt",  cmd_stub },
};

#define CMD_TABLE_SIZE ((int)(sizeof(g_cmd_table) / sizeof(g_cmd_table[0])))

/* ------------------------------------------------------------------ */
/*  Core dispatch: called by mqtt_service cmd callback                  */
/* ------------------------------------------------------------------ */
static void mqtt_cmd_dispatch(const char *topic,
                               const void *payload, int payload_len)
{
    char json[512];
    char cmd_name[CMD_NAME_MAX_LEN];
    char session_id[CMD_ID_MAX_LEN]; /* "i" field for request/response correlation */
    char resp[CMD_RESP_BUF_SIZE];
    int  copy_len;
    int  i;

    (void)topic;

    if (payload == NULL || payload_len <= 0) {
        return;
    }

    /* Copy payload to a NUL-terminated local buffer */
    copy_len = (payload_len < (int)sizeof(json) - 1)
               ? payload_len : (int)sizeof(json) - 1;
    (void)memcpy(json, payload, (size_t)copy_len);
    json[copy_len] = '\0';

    /* Extract command name */
    if (!json_get_str(json, "c", cmd_name, sizeof(cmd_name))) {
        printf("[MQTT CMD] No 'c' field in payload: %s\n", json);
        return;
    }

    /* Extract optional session ID "i" for response correlation */
    if (!json_get_str(json, "i", session_id, sizeof(session_id))) {
        session_id[0] = '\0';   /* No session ID — omit in response */
    }

    printf("[MQTT CMD] Received cmd='%s' i='%s'\n", cmd_name, session_id);

    /* Look up and dispatch */
    resp[0] = '\0';
    for (i = 0; i < CMD_TABLE_SIZE; i++) {
        if (strcmp(g_cmd_table[i].cmd, cmd_name) == 0) {
            g_cmd_table[i].handler(cmd_name, json, resp, sizeof(resp));
            /* cmd_reboot may have already called publish_response and cleared resp */
            if (resp[0] != '\0') {
                /* Append "i" field BEFORE closing brace if session ID is present */
                if (session_id[0] != '\0') {
                    int rlen = (int)strlen(resp);
                    /* resp ends with '}' — insert ,"i":"..." before it */
                    if (rlen > 0 && resp[rlen - 1] == '}' &&
                        rlen + (int)sizeof(session_id) + 8 < CMD_RESP_BUF_SIZE) {
                        char id_suffix[CMD_ID_MAX_LEN + 10];
                        (void)snprintf(id_suffix, sizeof(id_suffix),
                                       ",\"i\":\"%s\"}", session_id);
                        resp[rlen - 1] = '\0';          /* strip trailing } */
                        (void)strncat(resp, id_suffix,
                                      (size_t)(CMD_RESP_BUF_SIZE - rlen - 1));
                    }
                }
                publish_response(resp);
            }
            return;
        }
    }

    /* Unknown command */
    printf("[MQTT CMD] Unknown command: '%s'\n", cmd_name);
    if (session_id[0] != '\0') {
        (void)snprintf(resp, sizeof(resp),
                       "{\"c\":\"%s\",\"e\":-1,\"i\":\"%s\"}",
                       cmd_name, session_id);
    } else {
        (void)snprintf(resp, sizeof(resp),
                       "{\"c\":\"%s\",\"e\":-1}", cmd_name);
    }
    publish_response(resp);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void mqtt_cmd_handler_init(void)
{
    mqtt_service_register_cmd_callback(mqtt_cmd_dispatch);
    printf("[MQTT CMD] Command handler registered (%d commands)\n",
           CMD_TABLE_SIZE);
}

bool mqtt_cmd_handler_live_is_on(void)
{
    return g_live_enabled;
}
