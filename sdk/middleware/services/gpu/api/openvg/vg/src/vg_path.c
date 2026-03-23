/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg path source file
 * Author: @CompanyNameTag
 * Create: 2021-08-16
 */

#include "vg_path.h"
#include <securec.h>
#include "vg_debug.h"
#include "vau_arc.h"
#include "hdi_gfx.h"
#include "vau_mem.h"

static void vg_path_add_reference(vg_path *thiz)
{
    thiz->ref_count++;
}

static int vg_path_remove_reference(vg_path *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static VGErrorCode vg_path_geti(const vg_path *thiz, VGint type, VGint *value)
{
    switch (type) {
        case VG_PATH_FORMAT:
            *value = (VGint)thiz->format;
            break;
        case VG_PATH_DATATYPE:
            *value = (VGint)thiz->data_type;
            break;
        case VG_PATH_NUM_SEGMENTS:
            *value = (VGint)thiz->segment_num;
            break;
        case VG_PATH_NUM_COORDS:
            *value = (VGint)thiz->data_num;
            break;
        case VG_PATH_BIAS:
        case VG_PATH_SCALE:
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }

    return VG_NO_ERROR;
}

static VGErrorCode vg_path_clear_path(vg_path *thiz, VGbitfield capabilities)
{
    if (thiz->datas != NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)thiz->datas);
        thiz->datas = NULL;
        thiz->data_num = 0;
    }
    if (thiz->segments != NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_CMD, (td_uchar *)thiz->segments);
        thiz->segments = NULL;
        thiz->segment_num = 0;
    }
    thiz->capabilities = capabilities;
    return VG_NO_ERROR;
}

static VGErrorCode vg_path_remove_path_capabilities(vg_path *thiz, VGbitfield capabilities)
{
    VGbitfield caps = capabilities & VG_PATH_CAPABILITY_ALL; // undefined bits are ignored

    thiz->capabilities &= (~caps);

    return VG_NO_ERROR;
}

static VGbitfield vg_path_get_path_capabilities(const vg_path *thiz)
{
    return thiz->capabilities;
}

static VGPathSegment get_path_segment(const VGubyte segments)
{
    VGuint segment = (VGuint)segments;
    return (VGPathSegment)(segment & 0x1e);
}

static int segment_to_num_coordinates(VGPathSegment segment)
{
    /* 0,1,2,4,5,6 are all coordinates */
    static const int coords[13] = {0, 2, 2, 1, 1, 4, 6, 2, 4, 5, 5, 5, 5};
    return coords[(unsigned int)segment >> 1];
}

static void rollback_segments(vg_path *thiz, VGubyte *old_segments, int old_num)
{
    if (old_segments == NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_CMD, (td_uchar *)thiz->segments);
    }
    thiz->segments = old_segments;
    thiz->segment_num = (VGuint)old_num;
}
static VGubyte *vg_path_segment_realloc(vau_mem_type mem_type, VGuint new_num, VGuint old_num, VGubyte *old_segments)
{
    VGubyte *new_segments = NULL;
    VGubyte *tmp_segments = NULL;
    tmp_segments = (VGubyte *)malloc(sizeof(VGubyte) * old_num);
    if (tmp_segments == NULL) {
        vg_err("vau malloc %d failed", sizeof(VGubyte) * old_num);
        return TD_NULL;
    }

    td_s32 ret = memcpy_s(tmp_segments, old_num * sizeof(VGubyte), old_segments, sizeof(VGubyte) * old_num);
    if (ret != EXT_SUCCESS) {
        free(tmp_segments);
        vg_err("memcpy failed!");
        return TD_NULL;
    }
    new_segments = (VGubyte *)vau_mem_realloc(mem_type, (td_uchar *)old_segments, sizeof(VGubyte) * new_num);
    if (new_segments == NULL) {
        free(tmp_segments);
        vg_err("vau malloc %d failed", sizeof(VGubyte) * new_num);
        return TD_NULL;
    }
    ret = memcpy_s(new_segments, new_num * sizeof(VGubyte), tmp_segments, sizeof(VGubyte) * old_num);
    if (ret != EXT_SUCCESS) {
        free(tmp_segments);
        vau_mem_free(mem_type, new_segments);
        vg_err("memcpy failed!");
        return TD_NULL;
    }
    free(tmp_segments);
    return new_segments;
}

