/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc access sample, should enable build macro HAVE_NFC_LISTEN & HAVE_NFC_POLL\n
 * Author: @CompanyNameTag \n
 * Date: 2025-06-25 \n
 */
#ifndef SAMPLE_ACCESS_H
#define SAMPLE_ACCESS_H

#include "nfc_error_code.h"
#include "nfc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NFCID1_MAX_LEN 10

typedef struct {
    uint8_t nfcid[NFCID1_MAX_LEN];
    uint8_t nfcidLen;
    uint8_t sensRes[SENS_RES_LEN];
    uint8_t sak;
} NonContactParam;

NfcErrorCode NFC_SAMPLE_AccessRwTaskMain(void);
NfcErrorCode NFC_SAMPLE_AccessHceTaskMain(void);

#ifdef __cplusplus
}
#endif

#endif // SAMPLE_ACCESS_H