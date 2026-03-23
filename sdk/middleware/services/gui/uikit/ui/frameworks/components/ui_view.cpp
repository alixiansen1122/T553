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

#include "components/ui_view.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "core/render_manager.h"
#include "dfx/ui_view_bounds.h"
#include "dock/focus_manager.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "imgdecode/image_load.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "hals/display_dev.h"
#include "securec.h"
#include "themes/theme_manager.h"
#include "hals/gralloc_engines.h"
#include <cstdio>

namespace OHOS {
UIView::UIView()
    : touchable_(false),
      visible_(true),
      draggable_(false),
      dragParentInstead_(true),
      isViewGroup_(false),
      needRedraw_(false),
      styleAllocFlag_(false),
      isIntercept_(false),
#if ENABLE_FOCUS_MANAGER
      focusable_(false),
#endif
      fullScreenBlur_(false),
      opaScale_(OPA_OPAQUE),
      index_(0),
      pPrevWidth_(0),
      pPrevHeight_(0),
      id_(nullptr),
      parent_(nullptr),
      nextSibling_(nullptr),
      alignType_(ALIGN_DEFAULT),
      style_(nullptr),
      transMap_(nullptr),
      onClickListener_(nullptr),
      onLongPressListener_(nullptr),
      onDragListener_(nullptr),
      onTouchListener_(nullptr),
#if ENABLE_FOCUS_MANAGER
      onFocusListener_(nullptr),
#endif
#if ENABLE_ROTATE_INPUT
      onRotateListener_(nullptr),
#endif
      viewExtraMsg_(nullptr),
      rect_(0, 0, -1, -1),
      visibleRect_(nullptr),
      blurRadius_(0.0f)
{
    SetupThemeStyles();
}

UIView::~UIView()
{
#if ENABLE_ROTATE_INPUT
    ClearFocus();
#endif
    ClearBackgroundBlur();
    if (parent_ != nullptr) {
        GRAPHIC_LOGE("UIView::~UIView failed, parent is not nullptr! Need to remove from parent component first");
    }
    if (transMap_ != nullptr) {
        delete transMap_;
        transMap_ = nullptr;
    }
    if (visibleRect_ != nullptr) {
        delete visibleRect_;
        visibleRect_ = nullptr;
    }
    if (styleAllocFlag_) {
        delete style_;
        style_ = nullptr;
        styleAllocFlag_ = false;
    }
    if (blurCache_ != nullptr) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t *>(blurCache_->virAddr));
        UIFree(blurCache_);
        blurCache_ = nullptr;
    }
}

Rect UIView::GetInnerRect(const Rect &rect, int16_t topRadius, int16_t bottomRadius) const
{
    int16_t minR = MATH_MIN(rect.GetHeight(), rect.GetWidth()) / 2;  // 2: half
    topRadius = MATH_MIN(topRadius, minR);
    bottomRadius = MATH_MIN(bottomRadius, minR);
    topRadius = (topRadius + 1) / 2;        // 2: half
    bottomRadius = (bottomRadius + 1) / 2;  // 2: half

    int16_t x = rect.GetX();
    int16_t y = rect.GetY();
    Rect innerRect;
    if (topRadius >= bottomRadius) {
        innerRect.SetLeft(x + topRadius);
        innerRect.SetTop(y + topRadius);
        innerRect.SetWidth(rect.GetWidth() - 2 * topRadius);  // 2: half
    } else {
        innerRect.SetLeft(x + bottomRadius);
        innerRect.SetTop(y + topRadius);
        innerRect.SetWidth(rect.GetWidth() - 2 * bottomRadius);  // 2: half
    }
    innerRect.SetHeight(rect.GetHeight() - topRadius - bottomRadius);

    return innerRect;
}

bool UIView::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t topRadius = style_->GetStyle(STYLE_TOP_BORDER_RADIUS);
    int16_t bottomRadius = style_->GetStyle(STYLE_BOTTOM_BORDER_RADIUS);
    if (topRadius == bottomRadius && topRadius == COORD_MAX) {
        return true;
    }
    if (topRadius != 0 || bottomRadius != 0) {
        rect = GetInnerRect(rect, topRadius, bottomRadius);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}

