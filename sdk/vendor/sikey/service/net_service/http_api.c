#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "httpclient.h"
#include "http_api.h"
#include "cJSON.h"
#include "dev_storage.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rgb_led.h"

extern unsigned int uploading_log;
char str_code[2][12] = {""};
// 定义边界字符串
#define BOUNDARY "----WebKitFormBoundary7MA4YWxkTrZu0gW"

/* @brief http request buffer */
#define REQ_BUF_SIZE 2048
static char req_buf[REQ_BUF_SIZE];

/* @brief http response buffer */
#define RSP_BUF_SIZE 2048
static char rsp_buf[RSP_BUF_SIZE] = {0};

osThreadId_t http_task_id;
unsigned long http_msgqueue_id;
static unsigned char g_download_file_state = 0;
bool g_ota_skip_crc = false;
/**
 * 上传六轴数据文件
 */
int http_upload_file_ll(const char *file_path,  char *full_url, uint8_t type)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    int ret = -1;
    char pet_mac[32] = {0};
    char code[12] = {0};

    // 读取文件内容
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        printf("fopen %s failed!\n", file_path);
        return -1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 读取文件内容
    unsigned char *file_data = malloc(file_size);
    if (!file_data)
    {
        fclose(file);
        return -1;
    }
    fread(file_data, 1, file_size, file);
    fclose(file);

    // 构建 multipart/form-data 请求体
    const char *form_template =
        "--" BOUNDARY "\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
        "Content-Type: application/octet-stream\r\n"
        "\r\n";

    const char *field_template =
        "--" BOUNDARY "\r\n"
        "Content-Disposition: form-data; name=\"%s\"\r\n"
        "\r\n"
        "%s\r\n";

    const char *footer = "--" BOUNDARY "--\r\n";

    // 获取文件名
    const char *filename = strrchr(file_path, '/');
    if (!filename)
        filename = strrchr(file_path, '\\');
    if (filename)
        filename++;
    else
        filename = file_path;

    printf("file name = %s\n", filename);
    // 计算各部分大小
    size_t file_header_len = snprintf(NULL, 0, form_template, filename);

    watch_storage_get(STORAGE_HARD_CODE, pet_mac, sizeof(pet_mac));
    if (pet_mac[0] == 0)
    {
        // if not find hardcode, use default value
        snprintf(pet_mac, 32, "11:11:11:11:11:11");
    }
    size_t mac_field_len = snprintf(NULL, 0, field_template, "petNecklaceCode", pet_mac);

    //watch_storage_get(STORAGE_DATA_CODE, code, 12);
    if (type == TYPE_USING_CODE0)
    {
        memcpy(code, str_code[0], 12);
        memset(str_code[0], 0, 12);
    }
    else if (type == TYPE_USING_CODE1)
    {
        memcpy(code, str_code[1], 12);
        memset(str_code[1], 0, 12);
    }
    else {
        strcpy(code, "debug");
    }
    size_t code_field_len = snprintf(NULL, 0, field_template, "code", code);
    size_t footer_len = strlen(footer);

    size_t total_size = file_header_len + file_size + 2 + // +2 for \r\n after file data
                        mac_field_len + code_field_len + footer_len;

    // 分配内存
    unsigned char *request_data = malloc(2048);
    if (!request_data)
    {
        free(file_data);
        return -1;
    }
    memset(request_data, 0, 2048);

    unsigned char *response_data = malloc(2048);
    if (!response_data)
    {
        free(file_data);
        free(request_data);
        return -1;
    }
    memset(response_data, 0, 2048);

    unsigned char *post_data = malloc(total_size);
    if (!post_data)
    {
        free(file_data);
        free(request_data);
        free(response_data);
        return -1;
    }

    // 构建完整的请求体
    char *ptr = (char *)post_data;

    // 文件部分头部
    ptr += sprintf(ptr, form_template, filename);

    // 文件内容
    memcpy(ptr, file_data, file_size);
    ptr += file_size;

    // 文件部分结束的\r\n
    ptr += sprintf(ptr, "\r\n");

    // petNecklaceCode 字段
    ptr += sprintf(ptr, field_template, "petNecklaceCode", pet_mac);

    // code 字段
    ptr += sprintf(ptr, field_template, "code", code);

    // 结束边界
    strcpy(ptr, footer);

    // 设置缓冲区
    client_data.headerBuf = request_data;
    client_data.headerBufLen = 2048;

    client_data.responseBuf = response_data;
    client_data.responseBufLen = 2048;

    // 设置 POST 数据
    client_data.postBuf = (char *)post_data;
    client_data.postBufLen = total_size;

    // 设置自定义头部
    char content_type[128];
    snprintf(content_type, sizeof(content_type),
             "Content-Type: multipart/form-data; boundary=" BOUNDARY "\r\n");

    HttpClientSetCustomHeader(&client, content_type);

    //char full_url[256];
    //snprintf(full_url, sizeof(full_url),
             //"http://test.t553api.qialg.com/api/deviceWsServer/deviceWs/device/uploadSixFile");

    printf("upload url: %s\n", full_url);
    HTTP_PRINT("filepath: %s, size: %ld bytes\n", file_path, file_size);
    printf("mac: %s\n", pet_mac);
    HTTP_PRINT("code: %s\n", code);

    // 发送 POST 请求
    ret = HttpClientPostRequest(&client, full_url, &client_data);

    if (ret >= 0 && client.responseCode == 200)
    {
        HTTP_PRINT("file upload successful status: %d\n", client.responseCode);
        ret = 0;
    }
    else
    {
        HTTP_PRINT("file upload failed status: %d, err: 0x%x\n", client.responseCode, ret);
        if (client_data.responseBuf && strlen(client_data.responseBuf) > 0)
        {
            printf("err info: %s\n", client_data.responseBuf);
        }
    }

    // 清理资源
    HttpClientClose(&client);
    free(file_data);
    free(request_data);
    free(response_data);
    free(post_data);
    return ret;
}

