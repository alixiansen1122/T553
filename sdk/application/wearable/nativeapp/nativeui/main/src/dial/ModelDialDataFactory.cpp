/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: ModelDialDataFactory
 * Author:
 * Create: 2023-12
 */

#include "main/dial/ModelDialDataFactory.h"
namespace OHOS {
bool ModelDialDataFactory::GetDialTextData(DialDataType& type, std::string*& out, int16_t& strNum)
{
    DialBaseModel* model = GetModuleByType(type);
    if (model == nullptr) {
        return false;
    }
    return model->GetDialTextData(type, out, strNum);
}

bool ModelDialDataFactory::GetDialFloatData(DialDataType& type, float& out)
{
    DialBaseModel* model = GetModuleByType(type);
    if (model == nullptr) {
        return false;
    }
    return model->GetDialFloatData(type, out);
}

bool ModelDialDataFactory::GetDialFloatData(DialDataType& type, float*& out, int16_t& len)
{
    DialBaseModel* model = GetModuleByType(type);
    if (model == nullptr) {
        return false;
    }
    return model->GetDialFloatData(type, out, len);
}

DialBaseModel* ModelDialDataFactory::GetModuleByType(DialDataType& type)
{
    auto iter = models_.find(static_cast<int16_t>(type));
    if (iter == models_.end()) {
        return nullptr;
    }
    return iter->second;
}

bool ModelDialDataFactory::RegisterDialDataListener(DialDataType type, OnDialDataUpdateListener* listener)
{
    DialBaseModel* model = GetModuleByType(type);
    if (model == nullptr) {
        return false;
    }
    model->RegisterDialDataListener(listener);
    return true;
}

bool ModelDialDataFactory::UnRegisterDialDataListener(DialDataType type)
{
    DialBaseModel* model = GetModuleByType(type);
    if (model == nullptr) {
        return false;
    }
    model->UnRegisterDialDataListener();
    return true;
}

void ModelDialDataFactory::DumpModules()
{
    printf("ModelDialDataFactory::DumpModules start\n");
    for (auto it : models_) {
        printf("module: %d\n", it.first);
    }
    printf("ModelDialDataFactory::DumpModules end\n");
}
}
