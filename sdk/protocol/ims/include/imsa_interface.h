/**
 * Copyright (c) @CompanyNameMagicTag 2023-2025. All rights reserved.
 * Description: Provides imsa api header.
 */

#ifndef    IMSA_INTERFACE_H
#define    IMSA_INTERFACE_H

#include "errcode.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef enum : uint8_t {
    IMS_NETIF_NUM_DATA = 0,
    IMS_NETIF_NUM_NORMAL = 9,
    IMS_NETIF_NUM_EMC = 10,
    IMS_NETIF_NUM_BUTT = 0xFF,
} ims_netif_num;

typedef enum : uint8_t {
    SPI_DATA_HEADER = 0,
    SPI_FT_HEADER = 1,
    SPI_HEADER_BUTT = 0xFF,
} spi_header_type;

uint32_t imsa_process_thread(void *data);
errcode_t imsa_process_task_init(void);

uint32_t imsa_pip_task_message_proc(void *param);
errcode_t imsa_pip_task_init(void);

uint32_t imsa_pip_spi_message_proc(void *param);
errcode_t imsa_pip_spi_init(void);
void imsa_pip_gpio_resum(void);

void uapi_imsa_pip_receive_data_uart_cb(const uint8_t *data, uint16_t length, bool remaining);
void uapi_volte_logfile_open(uint8_t flag);

#ifdef SUPPORT_IMS_CAT1_CHANNEL_TEST
uint32_t imsa_pip_chan_at_test(uint8_t *para, uint32_t para_len);
#endif
#ifdef SUPPORT_IMS_FUZZ_TEST
uint32_t imsa_pip_chan_fuzz_fun_at_test(uint8_t index);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
