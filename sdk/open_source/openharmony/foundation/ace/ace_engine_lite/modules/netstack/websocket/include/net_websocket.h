/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NET_WEBSOCKET_H
#define NET_WEBSOCKET_H

#include <stdio.h>
#include <stdbool.h>
#include "net_websocket_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Constructor of websocket.
 *
 * @param onMessage Callback function invoked when a message is received.
 * @param onClose Callback function invoked when a connection closing message is closed.
 * @param onError Callback function invoked when a connection error message is received.
 * @param onOpen Callback function invoked when a connection setup message is received.
 * @return client fd >= 0 if success; -1 otherwise
 */
int WebSocketClientConstructor(WebSocketOnOpenCallback onOpen, WebSocketOnMessageCallback onMessage,
    WebSocketOnErrorCallback onError, WebSocketOnCloseCallback onclose);

#ifdef WEBSOCKET_SSL_ENABLE
/**
 * @brief Set server certificate for ssl connection.
 * @param client client fd.
 * @param serverCert Server certificate.
 * @param serverCertLen The length of the server certificate caculated by sizeof
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebSocketClientSetServerCert(int client, const char *serverCert, int serverCertLen);
#endif

/**
 * @brief Connects the client to the server.
 *
 * @param client client fd.
 * @param url URL for the client to connect to the server.
 * @param option string for websocket custom header, must end with '\r\n'.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebSocketClientConnect(int client, const char *url, const char *option);

/**
 * @brief Sends data from the client to the server.
 *
 * @param client Client fd.
 * @param text Text string sent by the client.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebSocketClientSendText(int client, const char *text, size_t length);

/**
 * @brief Send bin header to connect socket.
 * @detailed Thread safe method.
 * @param client client fd.
 * @param data header data for sending.
 * @param length length of header.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebSocketClientSendBinStart(int client, const char *data, size_t length);

/**
 @brief Send bin content to connect socket.
 @detailed Thread safe method.
 @param client Socket object.
 @param data content for sending.
 @param len length of binary content.
 @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
*/
int WebSocketClientSendBinContinue(int client, const char *data, size_t length);

/**
 @brief Send final bin to connect socket.
 @detailed Thread safe method.
 @param client Socket object.
 @param data bin string for sending.
 @param length length of final.
 @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
*/
int WebSocketClientSendBinEnd(int client, const char *data, size_t length);

/**
 * @brief Closes a WebSocket connection.
 * @detailed do not use it under onMessage/onOpen/OnError callback
 * @param client Client fd.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebSocketClientClose(int client);

/**
 @brief Check is client has connection to host and handshake(sucessfully done).
 @detailed Thread safe getter.
 @param client Client fd.
 @return trw_true - connected to host and handshacked, otherwice false.
 */
bool WebSocketClientIsConnected(int client);

/**
 @brief suspend a WebSocket connection.
 @param client Client fd.
 @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebsocketSuspend(int client);

/**
 @brief resume a WebSocket connection.
 @param client Client fd.
 @return 0 if success; non-0 otherwise. For details about error codes, see {@link WebsocketErrCode}.
 */
int WebsocketResume(int client);

#ifdef __cplusplus
}
#endif

#endif
