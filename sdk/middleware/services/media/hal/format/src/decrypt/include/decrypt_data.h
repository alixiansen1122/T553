/*
 * Copyright (c) @CompanyNameMagicTag. 2024-2024. All rights reserved.
 * Description: Decryption Data Definition
 * Author: Media Software Group
 * Create: 2024-11-01
 */

#ifndef DECRYPT_DATA_H
#define DECRYPT_DATA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef struct {
    uint8_t *readBuf;
    uint8_t *decryptBuf;
    int32_t pos;
    bool isNeedDecryptData;
    int32_t decryptSize;
} DecryptBufContext;

bool IsDecrypt(void *context);

int32_t InitDecrypt(void *context, const char *url);

int32_t DecryptReadData(int32_t fd, void *context, uint8_t *buf, int32_t size);

int64_t DecryptSeekData(int32_t fd, void *context, uint64_t seekPos, const int32_t whence);

int32_t DeInitDecrypt(void *context);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif