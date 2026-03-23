/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define http_info printf
#define http_err printf
#define http_debug

#ifndef HTTP_MIN
#define HTTP_MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef HTTP_MAX
#define HTTP_MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

int HttpTcpConnWrapper(HttpClient *client, const char *host);
int HttpTcpCloseWrapper(HttpClient *client);
int HttpTcpSendWrapper(HttpClient *client, const char *data, int length);
int HttpTcpRecvWrapper(HttpClient *client, char *buf, int buflen, int timeout_ms, int *p_read_len);

#if CONFIG_HTTP_SECURE
int HttpSslConnWrapper(HttpClient *client, const char *host);
int HttpSslCloseWrapper(HttpClient *client);
int HttpSslSendWrapper(HttpClient *client, const char *data, size_t length);
int HttpSslRecvWrapper(HttpClient *client, char *buf, int buflen, int timeout_ms, int *p_read_len);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HTTP_CLIENT_WRAPPER_H */