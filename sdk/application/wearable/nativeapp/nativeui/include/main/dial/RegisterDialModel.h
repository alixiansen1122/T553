/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: RegisterDialModel
 * Author:
 * Create: 2023-12
 */

#ifndef REGISTER_DIAL_MODEL_H
#define REGISTER_DIAL_MODEL_H

#include "main/dial/DialDataBase.h"
#include "main/dial/ModelDialDataFactory.h"
namespace OHOS {
class RegisterDialModel {
public:
    RegisterDialModel(DialDataType type, DialBaseModel* func)
    {
        ModelDialDataFactory::GetInstance().RegisterModel(type, func);
    }
    virtual ~RegisterDialModel() {}
};
}

#define REGIST_DIAL_MODULE(type, func) static OHOS::RegisterDialModel(REGIST_OCCURRENCES)(type, func)
#endif