/* http get请求下载音源 */
int http_download_audio(const char *req_url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    size_t total_written = 0, file_size = 0;
    int ret = -1;
    long local_file_size = 0;
    char range_header[128] = {0};
    FILE *fd = NULL;
    struct stat file_stat;

    unsigned char *request_data = malloc(2048);
    if (!request_data)
    {
        return -1;
    }
    memset(request_data, 0, 2048);

    unsigned char *response_data = malloc(2048);
    if (!response_data)
    {
        free(request_data);
        return -1;
    }
    memset(response_data, 0, 2048);

    client_data.headerBuf = request_data;
    client_data.headerBufLen = 2048;

    client_data.responseBuf = response_data;
    client_data.responseBufLen = 2048;

    printf("\n http request: %s \r\n", req_url);

    /* 检查本地文件是否存在并获取大小 */
    if (stat("/user/audio.ogg", &file_stat) == 0)
    {
        local_file_size = file_stat.st_size;
        printf("find file size: %ld bytes\n", local_file_size);

        /* 设置Range请求头 */
        snprintf(range_header, sizeof(range_header), "Range: bytes=%ld-\r\n", local_file_size);
        HttpClientSetCustomHeader(&client, range_header);

        /* 以追加模式打开文件 */
        fd = fopen("/user/audio.ogg", "ab");
    }
    else
    {
        local_file_size = 0;
        fd = fopen("/user/audio.ogg", "wb");
        snprintf(range_header, sizeof(range_header), "Range: bytes=%ld-\r\n", local_file_size);
        HttpClientSetCustomHeader(&client, range_header);
    }

    if (fd == NULL)
    {
        printf("fopen failed \r\n");
        printf("Error: %s \r\n", strerror(errno));
        free(request_data);
        free(response_data);
        return -1;
    }

    ret = HttpClientConn(&client, req_url);
    if (ret == 0)
    {
        ret = HttpClientSend(&client, req_url, HTTP_GET, &client_data);
        if (ret == 0)
        {
            do
            {
                ret = HttpClientRecvResponse(&client, &client_data);
                if (client_data.contentBlockLen > 0 && (ret >= 0))
                {
                    //printf("receive client_data.contentBlockLen= %d \r\n", client_data.contentBlockLen);
                    file_size = fwrite(client_data.responseBuf, 1, client_data.contentBlockLen, fd);
                    total_written += file_size;

                    /* 显示下载进度 */
                    if (client_data.responseContentLen > 0)
                    {
                        long total_expected = local_file_size + client_data.responseContentLen;
                        double progress = (double)(local_file_size + total_written) / total_expected * 100;
                        printf("downloading: %.2f%% (%ld/%ld bytes)\n",
                               progress, local_file_size + total_written, total_expected);
                    }
                }
            } while (client_data.isMore && (ret >= 0));

            /* 验证下载完整性 */
            long total_downloaded = local_file_size + total_written;

            if (total_downloaded >= client_data.responseContentLen &&
                (client.responseCode == 200 || client.responseCode == 206) &&
                file_size > 0 && (ret == 0))
            {
                ret = 0;
                printf("responsecode = %d \n", client.responseCode);
                printf("audio file is downloaded successfully and the data is complete! \n");
                printf("total size: %ld bytes\n", total_downloaded);
            }
            else
            {
                printf("responsecode = %d \n", client.responseCode);
                printf("audio file is incomplete! \n");
                printf("expect: %d, act: %ld\n",
                       client_data.responseContentLen, total_downloaded);
            }
        }
        else
        {
            printf("responsecode = %d \n", client.responseCode);
            printf("receive failed ret= 0x%x \r\n", ret);
        }
    }

    if (fd)
    {
        fclose(fd);
    }
    HttpClientClose(&client);
    free(request_data);
    free(response_data);
    return ret;
}

