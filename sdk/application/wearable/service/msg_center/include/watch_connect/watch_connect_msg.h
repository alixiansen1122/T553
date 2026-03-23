/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: watch connect msg
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef WATCH_CONNECT_MSG_H
#define WATCH_CONNECT_MSG_H
#include <stdint.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define BUNDLE_NAME_LENGTH_MAX 127
#define FINGERPRIN_LENGTH_MAX 64
#define FILE_DESC_NAME_LENGTH 128
#define FILE_DESC_PATH_LENGTH 128
#define WATCH_CONNECT_HARMONY_API_LEN 14
#define WATCH_CONNECT_HARMONY_API "internal://app/"
#define APP_DATA_PATH "/user/app/user/ace/data/"

typedef enum {
    WATCH_CONNECT_PING_NOT_INSTALL = 200,
    WATCH_CONNECT_PING_INSTALL_NOT_STARTED = 201,
    WATCH_CONNECT_PING_INSTALL_AND_STARTED = 202,
    WATCH_CONNECT_PING_OTHER_ERROR = 203,
    WATCH_CONNECT_PING_MAX
} WatchConnectPingStatus;

typedef enum {
    WATCH_CONNECT_INSTALL_NOT_INSTALLED = 200,
    WATCH_CONNECT_INSTALL_OTHER_ERROR = 203,
    WATCH_CONNECT_INSTALL_INSTALLED = 208,
    WATCH_CONNECT_INSTALL_MAX
} WatchConnectInstallStatus;

typedef enum {
    WATCH_CONNECT_PING_PHONE_APP_NOT_INSTALL = 204,
    WATCH_CONNECT_PING_PHONE_APP_INSTALL = 205,
    WATCH_CONNECT_PING_PHONE_MAX
} WatchConnectPingPhoneStatus;

typedef enum {
    WATCH_CONNECT_SEND_MESSAGE_FAIL = 206,
    WATCH_CONNECT_SEND_MESSAGE_SUCCESS = 207,
    WATCH_CONNECT_SEND_OFFLINE_MESSAGE_SUCCESS = 209,
    WATCH_CONNECT_SEND_MESSAGE_MAX
} WatchConnectMessageStatus;

typedef enum {
    WATCH_CONNECT_DATA_TYPE_MESSAGE,
    WATCH_CONNECT_DATA_TYPE_FILE,
} WatchConnectDataType;

typedef enum {
    WATCH_CONNECT_MSG_TYPE_REQ = 1,
    WATCH_CONNECT_MSG_TYPE_ACK,
} WatchConnectMsgType;

typedef struct {
    uint8_t bundleName[BUNDLE_NAME_LENGTH_MAX + 1];
    uint8_t bundleNameLen;
    uint8_t fingerPrint[FINGERPRIN_LENGTH_MAX + 1];
    uint8_t fingerPrintLen;
    bool setFlag;
} WatchConnectPeerInfo;;

typedef struct {
    char *message;
    uint32_t length;
    uint32_t successCallbackFunc;
    uint32_t failCallbackFunc;
    uint32_t onPingResultCallbackFunc;
    uint32_t context;
} WatchConnectPingInfo;

typedef struct {
    char *message;
    uint32_t length;
    uint32_t successCallbackFunc;
    uint32_t failCallbackFunc;
    uint32_t onSendResultCallbackFunc;
    uint32_t onSendProgressCallbackFunc;
    uint32_t context;
} WatchConnectSendMsgInfo;

typedef struct {
    char *message;
    uint32_t length;
    char *fileName;
    uint32_t fileNameLength;
    char *filePath;
    uint32_t filePathLength;
    uint32_t successCallbackFunc;
    uint32_t failCallbackFunc;
    uint32_t onSendResultCallbackFunc;
    uint32_t onSendProgressCallbackFunc;
    uint32_t context;
} WatchConnectSendFileInfo;

typedef struct {
    uint32_t fileSize;
    uint8_t fileName[FILE_DESC_NAME_LENGTH];
    uint8_t filePath[FILE_DESC_PATH_LENGTH];
} WatchConnectSendFileDesc;

typedef struct {
    char msgType;
    uint16_t seqId;
    uint16_t srcPkgNameLen;
    char *srcPkgName;
    uint16_t destPkgNameLen;
    char *destPkgName;
    uint16_t payloadLen;
    char *payload;
    uint32_t result;
} WatchConnectTlvMsgInfo;

typedef struct {
    uint32_t totalSize;
    uint32_t freeSize;
} WatchConnectSpaceInfo;

void WatchConnectResourceInit(void);
void WatchConnectUpdatePeerInfo(const char *bundleName, uint8_t bundleNameLen, const char *fingerPrint,
    uint8_t fingerPrintLen);
int32_t WatchConnectGetPhoneAppVersion(void);
int32_t WatchConnectPingPhoneApp(WatchConnectPingInfo *pingInfo);
int32_t WatchConnectSendMsg(WatchConnectSendMsgInfo *msgInfo);
int32_t WatchConnectSendFile(WatchConnectSendFileInfo *fileInfo);
void WatchConnectHandleMsgResponse(uint8_t cmdType, WatchConnectTlvMsgInfo *tlvMsgInfo);
void WatchConnectHandleFileTransferResponse(uint8_t cmdType, WatchConnectTlvMsgInfo *tlvMsgInfo);
uint32_t WatchConnectGetPingStatus(WatchConnectTlvMsgInfo *msgInfo);
uint32_t WatchConnectHandleSendMsg(WatchConnectTlvMsgInfo *msgInfo);
uint32_t WatchConnectCheckAppInstall(WatchConnectTlvMsgInfo *msgInfo);
int32_t WatchConnectGetVersionCode(WatchConnectTlvMsgInfo *msgInfo);
uint32_t WatchConnectSendPkt(uint8_t cmdId, uint8_t type, WatchConnectTlvMsgInfo *msgInfo);
void WatchConnectSendAckPkt(uint8_t cmdId, uint8_t type, WatchConnectTlvMsgInfo *msgInfo);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif