/*
 * Copyright (c) 2024 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dfx/dfx_dom_dump.h"
#include "graphic_service.h"
#include "draw/draw_image.h"
#include "common/graphic_utils.h"

namespace OHOS {
static const struct {
    UIViewType type;
    const char *name;
} g_type2Name[] = {
    {UI_ROOT_VIEW, "UI_ROOT_VIEW"},
    {UI_VIEW_GROUP, "UI_VIEW_GROUP"},
    {UI_LABEL, "UI_LABEL"},
    {UI_ARC_LABEL, "UI_ARC_LABEL"},
    {UI_LABEL_BUTTON, "UI_LABEL_BUTTON"},
    {UI_CHECK_BOX, "UI_CHECK_BOX"},
    {UI_TOGGLE_BUTTON, "UI_TOGGLE_BUTTON"},
    {UI_RADIO_BUTTON, "UI_RADIO_BUTTON"},
    {UI_IMAGE_VIEW, "UI_IMAGE_VIEW"},
    {UI_BOX_PROGRESS, "UI_BOX_PROGRESS"},
    {UI_SLIDER, "UI_SLIDER"},
    {UI_CIRCLE_PROGRESS, "UI_CIRCLE_PROGRESS"},
    {UI_SCROLL_VIEW, "UI_SCROLL_VIEW"},
    {UI_LIST, "UI_LIST"},
    {UI_DIGITAL_CLOCK, "UI_DIGITAL_CLOCK"},
    {UI_ANALOG_CLOCK, "UI_ANALOG_CLOCK"},
#if ENABLE_SWEEP_CLOCK
    {UI_SWEEP_CLOCK, "UI_SWEEP_CLOCK"},
#endif
    {UI_PICKER, "UI_PICKER"},
    {UI_SWIPE_VIEW, "UI_SWIPE_VIEW"},
    {UI_TIME_PICKER, "UI_TIME_PICKER"},
    {UI_ABSTRACT_CLOCK, "UI_ABSTRACT_CLOCK"},
    {UI_ABSTRACT_PROGRESS, "UI_ABSTRACT_PROGRESS"},
    {UI_ABSTRACT_SCROLL, "UI_ABSTRACT_SCROLL"},
    {UI_AXIS, "UI_AXIS"},
    {UI_BUTTON, "UI_BUTTON"},
    {UI_CANVAS, "UI_CANVAS"},
    {UI_CHART, "UI_CHART"},
    {UI_IMAGE_ANIMATOR_VIEW, "UI_IMAGE_ANIMATOR_VIEW"},
    {UI_REPEAT_BUTTON, "UI_REPEAT_BUTTON"},
    {UI_TEXTURE_MAPPER, "UI_TEXTURE_MAPPER"},
    {UI_DIALOG, "UI_DIALOG"},
    {UI_QRCODE, "UI_QRCODE"},
    {UI_LABEL_EXT, "UI_LABEL_EXT"},
    {UI_LABEL_BUTTON_EXT, "UI_LABEL_BUTTON_EXT"},
    {UI_CANVAS_EXT, "UI_CANVAS_EXT"},
    {UI_CHART_PILLAR, "UI_CHART_PILLAR"},
    {UI_CHART_POLYLINE, "UI_CHART_POLYLINE"},
    {UI_X_AXIS, "UI_X_AXIS"},
    {UI_Y_AXIS, "UI_Y_AXIS"},
    {UI_LITE_SURFACE_VIEW, "UI_LITE_SURFACE_VIEW"},
    {LAYOUT, "LAYOUT"},
    {LIST_LAYOUT, "LIST_LAYOUT"},
    {FLEX_LAYOUT, "FLEX_LAYOUT"},
    {GRID_LAYOUT, "GRID_LAYOUT"},
#if ENABLE_SOFT_KEYBOARD
    {UI_EDIT_TEXT, "UI_EDIT_TEXT"},
    {UI_TEXT_PREVIEW_BOX, "UI_TEXT_PREVIEW_BOX"},
    {UI_KEYBOARD_BUTTON, "UI_KEYBOARD_BUTTON"},
#endif
    {UI_MENU_ITEM, "UI_MENU_ITEM"},
    {UI_CARD_PAGE, "UI_CARD_PAGE"},
    {UI_COVER_FLOW, "UI_COVER_FLOW"},
    {UI_FOLDABLE_VIEW, "UI_FOLDABLE_VIEW"},
    {UI_CROSS_VIEW, "UI_CROSS_VIEW"},
    {UI_BARCODE, "UI_BARCODE"},
    {UI_MAP_VIEW, "UI_MAP_VIEW"},
    {UI_KALEIDOSCOPE_VIEW, "UI_KALEIDOSCOPE_VIEW"},
    {UI_ROLLER_VIEW, "UI_ROLLER_VIEW"},
    {UI_HEXAGONS_LIST, "UI_HEXAGONS_LIST"},
    {UI_IMAGE_PICKER, "UI_IMAGE_PICKER"},
    {UI_ICOSAHEDRON_VIEW, "UI_ICOSAHEDRON_VIEW"},
    {UI_TRANSFORM_GROUP, "UI_TRANSFORM_GROUP"},
    {UI_TRANSFORM_LIST, "UI_TRANSFORM_LIST"},
    {UI_COVER_FLOW_2, "UI_COVER_FLOW_2"},
    {UI_SPHERE_VIEW, "UI_SPHERE_VIEW"},
    {UI_PARTICLE_VIEW, "UI_PARTICLE_VIEW"},
    {UI_OPTION_BOX, "UI_OPTION_BOX"},
    {UI_CARD_CONTAINER, "UI_CARD_CONTAINER"},
    {UI_VIDEO, "UI_VIDEO"},
    {UI_LOTT_VIEW, "UI_LOTT_VIEW"},
    {UI_FLOAT_BOX_VIEW, "UI_FLOAT_BOX_VIEW"},
    {UI_TRANSFORM_BUTTON, "UI_TRANSFORM_BUTTON"},
    {UI_NUMBER_MAX, "UI_NUMBER_MAX"},
};

DfxDomDump::DfxDomDump()
{
}

DfxDomDump::~DfxDomDump()
{
}

DfxDomDump& DfxDomDump::GetInstance()
{
    static DfxDomDump dfxDomDump;
    return dfxDomDump;
}

void DfxDomDump::DumpRootView()
{
    DumpView(RootView::GetInstance());
}

void DfxDomDump::DumpView(UIView *view)
{
    if (view == nullptr) {
        return;
    }

    Graphic::Vector<DomNode*> stackView; // traverse stack
    Graphic::Vector<DomNode*> buf; // nodes to free
    DomNode domNode(nullptr, view, nullptr);
    stackView.PushBack(&domNode);
    while (!stackView.IsEmpty()) {
        DomNode &curNode = *(stackView.Back());
        stackView.PopBack();
        curNode.obj = cJSON_CreateObject();
        if (curNode.obj == nullptr) {
            break;
        }

        if (!RecordAndAddToParent(curNode, buf)) {
            break;
        }

        if (!AddAttribute(curNode.obj, curNode.view)) {
            break;
        }

        UIViewGroup *group = dynamic_cast<UIViewGroup*>(curNode.view);
        if (group == nullptr) {
            continue;
        }

        if (!AddChildNodeToStack(group, &curNode, stackView)) {
            break;
        }
    }
    PrintAndReleaseCjson(domNode.obj);

    FreeDomNodeVector(buf);
    FreeDomNodeVector(stackView);
}

bool DfxDomDump::AddAttribute(cJSON *node, UIView *view) const
{
    if (node == nullptr || view == nullptr) {
        return false;
    }
    bool ret = AddCommonAttribute(node, view);
    AddSpecialAttribute(node, view);
    return ret;
}

bool DfxDomDump::AddCommonAttribute(cJSON *node, UIView *view) const
{
    if (node == nullptr || view == nullptr) {
        return false;
    }

    Rect rect = view->GetRect();
    const size_t size = 256;
    char *buf = new char[size];
    defer {
        if (buf != nullptr) {
            delete []buf;
        }
    };
    cJSON_AddStringToObject(node, "typename", UIViewType2Name(view->GetViewType()));
    if (buf == nullptr) {
        return false;
    }
    memset_s(buf, size, 0, size);
    sprintf_s(buf, size, "[ptr %p, x %d y %d w %d h %d, opa %u, visible %d, touchable %d, draggable %d, view-id %s]",
        view, rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), view->GetOpaScale(), view->IsVisible(),
        view->IsTouchable(), view->IsDraggable(), view->GetViewId());
    cJSON_AddStringToObject(node, "common-info", buf);
    return true;
}

void DfxDomDump::AddSpecialAttribute(cJSON *node, UIView *view) const
{
    if (node == nullptr || view == nullptr) {
        return;
    }
    cJSON_AddStringToObject(node, "special info", view->GetGuiInfo().c_str());
}

bool DfxDomDump::AddChildNodeToStack(UIViewGroup *group, DomNode *parent, Graphic::Vector<DomNode*> &stackView) const
{
    if (group == nullptr || parent == nullptr) {
        return false;
    }
    Graphic::Vector<UIView*> reverse;
    for (UIView *child = group->GetChildrenHead();
            child != nullptr; child = child->GetNextSibling()) {
        reverse.PushBack(child);
    }
    int size = reverse.Size();
    if (size == 0) {
        return true;
    }
    if (cJSON_AddNumberToObject(parent->obj, "child-num", size) == nullptr) {
        return false;
    }
    if (cJSON_AddArrayToObject(parent->obj, "children") == nullptr) {
        return false;
    }
    for (int i = size - 1; i >= 0; --i) {
        DomNode *childNode = new DomNode(parent, reverse[i]);
        if (childNode == nullptr) {
            return false;
        }
        stackView.PushBack(childNode);
    }
    return true;
}

bool DfxDomDump::RecordAndAddToParent(DomNode &curNode, Graphic::Vector<DomNode*> &buf) const
{
    if (curNode.parent != nullptr) {
        buf.PushBack(&curNode);
        cJSON *parentArray = cJSON_GetObjectItemCaseSensitive(curNode.parent->obj, "children");
        if (parentArray == nullptr) {
            return false;
        }
        if (!cJSON_AddItemToArray(parentArray, curNode.obj)) {
            return false;
        }
    }
    return true;
}

void DfxDomDump::FreeDomNodeVector(Graphic::Vector<DomNode*> &vec) const
{
    for (uint16_t i = 0; i < vec.Size(); ++i) {
        if (vec[i]->parent == nullptr) {
            continue;
        }
        delete vec[i];
    }
}

void DfxDomDump::PrintAndReleaseCjson(cJSON *obj) const
{
    if (obj == nullptr) {
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent([obj]() {
        FILE *file = fopen(RES_PATH"view_tree.json", "w");
        defer {
            if (file != nullptr) {
                fclose(file);
            }
        };
        char *info = cJSON_Print(obj);
        if (info != nullptr) {
            printf("%s\n", info);
            if (file != nullptr) {
                fwrite(info, 1, strlen(info), file);
            }
            cJSON_free(info);
        }
        cJSON_free(obj);
    });
}

const char *DfxDomDump::UIViewType2Name(UIViewType type) const
{
    constexpr int size = sizeof(g_type2Name) / sizeof(g_type2Name[0]);
    for (int i = 0; i < size; ++i) {
        if (g_type2Name[i].type == type) {
            return g_type2Name[i].name;
        }
    }
    return "unkonwn";
}
}