/**
 * 上传音频文件
 */
int http_upload_audio_ll(const char *file_path)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    char pet_mac[32] = {0};
    int ret = -1;

    // 读取文件内容
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        printf("fopen %s failed!\n", file_path);
        return -1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 读取文件内容
    unsigned char *file_data = malloc(file_size);
    if (!file_data)
    {
        fclose(file);
        return -1;
    }
    fread(file_data, 1, file_size, file);
    fclose(file);

    // 构建 multipart/form-data 请求体
    const char *form_template =
        "--" BOUNDARY "\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
        "Content-Type: application/octet-stream\r\n"
        "\r\n";

    const char *field_template =
        "--" BOUNDARY "\r\n"
        "Content-Disposition: form-data; name=\"%s\"\r\n"
        "\r\n"
        "%s\r\n";

    const char *footer = "--" BOUNDARY "--\r\n";

    // 获取文件名
    const char *filename = strrchr(file_path, '/');
    if (!filename)
        filename = strrchr(file_path, '\\');
    if (filename)
        filename++;
    else
        filename = file_path;

    printf("file name = %s\n", filename);
    // 计算各部分大小
    size_t file_header_len = snprintf(NULL, 0, form_template, filename);

    watch_storage_get(STORAGE_HARD_CODE, pet_mac, sizeof(pet_mac));
    if (pet_mac[0] == 0)
    {
        // if not find hardcode, use default value
        snprintf(pet_mac, 32, "11:11:11:11:11:11");
    }
    size_t mac_field_len = snprintf(NULL, 0, field_template, "petNecklaceCode", pet_mac);
    size_t footer_len = strlen(footer);

    size_t total_size = file_header_len + file_size + 2 + // +2 for \r\n after file data
                        mac_field_len + footer_len;

    // 分配内存
    unsigned char *request_data = malloc(2048);
    if (!request_data)
    {
        free(file_data);
        return -1;
    }
    memset(request_data, 0, 2048);

    unsigned char *response_data = malloc(2048);
    if (!response_data)
    {
        free(file_data);
        free(request_data);
        return -1;
    }
    memset(response_data, 0, 2048);

    unsigned char *post_data = malloc(total_size);
    if (!post_data)
    {
        free(file_data);
        free(request_data);
        free(response_data);
        return -1;
    }

    // 构建完整的请求体
    char *ptr = (char *)post_data;

    // 文件部分头部
    ptr += sprintf(ptr, form_template, filename);

    // 文件内容
    memcpy(ptr, file_data, file_size);
    ptr += file_size;

    // 文件部分结束的\r\n
    ptr += sprintf(ptr, "\r\n");

    // petNecklaceCode 字段
    ptr += sprintf(ptr, field_template, "petNecklaceCode", pet_mac);

    // 结束边界
    strcpy(ptr, footer);

    // 设置缓冲区
    client_data.headerBuf = request_data;
    client_data.headerBufLen = 2048;

    client_data.responseBuf = response_data;
    client_data.responseBufLen = 2048;

    // 设置 POST 数据
    client_data.postBuf = (char *)post_data;
    client_data.postBufLen = total_size;

    // 设置自定义头部
    char content_type[128];
    snprintf(content_type, sizeof(content_type),
             "Content-Type: multipart/form-data; boundary=" BOUNDARY "\r\n");

    HttpClientSetCustomHeader(&client, content_type);

    // 设置 URL（注意：需要处理查询参数）
    char full_url[256];
    snprintf(full_url, sizeof(full_url),
             "http://test.t553api.qialg.com/api/deviceWsServer/deviceWs/device/deviceUploadFile");

    printf("upload url: %s\n", full_url);
    printf("filepath: %s, size: %ld bytes\n", file_path, file_size);
    printf("mac: %s\n", pet_mac);

    // 发送 POST 请求
    ret = HttpClientPostRequest(&client, full_url, &client_data);

    if (ret >= 0 && client.responseCode == 200)
    {
        printf("file upload successful status: %d\n", client.responseCode);
        printf("client_data.responseContentLen = %d\n", client_data.responseContentLen);
        // printf("服务器响应: %s\n", client_data.responseBuf);
        ret = 0;
    }
    else
    {
        printf("file upload failed status: %d, err: 0x%x\n", client.responseCode, ret);
        if (client_data.responseBuf && strlen(client_data.responseBuf) > 0)
        {
            printf("err info: %s\n", client_data.responseBuf);
        }
    }

    // 清理资源
    HttpClientClose(&client);
    free(file_data);
    free(request_data);
    free(response_data);
    free(post_data);
    return ret;
}

