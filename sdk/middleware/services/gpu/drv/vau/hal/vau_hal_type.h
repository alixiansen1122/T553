/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu hal type define file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_TYPE_H
#define DRV_GRAPHIC_HAL_VAU_HAL_TYPE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAL_VAU_CMP_INFO_CNT 4
#define HAL_VAU_STOPS_COLOR_MAX 7
#define HAL_VAU_COLOR_RECT_MAX 6
#define HAL_VAU_MATRIX_ARRAY_LENGTH 3
#define HAL_VAU_GRADIENT_STOP_RATIO 128
#define HAL_VAU_GRADIENT_AUXILIARY (128 * 256)

typedef enum {
    HAL_VAU_FMT_ARGB8888 = 0x00,
    HAL_VAU_FMT_KRGB8888 = 0x01,
    HAL_VAU_FMT_ARGB4444 = 0x02,
    HAL_VAU_FMT_ARGB1555 = 0x03,
    HAL_VAU_FMT_ARGB8565 = 0x04,
    HAL_VAU_FMT_RGB888   = 0x05,
    HAL_VAU_FMT_RGB444   = 0x06,
    HAL_VAU_FMT_RGB555   = 0x07,
    HAL_VAU_FMT_RGB565   = 0x08,
    HAL_VAU_FMT_A1       = 0x09,
    HAL_VAU_FMT_A8       = 0x0A,
    HAL_VAU_FMT_A1B      = 0x0D,
    HAL_VAU_FMT_CLUT1    = 0x10,
    HAL_VAU_FMT_CLUT2    = 0x11,
    HAL_VAU_FMT_CLUT4    = 0x12,
    HAL_VAU_FMT_CLUT8    = 0x13,
    HAL_VAU_FMT_ACLUT44  = 0x14,
    HAL_VAU_FMT_ACLUT88  = 0x15,
    HAL_VAU_FMT_CLUT1B   = 0x16,
    HAL_VAU_FMT_CLUT2B   = 0x17,
    HAL_VAU_FMT_CLUT4B   = 0x18,
    HAL_VAU_FMT_A2       = 0x19,
    HAL_VAU_FMT_A4       = 0x1a,
    HAL_VAU_FMT_A2B      = 0x1b,
    HAL_VAU_FMT_A4B      = 0x1c,

    HAL_VAU_FMT_MAX
} hal_vau_fmt;

typedef enum {
    HAL_VAU_FMT_ORDER_ARGB = 0x00,
    HAL_VAU_FMT_ORDER_ARBG = 0x01,
    HAL_VAU_FMT_ORDER_AGRB = 0x02,
    HAL_VAU_FMT_ORDER_AGBR = 0x03,
    HAL_VAU_FMT_ORDER_ABRG = 0x04,
    HAL_VAU_FMT_ORDER_ABGR = 0x05,

    HAL_VAU_FMT_ORDER_RAGB = 0x06,
    HAL_VAU_FMT_ORDER_RABG = 0x07,
    HAL_VAU_FMT_ORDER_RGAB = 0x08,
    HAL_VAU_FMT_ORDER_RGBA = 0x09,
    HAL_VAU_FMT_ORDER_RBAG = 0x0a,
    HAL_VAU_FMT_ORDER_RBGA = 0x0b,

    HAL_VAU_FMT_ORDER_GRAB = 0x0c,
    HAL_VAU_FMT_ORDER_GRBA = 0x0d,
    HAL_VAU_FMT_ORDER_GARB = 0x0e,
    HAL_VAU_FMT_ORDER_GABR = 0x0f,
    HAL_VAU_FMT_ORDER_GBRA = 0x10,
    HAL_VAU_FMT_ORDER_GBAR = 0x11,

    HAL_VAU_FMT_ORDER_BRGA = 0x12,
    HAL_VAU_FMT_ORDER_BRAG = 0x13,
    HAL_VAU_FMT_ORDER_BGRA = 0x14,
    HAL_VAU_FMT_ORDER_BGAR = 0x15,
    HAL_VAU_FMT_ORDER_BARG = 0x16,
    HAL_VAU_FMT_ORDER_BAGR = 0x17,

    HAL_VAU_MEM_ORDER_MAX
} hal_vau_argb_order;

typedef enum {
    HAL_VAU_COMPRESS_NONE = 0x0,
    HAL_VAU_COMPRESS_HFBC,
    HAL_VAU_COMPRESS_HFBC_ABYPASS,

    HAL_VAU_COMPRESS_MAX
} hal_vau_compress_mode;

typedef struct {
    td_bool tile_trans_en;
    td_u16 fill_mode;
    td_u16 bkg_width;
    td_u16 bkg_height;
    td_u16 trans_x;
    td_u16 trans_y;
    td_u16 in_reso_width;
    td_u16 in_reso_height;
    td_u16 in_crop_x;
    td_u16 in_crop_y;
    td_u16 out_reso_width;
    td_u16 out_reso_height;
} hal_vau_trans_attr;

