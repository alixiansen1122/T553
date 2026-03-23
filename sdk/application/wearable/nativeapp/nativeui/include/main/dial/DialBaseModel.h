/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialBaseModel
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_BASE_MODEL_H
#define DIAL_BASE_MODEL_H

#include <cstdint>
#include "heap_base.h"
#include "main/dial/OnDialDataUpdateListener.h"

namespace OHOS {
class DialBaseModel : public HeapBase {
public:
    DialBaseModel() {}
    virtual ~DialBaseModel() {}

    /* *
     * @brief Regist on dial data change listener
     */
    void RegisterDialDataListener(OnDialDataUpdateListener* listener)
    {
        onChangeListener_ = listener;
    }

    /* *
     * @brief Unregist on dial data change listener
     */
    void UnRegisterDialDataListener()
    {
        onChangeListener_ = nullptr;
    }

    /* *
     * @brief Get dial data of text type
     */
    virtual bool GetDialTextData(DialDataType& type, std::string*& out, int16_t& strNum)
    {
        return false;
    }

    /* *
     * @brief Get dial data of single float type
     */
    virtual bool GetDialFloatData(DialDataType& type, float& out)
    {
        return false;
    }

    /* *
     * @brief Get dial data of multi float type
     */
    virtual bool GetDialFloatData(DialDataType& type, float*& out, int16_t len)
    {
        return false;
    }

protected:
    OnDialDataUpdateListener* onChangeListener_ = nullptr;
};
}
#endif
