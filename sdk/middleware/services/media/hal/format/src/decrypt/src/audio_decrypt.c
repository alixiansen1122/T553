/*
 * Copyright (c) @CompanyNameMagicTag. 2024-2024. All rights reserved.
 * Description: audio Decryption implementation
 * Author: Media Software Group
 * Create: 2024-11-01
 */

#include "audio_decrypt.h"
#include <stdlib.h>
#ifdef SUPPORT_DYNLOAD
#include "svr_dynload.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
IsEncrypted g_isEncryptedCbk = NULL;
DecryptFunction g_decryptFunctionCbk = NULL;
GetDecryptUnitLength g_getDecryptUnitLengthCbk = NULL;

bool AUDIO_IsEncrypted(char *path)
{
    if (g_isEncryptedCbk != NULL) {
        return g_isEncryptedCbk(path);
    } else {
        return false;
    }
}

int32_t AUDIO_DecryptFunction(uint8_t *inBuf, int32_t inSize, uint8_t *outBuf, int32_t outSize)
{
    if (g_decryptFunctionCbk != NULL) {
        return g_decryptFunctionCbk(inBuf, inSize, outBuf, outSize);
    } else {
        return -1;
    }
}

uint32_t AUDIO_GetDecryptUnitLength(void)
{
    if (g_getDecryptUnitLengthCbk != NULL) {
        return g_getDecryptUnitLengthCbk();
    } else {
        return 0;
    }
}

void AUDIO_LoadDecryptLibrary(char *decryptFileName)
{
#ifdef SUPPORT_DYNLOAD
    dynload_load_library(decryptFileName);
    g_isEncryptedCbk = (IsEncrypted)dynload_get_symbol((char*)"AUDIO_IsEncrypted");
    g_decryptFunctionCbk = (DecryptFunction)dynload_get_symbol((char*)"AUDIO_DecryptFunction");
    g_getDecryptUnitLengthCbk = (GetDecryptUnitLength)dynload_get_symbol((char*)"AUDIO_GetDecryptUnitLength");
#endif
}

void AUDIO_UnLoadDecryptLibrary(void)
{
#ifdef SUPPORT_DYNLOAD
    g_isEncryptedCbk = NULL;
    g_decryptFunctionCbk = NULL;
    g_getDecryptUnitLengthCbk = NULL;
    dynload_unload_library();
#endif
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
