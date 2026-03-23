/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Breath Ease
 * Author: wangle
 * Create: 2025-4-23
 */
#include "uiservice/ui_language.h"
#include "ui_resource_string.h"
#include "font/font_global_manager.h"
#include "nv.h"
#include "wearable_log.h"

#include "nv_config/include/key_id.h"
#include "nv_config/include/common.h"
#include "nv.h"

namespace OHOS {
static constexpr const char *BIN_LANG_EN = "/music/font/en.lang";
static constexpr const char *BIN_LANG_ZH = "/music/font/zh.lang";

const char* UILanguage::GetText(uint16_t textId)
{
    if (RegisterGlobalManager()) {
        return FontGlobalManager::GetInstance()->GetText(textId);
    }
    return nullptr;
}
void UILanguage::SetLanguage(uint8_t id)
{
    sys_language_id_t lang_id = {.language_id = id};

    errcode_t ret = uapi_nv_write(NV_ID_SYS_LANGUAGE_ID, (uint8_t*)&lang_id, sizeof(sys_language_id_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nv set language id error, ret=%u", ret);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "set language id = %u", lang_id.language_id);
    UpdateLanguage();
}
void UILanguage::UpdateLanguage()
{
    sys_language_id_t lang_id = {0};
    uint16_t  nvsize = 0;

    errcode_t ret = uapi_nv_read(NV_ID_SYS_LANGUAGE_ID, sizeof(sys_language_id_t), &nvsize, (uint8_t*)&lang_id);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nv get language id error, ret=%u", ret);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "language nvsize=%u id = %u", nvsize, lang_id.language_id);
    if (RegisterGlobalManager()) {
        if (lang_id.language_id<0 || lang_id.language_id > OHOS::LANGUAGE_ID_MAX) {
            lang_id.language_id = OHOS::LANGUAGE_ID_ZH;
        }
        FontGlobalManager::GetInstance()->SetCurrentLangId(lang_id.language_id);
    }
}
bool UILanguage::RegisterGlobalManager()
{
    static bool isRegister = false;
    if (!isRegister) {
        const char *resFiles[] = {BIN_LANG_ZH, BIN_LANG_EN};
        if (!FontGlobalManager::GetInstance()->RegisterLanguageInfo(
            resFiles, OHOS::LANGUAGE_ID_MAX, OHOS::LANGUAGE_ID_ZH)) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "register global manager error");
            return isRegister;
        }
        isRegister = true;
    }
    return isRegister;
}
}