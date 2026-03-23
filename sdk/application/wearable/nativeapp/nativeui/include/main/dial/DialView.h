/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef DIAL_VIEW_H
#define DIAL_VIEW_H

#include <cstdint>
#include <string>
#include "main/dial/DialDataType.h"

namespace OHOS {

class DialView {
public:
    DialView() {}
    virtual ~DialView() {}

    virtual void HandleFloatData(float data) {}
    virtual void HandleFloatData(const float* data, uint16_t num) {}
    virtual void HandleTextData(const std::string* data, uint16_t num) {}
    virtual DialDataType GetDataType()
    {
        return type_;
    }

    virtual void SetDataType(DialDataType type)
    {
        type_ = type;
    }

private:
    DialDataType type_ = DialDataType::DATA_TYPE_MAX;
};
}
#endif
