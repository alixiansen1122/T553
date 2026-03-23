/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : drv vau type file
 */

#ifndef DRV_INCLUDE_DRV_VAU_TYPE_H
#define DRV_INCLUDE_DRV_VAU_TYPE_H

#include "td_base.h"
#include "drv_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_VAU_MAX_SAMPLES 8

#define DRV_ERR_VAU_BASE ((td_s32)(((0x80UL + 0x20UL) << 24) | (100 << 16) | (4 << 13) | 1))
#define DRV_VAU_MAX_STOPS 7
#define DRV_VAU_AFFILE_COEF_ROW 3
#define DRV_VAU_AFFILE_COEF_COLUMN 3

typedef td_u32   drv_vau_color;
typedef td_s32   drv_vau_coord;

enum {
    DRV_ERR_VAU_DEV_NOT_OPEN = DRV_ERR_VAU_BASE, /* tde device not open yet */
    DRV_ERR_VAU_DEV_OPEN_FAILED,                /* open tde device failed */
    DRV_ERR_VAU_NULL_PTR,                       /* input parameters contain null ptr */
    DRV_ERR_VAU_NO_MEM,                         /* malloc failed  */
    DRV_ERR_VAU_INVALID_HANDLE,                 /* invalid job handle */
    DRV_ERR_VAU_INVALID_PARA,                   /* invalid parameter */
    DRV_ERR_VAU_NOT_ALIGNED,                    /* aligned error for position, stride, width */
    DRV_ERR_VAU_MINIFICATION,                   /* invalid minification */
    DRV_ERR_VAU_CLIP_AREA,                      /* clip area and operation area have no intersection */
    DRV_ERR_VAU_UNSUPPORTED_OPERATION,          /* unsupported operation */
    DRV_ERR_VAU_QUERY_TIMEOUT,                  /* query time out */
    DRV_ERR_VAU_INTERRUPT,                      /* blocked job was interrupted */
    DRV_ERR_VAU_INVLIAD_RECT,
    DRV_ERR_VAU_INVLIAD_FMT,
    DRV_ERR_VAU_INVLIAD_RESO,
    DRV_ERR_VAU_WAIT_TIMEOUT,                   /* blocked job wait timeout */
    DRV_ERR_VAU_INT_ERR,                        /* int error */

    DRV_ERR_VAU_MAX
};

typedef enum {
    DRV_VAU_EVEN_ODD_FILL = 0,
    DRV_VAU_NON_ZERO_FILL,
    DRV_VAU_FILL_MAX
} drv_vau_fill_type;

typedef enum {
    DRV_VAU_PAINT_TYPE_COLOR = 0,
    DRV_VAU_PAINT_TYPE_COLOR_ALPHA_FF,
    DRV_VAU_PAINT_TYPE_LINEAR_GRADIENT,
    DRV_VAU_PAINT_TYPE_RADIAL_GRADIENT,
    DRV_VAU_PAINT_TYPE_SWEEP_GRADIENT,
    DRV_VAU_PAINT_TYPE_PATTERN,
    DRV_VAU_PAINT_TYPE_MAX
} drv_vau_paint_type;

typedef enum {
    DRV_VAU_PAINT_TILE_MODE_CLAMP = 0,
    DRV_VAU_PAINT_TILE_MODE_REPEAT,
    DRV_VAU_PAINT_TILE_MODE_MIRROR,
    DRV_VAU_PAINT_TILE_MODE_FILL, // for matrix(pattern) only
    DRV_VAU_PAINT_TILE_MODE_NONE, // for matrix only
    DRV_VAU_PAINT_TILE_MODE_MAX
} drv_vau_paint_tile_mode;

typedef enum {
    LINEAR_POINT,
    LINEAR_VERTICAL,
    LINEAR_HORIZONTAL,
    LINEAR_SLASH,
} drv_vau_linear_mode;

typedef td_void (*drv_vau_finish_callback)(const td_void *para1);
typedef td_void (*drv_vau_lowdelay_callback)(const td_void *para1);

typedef struct {
    const td_void *param;
    drv_vau_finish_callback finish_callback;
    td_bool is_lowdelay;
    drv_vau_lowdelay_callback lowdelay_callback;
} drv_vau_callback_func;

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
} drv_vau_alpha_info;

typedef enum {
    DRV_VAU_REFLECT_MODE_X = 0x1,
    DRV_VAU_REFLECT_MODE_Y = 0x2,
    DRV_VAU_REFLECT_MODE_XY = 0x4,
} drv_vau_reflect_mode;

typedef struct {
    td_float x;
    td_float y;
} drv_vau_affine_translate_attr;

typedef struct {
    td_float x_multiplier;
    td_float y_multiplier;
} drv_vau_affine_zoom_attr;

