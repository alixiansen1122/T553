/**
 * mqtt_activity.c
 *
 * /a topic publisher — reports accumulated activity posture, mileage,
 * and resting breathing data to the cloud per the v1.0.3 protocol.
 *
 * Each data type is published as a separate JSON array message:
 *   [{"d":[static_s, walk_s, run_s, shake_s, total_active_s], "ts":<utc>}]
 *   [{"m":[start_ts, end_ts, dist_cm, track_cm],              "ts":<utc>}]
 *   [{"h":[breath_bpm, confidence],                           "ts":<utc>}]
 *
 * d[4] = d[1]+d[2]+d[3] (total active time, derived at publish time).
 *
 * Data sources:
 *   d[]  → paqichong predicted_class (LSM6DSOW 6-axis NN inference)
 *           Bucket mapping: 0=Static, 1=Walk, 2=Run, 3-9=Shake/Other
 *   m[]  → STORAGE_STEP_COUNT (NV) × step_length estimate (65 cm avg)
 *   h[]  → mqtt_activity_set_breath() setter (injected by sensor subsystem)
 *
 * Integration points:
 *   - Call mqtt_activity_on_posture() from paqichong_data_process.c
 *     every prediction sample (~90 ms).
 *   - Call mqtt_activity_set_breath(bpm, conf) when breathing algo outputs.
 *   - Call mqtt_activity_publish(false) from a periodic timer (e.g. every 5 min).
 */

#include "mqtt_activity.h"
#include "mqtt_service.h"
#include "dev_storage.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>         /* time(NULL) for UTC timestamp */

/* ------------------------------------------------------------------ */
/*  Configuration                                                       */
/* ------------------------------------------------------------------ */
/* Average step length in cm (used to estimate distance from steps)    */
#define ACTIVITY_STEP_LEN_CM     65u

/* Number of posture buckets tracked internally (d_s[4] is derived, not stored) */
#define ACTIVITY_D_BUCKETS       4

/* Bucket index assignments (map paqichong class to d[] index)
 * Protocol d[]: [static_s, walk_s, run_s, shake_s, total_active_s]
 *   d[0] = static time          (class 0)
 *   d[1] = walking time         (class 1)
 *   d[2] = running time         (class 2)
 *   d[3] = shaking/other time   (class 3)
 *   d[4] = total active time    = d[1] + d[2] + d[3]  (derived, not stored)
 *
 * Classes 4-9 from paqichong (unclassified) are mapped to d[3] (shake/other). */
#define POSTURE_IDX_STATIC       0
#define POSTURE_IDX_WALK         1
#define POSTURE_IDX_RUN          2
#define POSTURE_IDX_SHAKE        3  /* also catches paqichong class 4-9 */

/* Sampling interval assumed by caller (paqichong fires every 90 ms)  */
#define ACTIVITY_SAMPLE_MS       90u

/* No-breath sentinel: breath not yet set                               */
#define BREATH_BPM_INVALID       (-1)

/* Activity payload JSON buffer size                                    */
#define ACTIVITY_JSON_BUF        256

/* ------------------------------------------------------------------ */
/*  Internal state                                                      */
/* ------------------------------------------------------------------ */
static osMutexId_t  g_act_mutex            = NULL;

/* d[] accumulators: sample counts per posture bucket                  */
static uint32_t     g_posture_ticks[ACTIVITY_D_BUCKETS];

/* m[] accumulators                                                     */
static uint64_t     g_period_start_ts      = 0;   /* UTC seconds */
static uint32_t     g_step_count_at_start  = 0;   /* steps at period start */

/* h[] latest breath reading                                            */
static volatile int g_breath_bpm           = BREATH_BPM_INVALID;
static volatile int g_breath_confidence    = 0;

/* has any posture data arrived this period?                            */
static volatile bool g_has_data            = false;

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/** Map paqichong class (0-9) to d[] bucket index */
static int posture_to_bucket(int64_t cls)
{
    switch (cls) {
    case 0:  return POSTURE_IDX_STATIC;
    case 1:  return POSTURE_IDX_WALK;
    case 2:  return POSTURE_IDX_RUN;
    default: return POSTURE_IDX_SHAKE;   /* class 3-9 → shake/other */
    }
}

/** Read step count from NV storage (returns 0 on failure) */
static uint32_t read_step_count(void)
{
    uint32_t steps = 0;
    (void)watch_storage_get(STORAGE_STEP_COUNT, &steps, (uint32_t)sizeof(steps));
    return steps;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

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
    /*
     * v1.0.3 protocol: each data type is published as a separate JSON array
     * to the /a topic.  Format per data type:
     *
     *   d: [{"d":[static_s, walk_s, run_s, shake_s, activity_s], "ts": <utc>}]
     *   m: [{"m":[start_ts, end_ts, dist_m×100, track_m×100],    "ts": <utc>}]
     *   h: [{"h":[bpm, confidence],                               "ts": <utc>}]
     *
     * d[] and m[] are always published together per period.
     * h[] is only published when a valid breathing measurement is available.
     */
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
    g_has_data = false;

    (void)osMutexRelease(g_act_mutex);

    /* Wall-clock end timestamp */
    now_ts = (unsigned long long)time(NULL);
    end_ts = now_ts;
    if (start_ts == 0u) {
        /* No GNSS time: estimate period start as 60s ago */
        start_ts = (end_ts > 60u) ? (end_ts - 60u) : 0u;
    }

    /* Mileage: step delta × step length */
    step_now   = read_step_count();
    step_delta = (step_now >= g_step_count_at_start)
                 ? (step_now - g_step_count_at_start)
                 : step_now;                   /* handles counter wrap */
    dist_cm    = step_delta * ACTIVITY_STEP_LEN_CM;
    track_cm   = dist_cm;                      /* no separate track data */
    g_step_count_at_start = step_now;

    /* Convert sample counts to whole seconds */
    for (i = 0; i < ACTIVITY_D_BUCKETS; i++) {
        d_s[i] = (ticks[i] * ACTIVITY_SAMPLE_MS) / 1000u;
    }
    /* d[4] = total active time = walk + run + shake (protocol semantics) */
    {
        uint32_t d_active = d_s[POSTURE_IDX_WALK]
                          + d_s[POSTURE_IDX_RUN]
                          + d_s[POSTURE_IDX_SHAKE];

    /* ---- Publish 1: d[] posture data -------------------------------- */
    /* Protocol: [{"d":[s,w,r,sh,act],"ts":<utc>}]                       */
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
    /* Protocol: [{"m":[start,end,dist×100,track×100],"ts":<utc>}]        */
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
    /* Protocol: [{"h":[bpm,confidence],"ts":<utc>}]                      */
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

