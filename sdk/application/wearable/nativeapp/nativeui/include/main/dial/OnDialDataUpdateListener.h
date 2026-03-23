/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: OnDialDataUpdateListener
 * Author:
 * Create: 2023-12
 */

#ifndef ON_DIAL_DATA_UPDATE_LISTENER_H
#define ON_DIAL_DATA_UPDATE_LISTENER_H

#include "main/dial/DialDataBase.h"

namespace OHOS {
class OnDialDataUpdateListener : public HeapBase {
public:
    OnDialDataUpdateListener() {}
    virtual ~OnDialDataUpdateListener() {}
    virtual void OnDialFloatDataUpdate(DialDataType type, float data) {}
    virtual void OnDialFloatDataUpdate(DialDataType type, const float* data, uint16_t num) {}
    virtual void OnDialTextDataUpdate(DialDataType type, const std::string* data, uint16_t num) {}
};
}
#endif
