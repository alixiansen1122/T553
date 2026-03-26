#include "mqtt_service.h"
#include "mqtt_service_config.h"
#include "MQTTClient.h"
#include "ohos_init.h"
#include "dev_storage.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define IMEI_LEN WATCH_IMEI_LEN

typedef enum {
    MQTT_CONN_STATE_INIT = 0,
    MQTT_CONN_STATE_CONNECTING,
    MQTT_CONN_STATE_READY,
    MQTT_CONN_STATE_DISCONNECTED,
} mqtt_connection_state_t;

typedef struct {
    char suffix[MQTT_SERVICE_SUFFIX_MAX_LEN];
    int payload_len;
    int qos;
    uint8_t payload[MQTT_SERVICE_PAYLOAD_MAX_LEN];
} mqtt_publish_request_t;

static MQTTClient g_mqtt_client = NULL;
static char g_device_imei[IMEI_LEN + 1] = {0};
static volatile mqtt_msg_callback_t g_cmd_callback = NULL;
static volatile mqtt_msg_callback_t g_msg_callback = NULL;
static osThreadId_t g_mqtt_task_id = NULL;
static osMessageQueueId_t g_publish_queue = NULL;
static volatile mqtt_connection_state_t g_mqtt_state = MQTT_CONN_STATE_INIT;
static volatile bool g_mqtt_stop = false;
static uint32_t g_backoff_ms = MQTT_SERVICE_BACKOFF_INITIAL_MS;
static osSemaphoreId_t g_mqtt_exit_sem = NULL;

static int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message);
static void connectionLost(void *context, char *cause);
static void deliveryComplete(void *context, MQTTClient_deliveryToken dt);
static void mqtt_backoff_reset(void);

static bool mqtt_service_is_valid_qos(int qos)
{
    return (qos >= 0 && qos <= 2);
}

static bool mqtt_service_is_ready_imei(const char *imei)
{
    size_t i;

    if (imei == NULL || imei[0] == '\0') {
        return false;
    }

    for (i = 0; imei[i] != '\0'; ++i) {
        if (!isprint((unsigned char)imei[i]) || imei[i] == '/') {
            return false;
        }
    }

    return i > 0;
}

static bool mqtt_service_load_imei(void)
{
    int ret;

    (void)memset(g_device_imei, 0, sizeof(g_device_imei));
    ret = watch_storage_get(STORAGE_IMEI, g_device_imei, IMEI_LEN);
    g_device_imei[IMEI_LEN] = '\0';
    if (ret != 0 || !mqtt_service_is_ready_imei(g_device_imei)) {
        printf("[MQTT] IMEI unavailable, retry later. ret=%d\n", ret);
        (void)memset(g_device_imei, 0, sizeof(g_device_imei));
        return false;
    }
    return true;
}

static bool mqtt_service_file_exists(const char *path)
{
    FILE *fp;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return false;
    }
    (void)fclose(fp);
    return true;
}

static bool mqtt_service_cert_files_ready(void)
{
    if (!mqtt_service_file_exists(MQTT_SERVICE_ROOT_CA_FILE)) {
        printf("[MQTT] Missing CA file: %s\n", MQTT_SERVICE_ROOT_CA_FILE);
        return false;
    }
    if (!mqtt_service_file_exists(MQTT_SERVICE_CLIENT_CRT_FILE)) {
        printf("[MQTT] Missing client cert: %s\n", MQTT_SERVICE_CLIENT_CRT_FILE);
        return false;
    }
    if (!mqtt_service_file_exists(MQTT_SERVICE_CLIENT_KEY_FILE)) {
        printf("[MQTT] Missing client key: %s\n", MQTT_SERVICE_CLIENT_KEY_FILE);
        return false;
    }
    return true;
}

