/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UICardPageFactory
 * Author:
 * Create: 2025-06
 */

#ifndef UI_CARD_PAGE_FACTORY_H
#define UI_CARD_PAGE_FACTORY_H

#include <unordered_map>
#include "components/ui_card_page.h"
#include "UiConfig.h"

namespace OHOS {
class UICardPageProxyInterface;
class UICardPageFactory {
public:
    static UICardPageFactory& GetInstance()
    {
        static UICardPageFactory instance;
        return instance;
    }
    void RegisterCardPage(const uint16_t id, UICardPageProxyInterface* uiCardPage);
    UICardPage* CreateCardPage(uint16_t id);

private:
    UICardPageFactory() {};
    ~UICardPageFactory() {};
    UICardPageFactory(const UICardPageFactory&) = delete;
    UICardPageFactory &operator=(const UICardPageFactory&) = delete;
    std::unordered_map<uint16_t, UICardPageProxyInterface*> cardPage_;
};

class CardPageRegister {
public:
    CardPageRegister(const uint16_t id, UICardPageProxyInterface* uiCardPage)
    {
        UICardPageFactory::GetInstance().RegisterCardPage(id, uiCardPage);
    }
};

class UICardPageProxyInterface {
public:
    virtual UICardPage* CreateUICardPage() = 0;
    UICardPageProxyInterface() = default;
    virtual ~UICardPageProxyInterface() = default;
    UICardPageProxyInterface(const UICardPageProxyInterface&) = delete;
    UICardPageProxyInterface& operator=(const UICardPageProxyInterface&) = delete;
    UICardPageProxyInterface(UICardPageProxyInterface&&) = delete;
    UICardPageProxyInterface& operator=(UICardPageProxyInterface&&) = delete;
};

template <class U>
class CardPageProxy : public UICardPageProxyInterface {
public:
    UICardPage* CreateUICardPage() override
    {
        U* u = new U();
        UICardPage* page = dynamic_cast<UICardPage*>(u);
        if (page == nullptr) {
            delete u;
        }
        return page;
    }
};
} // namespace OHOS

#define REGIST_CARD_PAGE(id, U) \
    const static OHOS::CardPageRegister (STATIC_VAR)(static_cast<uint16_t>(id), \
                                                      new CardPageProxy<U>())

#endif // UI_CARD_PAGE_FACTORY_H