#ifndef _MODEM_CTRL_H_
#define _MODEM_CTRL_H_

#include "stdlib.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_semaphore.h"
#include "ril_interface.h"



extern uint8_t modem_get_reg_status(void);
soc_ue_cell_radio_info modem_get_radio_info(void);
void modem_register_init(void);
#endif