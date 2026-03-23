/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl config file
 */

#include "egl_config.h"
#include <stdlib.h>

static void color_desc_format_to_descriptor(void)
{
    return;
}

EGLint egl_config_init(egl_config *config, VGImageFormat format, EGLint id)
{
    if (config == NULL) {
        vg_err("null ptr");
        return EGL_BAD_PARAMETER;
    }
    config->config_id = id;
    color_desc_format_to_descriptor();
    config->desc.format = format;
    return EGL_SUCCESS;
}

egl_config *egl_config_new(VGImageFormat format, EGLint id)
{
    egl_config *config = (egl_config *)malloc(sizeof(egl_config));
    if (config == NULL) {
        vg_err("malloc failed");
        return NULL;
    }
    egl_config_init(config, format, id);
    return config;
}

void egl_config_delete(egl_config *config)
{
    if (config != NULL) {
        free(config);
    }
}
