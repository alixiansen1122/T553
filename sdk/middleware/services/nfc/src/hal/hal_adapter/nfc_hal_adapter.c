/**
* Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
* Description: nfc hal adapter interface. \n
* Author: @CompanyNameTag \n
* Date: 2024-05-24 \n
*/

#include "nfc_hal_adapter.h"
#include "tiot_service_interface.h"
#include "soc_osal.h"

tiot_handle g_tiotHandle = 0;

int HW_NFC_Open(void)
{
    PS_PRINT_INFO("hwnfc open\n");

#ifndef CONFIG_PRODUCT_EVB_DITING
    /* 初始化驱动. */
    if (tiot_service_init() != NFC_SUCC) {
        PS_PRINT_ERR("nfc_driver_init failed\n");
        return -NFC_EFAIL;
    }
#endif

    g_tiotHandle = tiot_service_open("ca01", NULL);
    if (g_tiotHandle == 0) {
        PS_PRINT_ERR("\nopen tiot service fail\n");
        return -NFC_EFAIL;
    }
    PS_PRINT_ERR("\nopen tiot service success\n");

    return 0;
}

int HW_NFC_Close(void)
{
    PS_PRINT_INFO("hwnfc close\n");
    if (g_tiotHandle == 0) {
        PS_PRINT_ERR("No valid handle, need open first!");
        return -NFC_EFAIL;
    }
    tiot_service_close(g_tiotHandle);

#ifndef CONFIG_PRODUCT_EVB_DITING
    tiot_service_deinit();
#endif

    return 0;
}

int HW_NFC_Write(uint8_t *buf, uint16_t count)
{
    if (g_tiotHandle == 0) {
        PS_PRINT_ERR("No valid handle, need open first!");
        return -NFC_EFAIL;
    }
    return tiot_service_write(g_tiotHandle, buf, count);
}

int HW_NFC_NciRead(uint8_t *buf, uint16_t bufLen, unsigned long timeout)
{
    if (g_tiotHandle == 0) {
        PS_PRINT_ERR("No valid handle, need open first!");
        return -NFC_EFAIL;
    }

    if (bufLen <= NCI_HEAD_LEN) {
        PS_PRINT_WARNING("bufLen <= NCI_HEAD_LEN\n");
        return 0;
    }

    return tiot_service_read(g_tiotHandle, buf, bufLen, timeout);
}

void NFC_HAL_Msleep(unsigned int msecs)
{
    osal_msleep(msecs);
}