// Topic building helper.
static int build_topic(char *topic_buf, size_t buf_len, const char *suffix)
{
    int written;

    if (topic_buf == NULL || buf_len == 0 || suffix == NULL || suffix[0] == '\0') {
        return -1;
    }

#if MQTT_SERVICE_USE_T_TOPIC_PREFIX
    written = snprintf(topic_buf, buf_len, "t/%s/%s", g_device_imei, suffix);
#else
    written = snprintf(topic_buf, buf_len, "%s/%s", g_device_imei, suffix);
#endif
    if (written < 0 || (size_t)written >= buf_len) {
        return -1;
    }
    return 0;
}

static bool mqtt_service_topic_equals(const char *topic_name, int topic_len, const char *expected_topic)
{
    size_t expected_len;

    if (topic_name == NULL || expected_topic == NULL) {
        return false;
    }

    expected_len = strlen(expected_topic);
    if (topic_len > 0) {
        return (size_t)topic_len == expected_len &&
               memcmp(topic_name, expected_topic, expected_len) == 0;
    }

    return strcmp(topic_name, expected_topic) == 0;
}

static int mqtt_service_create_client(void)
{
    int rc;

    if (g_mqtt_client != NULL) {
        return MQTTCLIENT_SUCCESS;
    }

    rc = MQTTClient_create(&g_mqtt_client, MQTT_SERVICE_BROKER_URI, g_device_imei,
                           MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[MQTT] Failed to create client, rc=%d\n", rc);
        g_mqtt_client = NULL;
        return rc;
    }

    rc = MQTTClient_setCallbacks(g_mqtt_client, NULL, connectionLost, messageArrived, deliveryComplete);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[MQTT] Failed to set callbacks, rc=%d\n", rc);
        MQTTClient_destroy(&g_mqtt_client);
        return rc;
    }

    return MQTTCLIENT_SUCCESS;
}

static void mqtt_service_mark_disconnected(void)
{
    g_mqtt_state = MQTT_CONN_STATE_DISCONNECTED;
}

// Callbacks for Paho MQTT
static int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char cmd_topic[MQTT_SERVICE_TOPIC_MAX_LEN];
    char topic_copy[MQTT_SERVICE_TOPIC_MAX_LEN];
    int copy_len;

    (void)context;

    if (build_topic(cmd_topic, sizeof(cmd_topic), MQTT_TOPIC_SUFFIX_CMD) != 0) {
        printf("[MQTT] Failed to build command topic for incoming message\n");
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
        return 1;
    }

    if (topicLen > 0) {
        copy_len = (topicLen < (int)sizeof(topic_copy) - 1) ? topicLen : (int)sizeof(topic_copy) - 1;
        (void)memcpy(topic_copy, topicName, (size_t)copy_len);
        topic_copy[copy_len] = '\0';
    } else if (topicName != NULL) {
        (void)snprintf(topic_copy, sizeof(topic_copy), "%s", topicName);
    } else {
        topic_copy[0] = '\0';
    }

    printf("[MQTT] Message arrived on topic: %s\n", topic_copy);

    // Dispatch to generic message callback (all topics)
    if (g_msg_callback != NULL) {
        g_msg_callback(topic_copy, message->payload, message->payloadlen);
    }

    // Dispatch to cmd-specific callback
    if (g_cmd_callback != NULL && mqtt_service_topic_equals(topicName, topicLen, cmd_topic)) {
        g_cmd_callback(topic_copy, message->payload, message->payloadlen);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

static void connectionLost(void *context, char *cause)
{
    (void)context;
    printf("[MQTT] Connection lost. Cause: %s\n", cause ? cause : "unknown");
    mqtt_backoff_reset();
    mqtt_service_mark_disconnected();
}

static void deliveryComplete(void *context, MQTTClient_deliveryToken dt)
{
    (void)context;
    printf("[MQTT] Message delivery complete, token: %d\n", dt);
}

static int mqtt_connect_internal(void)
{
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    char cmd_topic[MQTT_SERVICE_TOPIC_MAX_LEN];
    int rc;

    if (g_mqtt_client == NULL) {
        return MQTTCLIENT_FAILURE;
    }
    if (!mqtt_service_cert_files_ready()) {
        return MQTTCLIENT_FAILURE;
    }

    g_mqtt_state = MQTT_CONN_STATE_CONNECTING;

    conn_opts.keepAliveInterval = MQTT_SERVICE_KEEPALIVE_SEC;
    conn_opts.cleansession = 1;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;

    ssl_opts.trustStore = MQTT_SERVICE_ROOT_CA_FILE;
    ssl_opts.keyStore = MQTT_SERVICE_CLIENT_CRT_FILE;
    ssl_opts.privateKey = MQTT_SERVICE_CLIENT_KEY_FILE;
    ssl_opts.enableServerCertAuth = 1;
    ssl_opts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
#if MQTT_SERVICE_ENABLE_ALPN
    ssl_opts.protos = MQTT_SERVICE_ALPN_PTR;
    ssl_opts.protos_len = MQTT_SERVICE_ALPN_LEN;
#endif
    conn_opts.ssl = &ssl_opts;

    printf("[MQTT] Connecting to %s with ClientID %s\n", MQTT_SERVICE_BROKER_URI, g_device_imei);
    rc = MQTTClient_connect(g_mqtt_client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[MQTT] Failed to connect, rc=%d\n", rc);
        mqtt_service_mark_disconnected();
        return rc;
    }

    printf("[MQTT] Connected successfully\n");

    if (build_topic(cmd_topic, sizeof(cmd_topic), MQTT_TOPIC_SUFFIX_CMD) != 0) {
        printf("[MQTT] Failed to build command topic\n");
        (void)MQTTClient_disconnect(g_mqtt_client, 1000);
        mqtt_service_mark_disconnected();
        return MQTTCLIENT_FAILURE;
    }

    rc = MQTTClient_subscribe(g_mqtt_client, cmd_topic, MQTT_SERVICE_SUBSCRIBE_QOS);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[MQTT] Failed to subscribe to %s, rc=%d\n", cmd_topic, rc);
        (void)MQTTClient_disconnect(g_mqtt_client, 1000);
        mqtt_service_mark_disconnected();
        return rc;
    }

    g_mqtt_state = MQTT_CONN_STATE_READY;
    printf("[MQTT] Subscribed to %s\n", cmd_topic);
    return MQTTCLIENT_SUCCESS;
}

