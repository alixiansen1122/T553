/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#ifndef UI_TEST_CANVAS_EXT_H
#define UI_TEST_CANVAS_EXT_H

#include "ui_test.h"
#include "components/ui_canvas.h"
#include "components/ui_scroll_view.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label_button.h"
#include "common/image_cache_manager.h"
namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
class SectorAnimatorCallback : public AnimatorCallback {
public:
    SectorAnimatorCallback() {}
    ~SectorAnimatorCallback() override {}
    void Callback(UIView *view) override;
private:
    float sectorAngle_ = -3600.0f;
    float sectorRange_ = 0.0f;
    float angleStep_ = 4.0f;
    float rangeStep_ = 4.0f;
};

class UITestCanvasExt : public UITest, public UIView::OnClickListener {
public:
    UITestCanvasExt() {}
    ~UITestCanvasExt() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void Draw123(UICanvasExt* canvas);
    void DrawSquare(UICanvasExt* canvas);
    void Add1(UICanvasExt* canvas);
    void Add2(UICanvasExt* canvas);
    void Add3(UICanvasExt* canvas);
    void DrawCurve(UICanvasExt* canvas);
    void DrawRect(UICanvasExt* canvas);
    void DrawRoundRect(UICanvasExt* canvas);
    void DrawLine(UICanvasExt* canvas);
    void DrawArc(UICanvasExt* canvas);
    void DrawSector(UICanvasExt* canvas);
    void SetUpBtn(UILabelButton*& btn, const char* text, int16_t x, int16_t y);
    void DrawPattern(UICanvasExt* canvas);
    void DrawImage(UICanvasExt* canvas);
    void DrawLabel(UICanvasExt* canvas);
    int16_t matrixIndex_ = -1;
    UICanvasExt* canvas_ = nullptr;
    UILabelButton* addSquareBtn_ = nullptr;
    UILabelButton* addSectorBtn_ = nullptr;
    UILabelButton* clearBtn_ = nullptr;
    UILabelButton* transBtn_ = nullptr;
    UILabelButton* scaleBtn_ = nullptr;
    UILabelButton* rotateBtn_ = nullptr;
    UILabelButton* matrixBtn_ = nullptr;
    UILabelButton* paddingBtn_ = nullptr;
    UIScrollView* container_ = nullptr;
    SectorAnimatorCallback *sectorCallBack_ = nullptr;
    Animator *sectorAnimator_ = nullptr;
    ImageInfo* img_ = nullptr;
};
#endif
}
#endif