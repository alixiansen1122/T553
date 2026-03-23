/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc raw data exchange sample, should enable build macro HAVE_NFC_LISTEN & HAVE_NFC_POLL\n
 * Author: @CompanyNameTag \n
 * Date: 2025-07-04 \n
 */
#ifndef SAMPLE_RAWDATA_EXCHANGE_H
#define SAMPLE_RAWDATA_EXCHANGE_H

#include "nfc_error_code.h"
#include "nfc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

NfcErrorCode NFC_SAMPLE_RawDataExchangeRwTaskMain(void);
NfcErrorCode NFC_SAMPLE_RawDataExchangeHceTaskMain(void);

#ifdef __cplusplus
}
#endif

#endif // SAMPLE_RAWDATA_EXCHANGE_H