void UIView::SaveBlurCache(const BufferInfo& gfxDstBuffer, const Rect &mask)
{
    BufferInfo srcInfo = gfxDstBuffer;
    srcInfo.rect = mask;
    if (blurCache_ != nullptr) {
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t *>(blurCache_->virAddr));
        UIFree(blurCache_);
        blurCache_ = nullptr;
    }
    blurCache_ = static_cast<BufferInfo *>(UIMalloc(sizeof(BufferInfo)));
    if (blurCache_ == nullptr) {
        return;
    }

    ImageInfo info;
    if (!Screen::GetInstance().GetBitMapFromBuffer(gfxDstBuffer, info, &mask)) {
        UIFree(blurCache_);
        blurCache_ = nullptr;
        return;
    }
    blurCache_->rect = {0, 0, mask.GetWidth() - 1, mask.GetHeight() - 1};
    blurCache_->stride = info.dataSize / info.header.height;
    blurCache_->virAddr = static_cast<void*>(const_cast<uint8_t*>(info.data));
    blurCache_->phyAddr = static_cast<void*>(const_cast<uint8_t*>(info.data));
    blurCache_->width = info.header.width;
    blurCache_->height = info.header.height;
    blurCache_->mode = static_cast<ColorMode>(info.header.colorMode);
    blurCache_->color = 0;
#ifdef VERSION_IOT
    blurCache_->compressMode = info.header.compressMode;
#endif
}

void UIView::DrawBlurCache(BufferInfo& gfxDstBuffer, const Rect &mask)
{
    BlendOption opt;
    opt.mode = BLEND_SRC_OVER;
    opt.opacity = OPA_OPAQUE;
    opt.transMap.SetTransMapRect(blurCache_->rect);
    opt.transMap.Translate(Vector2<int16_t>{mask.GetX(), mask.GetY()});
    BufferInfo src = *blurCache_;
    auto minFunc = [](int16_t a, int16_t b) -> int16_t {
        return a < b ? a : b;
    };
    src.rect.SetWidth(minFunc(blurCache_->rect.GetWidth(), mask.GetWidth()));
    src.rect.SetHeight(minFunc(mask.GetHeight(), blurCache_->rect.GetHeight()));
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, {0, 0}, src, mask, opt);
}

void UIView::HandleGaussianBlur(BufferInfo& gfxDstBuffer, bool isNeedRefresh)
{
    if (visible_ && (!FloatEqual(blurRadius_, 0.0f))) {
        BufferInfo gfxTargetBuffer = gfxDstBuffer;
        Rect mask = {0, 0, gfxTargetBuffer.width - 1, gfxTargetBuffer.height - 1};
        if (!fullScreenBlur_) {
            Rect rect = GetRect();
            mask.Intersect(rect, mask);
            if (!mask.IsValid()) {
                return;
            }
        }
        gfxTargetBuffer.rect = mask;
#if ENABLE_VGU_ENGINE
        if (!isNeedRefresh && blurCache_ != nullptr && !IsFullScreenBlur() &&
            lastBlurMask_ == mask && lastBlurRadius_ == blurRadius_) {
            DrawBlurCache(gfxDstBuffer, mask);
            return;
        }
        lastBlurMask_ = mask;
        lastBlurRadius_ = blurRadius_;
#endif
        BaseGfxEngine::GetInstance()->Blur(gfxTargetBuffer, blurRadius_);
#if ENABLE_VGU_ENGINE
        if (isGaussCacheEnabled_ && !IsFullScreenBlur()) {
            SaveBlurCache(gfxDstBuffer, mask);
        }
#endif
    }
}

#if ENABLE_VGU_ENGINE
void UIView::EnableGaussianBlurCache(bool enable)
{
    isGaussCacheEnabled_ = enable;
    if (!isGaussCacheEnabled_) {
        if (blurCache_ != nullptr) {
            LiteMGfxEngine::GetInstance()->SyncHwDraw();
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t *>(blurCache_->virAddr));
            UIFree(blurCache_);
            blurCache_ = nullptr;
        }
    }
    Invalidate();
}

bool UIView::IsGaussianBlurCacheEnable() const
{
    return isGaussCacheEnabled_;
}
#endif

void UIView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    uint8_t opa = GetMixOpaScale();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea, *style_, opa);
}

void UIView::OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    DrawViewBounds(gfxDstBuffer, invalidatedArea);
}

void UIView::DrawViewBounds(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
#if ENABLE_DEBUG
    if (!UIViewBounds::GetInstance()->GetShowState()) {
        return;
    }
    Style* style = new Style();
    style->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    style->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    style->SetStyle(STYLE_BORDER_WIDTH, 1);
    style->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE / 2); // 2: half opacity
    Rect viewRect(GetRect());
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, viewRect, invalidatedArea, *style, OPA_OPAQUE);

    style->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    style->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    style->SetStyle(STYLE_BORDER_WIDTH, 0);
    Rect tmpRect(viewRect);
    int16_t length = 10; // 10: corner length

    // left top corner
    tmpRect.SetRight(viewRect.GetLeft() + length);
    tmpRect.SetBottom(viewRect.GetTop());
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetRight(viewRect.GetLeft());
    tmpRect.SetBottom(viewRect.GetTop() + length);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // left bottom corner
    tmpRect.SetLeft(viewRect.GetLeft());
    tmpRect.SetTop(viewRect.GetBottom() - length);
    tmpRect.SetRight(viewRect.GetLeft());
    tmpRect.SetBottom(viewRect.GetBottom());
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetTop(viewRect.GetBottom());
    tmpRect.SetRight(viewRect.GetLeft() + length);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // right top corner
    tmpRect.SetLeft(viewRect.GetRight() - length);
    tmpRect.SetTop(viewRect.GetTop());
    tmpRect.SetRight(viewRect.GetRight());
    tmpRect.SetBottom(viewRect.GetTop());
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetLeft(viewRect.GetRight());
    tmpRect.SetBottom(viewRect.GetTop() + length);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);

    // right bottom corner
    tmpRect = viewRect;
    tmpRect.SetLeft(viewRect.GetRight());
    tmpRect.SetTop(viewRect.GetBottom() - length);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    tmpRect.SetLeft(viewRect.GetRight() - length);
    tmpRect.SetTop(viewRect.GetBottom());
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, tmpRect, invalidatedArea, *style, OPA_OPAQUE);
    delete style;