typedef struct {
    td_bool is_alpha_max_128;

    td_u8 alpha0;
    td_u8 alpha1;
    td_u8 alpha_thd; /* for output fmt 1555 */

    td_u8 global_alpha;
    td_bool golbal_premult_en;
    td_bool global_alpha_en;

    td_bool premult_en;
    td_bool pixel_alpha_en;
} hal_vau_alpha_info;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stride;

    hal_vau_fmt fmt;
    hal_vau_argb_order argb_order;
    hal_vau_compress_mode compress_mode;
    td_u32 compress_header[HAL_VAU_CMP_INFO_CNT];
    td_u32 addr;
    td_u32 size;

    td_u32 clut_addr;

    td_bool is_color_surface;
    td_u32 color;

    hal_vau_alpha_info alpha_info;
    td_bool affine_en;
    ext_rect rect;
} hal_vau_surface;

typedef enum {
    HAL_VAU_MODE_TYPE_MEM = 0x0,
    HAL_VAU_MODE_TYPE_REG = 0x01,
    HAL_VAU_MODE_TYPE_DMA = 0x02,
    HAL_VAU_MODE_TYPE_REG_DMA = 0x03,

    HAL_VAU_MODE_TYPE_MAX
} hal_vau_src_mode;

typedef enum {
    HAL_VAU_CBM_TYPE_DMA = 0x0,
    HAL_VAU_CBM_TYPE_NORMAL = 0x01,

    HAL_VAU_CBM_TYPE_MAX
} hal_vau_cbm_type;

typedef enum {
    HAL_VAU_BLEND_ZERO = 0x0,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_SRC2COLOR,
    HAL_VAU_BLEND_INVSRC2COLOR,
    HAL_VAU_BLEND_SRC2ALPHA,
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_SRC1COLOR,
    HAL_VAU_BLEND_INVSRC1COLOR,
    HAL_VAU_BLEND_SRC1ALPHA,
    HAL_VAU_BLEND_INVSRC1ALPHA,
    HAL_VAU_BLEND_SRC2ALPHASAT,

    HAL_VAU_BLEND_MAX
} hal_vau_blend_mode;

typedef enum {
    HAL_VAU_OUTALPHA_FROM_NORM = 0,
    HAL_VAU_OUTALPHA_FROM_BACKGROUND,
    HAL_VAU_OUTALPHA_FROM_FOREGROUND,
    HAL_VAU_OUTALPHA_FROM_GLOBALALPHA,
    HAL_VAU_OUTALPHA_FROM_MAX
} hal_vau_blend_alpha_mode;

typedef enum {
    HAL_VAU_TRANSFORM_MODE_DIRECT = 0,
    HAL_VAU_TRANSFORM_MODE_BILINEAR
} hal_vau_transform_mode;

typedef enum {
    HAL_VAU_PATTERN_DISABLE = 0x0,
    HAL_VAU_PATTERN_FILL    = 0x1,
    HAL_VAU_PATTERN_PAD     = 0x2,
} hal_vau_pattern_mode;

typedef td_float hal_vau_matrix[HAL_VAU_MATRIX_ARRAY_LENGTH][HAL_VAU_MATRIX_ARRAY_LENGTH];

typedef struct {
    td_u32 src_width;
    td_u32 src_height;
    td_u32 dst_width;
    td_u32 dst_height;
    hal_vau_fmt src_fmt;
    hal_vau_fmt dst_fmt;
    hal_vau_transform_mode transform_mode;
    hal_vau_matrix pos_m;
    hal_vau_matrix inv_m;
    hal_vau_pattern_mode pattern_mode;
    td_u32 pattern_argb;
} hal_vau_affine_cfg;

typedef enum {
    HAL_VAU_FILL_MODE_EVEN_ODD = 0x0,
    HAL_VAU_FILL_MODE_NONE_ZERO,
    HAL_VAU_FILL_MODE_BUTT
} hal_vau_fill_mode;

typedef enum {
    HAL_VAU_FILL_EFFECT_PURE_COLOR = 0x0,
    HAL_VAU_FILL_EFFECT_ALPHA_FF,
    HAL_VAU_FILL_EFFECT_LINEAR_GRADIENT,
    HAL_VAU_FILL_EFFECT_RADIAL_GRADIENT,
    HAL_VAU_FILL_EFFECT_SWEEP_GRADIENT,
    HAL_VAU_FILL_EFFECT_PATTERN,
    HAL_VAU_FILL_EFFECT_BUTT
} hal_vau_fill_effect;

enum {
    HAL_VAU_PAINT_TILE_MODE_CLAMP = 0,
    HAL_VAU_PAINT_TILE_MODE_REPEAT,
    HAL_VAU_PAINT_TILE_MODE_MIRROR
};

typedef enum {
    HAL_VAU_MSAA_MODE_NONE = 0x0,
    HAL_VAU_MSAA_MODE_4X_FIXED,
    HAL_VAU_MSAA_MODE_4X_CFG,
    HAL_VAU_MSAA_MODE_8X_FIXED,
    HAL_VAU_MSAA_MODE_8X_CFG,
    HAL_VAU_MSAA_MODE_BUTT
} hal_vau_msaa_mode;

