/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl adapter source file
 */

#include "egl_mutex.h"
#include "soc_osal.h"

static osal_mutex g_egl_mutex;

static void egl_init_mutex(void)
{
    if (g_egl_mutex.mutex == NULL) {
        osal_mutex_init(&g_egl_mutex);
    }
}

void egl_deinit_mutex(void)
{
    if (g_egl_mutex.mutex != NULL) {
        osal_mutex_destroy(&g_egl_mutex);
        g_egl_mutex.mutex = NULL;
    }
}

void egl_acquire_mutex(void)
{
    egl_init_mutex();
    osal_mutex_lock(&g_egl_mutex);
}

void egl_release_mutex(void)
{
    if (g_egl_mutex.mutex == NULL) {
        return;
    }
    osal_mutex_unlock(&g_egl_mutex);
}