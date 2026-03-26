#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Topic suffix definitions
#define MQTT_TOPIC_SUFFIX_CMD      "c"
#define MQTT_TOPIC_SUFFIX_RESP     "r"
#define MQTT_TOPIC_SUFFIX_EVENT    "e"
#define MQTT_TOPIC_SUFFIX_GPS      "g"
#define MQTT_TOPIC_SUFFIX_ACTIVITY "a"

// Message callback type. The topic/payload memory is only valid during the callback.
typedef void (*mqtt_msg_callback_t)(const char *topic, const void *payload, int payload_len);

// Publish return codes
#define MQTT_SERVICE_OK             0
#define MQTT_SERVICE_ERR_NOT_INIT  (-1)
#define MQTT_SERVICE_ERR_PARAM     (-2)
#define MQTT_SERVICE_ERR_TOO_LONG  (-3)
#define MQTT_SERVICE_ERR_NULL_DATA (-4)
#define MQTT_SERVICE_ERR_SUFFIX    (-5)
#define MQTT_SERVICE_ERR_QUEUE     (-6)
#define MQTT_SERVICE_ERR_ALLOC     (-7)

// Initialize MQTT Service
void mqtt_service_init(void);

// Gracefully shut down the MQTT service: stop the task, disconnect, and release resources.
void mqtt_service_deinit(void);

// Queue a message for publish to a specific suffix (for example "r", "e", "a").
int mqtt_service_publish(const char *suffix, const void *payload, int payload_len, int qos);

// Check if MQTT is currently connected
bool mqtt_service_is_connected(void);

// Register callback for incoming commands (from /c topic)
void mqtt_service_register_cmd_callback(mqtt_msg_callback_t cb);

// Register callback for ALL incoming messages (any subscribed topic)
void mqtt_service_register_msg_callback(mqtt_msg_callback_t cb);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_SERVICE_H */
