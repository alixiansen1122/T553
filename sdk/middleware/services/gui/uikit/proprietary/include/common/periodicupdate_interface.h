/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: PeriodicUpdateInterface
 * Author:
 * Create: 2025-10
 */

#ifndef PERIODIC_UPDATE_INTERFACE_H
#define PERIODIC_UPDATE_INTERFACE_H

namespace OHOS {
class PeriodicUpdateInterface {
public:
    virtual void Update(void) = 0;
    virtual uint32_t GetPeriod(void) = 0;
    PeriodicUpdateInterface() = default;
    virtual ~PeriodicUpdateInterface() = default;
    PeriodicUpdateInterface(const PeriodicUpdateInterface&) = delete;
    PeriodicUpdateInterface& operator=(const PeriodicUpdateInterface&) = delete;
    PeriodicUpdateInterface(PeriodicUpdateInterface&&) = delete;
    PeriodicUpdateInterface& operator=(PeriodicUpdateInterface&&) = delete;
};
}

#endif // PERIODIC_UPDATE_INTERFACE_H