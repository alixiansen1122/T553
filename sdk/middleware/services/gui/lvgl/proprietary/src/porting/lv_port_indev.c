/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_port_indev.h"
#include "lv_touch_injector.h"
#include "lv_key_injector.h"
#include "lv_rotate_injector.h"
#include "lv_input_device.h"

static lv_indev_t* g_indevTouchInjector = NULL;
static lv_indev_t* g_indevKeyInjector = NULL;
static lv_indev_t* g_indevRotateInjector = NULL;
static lv_indev_t* g_indevTouchDev = NULL;
static lv_indev_t* g_indevKeyDev = NULL;
static lv_indev_t* g_indevRotateDev = NULL;

void LvPortIndevInit(void)
{
    /* Init touch injector */
    LvInitTouchInjector();
    static lv_indev_drv_t touchInjector;
    lv_indev_drv_init(&touchInjector);
    touchInjector.type = LV_INDEV_TYPE_POINTER;
    touchInjector.read_cb = LvTouchInjectorRead;
    g_indevTouchInjector = lv_indev_drv_register(&touchInjector);

    /* Init key injector */
    LvInitKeyInjector();
    static lv_indev_drv_t keyInjector;
    lv_indev_drv_init(&keyInjector);
    keyInjector.type = LV_INDEV_TYPE_KEYPAD;
    keyInjector.read_cb = LvKeyInjectorRead;
    g_indevKeyInjector = lv_indev_drv_register(&keyInjector);

    /* Init rotate injector */
    LvInitRotateInjector();
    static lv_indev_drv_t rotateInjector;
    lv_indev_drv_init(&rotateInjector);
    rotateInjector.type = LV_INDEV_TYPE_ENCODER;
    rotateInjector.read_cb = LvRotateInjectorRead;
    g_indevRotateInjector = lv_indev_drv_register(&rotateInjector);

    /* Init touch device */
    if (!LvSetUpInputDevices()) {
        return;
    }
    if (LvOpenTouchDev()) {
        static lv_indev_drv_t touchDev;
        lv_indev_drv_init(&touchDev);
        touchDev.type = LV_INDEV_TYPE_POINTER;
        touchDev.read_cb = LvTouchDevRead;
        g_indevTouchDev = lv_indev_drv_register(&touchDev);
    }

    if (LvOpenKeyDev()) {
        static lv_indev_drv_t keyDev;
        lv_indev_drv_init(&keyDev);
        keyDev.type = LV_INDEV_TYPE_KEYPAD;
        keyDev.read_cb = LvKeyDevRead;
        g_indevKeyDev = lv_indev_drv_register(&keyDev);
    }

    if (LvOpenRotateDev()) {
        static lv_indev_drv_t rotateDev;
        lv_indev_drv_init(&rotateDev);
        rotateDev.type = LV_INDEV_TYPE_ENCODER;
        rotateDev.read_cb = LvRotateDevRead;
        g_indevRotateDev = lv_indev_drv_register(&rotateDev);
    }
}