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

#include "components/ui_sweep_clock.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/style.h"
#include "imgdecode/cache_manager.h"
#include "common/image_cache_manager.h"

#if ENABLE_SWEEP_CLOCK
namespace OHOS {
/**
 * @brief Represents 1000 milliseconds per second.
 */
static constexpr uint16_t ONE_SECOND_IN_MILLISECOND = 1000;

UISweepClock::UISweepClock() : currentMillisecond_(0) {}

void UISweepClock::SetHandImage(HandType type, const Image& img, Point position, Point center)
{
    Hand *hand = nullptr;
    if (type == HandType::HOUR_HAND) {
        hand = &hourHand_;
    } else if (type == HandType::MINUTE_HAND) {
        hand = &minuteHand_;
    } else {
        hand = &secondHand_;
    }

    hand->center_ = center;
    hand->position_ = position;
    hand->initAngle_ = 0;
    hand->preAngle_ = 0;
    hand->nextAngle_ = 0;
    hand->drawtype_ = DrawType::DRAW_IMAGE;

    if (img.GetSrcType() == IMG_SRC_VARIABLE) {
        hand->image_.SetSrc(img.GetImageInfo());
        hand->imageInfo_ = *(img.GetImageInfo());
    } else if (img.GetSrcType() == IMG_SRC_FILE) {
        hand->image_.SetSrc(img.GetPath());
        CacheEntry entry;
        RetCode ret = CacheManager::GetInstance().Open(img.GetPath(), *style_, entry);
        if (ret != RetCode::OK) {
            return;
        }
        hand->imageInfo_ = entry.GetImageInfo();
    } else {
        GRAPHIC_LOGE("unknown img src type.");
        return;
    }
    hand->height_ = hand->imageInfo_.header.height;
    hand->width_ = hand->imageInfo_.header.width;
}

void UISweepClock::SetHandCircleImage(HandType type, const Image& img, Point position)
{
    HandCircle *handCircle = nullptr;
    if (type == HandType::HOUR_HAND) {
        handCircle = &hourHandCircle_;
    } else if (type == HandType::MINUTE_HAND) {
        handCircle = &minuteHandCircle_;
    } else {
        handCircle = &secondHandCircle_;
    }

    handCircle->position_ = position;

    if (img.GetSrcType() == IMG_SRC_FILE) {
        CacheEntry entry;
        RetCode ret = CacheManager::GetInstance().Open(img.GetPath(), *style_, entry);
        if (ret != RetCode::OK) {
            return;
        }
        handCircle->circleImg_.SetSrc(img.GetPath());
        handCircle->circleImgInfo_ = entry.GetImageInfo();
    } else if (img.GetSrcType() == IMG_SRC_VARIABLE) {
        handCircle->circleImg_.SetSrc(img.GetImageInfo());
        handCircle->circleImgInfo_ = *(img.GetImageInfo());
    } else {
        GRAPHIC_LOGE("unknown img src type.");
        return;
    }
}

void UISweepClock::SetHandLine(HandType type, Point position, Point center, ColorType color,
    uint16_t width, uint16_t height, OpacityType opacity)
{
    Hand* hand = nullptr;
    if (type == HandType::HOUR_HAND) {
        hand = &hourHand_;
    } else if (type == HandType::MINUTE_HAND) {
        hand = &minuteHand_;
    } else {
        hand = &secondHand_;
    }

    hand->color_ = color;
    hand->height_ = height;
    hand->width_ = width;
    hand->position_ = position;
    hand->center_ = center;
    hand->opacity_ = opacity;
    hand->initAngle_ = 0;
    hand->preAngle_ = 0;
    hand->nextAngle_ = 0;
    hand->drawtype_ = DrawType::DRAW_LINE;
}

Point UISweepClock::GetHandRotateCenter(HandType type) const
{
    if (type == HandType::HOUR_HAND) {
        return hourHand_.center_;
    } else if (type == HandType::MINUTE_HAND) {
        return minuteHand_.center_;
    } else {
        return secondHand_.center_;
    }
}

Point UISweepClock::GetHandPosition(HandType type) const
{
    if (type == HandType::HOUR_HAND) {
        return hourHand_.position_;
    } else if (type == HandType::MINUTE_HAND) {
        return minuteHand_.position_;
    } else {
        return secondHand_.position_;
    }
}

float UISweepClock::GetHandInitAngle(HandType type) const
{
    if (type == HandType::HOUR_HAND) {
        return hourHand_.initAngle_;
    } else if (type == HandType::MINUTE_HAND) {
        return minuteHand_.initAngle_;
    } else {
        return secondHand_.initAngle_;
    }
}

float UISweepClock::GetHandCurrentAngle(HandType type) const
{
    if (type == HandType::HOUR_HAND) {
        return hourHand_.nextAngle_;
    } else if (type == HandType::MINUTE_HAND) {
        return minuteHand_.nextAngle_;
    } else {
        return secondHand_.nextAngle_;
    }
}

uint16_t UISweepClock::ConvertHandValueToAngle(uint8_t handValue, uint8_t range,
    uint16_t secondHandValue, uint16_t ratio) const
{
    if ((range == 0) || (ratio == 0)) {
        GRAPHIC_LOGW("UISweepClock::ConvertHandValueToAngle Invalid range or ratio\n");
        return 0;
    }
    /*
     * Example: calculate the angle of hour hand
     * Assume that the time is 5: 30, then range is 12, radio is 60
     * angle is [(5 * 60  + 30) / (12 * 60)] * 360
     */
    uint32_t degree = (static_cast<uint16_t>(handValue) * ratio + secondHandValue);
    degree = static_cast<uint32_t>(CIRCLE_IN_DEGREE * degree / (static_cast<uint16_t>(range) * ratio));

    return static_cast<uint16_t>(degree % CIRCLE_IN_DEGREE);
}

float UISweepClock::ConvertHandValueToAngle(uint16_t handValue, uint16_t range) const
{
    if (range == 0) {
        GRAPHIC_LOGW("UISweepClock::ConvertHandValueToAngle Invalid range or ratio\n");
        return 0;
    }
    /*
     * Example: calculate the angle of second hand without millisecond handle
     * Assume that the time is 5:30:30, then range is 60
     * angle is (30 / 60) * 360
     */
    return (static_cast<float>(static_cast<uint16_t>(handValue) * CIRCLE_IN_DEGREE) / range);
}

void UISweepClock::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
}

void UISweepClock::OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect current = GetOrigRect();
    DrawHand(gfxDstBuffer, current, invalidatedArea, hourHand_);
    DrawHandCircle(gfxDstBuffer, current, invalidatedArea, hourHandCircle_);
    DrawHand(gfxDstBuffer, current, invalidatedArea, minuteHand_);
    DrawHandCircle(gfxDstBuffer, current, invalidatedArea, minuteHandCircle_);
    if (GetWorkMode() == WorkMode::NORMAL) {
        DrawHand(gfxDstBuffer, current, invalidatedArea, secondHand_);
        DrawHandCircle(gfxDstBuffer, current, invalidatedArea, secondHandCircle_);
    }
    UIView::OnPostDraw(gfxDstBuffer, invalidatedArea);
}

void UISweepClock::CalculateRedrawArea(const Rect& current, Hand& hand, bool clockInit)
{
    /*
     * Use the current image as an independent rectangular area
     * to calculate the coordinate conversion coefficient.
     */
    int16_t left = hand.position_.x + current.GetLeft();
    int16_t right = left + hand.width_ - 1;
    int16_t top = hand.position_.y + current.GetTop();
    int16_t bottom = top + hand.height_ - 1;
    int16_t compensationDistance = hand.width_ / 2;
    Rect handRect(left, top, right, bottom);
    TransformMap backwardMap(handRect);
    Vector2<float> pivot;
    pivot.x_ = hand.center_.x;
    pivot.y_ = hand.center_.y;

    /* Rotate the specified angle,  */
    backwardMap.Rotate(hand.nextAngle_ - hand.initAngle_, pivot);
    Rect redraw = hand.target_;
    hand.target_ = backwardMap.GetBoxRect();
    hand.trans_ = backwardMap;
    hand.preAngle_ = hand.nextAngle_;
    if (!clockInit) {
        /* Prevent old images from being residued */
        redraw.Join(redraw, hand.target_);
        redraw.SetRect(redraw.GetLeft() - compensationDistance, redraw.GetTop() - compensationDistance,
                       redraw.GetRight() + compensationDistance, redraw.GetBottom() + compensationDistance);
        InvalidateRect(redraw);
    }
}

void UISweepClock::DrawHand(BufferInfo& gfxDstBuffer, const Rect& current, const Rect& invalidatedArea, Hand& hand)
{
    if (hand.drawtype_ == DrawType::DRAW_IMAGE) {
        if (hand.image_.GetSrcType() == IMG_SRC_FILE) {
            CacheEntry entry;
            RetCode ret = CacheManager::GetInstance().Open(hand.image_.GetPath(), *style_, entry);
            if (ret != RetCode::OK) {
                GRAPHIC_LOGE("CacheManager Open failed. DrawHand failed.");
                return;
            }
            hand.imageInfo_ = entry.GetImageInfo();
        } else if (hand.image_.GetSrcType() == IMG_SRC_VARIABLE) {
            ImageCacheManager::GetInstance().UpdateImageInfoIfNecessary(hand.imageInfo_);
            if (hand.imageInfo_.data == nullptr) {
                GRAPHIC_LOGE("DrawHand failed. UpdateImageInfoIfNecessary failed.");
                return;
            }
        } else {
            return;
        }
        DrawHandImage(gfxDstBuffer, current, invalidatedArea, hand);
    } else {
        DrawHandLine(gfxDstBuffer, invalidatedArea, hand);
    }
}

void UISweepClock::DrawHandCircle(BufferInfo& gfxDstBuffer, const Rect& current,
    const Rect& invalidatedArea, HandCircle& circle)
{
    if (circle.circleImg_.GetSrcType() == IMG_SRC_UNKNOWN) {
        return;
    }

    Rect mask;
    mask.Intersect(current, invalidatedArea);

    uint16_t x = static_cast<uint16_t>(circle.position_.x) + current.GetLeft();
    uint16_t y = static_cast<uint16_t>(circle.position_.y) + current.GetTop();
    Rect blitArea = {x, y,
        x + circle.circleImgInfo_.header.width - 1,
        y + circle.circleImgInfo_.header.height - 1};
    circle.circleImg_.DrawImage(gfxDstBuffer, blitArea, mask, *style_, OPA_OPAQUE);
}

void UISweepClock::DrawHandImage(BufferInfo& gfxDstBuffer,
                                 const Rect& current,
                                 const Rect& invalidatedArea,
                                 Hand& hand)
{
    int16_t left = hand.position_.x + current.GetLeft();
    int16_t right = left + hand.imageInfo_.header.width - 1;
    int16_t top = hand.position_.y + current.GetTop();
    int16_t bottom = top + hand.imageInfo_.header.height - 1;

    if (hand.trans_.GetTransMapRect().GetX() != left || hand.trans_.GetTransMapRect().GetY() != top) {
        Rect imgRect(left, top, right, bottom);
        hand.trans_.SetTransMapRect(imgRect);
        hand.target_ = hand.trans_.GetBoxRect();
    }

    uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(hand.imageInfo_.header.colorMode);
    TransformDataInfo imageTranDataInfo = {
        hand.imageInfo_.header, hand.imageInfo_.data, pxSize,
        BlurLevel::LEVEL0, TransformAlgorithm::BILINEAR
    };
    BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, invalidatedArea, { 0, 0 },
        Color::Black(), opaScale_, hand.trans_, imageTranDataInfo);
}

void UISweepClock::DrawHandLine(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, Hand& hand)
{
    float sinma = Sin(hand.nextAngle_);
    float cosma = Sin(hand.nextAngle_ + THREE_QUARTER_IN_DEGREE);
    int32_t handLength = hand.height_;
    Rect rect = GetRect();
    Point start;
    Point end;
    Point curCenter;
    curCenter.x = hand.position_.x + hand.center_.x + rect.GetLeft();
    curCenter.y = hand.position_.y + hand.center_.y + rect.GetTop();

    int32_t startToCenterLength = hand.center_.y;

    int32_t xPointLength = static_cast<int32_t>(startToCenterLength * sinma);
    int32_t yPointLength = static_cast<int32_t>(startToCenterLength * cosma);

    start.x = xPointLength + curCenter.x;
    start.y = yPointLength + curCenter.y;

    /*
     * @ startToCenterLength: means the length between StartPoint and CenterPoint.
     * @ handlength: means the hand height.
     * @ xlength: means X-axis length relative to the center point
     * @ ylength: means Y-axis length relative to the center point
     */
    int32_t xlength = static_cast<int32_t>((startToCenterLength - handLength) * sinma);
    int32_t ylength = static_cast<int32_t>((startToCenterLength - handLength) * cosma);
    end.x = xlength + curCenter.x;
    end.y = ylength + curCenter.y;

    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea,
                                           hand.width_, hand.color_, hand.opacity_);
}

void UISweepClock::SetWorkMode(WorkMode newMode)
{
    WorkMode oldMode = mode_;

    if (oldMode != newMode) {
        /*
         * After entering the alwayson mode, all child controls are no longer drawn,
         * making the simplest sweep clock.
         */
        isViewGroup_ = (newMode == ALWAYS_ON) ? false : true;
        mode_ = newMode;
        Invalidate();
    }
}

void UISweepClock::SetTime24Hour(uint8_t hour, uint8_t minute, uint8_t second)
{
    currentHour_ = hour % ONE_DAY_IN_HOUR;
    currentMinute_ = minute % ONE_HOUR_IN_MINUTE;
    currentSecond_ = second % ONE_MINUTE_IN_SECOND;
    currentMillisecond_ = currentSecond_ * ONE_SECOND_IN_MILLISECOND;
    UpdateClock(false);
}

