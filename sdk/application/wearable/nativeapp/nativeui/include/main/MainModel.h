/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: MainModel.h
 * Author:
 * Create: 2021-11-23
 */

#ifndef MAIN_MODEL_H
#define MAIN_MODEL_H

#include <string>
#include "UiConfig.h"

namespace OHOS {
    typedef struct {
        CardId id;
        const char *cardImg;
    } CardInfo;

    typedef struct {
        uint8_t id;
        const char *dialName;
        const char *dialImg;
        uint8_t freq;
        uint8_t capability;
    } DialInfo;

    typedef struct {
        uint8_t dialFlag; // 0 在线表盘 1 离线表盘
        uint8_t dialId;
        std::string dialFullName;
    } DialSetting;

    class MainModel {
    public:
        static MainModel &GetInstance(void);
        void InitCardSettings(void);
        uint8_t GetCardSettingCount(void);
        CardId *GetCardSettings(void);
        void DeleteCardSetting(uint8_t index);
        void AddCardSetting(uint8_t index);
        const CardInfo *GetCardInfo(CardId card) const;
        const DialInfo *GetDialInfo(uint8_t dial) const; // 获取在线表盘配置
        uint8_t GetDialInfoNum() const; // 获取在线表盘的数量
        void AddDialSetting(const DialSetting &setting);
        void InitDialSettings(void);
        DialSetting &GetDialSetting();
        bool IsFromSetCardView(void) const;
        void SetFromSetCardView(bool isFrom);
        void SetFromCardId(uint8_t cardId);
        uint8_t GetFromCardId(void);
        bool IsAodEnabled();
        void EnableAod(bool enable);

    private:
        void InitDialSettingDefault(void);
        MainModel();
        MainModel(const MainModel &);
        MainModel &operator=(const MainModel &);
        virtual ~MainModel();

        CardId cardSettings[CARD_SETTING_COUNT_MAX] = { MAX_CARD };
        uint8_t cardSettingCount{0};
        DialSetting dialSetting;
        bool isFromSetCardView{false};
        uint8_t fromCardId{MAX_CARD};
        bool isAodEnabled{false};
    };
}

#endif