static void mqtt_service_check_and_mark_disconnected(void)
{
    if (g_mqtt_client == NULL || !MQTTClient_isConnected(g_mqtt_client)) {
        mqtt_service_mark_disconnected();
    }
}

static int mqtt_service_publish_internal(const mqtt_publish_request_t *req)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    char topic[MQTT_SERVICE_TOPIC_MAX_LEN];
    int rc;

    if (req == NULL || g_mqtt_state != MQTT_CONN_STATE_READY || g_mqtt_client == NULL) {
        return MQTTCLIENT_FAILURE;
    }
    if (build_topic(topic, sizeof(topic), req->suffix) != 0) {
        printf("[MQTT] Failed to build publish topic for suffix %s\n", req->suffix);
        return MQTTCLIENT_FAILURE;
    }

    pubmsg.payload = (void *)req->payload;
    pubmsg.payloadlen = req->payload_len;
    pubmsg.qos = req->qos;
    pubmsg.retained = 0;

    rc = MQTTClient_publishMessage(g_mqtt_client, topic, &pubmsg, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("[MQTT] Failed to publish to %s, rc=%d\n", topic, rc);
        mqtt_service_check_and_mark_disconnected();
        return rc;
    }

    if (req->qos > 0) {
        rc = MQTTClient_waitForCompletion(g_mqtt_client, token, MQTT_SERVICE_QOS_TIMEOUT_MS);
        if (rc != MQTTCLIENT_SUCCESS) {
            printf("[MQTT] Publish wait failed for %s, rc=%d\n", topic, rc);
            mqtt_service_check_and_mark_disconnected();
            return rc;
        }
    }

    printf("[MQTT] Published to %s\n", topic);
    return MQTTCLIENT_SUCCESS;
}

