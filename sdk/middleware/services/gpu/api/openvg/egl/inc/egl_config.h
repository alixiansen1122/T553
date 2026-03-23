/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : egl config file
 */

#ifndef API_OPENVG_EGL_CONFIG_H
#define API_OPENVG_EGL_CONFIG_H

#include <VG/openvg.h>
#include <EGL/egl.h>
#include "vg_object.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    EGLint red_bits;
    EGLint red_shift;
    EGLint green_bits;
    EGLint green_shift;
    EGLint blue_bits;
    EGLint blue_shift;
    EGLint alpha_bits;
    EGLint alpha_shift;
    EGLint luminace_bits;
    EGLint luminace_shift;
    EGLint bpp;

    VGImageFormat format;
} color_desc;

typedef struct egl_config_tag egl_config;
struct egl_config_tag {
    EGLint config_id;
    color_desc desc;
};

EGLint egl_config_init(egl_config *config, VGImageFormat format, EGLint id);
egl_config *egl_config_new(VGImageFormat format, EGLint id);
void egl_config_delete(egl_config *config);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_EGL_CONFIG_H */
