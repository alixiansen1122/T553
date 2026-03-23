/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodModel
 * Author:
 * Create: 2024-10-30
 */

#include "aod_view/AodModel.h"
#include <unistd.h>
#include "securec.h"
#include "kv_store.h"

namespace OHOS {
static const int DIAL_SETTING_VALUE_LEN = 64;
AodModel::AodModel()
{
    aodDialSetting_.dialFlag = 0;
    aodDialSetting_.dialId = 0;
    aodDialSetting_.dialFullName = "";
}

AodModel::~AodModel()
{
}

void AodModel::GetAodDialSetting(DialSetting &dial)
{
    char temp[DIAL_SETTING_VALUE_LEN] = { 0 };
    int32_t len = UtilsGetValue("aod_dial_setting", temp, DIAL_SETTING_VALUE_LEN);
    if (len < 0) {
        dial = aodDialSetting_;
        return;
    }
    char *setBuffer = nullptr;
    char *setValue = strtok_s(temp, ",", &setBuffer);
    if (setValue != nullptr) {
        dial.dialFlag = atoi(setValue);
    } else {
        dial = aodDialSetting_;
        return;
    }

    setValue = strtok_s(nullptr, ",", &setBuffer);
    if (setValue != nullptr) {
        dial.dialId = atoi(setValue);
    } else {
        dial = aodDialSetting_;
        return;
    }

    setValue = strtok_s(nullptr, ",", &setBuffer);
    if (setValue != nullptr) {
        dial.dialFullName = setValue;
        if (!((dial.dialFullName.length() > 0) &&
            (access(dial.dialFullName.c_str(), F_OK) == 0))) {
            dial = aodDialSetting_;
            return;
        }
    } else {
        dial = aodDialSetting_;
        return;
    }
    aodDialSetting_ = dial;
}

void AodModel::SaveAodDialSetting(const DialSetting &dial)
{
    aodDialSetting_ = dial;
    std::string tmp;
    tmp += std::to_string(dial.dialFlag);
    tmp += ",";
    tmp += std::to_string(dial.dialId);
    tmp += ",";
    tmp += dial.dialFullName;
    tmp += ",";
    UtilsSetValue("aod_dial_setting", tmp.c_str());
}
} // OHOS
