/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: TransitionManager
 * Author:
 * Create: 2023-11
 */

#ifndef TRANSITION_MANAGER_H
#define TRANSITION_MANAGER_H

#include "gfx_utils/vector.h"
#include "TransitionCallback.h"
#include "TransitionType.h"

namespace OHOS {
struct TransitionMap {
    TransitionType type;
    TransitionCallback* transition;
};

class TransitionManager : public HeapBase {
public:
    static TransitionManager& GetInstance()
    {
        static TransitionManager instance;
        return instance;
    }
    TransitionCallback* GetTransition(TransitionType type);
    void RegisterTransition(TransitionType type, TransitionCallback* transition);

private:
    TransitionManager() {};
    ~TransitionManager();
    TransitionManager(const TransitionManager &) = delete;
    TransitionManager &operator=(const TransitionManager &) = delete;
    TransitionManager(TransitionManager &&) = delete;
    TransitionManager &operator=(TransitionManager &&) = delete;
    Graphic::Vector<TransitionMap> transitions_;
};

class TransitionRegister : public HeapBase {
public:
    TransitionRegister(TransitionType type, TransitionCallback* transition)
    {
        TransitionManager::GetInstance().RegisterTransition(type, transition);
    }
};
} // namespace OHOS

#define REGIST_TRANSITION(type, func, time, enableCurSnapShot, enableTargetSnapShot) \
    const static OHOS::TransitionRegister (STATIC_VAR)(type, new func(time, enableCurSnapShot, enableTargetSnapShot))
#endif // TRANSITION_MANAGER_H