/*
* Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description: media memory manager header
* Author: Media Software Group
* Create: 2025-03-03
*/

#ifndef MEDIA_MEM_H
#define MEDIA_MEM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void *MediaMemMalloc(const size_t bytesNum);
void *MediaMemCalloc(const size_t bytesNum);
void MediaMemFree(const void* src);

#ifdef __cplusplus
};
#endif
#endif // MEDIA_MEM_H
