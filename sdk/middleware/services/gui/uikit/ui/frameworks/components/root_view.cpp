/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/root_view.h"

#include "common/screen.h"
#include "core/render_manager.h"
#include "draw/draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/graphic_log.h"
#if ENABLE_WINDOW
#include "window/window_impl.h"
#endif
#if ENABLE_DFX_CMD
#include "dfx/dfx_cmd.h"
#endif
#include "securec.h"
#include "graphic_mutex.h"

namespace OHOS {
namespace {
#if LOCAL_RENDER
const constexpr uint8_t MAX_SPLIT_NUM = 32; // split at most 32 parts
// view along with its parents and siblings are at most 128
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH * 2;
#else
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH;
const constexpr uint8_t MAX_INVALIDATE_SIZE = 24;
#endif
static Rect g_maskStack[COMPONENT_NESTING_DEPTH];
static UIView* g_viewStack[VIEW_STACK_DEPTH];
} // namespace
RootView::RootView()
{
    InitDrawContext();
    appView_.SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    appView_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    UIViewGroup::Add(&appView_);

    sysView_.SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    sysView_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    UIViewGroup::Add(&sysView_);
}

RootView* RootView::GetInstance()
{
    static RootView instance;
    return &instance;
}

RootView::~RootView()
{
    DestroyDrawContext();
}
#if ENABLE_WINDOW
Window* RootView::GetBoundWindow() const
{
    return boundWindow_;
}
#endif

Rect RootView::GetScreenRect()
{
#if ENABLE_WINDOW
    Rect screenRect = GetRect();
#else
    Rect screenRect(0, 0, Screen::GetInstance().GetWidth() - 1, Screen::GetInstance().GetHeight() - 1);
#endif
    return screenRect;
}

#if LOCAL_RENDER
using namespace Graphic;
static void DivideInvalidateRect(const Rect& originRect, Rect& leftoverRect, Vector<Rect>& splitRects)
{
    Rect mask;
    if (!mask.Intersect(originRect, leftoverRect)) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    /*
     *   +---+---+---+
     *   |   | A |   |      originRect           :A+B
     *   |   +---+   |      leftoverRect         :A->0
     *   |     B     |      mask                 :A
     *   +-----------+
     */
    if (originRect.IsContains(leftoverRect)) {
        return;
    }

    int16_t reserveCnt = MAX_SPLIT_NUM - splitRects.Size();
    if (reserveCnt <= 0) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    if (mask.GetWidth() == leftoverRect.GetWidth()) {
        /*
         *       +---+
         *       | A |        originRect           :B+C
         *   +-----------+    leftoverRect         :A+B->A
         *   |   | B |   |    mask                 :B
         *   |   +---+   |
         *   |     C     |
         *   +-----------+
         */
        if (mask.GetBottom() == leftoverRect.GetBottom()) {
            leftoverRect.SetBottom(mask.GetTop() - 1);
        } else if (mask.GetTop() == leftoverRect.GetTop()) {
            leftoverRect.SetTop(mask.GetBottom() + 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetBottom(mask.GetTop() - 1);
            leftoverRect.SetTop(mask.GetBottom() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }
    if (mask.GetHeight() == leftoverRect.GetHeight()) {
        /*
         *      +---------+   originRect           :B+C
         *  +-------+     |   leftoverRect         :A+B->A
         *  | A | B |  C  |   mask                 :B
         *  +-------+     |
         *      +---------+
         */
        if (mask.GetLeft() == leftoverRect.GetLeft()) {
            leftoverRect.SetLeft(mask.GetRight() + 1);
        } else if (mask.GetRight() == leftoverRect.GetRight()) {
            leftoverRect.SetRight(mask.GetLeft() - 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetRight(mask.GetLeft() - 1);
            leftoverRect.SetLeft(mask.GetRight() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }

    Vector<Rect> copyRect(splitRects);
    if (mask.GetLeft() != leftoverRect.GetLeft()) {
        /*
         *     |
         * +-------+
         * |   +---+   mask                 :A
         * | B | A |   leftoverRect         :A+B
         * |   +---+
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetRight(mask.GetLeft() - 1);
        leftoverRect.SetLeft(mask.GetLeft());
    }

    if (mask.GetTop() != leftoverRect.GetTop()) {
        /*
         *  +-------+
         *  |   B   |   mask                 :A
         * ---+---+---  leftoverRect         :A+B
         *  | | A | |
         *  +-+---+-+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetBottom(mask.GetTop() - 1);
        leftoverRect.SetTop(mask.GetTop());
    }

    if (mask.GetRight() != leftoverRect.GetRight()) {
        /*
         *     |
         * +-------+
         * +---+   |    mask                 :A
         * | A | B |    leftoverRect         :A+B
         * +---+   |
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetLeft(mask.GetRight() + 1);
        leftoverRect.SetRight(mask.GetRight());
    }

    if (mask.GetBottom() != leftoverRect.GetBottom()) {
        /*
         *  +-+---+-+
         *  | | A | |     mask                 :A
         * ---+---+---    leftoverRect         :A+B
         *  |   B   |
         *  +-------+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetTop(mask.GetBottom() + 1);
        leftoverRect.SetBottom(mask.GetBottom());
    }
    return;
}

static void AddRenderedRects(Rect& rect, List<Rect>& renderedRects, ListNode<Rect>* iter)
{
    /* Elements at front have larger area and more relevance */
    for (; iter != renderedRects.End(); iter = iter->next_) {
        Rect& curRect = iter->data_;
        if (!curRect.IsIntersect(rect)) {
            continue;
        }

        if (curRect.IsContains(rect)) {
            return;
        }

        /* Merge two rects */
        if (rect.IsContains(curRect)) {
        } else if (((curRect.GetLeft() == rect.GetLeft()) && (curRect.GetRight() == rect.GetRight())) ||
                   ((curRect.GetTop() == rect.GetTop()) && (curRect.GetBottom() == rect.GetBottom()))) {
            rect.Join(curRect, rect);
        } else {
            continue;
        }
        iter = renderedRects.Remove(iter)->prev_;
        break;
    }
    if (iter == renderedRects.End()) {     // No merge rises
        if (renderedRects.Size() == 128) { // record 128 rendered rects at most
            renderedRects.PopBack();
        }
        renderedRects.PushFront(rect);
    } else { // merge rises, go over the rest nodes
        AddRenderedRects(rect, renderedRects, iter);
    }
}

void RootView::RemoveViewFromInvalidMap(UIView* view)
{
    lock_.Lock();
    int16_t stackCount = 0;
    do {
        while (view != nullptr) {
            /* delete node itself */
            auto entry = invalidateMap_.find(view);
            if (entry != invalidateMap_.end()) {
                invalidateMap_.erase(entry);
            }
            /* delete node's children */
            if (view->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                g_viewStack[stackCount++] = view;
                view = static_cast<UIViewGroup*>(view)->GetChildrenHead();
                continue;
            }
            /* only go to child's sibling */
            view = view->GetNextSibling();
        }
        if (--stackCount >= 0) {
            view = g_viewStack[stackCount]->GetNextSibling();
        }
    } while (stackCount >= 0);
    lock_.Unlock();
}

void RootView::OptimizeInvalidView(UIView* curview, UIView* background, List<Rect>& renderedRects)
{
    if (curview == nullptr) {
        return;
    }
    auto mapEntry = invalidateMap_.find(curview);
    if (mapEntry == invalidateMap_.end()) {
        return;
    }

    Rect& invalidRect = mapEntry->second.Front();
    /* Transparent views should draw from background */
    if ((((curview->GetStyleConst().borderWidth_ != 0) && (curview->GetStyleConst().borderOpa_ != OPA_OPAQUE)) ||
        (curview->GetStyleConst().borderRadius_ != 0) ||
        (curview->GetStyleConst().bgOpa_ != OPA_OPAQUE) || (curview->GetOpaScale() != OPA_OPAQUE) ||
        (!curview->IsTransInvalid())) && (curview != this)) {
        AddInvalidateRect(invalidRect, background);
        invalidateMap_.erase(mapEntry);
        return;
    }

    /* Remove the rendered parts and split the origin rect into splitInvalidRects
     * For performance reason, split numbers are strictly restrained.
     */
    Vector<Rect> splitInvalidRects(MAX_SPLIT_NUM << 1);
    Rect invalidRectCopy(invalidRect);
    /* Using forward order because entries at the front are closer to the current view and have larger Size */
    for (auto iter = renderedRects.Begin(); iter != renderedRects.End(); iter = iter->next_) {
        for (int8_t i = 0; i < mapEntry->second.Size(); i++) {
            DivideInvalidateRect(iter->data_, mapEntry->second[i], splitInvalidRects);
        }
        mapEntry->second.Swap(splitInvalidRects);
        splitInvalidRects.Clear();
    }

    /* Add new opaque rects */
    Rect preDrawRect(invalidRectCopy);
    if (!curview->OnPreDraw(preDrawRect)) {
        AddInvalidateRect(invalidRectCopy, background);
    }
    AddRenderedRects(preDrawRect, renderedRects, renderedRects.Begin());
}

void RootView::OptimizeInvalidMap()
{
    UIView* curview = this;
    int16_t stackCount = 0;
    int16_t opaStackCount = 0;
    UIView* background[VIEW_STACK_DEPTH];
    bool flags[VIEW_STACK_DEPTH]; // indicate whether stack go back from child
    List<Rect> renderedRects;     // Record rendered areas to avoid rerendering

    do {
        /* push stack */
        if (curview != nullptr) {
            if (stackCount >= VIEW_STACK_DEPTH) {
                return;
            }
            g_viewStack[stackCount] = curview;
            flags[stackCount++] = false;
            curview = curview->GetNextSibling();
            continue;
        }

        curview = g_viewStack[--stackCount];
        Rect rect(curview->GetRect());
        if (!curview->IsVisible() || !rect.Intersect(rect, GetScreenRect())) {
            curview = nullptr;
            continue;
        }
        if (!flags[stackCount]) { // Back from sibling
            if (curview->IsViewGroup()) {
                /* Set background/topview */
                if (((curview->GetStyleConst().bgOpa_ == OPA_OPAQUE) && (curview->GetOpaScale() == OPA_OPAQUE) &&
                     curview->IsTransInvalid()) ||
                    (curview == this)) {
                    background[opaStackCount] = curview;
                } else {
                    background[opaStackCount] = background[opaStackCount - 1];
                }
                ++opaStackCount;
                if (opaStackCount >= VIEW_STACK_DEPTH) {
                    return;
                }
                flags[stackCount++] = true;
                curview = static_cast<UIViewGroup*>(curview)->GetChildrenHead();
                continue;
            }
        } else { // Back from child
            opaStackCount--;
        }
        OptimizeInvalidView(curview, background[opaStackCount - 1], renderedRects);
        curview = nullptr;
    } while (stackCount > 0);
    renderedRects.Clear();
}

void RootView::DrawInvalidMap(const Rect& buffRect)
{
    if (blurView_ != nullptr) {
        UIView* topView = GetTopUIView(buffRect);
        DrawTop(topView, buffRect);
        return;
    }
    Rect rect;
    for (auto& viewEntry : invalidateMapDrawing_) {
        Vector<Rect>& viewRenderRect = viewEntry.second;
        for (uint16_t i = 0; i < viewRenderRect.Size(); i++) {
            rect.Intersect(viewRenderRect[i], buffRect);
            DrawTop(viewEntry.first, rect);
        }
    }
}

#else

void RootView::OptimizeAddRect(Rect& rect)
{
    Rect joinRect;
    for (ListNode<Rect>* iter = invalidateRects_.Begin(); iter != invalidateRects_.End(); iter = iter->next_) {
        if (iter->data_.IsContains(rect)) {
            return;
        }

        if (iter->data_.IsIntersect(rect)) {
            joinRect.Join(iter->data_, rect);
            if (joinRect.GetSize() < iter->data_.GetSize() + rect.GetSize()) {
                iter->data_ = joinRect;
                return;
            }
        }
    }

    if (invalidateRects_.Size() < MAX_INVALIDATE_SIZE) {
        invalidateRects_.PushBack(rect);
    } else {
        invalidateRects_.Clear();
        invalidateRects_.PushBack(GetScreenRect());
    }
}

void RootView::OptimizeInvalidateRects()
{
    Rect joinRect;
    for (ListNode<Rect>* iter1 = invalidateRects_.Begin(); iter1 != invalidateRects_.End(); iter1 = iter1->next_) {
        for (ListNode<Rect>* iter2 = invalidateRects_.Begin(); iter2 != invalidateRects_.End(); iter2 = iter2->next_) {
            if (iter1 == iter2) {
                continue;
            }

            if (iter2->data_.IsIntersect(iter1->data_)) {
                joinRect.Join(iter1->data_, iter2->data_);
                if (joinRect.GetSize() < (iter1->data_.GetSize() + iter2->data_.GetSize())) {
                    iter2->data_ = joinRect;
                    iter1 = invalidateRects_.Remove(iter1)->prev_;
                    break;
                }
            }
        }
    }
}
#endif

void RootView::AddInvalidateRect(Rect& rect, UIView* view)
{
    Rect commonRect;
    if (commonRect.Intersect(rect, GetScreenRect())) {
#if LOCAL_RENDER
        Vector<Rect>& invalidRects = invalidateMap_[view];
        if (invalidRects.IsEmpty()) {
            invalidRects.PushBack(commonRect);
        } else {
            invalidRects[0].Join(invalidRects[0], commonRect);
        }
#else
        isDrawBeforeBlurViews_[view] = view->IsVisible();
        OptimizeAddRect(commonRect);
#endif
        if (!renderFlag_) {
            invalidRect_ = commonRect;
            renderFlag_ = true;
        } else {
            invalidRect_.Join(invalidRect_, commonRect);
        }
    }
}

void RootView::AddInvalidateRectWithLock(Rect& rect, UIView* view)
{
    lock_.Lock();
    AddInvalidateRect(rect, view);
    lock_.Unlock();
}

void RootView::Measure()
{
#if LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        MeasureView(childrenHead_);
    }
#else
    if (renderFlag_) {
        MeasureView(childrenHead_);
    }
#endif
}

static bool IsDisplaying(UIView* container, UIView* view)
{
    if (view == nullptr) {
        return false;
    }
    UIView* cur = view;
    UIView* pre = nullptr;
    Rect rvRect = container->GetRect();
    while (cur != nullptr) {
        if (!cur->IsVisible()) {
            return false;
        }
        bool isIntersect = rvRect.Intersect(cur->GetContentRect(), rvRect);
        if (!isIntersect) {
            return false;
        }
        pre = cur;
        cur = cur->GetParent();
    }
    if (pre == container) {
        return true;
    }
    return false;
}

void RootView::MeasureView(UIView* view)
{
    int16_t stackCount = 0;
    UIView* curView = view;
    bool isDrawAfterBlurView = false;
    ListNode<UIView*>* node = blurViewListStack_.Head();
    Rect rvRect = GetRect();
    blurView_ = nullptr;
    while (node != nullptr && node->data_ != nullptr) {
        if (IsDisplaying(this, node->data_)) {
            blurView_ = node->data_;
            break;
        } else {
            node = node->next_;
        }
    }
    while (stackCount >= 0) {
        while (curView != nullptr) {
            if (curView->IsVisible()) {
                curView->ReMeasure();
                curView->UpdatePos();
                if (curView == blurView_) {
                    isDrawAfterBlurView = true;
                }
                if (isDrawAfterBlurView && isDrawBeforeBlurViews_.find(curView) != isDrawBeforeBlurViews_.end()) {
                    isDrawBeforeBlurViews_.erase(curView);
                }
                if (curView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = curView;
                    curView = static_cast<UIViewGroup*>(curView)->GetChildrenHead();
                    continue;
                }
            }
            curView = curView->GetNextSibling();
        }
        if (--stackCount >= 0) {
            curView = (g_viewStack[stackCount])->GetNextSibling();
        }
    }
}

#if DOUBLE_BUFFER
static bool AdjustRect(Rect& rect)
{
    bool isAdjusted = false;
    bool widthInc = false;
    bool heightInc = false;
    if (rect.GetWidth() % 2 == 1) { // 2 : width aligned by 2
        rect.SetWidth((rect.GetWidth() + 1));
        widthInc = true;
        isAdjusted = true;
    }

    // 2: height aligned by 2
    if (rect.GetHeight() % 2 == 1) {
        rect.SetHeight((rect.GetHeight() + 1));
        heightInc = true;
        isAdjusted = true;
    }

    // 2: x aligned by 2
    if (rect.GetX() % 2 == 1) {
        rect.SetX((rect.GetX() - 1));
        if (!widthInc && (rect.GetX() + rect.GetWidth() + 2 <= Screen::GetInstance().GetWidth())) { // 2: inc by 2
            rect.SetWidth((rect.GetWidth() + 2)); // 2: inc by 2
        }
        isAdjusted = true;
    }

    // 2: y aligned by 2
    if (rect.GetY() % 2 == 1) {
        rect.SetY((rect.GetY() - 1));
        if (!heightInc && (rect.GetY() + rect.GetHeight() + 2 <= Screen::GetInstance().GetHeight())) { // 2: inc by 2
            rect.SetHeight((rect.GetHeight() + 2)); // 2: inc by 2
        }
        isAdjusted = true;
    }
    return isAdjusted;
}

void RootView::SyncAndSwapBuffer(const Rect& invalidRect)
{
    Rect syncRect;
    Rect mask;
    syncRect = lastInvalidRect_;
#if LOCAL_RENDER
    if (lastInvalidRect_.IsValid() && mask.Intersect(lastInvalidRect_, invalidRect) &&
        (invalidateMapDrawing_.size() == 1)) {
#else
    if (lastInvalidRect_.IsValid() && mask.Intersect(lastInvalidRect_, invalidRect) &&
        (invalidateRects_.Size() == 1)) {
#endif
        if (mask == lastInvalidRect_) {
            syncRect = {0, 0, 0, 0};
        } else if (mask.GetWidth() == lastInvalidRect_.GetWidth()) {
            if (mask.GetTop() == lastInvalidRect_.GetTop()) {
                syncRect.SetTop(mask.GetBottom() + 1);
            } else if (mask.GetBottom() == lastInvalidRect_.GetBottom()) {
                syncRect.SetBottom(mask.GetTop() - 1);
            }
        } else if (mask.GetHeight() == lastInvalidRect_.GetHeight()) {
            if (mask.GetLeft() == lastInvalidRect_.GetLeft()) {
                syncRect.SetLeft(mask.GetRight() + 1);
            } else if (mask.GetRight() == lastInvalidRect_.GetRight()) {
                syncRect.SetRight(mask.GetLeft() - 1);
            }
        }
    }
    if (syncRect.IsValid()) {
        LiteMGfxEngine::GetInstance()->SwapBuffer(&syncRect);
    } else {
        LiteMGfxEngine::GetInstance()->SwapBuffer(nullptr);
    }
    lastInvalidRect_ = invalidRect;
}
#endif

bool RootView::Render()
{
#if ENABLE_DFX_CMD
    if (needScreencap_) {
        SaveScreencapToFile();
        needScreencap_ = false;
    }
#endif
    lock_.Lock();
#if LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        if (blurView_ == nullptr) {
            OptimizeInvalidMap();
        }
    }
    invalidateMapDrawing_.swap(invalidateMap_);
#else
    if (renderFlag_) {
        isBlurCacheNeedRefresh_ = (blurView_ != nullptr && !isDrawBeforeBlurViews_.empty());
        if (blurView_ != nullptr) {
            Rect rect = blurView_->GetRect();
            AddInvalidateRect(rect, blurView_);
        }
        isDrawBeforeBlurViews_.clear();
    }
#endif
    Rect invalidRectTmp = invalidRect_;
    invalidRect_ = {0, 0, 0, 0};
    bool renderFlagTmp = renderFlag_;
    renderFlag_ = false;
    lock_.Unlock();

#if LOCAL_RENDER
    if (!invalidateMapDrawing_.empty()) {
#if DOUBLE_BUFFER
        bool adjusted = AdjustRect(invalidRectTmp);
        if (adjusted) {
            invalidateMapDrawing_.clear();
            invalidateMapDrawing_[GetTopUIView(invalidRectTmp)].PushBack(invalidRectTmp);
        }
        SyncAndSwapBuffer(invalidRectTmp);
#endif
#if ENABLE_VGU_ENGINE
        LiteMGfxEngine::GetInstance()->StartDraw();
#endif
        RenderManager::RenderRect(GetRect(), this);
        invalidateMapDrawing_.clear();
#else
    if (renderFlagTmp) {
#if DOUBLE_BUFFER
        bool adjusted = AdjustRect(invalidRectTmp);
        if (adjusted) {
            invalidateRects_.Clear();
            invalidateRects_.PushBack(invalidRectTmp);
        } else {
            OptimizeInvalidateRects();
        }
        SyncAndSwapBuffer(invalidRectTmp);
#else
        OptimizeInvalidateRects();
#endif
        if (invalidateRects_.Size() > 0) {
#if ENABLE_VGU_ENGINE
            LiteMGfxEngine::GetInstance()->StartDraw();
#endif
#if (FULLY_RENDER != 1)
            if ((blurView_ != nullptr) || (invalidateRects_.Size() == 1)) {
                RenderManager::RenderRect(invalidRectTmp, this);
            } else {
                for (ListNode<Rect>* iter = invalidateRects_.Begin(); iter != invalidateRects_.End(); iter = iter->next_) {
                    RenderManager::RenderRect(iter->data_, this);
                }
            }
#else
            RenderManager::RenderRect(invalidRectTmp, this);
#endif
            invalidateRects_.Clear();
        }
#endif

#if ENABLE_WINDOW
        if (boundWindow_) {
            boundWindow_->Flush();
            boundWindow_->Update(invalidRectTmp);
        }
#endif
        BaseGfxEngine::GetInstance()->Flush();
#ifdef VERSION_IOT
        LiteMGfxEngine::GetInstance()->Flush(invalidRectTmp);
#if ENABLE_VGU_ENGINE
        if (hardwareView_ != nullptr) {
            hardwareView_->Flush();
        }
#endif
#endif
        return true;
    } else {
#if ENABLE_VGU_ENGINE && DOUBLE_BUFFER
        LiteMGfxEngine::GetInstance()->Flush({0, 0, 0, 0});
#endif
    }
    return false;
}

#if ENABLE_MAP_BUFFER
void RootView::BlitMapBuffer(Rect& curViewRect, TransformMap& transMap, const Rect& invalidatedArea)
{
    Rect invalidRect = curViewRect;
    transMap.SetTransMapRect(curViewRect);
    invalidRect.Join(invalidRect, transMap.GetBoxRect());

    if (invalidRect.Intersect(invalidRect, invalidatedArea)) {
        uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode);
        ImageInfo imageInfo = {0};
        imageInfo.header.colorMode = dc_.mapBufferInfo->mode;
#ifdef VERSION_IOT
        imageInfo.dataSize = ALIGN_BYTE(dc_.mapBufferInfo->width, BYTE_ALIGNMENT) *
            dc_.mapBufferInfo->height * (pxSize >> 3);
#else
        imageInfo.dataSize = dc_.mapBufferInfo->width * dc_.mapBufferInfo->height * (pxSize >> 3);
#endif
        imageInfo.header.width = dc_.mapBufferInfo->width;
        imageInfo.header.height = dc_.mapBufferInfo->height;
        imageInfo.header.reserved = 0;
        imageInfo.data = reinterpret_cast<uint8_t*>(dc_.mapBufferInfo->virAddr);
        TransformDataInfo imageTranDataInfo = {imageInfo.header, imageInfo.data, pxSize, LEVEL0, BILINEAR};
        BaseGfxEngine::GetInstance()->DrawTransform(*dc_.bufferInfo, invalidRect, {0, 0}, Color::Black(), OPA_OPAQUE,
                                                    transMap, imageTranDataInfo);
#if ENABLE_VGU_ENGINE
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
#endif
    }
}

void RootView::ClearMapBuffer()
{
    uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode);
#ifdef VERSION_IOT
    uint32_t dataSize = ALIGN_BYTE(dc_.mapBufferInfo->width, BYTE_ALIGNMENT) *
        dc_.mapBufferInfo->height * (pxSize >> 3);
#else
    uint32_t dataSize = dc_.mapBufferInfo->width * dc_.mapBufferInfo->height * (pxSize >> 3);
#endif
    if (memset_s(dc_.mapBufferInfo->virAddr, dataSize, 0, dataSize) != EOK) {
        GRAPHIC_LOGE("animator buffer memset failed.");
    }
}

void RootView::UpdateMapBufferInfo(Rect& invalidatedArea)
{
    int16_t width = invalidatedArea.GetWidth();
    int16_t height = invalidatedArea.GetHeight();
    invalidatedArea.SetWidth(height);
    invalidatedArea.SetHeight(width);
    dc_.mapBufferInfo->width = height;
    dc_.mapBufferInfo->height = width;
#ifdef VERSION_IOT
    dc_.mapBufferInfo->stride = ALIGN_BYTE(dc_.mapBufferInfo->width, BYTE_ALIGNMENT) *
        (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift right 3 bits
#else
    dc_.mapBufferInfo->stride = dc_.mapBufferInfo->width *
                                (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift 3 bits
#endif
}

void RootView::RestoreMapBufferInfo()
{
    dc_.mapBufferInfo->width = dc_.bufferInfo->width;
    dc_.mapBufferInfo->height = dc_.bufferInfo->height;
#ifdef VERSION_IOT
    dc_.mapBufferInfo->stride = ALIGN_BYTE(dc_.mapBufferInfo->width, BYTE_ALIGNMENT) *
        (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift right 3 bits
#else
    dc_.mapBufferInfo->stride = dc_.mapBufferInfo->width *
                                (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift 3 bits
#endif
}
#endif

void RootView::DrawTop(UIView* view, const Rect& rect)
{
    if (view == nullptr) {
        return;
    }

    int16_t stackCount = 0;
    UIView* par = view->GetParent();
    if (par == nullptr) {
        par = view;
    }
    UIView* curView = view;
#if ENABLE_MAP_BUFFER
    UIView* transViewGroup = nullptr;
#endif
    Rect curViewRect;
    Rect mask = rect;
    if (blurView_ != nullptr) {
        mask = GetRect();
    }
    Rect origRect;
    Rect relativeRect;
    bool enableAnimator = false;
    TransformMap curTransMap;
#if ENABLE_MAP_BUFFER
    bool updateMapBufferInfo = false;
#endif

#if ENABLE_WINDOW
    WindowImpl* boundWin = static_cast<WindowImpl*>(GetBoundWindow());
    BufferInfo* gfxDstBuffer = boundWin->GetBufferInfo();
    UpdateBufferInfo(gfxDstBuffer);
#endif

    while (par != nullptr) {
        if (curView != nullptr) {
            if (curView->IsVisible()) {
                if (curViewRect.Intersect(curView->GetMaskedRect(), mask) || enableAnimator) {
#if ENABLE_MAP_BUFFER
                    if ((curView->GetViewType() != UI_IMAGE_VIEW) && (curView->GetViewType() != UI_TEXTURE_MAPPER) &&
                        (curView->GetViewType() != UI_CANVAS_EXT) && (curView->GetViewType() != UI_MENU_ITEM) &&
                        (curView->GetViewType() != UI_MAP_VIEW) && (curView->GetViewType() != UI_LABEL_EXT) &&
                        (curView->GetViewType() != UI_LABEL) && (curView->GetViewType() != UI_TRANSFORM_GROUP) &&
                        (curView->GetViewType() != UI_IMAGE_ANIMATOR_VIEW) && (curView->GetViewType() != UI_SLIDER) &&
                        (curView->GetViewType() != UI_LOTT_VIEW) && !curView->IsTransInvalid() && !enableAnimator) {
                        origRect = curView->GetOrigRect();
                        relativeRect = curView->GetRelativeRect();
                        curView->GetTransformMap().SetInvalid(true);
                        curView->SetPosition(
                            relativeRect.GetX() - origRect.GetX() - curView->GetStyle(STYLE_MARGIN_LEFT),
                            relativeRect.GetY() - origRect.GetY() - curView->GetStyle(STYLE_MARGIN_TOP));

                        ClearMapBuffer();
                        curTransMap = curView->GetTransformMap();
                        enableAnimator = true;
                    }

                    if (enableAnimator) {
                        Rect invalidatedArea;
                        invalidatedArea.SetWidth(dc_.mapBufferInfo->width);
                        invalidatedArea.SetHeight(dc_.mapBufferInfo->height);
                        if (invalidatedArea.GetWidth() < curView->GetWidth()) {
                            UpdateMapBufferInfo(invalidatedArea);
                            updateMapBufferInfo = true;
                        }
                        if (curView == blurView_) {
                            curView->HandleGaussianBlur(*dc_.mapBufferInfo);
                        }
                        curView->OnDraw(*dc_.mapBufferInfo, invalidatedArea);
                        curViewRect = invalidatedArea;
                    } else {
                        if (curView == blurView_) {
                            curView->HandleGaussianBlur(*dc_.bufferInfo);
                        }
                        curView->OnDraw(*dc_.bufferInfo, curViewRect);
                    }
#else
                    if (curView == blurView_) {
                        curView->HandleGaussianBlur(*dc_.bufferInfo, isBlurCacheNeedRefresh_);
                    }
                    curView->OnDraw(*dc_.bufferInfo, curViewRect);
#endif

                    if ((curView->IsViewGroup()) && (stackCount < COMPONENT_NESTING_DEPTH)) {
#if ENABLE_MAP_BUFFER
                        if (enableAnimator && (transViewGroup == nullptr)) {
                            transViewGroup = curView;
                        }
#endif
                        par = curView;
                        g_viewStack[stackCount] = curView;
                        g_maskStack[stackCount] = mask;
                        stackCount++;
                        curView = static_cast<UIViewGroup*>(curView)->GetChildrenHead();
                        mask = par->GetContentRect();
                        mask.Intersect(mask, curViewRect);
                        continue;
                    }

#if ENABLE_MAP_BUFFER
                    if (enableAnimator) {
                        curView->OnPostDraw(*dc_.mapBufferInfo, curViewRect);
                    } else {
                        curView->OnPostDraw(*dc_.bufferInfo, curViewRect);
                    }

                    if (enableAnimator && (transViewGroup == nullptr)) {
                        BlitMapBuffer(origRect, curTransMap, mask);
                        if (updateMapBufferInfo) {
                            RestoreMapBufferInfo();
                            updateMapBufferInfo = false;
                        }
                        curView->GetTransformMap().SetInvalid(false);
                        enableAnimator = false;
                        curView->SetPosition(relativeRect.GetX() - curView->GetStyle(STYLE_MARGIN_LEFT),
                                             relativeRect.GetY() - curView->GetStyle(STYLE_MARGIN_TOP));
                    }
#else
                    curView->OnPostDraw(*dc_.bufferInfo, curViewRect);
#endif
                }
            }
            curView = curView->GetNextSibling();
            continue;
        }
        if (--stackCount >= 0) {
            curViewRect = par->GetMaskedRect();
            mask = g_maskStack[stackCount];
#if ENABLE_MAP_BUFFER
            if (enableAnimator) {
                par->OnPostDraw(*dc_.mapBufferInfo, curViewRect);
            } else if (curViewRect.Intersect(curViewRect, mask)) {
                par->OnPostDraw(*dc_.bufferInfo, curViewRect);
            }

            if (enableAnimator && transViewGroup == g_viewStack[stackCount]) {
                BlitMapBuffer(origRect, curTransMap, mask);
                if (updateMapBufferInfo) {
                    RestoreMapBufferInfo();
                    updateMapBufferInfo = false;
                }
                transViewGroup->GetTransformMap().SetInvalid(false);
                enableAnimator = false;
                transViewGroup->SetPosition(relativeRect.GetX() - transViewGroup->GetStyle(STYLE_MARGIN_LEFT),
                                            relativeRect.GetY() - transViewGroup->GetStyle(STYLE_MARGIN_TOP));
                transViewGroup = nullptr;
            }
#else
            if (curViewRect.Intersect(curViewRect, mask)) {
                par->OnPostDraw(*dc_.bufferInfo, curViewRect);
            }
#endif
            curView = g_viewStack[stackCount]->GetNextSibling();
            par = par->GetParent();
            continue;
        }
        stackCount = 0;
        curView = par->GetNextSibling();
#if ENABLE_MAP_BUFFER
        if (enableAnimator) {
            par->OnPostDraw(*dc_.mapBufferInfo, curViewRect);
        } else {
            par->OnPostDraw(*dc_.bufferInfo, curViewRect);
        }
#else
        par->OnPostDraw(*dc_.bufferInfo, curViewRect);
#endif
        par = par->GetParent();
    }
}

UIView* RootView::GetTopUIView(const Rect& rect)
{
    int16_t stackCount = 0;
    UIView* currentView = this;
    UIView* topView = currentView;
    Rect copyRect(rect);
    while (stackCount >= 0) {
        while (currentView != nullptr) {
            if (currentView->GetOrigRect().IsContains(rect) && currentView->IsVisible()) {
                if (currentView->GetStyle(STYLE_BACKGROUND_OPA) == OPA_OPAQUE && currentView->OnPreDraw(copyRect) &&
                    currentView->GetOpaScale() == OPA_OPAQUE) {
                    topView = currentView;
                }
                if (currentView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = currentView;
                    currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenHead();
                    continue;
                }
            }
            currentView = currentView->GetNextSibling();
        }
        if (--stackCount >= 0) {
            currentView = (g_viewStack[stackCount])->GetNextSibling();
        }
    }
    UIView* parentView = topView;
    while (parentView->GetParent() != nullptr) {
        UIView* tempView = parentView;
        parentView = parentView->GetParent();
        if (!tempView->IsTransInvalid()) {
            topView = parentView;
        }
    }
    return topView;
}

bool RootView::FindSubView(const UIView& parentView, const UIView* subView)
{
    const UIView* root = &parentView;
    if (root == subView) {
        return true;
    } else if (!root->IsViewGroup() || (subView == nullptr)) {
        return false;
    }

    UIView* currentView = static_cast<const UIViewGroup*>(root)->GetChildrenHead();
    const UIView* parent = root;
    int8_t deep = 1;
    while (deep > 0) {
        if (currentView == subView) {
            return true;
        }
        if (currentView == nullptr) {
            currentView = parent->GetNextSibling();
            parent = parent->GetParent();
            deep--;
        } else if (currentView->IsViewGroup()) {
            parent = currentView;
            currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenHead();
            deep++;
        } else {
            currentView = currentView->GetNextSibling();
        }
    }
    return false;
}

#if ENABLE_MAP_BUFFER
void RootView::InitMapBufferInfo(BufferInfo* bufferInfo)
{
    dc_.mapBufferInfo = new BufferInfo();
    if (dc_.mapBufferInfo == nullptr) {
        return;
    }

    if (memcpy_s(dc_.mapBufferInfo, sizeof(BufferInfo), bufferInfo, sizeof(BufferInfo)) != EOK) {
        delete dc_.mapBufferInfo;
        dc_.mapBufferInfo = nullptr;
        return;
    }

#ifdef VERSION_IOT
    dc_.mapBufferInfo->stride = ALIGN_BYTE(dc_.mapBufferInfo->width, BYTE_ALIGNMENT) *
        (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift right 3 bits
#else
    dc_.mapBufferInfo->stride = dc_.mapBufferInfo->width *
        (DrawUtils::GetPxSizeByColorMode(dc_.mapBufferInfo->mode) >> 3); // 3: Shift right 3 bits
#endif
    uint32_t bufferSize = dc_.mapBufferInfo->stride * dc_.mapBufferInfo->height;
    dc_.mapBufferInfo->virAddr = dc_.mapBufferInfo->phyAddr =
        BaseGfxEngine::GetInstance()->AllocBuffer(bufferSize, BUFFER_MAP_SURFACE);
}

void RootView::DestroyMapBufferInfo()
{
    if (dc_.mapBufferInfo != nullptr) {
        BaseGfxEngine::GetInstance()->FreeBuffer(static_cast<uint8_t*>(dc_.mapBufferInfo->virAddr));
        dc_.mapBufferInfo->virAddr = dc_.mapBufferInfo->phyAddr = nullptr;
        delete dc_.mapBufferInfo;
        dc_.mapBufferInfo = nullptr;
    }
}
#endif

void RootView::InitDrawContext()
{
    dc_.bufferInfo = BaseGfxEngine::GetInstance()->GetFBBufferInfo();
#if ENABLE_MAP_BUFFER
    if (dc_.bufferInfo != nullptr) {
        InitMapBufferInfo(dc_.bufferInfo);
    }
    bakDc_.mapBufferInfo = nullptr;
#endif
    bakDc_.bufferInfo = nullptr;
}

void RootView::DestroyDrawContext()
{
#if ENABLE_MAP_BUFFER
    DestroyMapBufferInfo();
#endif
}

void RootView::UpdateBufferInfo(BufferInfo* fbBufferInfo)
{
#if ENABLE_MAP_BUFFER
    if (dc_.bufferInfo == nullptr) {
        dc_.bufferInfo = fbBufferInfo;
        InitMapBufferInfo(fbBufferInfo);
    } else {
        if (dc_.bufferInfo->width != fbBufferInfo->width || dc_.bufferInfo->height != fbBufferInfo->height ||
            dc_.bufferInfo->mode != fbBufferInfo->mode) {
            DestroyMapBufferInfo();
            InitMapBufferInfo(fbBufferInfo);
        }
        dc_.bufferInfo = fbBufferInfo;
    }
#else
    dc_.bufferInfo = fbBufferInfo;
#endif
}

void RootView::SaveDrawContext()
{
    bakDc_.bufferInfo = dc_.bufferInfo;
    dc_.bufferInfo = nullptr;
#if ENABLE_MAP_BUFFER
    bakDc_.mapBufferInfo = dc_.mapBufferInfo;
    dc_.mapBufferInfo = nullptr;
#endif
}

void RootView::RestoreDrawContext()
{
    DestroyDrawContext();

    dc_.bufferInfo = bakDc_.bufferInfo;
    bakDc_.bufferInfo = nullptr;
#if ENABLE_MAP_BUFFER
    dc_.mapBufferInfo = bakDc_.mapBufferInfo;
    bakDc_.mapBufferInfo = nullptr;
#endif
}
} // namespace OHOS
