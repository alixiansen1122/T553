/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_canvas_ext.h"
#include "lv_canvas_ext.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_CANVAS_EXT

static const uint32_t YELLOW = 0XFFFFFF00;
static const uint32_t RED = 0XFFFF0000;
static const uint32_t BLUE = 0XFF0000FF;
static const uint32_t GREY = 0XFFAAAAAA;
static const uint32_t ORANGE = 0XFFFF7F00;
static const uint32_t GREEN = 0XFF00FF00;
static bool g_isInited = false;

static uint32_t GetMixedColor(uint32_t color, uint8_t opa)
{
    uint8_t alpha = (uint8_t)(((color >> 24) * opa) / LV_OPA_COVER); // 24: alpha shift
    uint32_t mixedColor = (alpha << 24) | (color & 0x00FFFFFF); // 24: alpha shift
    return mixedColor;
}

static void InitBasicStrokeAttr(LvStrokeAttr* strokeAttr, uint32_t color,
    uint16_t strokeWidth, LvCapType cap, LvJoinType join)
{
    strokeAttr->shader.type = SHADER_COLOR;
    strokeAttr->shader.color = color;
    strokeAttr->strokeWidth = strokeWidth;
    strokeAttr->capType = cap;
    strokeAttr->joinType = join;
}

static void TestSweepGradient(lv_obj_t* canvas, const LvPaint* basePaint)
{
    static LvPaint paint = {0};
    paint = *basePaint;
    paint.fillAttr.shader.gradient.gradientType = GRADIENT_SWEEP;
    paint.fillAttr.shader.gradient.sweepGradient.centerX = 75; // 75: centerX
    paint.fillAttr.shader.gradient.sweepGradient.centerY = 125; // 125: centerY
    paint.fillAttr.shader.gradient.sweepGradient.startAngle = 90; // 90: startAngle

    uint32_t cmdNum = 5; // 5: cmdNum
    uint32_t dataNum = 5; // 5: dataNum
    uint8_t c[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_CLOSE}; // 5: cmdNum
    float d[5] = {50, 100, 100, 150, 50}; // 5: dataNum, 50, 100, 100, 150, 50: data

    int16_t pathId = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvCanvasExtDrawPath(canvas);
}

static void TestLinearGradient(lv_obj_t* canvas, const LvPaint* basePaint)
{
    static LvPaint paint = {0};
    paint = *basePaint;
    paint.fillAttr.shader.gradient.gradientType = GRADIENT_LINEAR;
    paint.fillAttr.shader.gradient.linearGradient.startX = 150; // 150: startX
    paint.fillAttr.shader.gradient.linearGradient.startY = 0;
    paint.fillAttr.shader.gradient.linearGradient.endX = 180; // 180: endX
    paint.fillAttr.shader.gradient.linearGradient.endY = 0;
    paint.fillAttr.shader.gradient.gradientSpread = SPREAD_PAD;

    uint32_t cmdNum = 5; // 5: cmdNum
    uint32_t dataNum = 5; // 5: dataNum
    uint8_t c[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_CLOSE}; // 5: cmdNum
    float d[5] = {150, 100, 200, 150, 150}; // 5: dataNum, 150, 100, 200, 150, 150: data

    int16_t pathId = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvCanvasExtDrawPath(canvas);
}

static void TestRadialGradient(lv_obj_t* canvas, const LvPaint* basePaint)
{
    // SPREAD_REFLECT
    static LvPaint paint = {0};
    paint = *basePaint;
    paint.fillAttr.shader.gradient.gradientType = GRADIENT_RADIAL;
    paint.fillAttr.shader.gradient.radialGradient.centerX = 275; // 275: centerX
    paint.fillAttr.shader.gradient.radialGradient.centerY = 125; // 125: centerY
    paint.fillAttr.shader.gradient.radialGradient.radius = 20; // 20: radius
    paint.fillAttr.shader.gradient.gradientSpread = SPREAD_REFLECT;

    uint32_t cmdNum = 5; // 5: cmdNum
    uint32_t dataNum = 5; // 5: dataNum
    uint8_t c[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_CLOSE}; // 5: cmdNum
    float d[5] = {250, 100, 300, 150, 250}; // 5: dataNum, 250, 100, 300, 150, 250: data

    int16_t pathId = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvCanvasExtDrawPath(canvas);

    // SPREAD_REPEAT
    paint.fillAttr.shader.gradient.radialGradient.centerX = 275; // 275: center
    paint.fillAttr.shader.gradient.gradientSpread = SPREAD_REPEAT;

    int16_t pathId1 = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvMatrix mat = {{1, 0, 100, 0, 1, 0, 0, 0, 1}}; // 100: data
    LvCanvasExtSetMatrix(canvas, &mat, pathId1);
    LvCanvasExtDrawPath(canvas);
}

