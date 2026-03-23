/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_COMMON_H
#define MSG_CENTER_COMMON_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t msg_center_ping_rec(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_COMMON_H */
