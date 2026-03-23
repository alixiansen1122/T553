/**
 * @file lv_snapshot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_snapshot.h"
#if LV_USE_SNAPSHOT

#include <stdbool.h>
#include "../../../core/lv_disp.h"
#include "../../../core/lv_refr.h"
#if LV_USE_SNAPSHOT_EXT
#include "lv_port_disp.h"
#include "lv_gralloc_engine.h"
#endif
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/** Get the buffer needed for object snapshot image.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 *
 * @return the buffer size needed in bytes
 */
uint32_t lv_snapshot_buf_size_needed(lv_obj_t * obj, lv_img_cf_t cf)
{
    LV_ASSERT_NULL(obj);
#if LV_USE_DPU
    if (cf != LV_IMG_CF_DEFAULT) {
        return 0;
    }
#else
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR:
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            break;
        default:
            return 0;
    }
#endif

    lv_obj_update_layout(obj);

    /*Width and height determine snapshot image size.*/
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    uint8_t px_size = lv_img_cf_get_px_size(cf);
#if LV_USE_DPU
    return LV_ALIGN_BYTE((uint32_t)w, LV_BYTE_ALIGNMENT) * h * (px_size >> 3); // 3: shift 3 bits
#else
    return w * h * ((px_size + 7) >> 3);
#endif
}

/** Take snapshot for object with its children, save image info to provided buffer.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 * @param dsc    image descriptor to store the image result.
 * @param buf    the buffer to store image data.
 * @param buff_size provided buffer size in bytes.
 *
 * @return LV_RES_OK on success, LV_RES_INV on error.
 */
lv_res_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_img_cf_t cf, lv_img_dsc_t * dsc, void * buf, uint32_t buff_size)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(buf);

#if LV_USE_DPU
    if (cf != LV_IMG_CF_DEFAULT) {
        LV_LOGE("[%s: %d] cf is not LV_IMG_CF_DEFAULT.\n", __FUNCTION__, __LINE__);
        return LV_RES_INV;
    }
#else
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR:
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            break;
        default:
            return LV_RES_INV;
    }
#endif

    if(lv_snapshot_buf_size_needed(obj, cf) > buff_size)
        return LV_RES_INV;

    /*Width and height determine snapshot image size.*/
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    lv_area_t snapshot_area;
    lv_obj_get_coords(obj, &snapshot_area);
    lv_area_increase(&snapshot_area, ext_size, ext_size);

    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    lv_disp_t * obj_disp = lv_obj_get_disp(obj);
    lv_disp_drv_t driver;
    lv_disp_drv_init(&driver);
    /*In lack of a better idea use the resolution of the object's display*/
    driver.hor_res = lv_disp_get_hor_res(obj_disp);
    driver.ver_res = lv_disp_get_hor_res(obj_disp);
    lv_disp_drv_use_generic_set_px_cb(&driver, cf);

    lv_disp_t fake_disp;
    lv_memset_00(&fake_disp, sizeof(lv_disp_t));
    fake_disp.driver = &driver;

    lv_draw_ctx_t * draw_ctx = lv_mem_alloc(obj_disp->driver->draw_ctx_size);
    LV_ASSERT_MALLOC(draw_ctx);
    if(draw_ctx == NULL) return LV_RES_INV;
    obj_disp->driver->draw_ctx_init(fake_disp.driver, draw_ctx);
    fake_disp.driver->draw_ctx = draw_ctx;
    draw_ctx->clip_area = &snapshot_area;
    draw_ctx->buf_area = &snapshot_area;
    draw_ctx->buf = (void *)buf;
    driver.draw_ctx = draw_ctx;
#if LV_USE_DPU
    draw_ctx->buf_stride = LV_ALIGN_BYTE((uint32_t)lv_area_get_width(&snapshot_area), LV_BYTE_ALIGNMENT) *
        (lv_img_cf_get_px_size(cf) >> 3); // 3: shift by 3 bits
#if LV_USE_SNAPSHOT_EXT
    draw_ctx->isSnapshot = true;
#endif
    driver.full_refresh = true;
    driver.set_px_cb = LvSetPxCb;
    driver.draw_buf = obj_disp->driver->draw_buf;
#endif

    lv_draw_rect_dsc_t rectDsc = {0};
    lv_draw_rect_dsc_init(&rectDsc);
    rectDsc.bg_color = lv_color_black();
    fake_disp.driver->draw_ctx->draw_bg(fake_disp.driver->draw_ctx, &rectDsc, &snapshot_area);

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&fake_disp);

    lv_obj_redraw(draw_ctx, obj);

#if LV_USE_VGU
    fake_disp.driver->draw_ctx->wait_for_finish(fake_disp.driver->draw_ctx);
#endif

    _lv_refr_set_disp_refreshing(refr_ori);
    obj_disp->driver->draw_ctx_deinit(fake_disp.driver, draw_ctx);
    lv_mem_free(draw_ctx);

    dsc->data = buf;
    dsc->header.w = w;
    dsc->header.h = h;
    dsc->header.cf = cf;
#if LV_USE_DPU
    dsc->data_size = buff_size;
#endif
    return LV_RES_OK;
}

/** Take snapshot for object with its children, alloc the memory needed.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 *
 * @return a pointer to an image descriptor, or NULL if failed.
 */
lv_img_dsc_t * lv_snapshot_take(lv_obj_t * obj, lv_img_cf_t cf)
{
    LV_ASSERT_NULL(obj);
    uint32_t buff_size = lv_snapshot_buf_size_needed(obj, cf);

#if LV_USE_DPU
    void* buf = LvAllocBufferForImg(buff_size);
#else
    void * buf = lv_mem_alloc(buff_size);
#endif
    LV_ASSERT_MALLOC(buf);
    if(buf == NULL) {
        return NULL;
    }

    lv_img_dsc_t * dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    LV_ASSERT_MALLOC(buf);
    if(dsc == NULL) {
#if LV_USE_DPU
        LvFreeBuffer((uint8_t*)buf);
#else
        lv_mem_free(buf);
#endif
        return NULL;
    }

    if(lv_snapshot_take_to_buf(obj, cf, dsc, buf, buff_size) == LV_RES_INV) {
#if LV_USE_DPU
        LvFreeBuffer((uint8_t*)buf);
#else
        lv_mem_free(buf);
#endif
        lv_mem_free(dsc);
        return NULL;
    }

    return dsc;
}

/** Free the snapshot image returned by @ref lv_snapshot_take
 *
 * It will firstly free the data image takes, then the image descriptor.
 *
 * @param dsc    The image descriptor generated by lv_snapshot_take.
 *
 */
void lv_snapshot_free(lv_img_dsc_t * dsc)
{
    if(!dsc)
        return;

#if LV_USE_DPU
    if (dsc->data != NULL) {
        LvFreeBuffer((uint8_t*)dsc->data);
    }
#else
    if(dsc->data)
        lv_mem_free((void *)dsc->data);
#endif

    lv_mem_free(dsc);
}

#if LV_USE_SNAPSHOT_EXT
#if !LV_USE_SNAPSHOT_EXT_REDRAW
lv_img_dsc_t* LvSnapshotByArea(lv_area_t area)
{
    lv_disp_t* refrOri = _lv_refr_get_disp_refreshing();
    lv_area_t dispArea = {0, 0, lv_disp_get_hor_res(refrOri) - 1, lv_disp_get_ver_res(refrOri) - 1};

    if (!_lv_area_is_in(&area, &dispArea, 0)) {
        LV_LOGE("[%s: %d] snapshot area is invalid.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    uint32_t buffSize = LV_ALIGN_BYTE((uint32_t)lv_area_get_width(&area), LV_BYTE_ALIGNMENT) *
        lv_area_get_height(&area) * (lv_img_cf_get_px_size(LV_IMG_CF_DEFAULT) >> 3); // 3: shift by 3 bits
    uint8_t* buf = LvAllocBufferForImg(buffSize);
    if (buf == NULL) {
        LV_LOGE("[%s: %d] AllocBuffer failed, size = %u.\n", __FUNCTION__, __LINE__, buffSize);
        return NULL;
    }

    lv_img_dsc_t* dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if (dsc == NULL) {
        LvFreeBuffer(buf);
        LV_LOGE("[%s: %d] alloc failed.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    lv_draw_ctx_t* drawCtx = refrOri->driver->draw_ctx;

    lv_draw_img_dsc_t imgDsc = {0};
    lv_draw_img_dsc_init(&imgDsc);
    imgDsc.opa = LV_OPA_COVER;
#if LV_USE_VGU
    imgDsc.blend_mode = LV_BLEND_MODE_SRC;
    imgDsc.stride = drawCtx->buf_stride;
#endif

    uint8_t* oriBuf = drawCtx->buf;
    drawCtx->buf = buf;
    lv_area_t* oriBufArea = drawCtx->buf_area;
    drawCtx->buf_area = &area;
    lv_area_t* oriClip = drawCtx->clip_area;
    drawCtx->clip_area = &area;
    lv_coord_t oriBufStride = drawCtx->buf_stride;
    drawCtx->buf_stride = LV_ALIGN_BYTE((uint32_t)lv_area_get_width(&area), LV_BYTE_ALIGNMENT) *
        (lv_img_cf_get_px_size(LV_IMG_CF_DEFAULT) >> 3);

    drawCtx->draw_img_decoded(drawCtx, &imgDsc, &dispArea, oriBuf, LV_IMG_CF_DEFAULT);
    refrOri->driver->draw_ctx->wait_for_finish(refrOri->driver->draw_ctx);

    drawCtx->buf = oriBuf;
    drawCtx->buf_area = oriBufArea;
    drawCtx->clip_area = oriClip;
    drawCtx->buf_stride = oriBufStride;

    dsc->data = buf;
    dsc->header.w = lv_area_get_width(&area);
    dsc->header.h = lv_area_get_height(&area);
    dsc->header.cf = LV_IMG_CF_DEFAULT;
    dsc->data_size = buffSize;
    return dsc;
}

#else
lv_img_dsc_t* LvSnapshotByArea(lv_area_t area)
{
    lv_disp_t* refrOri = _lv_refr_get_disp_refreshing();
    lv_area_t dispArea = {0, 0, lv_disp_get_hor_res(refrOri) - 1, lv_disp_get_ver_res(refrOri) - 1};

    if (!_lv_area_is_in(&area, &dispArea, 0)) {
        LV_LOGE("[%s: %d] snapshot area is invalid.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    uint32_t buffSize = LV_ALIGN_BYTE((uint32_t)lv_area_get_width(&area), LV_BYTE_ALIGNMENT) *
        lv_area_get_height(&area) * (lv_img_cf_get_px_size(LV_IMG_CF_DEFAULT) >> 3); // 3: shift by 3 bits
    uint8_t* buf = LvAllocBufferForImg(buffSize);
    if (buf == NULL) {
        LV_LOGE("[%s: %d] AllocBuffer failed, size = %d.\n", __FUNCTION__, __LINE__, buffSize);
        return NULL;
    }

    lv_img_dsc_t* dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if (dsc == NULL) {
        LvFreeBuffer(buf);
        LV_LOGE("[%s: %d] alloc failed.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    lv_disp_drv_t driver;
    lv_disp_drv_init(&driver);
    driver.hor_res = lv_disp_get_hor_res(refrOri);
    driver.ver_res = lv_disp_get_ver_res(refrOri);

    lv_disp_t fakeDisp;
    lv_memset_00(&fakeDisp, sizeof(lv_disp_t));
    fakeDisp.driver = &driver;

    lv_draw_ctx_t* drawCtx = lv_mem_alloc(refrOri->driver->draw_ctx_size);
    LV_ASSERT_MALLOC(drawCtx);
    if (drawCtx == NULL) {
        LvFreeBuffer(buf);
        lv_mem_free(dsc);
        LV_LOGE("[%s: %d] alloc drawCtx failed.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    refrOri->driver->draw_ctx_init(fakeDisp.driver, drawCtx);
    fakeDisp.driver->draw_ctx = drawCtx;
    drawCtx->clip_area = &area;
    drawCtx->buf_area = &area;
    drawCtx->buf = (void*)buf;
    drawCtx->buf_stride = LV_ALIGN_BYTE((uint32_t)lv_area_get_width(&area), LV_BYTE_ALIGNMENT) *
        (lv_img_cf_get_px_size(LV_IMG_CF_DEFAULT) >> 3); // 3: shift by 3 bits
    drawCtx->isSnapshot = true;
    driver.full_refresh = true;
    driver.set_px_cb = LvSetPxCb;

    driver.draw_buf = refrOri->driver->draw_buf;
    fakeDisp.act_scr = refrOri->act_scr;
    fakeDisp.top_layer = refrOri->top_layer;
    fakeDisp.sys_layer = refrOri->sys_layer;

    lv_draw_rect_dsc_t rectDsc = {0};
    lv_draw_rect_dsc_init(&rectDsc);
    rectDsc.bg_color = lv_color_black();
    fakeDisp.driver->draw_ctx->draw_bg(fakeDisp.driver->draw_ctx, &rectDsc, &area);

    _lv_refr_set_disp_refreshing(&fakeDisp);

    LvRefrAreaForSnapshot();

#if LV_USE_VGU
    fakeDisp.driver->draw_ctx->wait_for_finish(fakeDisp.driver->draw_ctx);
#endif

    _lv_refr_set_disp_refreshing(refrOri);

    refrOri->driver->draw_ctx_deinit(fakeDisp.driver, drawCtx);
    lv_mem_free(drawCtx);
    dsc->data = buf;
    dsc->header.w = lv_area_get_width(&area);
    dsc->header.h = lv_area_get_height(&area);
    dsc->header.cf = LV_IMG_CF_DEFAULT;
    dsc->data_size = buffSize;
    return dsc;
}
#endif
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SNAPSHOT*/
