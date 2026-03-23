/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc alipay sample, should enable build macro HAVE_NFC_POLL \n
 * Author: @CompanyNameTag \n
 * Date: 2025-06-25 \n
 */
#ifndef SAMPLE_ALIPAY_H
#define SAMPLE_ALIPAY_H

#include "nfc_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

NfcErrorCode NFC_SAMPLE_AlipayStartRw(void);

#ifdef __cplusplus
}
#endif

#endif // SAMPLE_ALIPAY_H