static void mqtt_backoff_reset(void)
{
    g_backoff_ms = MQTT_SERVICE_BACKOFF_INITIAL_MS;
}

static void mqtt_backoff_next(void)
{
    g_backoff_ms *= MQTT_SERVICE_BACKOFF_MULTIPLIER;
    if (g_backoff_ms > MQTT_SERVICE_BACKOFF_MAX_MS) {
        g_backoff_ms = MQTT_SERVICE_BACKOFF_MAX_MS;
    }
}

static void mqtt_task_drain_queue(void)
{
    mqtt_publish_request_t *req_ptr = NULL;

    while (osMessageQueueGet(g_publish_queue, &req_ptr, NULL, 0) == osOK) {
        if (req_ptr != NULL) {
            free(req_ptr);
        }
    }
}

static void mqtt_task(void *argument)
{
    mqtt_publish_request_t *req_ptr = NULL;
    osStatus_t queue_status;
    int rc;

    (void)argument;
    osDelay(MQTT_SERVICE_CONNECT_DELAY_MS);

    while (!g_mqtt_stop) {
        if (!mqtt_service_is_ready_imei(g_device_imei)) {
            if (!mqtt_service_load_imei()) {
                osDelay(MQTT_SERVICE_RETRY_DELAY_MS);
                continue;
            }
        }

        if (g_mqtt_client == NULL) {
            rc = mqtt_service_create_client();
            if (rc != MQTTCLIENT_SUCCESS) {
                osDelay(MQTT_SERVICE_RETRY_DELAY_MS);
                continue;
            }
        }

        if (g_mqtt_state != MQTT_CONN_STATE_READY) {
            if (g_mqtt_client != NULL && MQTTClient_isConnected(g_mqtt_client)) {
                (void)MQTTClient_disconnect(g_mqtt_client, 1000);
            }
            rc = mqtt_connect_internal();
            if (rc != MQTTCLIENT_SUCCESS) {
                printf("[MQTT] Reconnect failed. Waiting %u ms...\n", (unsigned)g_backoff_ms);
                osDelay(g_backoff_ms);
                mqtt_backoff_next();
                continue;
            }
            mqtt_backoff_reset();
        }

        req_ptr = NULL;
        queue_status = osMessageQueueGet(g_publish_queue, &req_ptr, NULL, 1000);
        if (queue_status == osOK && req_ptr != NULL) {
            (void)mqtt_service_publish_internal(req_ptr);
            free(req_ptr);
            continue;
        }
        if (queue_status != osErrorTimeout) {
            printf("[MQTT] Publish queue error: %d\n", (int)queue_status);
        }
    }

    // Graceful shutdown: drain remaining queued messages
    mqtt_task_drain_queue();

    if (g_mqtt_client != NULL) {
        if (MQTTClient_isConnected(g_mqtt_client)) {
            (void)MQTTClient_disconnect(g_mqtt_client, 1000);
        }
        MQTTClient_destroy(&g_mqtt_client);
    }
    g_mqtt_state = MQTT_CONN_STATE_INIT;
    printf("[MQTT] Task stopped\n");

    // Signal deinit that we have exited
    if (g_mqtt_exit_sem != NULL) {
        (void)osSemaphoreRelease(g_mqtt_exit_sem);
    }
}

void mqtt_service_init(void)
{
    osMessageQueueAttr_t queue_attr = {0};
    osThreadAttr_t task_attr = {0};

    if (g_mqtt_task_id != NULL) {
        return;
    }

    printf("[MQTT] mqtt_service_init()\n");

    queue_attr.name = "mqtt_pub_q";
    g_publish_queue = osMessageQueueNew(MQTT_SERVICE_PUBLISH_QUEUE_LEN,
                                        sizeof(mqtt_publish_request_t *), &queue_attr);
    if (g_publish_queue == NULL) {
        printf("[MQTT] Failed to create publish queue\n");
        return;
    }

    task_attr.name = "mqtt_task";
    task_attr.stack_size = 4096;
    task_attr.priority = osPriorityNormal;

    g_mqtt_task_id = osThreadNew(mqtt_task, NULL, &task_attr);
    if (g_mqtt_task_id == NULL) {
        printf("[MQTT] Failed to create mqtt task!\n");
        (void)osMessageQueueDelete(g_publish_queue);
        g_publish_queue = NULL;
    }
}