void UISweepClock::SetInitTime24Hour(uint8_t hour, uint8_t minute, uint8_t second)
{
    currentHour_ = hour % ONE_DAY_IN_HOUR;
    currentMinute_ = minute % ONE_HOUR_IN_MINUTE;
    currentSecond_ = second % ONE_MINUTE_IN_SECOND;
    currentMillisecond_ = currentSecond_ * ONE_SECOND_IN_MILLISECOND;

    hourHand_.initAngle_ = ConvertHandValueToAngle(currentHour_,
        HALF_DAY_IN_HOUR, currentMinute_, ONE_HOUR_IN_MINUTE);
    hourHand_.preAngle_ = hourHand_.initAngle_;
    hourHand_.nextAngle_ = hourHand_.initAngle_;

    minuteHand_.initAngle_ = ConvertHandValueToAngle(currentMinute_,
        ONE_HOUR_IN_MINUTE, currentSecond_, ONE_MINUTE_IN_SECOND);
    minuteHand_.preAngle_ = minuteHand_.initAngle_;
    minuteHand_.nextAngle_ = minuteHand_.initAngle_;

    secondHand_.initAngle_ = ConvertHandValueToAngle(currentSecond_, ONE_MINUTE_IN_SECOND);
    secondHand_.preAngle_ = secondHand_.initAngle_;
    secondHand_.nextAngle_ = secondHand_.initAngle_;

    UpdateClock(true);
}

void UISweepClock::SetInitTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am)
{
    SetInitTime24Hour((hour % HALF_DAY_IN_HOUR) + (am ? 0 : HALF_DAY_IN_HOUR), minute, second);
}

void UISweepClock::SetSweepTime24Hour(uint8_t hour, uint8_t minute, uint16_t millisecond)
{
    currentHour_ = hour % ONE_DAY_IN_HOUR;
    currentMinute_ = minute % ONE_HOUR_IN_MINUTE;
    currentMillisecond_ = millisecond % (ONE_SECOND_IN_MILLISECOND * ONE_MINUTE_IN_SECOND);
    currentSecond_ = millisecond / ONE_SECOND_IN_MILLISECOND;
    UpdateClock(false);
}

void UISweepClock::SetSweepTime12Hour(uint8_t hour, uint8_t minute, uint16_t millisecond, bool am)
{
    SetSweepTime24Hour((hour % HALF_DAY_IN_HOUR) + (am ? 0 : HALF_DAY_IN_HOUR), minute, millisecond);
}

void UISweepClock::UpdateClock(bool clockInit)
{
    float hourAngle = ConvertHandValueToAngle(currentHour_,
        HALF_DAY_IN_HOUR, currentMinute_, ONE_HOUR_IN_MINUTE);

    float minuteAngle = ConvertHandValueToAngle(currentMinute_,
        ONE_HOUR_IN_MINUTE, currentSecond_, ONE_MINUTE_IN_SECOND);

    float secondAngle = ConvertHandValueToAngle(currentMillisecond_,
        ONE_SECOND_IN_MILLISECOND * ONE_MINUTE_IN_SECOND);

    Rect rect = GetRect();
    if (hourAngle != hourHand_.nextAngle_) {
        hourHand_.nextAngle_ = hourAngle;
        CalculateRedrawArea(rect, hourHand_, clockInit);
    }
    if (minuteAngle != minuteHand_.nextAngle_) {
        minuteHand_.nextAngle_ = minuteAngle;
        CalculateRedrawArea(rect, minuteHand_, clockInit);
    }
    if (GetWorkMode() == WorkMode::NORMAL) {
        secondHand_.nextAngle_ = secondAngle;
        CalculateRedrawArea(rect, secondHand_, clockInit);
    }
}

void UISweepClock::DumpImgInfo()
{
    printf("view[%p]: viewType = %d\n", this, GetViewType());
    if ((hourHand_.drawtype_ == DrawType::DRAW_IMAGE) && (hourHand_.image_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = hourHand_.image_.GetImageInfo();
        PrintImgInfo(info);
    }
    if ((minuteHand_.drawtype_ == DrawType::DRAW_IMAGE) && (minuteHand_.image_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = minuteHand_.image_.GetImageInfo();
        PrintImgInfo(info);
    }
    if ((secondHand_.drawtype_ == DrawType::DRAW_IMAGE) && (secondHand_.image_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = secondHand_.image_.GetImageInfo();
        PrintImgInfo(info);
    }

    if ((hourHandCircle_.circleImg_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = hourHandCircle_.circleImg_.GetImageInfo();
        PrintImgInfo(info);
    }

    if ((minuteHandCircle_.circleImg_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = minuteHandCircle_.circleImg_.GetImageInfo();
        PrintImgInfo(info);
    }

    if ((secondHandCircle_.circleImg_.GetSrcType() == IMG_SRC_VARIABLE)) {
        const ImageInfo* info = secondHandCircle_.circleImg_.GetImageInfo();
        PrintImgInfo(info);
    }
}
}
#endif
