/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef __NET_WS_STRING_H__
#define __NET_WS_STRING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *WsStringCopy(const char *str);

char *WsStringCopyLen(const char *str, const size_t len);

void WsStringDelete(char *str);

void WsStringDeleteClean(char **str);

#endif
