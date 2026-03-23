/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau adp vector draw file
 */

#include "vau_dfx.h"
#include "drv_vau_type.h"
#include "vau_mem.h"
#include "vau_hal_node.h"
#include "vau_hal_vector_draw.h"

static td_u32 g_vau_draw_et_addr;
static td_u32 g_vau_draw_et_link_addr;
static td_u32 g_vau_draw_et_link_end;
static td_u32 g_vau_draw_et_ext_addr;
static td_u32 g_vau_draw_et_ext_end;

td_void vau_hal_vector_draw_mem_init(td_void)
{
    td_u32 start = 0;
    td_u32 end = 0;
    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_ET, &start, &end);
    g_vau_draw_et_addr = start;
    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_LINK, &start, &end);
    g_vau_draw_et_link_addr = start;
    g_vau_draw_et_link_end = end - 1;
    vau_mem_get_range(VAU_MEM_TYPE_TESSLATION_LINK_EXT, &start, &end);
    g_vau_draw_et_ext_addr = start;
    g_vau_draw_et_ext_end = end - 1;
}

td_void vau_hal_vector_draw_mem_deinit(td_void)
{
    g_vau_draw_et_addr = 0;
    g_vau_draw_et_link_addr = 0;
    g_vau_draw_et_ext_addr = 0;
    g_vau_draw_et_link_end = 0;
    g_vau_draw_et_ext_end = 0;
}

static td_void vau_hal_vector_draw_gradient_color_config(hal_vau_vector_cfg *hal_cfg, const drv_vau_stops *stops,
    const drv_auxiliary_calc_value *aux)
{
    td_s32 ret;
    td_float reci;

    hal_cfg->stops_offset0 =  stops->offsets[0] +
    (stops->offsets[1] << 8) + (stops->offsets[2] << 16) + (stops->offsets[3] << 24); /* 8 2 16 3 24: index */
    hal_cfg->stops_offset1 =  stops->offsets[4] +                                     /* 4: index */
    (stops->offsets[5] << 8) + (stops->offsets[6] << 16);                             /* 5 8 6 16: index */
    if (stops->num > 0) {
        ret = memcpy_s(hal_cfg->stops_color, sizeof(hal_cfg->stops_color), stops->colors,
                 stops->num * sizeof(drv_vau_color));
        if (ret != EOK) {
            vau_err("memcpy_s failure! ret = %d\n", ret);
            return;
        }
        ret = memcpy_s(hal_cfg->color_reci, sizeof(hal_cfg->color_reci), aux->stop_reciprocal,
            sizeof(aux->stop_reciprocal));
        if (ret != EOK) {
            vau_err("memcpy_s failure! ret = %d\n", ret);
            return;
        }
        /* finally vg shoule give the reci value */
        for (td_s32 i = 0; i < stops->num - 1; i++) {
            if (stops->offsets[i +  1] != stops->offsets[i]) {
                // 256:index
                reci = (td_float)(1.0 / (td_float)((stops->offsets[i +  1] - stops->offsets[i]) * 256));
                hal_cfg->color_reci[i] = (td_u32)reci;
            } else {
                hal_cfg->color_reci[i] = 0;
            }
        }
    }

    hal_cfg->stops_num = stops->num;
}

static td_void vau_hal_vector_draw_gradient_config(hal_vau_vector_cfg *hal_cfg, const drv_vau_paint_attr *paint,
    const drv_auxiliary_calc_value *aux)
{
    const drv_vau_stops *stops = NULL;

    hal_cfg->fill_effect = (hal_vau_fill_effect)paint->paint_type;
    if ((hal_cfg->fill_effect == HAL_VAU_FILL_EFFECT_PURE_COLOR) ||
        (hal_cfg->fill_effect == HAL_VAU_FILL_EFFECT_ALPHA_FF)) {
        hal_cfg->gradient.color.typical_color = paint->solid.color;
    } else if (hal_cfg->fill_effect == HAL_VAU_FILL_EFFECT_LINEAR_GRADIENT) {
        hal_cfg->gradient.linear.linear_const_x = aux->linear.const_x;
        hal_cfg->gradient.linear.linear_const_y = aux->linear.const_y;
        hal_cfg->gradient.linear.linear_const_mode = aux->linear.mode;
        hal_cfg->gradient.linear.linear_start_x = (td_u32)paint->gradient.linear.start_point.x;
        hal_cfg->gradient.linear.linear_start_y = (td_u32)paint->gradient.linear.start_point.y;
        hal_cfg->tile_mode = paint->gradient.tile_mode;
        stops = &paint->gradient.stops;
    } else if (hal_cfg->fill_effect == HAL_VAU_FILL_EFFECT_RADIAL_GRADIENT) {
        hal_cfg->gradient.radial.radial_center_x = (td_u32)paint->gradient.radial.center.x;
        hal_cfg->gradient.radial.radial_center_y = (td_u32)paint->gradient.radial.center.y;
        hal_cfg->gradient.radial.radial_const_r = aux->radial.const_r;
        hal_cfg->tile_mode = paint->gradient.tile_mode;
        stops = &paint->gradient.stops;
    } else if (hal_cfg->fill_effect == HAL_VAU_FILL_EFFECT_SWEEP_GRADIENT) {
        hal_cfg->gradient.sweep.sweep_center_x = (td_u32)paint->gradient.sweep.center.x;
        hal_cfg->gradient.sweep.sweep_center_y = (td_u32)paint->gradient.sweep.center.y;
        hal_cfg->gradient.sweep.sweep_start_angle = paint->gradient.sweep.start_angle;
        hal_cfg->tile_mode = paint->gradient.tile_mode;
        stops = &paint->gradient.stops;
    }
    if (stops != NULL) {
        vau_hal_vector_draw_gradient_color_config(hal_cfg, stops, aux);
    } else {
        hal_cfg->stops_num = 0;
    }

    return;
}

