/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg api release
 */

#include "soc_jpeg_api.h"
#include "soc_osal.h"
#include "jpeg_struct.h"
#include "jpeg_parse.h"
#include "jpeg_mem.h"
#include "jpeg_csc.h"
#include "jpeg_exif.h"
#include "jpeg_stream.h"
#include "drv_jpeg_ioctl.h"

/*-------------------------------------- macro define -------------------------------------*/

#define JPEG_DEV  "/dev/jpeg"

/*-------------------------------------- struct info --------------------------------------*/


/*-------------------------------------- func define --------------------------------------*/

/* for create decompress */
static td_void jpeg_config_par(jpeg_decompress_ptr dinfo, jpeg_source_ptr src, td_bool is_mem);
static td_void jpeg_convert_dinfoscale_to_parscale(jpeg_decompress_ptr dinfo);
static td_void jpeg_convert_dinfooutputcolorspace_to_paroutputcolorspace(jpeg_decompress_ptr dinfo);
static td_void jpeg_config_dinfo(jpeg_decompress_ptr dinfo, jpeg_decompress_par_ptr par);
static td_void jpeg_convert_paroutputcolorspace_to_dinfooutputcolorspace(jpeg_decompress_ptr dinfo);
static td_void jpeg_convert_parinputcolorspace_to_dinfoinputcolorspace(jpeg_decompress_ptr dinfo);
/* for start decompress */
static td_s32  jpeg_decompress_init(jpeg_decompress_par_ptr par, jpeg_dst_ptr dst);
static td_s32  jpeg_decompress_begin(jpeg_decompress_par_ptr par);
static td_void jpeg_decompress_deinit(jpeg_decompress_par_ptr par);

/*-------------------------------------- func release -------------------------------------*/

/* create decompress
 * 1. parse exif
 * 2. parse head
 */
td_s32 uapi_jpeg_decomress_create(jpeg_decompress_ptr dinfo, jpeg_source_ptr src, td_bool is_mem)
{
    td_s32 ret;
    jpeg_mem_mgr mem = { 0 };
    jpeg_decompress_par_ptr par = TD_NULL;

    if ((dinfo == TD_NULL) || (src == TD_NULL) || (dinfo->private_data != TD_NULL)) {
        jpeg_err("input information is error");
        return EXT_FAILURE;
    }

    mem.size = (td_u32)sizeof(struct jpeg_decompress_par);
    ret = jpeg_mem_alloc_small(&mem);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_mem_alloc_small failure");
        return EXT_FAILURE;
    }

    par = (jpeg_decompress_par_ptr)mem.vir_buf;
    dinfo->private_data = (td_void *)mem.vir_buf;

    jpeg_config_par(dinfo, src, is_mem);
    jpeg_exif_init(par);

    ret = jpeg_stream_init(par);
    if (ret != EXT_SUCCESS) {
        uapi_jpeg_decompress_destroy(dinfo);
        jpeg_err("call jpeg_stream_init failure");
        return EXT_FAILURE;
    }

    ret = jpeg_parse_start(par);
    if (ret != EXT_SUCCESS) {
        uapi_jpeg_decompress_destroy(dinfo);
        jpeg_err("call jpeg_parse_start failure");
        return EXT_FAILURE;
    }

    jpeg_config_dinfo(dinfo, par);

    ret = jpeg_ioctl(par->dev_fd, DRV_JPEG_CMD_CREATE, TD_NULL);
    if (ret != EXT_SUCCESS) {
        uapi_jpeg_decompress_destroy(dinfo);
        jpeg_err("call ioctl create failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void jpeg_config_par(jpeg_decompress_ptr dinfo, jpeg_source_ptr src, td_bool is_mem)
{
    jpeg_decompress_par_ptr par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    par->is_stop         = TD_FALSE;
    par->thumbnail_en    = dinfo->thumbnail_en;
    par->output_yuv420sp = dinfo->output_yuv420sp;
    par->src.infile      = src->infile;
    par->src.vir_buf     = src->vir_buf;
    par->src.phy_buf     = src->phy_buf;
    par->src.size        = src->size;
    par->src.size_in_buffer = (is_mem == TD_TRUE) ? src->size : 0;
    par->src.is_mem      = is_mem;
    par->has_sof         = TD_FALSE;
    par->is_progressive  = TD_FALSE;
    par->arith_code      = TD_FALSE;
    par->is_hdr          = TD_FALSE;
    par->out_uv_order    = TD_TRUE;
    par->dev_fd          = -1;
    par->image_color_space = MY_JPEG_COLOR_SPACE_MAX;
    par->reg_mgr.is_start = TD_FALSE;
    par->reg_mgr.mask = DRV_JPEG_MASK;
    par->status.mask  = DRV_JPEG_MASK;
    jpeg_convert_dinfoscale_to_parscale(dinfo);
    jpeg_convert_dinfooutputcolorspace_to_paroutputcolorspace(dinfo);
    return;
}

static td_void jpeg_convert_dinfoscale_to_parscale(jpeg_decompress_ptr dinfo)
{
    jpeg_decompress_par_ptr par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    switch (dinfo->scale) {
        case JPEG_SCALE_1:
            par->scale = MY_JPEG_SCALE_1;
            break;
        case JPEG_SCALE_2:
            par->scale = MY_JPEG_SCALE_2;
            break;
        case JPEG_SCALE_4:
            par->scale = MY_JPEG_SCALE_4;
            break;
        case JPEG_SCALE_8:
            par->scale = MY_JPEG_SCALE_8;
            break;
        default:
            par->scale = MY_JPEG_SCALE_MAX;
            break;
    }
    return;
}

static td_void jpeg_convert_dinfooutputcolorspace_to_paroutputcolorspace(jpeg_decompress_ptr dinfo)
{
    jpeg_decompress_par_ptr par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    switch (dinfo->output_color_space) {
        case JPEG_COLOR_SPACE_YUV400:
            par->output_color_space = MY_JPEG_COLOR_SPACE_YUV400;
            break;
        case JPEG_COLOR_SPACE_YUV420:
            par->output_color_space = MY_JPEG_COLOR_SPACE_YUV420;
            break;
        case JPEG_COLOR_SPACE_YUV422_12:
            par->output_color_space = MY_JPEG_COLOR_SPACE_YUV422_12;
            break;
        case JPEG_COLOR_SPACE_YUV422_21:
            par->output_color_space = MY_JPEG_COLOR_SPACE_YUV422_21;
            break;
        case JPEG_COLOR_SPACE_YUV444:
            par->output_color_space = MY_JPEG_COLOR_SPACE_YUV444;
            break;
        case JPEG_COLOR_SPACE_ARGB8888:
            par->output_color_space = MY_JPEG_COLOR_SPACE_ARGB8888;
            break;
        case JPEG_COLOR_SPACE_RGB888:
            par->output_color_space = MY_JPEG_COLOR_SPACE_RGB888;
            break;
        default:
            par->output_color_space = MY_JPEG_COLOR_SPACE_MAX;
            break;
    }
    return;
}

static td_void jpeg_config_dinfo(jpeg_decompress_ptr dinfo, jpeg_decompress_par_ptr par)
{
    int i;
    dinfo->is_hdr = par->is_hdr;
    dinfo->image_width   = par->image_width;
    dinfo->image_height  = par->image_height;
    dinfo->output_width  = par->output_width;
    dinfo->output_height = par->output_height;
    dinfo->output_stride = par->output_stride;
    dinfo->thumbnail.buffer  = par->exif.buffer;
    dinfo->thumbnail.phy_buf = par->exif.phy_buf;
    dinfo->thumbnail.size    = par->exif.size;
    dinfo->time.year   = par->time.year;
    dinfo->time.month  = par->time.month;
    dinfo->time.day    = par->time.day;
    dinfo->time.hour   = par->time.hour;
    dinfo->time.minute = par->time.minute;
    dinfo->time.second = par->time.second;
    for (i = 0; (i < JPEG_COMPONENTS) && (i < JPEG_MAX_COMPONENTS); i++) {
        dinfo->output.width[i]  = par->output.width[i];
        dinfo->output.height[i] = par->output.height[i];
        dinfo->output.stride[i] = par->output.stride[i];
        dinfo->output.size[i]   = par->output.size[i];
    }
    jpeg_convert_paroutputcolorspace_to_dinfooutputcolorspace(dinfo);
    jpeg_convert_parinputcolorspace_to_dinfoinputcolorspace(dinfo);
    return;
}

static td_void jpeg_convert_paroutputcolorspace_to_dinfooutputcolorspace(jpeg_decompress_ptr dinfo)
{
    jpeg_decompress_par_ptr par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    switch (par->output_color_space) {
        case MY_JPEG_COLOR_SPACE_YUV400:
            dinfo->output_color_space = JPEG_COLOR_SPACE_YUV400;
            break;
        case MY_JPEG_COLOR_SPACE_YUV420:
            dinfo->output_color_space = JPEG_COLOR_SPACE_YUV420;
            break;
        case MY_JPEG_COLOR_SPACE_YUV422_12:
            dinfo->output_color_space = JPEG_COLOR_SPACE_YUV422_12;
            break;
        case MY_JPEG_COLOR_SPACE_YUV422_21:
            dinfo->output_color_space = JPEG_COLOR_SPACE_YUV422_21;
            break;
        case MY_JPEG_COLOR_SPACE_YUV444:
            dinfo->output_color_space = JPEG_COLOR_SPACE_YUV444;
            break;
        case MY_JPEG_COLOR_SPACE_ARGB8888:
            dinfo->output_color_space = JPEG_COLOR_SPACE_ARGB8888;
            break;
        case MY_JPEG_COLOR_SPACE_RGB888:
            dinfo->output_color_space = JPEG_COLOR_SPACE_RGB888;
            break;
        default:
            dinfo->output_color_space = JPEG_COLOR_SPACE_MAX;
            jpeg_err("output color space is not support");
            break;
    }
    return;
}

static td_void jpeg_convert_parinputcolorspace_to_dinfoinputcolorspace(jpeg_decompress_ptr dinfo)
{
    jpeg_decompress_par_ptr par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    switch (par->image_color_space) {
        case MY_JPEG_COLOR_SPACE_YUV400:
            dinfo->image_color_space = JPEG_COLOR_SPACE_YUV400;
            break;
        case MY_JPEG_COLOR_SPACE_YUV420:
            dinfo->image_color_space = JPEG_COLOR_SPACE_YUV420;
            break;
        case MY_JPEG_COLOR_SPACE_YUV422_12:
            dinfo->image_color_space = JPEG_COLOR_SPACE_YUV422_12;
            break;
        case MY_JPEG_COLOR_SPACE_YUV422_21:
            dinfo->image_color_space = JPEG_COLOR_SPACE_YUV422_21;
            break;
        case MY_JPEG_COLOR_SPACE_YUV444:
            dinfo->image_color_space = JPEG_COLOR_SPACE_YUV444;
            break;
        default:
            dinfo->image_color_space = JPEG_COLOR_SPACE_MAX;
            jpeg_err("image color space is not support");
            break;
    }
    return;
}

/* start decompress
 * 1. open dev
 * 2. read stream
 * 3. decompress
 * 4. csc
 */
td_s32 uapi_jpeg_decompress_start(jpeg_decompress_ptr dinfo, jpeg_dst_ptr dst)
{
    td_s32 ret;
    jpeg_decompress_par_ptr par = TD_NULL;

    if ((dinfo == TD_NULL) || (dinfo->private_data == TD_NULL) || (dst == TD_NULL)) {
        jpeg_err("input information is err");
        return EXT_FAILURE;
    }

    par = (jpeg_decompress_par_ptr)(dinfo->private_data);

    ret = jpeg_decompress_init(par, dst);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_decompress_init failure");
        return EXT_FAILURE;
    }

    ret = jpeg_decompress_begin(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_decompress_begin failure");
        jpeg_decompress_deinit(par);
        return EXT_FAILURE;
    }

    ret = jpeg_csc_convert(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_csc_convert failure");
        jpeg_decompress_deinit(par);
        return EXT_FAILURE;
    }

    jpeg_decompress_deinit(par);

    return EXT_SUCCESS;
}

static td_s32 jpeg_decompress_init(jpeg_decompress_par_ptr par, jpeg_dst_ptr dst)
{
    jpeg_mem_mgr mem = { 0 };

    par->dev_fd = jpeg_open(JPEG_DEV, OSAL_O_RDWR, 0);
    if (par->dev_fd < 0) {
        jpeg_err("open jpeg device failure");
        return EXT_FAILURE;
    }

    mem.size  = par->output.size[0] + par->output.size[1];
    mem.align = JPEG_MEM_OUTPUT_ALIGN;

    if ((par->output_color_space == MY_JPEG_COLOR_SPACE_ARGB8888) ||
        (par->output_color_space == MY_JPEG_COLOR_SPACE_RGB888)) {
        jpeg_err("not support yuv to rgb!");
        return EXT_FAILURE;
    } else {
        par->reg_mgr.y_phy_buf = dst->phy_buf;
        par->reg_mgr.y_vir_buf = dst->vir_buf;
    }

    par->reg_mgr.uv_phy_buf    = par->reg_mgr.y_phy_buf + par->output.size[0];
    par->reg_mgr.y_buf_stride  = par->output.stride[0];
    par->reg_mgr.uv_buf_stride = par->output.stride[1];

    return EXT_SUCCESS;
}

static td_s32 jpeg_decompress_begin(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    td_char *buffer = TD_NULL;
    while (1) {
        if (par->is_stop == TD_TRUE) {
            return EXT_FAILURE;
        }

        if (par->src.size_in_buffer == 0) {
            jpeg_stream_fill_buffer(par);
        }
        buffer = par->src.vir_buf + par->src.cur_offset;
        if ((par->src.size_in_buffer == JPEG_2BYTES) &&
            ((td_u32)buffer[0] == JPEG_MARK_TAG) &&
            ((td_u32)buffer[1] == JPEG_MARK_EOI)) {
            break;
        }

        par->reg_mgr.data_phy_buf  = par->src.phy_buf + par->src.cur_offset;
        par->reg_mgr.data_buf_size = par->src.size_in_buffer;
        if (par->reg_mgr.is_start == TD_FALSE) {
            par->reg_mgr.start_value  = 1;
            par->reg_mgr.resume_value = 0;
        } else {
            par->reg_mgr.start_value  = 0;
            par->reg_mgr.resume_value = 1;
        }

        ret = jpeg_ioctl(par->dev_fd, DRV_JPEG_CMD_DECODE, &(par->reg_mgr));
        if (ret != EXT_SUCCESS) {
            jpeg_err("call ioctl decode failure");
            return EXT_FAILURE;
        }
        par->reg_mgr.is_start = TD_TRUE;

        /* get decode state */
        par->status.int_type = DRV_JPEG_INT_TYPE_MAX;
        ret = jpeg_ioctl(par->dev_fd, DRV_JPEG_CMD_GETINTSTATUS, &(par->status));
        if (ret != EXT_SUCCESS) {
            jpeg_err("call ioctl get interrupt state failure");
            return EXT_FAILURE;
        }
        if (par->status.int_type != DRV_JPEG_INT_TYPE_CONTINUE) {
            break;
        }
    }
    return EXT_SUCCESS;
}

static td_void jpeg_decompress_deinit(jpeg_decompress_par_ptr par)
{
    jpeg_mem_mgr mem = { 0 };

    if ((par->output_color_space == MY_JPEG_COLOR_SPACE_ARGB8888) ||
        (par->output_color_space == MY_JPEG_COLOR_SPACE_RGB888)) {
        mem.vir_buf = par->reg_mgr.y_vir_buf;
        mem.phy_buf = (td_u32)par->reg_mgr.y_phy_buf;
        jpeg_mem_free_large(&mem);
    }

    if (par->dev_fd >= 0) {
        jpeg_close(par->dev_fd);
        par->dev_fd = -1;
    }

    return;
}

/* stop decompress
 * 1. reset decompress
 * 2. exit decompress
 */
td_s32 uapi_jpeg_decompress_stop(jpeg_decompress_ptr dinfo)
{
    jpeg_decompress_par_ptr par = TD_NULL;
    if ((dinfo == TD_NULL) || (dinfo->private_data == TD_NULL)) {
        return EXT_FAILURE;
    }
    par = (jpeg_decompress_par_ptr)(dinfo->private_data);
    par->is_stop = TD_TRUE;
    return EXT_SUCCESS;
}

/* destroy decompress
 * 1. stream dinit
 * 2. free private_data of par mem
 */
td_void uapi_jpeg_decompress_destroy(jpeg_decompress_ptr dinfo)
{
    jpeg_mem_mgr mem = { 0 };
    jpeg_decompress_par_ptr par = TD_NULL;

    if ((dinfo == TD_NULL) || (dinfo->private_data == TD_NULL)) {
        return;
    }

    par = (jpeg_decompress_par_ptr)(dinfo->private_data);

    jpeg_stream_deinit(par);

    jpeg_exif_deinit(par);

    td_s32 ret = jpeg_ioctl(par->dev_fd, DRV_JPEG_CMD_DESTROY, TD_NULL);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call ioctl destroy failure");
    }

    mem.vir_buf = (td_char *)dinfo->private_data;
    jpeg_mem_free_small(&mem);

    dinfo->private_data = TD_NULL;

    return;
}
