/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainModel.cpp
 * Author:
 * Create: 2021-11-23
 */

#include "unistd.h"
#include "main/MainModel.h"
#include "kv_store.h"
#include "securec.h"
#include "wearable_log.h"
#include "main/DialBinTypesV2.h"
#include "power_display_service.h"
#include "UIWatchDialFactory.h"

namespace OHOS {
    static const int CARD_SETTING_VALUE_LEN = 32;
    static const int DIAL_SETTING_VALUE_LEN = 64;
    const CardInfo cardInfoMap[] = {
        { COMPASS, PNG_SET_CARD_COMPASS },
        { ACTIVITY, PNG_SET_CARD_ACTIVITY },
        { MUSIC_PLAYER, PNG_SET_CARD_PLAYER },
        { VIDEO_CARD1, PNG_SET_CARD_VIDEO },
        { VIDEO_CARD2, PNG_SET_CARD_VIDEO },
        { VIDEO_CARD3, PNG_SET_CARD_VIDEO },
    };

    MainModel::MainModel()
    {
        power_display_gui_enable_aod(IsAodEnabled());
    }

    MainModel::~MainModel() {}

    MainModel &MainModel::GetInstance()
    {
        static MainModel instance;
        return instance;
    }

    const CardInfo *MainModel::GetCardInfo(CardId card) const
    {
        for (uint8_t i = 0; i < (sizeof(cardInfoMap) / sizeof(cardInfoMap[0])); i++) {
            if (card == cardInfoMap[i].id) {
                return &cardInfoMap[i];
            }
        }
        return nullptr;
    }

    const DialInfo *MainModel::GetDialInfo(uint8_t dialId) const
    {
        return UIWatchDialFactory::GetInstance().GetNormalDialInfo(dialId);
    }

    uint8_t MainModel::GetDialInfoNum() const
    {
        return UIWatchDialFactory::GetInstance().GetNormalDialInfoNum();
    }

    void MainModel::InitCardSettings(void)
    {
        char temp[CARD_SETTING_VALUE_LEN] = { 0 };
        int len;
        int index = 0;

        memset_s(cardSettings, CARD_SETTING_COUNT_MAX, 0, CARD_SETTING_COUNT_MAX);
        len = UtilsGetValue("card_setting", temp, CARD_SETTING_VALUE_LEN);
        if (len >= 0 && strlen(temp) > 0) {
            /* 读取用户card配置 */
            char *setBuffer = nullptr;
            char *setValue = strtok_s(temp, ",", &setBuffer);
            while (setValue != nullptr) {
                if (index >= CARD_SETTING_COUNT_MAX) {
                    break;
                }
                int val = atoi(setValue);
                if (val >= 0 && val < CARD_SETTING_COUNT_MAX) {
                    cardSettings[index++] = static_cast<CardId>(val);
                }
                setValue = strtok_s(nullptr, ",", &setBuffer);
            }
            cardSettingCount = index;
        } else {
            /* 使用默认card配置 */
            cardSettings[index++] = WATCH_FACE;
            cardSettings[index++] = COMPASS;
            cardSettings[index++] = ACTIVITY;
            cardSettings[index++] = MUSIC_PLAYER;
            cardSettings[index++] = VIDEO_CARD1;
            cardSettings[index++] = VIDEO_CARD2;
            cardSettings[index++] = VIDEO_CARD3;
            cardSettingCount = index;
        }
    }