static void TestGradient(lv_obj_t* canvas)
{
    // Sweep Gradient
    static LvPaint paint = {0};
    paint.style = STROKE_FILL_STYLE;
    InitBasicStrokeAttr(&paint.strokeAttr, RED, 5, 0, JOIN_BEVEL); // 5: width

    static LvColorStop stops[3] = {{0, BLUE}, {0.5, ORANGE}, {1, GREEN}}; // 3: stop size
    paint.fillAttr.shader.gradient.stops = stops;
    paint.fillAttr.shader.gradient.stopCount = 3; // 3: stop size
    paint.fillAttr.shader.type = SHADER_GRADIENT;

    TestSweepGradient(canvas, &paint);
    TestLinearGradient(canvas, &paint);
    TestRadialGradient(canvas, &paint);
}

static void TestDrawLine(lv_obj_t* canvas)
{
    uint16_t x = 150;
    uint16_t y = 20;
    uint16_t xGap = 30;
    uint16_t yEnd = 50;

    static LvPaint paint = {0};
    paint.isAntialiased = false;
    paint.style = STROKE_STYLE;
    InitBasicStrokeAttr(&paint.strokeAttr, GREEN, 10, 0, 0); // 10: width

    LvCanvasExtDrawLine(canvas, (lv_point_t){x, y}, (lv_point_t){x, yEnd}, &paint);
    x += xGap;

    paint.strokeAttr.shader.color = GetMixedColor(GREEN, LV_OPA_50);
    LvCanvasExtDrawLine(canvas, (lv_point_t){x, y}, (lv_point_t){x, yEnd}, &paint);
    x += xGap;

    paint.strokeAttr.capType = CAP_ROUND;
    LvCanvasExtDrawLine(canvas, (lv_point_t){x, y}, (lv_point_t){x, yEnd}, &paint);
    x += xGap;

    paint.strokeAttr.capType = CAP_SQUARE;
    LvCanvasExtDrawLine(canvas, (lv_point_t){x, y}, (lv_point_t){x, yEnd}, &paint);
    x += xGap;

    paint.strokeAttr.strokeWidth = 0;
    LvCanvasExtDrawLine(canvas, (lv_point_t){x, y}, (lv_point_t){x, yEnd}, &paint);
    x += xGap;
}

static void TestJoinType(lv_obj_t* canvas)
{
    static LvPaint paint = {0};
    paint.isAntialiased = false;
    paint.style = STROKE_STYLE;
    InitBasicStrokeAttr(&paint.strokeAttr, GREEN, 10, 0, JOIN_BEVEL); // 10: width

    uint32_t cmdNum = 3; // 3: cmdNum
    uint32_t dataNum = 5; // 5: dataNum
    uint8_t c[3] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_LINE_TO};
    float d[5] = {50, 60, 90, 50, 100}; // 5: dataNum, 50, 60, 90, 50, 100: data

    int16_t pathId = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvCanvasExtDrawPath(canvas);

    /* round */
    paint.strokeAttr.joinType = JOIN_ROUND;
    int16_t pathId1 = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvMatrix mat = {{1, 0, 100, 0, 1, 0, 0, 0, 1}}; // 100: data
    LvCanvasExtSetMatrix(canvas, &mat, pathId1);
    LvCanvasExtDrawPath(canvas);

    /* miter */
    paint.strokeAttr.joinType = JOIN_MITER;
    paint.strokeAttr.miterLimit = 5; // 5: miter limit
    int16_t pathId2 = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    mat.m[0][2] = 200; // 2: index, 200: x translate
    LvCanvasExtSetMatrix(canvas, &mat, pathId2);
    LvCanvasExtDrawPath(canvas);

    paint.strokeAttr.joinType = JOIN_MITER;
    paint.strokeAttr.miterLimit = 50; // 50: miterLimit
    int16_t pathId3 = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    mat.m[0][2] = 300; // 2: index, 300: x translate
    LvCanvasExtSetMatrix(canvas, &mat, pathId3);
    LvCanvasExtDrawPath(canvas);
}