typedef struct {
    td_s32 degree;
} drv_vau_affine_rotate_attr;

typedef struct {
    drv_vau_reflect_mode mode;
} drv_vau_affine_reflect_attr;

typedef struct {
    td_float x;
    td_float y;
} drv_vau_affine_shearing_attr;

typedef enum {
    DRV_VAU_SRC_1 = 0,
    DRV_VAU_SRC_2,
    DRV_VAU_DST,
} drv_vau_src_no;

typedef enum {
    DRV_VAU_AFFINE_MODE_TRANSLATE,
    DRV_VAU_AFFINE_MODE_ZOOM,
    DRV_VAU_AFFINE_MODE_ROTATE,
    DRV_VAU_AFFINE_MODE_REFLECT,
    DRV_VAU_AFFINE_MODE_SHEARING,

    DRV_VAU_AFFINE_MODE_USE_COEF,
} drv_vau_affine_mode;

typedef td_float drv_vau_matrix[3][3];

typedef struct {
    td_bool affine_en;
    drv_vau_affine_mode mode;
    drv_vau_affine_translate_attr translate;
    drv_vau_affine_zoom_attr zoom;
    drv_vau_affine_rotate_attr rotate;
    drv_vau_affine_reflect_attr reflect;
    drv_vau_affine_shearing_attr shearing;
    drv_vau_matrix matrix;
} drv_vau_affine_attr;

typedef enum {
    DRV_VAU_CROP_INSIDE,
    DRV_VAU_CROP_OUTSIDE,
    DRV_VAU_CROP_MAX
} drv_vau_crop_mode;

typedef struct {
    td_bool crop_en;
    drv_vau_crop_mode crop_mode;
    ext_rect crop_rect;
} drv_vau_crop_attr;

typedef enum {
    DRV_VAU_TRANS_FILLMODE_ALL0,
    DRV_VAU_TRANS_FILLMODE_SRC1,
    DRV_VAU_TRANS_FILLMODE_MAX
} drv_vau_trans_fill_mode;

typedef struct {
    td_bool enable;
    drv_vau_trans_fill_mode fill_mode;
    td_s32 x;
    td_s32 y;
    td_u32 bkg_width;
    td_u32 bkg_height;
} drv_vau_trans_attr;

typedef enum {
    DRV_VAU_NONE_MSAA,
    DRV_VAU_4X_MSAA,
    DRV_VAU_4X_CFG_MSAA,
    DRV_VAU_8X_MSAA,
    DRV_VAU_8X_CFG_MSAA,
} drv_vau_sample_mode;

typedef struct {
    td_float x;
    td_float y;
    td_u8 weight;
} drv_vau_sample;

typedef struct {
    drv_vau_sample samples[DRV_VAU_MAX_SAMPLES];
    td_float sample_radius;
} drv_vau_sample_attr;

typedef struct {
    td_float err_quad;
    td_float err_cubic;
} drv_vau_bezier_err;

typedef enum {
    DRV_VAU_CFG_BBOX_DYNAMIC,
    DRV_VAU_CFG_BBOX_FIXED,
} drv_vau_cfg_bbox_mode;

typedef struct {
    td_u16 bbox_minx;
    td_u16 bbox_maxx;
    td_u16 bbox_miny;
    td_u16 bbox_maxy;
    drv_vau_cfg_bbox_mode cfg_bbox_mode;
} drv_vau_cfg_bbox_attr;

typedef struct {
    td_float const_x;
    td_float const_y;
    drv_vau_linear_mode mode;
} drv_linear_auxiliary_value;

typedef struct {
    td_float const_r;
} drv_radial_auxiliary_value;

typedef struct {
    td_float const_pi; // fixed value
    td_float const_ratio; // fixed value
} drv_sweep_auxiliay_value;

typedef struct {
    union {
        drv_linear_auxiliary_value linear;
        drv_radial_auxiliary_value radial;
        drv_sweep_auxiliay_value sweep;
    };

    td_float stop_reciprocal[DRV_VAU_MAX_STOPS - 1]; // max 7 stops
} drv_auxiliary_calc_value;

typedef struct {
    drv_vau_sample_mode sample_mode;
    drv_vau_sample_attr sample_attr;
    drv_vau_bezier_err bezier_err;
    drv_vau_cfg_bbox_attr cfg_bbox_attr; /* cfg_bbox: value & mode */
    drv_auxiliary_calc_value auxiliary_value;
} drv_vau_render_cfg;

typedef struct {
    drv_vau_coord x;
    drv_vau_coord y;
} drv_vau_point;

typedef struct {
    drv_vau_color colors[DRV_VAU_MAX_STOPS]; /* include alpha */
    td_u8 offsets[DRV_VAU_MAX_STOPS];
    td_u8 num;
} drv_vau_stops;

