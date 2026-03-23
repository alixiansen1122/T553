/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: traceui buffer manager interface
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef TRACEUI_BUF_H
#define TRACEUI_BUF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void TraceuiBufInit(uint8_t *buf, uint32_t size);
void TraceuiBufDeinit(void);

uint8_t *TraceuiGetBuf(uint32_t len);
uint32_t TraceuiGetBufUsedSize(void);

#ifdef __cplusplus
}
#endif
#endif
