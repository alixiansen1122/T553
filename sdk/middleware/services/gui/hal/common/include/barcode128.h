/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: barcode128
 * Author:
 * Create: 2023-12
 */

#ifndef BARCODE128_H
#define BARCODE128_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t Code128EstimateLen(const char* s);
int32_t Code128Encode(const char *s, char *out, int32_t maxLength);

#ifdef __cplusplus
}
#endif

#endif // BARCODE128_H
