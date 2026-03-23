/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UIWatchDialFactory
 * Author:
 * Create: 2025-06
 */

#ifndef UI_WATCH_DIAL_FACTORY_H
#define UI_WATCH_DIAL_FACTORY_H

#include <unordered_map>
#include "components/ui_card_page.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "main/MainModel.h"
#include "components/ui_fragment.h"

namespace OHOS {
class UIWatchDialProxyInterface;
class UIWatchDialFactory {
public:
    static UIWatchDialFactory& GetInstance()
    {
        static UIWatchDialFactory instance;
        return instance;
    }
    void RegisterWatchDial(const uint16_t ability, const uint16_t id, UIWatchDialProxyInterface* uiWatchDial,
        const DialInfo* dialInfo);
    UICardPage* CreateNormalDial(uint16_t id);
    UIFragment* CreateAodDial(uint16_t id);
    const DialInfo* GetNormalDialInfo(uint16_t id);
    uint8_t GetNormalDialInfoNum();

private:
    UIWatchDialFactory() {};
    ~UIWatchDialFactory() {};
    UIWatchDialFactory(const UIWatchDialFactory&) = delete;
    UIWatchDialFactory &operator=(const UIWatchDialFactory&) = delete;
    std::unordered_map<uint16_t, UIWatchDialProxyInterface*> watchDials_[static_cast<uint16_t>(DIAL_DISPLAY_MODE_MAX)];
    std::unordered_map<uint16_t, const DialInfo*> normalDialInfo_;
};

class WatchDialRegister {
public:
    WatchDialRegister(const uint16_t ability, const uint16_t id, UIWatchDialProxyInterface* uiWatchDial,
        const DialInfo* dialInfo)
    {
        UIWatchDialFactory::GetInstance().RegisterWatchDial(ability, id, uiWatchDial, dialInfo);
    }
};

class UIWatchDialProxyInterface {
public:
    virtual UICardPage* CreateWatchDialCardPage(const uint16_t id) = 0;
    virtual UIFragment* CreateWatchDialFragment(const uint16_t id) = 0;
    UIWatchDialProxyInterface() = default;
    virtual ~UIWatchDialProxyInterface() = default;
    UIWatchDialProxyInterface(const UIWatchDialProxyInterface&) = delete;
    UIWatchDialProxyInterface& operator=(const UIWatchDialProxyInterface&) = delete;
    UIWatchDialProxyInterface(UIWatchDialProxyInterface&&) = delete;
    UIWatchDialProxyInterface& operator=(UIWatchDialProxyInterface&&) = delete;
};

template <class U>
class WatchDialProxy : public UIWatchDialProxyInterface {
public:
    UICardPage* CreateWatchDialCardPage(const uint16_t id) override
    {
        U* u = new U();
        UICardPage* page = dynamic_cast<UICardPage*>(u);
        if (page == nullptr) {
            delete u;
        }
        return page;
    }

    UIFragment* CreateWatchDialFragment(const uint16_t id) override
    {
        U* u = new U();
        UIFragment* fragment = dynamic_cast<UIFragment*>(u);
        if (fragment == nullptr) {
            delete u;
        }
        fragment->CreateView();
        return fragment;
    }
};
} // namespace OHOS

#define REGIST_WATCH_DIAL(ability, id, U, info) \
    const static OHOS::WatchDialRegister (STATIC_VAR)(static_cast<uint16_t>(ability), static_cast<uint16_t>(id), \
                                                      new WatchDialProxy<U>(), static_cast<const DialInfo*>(info))

#endif // UI_WATCH_DIAL_FACTORY_H