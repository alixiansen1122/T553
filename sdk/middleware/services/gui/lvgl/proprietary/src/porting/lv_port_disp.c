/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_port_disp.h"
#include "lv_display_device.h"

static void LvDispFlush(lv_disp_drv_t* dispDrv, const lv_area_t* area, lv_color_t* color);

static uint8_t g_bufferCount = 2;

void LvPortDispInit(lv_coord_t width, lv_coord_t height)
{
    if (!LvInitDisplayDevice(width, height)) {
        LV_LOGE("Init Display failed.\n");
        return;
    }

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
    uint8_t bufferCount;
    LayerBuffer* buffers = LvGetLayerBuffer(LV_LAYER_0, &bufferCount);
    if (bufferCount != g_bufferCount) {
        LV_LOGE("Double buffer needed.\n");
        return;
    }
    lv_color_t* buf1 = (lv_color_t*)buffers[0].data.virAddr;
    lv_color_t* buf2 = (lv_color_t*)buffers[1].data.virAddr;

    static lv_disp_draw_buf_t drawBuf;
    lv_disp_draw_buf_init(&drawBuf, buf1, buf2, (uint32_t)(width * height));
    drawBuf.buf_stride = (uint32_t)buffers[0].pitch;

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
    static lv_disp_drv_t dispDrv;
    lv_disp_drv_init(&dispDrv);

    /* Set the resolution of the display */
    dispDrv.hor_res = width;
    dispDrv.ver_res = height;

    /* Used to copy the buffer's content to the display */
    dispDrv.flush_cb = LvDispFlush;

    /* Set a display buffer */
    dispDrv.draw_buf = &drawBuf;

#if LV_USE_LOCAL_REFRESH
    dispDrv.full_refresh = 0;
    dispDrv.direct_mode = 1;
#else
    dispDrv.full_refresh = 1;
#endif
    dispDrv.set_px_cb = LvSetPxCb;
    /* Finally register the driver */
    lv_disp_drv_register(&dispDrv);
}

static void LvDispFlush(lv_disp_drv_t* dispDrv, const lv_area_t* area, lv_color_t* color)
{
    (void)color;
    LvFlushHandler(dispDrv, area);
}

/**********************
 *   supports target format as the pixel format of dest buffer.
 **********************/
#if LV_COLOR_DEPTH == 16 // RGB565
#define LV_SHIFT_R 5
#define LV_SHIFT_G 6
#define LV_SHIFT_B 5
#else // RGB888
#define LV_SHIFT_R 8
#define LV_SHIFT_G 8
#define LV_SHIFT_B 8
#endif
LV_ATTRIBUTE_FAST_MEM static inline void LvColorMixPremultExt(const uint16_t* premultSrc, const lv_color_t* dst,
    uint8_t mix, lv_color_t* finalColor)
{
    finalColor->ch.blue = LV_UDIV255(premultSrc[2] + dst->ch.blue * mix + LV_COLOR_MIX_ROUND_OFS); // 2: index
    finalColor->ch.green = LV_UDIV255(premultSrc[1] + dst->ch.green * mix + LV_COLOR_MIX_ROUND_OFS);
    finalColor->ch.red = LV_UDIV255(premultSrc[0] + dst->ch.red * mix + LV_COLOR_MIX_ROUND_OFS); // 2: index
}

LV_ATTRIBUTE_FAST_MEM static inline void LvColorMixExt(lv_color_t src, const lv_color_t* dst,
    uint8_t mix, lv_color_t* finalColor)
{
    finalColor->ch.red = LV_UDIV255((uint16_t)LV_COLOR_GET_R(src) * mix + dst->ch.red *
        (LV_OPA_COVER - mix) + LV_COLOR_MIX_ROUND_OFS);
    finalColor->ch.green = LV_UDIV255((uint16_t)LV_COLOR_GET_G(src) * mix + dst->ch.green *
        (LV_OPA_COVER - mix) + LV_COLOR_MIX_ROUND_OFS);
    finalColor->ch.blue = LV_UDIV255((uint16_t)LV_COLOR_GET_B(src) * mix + dst->ch.blue *
        (LV_OPA_COVER - mix) + LV_COLOR_MIX_ROUND_OFS);
}