int mqtt_service_publish(const char *suffix, const void *payload, int payload_len, int qos)
{
    mqtt_publish_request_t *req;
    osStatus_t status;
    size_t suffix_len;

    if (g_publish_queue == NULL || g_mqtt_task_id == NULL) {
        return MQTT_SERVICE_ERR_NOT_INIT;
    }
    if (suffix == NULL || suffix[0] == '\0' || !mqtt_service_is_valid_qos(qos)) {
        return MQTT_SERVICE_ERR_PARAM;
    }
    if (payload_len < 0 || payload_len > MQTT_SERVICE_PAYLOAD_MAX_LEN) {
        return MQTT_SERVICE_ERR_TOO_LONG;
    }
    if (payload_len > 0 && payload == NULL) {
        return MQTT_SERVICE_ERR_NULL_DATA;
    }

    suffix_len = strlen(suffix);
    if (suffix_len >= MQTT_SERVICE_SUFFIX_MAX_LEN) {
        return MQTT_SERVICE_ERR_SUFFIX;
    }

    req = (mqtt_publish_request_t *)malloc(sizeof(mqtt_publish_request_t));
    if (req == NULL) {
        printf("[MQTT] Failed to alloc publish request\n");
        return MQTT_SERVICE_ERR_ALLOC;
    }

    (void)memset(req, 0, sizeof(*req));
    (void)memcpy(req->suffix, suffix, suffix_len);
    req->suffix[suffix_len] = '\0';
    req->payload_len = payload_len;
    req->qos = qos;
    if (payload_len > 0) {
        (void)memcpy(req->payload, payload, (size_t)payload_len);
    }

    status = osMessageQueuePut(g_publish_queue, &req, 0, 0);
    if (status != osOK) {
        printf("[MQTT] Failed to queue publish, status=%d\n", (int)status);
        free(req);
        return MQTT_SERVICE_ERR_QUEUE;
    }

    return MQTT_SERVICE_OK;
}

bool mqtt_service_is_connected(void)
{
    return g_mqtt_state == MQTT_CONN_STATE_READY;
}

void mqtt_service_register_cmd_callback(mqtt_msg_callback_t cb)
{
    g_cmd_callback = cb;
}

void mqtt_service_register_msg_callback(mqtt_msg_callback_t cb)
{
    g_msg_callback = cb;
}

void mqtt_service_deinit(void)
{
    osSemaphoreAttr_t sem_attr = {0};

    if (g_mqtt_task_id == NULL) {
        return;
    }

    printf("[MQTT] mqtt_service_deinit()\n");

    // Create exit semaphore for synchronization
    sem_attr.name = "mqtt_exit";
    g_mqtt_exit_sem = osSemaphoreNew(1, 0, &sem_attr);

    g_mqtt_stop = true;

    // Wait for the task to signal exit (timeout 30s as TLS teardown may be slow)
    if (g_mqtt_exit_sem != NULL) {
        (void)osSemaphoreAcquire(g_mqtt_exit_sem, 30000);
        (void)osSemaphoreDelete(g_mqtt_exit_sem);
        g_mqtt_exit_sem = NULL;
    } else {
        // Fallback if semaphore creation fails
        osDelay(3000);
    }

    if (g_publish_queue != NULL) {
        (void)osMessageQueueDelete(g_publish_queue);
        g_publish_queue = NULL;
    }

    g_mqtt_task_id = NULL;
    g_mqtt_stop = false;
    g_cmd_callback = NULL;
    g_msg_callback = NULL;
    mqtt_backoff_reset();
}

// Auto init during module load phase (priority range: 0-4)
APP_FEATURE_INIT_PRI(mqtt_service_init, 4);
