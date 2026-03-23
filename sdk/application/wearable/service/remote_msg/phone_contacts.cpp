/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneContacts
 * Create: 2025-07-12
 */

#include "wearable_log.h"
#include "message.h"
#include "PhoneMenuSync.h"
#include "phone_contacts.h"

#ifdef __cplusplus
extern "C" {
#endif
uint8_t updata_contacts(Request *request)
{
    std::string content((char *)request->data, request->len);
    uint8_t ret = OHOS::PhoneMenuSync::ParseContactsMsg(content);
    if (ret != OHOS::OK) {
        return ret;
    }
    OHOS::PhoneMenuSync::WriteFileContact(OHOS::CONTACTS_FILE_PATH, content);
    return 0;
}

uint8_t lookup_contacts(std::string &lookupContacts)
{
    return OHOS::PhoneMenuSync::LookContacts(lookupContacts);
}
#ifdef __cplusplus
}
#endif