    void MainModel::DeleteCardSetting(uint8_t index)
    {
        char temp[CARD_SETTING_VALUE_LEN] = { 0 };
        for (uint8_t i = 0; i < cardSettingCount; i++) {
            if (i == index + 1) {
                uint8_t j = i;
                for (; j < cardSettingCount - 1; j++) {
                    cardSettings[j] = cardSettings[j + 1];
                }
                cardSettings[j] = MAX_CARD;
                cardSettingCount--;
            }
        }

        for (uint8_t i = 0; i < cardSettingCount; i++) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "cardSettings[%d] = %d", i, cardSettings[i]);
            (void)sprintf_s(temp + strlen(temp), CARD_SETTING_VALUE_LEN, "%d,", cardSettings[i]);
        }
        (void)UtilsSetValue("card_setting", temp);
    }

    void MainModel::AddCardSetting(uint8_t index)
    {
        char temp[CARD_SETTING_VALUE_LEN] = { 0 };
        if (cardSettingCount >= CARD_SETTING_COUNT_MAX) {
            return;
        }
        for (int i = 0; i < cardSettingCount; i++) {
            if (cardSettings[i] == cardInfoMap[index].id) {
                return;
            }
        }
        cardSettings[cardSettingCount++] = cardInfoMap[index].id;
        for (uint8_t i = 0; i < cardSettingCount; i++) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "cardSettings[%d] = %d", i, cardSettings[i]);
            (void)sprintf_s(temp + strlen(temp), CARD_SETTING_VALUE_LEN, "%d,", cardSettings[i]);
        }
        (void)UtilsSetValue("card_setting", temp);
    }

    uint8_t MainModel::GetCardSettingCount(void)
    {
        return cardSettingCount;
    }

    CardId *MainModel::GetCardSettings(void)
    {
        return cardSettings;
    }

    void MainModel::InitDialSettingDefault(void)
    {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "InitDialSettingDefault");
        dialSetting.dialFlag = 0;
        dialSetting.dialId = 0;
        dialSetting.dialFullName = "";
    }

    void MainModel::AddDialSetting(const DialSetting &setting)
    {
        std::string tmp;
        tmp += std::to_string(setting.dialFlag);
        tmp += ",";
        tmp += std::to_string(setting.dialId);
        tmp += ",";
        tmp += setting.dialFullName;
        tmp += ",";
        (void)UtilsSetValue("dial_setting", tmp.c_str());
    }

    void MainModel::InitDialSettings(void)
    {
        char temp[DIAL_SETTING_VALUE_LEN] = { 0 };
        int32_t len = UtilsGetValue("dial_setting", temp, DIAL_SETTING_VALUE_LEN);
        if (len < 0) {
            InitDialSettingDefault();
            return;
        }
        char* setBuffer = nullptr;
        char* setValue = strtok_s(temp, ",", &setBuffer);
        if (!setValue) {
            InitDialSettingDefault();
            return;
        }
        dialSetting.dialFlag = atoi(setValue);

        setValue = strtok_s(nullptr, ",", &setBuffer);
        if (!setValue) {
            InitDialSettingDefault();
            return;
        }
        dialSetting.dialId = atoi(setValue);

        if (dialSetting.dialFlag  == 0) {
            dialSetting.dialFullName = "";
            return;
        }

        setValue = strtok_s(nullptr, ",", &setBuffer);
        if (!setValue) {
            InitDialSettingDefault();
            return;
        }
        dialSetting.dialFullName = setValue;

        if (access(dialSetting.dialFullName.c_str(), F_OK) != 0) {
            InitDialSettingDefault();
        }
    }

    DialSetting& MainModel::GetDialSetting()
    {
        return dialSetting;
    }

    bool MainModel::IsFromSetCardView(void) const
    {
        return isFromSetCardView;
    }

    void MainModel::SetFromSetCardView(bool isFrom)
    {
        isFromSetCardView = isFrom;
    }

    void MainModel::SetFromCardId(uint8_t cardId)
    {
        fromCardId = cardId;
    }

    uint8_t MainModel::GetFromCardId(void)
    {
        return fromCardId;
    }

    bool MainModel::IsAodEnabled()
    {
        char temp[DIAL_SETTING_VALUE_LEN] = { 0 };
        int32_t len = UtilsGetValue("enable_aod", temp, DIAL_SETTING_VALUE_LEN);
        if (len >= 0) {
            int enable = atoi(temp);
            isAodEnabled = (enable != 0);
        }
        return isAodEnabled;
    }

    void  MainModel::EnableAod(bool enable)
    {
        power_display_gui_enable_aod(enable);
        isAodEnabled = enable;
        if (isAodEnabled) {
            UtilsSetValue("enable_aod", "1");
        } else {
            UtilsSetValue("enable_aod", "0");
        }
    }
}
