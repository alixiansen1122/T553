/*
 * Copyright (c) @CompanyNameMagicTag. 2024-2024. All rights reserved.
 * Description: audio Decryption Definition
 * Author: Media Software Group
 * Create: 2024-11-01
 */

#ifndef AUDIO_DECRYPT_H
#define AUDIO_DECRYPT_H
#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef bool (*IsEncrypted)(char *path);
typedef int32_t (*DecryptFunction)(uint8_t *inBuf, int32_t inSize, uint8_t *outBuf, int32_t outSize);
typedef uint32_t (*GetDecryptUnitLength)(void);

bool AUDIO_IsEncrypted(char *path);
int32_t AUDIO_DecryptFunction(uint8_t *inBuf, int32_t inSize, uint8_t *outBuf, int32_t outSize);
uint32_t AUDIO_GetDecryptUnitLength(void);

void AUDIO_LoadDecryptLibrary(char *decryptFileName);
void AUDIO_UnLoadDecryptLibrary(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif