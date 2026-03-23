/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef ALIPAY_APP_H
#define ALIPAY_APP_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_alipay_type_id {
    MSGCENTER_TYPE_ID_ALIPAY_NETWORK_STATUS = 0x1,
    MSGCENTER_TYPE_ID_ALIPAY_HOST_INFO,
    MSGCENTER_TYPE_ID_ALIPAY_CONNECT,
    MSGCENTER_TYPE_ID_ALIPAY_TCPDATA_RECV,
    MSGCENTER_TYPE_ID_ALIPAY_CLOSE,
    MSGCENTER_TYPE_ID_ALIPAY_TIME_SYNC = 0x7,
} msg_center_alipay_type_id_t;

errcode_t msg_center_alipay_network_status(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
uint8_t msg_center_alipay_get_network_status(void);

int msg_center_alipay_hostent_request(void *usr_data, uint16_t usr_len);
errcode_t msg_center_alipay_hostent_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
int msg_center_alipay_hostent_reponse(unsigned char *ip);

int msg_center_alipay_connect_request(void *usr_data, uint16_t usr_len);
errcode_t msg_center_alipay_connect_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
int msg_center_alipay_connect_reponse(void);

int msg_center_alipay_tcpdata_request(void *usr_data, uint16_t usr_len);
int msg_center_alipay_tcpdata_wait(uint32_t timeout);
errcode_t msg_center_alipay_tcpdata_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
int msg_center_alipay_tcpdata_read(void *mem, int len);

int msg_center_alipay_close_request(void);
errcode_t msg_center_alipay_close(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

errcode_t msg_center_alipay_time_sync(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* ALIPAY_APP_H */
