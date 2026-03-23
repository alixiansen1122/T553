/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: ModelDialDataFactory
 * Author:
 * Create: 2023-12
 */

#ifndef MODE_DIAL_DATA_FACTORY
#define MODE_DIAL_DATA_FACTORY

#include <unordered_map>
#include "main/dial/DialBaseModel.h"
#include "main/dial/DialDataBase.h"
#include "main/dial/ModelDialDataFactory.h"

namespace OHOS {
class ModelDialDataFactory : public HeapBase {
public:
    ModelDialDataFactory() {}
    virtual ~ModelDialDataFactory() {}

    static ModelDialDataFactory& GetInstance()
    {
        static ModelDialDataFactory instance;
        return instance;
    }

    void RegisterModel(DialDataType type, DialBaseModel* model)
    {
        models_[static_cast<int16_t>(type)] = model;
    }

    bool RegisterDialDataListener(DialDataType type, OnDialDataUpdateListener* listener);
    bool UnRegisterDialDataListener(DialDataType type);
    bool GetDialTextData(DialDataType& type, std::string*& out, int16_t& strNum);
    bool GetDialFloatData(DialDataType& type, float& out);
    bool GetDialFloatData(DialDataType& type, float*& out, int16_t& len);
    DialBaseModel* GetModuleByType(DialDataType& type);
    void DumpModules();

private:
    std::unordered_map<int16_t, DialBaseModel*> models_;
};
}
#endif