static VGfloat *vg_path_data_realloc(vau_mem_type mem_type, VGuint new_num, VGuint old_num, VGfloat *old_datas)
{
    VGfloat *new_datas = NULL;
    VGfloat *tmp_datas = NULL;
    tmp_datas = (VGfloat *)malloc(sizeof(VGfloat) * old_num);
    if (tmp_datas == NULL) {
        vg_err("malloc %d failed", sizeof(VGfloat) * old_num);
        return TD_NULL;
    }
    td_s32 ret = memcpy_s(tmp_datas, old_num * sizeof(VGfloat), old_datas, sizeof(VGfloat) * old_num);
    if (ret != EXT_SUCCESS) {
        free(tmp_datas);
        vg_err("memcpy failed!");
        return TD_NULL;
    }
    new_datas = (VGfloat *)vau_mem_realloc(mem_type, (td_uchar *)old_datas, sizeof(VGfloat) * new_num);
    if (new_datas == NULL) {
        free(tmp_datas);
        vg_err("vau malloc %d failed", sizeof(VGfloat) * new_num);
        return TD_NULL;
    }
    ret = memcpy_s(new_datas, new_num * sizeof(VGfloat), tmp_datas, sizeof(VGfloat) * old_num);
    if (ret != EXT_SUCCESS) {
        free(tmp_datas);
        vau_mem_free(mem_type, (td_uchar *)(uintptr_t)new_datas);
        vg_err("memcpy failed!");
        return TD_NULL;
    }
    free(tmp_datas);
    return new_datas;
}

static VGErrorCode make_new_segments(vg_path *thiz, VGuint segment_num, const VGubyte *segments)
{
    VGubyte *old_segments = thiz->segments;
    int old_num = (int)thiz->segment_num;
    VGubyte *new_segments = NULL;
    VGuint new_num = thiz->segment_num + segment_num;

    if (thiz->segments == NULL) {
        new_segments = (VGubyte *)vau_mem_alloc(VAU_MEM_TYPE_PATH_CMD, sizeof(VGubyte) * new_num);
    } else {
        new_segments = vg_path_segment_realloc(VAU_MEM_TYPE_PATH_CMD, new_num, thiz->segment_num, old_segments);
    }
    if (new_segments == NULL) {
        vg_err("vau malloc %d failed", sizeof(VGubyte) * new_num);
        return VG_OUT_OF_MEMORY_ERROR;
    }
    thiz->segments = new_segments;
    thiz->segment_num = new_num;
    td_s32 ret = vau_mem_round_copy(VAU_MEM_TYPE_PATH_CMD, new_segments + old_num, segment_num * sizeof(VGubyte),
        segments, sizeof(VGubyte) * segment_num);
    if (ret != EXT_SUCCESS) {
        rollback_segments(thiz, old_segments, old_num);
        vg_err("vau_mem_round_copy failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    return VG_NO_ERROR;
}

static void rollback_datas(vg_path *thiz, VGfloat *old_datas, int old_num)
{
    if (old_datas == NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)thiz->datas);
    }
    thiz->datas = old_datas;
    thiz->data_num = (VGuint)old_num;
}

