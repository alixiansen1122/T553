/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: ace_ext listadapter
 * Author: SoftwarePlatform Group
 * Create: 2024-08-10
 */

#ifndef OHOS_ACELITE_LIST_ADAPTER_EXT_H
#define OHOS_ACELITE_LIST_ADAPTER_EXT_H

#include <string>
#include <vector>
#include "text_component.h"

namespace OHOS {
namespace ACELite {

class ListAdapterExt {
public:
    ListAdapterExt();

    ~ListAdapterExt();

    static void SetItemAdaptive(bool isItemAdaptive);

    static bool GetItemAdaptive();

    static void SetListValue(char *text);

    static void SetItemIndex(int16_t index);

    static int16_t GetItemIndex();

    static int16_t BuildItemAdaptive(Component *component, int16_t index);

    static void CleanVector();

    static void CleanString();

    static void CleanAll();

private:
    static int16_t CalculateItems(UILabel *uiLable, int16_t index);
    static bool isItemAdaptive_;
    static int16_t itemIndex_;
    static std::vector<std::string> strings_;
    static std::string all_;
};
}  // namespace ACELite
}  // namespace OHOS
#endif // OHOS_ACELITE_LIST_ADAPTER_EXT_H
