/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TIGER_VIEW_H
#define TIGER_VIEW_H

#include "components/ui_label.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_view_group.h"
#include "common/screen.h"
#include "animator/animator_manager.h"
#include "components/ui_scroll_view.h"
#include "hal_tick.h"

namespace OHOS {
class TigerAnimatorCallback : public AnimatorCallback {
public:
    explicit TigerAnimatorCallback(UICanvasExt *canvas) : canvas_(canvas) {}
    ~TigerAnimatorCallback() override {}
    void Callback(UIView *view) override
    {
        uint32_t elapseTime = HALTick::GetInstance().GetElapseTime(startTime_);
        if (elapseTime <= ANIMATE_TIME) {
            rect = canvas_->GetRelativeRect();
            scale = 1 - 0.3 * static_cast<float>(elapseTime) / ANIMATE_TIME; // 0.3: max attenuation rate
            x = (center.x - rect.GetLeft()) * (1 - scale);
            y = (center.y - rect.GetTop()) * (1 - scale) + 800 * scale; // 800: y translate length
            canvas_->Scale({ scale, -scale }, {0, 0});
            canvas_->Translate({x, y});
            canvas_->Invalidate();
        }
    }

    uint32_t startTime_ = 0;
    Point center = {0, 0};
private:
    static constexpr int16_t ANIMATE_TIME = 2000; // 2000: animate time
    UICanvasExt *canvas_ { nullptr };
    float scale = 1.0;
    Rect rect;
    int16_t x = 0;
    int16_t y = 0;
};

class TigerView : public UIScrollView {
public:
    struct PathHeader {
        int cmdsSize;
        int cmdIndexSize;
        int cmdNumSize;
        int dataSize;
        int dataIndexSize;
        int dataNumSize;
        int colorSize;
    };

    struct PathInfo {
        PathHeader header;
        int cmdCounts;
        unsigned char *cmds;
        int *cmdIndex;
        int *cmdNum;
        float *data;
        int *dataIndex;
        int *dataNum;
        unsigned int *color;
    };

    TigerView();
    ~TigerView() override;
    TigerView(const TigerView &) = delete;
    TigerView &operator=(const TigerView &) = delete;
    bool InitView();
    TigerAnimatorCallback *callBack { nullptr };
    Animator *tigerAnimator { nullptr };
    bool OnLongPressEvent(const LongPressEvent& event) override;
    bool OnReleaseEvent(const ReleaseEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;

private:
    int TigerPathInit(PathInfo *path);
    void TigerPathDeinit(PathInfo *path);
    void LoadTigerPathData();

    bool isScaleStart { false };
    bool isNeedReset { false };
    PathInfo pathInfo { 0 };
    UICanvasExt *canvas { nullptr };
};
} // namespace OHOS

#endif // TIGER_VIEW_H