typedef union {
    struct {
        td_u32 rgb : 24;
        td_u32 opa : 8;
    };
    td_u32 color;
} drv_vau_solid_attr;

typedef struct {
    drv_vau_point start_point;
    drv_vau_point end_point;
} drv_vau_linear_gradient_attr;

typedef struct {
    td_u16 radius;
    drv_vau_point center;
} drv_vau_radial_gradient_attr;

typedef struct {
    drv_vau_point center;
    td_u32 start_angle;
} drv_vau_sweep_gradient_attr;

typedef struct {
    drv_vau_paint_tile_mode tile_mode;
    drv_vau_stops stops;
    union {
        drv_vau_linear_gradient_attr linear;
        drv_vau_radial_gradient_attr radial;
        drv_vau_sweep_gradient_attr sweep;
    };
} drv_vau_gradient_attr;

typedef struct {
    drv_vau_paint_type paint_type;
    union {
        drv_vau_solid_attr solid;
        drv_vau_gradient_attr gradient;
    };
} drv_vau_paint_attr;

typedef struct {
    drv_vau_fill_type type;
    drv_vau_paint_attr paint_attr;
} drv_vau_fill_attr;

typedef enum {
    DRV_VAU_PAINT_MODE_NONE = 0x0,
    DRV_VAU_PAINT_MODE_STROKE = 0x1,
    DRV_VAU_PAINT_MODE_FILL = 0x2,
    DRV_VAU_PAINT_MODE_ALL = DRV_VAU_PAINT_MODE_STROKE | DRV_VAU_PAINT_MODE_FILL,
    DRV_VAU_PAINT_MODE_MAX
} drv_vau_paint_mode;

typedef struct {
    td_bool enable;
    td_uchar *cmds;
    td_u32 cmd_num;
    float *datas;
    td_u32 data_num;

    drv_vau_render_cfg render_cfg;
    drv_vau_fill_attr fill_attr;
    drv_vau_paint_mode paint_mode;
} drv_vau_vector_draw_attr;

typedef enum {
    DRV_VAU_ROP_BLACK = 0x0,
    DRV_VAU_ROP_NOTMERGEPEN,
    DRV_VAU_ROP_MASKNOTPEN,
    DRV_VAU_ROP_NOTCOPYPEN,
    DRV_VAU_ROP_MASKPENNOT,
    DRV_VAU_ROP_NOT,
    DRV_VAU_ROP_XORPEN,
    DRV_VAU_ROP_NOTMASKPEN,
    DRV_VAU_ROP_MASKPEN,
    DRV_VAU_ROP_NOTXORPEN,
    DRV_VAU_ROP_NOP,
    DRV_VAU_ROP_MERGENOTPEN,
    DRV_VAU_ROP_COPYPEN,
    DRV_VAU_ROP_MERGEPENNOT,
    DRV_VAU_ROP_MERGEPEN,
    DRV_VAU_ROP_WHITE,
    DRV_VAU_ROP_MAX
} drv_vau_rop_mode;

typedef struct {
    td_bool rop_enable;
    drv_vau_rop_mode alpha_rop_mode;
    drv_vau_rop_mode color_rop_mode;
} drv_vau_rop_opt;

typedef enum {
    DRV_VAU_COLORKEY_NONE = 0,
    DRV_VAU_COLORKEY_FOREGROUND,
    DRV_VAU_COLORKEY_BACKGROUND,
    DRV_VAU_COLORKEY_MAX
} drv_vau_colorkey_mode;

typedef struct {
    td_u16 component_min;
    td_u16 component_max;
    td_bool is_component_out;
    td_bool is_component_ignore;
    td_u16 component_mask;
} drv_vau_colorkey_component;

typedef union {
    struct {
        drv_vau_colorkey_component alpha;
        drv_vau_colorkey_component red;
        drv_vau_colorkey_component green;
        drv_vau_colorkey_component blue;
    } colorkey_argb;

    struct {
        drv_vau_colorkey_component alpha;
        drv_vau_colorkey_component clut;
    } colorkey_clut;
} drv_vau_colorkey_value;

typedef struct {
    td_bool colorkey_enable;
    drv_vau_colorkey_mode colorkey_mode;
    drv_vau_colorkey_value colorkey_value;
} drv_vau_colorkey_opt;

typedef enum {
    DRV_VAU_BLEND_ZERO = 0x0,
    DRV_VAU_BLEND_ONE,
    DRV_VAU_BLEND_SRC2COLOR,
    DRV_VAU_BLEND_INVSRC2COLOR,
    DRV_VAU_BLEND_SRC2ALPHA,
    DRV_VAU_BLEND_INVSRC2ALPHA,
    DRV_VAU_BLEND_SRC1COLOR,
    DRV_VAU_BLEND_INVSRC1COLOR,
    DRV_VAU_BLEND_SRC1ALPHA,
    DRV_VAU_BLEND_INVSRC1ALPHA,
    DRV_VAU_BLEND_SRC2ALPHASAT,
    DRV_VAU_BLEND_MAX
} drv_vau_blend_mode;