#define WATCH_GNSS_MODE_GPS 0x01
#define WATCH_GNSS_MODE_GLO 0x02
#define WATCH_GNSS_MODE_BDS 0x04
#define WATCH_GNSS_MODE_GAL 0x08
#define WATCH_GNSS_MODE_ALL 0x0F
#define WATCH_GNSS_DATA_EPH 0x01
#define WATCH_GNSS_DATA_UTC 0x02
#define WATCH_GNSS_DATA_ION 0x04
#define WATCH_GNSS_DATA_RTI 0x08
#define WATCH_GNSS_DATA_AUX 0x10
#define WATCH_GNSS_DATA_ALL 0x1F

#define AGPS_BUFFER_SIZE (20 * 1024)
extern pthread_mutex_t agps_mutex;
char agps_buffer[AGPS_BUFFER_SIZE] = {0};

/* 获取agps数据下载 */
int http_agps_download(void)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    size_t total_written = 0, file_size = 0;
    int ret = -1;
    const char *req_url = "http://api.tools.sikey.com.cn/app-api/tools/hisi/agnss";
    cJSON *pJson = NULL;
    char *request_buf = NULL, *response_buf = NULL, *post_data = NULL;
    size_t buffer_used = 0;
    FILE *fd = NULL;

    // 创建JSON请求数据
    pJson = cJSON_CreateObject();
    if (!pJson)
    {
        return -1;
    }

    cJSON_AddNumberToObject(pJson, "constellation", WATCH_GNSS_MODE_ALL);
    cJSON_AddNumberToObject(pJson, "datatype", WATCH_GNSS_DATA_ALL);
    cJSON_AddStringToObject(pJson, "deviceId", "86724507012290");

    post_data = cJSON_Print(pJson);
    if (!post_data)
    {
        cJSON_Delete(pJson);
        return -1;
    }

    request_buf = malloc(2048);
    if (!request_buf)
    {
        printf("request buf malloc failed\n");
        goto err0;
    }
    memset(request_buf, 0, 2048);

    response_buf = malloc(2048);
    if (!response_buf)
    {
        printf("response buf malloc failed\n");
        goto err1;
    }
    memset(response_buf, 0, 2048);

    memset(agps_buffer, 0, AGPS_BUFFER_SIZE);

    client_data.headerBuf = request_buf;
    client_data.headerBufLen = 2048;

    client_data.postBuf = post_data;
    client_data.postBufLen = strlen(post_data);

    client_data.responseBuf = response_buf;
    client_data.responseBufLen = 2048;

    printf("\n http request: %s \r\n", req_url);
    // 设置HTTP头部
    HttpClientSetCustomHeader(&client, "Content-Type: application/json\r\n");

    ret = HttpClientConn(&client, req_url);
    if (ret == 0)
    {
        ret = HttpClientSend(&client, req_url, HTTP_POST, &client_data);
        if (ret == 0)
        {
            do
            {
                ret = HttpClientRecvResponse(&client, &client_data);
                if (client_data.contentBlockLen > 0)
                {
                    printf("receive client_data.contentBlockLen= %d\n", client_data.contentBlockLen);

                    if (buffer_used + client_data.contentBlockLen > AGPS_BUFFER_SIZE)
                    {
                        printf("agps buffer overflow! required=%zu, available=%zu\n",
                               buffer_used + client_data.contentBlockLen, AGPS_BUFFER_SIZE);
                        ret = -1;
                        break;
                    }

                    memcpy(agps_buffer + buffer_used, client_data.responseBuf, client_data.contentBlockLen);
                    buffer_used += client_data.contentBlockLen;
                }
            } while (client_data.isMore && (ret >= 0));

            if (ret == 0 && client_data.responseContentLen > 0 &&
                buffer_used == client_data.responseContentLen &&
                client.responseCode == 200)
            {
                printf("responsecode = %d \n", client.responseCode);
                printf("agps data received successfully, total size: %zu bytes\n", buffer_used);

                fd = fopen("/user/xgnss/AGNSS.dat", "wb");
                if (fd == NULL)
                {
                    printf("fopen failed: %s \r\n", strerror(errno));
                    ret = -1;
                }
                else
                {
                    pthread_mutex_lock(&agps_mutex);
                    file_size = fwrite(agps_buffer, 1, buffer_used, fd);
                    pthread_mutex_unlock(&agps_mutex);
                    fclose(fd);

                    if (file_size == buffer_used)
                    {
                        printf("agps data written to file successfully! \n");
                        ret = 0;
                    }
                    else
                    {
                        printf("file write incomplete! expected=%zu, actual=%zu\n",
                               buffer_used, file_size);
                        ret = -1;
                    }
                }
            }
            else
            {
                printf("agps data receive failed! responsecode=%d, expected=%zu, received=%zu\n",
                       client.responseCode, client_data.responseContentLen, buffer_used);
                ret = -1;
            }
        }
        else
        {
            printf("http send failed, responsecode = %d, ret= 0x%x \r\n", client.responseCode, ret);
        }
    }

    HttpClientClose(&client);

