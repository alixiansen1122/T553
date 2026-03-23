/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd manager source\n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "soc_osal.h"
#include "common_def.h"
#include "debug_print.h"
#include "at_cmd_api.h"
#include "media_msg_manager.h"
#include "media_at_process.h"
#include "media_rx_process.h"

void media_at_task_init(void)
{
#ifdef SUPPORT_DIAG_V2_PROTOCOL
    uapi_diag_service_register(DIAG_SER_MEDIA_TEST, media_diag_cmd_process);
#endif
}

void media_at_task_process_handler(void *param)
{
    unused(param);
    int32_t ret = OSAL_FAILURE;
    at_cmd_msg_info_t msg_data;
    uint32_t msg_size = sizeof(at_cmd_msg_info_t);

    media_at_task_init();
    while (1) {
        (void)memset_s(&msg_data, sizeof(msg_data), 0, sizeof(msg_data));
        ret = osal_msg_queue_read_copy(get_media_at_queue_id(), &msg_data, &msg_size, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            continue;
        }

        media_at_process(msg_data.payload, msg_data.payload_len);
        osal_kfree(msg_data.payload);
    }
}
