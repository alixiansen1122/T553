#ifndef MQTT_EVENT_H
#define MQTT_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* MQTT_EVENT_H */
