/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuSync
 * Create: 2025-07-12
 */

#ifndef PHONE_MENU_SYNC_H
#define PHONE_MENU_SYNC_H
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <iostream>
#include "UiConfig.h"

namespace OHOS {

enum CONTACT_DATA_ERROE {
    OK = 0,
    JSON_CREATE_FAIL = 1,
    JSON_PARSE_FAIL = 2,
    JSON_TYPE_FAIL = 3,
    JSON_KEY_FAIL = 4,
    JSON_TO_STRING_FAIL = 5,
    JSON_FILE_OPEN_FAIL = 6,
};

typedef struct {
    std::string cid_;
    std::string name_;
    std::string phone_;
} Contacts;

const std::string CONTACTS_FILE_PATH = APP_RES_PATH "/contactsInfo.json";
class PhoneMenuSync {
public:
    static uint32_t WriteFileContact(const std::string &filePath, std::string &content);
    static uint32_t ReadFileContact(const std::string &filePath);
    static uint32_t ParseContactsMsg(std::string &msgContent);
    static std::list<Contacts> &GetContacts();
    static uint32_t PrintContacts();
    static uint32_t LookContacts(std::string &contacts);
    static uint32_t CleanFileContacts(const std::string &filePath);

private:
    static std::list<Contacts> list_;
};
}
#endif  // PHONE_MENU_SYNC_H
