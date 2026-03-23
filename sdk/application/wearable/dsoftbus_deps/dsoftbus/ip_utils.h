/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ip utils adapt header file.
 * Author: CompanyName
 * Create:
 */
#ifndef IP_UTILS_H
#define IP_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int32_t GetNetworkIpByIfName(const char *ifName, char *ip, char *netmask, uint32_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* IP_UTILS_H */

