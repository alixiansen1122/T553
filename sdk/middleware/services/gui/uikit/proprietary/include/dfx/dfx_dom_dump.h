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

#ifndef GRAPHIC_LITE_DFX_DOM_DUMP_H
#define GRAPHIC_LITE_DFX_DOM_DUMP_H
#include <cstdlib>
#include "cJSON.h"
#include "graphic_config.h"
#include "gfx_utils/vector.h"
#include "components/ui_view.h"
#include "components/root_view.h"

namespace OHOS {
class DfxDomDump : public HeapBase {
    struct DomNode : public HeapBase {
        DomNode *parent = nullptr;
        UIView *view = nullptr;
        cJSON *obj = nullptr;
        DomNode(DomNode *par = nullptr, UIView *vw = nullptr, cJSON *object = nullptr)
            : parent(par), view(vw), obj(object) {}
        ~DomNode() {}
    };
public:
    static DfxDomDump &GetInstance();
    virtual ~DfxDomDump();
    DfxDomDump(const DfxDomDump&) = delete;
    DfxDomDump &operator=(const DfxDomDump&) = delete;

    void DumpRootView();
    void DumpView(UIView *view);

private:
    DfxDomDump();
    bool AddAttribute(cJSON *node, UIView *view) const;
    bool AddCommonAttribute(cJSON *node, UIView *view) const;
    void AddSpecialAttribute(cJSON *node, UIView *view) const;
    bool AddChildNodeToStack(UIViewGroup *group, DomNode *parent, Graphic::Vector<DomNode*> &stackView) const;
    bool RecordAndAddToParent(DomNode &curNode, Graphic::Vector<DomNode*> &buf) const;
    void FreeDomNodeVector(Graphic::Vector<DomNode*> &vec) const;
    void PrintAndReleaseCjson(cJSON *obj) const;
    const char *UIViewType2Name(UIViewType type) const;
};
}
#endif // GRAPHIC_LITE_DFX_DOM_DUMP_H