err2:
    free(response_buf);
err1:
    free(request_buf);
err0:
    cJSON_Delete(pJson);
    free(post_data);
    return ret;
}
#if 0
/* http download */
int http_download(const char *req_url, const char *file_path)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    size_t total_written = 0, file_size = 0;
    int ret = -1;
    memset(req_buf, 0, sizeof(req_buf));
    client_data.headerBuf = req_buf;
    client_data.headerBufLen = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.responseBuf = rsp_buf;
    client_data.responseBufLen = sizeof(rsp_buf);

    printf("\n http request: %s \r\n", req_url);

    FILE *fd = fopen(file_path, "wb");
    if (fd == NULL)
    {
        printf("fopen failed \r\n");
        printf("Error: %s \r\n", strerror(errno));
        return -1;
    }

    ret = HttpClientConn(&client, req_url);
    if (ret == 0)
    {
        ret = HttpClientSend(&client, req_url, HTTP_GET, &client_data);
        if (ret == 0)
        {
            do
            {
                ret = HttpClientRecvResponse(&client, &client_data);
                if (client_data.contentBlockLen > 0)
                {
                    printf("receive client_data.contentBlockLen= %d \r\n", client_data.contentBlockLen);
                    file_size = fwrite(client_data.responseBuf, 1, client_data.contentBlockLen, fd);
                    total_written += file_size;
                }
            } while (client_data.isMore && ret >= 0);
            if (client_data.responseContentLen == 0)
            {
                printf("receive length = 0 \n");
            }
            if (total_written == client_data.responseContentLen && client.responseCode == 200 && file_size > 0)
            {
                ret = 0;
                printf("responsecode = %d \n", client.responseCode);
                printf("ota file is downloaded successfully and the data is complete!\n");
            }
            else
            {
                printf("responsecode = %d \n", client.responseCode);
                printf("ota file is incomplete!\n");
            }
        }
        else
        {
            printf("responsecode = %d \n", client.responseCode);
            printf("receive failed ret= 0x%x \r\n", ret);
        }
    }
    fclose(fd);
    HttpClientClose(&client);
    return ret;
}
#else
/* 使用断点续传的方式 */
int http_download(const char *req_url, const char *file_path)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    size_t total_written = 0, file_size = 0;
    int ret = -1;
    long local_file_size = 0;
    char range_header[128] = {0};
    FILE *fd = NULL;
    struct stat file_stat;

    memset(req_buf, 0, sizeof(req_buf));
    client_data.headerBuf = req_buf;
    client_data.headerBufLen = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.responseBuf = rsp_buf;
    client_data.responseBufLen = sizeof(rsp_buf);

    printf("\n http request: %s \r\n", req_url);

    /* 检查本地文件是否存在并获取大小 */
    if (stat(file_path, &file_stat) == 0)
    {
        local_file_size = file_stat.st_size;
        printf("find file size: %ld bytes\n", local_file_size);

        /* 设置Range请求头 */
        snprintf(range_header, sizeof(range_header), "Range: bytes=%ld-\r\n", local_file_size);
        HttpClientSetCustomHeader(&client, range_header);

        /* 以追加模式打开文件 */
        fd = fopen(file_path, "ab");
    }
    else
    {
        local_file_size = 0;
        printf("start new downloader\n");
        fd = fopen(file_path, "wb");
        snprintf(range_header, sizeof(range_header), "Range: bytes=%ld-\r\n", local_file_size);
        HttpClientSetCustomHeader(&client, range_header);
    }

    if (fd == NULL)
    {
        printf("fopen failed \r\n");
        printf("Error: %s \r\n", strerror(errno));
        return -1;
    }

    ret = HttpClientConn(&client, req_url);
    if (ret == 0)
    {
        ret = HttpClientSend(&client, req_url, HTTP_GET, &client_data);
        if (ret == 0)
        {
            do
            {
                ret = HttpClientRecvResponse(&client, &client_data);
                if (client_data.contentBlockLen > 0 && (ret >= 0))
                {
                    //printf("receive client_data.contentBlockLen= %d \r\n", client_data.contentBlockLen);
                    file_size = fwrite(client_data.responseBuf, 1, client_data.contentBlockLen, fd);
                    total_written += file_size;

                    /* 显示下载进度 */
                    if (client_data.responseContentLen > 0)
                    {
                        long total_expected = local_file_size + client_data.responseContentLen;
                        double progress = (double)(local_file_size + total_written) / total_expected * 100;
                        printf("downloading: %.2f%% (%ld/%ld bytes)\n",
                               progress, local_file_size + total_written, total_expected);
                    }
                }
            } while (client_data.isMore && (ret >= 0));

            /* 验证下载完整性 */
            long total_downloaded = local_file_size + total_written;

            if (total_downloaded >= client_data.responseContentLen &&
                (client.responseCode == 200 || client.responseCode == 206) &&
                file_size > 0 && (ret == 0))
            {

                ret = 0;
                printf("responsecode = %d \n", client.responseCode);
                printf("ota file is downloaded successfully and the data is complete! \n");
                printf("total size: %ld bytes\n", total_downloaded);
            }
            else
            {
                printf("responsecode = %d \n", client.responseCode);
                printf("ota file is incomplete! \n");
                printf("expect: %d, act: %ld\n",
                       client_data.responseContentLen, total_downloaded);
            }
        }
        else
        {
            printf("responsecode = %d \n", client.responseCode);
            printf("receive failed ret= 0x%x \r\n", ret);
        }
    }

    if (fd)
    {
        fclose(fd);
    }
    HttpClientClose(&client);
    return ret;
}
#endif

