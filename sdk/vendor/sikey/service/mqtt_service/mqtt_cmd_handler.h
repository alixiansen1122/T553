#ifndef MQTT_CMD_HANDLER_H
#define MQTT_CMD_HANDLER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the MQTT command handler.
 *
 * Registers itself as the /c-topic callback via mqtt_service_register_cmd_callback().
 * Must be called after mqtt_service_init().
 */
void mqtt_cmd_handler_init(void);

/**
 * @brief Query the current Live-mode state.
 *
 * @return true  if Live mode is enabled (device should report GNSS at high frequency).
 * @return false otherwise.
 */
bool mqtt_cmd_handler_live_is_on(void);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_CMD_HANDLER_H */
