/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef GRAPHIC_AT_PROCESS_H
#define GRAPHIC_AT_PROCESS_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "soc_osal.h"
#include "errcode.h"
#include "diag_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define GRAPHIC_CMD_LEN 4
#define GRAPHIC_GUI_TYPE_POS  0
#define GRAPHIC_SCENE_CMD_POS 1
#define GRAPHIC_CASE_CMD_POS  2
#define GRAPHIC_EXTRA_CMD_POS 3
#define GRAPHIC_PARAM_OFFSET  4

#define GRAPHIC_MAX_PARAM_CNT 64
#define GRAPHIC_MAX_PARAM_LEN 128

typedef int32_t (*GraphicProcessFunc)(commu_inter_info_t *cmd);

int32_t uapi_vau_module_init(void);
void uapi_vau_module_exit(void);
int32_t uapi_dpu_module_init(void);
void uapi_dpu_module_exit(void);

typedef enum {
    PARAM_TYPE_NULL,
    PARAM_TYPE_ADDR,
    PARAM_TYPE_SET_ADDR,
    PARAM_TYPE_CALL_NUM,
    PARAM_TYPE_MUSIC,
    PARAM_TYPE_MUSIC_PLAY,
    PARAM_TYPE_DISC_PHONE,
    PARAM_TYPE_HIGH_POWER,
    PARAM_TYPE_SET_VALUE,
} ParamTypeIdE;

typedef enum {
    GRAPHIC_UIKIT,
    GRAPHIC_SET_FREQ,
    GRAPHIC_LVGL,
    GRAPHIC_GPU,
    GRAPHIC_LCD_STATUS,
    GRAPHIC_CMD_MAX,
#if (CONFIG_MEDIAAW_UIKIT_ENABLE == FEATURE_ON)
    GRAPHIC_AW,
#endif
} GraphicCmd;

typedef enum {
    UIKIT_SAMPLE,
    UIKIT_DFX,
    UIKIT_MAX,
    UIKIT_SAMPLE_RUN,
    UIKIT_SAMPLE_HASH,
} UikitCmd;

typedef enum {
    LVGL_SAMPLE,
    LVGL_DFX,
    LVGL_MAX,
} LvglCmd;

typedef struct {
    uint8_t id;
    GraphicProcessFunc func;
} GraphicProcessType;

void sample_gfx_init(void);
void graphic_at_process(uint8_t *data, uint32_t data_len);
uint32_t graphic_diag_cmd_process(diag_ser_data_t *data);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif