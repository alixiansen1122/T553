/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NET_WEBSOCKET_TYPE_H
#define NET_WEBSOCKET_TYPE_H

#include <stdlib.h>
#include <stdbool.h>
/**
 * @addtogroup netstack
 * @{
 *
 * @brief Provides C APIs for the WebSocket client module.
 *
 */

/**
 * @file net_websocket_type.h
 * @brief Defines the data structure for the C APIs of the WebSocket client module.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the parameters for the connection error reported by the server.
 */
struct WebSocketErrorResult {
    /** Error code */
    int errorCode;
    /** Error message */
    const char *errorMessage;
};

/**
 * @brief Defines the callback function invoked when an <b>open</b> message is received.
 *
 * @param client WebSocket client fd.
 * @since 11
 * @version 1.0
 */
typedef void (*WebSocketOnOpenCallback)(int client);

/**
 * @brief Defines the callback function invoked when data is received.
 *
 * @param client WebSocket client fd.
 * @param data Data received by the WebSocket client.
 * @param length Length of the data received by the WebSocket client.
 * @param isFinish data received is finished or not
 * @param isBinary data received is binary or not
 * @since 11
 * @version 1.0
 */
typedef void (*WebSocketOnMessageCallback)(int client, const char *data, size_t length, bool isFinished, bool isBinary);

/**
 * @brief Defines the callback function invoked when an error message is received.
 *
 * @param client WebSocket client.
 * @param errorResult Content of the connection error message received by the WebSocket client.
 * @since 11
 * @version 1.0
 */
typedef void (*WebSocketOnErrorCallback)(int client, struct WebSocketErrorResult errorResult);

/**
 * @brief Defines the callback function invoked when a <b>close</b> message is received.
 *
 * @param client WebSocket client.
 */
typedef void (*WebSocketOnCloseCallback)(int client, struct WebSocketErrorResult errorResult);


typedef enum WebSocketErrCode {
    /**
     * Operation success.
     */
    WEBSOCKET_OK = 0,

    /**
     * @brief The WebSocket client is null.
     */
    WEBSOCKET_CLIENT_NULL = (-2),

    /**
     * @brief A WebSocket client is not created.
     */
    WEBSOCKET_CLIENT_NOT_CREATED = (-3),

    /**
     * @brief An error occurs while setting up a WebSocket connection.
     */
    WEBSOCKET_CONNECTION_ERROR = (-4),

    /**
     * @brief An error occurs while parsing WebSocket connection parameters.
     */
    WEBSOCKET_CONNECTION_PARSE_URL_ERROR = (-5),

    /**
     * @brief The memory is insufficient for creating a context during WebSocket connection setup.
     */
    WEBSOCKET_CONNECTION_NO_MEMORY = (-6),

    /**
     * @brief The WebSocket connection is closed by the peer.
     */
    WEBSOCKET_CONNECTION_CLOSED_BY_PEER = (-7),

    /**
     * @brief The WebSocket connection is destroyed.
     */
    WEBSOCKET_DESTROYED = (-8),

    /**
     * @brief An incorrect protocol is used for WebSocket connection.
     */
    WEBSOCKET_PROTOCOL_ERROR = (-9),

    /**
     * @brief The memory for the WebSocket client to send data is insufficient.
     */
    WEBSOCKET_NO_MEMORY = (-10),

    /**
     * @brief The data sent by the WebSocket client is null.
     */
    WEBSOCKET_SEND_DATA_NULL = (-11),

    /**
     * @brief The length of the data sent by the WebSocket client exceeds the limit.
     */
    WEBSOCKET_DATA_LENGTH_EXCEEDED = (-12),

    /**
     * @brief The queue length of the data sent by the WebSocket client exceeds the limit.
     */
    WEBSOCKET_QUEUE_LENGTH_EXCEEDED = (-13),

    /**
     * @brief The WebSocket client is not connected.
     */
    WEBSOCKET_NO_CONNECTION = (-14),

    WEBSOCKET_MISSED_PARAM = (-15),

    WEBSOCKET_ERROR_SEND_HANDSHAKE = (-16),

    WEBSOCKET_ERROR_PARSE_HANDSHAKE = (-17),

    WEBSOCKET_ERROR_READ_WRITE_SOCKET = (-18),

    WEBSOCKET_ERROR_CONNECT_TO_HOST = (-19),

    WEBSOCKET_ERROR_CONNECT_CLOSED = (-20),

    WEBSOCKE_ERROR_CONNECTION_NOT_CLOSED = (-21),
} WebSocketErrCode;


#ifdef __cplusplus
}
#endif

#endif // NET_WEBSOCKET_TYPE_H