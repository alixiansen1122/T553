/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved. \n
 * Description: NFC NDEF main process \n
 * Author: @CompanyNameTag \n
 * Date: 2023-03-22 \n
 */
#include "nfc_ndef.h"
#include "nfc_hal_adapter.h"
#include "securec.h"
#include "nfc_hal_core.h"
#include "nfc_ndef_t2t.h"
#include "nfc_ndef_t3t.h"
#include "nfc_ndef_t4t.h"
#include "nfc_ndef_t5t.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NfcErrorCode (*readFunc)(uint8_t *ndefBuff, uint16_t *ndefLen);
    NfcErrorCode (*writeFunc)(const uint8_t *ndefBuff, uint16_t ndefLen);
} NfcNdefFunc;

static NfcNdefFunc g_ndefProcFunc = {
    .readFunc = NFC_NDEF_T2TReadProc,
    .writeFunc = NFC_NDEF_T2TWriteProc,
};

NfcErrorCode NFC_NDEF_ResetProc(const NfcIntfActInfo *nfcIntfActInfo)
{
    if (nfcIntfActInfo == NULL) {
        return NFC_ERR_CODE_NDEF_PROC_INPUT_PARAM_INVALID;
    }
    NfcErrorCode ret = NFC_OK;

    switch (nfcIntfActInfo->protocol) {
        case NFC_NCI_RF_PROTOCOL_T1T:
            return NFC_ERR_CODE_NDEF_PROC_RESET_WRONG_PROTOCOL;
        case NFC_NCI_RF_PROTOCOL_T2T:
            NFC_NDEF_T2TResetProc();
            g_ndefProcFunc.readFunc = NFC_NDEF_T2TReadProc;
            g_ndefProcFunc.writeFunc = NFC_NDEF_T2TWriteProc;
            break;
        case NFC_NCI_RF_PROTOCOL_T3T:
            ret = NFC_NDEF_T3TResetProc(&nfcIntfActInfo->techParams.f);
            g_ndefProcFunc.readFunc = NFC_NDEF_T3TReadProc;
            g_ndefProcFunc.writeFunc = NFC_NDEF_T3TWriteProc;
            break;
        case NFC_NCI_RF_PROTOCOL_ISO_DEP:
            NFC_NDEF_T4TResetProc();
            g_ndefProcFunc.readFunc = NFC_NDEF_T4TReadProc;
            g_ndefProcFunc.writeFunc = NFC_NDEF_T4TWriteProc;
            break;
        case NFC_NCI_RF_PROTOCOL_T5T:
            ret = NFC_NDEF_T5TResetProc(&nfcIntfActInfo->techParams.v);
            g_ndefProcFunc.readFunc = NFC_NDEF_T5TReadProc;
            g_ndefProcFunc.writeFunc = NFC_NDEF_T5TWriteProc;
            break;
        default:
            return NFC_ERR_CODE_NDEF_PROC_RESET_WRONG_PROTOCOL;
    }

    return ret;
}

NfcErrorCode NFC_NDEF_ReadProc(uint8_t *ndefBuff, uint16_t *ndefLen)
{
    if (ndefBuff == NULL || ndefLen == NULL || *ndefLen == 0) {
        PS_PRINT_ERR("[NDEF] ReadProc input param invalid.\n");
        return NFC_ERR_CODE_NDEF_PROC_INPUT_PARAM_INVALID;
    }
    NfcErrorCode ret = g_ndefProcFunc.readFunc(ndefBuff, ndefLen);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] ReadProc read NDEF failed. ret=%x\n", ret);
    }
    return ret;
}

NfcErrorCode NFC_NDEF_WriteProc(const uint8_t *ndefBuff, uint16_t ndefLen)
{
    if (ndefBuff == NULL || ndefLen == 0) {
        PS_PRINT_ERR("[NDEF] WriteProc input param invalid.\n");
        return NFC_ERR_CODE_NDEF_PROC_INPUT_PARAM_INVALID;
    }
    NfcErrorCode ret = g_ndefProcFunc.writeFunc(ndefBuff, ndefLen);
    if (ret != NFC_OK) {
        PS_PRINT_ERR("[NDEF] WriteProc write NDEF failed. ret=%x\n", ret);
    }
    return ret;
}

#ifdef __cplusplus
}
#endif