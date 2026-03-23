/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: broadcast feature implement
 * Author: CompanyName
 * Create: 2021-11-11
 */

#include <stdio.h>
#include "securec.h"
#include "msg_center_protocol.h"
#include "wearable_log.h"
#include "osal_list.h"
#include "msg_center_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

static OSAL_LIST_HEAD(g_msg_center_cmds);

errcode_t msg_center_register_cmd(uint8_t cmd, msg_center_pkt_recv_hook handler)
{
    msg_center_cmd_t *inst = (msg_center_cmd_t *)malloc(sizeof(msg_center_cmd_t));
    if (inst == NULL) {
        return ERRCODE_MALLOC;
    }
    memset_s(inst, sizeof(*inst), 0, sizeof(msg_center_cmd_t));
    inst->cmd = cmd;
    inst->handler = handler;
    osal_list_add_tail(&inst->node, &g_msg_center_cmds);
    return ERRCODE_SUCC;
}

errcode_t msg_center_unregister_cmd(uint8_t cmd)
{
    msg_center_cmd_t *found = NULL;
    osal_list_for_each_entry(found, &g_msg_center_cmds, node) {
        if (found->cmd == cmd) {
            break;
        }
    }
    if (found != NULL) {
        osal_list_del(&found->node);
    }
    return ERRCODE_SUCC;
}

static errcode_t msg_center_pkt_dispatch(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    msg_center_cmd_t *cmd = NULL;
    osal_list_for_each_entry(cmd, &g_msg_center_cmds, node) {
        if (cmd->cmd == cmd_id) {
            cmd->handler(cmd_id, type, usr_data, usr_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

static void msg_center_debug_basic_info(diag_ser_frame_t *req, uint8_t type)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER,"msg_center_pkt_proc: module = %d, cmd = %d, type = %d\r\n",
        req->module_id, req->cmd_id, type);
}

errcode_t msg_center_pkt_proc(diag_ser_data_t *data)
{
    diag_ser_frame_t *req = (diag_ser_frame_t *)((uint8_t *)data + sizeof(diag_ser_data_t)); // app pkt
    uint8_t *usr_data = (uint8_t *)((uint8_t *)req + sizeof(diag_ser_frame_t)); // TLV data
    uint16_t size = data->header.length - (uint16_t)sizeof(diag_ser_frame_t); // TLV len
    uint8_t type = ((msg_center_pkt_tlv_t *)usr_data)->type & TLV_NO_CHILD_ONE_BYTE_LEN_MAX;

    msg_center_debug_basic_info(req, type);
    return msg_center_pkt_dispatch(req->cmd_id, type, usr_data, size);
}

#ifdef __cplusplus
}
#endif
