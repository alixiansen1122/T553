/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg path
 * Author: @CompanyNameTag
 * Create: 2021-08-16
 */

#ifndef API_OPENVG_PATH_H
#define API_OPENVG_PATH_H

#include "VG/openvg.h"
#include "vg_object.h"
#include "vg_context.h"
#include "vg_image.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct vg_path_tag vg_path;
struct vg_path_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(vg_path *thiz);
    int  (*remove_ref)(vg_path *thiz);
    void (*free)(vg_path *thiz);

    VGPath format;
    VGPathDatatype data_type;
    VGfloat scale;
    VGfloat bias;
    VGbitfield capabilities;

    VGubyte *segments;
    VGuint segment_num;
    VGfloat *datas;
    VGuint data_num;

    void (*hook)(vg_path *thiz);

    VGErrorCode (*geti)(const vg_path *thiz, VGint type, VGint *value);

    VGErrorCode (*clear_path)(vg_path *thiz, VGbitfield capabilities);
    VGErrorCode (*remove_path_capabilities)(vg_path *thiz, VGbitfield capabilities);
    VGbitfield (*get_path_capabilities)(const vg_path *thiz);

    VGErrorCode (*append_path)(vg_path *thiz, const vg_path *src);
    VGErrorCode (*append_path_data)(vg_path *thiz, VGuint segment_num, const VGubyte *segments,
        const float *path_data);
    VGErrorCode (*append_path_data_ext)(vg_path *thiz, VGuint segment_num, const VGubyte *segments,
        const float *datas, VGuint data_num);
    VGErrorCode (*modify_path_coords)(const vg_path *thiz, VGint start_index,
        VGuint segment_num, const float *path_data);
    VGErrorCode (*transform_path)(const vg_path *thiz, const vg_path *src, const vg_context *context);
    VGboolean (*interpolate_path)(const vg_path *thiz, const vg_path *start_path,
        const vg_path *end_path, VGfloat amount);
    VGfloat (*path_lenth)(const vg_path *thiz, VGint start_segment, VGint segment_num);
    VGErrorCode (*draw_path)(const vg_path *thiz, const vg_image *image, VGbitfield paint_modes, vg_context *context);
};

vg_path *vg_path_new(VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGbitfield capabilities);
void vg_path_delete(vg_path *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_PATH_H */

