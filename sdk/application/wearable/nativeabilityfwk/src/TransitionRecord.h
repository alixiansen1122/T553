/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: PageTransitionMgr
 * Author:
 * Create: 2024-12
 */
#ifndef TRANSITION_RECORD_H
#define TRANSITION_RECORD_H
#include "TransitionType.h"
#include "gfx_utils/list.h"

namespace OHOS {
class TransitionRecord : public HeapBase {
public:
    static TransitionRecord& GetInstance()
    {
        static TransitionRecord instance;
        return instance;
    }
    void SaveTransitionOp(TransitionCaller caller, const TransitionTarget& target, TransitionType type,
        bool enableSlideBack);
    bool IsPendingOpsEmpty();
    bool ConfigOperationToRun();
    void RunTransitionOperation();
    bool IsDelayRunningOperation(TransitionCaller caller, const TransitionTarget& target);
private:
    TransitionRecord() {};
    ~TransitionRecord() {};
    struct TransitionOperation {
        TransitionCaller caller;
        TransitionType type;
        TransitionTarget target;
        bool enableSlideBack;
    };
    List<TransitionOperation*> pendingOps_{};
    TransitionOperation* opToRun_{nullptr};
};
} // namespace OHOS
#endif // TRANSITION_RECORD_H