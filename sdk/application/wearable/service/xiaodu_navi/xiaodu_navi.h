/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef XIAODU_ANVI_H
#define XIAODU_ANVI_H

#include "errcode.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum msg_center_navigation_type_id {
    MSGCENTER_TYPE_ID_NAVIGATION_START = 0x1,
    MSGCENTER_TYPE_ID_NAVIGATION_RECV,
} msg_center_navigation_type_id_t;

errcode_t msg_center_navigation_start(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);
errcode_t msg_center_navigation_recv(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* BAIDU_APP_H */
