/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-12-05, Create file. \n
 */

#include "tiot_firmware.h"
#include "tiot_firmware_utils.h"
#include "tiot_board_log.h"
#include "tiot_cfg_handle_xci.h"

#define TIOT_FW_XCI_ACK         'a'
#define TIOT_FW_XCI_NAK         'n'

#define TIOT_FW_XCI_ACK_MAX     128

#define READ_TIMEOUT_MS     1000

typedef enum {
    TIOT_FW_EXTCMD_ACK = 0x0,
    TIOT_FW_EXTCMD_CUSTOMIZE = 0x1,
    TIOT_FW_EXTCMD_NUM
} tiot_fw_ext_cmd_xci;

typedef int32_t (*xci_ext_cmd_handle_func)(tiot_fw *fw, const uint8_t *cmd, uint16_t len);
typedef int32_t (*xci_pre_ext_cmd_handle_func)(tiot_fw *fw, uint8_t *cmd, uint16_t *len);

static int32_t tiot_fw_ack_handle_xci(tiot_fw *fw, const uint8_t *cmd, uint16_t len);
static int32_t tiot_fw_customize_ext_cmd_handle_xci(tiot_fw *fw, uint8_t *cmd, uint16_t *len);

static const xci_ext_cmd_handle_func tiot_xci_ext_cmd_table[TIOT_FW_EXTCMD_NUM] = {
    tiot_fw_ack_handle_xci,
    tiot_fw_ack_handle_xci,
};

static const xci_pre_ext_cmd_handle_func tiot_xci_pre_ext_cmd_table[TIOT_FW_EXTCMD_NUM] = {
    NULL,
    tiot_fw_customize_ext_cmd_handle_xci,
};

static int32_t tiot_fw_ack_handle_xci(tiot_fw *fw, const uint8_t *cmd, uint16_t len)
{
    tiot_unused(cmd);
    tiot_unused(len);
    uint16_t i;
    int32_t ret = 0;
    uint16_t ack_len = 0;
    uint8_t ack_content[TIOT_FW_XCI_ACK_MAX] = { 0 };
    // 读ACK头
    ret = tiot_fw_read_until(fw, (uint8_t *)&ack_len, sizeof(uint16_t), READ_TIMEOUT_MS);
    if ((ret <= 0) || ((uint32_t)ret != sizeof(uint16_t))) {
        return ret;
    }
    if (ack_len > sizeof(ack_content)) {
        return ERRCODE_TIOT_FW_LOAD_INVALID_ACK_LEN;
    }
    // 读ACK内容
    ret = tiot_fw_read_until(fw, ack_content, ack_len, READ_TIMEOUT_MS);
    if (ret != ack_len) {
        return ret;
    }
    if ((ack_len == sizeof(uint8_t)) && (ack_content[0] != TIOT_FW_XCI_ACK)) {
        return ERRCODE_TIOT_FW_LOAD_INVALID_ACK;
    }
    if (ack_len != sizeof(uint8_t)) {
        tiot_print_info("[TIoT:fw]ack content: ");
        for (i = 0; i < ack_len; i++) {
            tiot_print_info("0x%x ", ack_content[i]);
        }
        tiot_print_info("\r\n");
    } else {
        tiot_print_dbg("[TIoT:xci]ack 0x%x", ack_content[0]);
    }
    return ERRCODE_TIOT_SUCC;
}

static int32_t tiot_fw_customize_ext_cmd_handle_xci(tiot_fw *fw, uint8_t *cmd, uint16_t *len)
{
    int32_t ret;
    uint32_t cus_data;

    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    if (ctrl->dev_cus == 0) {
        tiot_print_info("[TIoT:xci] customize is NULL.\r\n");
        return ERRCODE_TIOT_SUCC;
    }

    ret = tiot_firmware_customize_value_get(fw, &cus_data);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }

    tiot_unused(cmd);
    tiot_unused(len);
    tiot_print_err("[TIoT:xci] customize ext cmd no handle.\r\n");
    return ERRCODE_TIOT_SUCC;
}

int32_t tiot_fw_ext_cmd_handle_xci(tiot_fw *fw, uint16_t ext_cmd, const uint8_t *cmd, uint16_t len)
{
    if (ext_cmd >= TIOT_FW_EXTCMD_NUM) {
        tiot_print_err("[TIoT:fw]xci ext cmd exceed max!\r\n");
        return ERRCODE_TIOT_FW_LOAD_INVALID_EXT_CMD;
    }
    return tiot_xci_ext_cmd_table[ext_cmd](fw, cmd, len);
}

int32_t tiot_fw_pre_ext_cmd_handle_xci(tiot_fw *fw, uint16_t ext_cmd, uint8_t *cmd, uint16_t *len)
{
    if (ext_cmd >= TIOT_FW_EXTCMD_NUM) {
        tiot_print_err("[TIoT:fw]xci pre ext cmd exceed max!\r\n");
        return ERRCODE_TIOT_FW_LOAD_INVALID_PRE_EXT_CMD;
    }

    if (tiot_xci_pre_ext_cmd_table[ext_cmd] == NULL) {
        return ERRCODE_TIOT_SUCC;
    }

    return tiot_xci_pre_ext_cmd_table[ext_cmd](fw, cmd, len);
}