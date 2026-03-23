/*
 * Copyright (c) CompanyNameMagicTag
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISPLAY_GRALLOC_INTERNAL_H
#define DISPLAY_GRALLOC_INTERNAL_H
#include "display_type.h"

/* max allowed malloc memory:256M */
#define MAX_MALLOC_SIZE  0x10000000L

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define HEIGHT_ALIGN 2U
#define WIDTH_ALIGN 8U
#define MAX_PLANES 3

#define DEFAULT_READ_WRITE_PERMISSIONS 0666
#define SHM_MAX_KEY 10000
#define SHM_START_KEY 1
#define BITS_PER_BYTE 8

typedef struct {
    BufferHandle hdl;
    int32_t shmid;
} PriBufferHandle;
#endif /* DISPLAY_GRALLOC_INTERNAL_H */