#endif // ENABLE_DEBUG
}

void UIView::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    if (theme != nullptr) {
        style_ = &(theme->GetMainStyle());
    } else {
        style_ = &(StyleDefault::GetDefaultStyle());
    }
}

void UIView::SetStyle(Style& style)
{
    if (styleAllocFlag_) {
        delete style_;
        styleAllocFlag_ = false;
    }
    style_ = &style;
}

void UIView::SetStyle(uint8_t key, int64_t value)
{
    if (!styleAllocFlag_) {
        style_ = new Style(*style_);
        if (style_ == nullptr) {
            GRAPHIC_LOGE("new Style fail");
            return;
        }
        styleAllocFlag_ = true;
    }
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    int16_t x = GetX();
    int16_t y = GetY();
    style_->SetStyle(key, value);
    Rect rect(x, y, x + width - 1, y + height -  1);
    UpdateRectInfo(key, rect);
}

void UIView::UpdateRectInfo(uint8_t key, const Rect& rect)
{
    switch (key) {
        case STYLE_BORDER_WIDTH: {
            SetWidth(rect.GetWidth());
            SetHeight(rect.GetHeight());
            break;
        }
        case STYLE_PADDING_LEFT:
        case STYLE_PADDING_RIGHT: {
            SetWidth(rect.GetWidth());
            break;
        }
        case STYLE_PADDING_TOP:
        case STYLE_PADDING_BOTTOM: {
            SetHeight(rect.GetHeight());
            break;
        }
        case STYLE_MARGIN_LEFT: {
            SetX(rect.GetX());
            break;
        }
        case STYLE_MARGIN_TOP: {
            SetY(rect.GetY());
            break;
        }
        default:
            break;
    }
}
#if ENABLE_SWEEP_CLOCK
void UIView::Rotate(float angle, const Vector2<float>& pivot)
#else
void UIView::Rotate(int16_t angle, const Vector2<float>& pivot)
#endif
{
    Vector3<float> pivotStart3D = Vector3<float>(pivot.x_, pivot.y_, 0);
    Vector3<float> pivotEnd3D = Vector3<float>(pivot.x_, pivot.y_, 1.0f);
    Rotate(angle, pivotStart3D, pivotEnd3D);
}

#if ENABLE_SWEEP_CLOCK
void UIView::Rotate(float angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd)
#else
void UIView::Rotate(int16_t angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd)
#endif
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Rotate(angle, pivotStart, pivotEnd);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Scale(const Vector2<float>& scale, const Vector2<float>& pivot)
{
    Vector3<float> scale3D = Vector3<float>(scale.x_, scale.y_, 1.0f);
    Vector3<float> pivot3D = Vector3<float>(pivot.x_, pivot.y_, 0);
    Scale(scale3D, pivot3D);
}

void UIView::Scale(const Vector3<float>& scale, const Vector3<float>& pivot)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
    joinRect.SetX(joinRect.GetX() - 1);
    joinRect.SetY(joinRect.GetY() - 1);
    joinRect.SetWidth(joinRect.GetWidth() + 5); // 5: align size
    joinRect.SetHeight(joinRect.GetHeight() + 5); // 5: align size
#endif
#endif
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Scale(scale, pivot);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Shear(const Vector2<float>& shearX, const Vector2<float>& shearY, const Vector2<float>& shearZ)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Shear(shearX, shearY, shearZ);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

void UIView::Translate(const Vector2<int16_t>& trans)
{
    Vector3<int16_t> trans3D = Vector3<int16_t>(trans.x_, trans.y_, 0);
    Translate(trans3D);
}

void UIView::Translate(const Vector3<int16_t>& trans)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    bool firstTrans = transMap_->IsInvalid();
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->Translate(trans);
    if (firstTrans) {
        joinRect = transMap_->GetBoxRect();
        joinRect.Join(joinRect, GetOrigRect());
    } else {
        joinRect.Join(joinRect, transMap_->GetBoxRect());
    }
    InvalidateRect(joinRect);
}

bool UIView::IsTransInvalid()
{
    if (transMap_ == nullptr) {
        return true;
    }
    return transMap_->IsInvalid();
}

void UIView::SetCameraDistance(int16_t distance)
{
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->SetCameraDistance(distance);
    joinRect.Join(joinRect, transMap_->GetBoxRect());
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::SetCameraPosition(const Vector2<float>& position)
{
    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
        ReMeasure();
    }
    Rect joinRect = transMap_->GetBoxRect();
    transMap_->SetTransMapRect(GetOrigRect());
    transMap_->SetCameraPosition(position);
    joinRect.Join(joinRect, transMap_->GetBoxRect());
    joinRect.Join(joinRect, GetOrigRect());
    InvalidateRect(joinRect);
}

void UIView::ResetTransParameter()
{
    if (transMap_ != nullptr) {
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
        Rect joinRect = transMap_->GetBoxRect();
        joinRect.SetX(joinRect.GetX() - 1);
        joinRect.SetY(joinRect.GetY() - 1);
        joinRect.SetWidth(joinRect.GetWidth() + 5); // 5: align size
        joinRect.SetHeight(joinRect.GetHeight() + 5); // 5: align size
        delete transMap_;
        transMap_ = nullptr;
        joinRect.Join(joinRect, GetOrigRect());
        InvalidateRect(joinRect);
        return;
#endif
#endif
        delete transMap_;
        transMap_ = nullptr;
        Invalidate();
    }
}

void UIView::SetBackgroundBlur(float radius, uint32_t quality)
{
    if (FloatEqual(radius, 0.0f)) {
        RootView::GetInstance()->ClearBlurView(this);
        blurRadius_ = 0.0f;
    } else if (RootView::GetInstance()->SetBlurView(this)) {
        blurRadius_ = radius;
    }
    RootView::GetInstance()->Invalidate();
}

void UIView::ClearBackgroundBlur()
{
    if (!FloatEqual(blurRadius_, 0.0f)) {
        RootView::GetInstance()->ClearBlurView(this);
        blurRadius_ = 0.0f;
        RootView::GetInstance()->Invalidate();
    }
}

#if ENABLE_ROTATE_INPUT
void UIView::RequestFocus()
{
    FocusManager::GetInstance()->RequestFocus(this);
}

void UIView::ClearFocus()
{
    if (FocusManager::GetInstance()->GetFocusedView() == this) {
        FocusManager::GetInstance()->ClearFocus();
    }
}
#endif

void UIView::Invalidate()
{
    InvalidateRect(GetRect());
}

void UIView::InvalidateRect(const Rect& invalidatedArea)
{
    if (!visible_) {
        if (needRedraw_) {
            needRedraw_ = false;
        } else {
            return;
        }
    }

    Rect trunc(invalidatedArea);
    bool isIntersect = true;
    UIView* par = parent_;
    UIView* cur = this;

    while (par != nullptr) {
        if (!par->visible_) {
            return;
        }

        isIntersect = trunc.Intersect(par->GetContentRect(), trunc);
        if (!isIntersect) {
            break;
        }

        cur = par;
        par = par->parent_;
    }

    if (isIntersect && (cur->GetViewType() == UI_ROOT_VIEW)) {
        RootView* rootView = reinterpret_cast<RootView*>(cur);
        if (!visible_) {
#if LOCAL_RENDER
            rootView->RemoveViewFromInvalidMap(this);
#endif
            rootView->AddInvalidateRectWithLock(trunc, parent_);
        } else {
            rootView->AddInvalidateRectWithLock(trunc, this);
        }
    }
}

bool UIView::OnLongPressEvent(const LongPressEvent& event)
{
    if (onLongPressListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onLongPressListener_->OnLongPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragStartEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragStart(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDrag(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnDragEndEvent(const DragEvent& event)
{
    if (onDragListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onDragListener_->OnDragEnd(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnClickEvent(const ClickEvent& event)
{
    if (onClickListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onClickListener_->OnClick(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnPressEvent(const PressEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnPress(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnReleaseEvent(const ReleaseEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnRelease(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

bool UIView::OnCancelEvent(const CancelEvent& event)
{
    if (onTouchListener_ != nullptr) {
        /* To ensure version compatibility, the listeners of both versions are invoked. */
        bool isConsumed = onTouchListener_->OnCancel(*this, event);
        return isConsumed;
    }
    return isIntercept_;
}

#if ENABLE_ROTATE_INPUT
bool UIView::OnRotateStartEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotateStart(*this, event);
    }
    return false;
}

bool UIView::OnRotateEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotate(*this, event);
    }
    return isIntercept_;
}

bool UIView::OnRotateEndEvent(const RotateEvent& event)
{
    if (onRotateListener_ != nullptr) {
        return onRotateListener_->OnRotateEnd(*this, event);
    }
    return false;
}
#endif

void UIView::GetTargetView(const Point& point, UIView** last)
{
    if (last == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetContentRect(), rect);
    }

    if (visible_ && touchable_ && rect.IsContains(point)) {
        *last = this;
    }
}

void UIView::GetTargetView(const Point& point, UIView** current, UIView** target)
{
    if (current == nullptr) {
        return;
    }
    UIView* par = parent_;
    Rect rect = GetRect();

    if (par != nullptr) {
        rect.Intersect(par->GetContentRect(), rect);
    }

    if (visible_ && rect.IsContains(point)) {
        if (touchable_) {
            *current = this;
        }
        *target = this;
    }
}

#if ENABLE_FOCUS_MANAGER
void UIView::Focus()
{
    if (focusable_ && onFocusListener_ != nullptr) {
        onFocusListener_->OnFocus(*this);
    }
}

void UIView::Blur()
{
    if (onFocusListener_ != nullptr) {
        onFocusListener_->OnBlur(*this);
    }
}
#endif

Rect UIView::GetRect() const
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }
    return GetOrigRect();
}

Rect UIView::GetContentRect()
{
    if ((transMap_ != nullptr) && !transMap_->IsInvalid()) {
        Rect r = transMap_->GetBoxRect();
        Rect origRect = GetOrigRect();
        r.SetX(r.GetX() + origRect.GetX() - transMap_->GetTransMapRect().GetX());
        r.SetY(r.GetY() + origRect.GetY() - transMap_->GetTransMapRect().GetY());
        return r;
    }

    Rect contentRect = GetRect();
    contentRect.SetX(contentRect.GetX() + style_->paddingLeft_ + style_->borderWidth_);
    contentRect.SetY(contentRect.GetY() + style_->paddingTop_ + style_->borderWidth_);
    contentRect.SetWidth(GetWidth());
    contentRect.SetHeight(GetHeight());
    return contentRect;
}

Rect UIView::GetOrigRect() const
{
    int16_t x = rect_.GetX();
    int16_t y = rect_.GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetRelativeRect().GetX() + par->GetStyle(STYLE_PADDING_LEFT) + par->GetStyle(STYLE_BORDER_WIDTH);
        y += par->GetRelativeRect().GetY() + par->GetStyle(STYLE_PADDING_TOP) + par->GetStyle(STYLE_BORDER_WIDTH);
        par = par->parent_;
    }
    return Rect(x, y, x + rect_.GetWidth() - 1, y + rect_.GetHeight() - 1);
}

Rect UIView::GetMaskedRect() const
{
    Rect mask;
    if (visibleRect_ != nullptr) {
        mask.Intersect(GetRect(), GetVisibleRect());
    } else {
        mask = GetRect();
    }
    return mask;
}

Rect UIView::GetVisibleRect() const
{
    if (visibleRect_ == nullptr) {
        return GetRect();
    }
    Rect absoluteRect;
    int16_t x = visibleRect_->GetX();
    int16_t y = visibleRect_->GetY();
    UIView* par = parent_;
    while (par != nullptr) {
        x += par->GetX();
        y += par->GetY();
        par = par->parent_;
    }
    absoluteRect.SetX(x);
    absoluteRect.SetY(y);
    absoluteRect.SetWidth(visibleRect_->GetWidth());
    absoluteRect.SetHeight(visibleRect_->GetHeight());
    return absoluteRect;
}

void UIView::SetTransformMap(const TransformMap& transMap)
{
#if !ENABLE_MAP_BUFFER
    if (!CheckIsNeedMapBufferToTrans(__PRETTY_FUNCTION__)) {
        return;
    }
#endif
    if ((transMap_ != nullptr) && (*transMap_ == transMap)) {
        return;
    }

    if (transMap_ == nullptr) {
        transMap_ = new TransformMap();
    }
    Rect preRect = GetRect();
#if HW_RECT_ALIGN_RESTRICTION
#if ENABLE_VGU_ENGINE
    preRect.SetX(preRect.GetX() - 1);
    preRect.SetY(preRect.GetY() - 1);
    preRect.SetWidth(preRect.GetWidth() + 5); // 5: align size
    preRect.SetHeight(preRect.GetHeight() + 5); // 5: align size
#endif
#endif

    *transMap_ = transMap;
    transMap_->SetTransMapRect(GetOrigRect());

    Rect joinRect;
    joinRect.Join(preRect, transMap_->GetBoxRect());
    InvalidateRect(joinRect);
}

void UIView::SetWidthPercent(float widthPercent)
{
    if (IsInvalid(widthPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        SetWidth(newWidth);
    }
}

void UIView::SetHeightPercent(float heightPercent)
{
    if (IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetHeight(newHeight);
    }
}

void UIView::ResizePercent(float widthPercent, float heightPercent)
{
    if (IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        Resize(newWidth, newHeight);
    }
}

void UIView::SetXPercent(float xPercent)
{
    if (IsInvalid(xPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        SetX(newX);
    }
}

void UIView::SetYPercent(float yPercent)
{
    if (IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetHeight() > 1)) {
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetY(newY);
    }
}

void UIView::SetPositionPercent(float xPercent, float yPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        SetPosition(newX, newY);
    }
}

void UIView::SetPositionPercent(float xPercent, float yPercent, float widthPercent, float heightPercent)
{
    if (IsInvalid(xPercent) || IsInvalid(yPercent) || IsInvalid(widthPercent) || IsInvalid(heightPercent)) {
        return;
    }
    if ((GetParent() != nullptr) && (GetParent()->GetWidth() > 1) && (GetParent()->GetHeight() > 1)) {
        int16_t newX = static_cast<int16_t>(GetParent()->GetWidth() * xPercent);
        int16_t newY = static_cast<int16_t>(GetParent()->GetHeight() * yPercent);
        int16_t newWidth = static_cast<int16_t>(GetParent()->GetWidth() * widthPercent);
        int16_t newHeight = static_cast<int16_t>(GetParent()->GetHeight() * heightPercent);
        SetPosition(newX, newY, newWidth, newHeight);
    }
}

bool UIView::IsInvalid(float percent)
{
    if ((percent < 1) && (percent > 0)) {
        return false;
    }
    return true;
}

void UIView::LayoutCenterOfParent(int16_t xOffset, int16_t yOffset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    int16_t leftMargin = style_->marginLeft_;
    int16_t rightMargin = style_->marginRight_;
    int16_t bottomMargin = style_->marginBottom_;
    // 2: half
    int16_t posX = parent_->GetWidth() / 2 - (rect_.GetWidth() - leftMargin + rightMargin) / 2 + xOffset;
    // 2: half
    int16_t posY = parent_->GetHeight() / 2 - (rect_.GetHeight() - topMargin + bottomMargin) / 2 + yOffset;
    SetPosition(posX, posY);
}

void UIView::LayoutLeftOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t leftMargin = style_->marginLeft_;
    SetPosition(leftMargin + offset, GetY());
}

void UIView::LayoutRightOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t rightMargin = style_->marginRight_;
    SetPosition(parent_->GetWidth() - offset - rect_.GetWidth() - rightMargin, GetY());
}

void UIView::LayoutTopOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t topMargin = style_->marginTop_;
    SetPosition(GetX(), topMargin + offset);
}

void UIView::LayoutBottomOfParent(int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }

    int16_t bottomMargin = style_->marginBottom_;
    SetPosition(GetX(), parent_->GetHeight() - offset - rect_.GetHeight() - bottomMargin);
}

void UIView::AlignLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ - style_->marginLeft_;
        SetPosition(sib->GetX() - margin + offset, GetY());
    }
}

void UIView::AlignRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ - style_->marginRight_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() - rect_.GetWidth() - offset + margin, GetY());
    }
}

void UIView::AlignTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ - style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + offset - margin);
    }
}

void UIView::AlignBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ - style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() - rect_.GetHeight() - offset + margin);
    }
}

void UIView::AlignHorCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginRight_ - sib->style_->marginLeft_ - style_->marginRight_ + style_->marginLeft_) /
            2; // 2 : half
        SetPosition(sib->GetX() + sib->rect_.GetWidth() / 2 - rect_.GetWidth() / 2 + margin + offset, GetY());
    }
}
void UIView::AlignVerCenterToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin =
            (sib->style_->marginBottom_ - sib->style_->marginTop_ - style_->marginBottom_ + style_->marginTop_) /
            2; // 2 : half
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() / 2 - rect_.GetHeight() / 2 + margin + offset);
    }
}

void UIView::UpdatePos()
{
    // 如果没有设置过对齐关系, 或者父容器不存在，不处理
    if (alignType_ == ALIGN_DEFAULT || parent_ == nullptr) {
        return;
    }

    int16_t rx = parent_->GetX();
    int16_t ry = parent_->GetY();
    int16_t rw = parent_->GetWidth();
    int16_t rh = parent_->GetHeight();

    // 如果父容器尺寸没有发生变化，不需要处理
    if (IsParentSizeStabled(rw, rh)) {
        return;
    }

    int16_t x = 0;
    int16_t y = 0;
    int16_t w = GetWidth();
    int16_t h = GetHeight();

    // 提取对齐位置 (高低各占4位)
    uint8_t alignHigh = (alignType_ & ALIGN_UP_MASK) >> 4;
    uint8_t alignLow = (alignType_ & ALIGN_DOWUN_MASK);

    x = alignLow * (rw - w) / 2;
    y = alignHigh * (rh - h) / 2;
    
    // 计算偏移量
    x += ofsX_;
    y += ofsY_;
    SetPosition(x, y);
}

void UIView::AlignToReference(AlignmentType alignType, UIView* referView, int16_t ofsX, int16_t ofsY)
{
    if (referView == nullptr && parent_ == nullptr) {
        return;
    }

    // 实际使用的参考控件
    UIView* actualReferView = (referView != nullptr) ? referView : parent_;

    if (actualReferView == parent_) {
        if (alignType >= ALIGN_OUT_TOP_LEFT) {
            GRAPHIC_LOGE("UIView::AlignToReference, alignment type set invalid");
            return;
        } else {
            alignType_ = alignType;
            pPrevWidth_ = parent_->GetWidth();
            pPrevHeight_ = parent_->GetHeight();
        }
    }

    // 获取参考控件的几何信息
    int16_t rx = actualReferView->GetX();
    int16_t ry = actualReferView->GetY();
    int16_t rw = actualReferView->GetWidth();
    int16_t rh = actualReferView->GetHeight();

    // 获取待对齐控件的几何信息
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = GetWidth();
    int16_t h = GetHeight();

    bool isOuter = (alignType & ALIGN_OUTER_MASK) != 0;
    
    // 提取对齐位置 (高低各占4位)
    uint8_t alignHigh = (alignType & ALIGN_UP_MASK) >> 4;
    uint8_t alignLow = (alignType & ALIGN_DOWUN_MASK);

    // 基础对齐计算
    if (!isOuter) {
        x = alignLow * (rw - w) / 2;
        y = alignHigh * (rh - h) / 2;
    } else {
        // 外部对齐计算
        if (alignHigh <= 1) {
            x = rx + alignLow * (rw - w) / 2;
            y = ry + (alignHigh * (rh + h)) - h;
        } else {
            x = rx + ((alignHigh - 2) * (rw + w)) - w;
            y = ry + alignLow * (rh - h) / 2;
        }
    }

    ofsX_ = ofsX;
    ofsY_ = ofsY;

    // 计算偏移量
    x += ofsX;
    y += ofsY;
    SetPosition(x, y);
}

void UIView::LayoutLeftToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginLeft_ + style_->marginRight_;
        SetPosition(sib->GetX() - offset - rect_.GetWidth() - margin, GetY());
    }
}

void UIView::LayoutRightToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginRight_ + style_->marginLeft_;
        SetPosition(sib->GetX() + sib->rect_.GetWidth() + offset + margin, GetY());
    }
}

void UIView::LayoutTopToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginTop_ + style_->marginBottom_;
        SetPosition(GetX(), sib->GetY() - offset - rect_.GetHeight() - margin);
    }
}

void UIView::LayoutBottomToSibling(const char* id, int16_t offset)
{
    if (parent_ == nullptr) {
        return;
    }
    UIView* sib = parent_->GetChildById(id);
    if (sib != nullptr) {
        int16_t margin = sib->style_->marginBottom_ + style_->marginTop_;
        SetPosition(GetX(), sib->GetY() + sib->rect_.GetHeight() + offset + margin);
    }
}

uint8_t UIView::GetMixOpaScale() const
{
    uint8_t opaMix = opaScale_;
    UIView* parent = parent_;
    uint8_t opaParent;
    while (parent != nullptr) {
        opaParent = parent->GetOpaScale();
        // 8: Shift right 8 bits
        opaMix = (opaParent == OPA_OPAQUE) ? opaMix : ((static_cast<uint16_t>(opaParent) * opaMix) >> 8);
        parent = parent->GetParent();
    }
    return opaMix;
}

bool UIView::GetBitmap(ImageInfo& bitmap)
{
    UIView* tempSibling = nextSibling_;
    UIView* tempParent = parent_;
    nextSibling_ = nullptr;
    parent_ = nullptr;

    int16_t tempX = rect_.GetX();
    int16_t tempY = rect_.GetY();
    rect_.SetPosition(0, 0);

    Rect screenRect(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    Rect mask = GetRect();
    mask.Intersect(mask, screenRect);
    uint16_t bufferWidth = static_cast<uint16_t>(mask.GetWidth());
    uint16_t bufferHeight = static_cast<uint16_t>(mask.GetHeight());

    bitmap.header.width = bufferWidth;
    bitmap.header.height = bufferHeight;
    bitmap.header.reserved = 0;
    bitmap.header.compressMode = COMPRESS_MODE_NONE;
    bitmap.header.colorMode = RGB888;
    uint32_t stride = DisplayDev::GetInstance()->CalcStride(bitmap.header.width,
        PIXEL_FMT_RGB_888, bitmap.header.compressMode);

#ifdef VERSION_IOT
    bitmap.dataSize = DisplayDev::GetInstance()->CalcSize(stride, bufferHeight, bitmap.header.compressMode);
#else
    bitmap.dataSize = bufferWidth * bufferHeight * DrawUtils::GetByteSizeByColorMode(bitmap.header.colorMode);
#endif

    uint8_t* viewBitmapBuffer = reinterpret_cast<uint8_t*>(ImageCacheMalloc(bitmap));
    if (viewBitmapBuffer == nullptr) {
        nextSibling_ = tempSibling;
        parent_ = tempParent;
        rect_.SetPosition(tempX, tempY);
        return false;
    }

    BufferInfo newBufferInfo;
    newBufferInfo.virAddr = static_cast<void*>(viewBitmapBuffer);
    newBufferInfo.phyAddr = newBufferInfo.virAddr;
    newBufferInfo.width = bufferWidth;
    newBufferInfo.height = bufferHeight;
    newBufferInfo.mode = RGB888;

    newBufferInfo.compressMode = COMPRESS_MODE_NONE;

#ifdef VERSION_IOT
    newBufferInfo.stride = DisplayDev::GetInstance()->CalcStride(bufferWidth,
        PIXEL_FMT_RGB_888, newBufferInfo.compressMode);
#else
    newBufferInfo.stride = bufferWidth * DrawUtils::GetByteSizeByColorMode(bitmap.header.colorMode);
#endif

    if ((DrawUtils::GetMixOpacity(opaScale_, style_->bgOpa_) != OPA_OPAQUE) ||
        (style_->borderRadius_ != 0)) {
        LiteMGfxEngine::GetInstance()->Fill(newBufferInfo, Rect(0, 0, bufferWidth - 1, bufferHeight - 1),
            Color::Black(), OPA_OPAQUE);
    }

    RootView::GetInstance()->SaveDrawContext();
    RootView::GetInstance()->UpdateBufferInfo(&newBufferInfo);
    RootView::GetInstance()->SetSnapshotFlag(true);
    RootView::GetInstance()->MeasureView(this);
    RootView::GetInstance()->DrawTop(this, mask);
#if ENABLE_VGU_ENGINE
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
#endif
    RootView::GetInstance()->RestoreDrawContext();
    RootView::GetInstance()->SetSnapshotFlag(false);
    nextSibling_ = tempSibling;
    parent_ = tempParent;
    rect_.SetPosition(tempX, tempY);
    return true;
}

bool UIView::IsVideoContained()
{
    if (GetViewType() == UI_LITE_SURFACE_VIEW) {
        return true;
    }
    UIViewGroup* group = dynamic_cast<UIViewGroup*>(this);
    if (group == nullptr) {
        return false;
    }

    UIView* view = group->GetChildrenHead();
    while (view != nullptr) {
        if (view->IsVideoContained()) {
            return true;
        }
        view = view->GetNextSibling();
    }
    return false;
}

void UIView::PrintImgInfo(const ImageInfo* info)
{
    if (info == nullptr) {
        printf("    ImgInfo[%p]\n", info);
    } else {
        printf("    ImgInfo[%p]: size = [%d, %d], fmt = %d, data = %p, dataSize = %d, file = %s, resId = %u\n",
            info, info->header.width, info->header.height, info->header.colorMode,
            info->data, info->dataSize, info->file, info->resId);
    }
}

void UIView::DumpImgInfoRecursive()
{
    DumpImgInfo();

    if (IsViewGroup()) {
        UIView* child = static_cast<UIViewGroup*>(this)->GetChildrenHead();
        while (child != nullptr) {
            child->DumpImgInfoRecursive();
            child = child->GetNextSibling();
        }
    }
}

std::string UIView::GetGuiInfo() const
{
    return "";
}

#if !ENABLE_MAP_BUFFER
bool UIView::CheckIsNeedMapBufferToTrans(const char* funcName)
{
    UIViewType type = GetViewType();
    if ((type != UI_IMAGE_VIEW) && (type != UI_TEXTURE_MAPPER) && (type != UI_CANVAS_EXT) && (type != UI_MAP_VIEW) &&
        (type != UI_MENU_ITEM) && (type != UI_LABEL_EXT) && (type != UI_LABEL) && (type != UI_TRANSFORM_GROUP) &&
        (type != UI_IMAGE_ANIMATOR_VIEW) && (type != UI_SLIDER) && (type != UI_LOTT_VIEW) && (type != UI_TRANSFORM_BUTTON)) {
        GRAPHIC_LOGE("%s is not supported for view type %d if ENABLE_MAP_BUFFER is 0", funcName, type);
        return false;
    }
    return true;
}
#endif
} // namespace OHOS
