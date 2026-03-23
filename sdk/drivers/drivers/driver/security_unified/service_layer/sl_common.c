/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides common utils for security_unified service layer. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-14, Create file. \n
 */
#include "sl_common.h"
#include "crypto_errno.h"
#include "crypto_common_macro.h"

typedef struct crypto_errcode_item {
    int32_t from_errcode;
    errcode_t to_errcode;
} crypto_errcode_item_t;

#define ERROR_SECURITY_UNIFIED      0x80001500
errcode_t crypto_sl_common_get_errcode(int32_t from_errcode)
{
    errcode_t ret = from_errcode & 0xFF;
    if ((uint32_t)from_errcode == CRYPTO_SUCCESS) {
        return ERRCODE_SUCC;
    } else if ((uint32_t)from_errcode == CRYPTO_FAILURE) {
        return ERRCODE_FAIL;
    }
    return ERROR_SECURITY_UNIFIED + ret;
}
