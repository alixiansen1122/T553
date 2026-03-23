/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef __NET_WEBSOCKET_PRIV_H__
#define __NET_WEBSOCKET_PRIV_H__

#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include <unistd.h>
#include <errno.h>

#include "net_websocket.h"
#include "net_websocket_type.h"
#include "net_websocket_opts.h"
#include "net_websocket_thread.h"
#include "net_websocket_frame.h"
#include "net_websocket_list.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

#ifdef WEBSOCKET_SSL_ENABLE
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#endif

typedef int WsSocket;
#define WS_INVALID_SOCKET -1
#define WS_SOCK_CLOSE(sock) closesocket(sock)

#define O_NONBLOCK 04000

#ifdef WEBSOCKET_SSL_ENABLE
typedef struct WsSslStruct {
    mbedtls_ssl_context sslCtx; /* mbedtls ssl context */
    mbedtls_net_context netCtx; /* Fill in socket id */
    mbedtls_ssl_config sslConf; /* SSL configuration */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_x509_crt_profile profile;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
} WsSsl;
#endif

#define WS_MAX_SCHEME_LEN 16

struct WebSocketClient {
    int clientfd;
    int port;
    WsSocket socket;
    char scheme[WS_MAX_SCHEME_LEN];
    char *url;
    char *host;
    char *requestOption;

    char *secWsProtocol;  // "Sec-WebSocket-Protocol" field
    char *secWsAccept;    // "Sec-WebSocket-Accept" field from handshake

    WsThread workThread;

    int command;

    bool isConnected;  // sock connected + handshake done

    /** Pointer to the callback invoked when a connection message is received */
    WebSocketOnOpenCallback onOpen;
    /** Pointer to the callback invoked when a message is received */
    WebSocketOnMessageCallback onMessage;
    /** Pointer to the callback invoked when an error message is received */
    WebSocketOnErrorCallback onError;
    /** Pointer to the callback invoked when a close message is received */
    WebSocketOnCloseCallback onClose;

    uint8_t *received;
    size_t receivedSize;  // size of 'received' memory
    size_t receivedLen;   // length of actualy readed message

    WsList *sendFrames;
    WsList *recvdFrames;

    WsMutex workMutex;
    WsMutex sendMutex;

    mbedtls_entropy_context randomEntropy; /* random entropy */
    mbedtls_ctr_drbg_context randomCtrDrbg;

#ifdef WEBSOCKET_SSL_ENABLE
    const char *serverCert; /**< Server certification. */
    const char *clientCert; /**< Client certification. */
    const char *clientPk;   /**< Client private key. */
    int serverCertLen;      /**< Server certification lenght, server_cert buffer size. */
    int clientCertLen;      /**< Client certification lenght, client_cert buffer size. */
    int clientPkLen;        /**< Client private key lenght, client_pk buffer size. */
    WsSsl *ssl;             /**< Ssl content. */
#endif
    char *recvBuffer;
    int recvBufferSize;

    struct WebSocketErrorResult errorResult;
};

void WebSocketClientClosePriv(struct WebSocketClient *clientPtr);

struct WebSocketClient *WsGetClient(int fd);

int AllocWsClient(void);

int WsSocketSendTextPriv(struct WebSocketClient *clientPtr, const char *text);

int WsClientSendBinStartPriv(struct WebSocketClient *clientPtr, const char *data, size_t length);

int WsClientIdleDataSend(struct WebSocketClient *clientPtr);

#define COMMAND_IDLE -1
#define COMMAND_NONE 0
#define COMMAND_CONNECT_TO_HOST 1
#define COMMAND_SEND_HANDSHAKE 2
#define COMMAND_WAIT_HANDSHAKE_RESPONCE 3
#define COMMAND_INFORM_CONNECTED 4
#define COMMAND_INFORM_DISCONNECTED 5

#define COMMAND_END 9999

#endif
