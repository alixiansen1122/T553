#include "thread_init.h"
#include "tcxo.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#include "modem_ctrl.h"

const char *rf_open = "AT+CFUN=1\r\n";
const char *rf_register = "AT+CGATT=1\r\n";
// 0 -> not registered, 1 -> registered
uint8_t modem_get_reg_status(void)
{
    int32_t ret;
    uint8_t status = 0;
    ret = ril_get_radio_reg_status(&status);
    if (ret!= 0) {
        printf("ril_get_radio_reg_status fail\n");
        return 0;
    }

    return status;
}

soc_ue_cell_radio_info modem_get_radio_info(void)
{
     soc_ue_cell_radio_info radio_info ={0};
     int32_t ret;

     ret = ril_get_radio_signal_info(&radio_info);
     if (ret!= 0) {
         printf("ril_get_radio_signal_info fail\n");
     }
     else
     return radio_info;
}

void modem_register_init(void)
{
    int32_t ret;
    ret = ril_at_cmd_exc(rf_open);
    if (ret!= 0) {
        printf("rf_open fail\n");
        return;
    }

    ret = ril_at_cmd_exc(rf_register);
    if (ret!= 0) {
        printf("rf_register fail\n");
        return;
    }

    printf("modem register init success\n");
}