static void TestDrawArc(lv_obj_t* canvas)
{
    static LvPaint paint = {0};
    lv_point_t center = {375, 200}; // 375, 200: x, y
    paint.isAntialiased = true;
    paint.style = STROKE_STYLE;
    InitBasicStrokeAttr(&paint.strokeAttr, ORANGE, 5, 0, JOIN_BEVEL); // 5: width

    LvCanvasExtDrawArc(canvas, center, 10, 0, 90, &paint); // 10, 0, 90: radius, start angle, end angle
    LvCanvasExtDrawArc(canvas, center, 10, 90, 180, &paint); // 10, 90, 180: radius, start angle, end angle
    LvCanvasExtDrawArc(canvas, center, 10, 180, 360, &paint); // 10, 180, 360: radius, start angle, end angle

    LvCanvasExtDrawArc(canvas, center, 20, 290, 30, &paint); // 20, 290, 30: radius, start angle, end angle
    LvCanvasExtDrawArc(canvas, center, 30, 10, 360, &paint); // 30, 10, 360: radius, start angle, end angle
    LvCanvasExtDrawArc(canvas, center, 40, 540, 630, &paint); // 40, 540, 630: radius, start angle, end angle
}

static void TestMatrix(lv_obj_t* canvas)
{
    static LvPaint paint = {0};
    paint.isAntialiased = true;
    paint.style = STROKE_STYLE;
    InitBasicStrokeAttr(&paint.strokeAttr, RED, 5, 0, JOIN_BEVEL); // 5: width

    int16_t pathId = LvCanvasExtDrawCurve(canvas, (lv_point_t){100, 280}, (lv_point_t){150, 300},
        (lv_point_t){100, 320}, (lv_point_t){150, 330}, &paint);

#if LV_USE_DEMO_ROTATE_SCALE_MATRIX
    int16_t pathId1 = LvCanvasExtDrawCurve(canvas, (lv_point_t){100, 280}, (lv_point_t){150, 300},
        (lv_point_t){100, 320}, (lv_point_t){150, 330}, &paint);
    LvMatrix mat = {{0, -1, 605, 1, 0, 225, 0, 0, 1}}; // rotate and translate
    LvCanvasExtSetMatrix(canvas, &mat, pathId1);

    int16_t pathId2 = LvCanvasExtDrawCurve(canvas, (lv_point_t){100, 280}, (lv_point_t){150, 300},
        (lv_point_t){100, 320}, (lv_point_t){150, 330}, &paint);
    LvMatrix mat1 = {{2, 0, 50, 0, 1, 0, 0, 0, 1}}; // scale and translate
    LvCanvasExtSetMatrix(canvas, &mat1, pathId2);
#endif
}

static void InitBasicFillAttr(LvFillAttr* fillAttr, LvFillRule rule, uint32_t color)
{
    fillAttr->fillRule = rule;
    fillAttr->shader.type = SHADER_COLOR;
    fillAttr->shader.color = color;
}

static void TestFillRule(lv_obj_t* canvas)
{
    // NON_ZERO
    static LvPaint paint = {0};
    paint.style = FILL_STYLE;
    InitBasicFillAttr(&paint.fillAttr, NON_ZERO, BLUE);

    uint32_t cmdNum = 10;
    uint32_t dataNum = 12;
    uint8_t c[10] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_VLINE_TO,
        CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_VLINE_TO};
    float d[12] = {50, 160, 100, 210, 50, 160, 60, 170, 90, 200, 60, 170};

    int16_t pathId = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvCanvasExtDrawPath(canvas);

    // EVEN_ODD
    paint.fillAttr.fillRule = EVEN_ODD;
    int16_t pathId1 = LvCanvasExtBeginPath(canvas, &paint);
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);
    LvMatrix mat = {{1, 0, 0, 0, 1, 55, 0, 0, 1}};
    LvCanvasExtSetMatrix(canvas, &mat, pathId1);
    LvCanvasExtDrawPath(canvas);
}

