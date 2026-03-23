/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NET_ERRCODE_H
#define NET_ERRCODE_H

/** @brief   http requst type */
typedef enum {
    NET_SUCCESS               =  0,
    NET_PARAMETER_ERROR       =  401,
    NET_PERMISSION_DENIED     =  201,
    NET_UNSUPPORTED_PROTOCOL  =  2300001,
    NET_BAD_URL_FORMAT        =  2300003,
    NET_PROXY_RESOLVE_ERROR   =  2300005,
    NET_HOST_RESOLVE_ERROR    =  2300006,
    NET_SERVER_CONNECT_ERROR  =  2300007,
    NET_WEIRD_SERVER_REPLY    =  2300008,
    NET_REMOTE_ACCESS_DENIED  =  2300009,
    NET_HTTP2_FRAME_ERROR     =  2300016,
    NET_PARTIAL_FILE          =  2300018,
    NET_WRITE_DATA_ERROR      =  2300023,
    NET_UPLOAD_FAILED         =  2300025,
    NET_OPEN_FILE_ERROR       =  2300026,
    NET_OUT_OF_MEMORY         =  2300027,
    NET_TIMEOUT_REACHED       =  2300028,
    NET_MAX_REDIRECTS         =  2300047,
    NET_SERVER_NO_RESPONSE    =  2300052,
    NET_SEND_DATA_ERROR       =  2300055,
    NET_RECEIVE_DATA_ERROR    =  2300056,
    NET_LOCAL_SSL_ERROR       =  2300058,
    NET_SSL_CIPHER_ERROR      =  2300059,
    NET_SSL_PEER_ERROR        =  2300060,
    NET_BAD_CONTENT_ENCODING  =  2300061,
    NET_FILE_SIZE_EXCEEDED    =  2300063,
    NET_DISK_FULL             =  2300070,
    NET_REMOTE_FILE_EXISTS    =  2300073,
    NET_SSL_CA_ERROR          =  2300077,
    NET_REMOTE_FILE_NOT_FOUND =  2300078,
    NET_AUTH_FUNCTION_ERROR   =  2300094,
    NET_UNKNOWN_ERROR         =  2300999,
} HTTP_ERRCODE;

typedef enum {
    WEBSOCKET_SUCCESS   =  0,
    WEBSOCKET_CONNECT_FAILED   =  200,
    WEBSOCKET_URL_ERROR        =  2302001,
    WEBSOCKET_CERT_MISSING     =  2302002,
    WEBSOCKET_CONN_EXISTS      =  2302003,
    WEBSOCKET_NIC_ERROR        =  2302004,
    WEBSOCKET_PORT_ERROR       =  2302005,
    WEBSOCKET_DOMAIN_DENIED    =  2302998,
    WEBSOCKET_INTERNAL_ERROR   =  2302999,
    WEBSOCKET_AUTH_FAILED      =  201,
    WEBSOCKET_API_AUTH_FAILED  =  202,
    WEBSOCKET_FUNC_DENIED      =  203,
    WEBSOCKET_PARAM_ERROR      =  401,
    WEBSOCKET_API_UNSUPPORTED  =  801
} WEBSOCKET_ERRCODE;

#endif /* HTTPCLIENT_H */