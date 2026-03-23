#ifndef HTTP_API_H
#define HTTP_API_H
#include "sk_ota.h"
#include <stdio.h>
#include <stdbool.h>
#include "app_at_process.h"

#define MAX_URL_LENGTH 512
#define MAX_FILE_PATH_LENGTH 256

typedef struct {
    char url[MAX_URL_LENGTH];
    char file_path[MAX_FILE_PATH_LENGTH];
} url_file_data_t;

typedef enum
{
    HTTP_DOWNlOAD_OTA,
    HTTP_DOWNLOAD_MODEL,
    HTTP_GET_AGPS,
    HTTP_UPLOAD_FILE,
    HTTP_UPLOAD_LOG,
} http_event_type_t;

enum
{
    TYPE_DOWN_MODEM = 1,
    TYPE_DOWN_HOST = 2,
    TYPE_DOWN_FIRST = 3,
    TYPE_DOWN_SECOND = 4,
};

typedef struct
{
    http_event_type_t event_type;
    void *data;
    size_t length;
    uint8_t other_type;
} http_event_t;

enum
{
    TYPE_USING_CODE0 = 0,
    TYPE_USING_CODE1 = 1,
};

int http_task_init(void);
int http_upload_audio(const char *file_path);
int http_download_audio(const char *req_url);
int http_upload_file(const char *file_path);
int http_send_data_to_server(void *data, size_t length, http_event_type_t type, uint8_t other_type);
int http_send_type_to_server(http_event_type_t type);
int update_model_file(void);
unsigned char get_download_file_state(void);
uint8_t download_crc8(uint8_t crc, const uint8_t *data, size_t len);
int32_t ws_report_ota_result(char *buf);
int http_upload_log(const char *file_path);
extern bool model_replacing;
extern uint8_t g_remote_crc_modem;
extern uint8_t g_remote_crc_host;

#define HTTP_PRINT(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)
#endif