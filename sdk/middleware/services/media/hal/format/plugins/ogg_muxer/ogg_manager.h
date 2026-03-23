/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: ogg manager file
 * Author: Media Software Group
 * Create: 2025-09-28
 */
#ifndef OGG_MANAGER_H
#define OGG_MANAGER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

int32_t OggCtxInit(int32_t fd, int32_t sampleRate, int32_t channels);
void OggCtxDeInit(void);
int32_t OggWritePacket(int32_t fd, uint8_t *buffer, int64_t bufferLen, int64_t samplePoint);
int32_t OggWriteEosPacket(int32_t fd);

#ifdef __cplusplus
}
#endif
#endif