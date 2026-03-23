/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 */

#ifndef GRAPHIC_HARDWARE_TYPES_H
#define GRAPHIC_HARDWARE_TYPES_H

#include <cstdint>
#include "gfx_utils/image_info.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
/**
 * @brief Enumerates path commands.
 */
enum PathCmd {
    CMD_CLOSE = 0,
    CMD_MOVE_TO = 1 << 1,
    CMD_LINE_TO = 2 << 1,
    CMD_HLINE_TO = 3 << 1,
    CMD_VLINE_TO = 4 << 1,
    CMD_QUARD_TO = 5 << 1,
    CMD_CUBIC_TO = 6 << 1,
    CMD_SQUARD_TO = 7 << 1,
    CMD_SCUBIC_TO = 8 << 1,
    CMD_SCCWARC_TO = 9 << 1,
    CMD_SCWARC_TO = 10 << 1,
    CMD_LCCWARC_TO = 11 << 1,
    CMD_LCWARC_TO = 12 << 1,
};

/**
 * @brief Enumerates fill rules.
 */
enum class FillRule {
    NON_ZERO = 0,
    EVEN_ODD,
};

/**
 * @brief Enumerates gradient spread mode that defines pixels outside the gradient area.
 */
enum class GradientSpread {
    SPREAD_PAD,
    SPREAD_REFLECT,
    SPREAD_REPEAT,
};

/**
 * @brief Enumerates join types for stroke.
 */
enum class JoinType {
    JOIN_MITER = 0,
    JOIN_ROUND,
    JOIN_BEVEL,
};

/**
 * @brief Enumerates gradient types.
 */
enum class GradientType {
    GRADIENT_LINEAR,
    GRADIENT_RADIAL,
    GRADIENT_SWEEP,
    GRADIENT_BUTT,
};

/**
 * @brief Define fill type of pattern.
 */
enum class PatternType {
    PATTERN_DISABLE = 0,
    PATTERN_FILL,
    PATTERN_PAD,
    PATTERN_REFLECT, /* < The pattern is reflected */
    PATTERN_REPEAT,  /* < The pattern is repeated */
};

/**
 * @brief Enumerates paint types.
 */
enum class PaintType {
    PAINT_GRADIENT,
    PAINT_PATTERN,
    PAINT_COLOR,
    PAINT_BUTT
};

/**
 * @brief Path params.
 */
struct Path {
    uint32_t cmdNum;   // number of commands.
    uint8_t* cmds;     // commands.
    uint32_t dataNum;  // number of data.
    float* data;       // data.
};

/**
 * @brief Define gradient paint start and stop color.
 */
struct ColorStop {
    float stop;
    uint32_t color;
};

/**
 * @brief Linear gradient params.
 */
struct LinearGradient {
    int16_t startX;
    int16_t startY;
    int16_t endX;
    int16_t endY;
};

/**
 * @brief Radial gradient params.
 */
struct RadialGradient {
    int16_t centerX;
    int16_t centerY;
    int16_t focalX; /* currently not supported */
    int16_t focalY; /* currently not supported */
    int16_t radius;
};

/**
 * @brief Sweep gradient params.
 * @param centerX Indicates the x coordinate of the circle center.
 * @param centerY Indicates the y coordinate of the circle center.
 * @param startAngle Indicates the start angle of the sector. Value <b>0</b> indicates the 12-o'clock direction,
 *                   and <b>90</b> indicates the 3-o'clock direction.
 */
struct SweepGradient {
    int16_t centerX;
    int16_t centerY;
    int16_t startAngle;
};

/**
 * @brief Pattern paint info.
 */
struct PatternInfo {
    ImageInfo img;
    PatternType wrap;
    uint32_t color;
    Matrix3<float> mat3;
};
}
#endif