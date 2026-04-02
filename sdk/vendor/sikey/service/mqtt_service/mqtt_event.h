#ifndef MQTT_EVENT_H
#define MQTT_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================== */
/*  Event publisher — /e topic                                          */
/* ================================================================== */

/**
 * @brief Initialize the MQTT event subsystem.
 *
 * Publishes the "boot" event immediately.
 * Must be called after mqtt_service_init() returns successfully.
 */
void mqtt_event_init(void);

/**
 * @brief Publish a gnss-fix event.
 *
 * Call this when a valid GNSS fix is obtained (status == 'A').
 *
 * @param lat_deg  Latitude in decimal degrees  (positive = N, negative = S)
 * @param lon_deg  Longitude in decimal degrees (positive = E, negative = W)
 */
void mqtt_event_gnss_fix(double lat_deg, double lon_deg);

/**
 * @brief Publish a gnss-lost event.
 *
 * Call this when the GNSS fix is lost after previously being valid.
 */
void mqtt_event_gnss_lost(void);

/**
 * @brief Query whether GNSS currently has a valid fix.
 */
bool mqtt_event_gnss_is_fixed(void);

/**
 * @brief Get last known latitude (decimal degrees).
 */
double mqtt_event_gnss_last_lat(void);

/**
 * @brief Get last known longitude (decimal degrees).
 */
double mqtt_event_gnss_last_lon(void);

/**
 * @brief Check and (if necessary) publish battery-level change events.
 *
 * Compares current battery state to the last-published snapshot.
 * Publish: "battery" (>5% voltage change), "charging-started",
 *          "charging-stopped", "charging-completed".
 *
 * Call this periodically (e.g. every 60 s from a sensor-manager tick).
 */
void mqtt_event_battery_poll(void);

/**
 * @brief Publish a "shutdown" event just before the device powers off.
 */
void mqtt_event_shutdown(void);

/* ------------------------------------------------------------------ */
/*  P3 Event APIs — call from respective subsystems when ready          */
/* ------------------------------------------------------------------ */

/** Fence state change event.
 * @param event_name  One of: "fence-approaching","fence-breach","fence-secured","fence-escaped"
 * @param lat_deg     Current latitude (decimal degrees)
 * @param lon_deg     Current longitude (decimal degrees)
 * @param fence_id    Fence identifier string
 * @param from_state  Previous state code (3=secured,4=approaching,5=breach,6=escaped)
 * @param to_state    New state code */
void mqtt_event_fence(const char *event_name, double lat_deg, double lon_deg,
                      const char *fence_id, int from_state, int to_state);

/** Beacon proximity event.
 * @param type    0=allow(safe), 1=forbid(dangerous)
 * @param action  0=entered range, 1=left range
 * @param addr    BLE MAC address string e.g. "C3000057F2A3" */
void mqtt_event_beacon(int type, int action, const char *addr);

/** Module status event (BLE channel). */
void mqtt_event_modules_status(int lte_state, int aws_state,
                                int gnss_state, int ble_state);

/** Power saving LTE/GNSS off event. */
void mqtt_event_lte_gnss_off(void);

/** Hourly usage statistics event. */
void mqtt_event_usage(int lte_conn, int lte_on_cnt, int lte_off_cnt,
                      int aws_conn, int aws_on_cnt, int aws_off_cnt,
                      int gnss_state, int gnss_on_cnt, int gnss_off_cnt,
                      int bat_drain_pct, int ps_mode);

/** Power saving suggestion events. */
void mqtt_event_suggest_ps(bool enable);

/** EPO download event.
 * @param constellation 0=GPS, 1=GALILEO, 2=BDS, 3=GPS_GLONASS */
void mqtt_event_epo_download(int constellation);

/** Charging abnormal event. */
void mqtt_event_charging_abnormal(void);

/** Charging error event.
 * @param reg08  Register 0x08 value
 * @param reg09  Register 0x09 value */
void mqtt_event_charging_error(uint8_t reg08, uint8_t reg09);

/* ================================================================== */
/*  Activity reporter — /a topic (merged from mqtt_activity)            */
/* ================================================================== */

/**
 * @brief Update the activity posture accumulator.
 *
 * Call this every time paqichong produces a new prediction.
 * @param posture_class  0-9 predicted class from paqichong model:
 *                       0=Static, 1=Walk, 2=Run, 3=Shake, others=grouped as Activity
 * @param utc_ts         Current UTC timestamp (seconds since epoch)
 */
void mqtt_activity_on_posture(int64_t posture_class, uint64_t utc_ts);

/**
 * @brief Inject resting breathing data (v1.0.3 "h" field).
 *
 * Call this whenever a new breathing measurement is available from the
 * sensor subsystem or algorithm. Thread-safe.
 *
 * @param bpm         Breathing rate in breaths-per-minute (integer)
 * @param confidence  Confidence score 0-100 (%)
 */
void mqtt_activity_set_breath(int bpm, int confidence);

/**
 * @brief Publish a complete /a activity payload to the cloud.
 *
 * Serialises accumulated d[], m[], and latest h[] into JSON and sends
 * to MQTT_TOPIC_SUFFIX_ACTIVITY.  Resets d[] and m[] accumulators.
 *
 * @param force_send  If true, send even if no new data has been collected.
 */
void mqtt_activity_publish(bool force_send);

/**
 * @brief Initialize the activity reporting module.
 *
 * Sets up internal state.  Call after mqtt_service_init().
 */
void mqtt_activity_init(void);

/* ================================================================== */
/*  GNSS track publisher — /g topic (merged from mqtt_gnss)             */
/* ================================================================== */

/**
 * @brief A single GNSS point for /g topic protobuf upload.
 *
 * Matches the proto3 schema:
 *   message GNSSPoint { int32 lat=1; int32 lon=2; int32 ts=3; int32 quality=4; int32 alt=5; }
 */
typedef struct {
    int32_t lat;        /**< Latitude  × 1,000,000 */
    int32_t lon;        /**< Longitude × 1,000,000 */
    int32_t ts;         /**< UTC timestamp (seconds) */
    int32_t quality;    /**< Fix quality (0=none,1=GPS,2=DGPS,...) */
    int32_t alt;        /**< Altitude in cm */
} mqtt_gnss_point_t;

/**
 * @brief Publish a batch of GNSS points to the /g topic as ProtoBuf.
 *
 * Encodes a GNSSTrack { repeated GNSSPoint points = 1; } message
 * using a lightweight hand-rolled proto3 encoder (no nanopb dependency).
 *
 * @param points     Array of GNSS points.
 * @param count      Number of points in the array (1..32).
 * @return 0 on success, negative on error.
 */
int mqtt_gnss_publish(const mqtt_gnss_point_t *points, int count);

/**
 * @brief Convenience: publish a single GNSS point.
 */
int mqtt_gnss_publish_single(int32_t lat, int32_t lon, int32_t ts,
                              int32_t quality, int32_t alt);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_EVENT_H */
