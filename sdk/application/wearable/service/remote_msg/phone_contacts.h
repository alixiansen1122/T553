/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneContacts
 * Create: 2025-07-12
 */

#ifndef PHONE_CONTACTS_H
#define PHONE_CONTACTS_H
#include <string>
#include "message.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
uint8_t updata_contacts(Request *request); // 同步手机侧联系人到表侧
uint8_t lookup_contacts(std::string &lookupContacts); // 查询表侧联系人到手机
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* PHONE_CONTACTS_H */