static td_void vau_hal_vector_draw_cmddata_config(hal_vau_vector_cfg *hal_cfg,
                                                  const drv_vau_vector_draw_attr *vector)
{
    td_bool inner_cmd;
    td_bool inner_data;
    td_u32 start, end;

    vau_mem_get_range(VAU_MEM_TYPE_PATH_CMD, &start, &end);
    inner_cmd = (((td_u32)(uintptr_t)vector->cmds) <= end) && (((td_u32)(uintptr_t)vector->cmds) >= start);

    vau_mem_get_range(VAU_MEM_TYPE_PATH_DATA, &start, &end);
    inner_data = (((td_u32)(uintptr_t)vector->datas) <= end) && (((td_u32)(uintptr_t)vector->datas) >= start);

#ifdef CONFIG_VAU_DFX_DEBUG
    hal_cfg->dyn_bdbx_en = vau_dfx_check_mask(VAU_DBG_MASK_DRAW_BBOX_DISABLE) ? 0x0 : 0x1;
    hal_cfg->cmd_return_en = vau_dfx_check_mask(VAU_DBG_MASK_DRAW_CMD_RTN_DISABLE) ? 0x0 : inner_cmd;
    hal_cfg->path_return_en = vau_dfx_check_mask(VAU_DBG_MASK_DRAW_PATH_RTN_DISABLE) ? 0x0 : inner_data;
#else
    hal_cfg->dyn_bdbx_en = 1;
    hal_cfg->cmd_return_en = inner_cmd;
    hal_cfg->path_return_en = inner_data;
#endif

    hal_cfg->draw_path_data_addr = (td_u32)(uintptr_t)vector->datas;
    hal_cfg->draw_path_data_cnt = vector->data_num;
    hal_cfg->draw_path_cmd_addr = (td_u32)(uintptr_t)vector->cmds;
    hal_cfg->draw_path_cmd_cnt = vector->cmd_num;

    if (inner_cmd) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_CMD, vector->cmds);
    }

    if (inner_data) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)vector->datas);
    }
    return;
}

td_void vau_hal_vector_draw_config(vau_hardware_node *node, const drv_vau_surface *src_surface,
                                   const drv_vau_surface *dst_surface)
{
    hal_vau_vector_cfg hal_cfg = {0};
    const drv_vau_draw_attr *attr = &src_surface->attr;
    const drv_vau_vector_draw_attr *vector = &(attr->vector);
    const drv_vau_paint_attr *paint = &(vector->fill_attr.paint_attr);
    const drv_auxiliary_calc_value *aux = &(vector->render_cfg.auxiliary_value);

    hal_cfg.draw_en = vector->enable;
    hal_cfg.fill_mode = (hal_vau_fill_mode)vector->fill_attr.type;
    hal_cfg.msaa_mode = (hal_vau_msaa_mode)vector->render_cfg.sample_mode;
    hal_cfg.draw_start_x = (td_u32)dst_surface->rect.x;
    hal_cfg.draw_start_y = (td_u32)dst_surface->rect.y;
    hal_cfg.draw_end_x = dst_surface->rect.width + dst_surface->rect.x - 1;
    hal_cfg.draw_end_y = dst_surface->rect.height + dst_surface->rect.y - 1;

    if (memcpy_s(&hal_cfg.matrix, sizeof(hal_cfg.matrix),
                 &attr->affine.matrix, sizeof(attr->affine.matrix)) != EOK) {
        return;
    }

    vau_hal_vector_draw_cmddata_config(&hal_cfg, vector);
    vau_hal_vector_draw_gradient_config(&hal_cfg, paint, aux);

    vau_hal_node_set_vector_info(node, &hal_cfg);
    vau_hal_set_et_buffer(node);
    return;
}

td_void vau_hal_set_et_buffer(vau_hardware_node *node)
{
    node->vau_draw_et_addr.u32 = g_vau_draw_et_addr;
    node->vau_draw_et_bk_addr.u32 = g_vau_draw_et_ext_addr;
    node->vau_draw_et_bkend_addr.u32 = g_vau_draw_et_ext_end;
    node->vau_draw_link_start_addr.u32 = g_vau_draw_et_link_addr;
    node->vau_draw_link_end_addr.u32 = g_vau_draw_et_link_end;
}
