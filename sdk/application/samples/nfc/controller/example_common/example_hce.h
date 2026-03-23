/*
 * Copyright (c) @CompanyNameMagicTag 2025. All rights reserved. \n
 * Description: nfc hce example, ExampleHostCardEmulation should enable build macro HAVE_NFC_LISTEN \n
 * Author: @CompanyNameTag \n
 * Date: 2025-07-02 \n
 */
#ifndef EXAMPLE_HCE_H
#define EXAMPLE_HCE_H

#include "nfc_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

NfcErrorCode NFC_EXAMPLE_HceInit(void);
NfcErrorCode NFC_EXAMPLE_StartHostCardEmulation(void);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_HCE_H