int ota_file_crc_check(char *saved_file_path, uint8_t other_type)
{
    int ret = -1;
    char buffer[4096] = {0};
    size_t bytes;
    uint8_t crc = 0x00;

    /* MQTT DFU path: protocol has no CRC field, skip download CRC check.
     * The firmware package has internal bootloader verification. */
    if (g_ota_skip_crc) {
        printf("ota_file_crc_check: skipped (MQTT DFU mode)\n");
        g_ota_skip_crc = false;
        return 0;
    }

    FILE *src = fopen(saved_file_path, "rb");
    if (!src)
    {
        printf("err: ota file %s open failed\n", saved_file_path);
        return -1;
    }
    /*下载成功之后，crc校验*/
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        crc = download_crc8(crc, (const uint8_t *)buffer, bytes);
    }
    fclose(src);
    /* 根据类型进行crc匹配 */
    switch (other_type)
    {
    case TYPE_DOWN_FIRST:
    case TYPE_DOWN_MODEM:
        printf("g_remote_crc_modem = %u, crc = %u\n", g_remote_crc_modem, crc);
        if (g_remote_crc_modem == crc)
        {
            printf("modem file crc check successful\n");
            ret = 0;
        }
        else
        {
            printf("modem file crc check failed\n");
            ret = -1;
        }
        break;
    case TYPE_DOWN_SECOND:
    case TYPE_DOWN_HOST:
        printf("g_remote_crc_host = %u, crc = %u\n", g_remote_crc_host, crc);
        if (g_remote_crc_host == crc)
        {
            printf("host file crc check successful\n");
            ret = 0;
        }
        else
        {
            printf("host file crc check failed\n");
            ret = -1;
        }
        break;
    default:
        break;
    }
    return ret;
}

/* http get请求下载model bin */
int http_download_model(const char *req_url)
{
    int ret = 0;
    int retry_times = 6;

    if (access("/user/model.bin", F_OK) == 0)
    {
        // 文件存在，删除它
        if (remove("/user/model.bin") == 0)
        {
            printf("file /user/model.bin removed\n");
        }
        else
        {
            printf("can't remove /user/model.bin\n");
            ret = -1;
            goto err;
        }
    }

    while (retry_times > 0)
    {
        ret = http_download(req_url, "/user/model.bin");
        if (ret == 0)
        {
            /*下载成功*/
            ret = update_model_file();
            if (ret == 0)
            {
                printf("update_model_file success\n");
                break;
            }
            else if (ret == -2)
            {
                printf("model crc error, to retry\n");
                remove("/user/model.bin");
            }
            else //unlike
            {
                printf("update_model_file failed, ret = %d\n", ret);
                break;
            }
        }
        retry_times--;
    }
err:
    return ret;
}

static int file_remove(char *saved_file_path)
{
    int ret = 0;
    if (access(saved_file_path, F_OK) == 0)
    {
        // 文件存在，删除它
        if (remove(saved_file_path) == 0)
        {
            printf("file %s removed\n", saved_file_path);
        }
        else
        {
            printf("can't remove %s\n", saved_file_path);
            ret = -1;
        }
    }
    return ret;
}

/* 下载ota镜像 */
int http_download_ota(const char *data, uint8_t other_type)
{
    if (data == NULL)
    {
        printf("Error: data is NULL\n");
        return -1;
    }

    char *copy = strdup(data);
    if (copy == NULL)
    {
        printf("Memory allocation failed\n");
        return -1;
    }

    int token_count = 0, ret = -1;
    char *token = strtok(copy, ";");
    char *saved_url = NULL, *saved_file_path = NULL;

    while (token != NULL && token_count < 2)
    {
        switch (token_count)
        {
        case 0:                        // URL
            saved_url = strdup(token); // 复制URL
            if (saved_url)
            {
                printf("url: %s\n", saved_url);
            }
            break;
        case 1:                              // 文件路径
            saved_file_path = strdup(token); // 复制文件路径
            if (saved_file_path)
            {
                printf("file path: %s\n", saved_file_path);
            }
            break;
        }
        token_count++;
        token = strtok(NULL, ";");
    }

    // 验证是否成功解析了两个token
    if (saved_url && saved_file_path)
    {
        int retry_times = 10;
        if (file_remove(saved_file_path) < 0)
        {
            goto err;
        }
        g_download_file_state = 1;
        char tmp[128] = {0};
        //蓝灯开始闪烁
        led_blue_blink_start();
        while (retry_times > 0)
        {
            ret = http_download(saved_url, saved_file_path);
            if (ret == 0)
            {
                ret = ota_file_crc_check(saved_file_path, other_type);
                if (ret == 0)
                {
                    switch (other_type)
                    {
                    case TYPE_DOWN_MODEM:
                        sk_ota_start_update_async(SK_UPDATE_MODEM_ONLY);
                        break;
                    case TYPE_DOWN_HOST:
                        sk_ota_start_update_async(SK_UPDATE_MCU_ONLY);
                        break;
                    case TYPE_DOWN_SECOND:
                        sk_ota_start_update_async(SK_UPDATE_MODEM_AND_MCU);
                    default:
                        break;
                    }
                    break;
                }
                else
                {
                    file_remove(saved_file_path);
                }
            }
            retry_times--;
        }

        if(retry_times == 0)
        {
            printf("download ota failed\n");
            led_blue_blink_stop();
            snprintf(tmp, 128, "%s failed", saved_file_path);
            ws_report_ota_result(saved_file_path);
            ret = -1;
        }
    }
    else
    {
        printf("Error: Invalid data format. Expected 'url;file_path'\n");
        ret = -1;
    }
    g_download_file_state = 0;

err:
    // 清理内存
    free(copy);
    free(saved_url);
    free(saved_file_path);

    return ret;
}

static void http_task_entry(void *data)
{
    int32_t ret = 0;
    http_event_t msg = {0};
    unsigned int msg_sz = sizeof(http_event_t);
    int retry = 5;
    (void)data;

    while (1)
    {
        ret = osal_msg_queue_read_copy(http_msgqueue_id, (void *)&msg, &msg_sz, OSAL_MSGQ_WAIT_FOREVER);
        if (ret == OSAL_FAILURE)
        {
            printf("osal msg queue read copy failed\n");
            continue;
        }
        switch (msg.event_type)
        {
        case HTTP_DOWNlOAD_OTA:
            http_download_ota((const char *)msg.data, msg.other_type);
            break;
        case HTTP_DOWNLOAD_MODEL:
            ret = http_download_model((const char *)msg.data);
            if (ret < 0)
            {
                model_replacing = 0;
            }
            break;
        case HTTP_GET_AGPS:
        {
            retry = 5;
            while (retry > 0)
            {
                ret = http_agps_download();
                if (ret == 0)
                {
                    break;
                }
                retry--;
            }
            break;
        }
        case HTTP_UPLOAD_FILE:
        {
            char full_url[256] = "http://test.t553api.qialg.com/api/deviceWsServer/deviceWs/device/uploadSixFile";
            retry = 5;
            while (retry > 0)
            {
                ret = http_upload_file_ll((const char *)msg.data, full_url, msg.other_type);
                if (ret == 0)
                {
                    break;
                }
                retry--;
            }
            break;
        }
        case HTTP_UPLOAD_LOG:
        {
            char full_url[256] = "http://test.t553api.qialg.com/api/deviceWsServer/deviceWs/device/deviceUploadDebug";
            retry = 5;
            uploading_log = 1;
            while (retry > 0)
            {
                ret = http_upload_file_ll((const char *)msg.data, full_url, 3);
                if (ret == 0)
                {
                    //上传成功就把ws.log文件删掉
                    if (access("/user/ws.log", F_OK) == 0)
                    {
                        // 文件存在，删除它
                        if (remove("/user/ws.log") == 0)
                        {
                            printf("file /user/ws.log removed\n");
                        }
                    }
                    break;
                }
                retry--;
            }
            uploading_log = 0;
            break;
        }
        default:
            break;
        }
        if (msg.data)
            free(msg.data);
    }
}

int http_task_init(void)
{
    int ret = 0;
    osThreadAttr_t threadAttr_pqc = {0};
    static const char http_task_name[] = "thread_http_task";
    static const char http_queue_name[] = "thread_http_queue";

    printf("http_task_init start\n");

    ret = osal_msg_queue_create(http_queue_name, 10, &http_msgqueue_id, 0, sizeof(http_event_t));
    if (ret != 0)
    {
        printf("Create msgqueue %s failed: %d\n", http_queue_name, ret);
        goto EXIT0;
    }

    threadAttr_pqc.name = http_task_name;
    threadAttr_pqc.stack_size = 0x2000;
    threadAttr_pqc.priority = 17;
    threadAttr_pqc.stack_mem = memalign(16, threadAttr_pqc.stack_size);

    http_task_id = osThreadNew(http_task_entry, NULL, &threadAttr_pqc);
    if (http_task_id == NULL)
    {
        printf("http task create failed\n");
        ret = -1;
        goto EXIT1;
    }

    return ret;
EXIT1:
    osal_msg_queue_delete(http_msgqueue_id);
EXIT0:
    return ret;
}

int http_send_data_to_server(void *data, size_t length, http_event_type_t type, uint8_t other_type)
{
    int32_t ret = 0;
    http_event_t event = {0};

    if (!data || length == 0)
    {
        printf("data or length invalid\n");
        return -1;
    }
    printf("data=%s,length =%d\n", data, length);
    event.data = calloc(length, sizeof(uint8_t));
    if (!event.data)
    {
        printf("calloc failed\n");
        return -1;
    }
    memcpy(event.data, data, length);
    event.length = length;
    event.event_type = type;
    event.other_type = other_type;

    ret = osal_msg_queue_write_copy(http_msgqueue_id, (void *)&event, sizeof(http_event_t), OSAL_MSGQ_WAIT_FOREVER);
    if (ret == OSAL_FAILURE)
    {
        printf("send event[%u] failed\n", type);
        if (event.data)
            free(event.data);
    }
    return ret;
}

int http_send_type_to_server(http_event_type_t type)
{
    int32_t ret = 0;
    http_event_t event = {0};

    event.data = NULL;
    event.length = 0;
    event.event_type = type;

    ret = osal_msg_queue_write_copy(http_msgqueue_id, (void *)&event, sizeof(http_event_t), OSAL_MSGQ_WAIT_FOREVER);
    if (ret == OSAL_FAILURE)
    {
        printf("send event[%u] failed\n", type);
    }
    return ret;
}

/* 使用异步方式上传六轴数据 */
int http_upload_file(const char *file_path)
{
    int ret = 0;
    size_t length = strlen(file_path) + 1;
    char code[12] = {0};
    uint8_t type = TYPE_USING_CODE0;

    watch_storage_get(STORAGE_DATA_CODE, code, 12);
    if (str_code[0][0] == '\0')
    {
        memcpy(str_code[0], code, 12);
        type = TYPE_USING_CODE0;
    }
    else
    {
        memcpy(str_code[1], code, 12);
        type = TYPE_USING_CODE1;
    }
    ret = http_send_data_to_server(file_path, length, HTTP_UPLOAD_FILE, type);
    return ret;
}

int http_upload_audio(const char *file_path)
{
    int ret = 0;
    int retry = 6;
    while (retry > 0)
    {
        ret = http_upload_audio_ll(file_path);
        if (ret == 0)
        {
            break;
        }
        retry--;
        printf("http upload audio retry cnt %d\n", 6 - retry);
    }
    return ret;
}

unsigned char get_download_file_state(void)
{
    return g_download_file_state;
}

/* 使用异步方式上传LOG */
int http_upload_log(const char *file_path)
{
    int ret = 0;
    size_t length = strlen(file_path) + 1;

    ret = http_send_data_to_server(file_path, length, HTTP_UPLOAD_LOG, 0);
    return ret;
}