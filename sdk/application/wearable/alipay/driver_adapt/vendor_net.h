/**
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Alipay vendor net adapt.
 * Author:
 * Create:
 */

#include <stdint.h>

#ifndef VENDOR_NET_H
#define VENDOR_NET_H
typedef struct alipay_connect_request {
    uint8_t dst_ip[4];
    uint16_t dsp_port;
} alipay_connect_request;

typedef struct alipay_connect_reponse {
    int32_t ret;
    int32_t fd;
} alipay_connect_reponse;

#endif /* VENDOR_NET_H */