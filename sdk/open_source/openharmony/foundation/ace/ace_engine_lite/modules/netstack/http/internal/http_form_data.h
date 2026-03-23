/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef HTTP_FORM_DATA
#define HTTP_FORM_DATA

#ifdef __cplusplus
extern "C" {
#endif

#define FORM_DATA_PATH_MAXLEN 64
#define CLIENT_FORM_DATA_NUM 1

#define FORM_DATA_BOUNDARY "----WebKitFormBoundarypXEfCkZnHjoOSnPc0"
#define FORM_DATA_BOUNDARY_START "\r\n--" FORM_DATA_BOUNDARY
#define FORM_DATA_BOUNDARY_END FORM_DATA_BOUNDARY_START "--\r\n"

typedef struct HttpFormDataNode HttpFormDataNode;
struct HttpFormDataNode {
    HttpFormDataNode *next;
    int isFile;
    char filePath[FORM_DATA_PATH_MAXLEN];
    char *formatContent;
    int formatLen;
};

typedef struct {
    int isUsed;
    HttpFormDataNode *formDataNode;
    HttpClient *clientData;
} FormDataInfo;

void HttpClientClearFormData(HttpClientData *clientData);
int HttpClientFormdataLen(HttpClientData *clientData);
int HttpClientSendFormdata(HttpClient *client, HttpClientData *clientData);

#ifdef __cplusplus
}
#endif

#endif