typedef struct {
    td_bool draw_en;
    hal_vau_fill_mode fill_mode;
    hal_vau_fill_effect fill_effect;
    hal_vau_msaa_mode msaa_mode;
    td_bool dyn_bdbx_en;
    td_bool cmd_return_en;
    td_bool path_return_en;
    td_u32 draw_start_x;
    td_u32 draw_start_y;
    td_u32 draw_end_x;
    td_u32 draw_end_y;
    td_u32 draw_path_data_addr;
    td_u32 draw_path_data_cnt;
    td_u32 draw_path_cmd_addr;
    td_u32 draw_path_cmd_cnt;
    union {
        struct {
            td_u32 typical_color;
        } color;
        struct {
            td_u32 linear_start_x;
            td_u32 linear_start_y;
            td_float linear_const_x;
            td_float linear_const_y;
            td_u32 linear_const_mode;
        } linear;
        struct {
            td_float radial_const_r;
            td_u32 radial_center_x;
            td_u32 radial_center_y;
        } radial;
        struct {
            td_u32 sweep_center_x;
            td_u32 sweep_center_y;
            td_u32 sweep_start_angle;
        } sweep;
    } gradient;
    td_u32 tile_mode;
    td_u32 stops_offset0;
    td_u32 stops_offset1;
    td_u32 stops_color[HAL_VAU_STOPS_COLOR_MAX];
    td_u32 color_reci[HAL_VAU_COLOR_RECT_MAX];
    td_u32 stops_num;
    td_float matrix[3][3];
} hal_vau_vector_cfg;

typedef struct {
    td_bool t2r_en;
    td_bool flip_en;
    td_bool cch_copy_en;
    td_bool delay_en;
    td_bool cfg_en;
    td_u32 tile_width;
    td_bool t2r_ck_gt_en;
} hal_vau_t2r_info;

enum {
    HAL_VAU_RGB_EXP_0 = 0,
    HAL_VAU_RGB_EXP_HIGHEST = 2,
    HAL_VAU_RGB_EXP_HIGH_BITS = 3
};

enum {
    HAL_VAU_CMP_MODE_CMP,
    HAL_VAU_CMP_MODE_RAW
};

enum {
    HAL_VAU_CMP_MODE_A_COMPRESS,
    HAL_VAU_CMP_MODE_A_BYPASS
};

typedef enum {
    HAL_VAU_ROP_BLACK = 0x0,
    HAL_VAU_ROP_NOTMERGEPEN,
    HAL_VAU_ROP_MASKNOTPEN,
    HAL_VAU_ROP_NOTCOPYPEN,
    HAL_VAU_ROP_MASKPENNOT,
    HAL_VAU_ROP_NOT,
    HAL_VAU_ROP_XORPEN,
    HAL_VAU_ROP_NOTMASKPEN,
    HAL_VAU_ROP_MASKPEN,
    HAL_VAU_ROP_NOTXORPEN,
    HAL_VAU_ROP_NOP,
    HAL_VAU_ROP_MERGENOTPEN,
    HAL_VAU_ROP_COPYPEN,
    HAL_VAU_ROP_MERGEPENNOT,
    HAL_VAU_ROP_MERGEPEN,
    HAL_VAU_ROP_WHITE,
    HAL_VAU_ROP_MAX
} hal_vau_rop_mode;

typedef struct {
    td_bool rop_enable;
    hal_vau_rop_mode alpha_rop_mode;
    hal_vau_rop_mode color_rop_mode;
} hal_vau_rop_opt;

typedef enum {
    HAL_VAU_COMPONENT_ALPHA = 0,
    HAL_VAU_COMPONENT_RED   = 1,
    HAL_VAU_COMPONENT_GREEN = 2,
    HAL_VAU_COMPONENT_BLUE  = 3,
    HAL_VAU_COMPONENT_MAX,
} hal_vau_component_num;

typedef enum {
    HAL_VAU_COLORKEY_VALUE_IN      = 0,
    HAL_VAU_COLORKEY_VALUE_OUT     = 1,
    HAL_VAU_COLORKEY_VALUE_IGNORE  = 2
} hal_vau_colorkey_mode;

typedef enum {
    HAL_VAU_COLORKEY_BACKGROUND             = 0,
    HAL_VAU_COLORKEY_FOREGROUND_BEFORE_CLUT = 2,
    HAL_VAU_COLORKEY_FOREGROUND_AFTER_CLUT  = 3
} hal_vau_colorkey_sel;

typedef struct {
    td_u16 key_min;
    td_u16 key_max;
    td_u16 key_mask;
    hal_vau_colorkey_mode key_mode;
} hal_vau_colorkey_component;

typedef struct {
    td_bool key_en;
    hal_vau_colorkey_sel key_sel;
    hal_vau_colorkey_component component[HAL_VAU_COMPONENT_MAX];
} hal_vau_colorkey_opt;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif