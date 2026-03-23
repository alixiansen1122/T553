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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_sweep_clock.h
 *
 * @brief Declares an sweep clock.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef UI_SWEEP_CLOCK_H
#define UI_SWEEP_CLOCK_H

#include "components/ui_abstract_clock.h"
#include "common/image.h"
#if ENABLE_SWEEP_CLOCK
namespace OHOS {
/**
 * @brief Provides the functions related to an sweep clock.
 *
 * @see UIAbstractClock
 * @since 1.0
 * @version 1.0
 */
class UISweepClock : public UIAbstractClock {
public:
    /**
     * @brief A constructor used to create an <b>UISweepClock</b> instance.
     * @since 1.0
     * @version 1.0
     */
    UISweepClock();

    /**
     * @brief A destructor used to delete the <b>UISweepClock</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~UISweepClock() override {}

    /**
     * @brief Sets the initial time in the 24-hour format.
     *
     * @param hour Indicates the hour to set.
     * @param minute Indicates the minute to set.
     * @param second Indicates the second to set.
     * @since 1.0
     * @version 1.0
     */
    void SetInitTime24Hour(uint8_t hour, uint8_t minute, uint8_t second);

    /**
     * @brief Sets the initial time in the 12-hour format.
     *
     * @param hour Indicates the hour to set.
     * @param minute Indicates the minute to set.
     * @param second Indicates the second to set.
     * @param am Specifies whether it is in the morning. <b>true</b> indicates that it is in the morning,
     *           and <b> false</b> indicates that it is in the afternoon.
     * @since 1.0
     * @version 1.0
     */
    void SetInitTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am);

    /**
     * @brief Updates the time of this sweep clock.
     *
     * @param clockInit Specifies whether it is the first initialization. <b>true</b> indicates it is the
     *                  first initialization, and <b> false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void UpdateClock(bool clockInit) override;

    /**
     * @brief Sets the time in 24-hour format.
     *
     * @param hour Indicates the hour to set, within [0, 23] after the modulo operation.
     * @param minute Indicates the minute to set, within [0, 59] after the modulo operation.
     * @param second Indicates the second to set, within [0, 59] after the modulo operation.
     * @since 1.0
     * @version 1.0
     */
    void SetTime24Hour(uint8_t hour, uint8_t minute, uint8_t second) override;

    /**
     * @brief Sets the time in 24-hour format.
     *
     * @param hour Indicates the hour to set, within [0, 23] after the modulo operation.
     * @param minute Indicates the minute to set, within [0, 59] after the modulo operation.
     * @param millisecond Indicates the millisecond to set, within [0, 59999] after the modulo operation.
     * @since 1.0
     * @version 1.0
     */
    void SetSweepTime24Hour(uint8_t hour, uint8_t minute, uint16_t millisecond);

    /**
     * @brief Sets the time in 12-hour format.
     *
     * @param hour Indicates the hour to set, within [0, 11] after the modulo operation.
     * @param minute Indicates the minute to set, within [0, 59] after the modulo operation.
     * @param millisecond Indicates the millisecond to set, within [0, 59999] after the modulo operation.
     * @param am Specifies whether it is in the morning. <b>true</b> indicates that it is in the morning,
     *           and <b> false</b> indicates that it is in the afternoon.
     * @since 1.0
     * @version 1.0
     */
    void SetSweepTime12Hour(uint8_t hour, uint8_t minute, uint16_t millisecond, bool am);

   /**
     * @brief Enumerates the clock hand types.
     */
    enum class HandType {
        /** Hour hand */
        HOUR_HAND,
        /** Minute hand */
        MINUTE_HAND,
        /** Second hand */
        SECOND_HAND,
    };

    /**
     * @brief Enumerates the drawing types of a clock hand.
     */
    enum class DrawType {
        /** Using a line to draw a clock hand */
        DRAW_LINE,
        /** Using an image to draw a clock hand */
        DRAW_IMAGE
    };

    /**
     * @brief Defines the basic attributes of the sweep clock hand circles.
     *        This is an inner class of <b>UIAbstractClock</b>.
     *
     * @since 1.0
     * @version 1.0
     */
    class HandCircle : public HeapBase {
    public:
        HandCircle() : position_{0, 0}, circleImgInfo_{{0, 0, 0, 0, 0}} {}
        /**
         * @brief Represents the position of a hand circle.
         */
        Point position_;
        /**
         * @brief Represents the image information of a hand circle.
         */
        ImageInfo circleImgInfo_;

        /**
         * @brief Represents the image of a hand circle.
         */
        Image circleImg_;
    };

    /**
     * @brief Defines the basic attributes of the sweep clock hands. This is an inner class of <b>UIAbstractClock</b>.
     *
     * @since 1.0
     * @version 1.0
     */
    class Hand : public HeapBase {
    public:
        /**
         * @brief A default constructor used to create a <b>Hand</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        Hand()
            : center_{0, 0},
              initAngle_(0),
              preAngle_(0),
              nextAngle_(0),
              position_{0, 0},
              imageInfo_{{0, 0, 0, 0, 0}},
              color_{{0, 0, 0}},
              width_(0),
              height_(0),
              opacity_(0),
              drawtype_(DrawType::DRAW_IMAGE)
        {
        }

        /**
         * @brief A destructor used to delete the <b>Hand</b> instance.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual ~Hand() {}

        /**
         * @brief Represents the rotation center of a clock hand.
         */
        Point center_;

        /**
         * @brief Represents the initial clockwise rotation angle of a clock hand. The default value is <b>0</b>,
         *        indicating that the hand direction is vertically upward.
         */
        float initAngle_;

        /**
         * @brief Represents the latest rotation angle of this clock hand.
         */
        float preAngle_;

        /**
         * @brief Represents the next rotation angle of this clock hand.
         */
        float nextAngle_;

        /**
         * @brief Represents the position of a hand on this sweep clock.
         */
        Point position_;

        /**
         * @brief Represents the image information of this clock hand.
         */
        ImageInfo imageInfo_;

        /**
         * @brief Represents the information about the rotation and translation of this clock hand.
         */
        TransformMap trans_;

        /**
         * @brief Represents the new rectangle area after the rotation and translation.
         */
        Rect target_;

        /**
         * @brief Represents the color of this clock hand.
         */
        ColorType color_;

        /**
         * @brief Represents the width of this clock hand.
         */
        uint16_t width_;

        /**
         * @brief Represents the height of this clock hand.
         */
        uint16_t height_;

        /**
         * @brief Represents the opacity of this clock hand.
         */
        OpacityType opacity_;

        /**
         * @brief Represents the drawing type of this clock hand.
         */
        DrawType drawtype_;

        /**
         * @brief Represents the img clock hand.
         */
        Image image_;
    };

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_SWEEP_CLOCK</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_SWEEP_CLOCK;
    }

    /**
     * @brief Sets the image used to draw a clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @param img Indicates the image to set.
     * @param position Indicates the start position of this image.
     * @param center Indicates the rotation center of this clock hand.
     * @since 1.0
     * @version 1.0
     */
    void SetHandImage(HandType type, const Image& img, Point position, Point center);

    /**
     * @brief Sets the image used to draw a circle of clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @param img Indicates the image to set as a hand circle.
     * @param position Indicates the start position of this image.
     * @since 1.0
     * @version 1.0
     */
    void SetHandCircleImage(HandType type, const Image& img, Point position);

    /**
     * @brief Sets the line used to draw a clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @param position Indicates the position of the line endpoint close to the rotation center.
     * @param center Indicates the rotation center of this clock hand.
     * @param color Indicates the color of this line.
     * @param width Indicates the width of this line when it is 12 o'clock.
     * @param height Indicates the height of this line when it is 12 o'clock.
     * @param opacity Indicates the opacity of this line.
     * @since 1.0
     * @version 1.0
     */
    void SetHandLine(HandType type,
                     Point position,
                     Point center,
                     ColorType color,
                     uint16_t width,
                     uint16_t height,
                     OpacityType opacity);

    /**
     * @brief Obtains the rotation center of a specified clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @return Returns the rotation center.
     * @since 1.0
     * @version 1.0
     */
    Point GetHandRotateCenter(HandType type) const;

    /**
     * @brief Obtains the position of a specified clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @return Returns the position of this specified clock hand.
     * @since 1.0
     * @version 1.0
     */
    Point GetHandPosition(HandType type) const;

    /**
     * @brief Obtains the initial rotation angle of the specified clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @return Returns the initial rotation angle of this specified clock hand.
     * @since 1.0
     * @version 1.0
     */
    float GetHandInitAngle(HandType type) const;

    /**
     * @brief Obtains the current rotation angle of the specified clock hand.
     *
     * @param type Indicates the clock hand type, as enumerated in {@link HandType}.
     * @return Returns the current rotation angle of this specified clock hand.
     * @since 1.0
     * @version 1.0
     */
    float GetHandCurrentAngle(HandType type) const;

    /**
     * @brief Draws a sweep clock.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Performs the operations needed after the drawing.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Sets the working mode for this sweep clock.
     *
     * @param newMode Indicates the working mode to set. For details, see {@link WorkMode}.
     * @since 1.0
     * @version 1.0
     */
    void SetWorkMode(WorkMode newMode) override;

    void DumpImgInfo() override;

private:
    Hand hourHand_;
    Hand minuteHand_;
    Hand secondHand_;
    HandCircle hourHandCircle_;
    HandCircle minuteHandCircle_;
    HandCircle secondHandCircle_;

    uint16_t currentMillisecond_;

    void DrawHand(BufferInfo& gfxDstBuffer, const Rect& current, const Rect& invalidatedArea, Hand& hand);
    void DrawHandImage(BufferInfo& gfxDstBuffer, const Rect& current, const Rect& invalidatedArea, Hand& hand);
    void DrawHandLine(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, Hand& hand);
    void DrawHandCircle(BufferInfo& gfxDstBuffer, const Rect& current, const Rect& invalidatedArea, HandCircle& circle);
    uint16_t ConvertHandValueToAngle(uint8_t handValue, uint8_t range,
        uint16_t secondHandValue, uint16_t ratio) const;
    float ConvertHandValueToAngle(uint16_t handValue, uint16_t range) const;
    void CalculateRedrawArea(const Rect& current, Hand& hand, bool clockInit);
};
} // namespace OHOS
#endif // UI_SWEEP_CLOCK_H
#endif
/**
 * @}
 */
