/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: AodModel
 * Author:
 * Create: 2024-10-30
 */

#ifndef AODMODEL_H
#define AODMODEL_H
#include "main/MainModel.h"

namespace OHOS {
class AodModel {
public:
    static AodModel& GetInstance()
    {
        static AodModel model;
        return model;
    }
    void GetAodDialSetting(DialSetting &dial);
    void SaveAodDialSetting(const DialSetting &dial);

private:
    AodModel();
    ~AodModel();
    AodModel(const AodModel&) = delete;
    AodModel& operator=(const AodModel&) = delete;
    DialSetting aodDialSetting_;
};
} // OHOS
#endif // AODMODEL_H