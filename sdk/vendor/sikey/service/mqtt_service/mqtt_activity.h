#ifndef MQTT_ACTIVITY_H
#define MQTT_ACTIVITY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Posture class IDs (mirror paqichong predicted_class)               */
/*  Protocol /a "d": [static, walk, run, shake, total_active]         */
/*    d[0..3] tracked; d[4]=d[1]+d[2]+d[3] (derived at publish)       */
/* ------------------------------------------------------------------ */

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

#ifdef __cplusplus
}
#endif

#endif /* MQTT_ACTIVITY_H */