#if LV_USE_DEMO_PATTERN
#ifdef CONFIG_PSRAM_SUPPORT
static void TestPattern(lv_obj_t* canvas)
{
    /* Pattern Fill */
    static LvPaint paint = {0};
    paint.style = FILL_STYLE;
    paint.fillAttr.shader.type = SHADER_PATTERN;
    paint.fillAttr.shader.pattern.type = PATTERN_FILL;
    paint.fillAttr.shader.pattern.fillColor = GREEN;
    memset_s(paint.fillAttr.shader.pattern.matrix.m, sizeof(float) * 9, 0, sizeof(float) * 9); // 9: size
    LvInitPatternSrc(&paint.fillAttr.shader.pattern, "/user/res/A094_051_8888_hfbc.bin");

    lv_point_t center = {0, 0};
    LvMatrix imgMat = {{0.36, 0, 0, 0, 0.36, 0, 0, 0, 1}}; // 0.36: scaler in x and y direction
    memcpy_s(paint.fillAttr.shader.pattern.matrix.m, sizeof(float) * 9, imgMat.m, sizeof(float) * 9); // 9: size
    int16_t pathId = LvCanvasExtDrawArc(canvas, center, 60, 0, 360, &paint);
    LvMatrix viewMat = {{1, 0, 170, 0, 1, 220, 0, 0, 1}}; // 170: translation in x; 220: translation in y
    LvCanvasExtSetMatrix(canvas, &viewMat, pathId);

    /* Pattern Pad */
    LvInitPatternSrc(&paint.fillAttr.shader.pattern, "/user/res/lele.jpg");
    paint.fillAttr.shader.pattern.type = PATTERN_PAD;
    LvMatrix imgMat1 = {{1, 0, 20, 0, 1, 20, 0, 0, 1}}; // 20: translation in x and y direction
    memcpy_s(paint.fillAttr.shader.pattern.matrix.m, sizeof(float) * 9, imgMat1.m, sizeof(float) * 9); // 9: size
    int16_t pathId1 = LvCanvasExtBeginPath(canvas, &paint);

    uint32_t cmdNum = 5;
    uint32_t dataNum = 5;
    uint8_t c[5] = {CMD_MOVE_TO, CMD_HLINE_TO, CMD_VLINE_TO, CMD_HLINE_TO, CMD_CLOSE}; // 5: cmdNum
    float d[5] = {0, 0, 90, 90, 0}; // 5: dataNum, 90: coordinate
    LvCanvasExtSetPathData(canvas, cmdNum, c, dataNum, d);

    LvCanvasExtDrawPath(canvas);

    LvMatrix viewMat1 = {{1, 0, 235, 0, 1, 170, 0, 0, 1}}; // 235: translation in x; 170: translation in y
    LvCanvasExtSetMatrix(canvas, &viewMat1, pathId1);
}
#endif
#endif

void LvDemoCanvasExt(lv_obj_t* par)
{
    lv_obj_set_style_bg_color(par, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(par, LV_OPA_COVER, 0);

    lv_obj_t* canvas = LvCanvasExtCreate(par);

#if LV_USE_TRANSFORM_EXT
    LvCanvasExtTranslate(canvas, (LvVector3){-5.0f, 20.0f, 0.0f});
    LvCanvasExtRotate(canvas, 90.0f, (LvVector3){220.0f, 220.0f, 0.0f}, (LvVector3){220.0f, 220.0f, 1.0f});
    LvCanvasExtScale(canvas, (LvVector3){0.95f, 0.95f, 1.0f}, (LvVector3){220.0f, 220.0f, 0.0f});
#endif

    lv_obj_set_style_bg_color(canvas, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(canvas, LV_OPA_90, LV_PART_MAIN);
    lv_obj_set_height(canvas, 500); // 500: height
    lv_obj_set_width(canvas, 420); // 420: width
    lv_obj_align_to(canvas, par, LV_ALIGN_TOP_MID, 0, 0);

    TestDrawLine(canvas);
    TestJoinType(canvas);
    TestGradient(canvas);
    TestMatrix(canvas);
    TestFillRule(canvas);
    TestDrawArc(canvas);
#if LV_USE_DEMO_PATTERN
#ifdef CONFIG_PSRAM_SUPPORT
    TestPattern(canvas);
#endif
#endif
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    LvDemoCanvasExt(par);
    g_isInited = true;
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "CanvasExt",
    Init,
    Deinit,
};

void LvRegisterDemoCanvasExt(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif
