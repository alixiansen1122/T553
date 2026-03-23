/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt entropy implement.
 *
 * Create: 2024-08-12
*/

#include "library/common.h"

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#include "mbedtls/error.h"
#include "mbedtls/entropy.h"
#include "entropy_poll.h"
#include "trng.h"

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    int ret;

    if (len == 0 || olen == NULL || output == NULL) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    (void)data;

    ret = uapi_drv_cipher_trng_get_random_bytes(output, len);
    if (ret != 0) {
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
    *olen = len;

    return 0;
}

#endif /* MBEDTLS_ENTROPY_C && MBEDTLS_ENTROPY_HARDWARE_ALT */