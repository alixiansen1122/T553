/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#ifndef _SAMPLE_PHONE_H_
#define _SAMPLE_PHONE_H_

#include "mem_addr.h"
#include "sample_audio_api.h"

#define ADP_HANDLE_SHARE_ADDR SOC_IPC_ADP_SHARE_BASE_ADDR
#define WAKE_MAGIC_VAL (0x9876fedc)
#define WAKE_MAGIC2_VAL (0x6789fedc)
#define ACK_MAGIC_VAL (0xedcb1234)

typedef struct {
    td_handle h_adp_uplnk;
    td_handle h_adp_dnlnk;
    td_u32 adp_wake_magic;
} adp_handle_share;

static inline td_void adp_handle_share_set(td_handle h_adp_uplnk, td_handle h_adp_dnlnk, td_u32 stream_type)
{
    volatile adp_handle_share *adp_handle_t = (volatile adp_handle_share *)(td_uintptr_t)ADP_HANDLE_SHARE_ADDR;
    sap_alert_log_info("[audio] handles:");
    sap_alert_log_u32(h_adp_uplnk);
    sap_alert_log_u32(h_adp_dnlnk);

    adp_handle_t->h_adp_uplnk = h_adp_uplnk;
    adp_handle_t->h_adp_dnlnk = h_adp_dnlnk;
    adp_handle_t->adp_wake_magic = (stream_type == 0) ? WAKE_MAGIC_VAL : WAKE_MAGIC2_VAL;
}

#endif /* _SAMPLE_PHONE_H_ */
