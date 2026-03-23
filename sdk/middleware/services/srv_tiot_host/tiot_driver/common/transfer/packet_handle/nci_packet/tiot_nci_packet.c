/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-04-23, Create file. \n
 */

#include "tiot_board_log.h"
#include "tiot_xfer_utils.h"

/* use default data store. */

int32_t tiot_nci_rx_data_out(tiot_xfer_manager *xfer, uint8_t *data, uint32_t len, const tiot_xfer_rx_param *param)
{
    int32_t ret;
    const uint32_t nci_head_len = 3;
    if (len <= nci_head_len) {
        tiot_print_err("buf_len <= NCI_HEAD_LEN(%u)\n", nci_head_len);
        return 0;
    }

    osal_mutex_lock(&xfer->read_mutex); /* 防止多个线程同时读 */

    ret = tiot_xfer_read_out(xfer, data, nci_head_len, param->timeout);
    if (ret == (int32_t)nci_head_len) {
        uint8_t *pars_buf = data; // 确保之后的计算是正数
        uint32_t payload_len = pars_buf[nci_head_len - 1];

        if (payload_len > len - nci_head_len) {
            payload_len = len - nci_head_len;
        }

        ret = tiot_xfer_read_out(xfer, data + nci_head_len, payload_len, param->timeout);

        if (payload_len != pars_buf[nci_head_len - 1]) {
            ret = ERRCODE_TIOT_XFER_INVALID_PAYLOAD_LEN;
        } else if (ret >= 0) {
            ret += nci_head_len;
        }
    }

    osal_mutex_unlock(&xfer->read_mutex);
    return ret;
}
