/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: mem manage
 */

#ifndef GPU_DRV_VAU_MEM_H
#define GPU_DRV_VAU_MEM_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define malloc_cmd(size)   vau_mem_alloc(VAU_MEM_TYPE_PATH_CMD, size)
#define malloc_data(size)  vau_mem_alloc(VAU_MEM_TYPE_PATH_DATA, size)
#define free_cmd(ptr)      vau_mem_free(VAU_MEM_TYPE_PATH_CMD, ptr)
#define free_data(ptr)     vau_mem_free(VAU_MEM_TYPE_PATH_DATA, ptr)

/*-------------------------------- struct define ------------------------------------------*/

typedef enum {
    VAU_MEM_TYPE_NODE = 0,
    VAU_MEM_TYPE_NODE_EXT,

    VAU_MEM_TYPE_HANDLE_MNG,

    VAU_MEM_TYPE_PATH_CMD,
    VAU_MEM_TYPE_PATH_DATA,

    VAU_MEM_TYPE_TESSLATION_ET,
    VAU_MEM_TYPE_TESSLATION_LINK,
    VAU_MEM_TYPE_TESSLATION_LINK_EXT,

    VAU_MEM_TYPE_BUTT
} vau_mem_type;

/*-------------------------------- func declares ------------------------------------------*/

td_s32 vau_mem_init(td_void);
td_void vau_mem_deinit(td_void);

td_s32 vau_mem_draw_pool_init(td_void);
td_s32 vau_mem_draw_pool_deinit(td_void);

td_uchar *vau_mem_alloc(vau_mem_type mem_type, td_u32 size);
td_uchar *vau_mem_realloc(vau_mem_type mem_type, const td_uchar *vir_addr, td_u32 new_size);
td_void vau_mem_free(vau_mem_type mem_type, const td_uchar *vir_addr);

td_uchar *vau_mem_map_to_viraddr(vau_mem_type mem_type, td_u32 phy_addr);
td_u32 vau_mem_map_to_phyaddr(vau_mem_type mem_type, const td_uchar *vir_addr);
td_s32 vau_mem_round_copy(vau_mem_type mem_type, td_uchar *dst, td_u32 dst_size,
                          const td_uchar *src, td_u32 src_size);

td_u32 vau_mem_free_count(vau_mem_type mem_type);
td_void vau_mem_ref_inc(vau_mem_type mem_type, const td_uchar *vir_addr);
td_void vau_mem_ref_dec(vau_mem_type mem_type, const td_uchar *vir_addr);

td_bool vau_mem_is_internal(vau_mem_type mem_type, const td_uchar *vir_addr);
td_void vau_mem_get_range(vau_mem_type mem_type, td_u32 *phy_start, td_u32 *phy_end);

/* mocker by ut testcase */
td_u32 vau_mem_get_sram_addr(td_void);
td_u32 vau_mem_get_psram_addr(td_void);
td_void vau_mem_check_psram_addr(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_DRV_VAU_MEM_H */
