/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides ohos sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "stdlib.h"
#include "stdbool.h"
#include "securec.h"
#include "common_def.h"
#include "debug_print.h"
#include "app_at_process.h"
#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
#include "imsa_interface.h"
#endif
#ifdef SUPPORT_VOLTE_AUTOTEST
#include "ril_interface.h"
#endif

#include "volte_at_process.h"
#define RIL_CMD_BACK_DATA_SIZE     512

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
static uint32_t imsa_pip_chan_test_at_handle(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }

    return imsa_pip_chan_at_test(para, para_len);
}
#endif

#ifdef SUPPORT_VOLTE_AUTOTEST
int32_t ims_autotest_rsp_fun(uint8_t *data, size_t size)
{
    char rsp_str[RIL_CMD_BACK_DATA_SIZE] = {0};
    if (memcpy_s(rsp_str, RIL_CMD_BACK_DATA_SIZE, data, size) != EOK) {
        factory_test_print(FT_RETURN_SUCC, "ims autotest at rsp fail!\r\n");
        return ERRCODE_FAIL;
    }
    factory_test_print(FT_RETURN_SUCC, "ims autotest at rsp: %s\r\n", rsp_str);
    return ERRCODE_SUCC;
}

void ims_autotest_init(void)
{
    //ril_register_at_reply_callback(ims_autotest_rsp_fun);
}

static uint32_t ims_auto_test_at_handle(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    unused(para_len);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    char at_str[RIL_SEND_CMD_DATA_SIZE] = {0};
    factory_test_print(FT_RETURN_SUCC, "ims autotest cmd:%s\r\n", (char *)para);
    if (snprintf_s(at_str, RIL_SEND_CMD_DATA_SIZE, RIL_SEND_CMD_DATA_SIZE - 1, "%s\r\n", para) <= 0) {
        factory_test_print(FT_RETURN_SUCC, "ims autotest cmd invalid param!\r\n");
        return ERRCODE_INVALID_PARAM;
    }

    // 异步执行at
    int32_t ret = ril_at_cmd_exc((char *)at_str);
    // 串口打印
    factory_test_print(FT_RETURN_SUCC, "ims autotest ret:%d\r\n", ret);

    return ERRCODE_SUCC;
}
#endif

#ifdef CONFIG_FT_DITING
static uint32_t ims_factory_test_handle(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(para);
    unused(para_len);
    uint32_t ret = 0;
    if (str_cmd == NULL) {
        return ERRCODE_FAIL;
    }

    if (strncmp(str_cmd, "VOLTEFTUART", sizeof("VOLTEFTUART")) == 0) {
        ret = ims_uart_factory_test_handle();
        if (ret != 0) {
            factory_test_print(FT_RETURN_SUCC, "volte uart production test ret:%d\r\n", ret);
        }
    } else if (strncmp(str_cmd, "VOLTEFTSPI", sizeof("VOLTEFTSPI")) == 0) {
        ret = ims_spi_factory_test_handle();
        if (ret != 0) {
            factory_test_print(FT_RETURN_SUCC, "volte spi production test ret:%d\r\n", ret);
        }
    } else {
        factory_test_print(FT_RETURN_SUCC, "volte production test cmd err:%s\r\n", str_cmd);
    }
    return ERRCODE_SUCC;
}
#endif

static const at_cmd_table_t g_volte_cmd[] = {
#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
    { "AT^VOLTECHAN",      imsa_pip_chan_test_at_handle,     "" },
#endif
#ifdef SUPPORT_VOLTE_AUTOTEST
    { "AT^IMSAUTOTEST",      ims_auto_test_at_handle,     "" },
#endif
#ifdef CONFIG_FT_DITING
    { "AT^VOLTEFTUART",           ims_factory_test_handle,    "VOLTEFTUART"},
    { "AT^VOLTEFTSPI",            ims_factory_test_handle,    "VOLTEFTSPI"},
#endif
};

at_cmd_table_t *get_volte_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = array_size(g_volte_cmd);
    return (at_cmd_table_t *)g_volte_cmd;
}
