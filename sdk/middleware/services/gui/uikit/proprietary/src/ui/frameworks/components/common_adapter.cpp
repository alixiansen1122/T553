/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Generic type adapter for view management
 * Author: Hisi Team
 * Created: 2025-07
 */
#include "components/common_adapter.h"
#include "components/ui_simple_list.h"
namespace OHOS {
CommonAdapter::CommonAdapter()
{
}

CommonAdapter::~CommonAdapter()
{
    ClearAll();
}

uint16_t CommonAdapter::GetCount()
{
    return contents_.Size();
}

uint8_t CommonAdapter::GetViewType(int16_t index)
{
    if (index < 0 || index >= contents_.Size()) {
        return 0;
    }

    auto& content = contents_[index];
    return content.type;
}

UIView* CommonAdapter::GetView(UIView* inView, int16_t index)
{
    if (index < 0 || index >= contents_.Size()) {
        return nullptr;
    }

    auto& content = contents_[index];

    UIView* view = inView;
    if (inView == nullptr) {
        view = content.createFunc(content.type);
    }

    if (content.updateFunc && view) {
        view->SetViewId(reinterpret_cast<char*>(content.type & 0xff));
        content.updateFunc(view, content.data, content.type);
        view->SetViewIndex(index);
    }

    return view;
}

void CommonAdapter::AddContent(Contents& data)
{
    contents_.PushBack(data);
}

bool CommonAdapter::UpdateContent(int16_t index, Contents& data)
{
    if (index < 0 || index >= contents_.Size()) {
        return false;
    }

    auto& content = contents_[index];
    content.createFunc = data.createFunc;
    content.updateFunc = data.updateFunc;
    content.type = data.type;
    content.data = data.data;
    return true;
}

void CommonAdapter::ClearAll()
{
    contents_.Clear();
}

bool CommonAdapter::RemoveContent(int16_t index)
{
    if (index < 0 || index >= contents_.Size()) {
        return false;
    }
    contents_.Erase(index);
    return true;
}
}
