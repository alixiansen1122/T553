/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
 
#ifndef NET_WEBSOCKET_OPTS_H
#define NET_WEBSOCKET_OPTS_H

#define WEBSOCKET_RECV_BUFF_SIZE (8 * 1024)

#define WS_MAX_THRESH_VAL (8 * 1024) /* common TTS play size */

#define WS_MAX_CLIENT_NUM 4

#define WS_MAX_DATA_LENGTH (20 * 1024)

#define WSS_READ_TIMEOUT_MS 100

#define WS_THREAD_STACK_SIZE (6 * 1024)

#define WS_THREAD_SLEEP_MS 100

#endif