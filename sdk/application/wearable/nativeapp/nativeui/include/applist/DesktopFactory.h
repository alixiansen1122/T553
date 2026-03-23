/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DesktopFactory
 * Author:
 * Create: 2025-07
 */

#ifndef DESKTOP_FACTORY_H
#define DESKTOP_FACTORY_H

#include <unordered_map>
#include "UIDesktopFragment.h"

namespace OHOS {
class DesktopInterface;
class DesktopFactory {
public:
    static DesktopFactory& GetInstance()
    {
        static DesktopFactory instance;
        return instance;
    }
    void RegisterDesktopStyle(const uint16_t style, DesktopInterface* desktop);
    UIDesktopFragment* CreateDesktopStyle(uint16_t style);

private:
    DesktopFactory() {};
    ~DesktopFactory() {};
    DesktopFactory(const DesktopFactory&) = delete;
    DesktopFactory &operator=(const DesktopFactory&) = delete;
    std::unordered_map<uint16_t, DesktopInterface*> desktops_;
};

class DesktopStyleRegister {
public:
    DesktopStyleRegister(const uint16_t style, DesktopInterface* uiCardPage)
    {
        DesktopFactory::GetInstance().RegisterDesktopStyle(style, uiCardPage);
    }
};

class DesktopInterface {
public:
    virtual UIDesktopFragment* CreateDesktop() = 0;
    DesktopInterface() = default;
    virtual ~DesktopInterface() = default;
    DesktopInterface(const DesktopInterface&) = delete;
    DesktopInterface& operator=(const DesktopInterface&) = delete;
    DesktopInterface(DesktopInterface&&) = delete;
    DesktopInterface& operator=(DesktopInterface&&) = delete;
};

template <class D>
class DesktopProxy : public DesktopInterface {
public:
    UIDesktopFragment* CreateDesktop() override
    {
        D* d = new D();
        UIDesktopFragment* fragment = dynamic_cast<UIDesktopFragment*>(d);
        if (fragment == nullptr) {
            delete d;
        }
        return fragment;
    }
};
} // namespace OHOS

#define REGIST_DESKTOP_STYLE(style, D) \
    const static OHOS::DesktopStyleRegister (STATIC_VAR)(static_cast<uint16_t>(style), new DesktopProxy<D>())

#endif // DESKTOP_FACTORY_H