typedef enum {
    DRV_VAU_BLENDCMD_NONE = 0x0,
    DRV_VAU_BLENDCMD_CLEAR,
    DRV_VAU_BLENDCMD_SRC,
    DRV_VAU_BLENDCMD_SRCOVER,
    DRV_VAU_BLENDCMD_DSTOVER,
    DRV_VAU_BLENDCMD_SRCIN,
    DRV_VAU_BLENDCMD_DSTIN,
    DRV_VAU_BLENDCMD_SRCOUT,
    DRV_VAU_BLENDCMD_DSTOUT,
    DRV_VAU_BLENDCMD_SRCATOP,
    DRV_VAU_BLENDCMD_DSTATOP,
    DRV_VAU_BLENDCMD_ADD,
    DRV_VAU_BLENDCMD_XOR,
    DRV_VAU_BLENDCMD_DST,
    DRV_VAU_BLENDCMD_CONFIG,
    DRV_VAU_BLENDCMD_MAX
} drv_vau_blend_cmd;

typedef enum {
    DRV_VAU_OUTALPHA_FROM_NORM = 0,
    DRV_VAU_OUTALPHA_FROM_BACKGROUND,
    DRV_VAU_OUTALPHA_FROM_FOREGROUND,
    DRV_VAU_OUTALPHA_FROM_GLOBALALPHA,
    DRV_VAU_OUTALPHA_FROM_MAX
} drv_vau_out_alpha_mode;

typedef struct {
    td_bool blend_enable;
    drv_vau_blend_cmd blend_cmd;
    drv_vau_blend_mode background_blend_mode;
    drv_vau_blend_mode foreground_blend_mode;
    drv_vau_out_alpha_mode out_alpha_mode;
} drv_vau_blend_opt;

typedef enum {
    DRV_VAU_BLUR_MODE_GAUSSIAN = 0,
    DRV_VAU_BLUR_MODE_MAX
} drv_vau_blur_mode;

typedef struct {
    td_float x_sigma;
    td_float y_sigma;
    td_bool alpha_gaussian;
} drv_vau_gaussian_para;

typedef struct {
    td_bool blur_en;
    drv_vau_blur_mode blur_mode;
    drv_vau_gaussian_para gaussian_attr;
} drv_vau_blur_opt;

typedef enum {
    DRV_VAU_PATTERN_DISABLE = 0x0,
    DRV_VAU_PATTERN_FILL    = 0x1,
    DRV_VAU_PATTERN_PAD     = 0x2,
} drv_vau_pattern_mode;

typedef struct {
    drv_vau_pattern_mode mode;
    td_u32 argb;
} drv_vau_pattern_attr;

typedef struct {
    drv_vau_affine_attr affine;
    drv_vau_crop_attr crop;
    drv_vau_trans_attr trans;
    drv_vau_vector_draw_attr vector;
    drv_vau_pattern_attr pattern;
} drv_vau_draw_attr;

typedef struct {
    drv_vau_rop_opt rop_opt;
    drv_vau_colorkey_opt colorkey_opt;
    drv_vau_blend_opt blend_opt;
    drv_vau_blur_opt blur_opt;
} drv_vau_blit_opt;

typedef enum {
    DRV_VAU_ALU_MODE_SRC1_BYPASS,
    DRV_VAU_ALU_MODE_SRC2_BYPASS,
    DRV_VAU_ALU_MODE_ROP,
    DRV_VAU_ALU_MODE_BLEND,

    DRV_VAU_ALU_MODE_MAX
} drv_vau_alu_mode;

typedef struct {
    td_u32 tile_width;
    td_u32 tile_size;
    td_u32 alpha_bypass;
    td_u32 alpha_value;
    drv_gfx_compress_mode compress_mode;
} drv_vau_compress_info;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    td_u32 size;
    td_u32 addr;
    ext_rect rect;
    ext_rect out_rect;
    drv_gfx_fmt fmt;
    drv_gfx_argb_order argb_order;
    drv_gfx_alpha_value alpha_vaule;
    td_u32 color;
    td_u32 clut_addr;
    td_bool is_color_surface;
    drv_vau_compress_info compress_info;
    drv_vau_alpha_info alpha_info;
    drv_vau_draw_attr attr; /* only foreground surface support recently */
} drv_vau_surface;

typedef struct {
    td_u32 src_surface_cnt;
    drv_vau_surface *src_surface;
    drv_vau_surface *dst_surface;
    drv_vau_blit_opt *opt;
} drv_vau_surface_list;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_VAU_TYPE_H */