static VGErrorCode make_new_datas(vg_path *thiz, VGuint data_num, const VGfloat *data)
{
    VGfloat *old_datas = thiz->datas;
    int old_num = (int)thiz->data_num;
    VGfloat *new_datas = NULL;
    VGuint new_num = thiz->data_num + data_num;

    if (thiz->datas == NULL) {
        new_datas = (VGfloat *)vau_mem_alloc(VAU_MEM_TYPE_PATH_DATA, sizeof(VGfloat) * new_num);
    } else {
        new_datas = vg_path_data_realloc(VAU_MEM_TYPE_PATH_DATA, new_num, thiz->data_num, old_datas);
    }
    if (new_datas == NULL) {
        vg_err("vau malloc %d failed", sizeof(VGfloat) * new_num);
        return VG_OUT_OF_MEMORY_ERROR;
    }
    thiz->datas = new_datas;
    thiz->data_num = new_num;
    td_s32 ret = vau_mem_round_copy(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)(new_datas + old_num),
        data_num * sizeof(VGfloat), (td_uchar *)data, sizeof(VGfloat) * data_num);
    if (ret != EXT_SUCCESS) {
        rollback_datas(thiz, old_datas, old_num);
        vg_err("vau_mem_round_copy failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode path_append_data(vg_path *thiz, VGuint segment_num, const VGubyte *segments,
    const float *path_data, VGuint data_num)
{
    VGubyte *old_segments = thiz->segments;
    VGuint old_num = thiz->segment_num;

    if (segment_num == 0) {
        return VG_NO_ERROR;
    }

    VGErrorCode ret = make_new_segments(thiz, segment_num, segments);
    if (ret != VG_NO_ERROR) {
        vg_err("make_new_segments failed, ret:0x%x", ret);
        return ret;
    }

    if (data_num == 0) {
        return VG_NO_ERROR;
    }

    ret = make_new_datas(thiz, data_num, path_data);
    if (ret != VG_NO_ERROR) {
        rollback_segments(thiz, old_segments, (VGint)old_num);
        vg_err("make_new_datas failed, ret:0x%x", ret);
        return ret;
    }

    return VG_NO_ERROR;
}

static VGboolean is_arc(VGPathSegment segment)
{
    if (segment < VG_SCCWARC_TO) {
        return VG_FALSE;
    }
    if ((segment == VG_SCCWARC_TO) || (segment == VG_SCWARC_TO) ||
        (segment == VG_LCCWARC_TO) || (segment == VG_LCWARC_TO)) {
        return VG_TRUE;
    }
    return VG_FALSE;
}

static VGErrorCode path_append_arc_cubic(vg_path *thiz, const VGubyte cmd, const float *path_data, int index)
{
    VGfloat *datas = NULL;
    // rh rv rot x0 y0
    if (index < 2) { /* at lease 2 point ahead */
        vg_err("arc cmd can not in first cmd");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    float_point start = { path_data[index - 2], path_data[index - 1] }; /* -2, -1 is start point */
    float_point end = { path_data[index + 3], path_data[index + 4] }; /* +3, +4 is end point */
    int num = 0;
    float_point *cubic = create_arc_cmd(&start, &end, cmd, path_data[index], &num);
    if (cubic == NULL || (num / 3) == 0) { /* cubic content 3 point */
        vg_err("call create_arc_cmd failed");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }

    VGint cmd_num = num / 3; /* cubic content 3 point */
    VGint data_num = num * 2 - 2; /* one point has 2 coords, - 2 start point */

    VGErrorCode ret = VG_OUT_OF_MEMORY_ERROR;

    VGubyte *cmds = (VGubyte *)malloc(sizeof(VGubyte) * cmd_num);
    if (cmds == NULL) {
        vg_err("malloc failed");
        goto destroy;
    }
    for (int i = 0; i < cmd_num; i++) {
        cmds[i] = VG_CUBIC_TO_ABS;
    }
    datas = (VGfloat *)malloc(sizeof(VGfloat) * data_num);
    if (datas == NULL) {
        vg_err("malloc failed");
        goto destroy;
    }
    for (int i = 0; i < num / 3; i++) { /* cubic content 3 point */
        datas[6 * i + 0] = cubic[3 * i + 1].x; /* cubic content 3 point, for 6 coords, index 0 for point 1 */
        datas[6 * i + 1] = cubic[3 * i + 1].y; /* cubic content 3 point, for 6 coords, index 1 for point 1 */
        datas[6 * i + 2] = cubic[3 * i + 2].x; /* cubic content 3 point, for 6 coords, index 2 for point 2 */
        datas[6 * i + 3] = cubic[3 * i + 2].y; /* cubic content 3 point, for 6 coords, index 3 for point 2 */
        datas[6 * i + 4] = cubic[3 * i + 3].x; /* cubic content 3 point, for 6 coords, index 4 for point 3 */
        datas[6 * i + 5] = cubic[3 * i + 3].y; /* cubic content 3 point, for 6 coords, index 5 for point 3 */
    }

    ret = path_append_data(thiz, (VGuint)cmd_num, cmds, datas, (VGuint)data_num);

destroy:
    if (cmds != NULL) {
        free(cmds);
    }
    if (datas != NULL) {
        free(datas);
    }
    destroy_arc_path(cubic);
    return ret;
}

static VGErrorCode vg_path_append_path_data(vg_path *thiz, VGuint segment_num, const VGubyte *segments,
                                            const float *path_data)
{
    VGErrorCode ret;
    /* calculate the number of coordinates */
    VGPathSegment segment = VG_PATH_SEGMENT_FORCE_SIZE;
    int coords;
    VGuint pre_seg_index = 0;
    VGuint pre_data_index = 0;
    VGuint current_seg_index = 0;
    VGuint current_data_index = 0;
    for (; current_seg_index < segment_num;) {
        vg_dbg("cmd[%d] = 0x%x", current_seg_index, segments[current_seg_index]);
        segment = get_path_segment(segments[current_seg_index]);
        if (is_arc(segment) != VG_FALSE) {
            // step1: append pre path
            ret = path_append_data(thiz, (current_seg_index - pre_seg_index), &segments[pre_seg_index],
                &path_data[pre_data_index], (current_data_index - pre_data_index));
            if (ret != VG_NO_ERROR) {
                vg_err("path_append_data failed");
                return ret;
            }
            // step2: arc to cubic and append
            ret = path_append_arc_cubic(thiz, segments[current_seg_index], path_data, (VGint)current_data_index);
            if (ret != VG_NO_ERROR) {
                vg_err("path_append_data failed");
                return ret;
            }

            current_seg_index++;
            coords = segment_to_num_coordinates(segment);
            current_data_index += (VGuint)coords;

            vg_dbg("pre[%d %d], current[%d %d]", pre_seg_index, pre_data_index, current_seg_index, current_data_index);

            pre_seg_index = current_seg_index;
            pre_data_index = current_data_index;

            // if has arc, can not modify data
            thiz->remove_path_capabilities(thiz, VG_PATH_CAPABILITY_MODIFY);
        } else {
            current_seg_index++;
            coords = segment_to_num_coordinates(segment);
            current_data_index += (VGuint)coords;
        }
    }

    vg_dbg("pre[%d %d], current[%d %d]", pre_seg_index, pre_data_index, current_seg_index, current_data_index);

    return path_append_data(thiz, (current_seg_index - pre_seg_index), &segments[pre_seg_index],
        &path_data[pre_data_index], (current_data_index - pre_data_index));
}

static VGErrorCode vg_path_append_path(vg_path *thiz, const vg_path *src)
{
    return vg_path_append_path_data(thiz, src->segment_num, src->segments, src->datas);
}

static VGErrorCode vg_path_modify_path_coords(const vg_path *thiz, VGint start_index, VGuint segment_num,
                                              const float *path_data)
{
    uapi_unused(thiz);
    uapi_unused(start_index);
    uapi_unused(segment_num);
    uapi_unused(path_data);

    return VG_NO_ERROR;
}

static VGErrorCode vg_path_transform_path(const vg_path *thiz, const vg_path *src, const vg_context *context)
{
    uapi_unused(thiz);
    uapi_unused(src);
    uapi_unused(context);

    return VG_NO_ERROR;
}

static VGboolean vg_path_interpolate_path(const vg_path *thiz, const vg_path *start_path,
                                          const vg_path *end_path, VGfloat amount)
{
    uapi_unused(thiz);
    uapi_unused(start_path);
    uapi_unused(end_path);
    uapi_unused(amount);

    return (VGboolean)VG_NO_ERROR;
}

static VGfloat vg_path_path_lenth(const vg_path *thiz, VGint start_segment, VGint segment_num)
{
    uapi_unused(thiz);
    uapi_unused(start_segment);
    uapi_unused(segment_num);

    return VG_NO_ERROR;
}

static VGErrorCode vg_path_draw_path(const vg_path *thiz, const vg_image *image,
                                     VGbitfield paint_modes, vg_context *context)
{
    return hdf_gfx_draw_path(thiz, image, paint_modes, context);
}

static int vg_path_init(vg_path *p, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGbitfield capabilities)
{
    uapi_unused(datatype);
    uapi_unused(scale);
    uapi_unused(bias);

    if (p == NULL) {
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    init_object_common_private_member(vg_path, p, VG_MAGIC_RESOURCE_PATH);
    p->capabilities = capabilities;
    p->geti = vg_path_geti;
    p->clear_path = vg_path_clear_path;
    p->remove_path_capabilities = vg_path_remove_path_capabilities;
    p->get_path_capabilities = vg_path_get_path_capabilities;
    p->append_path = vg_path_append_path;
    p->append_path_data = vg_path_append_path_data;
    p->append_path_data_ext = path_append_data;
    p->modify_path_coords = vg_path_modify_path_coords;
    p->transform_path = vg_path_transform_path;
    p->interpolate_path = vg_path_interpolate_path;
    p->path_lenth = vg_path_path_lenth;
    p->draw_path = vg_path_draw_path;

    return VG_NO_ERROR;
}

vg_path *vg_path_new(VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGbitfield capabilities)
{
    vg_path *p = (vg_path *)malloc(sizeof(vg_path));
    if (p == NULL) {
        vg_err("malloc %d failed", sizeof(vg_path));
        return NULL;
    }
    (void)memset_s(p, sizeof(*p), 0, sizeof(vg_path));
    vg_path_init(p, datatype, scale, bias, capabilities);
    return p;
}

void vg_path_delete(vg_path *p)
{
    if (p == NULL) {
        return;
    }

    if (p->ref_count != 0) {
        vg_err("image ref count not zero:%d\n", p->ref_count);
    }
    if (p->datas != NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)p->datas);
    }
    if (p->segments != NULL) {
        vau_mem_free(VAU_MEM_TYPE_PATH_CMD, (td_uchar *)p->segments);
    }
    free(p);
}
