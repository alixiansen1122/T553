/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast service api
 * Author: CompanyName
 * Create: 2021-11-11
 */

#ifndef MSG_CENTER_CMD_H
#define MSG_CENTER_CMD_H

#include "msg_center_protocol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

msg_center_cmd_map_t *msg_center_get_cmd_tbl_ops(uint8_t idx);
uint8_t msg_center_get_cmd_tbl_size(void);
errcode_t msg_center_register_cmd(uint8_t cmd, msg_center_pkt_recv_hook handler);
errcode_t msg_center_unregister_cmd(uint8_t cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MSG_CENTER_CMD_H */