static void ColorBlendTrueColorAddExt(lv_color_t fg, lv_color_t* bg, lv_opa_t opa, lv_color_t* finalColor)
{
    if (opa <= LV_OPA_MIN) {
        return;
    }

    uint32_t tmp = (uint32_t)bg->ch.red + fg.ch.red;
    fg.ch.red = LV_MIN(tmp, ((1 << LV_SHIFT_R) - 1));
    tmp = (uint32_t)bg->ch.green + fg.ch.green;
    fg.ch.green = LV_MIN(tmp, ((1 << LV_SHIFT_G) - 1));
    tmp = (uint32_t)bg->ch.blue + fg.ch.blue;
    fg.ch.blue = LV_MIN(tmp, ((1 << LV_SHIFT_B) - 1));

    if (opa == LV_OPA_COVER) {
        finalColor->ch.blue = fg.ch.blue;
        finalColor->ch.green = fg.ch.green;
        finalColor->ch.red = fg.ch.red;
        return;
    }
    LvColorMixExt(fg, bg, opa, finalColor);
}

static void ColorBlendTrueColorSubExt(lv_color_t fg, lv_color_t* bg,
    lv_opa_t opa, lv_color_t* finalColor)
{
    if (opa <= LV_OPA_MIN) {
        return;
    }

    int32_t tmp = bg->ch.red - fg.ch.red;
    fg.ch.red = (uint8_t)LV_MAX(tmp, 0);

    tmp = bg->ch.green - fg.ch.green;
    fg.ch.green = (uint8_t)LV_MAX(tmp, 0);

    tmp = bg->ch.blue - fg.ch.blue;
    fg.ch.blue = (uint8_t)LV_MAX(tmp, 0);

    if (opa == LV_OPA_COVER) {
        finalColor->ch.blue = fg.ch.blue;
        finalColor->ch.green = fg.ch.green;
        finalColor->ch.red = fg.ch.red;
        return;
    }

    LvColorMixExt(fg, bg, opa, finalColor);
}

static void ColorBlendTrueColorMulExt(lv_color_t fg, lv_color_t* bg,
    lv_opa_t opa, lv_color_t* finalColor)
{
    if (opa <= LV_OPA_MIN) {
        return;
    }

    fg.ch.red = ((uint16_t)fg.ch.red * bg->ch.red) >> LV_SHIFT_R;
    fg.ch.green = ((uint16_t)fg.ch.green * bg->ch.green) >> LV_SHIFT_G;
    fg.ch.blue = ((uint16_t)fg.ch.blue * bg->ch.blue) >> LV_SHIFT_B;

    if (opa == LV_OPA_COVER) {
        finalColor->ch.blue = fg.ch.blue;
        finalColor->ch.green = fg.ch.green;
        finalColor->ch.red = fg.ch.red;
        return;
    }

    LvColorMixExt(fg, bg, opa, finalColor);
}

void LvSetPxCb(struct _lv_disp_drv_t* dispDrv, uint8_t* buf, lv_coord_t bufW, lv_coord_t x, lv_coord_t y,
    lv_color_t color, lv_opa_t opa, lv_blend_mode_t blendMode)
{
    lv_color_t* dstBuf = (lv_color_t*)&buf[bufW * y + x * DEFAULT_PIX_BPP / 8]; // 8: 8bit per byte
    (void)dispDrv;

    if (blendMode == LV_BLEND_MODE_NORMAL) {
        if (opa >= LV_OPA_MAX) {
            dstBuf->ch.blue = color.ch.blue;
            dstBuf->ch.green = color.ch.green;
            dstBuf->ch.red = color.ch.red;
            return;
        }

        uint16_t colorPremult[3]; // 3: size
        lv_color_premult(color, opa, colorPremult);
        lv_opa_t opaInv = LV_OPA_COVER - opa;
        LvColorMixPremultExt(colorPremult, dstBuf, opaInv, dstBuf);
        return;
    }
#if LV_DRAW_COMPLEX
    if (blendMode == LV_BLEND_MODE_ADDITIVE) {
        ColorBlendTrueColorAddExt(color, dstBuf, opa, dstBuf);
    } else if (blendMode == LV_BLEND_MODE_SUBTRACTIVE) {
        ColorBlendTrueColorSubExt(color, dstBuf, opa, dstBuf);
    } else if (blendMode == LV_BLEND_MODE_MULTIPLY) {
        ColorBlendTrueColorMulExt(color, dstBuf, opa, dstBuf);
    }
#endif
}
