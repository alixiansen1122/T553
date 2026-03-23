/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: mem manage
 */

#include "vau_mem.h"
#include "soc_osal.h"
#include "vau_list.h"
#include "vau_handle.h"
#include "vau_dfx.h"
#include "gpu_mem_config.h"
#include "pmp_config.h"

/*-------------------------------- macro define -------------------------------------------*/
#define MEM_DRAW_POOL  GPU_SRAM_DRAW_ADDR
#define MEM_DRAW_LEN   GPU_SRAM_DRAW_SIZE

#define MEM_SRAM_ADDR  GPU_SRAM_NODE_ADDR
#define MEM_SRAM_LEN   GPU_SRAM_NODE_SIZE

#define VAU_PSRAM_MAGIC_VAL  0xEE20240601BB00AA

#define MEM_PSRAM_ADDR (GPU_PSRAM_ADDR)
#define MEM_PSRAM_LEN  (GPU_PSRAM_SIZE)

#define MEM_NODE_ALIGN                   16
#define MEM_HANDLE_ALIGN                 16
#define MEM_PATH_CMD_ALIGN               1
#define MEM_PATH_DATA_ALIGN              16
#define MEM_TESSLATION_ALIGN             64

#define MEM_NODE_MAX_NUM                 48
#define MEM_HANDLE_MAX_NUM               10
#define MEM_PATH_CMD_MAX_NUM             48
#define MEM_PATH_DATA_MAX_NUM            48
#define MEM_TESSLATION_ET_MAX_NUM        1
#define MEM_TESSLATION_LINK_MAX_NUM      1

#ifdef CONFIG_PSRAM_SUPPORT
#define MEM_NODE_EXT_MAX_NUM             592
#define MEM_TESSLATION_LINK_EXT_MAX_NUM  1
#else
#define MEM_NODE_EXT_MAX_NUM             0
#define MEM_TESSLATION_LINK_EXT_MAX_NUM  0
#endif

#define MEM_PATH_CMD_UNIT_SIZE          MEM_DRAW_LEN
#define MEM_PATH_DATA_UNIT_SIZE         MEM_DRAW_LEN
#define MEM_TESSLATION_ET_UNIT_SIZE     (64 * 600)

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    struct osal_list_head list_head;
    td_bool is_sram;
    td_uchar *vir_addr;
    td_u32 size;
    td_u32 ref_cnt;
} mem_node;

typedef struct {
    td_bool is_sram;
    td_bool is_draw_mem;
    td_bool is_dynamic_size;
    td_bool is_mem_lock_init;
    td_uchar *vir_addr;
    td_u32 phy_addr;
    td_u32 total_size;
    td_u32 unit_num;
    td_u32 unit_size;
    mem_node *node;
    osal_spinlock mem_lock;
} mem_info;

/*-------------------------------- func declares ------------------------------------------*/
static mem_node *mem_node_search(vau_mem_type mem_type, const td_uchar *vir_addr);

static td_s32 mem_info_init(td_void);
static td_void mem_list_init(td_void);

static td_void mem_info_deinit(td_void);
static td_void mem_list_deinit(td_void);

/*-------------------------------- par define ---------------------------------------------*/

static mem_info g_mem_info[VAU_MEM_TYPE_BUTT] = {
    /* VAU_MEM_TYPE_NODE */
    { .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_MAX_NUM,
      .unit_size       = uapi_byte_align(sizeof(vau_sw_node), MEM_NODE_ALIGN)
    },
    /* VAU_MEM_TYPE_NODE_EXT */
    { .is_sram         = TD_FALSE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_EXT_MAX_NUM,
      .unit_size       = uapi_byte_align(sizeof(vau_sw_node), MEM_NODE_ALIGN)
    },
    /* VAU_MEM_TYPE_HANDLE_MNG */
    { .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_HANDLE_MAX_NUM,
      .unit_size       = uapi_byte_align(sizeof(vau_handle_mgr) + sizeof(vau_sw_job), MEM_HANDLE_ALIGN)
    },
    /* VAU_MEM_TYPE_PATH_CMD */
    { .is_sram         = TD_TRUE,
      .is_draw_mem     = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_PATH_CMD_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_PATH_CMD_UNIT_SIZE, MEM_PATH_CMD_ALIGN)
    },
    /* VAU_MEM_TYPE_PATH_DATA */
    { .is_sram         = TD_TRUE,
      .is_draw_mem     = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_PATH_DATA_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_PATH_DATA_UNIT_SIZE, MEM_PATH_DATA_ALIGN)
    },
    /* VAU_MEM_TYPE_TESSLATION_ET */
    { .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_TESSLATION_ET_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_TESSLATION_ET_UNIT_SIZE, MEM_TESSLATION_ALIGN)
    },
    /* VAU_MEM_TYPE_TESSLATION_LINK */
    { .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_TRUE, /* use sram remaining mem */
      .unit_num        = MEM_TESSLATION_LINK_MAX_NUM,
      .unit_size       = 0
    },
    /* VAU_MEM_TYPE_TESSLATION_LINK_EXT */
    { .is_sram         = TD_FALSE,
      .is_dynamic_size = TD_TRUE, /* use psram remaining mem */
      .unit_num        = MEM_TESSLATION_LINK_EXT_MAX_NUM,
      .unit_size       = 0
    },
};

static td_bool g_mem_init_flag = TD_FALSE;
static struct osal_list_head g_mem_busy_list_head[VAU_MEM_TYPE_BUTT] = {0};
static struct osal_list_head g_mem_free_list_head[VAU_MEM_TYPE_BUTT] = {0};

/*-------------------------------- func release -------------------------------------------*/

/* mocker by ut testcase */
td_u32 vau_mem_get_sram_addr(td_void)
{
    return MEM_SRAM_ADDR;
}

td_u32 vau_mem_get_psram_addr(td_void)
{
#ifdef CONFIG_PSRAM_SUPPORT
    td_u64 *psram_protect_addr = (td_u64 *)(uintptr_t)PSRAM_DISP_VAU_RESERVE_ADDR;
    *psram_protect_addr = VAU_PSRAM_MAGIC_VAL; // PSRAM PMP保护区写入魔术字
    pmp_gpu_psram_init(); // 启动pmp保护
#endif
    return MEM_PSRAM_ADDR;
}

td_void vau_mem_check_psram_addr(td_void)
{
#ifdef CONFIG_PSRAM_SUPPORT
    td_u64 *psram_protect_addr = (td_u64 *)(uintptr_t)PSRAM_DISP_VAU_RESERVE_ADDR;
    if (*psram_protect_addr != VAU_PSRAM_MAGIC_VAL) {
        vau_err("psram mem corruption! curr val = 0x%llX, exp val = 0x%llx", *psram_protect_addr, VAU_PSRAM_MAGIC_VAL);
    }
#endif
}

td_s32 vau_mem_draw_pool_init(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_init((td_void *)MEM_DRAW_POOL, MEM_DRAW_LEN);
    if (ret != OSAL_SUCCESS) {
        vau_err("osal_pool_mem_init failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vau_mem_draw_pool_deinit(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_deinit((td_void *)MEM_DRAW_POOL);
    if (ret != OSAL_SUCCESS) {
        vau_err("osal_pool_mem_deinit failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_uchar *mem_draw_alloc(td_u32 size)
{
    return (td_uchar *)osal_pool_mem_alloc_align((td_void *)MEM_DRAW_POOL, size, MEM_PATH_DATA_ALIGN);
}

static td_void mem_draw_free(const td_uchar *vir_addr)
{
    osal_pool_mem_free((td_void *)MEM_DRAW_POOL, (td_void *)vir_addr);
    return;
}

/* insmod driver ko or system init for once */
td_s32 vau_mem_init(td_void)
{
    td_s32 ret;

    if (g_mem_init_flag == TD_TRUE) {
        return EXT_SUCCESS;
    }

    ret = vau_mem_draw_pool_init();
    if (ret != EXT_SUCCESS) {
        vau_err("call vau_mem_draw_pool_init failed");
        return ret;
    }

    ret = mem_info_init();
    if (ret != EXT_SUCCESS) {
        vau_mem_draw_pool_deinit();
        mem_info_deinit();
        vau_err("call mem_info_init failed");
        return ret;
    }

    mem_list_init();

    g_mem_init_flag = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 mem_info_init(td_void)
{
    mem_info *info;
    td_u32 addr_start;
    td_u32 free_size;
    td_u32 sram_free_size  = MEM_SRAM_LEN;
    td_u32 psram_free_size = MEM_PSRAM_LEN;

    td_u32 sram_start  = vau_mem_get_sram_addr();
    td_u32 psram_start = vau_mem_get_psram_addr();

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        info = &g_mem_info[mem_type];
        free_size = (info->is_sram == TD_TRUE) ? sram_free_size : psram_free_size;

        if (info->is_dynamic_size == TD_TRUE) {
            info->unit_size = free_size;
        }

        if (info->is_draw_mem == TD_TRUE) {
            info->total_size = MEM_DRAW_LEN;
            info->phy_addr   = MEM_DRAW_POOL;
            info->vir_addr   = (td_uchar *)(uintptr_t)info->phy_addr;
        } else {
            info->total_size = info->unit_size * info->unit_num;
            if (free_size < info->total_size) {
                vau_err("not enough mem, free_size[%u] need_size[%u]", free_size, info->total_size);
                return EXT_FAILURE;
            }

            if (info->is_sram == TD_TRUE) {
                addr_start = sram_start + (MEM_SRAM_LEN - free_size);
                sram_free_size = sram_free_size - info->total_size;
            } else {
                addr_start = psram_start + (MEM_PSRAM_LEN - free_size);
                psram_free_size = psram_free_size - info->total_size;
            }
            info->phy_addr = addr_start;
            info->vir_addr = (td_uchar *)(uintptr_t)info->phy_addr;
        }

        osal_spin_lock_init(&(info->mem_lock));
        info->is_mem_lock_init = TD_TRUE;

        if (info->unit_num != 0) {
            info->node = (mem_node *)osal_kmalloc(info->unit_num * sizeof(mem_node), OSAL_GFP_KERNEL);
            if (info->node == TD_NULL) {
                vau_err("osal_kmalloc mem node memory failed");
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

static td_void mem_list_init(td_void)
{
    mem_info *info = TD_NULL;

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        OSAL_INIT_LIST_HEAD(&g_mem_busy_list_head[mem_type]);
        OSAL_INIT_LIST_HEAD(&g_mem_free_list_head[mem_type]);

        info = &g_mem_info[mem_type];
        for (td_u32 i = 0; i < info->unit_num; i++) {
            info->node[i].is_sram  = info->is_sram;
            info->node[i].vir_addr = (td_uchar *)((uintptr_t)info->vir_addr + i * info->unit_size);
            info->node[i].size     = info->unit_size;
            info->node[i].ref_cnt  = 0;
            /* 1 -> 2 -> 3 -> N */
            osal_list_add_tail(&(info->node[i].list_head), &g_mem_free_list_head[mem_type]);
        }
    }
    /* merge list */
    osal_list_splice_tail(&g_mem_free_list_head[VAU_MEM_TYPE_NODE_EXT],
                          &g_mem_free_list_head[VAU_MEM_TYPE_NODE]);
}

/* rmmod driver ko or system deinit for once */
td_void vau_mem_deinit(td_void)
{
    if (g_mem_init_flag == TD_FALSE) {
        vau_err("vau_mem_deinit has been deinit");
        return;
    }

    mem_list_deinit();
    mem_info_deinit();
    vau_mem_draw_pool_deinit();
    g_mem_init_flag = TD_FALSE;

    return;
}

static td_void mem_list_deinit(td_void)
{
    mem_node *tmp  = TD_NULL;
    mem_node *node = TD_NULL;

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        /* 1 -> 2 -> 3 -> N */
        osal_list_for_each_entry_safe(node, tmp, &g_mem_free_list_head[mem_type], list_head) {
            if (node != TD_NULL) {
                osal_list_del_init(&(node->list_head));
            }
        }
        /* 1 -> 2 -> 3 -> N */
        osal_list_for_each_entry_safe(node, tmp, &g_mem_busy_list_head[mem_type], list_head) {
            if (node != TD_NULL) {
                osal_list_del_init(&(node->list_head));
            }
        }
    }
}

static td_void mem_info_deinit(td_void)
{
    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        if (g_mem_info[mem_type].node != TD_NULL) {
            osal_kfree(g_mem_info[mem_type].node);
            g_mem_info[mem_type].node = TD_NULL;
        }

        if (g_mem_info[mem_type].is_mem_lock_init == TD_TRUE) {
            osal_spin_lock_destroy(&(g_mem_info[mem_type].mem_lock));
            g_mem_info[mem_type].is_mem_lock_init = TD_FALSE;
        }

        g_mem_info[mem_type].vir_addr   = TD_NULL;
        g_mem_info[mem_type].phy_addr   = 0;
        g_mem_info[mem_type].total_size = 0;
    }
}

td_uchar *vau_mem_alloc(vau_mem_type mem_type, td_u32 size)
{
    mem_node *node = TD_NULL;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);

    if (osal_list_empty(&g_mem_free_list_head[mem_type]) != 0) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        vau_err("mem_free_list[%d] is empty!", mem_type);
        return TD_NULL;
    }

    node = osal_list_entry((&g_mem_free_list_head[mem_type])->next, mem_node, list_head); /* 1 -> 2 -> 3 -> N */
    if ((node == TD_NULL) || (size > node->size)) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        vau_err("get free list node fail, type[%u], size[%u]!", (td_u32)mem_type, size);
        return TD_NULL;
    }

    if (g_mem_info[mem_type].is_draw_mem == TD_TRUE) {
        node->vir_addr = mem_draw_alloc(size);
        if (node->vir_addr == TD_NULL) {
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            vau_err("mem_draw_alloc failed, type[%u], size[%u]!", (td_u32)mem_type, size);
            return TD_NULL;
        }
    }

    osal_list_del_init(&(node->list_head));
    node->ref_cnt = 1;
    osal_list_add_tail(&(node->list_head), &g_mem_busy_list_head[mem_type]);

    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return node->vir_addr;
}

td_uchar *vau_mem_realloc(vau_mem_type mem_type, const td_uchar *vir_addr, td_u32 new_size)
{
    /* vg append new path should realloc mem */
    mem_node *node = TD_NULL;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);

    if (g_mem_info[mem_type].is_draw_mem != TD_TRUE) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        vau_err("only support cmd/data type realloc, type[%u]!", (td_u32)mem_type);
        return TD_NULL;
    }

    node = mem_node_search(mem_type, vir_addr);
    if (node == TD_NULL) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        vau_err("vau_mem_node_search failed!");
        return TD_NULL;
    }

    mem_draw_free(node->vir_addr);
    node->vir_addr = mem_draw_alloc(new_size);
    if (node->vir_addr == TD_NULL) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        vau_err("vau_mem_realloc failed, type[%u], size[%u]!", (td_u32)mem_type, new_size);
        return TD_NULL;
    }

    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return node->vir_addr;
}

td_void vau_mem_free(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);
    mem_node *node = mem_node_search(mem_type, vir_addr);

    if (node != TD_NULL) { /* 1 be used only once, so can free */
        if (node->ref_cnt > 1) { /* can not free mem */
            node->ref_cnt--;
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            return;
        }
        if (node->ref_cnt == 0) {
            vau_err("can not free node\n");
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            return;
        }

        if (g_mem_info[mem_type].is_draw_mem == TD_TRUE) {
            mem_draw_free(node->vir_addr);
        }

        node->ref_cnt = 0;
        osal_list_del_init(&(node->list_head));
        if (node->is_sram == TD_TRUE) { /* mostly malloc sram mem from pre to tail */
            osal_list_add(&(node->list_head), &g_mem_free_list_head[mem_type]);
        } else {
            osal_list_add_tail(&(node->list_head), &g_mem_free_list_head[mem_type]);
        }
    }
    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return;
}

td_uchar *vau_mem_map_to_viraddr(vau_mem_type mem_type, td_u32 phy_addr)
{
    return (td_uchar *)((uintptr_t)g_mem_info[mem_type].vir_addr + (phy_addr - g_mem_info[mem_type].phy_addr));
}

td_u32 vau_mem_map_to_phyaddr(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    return g_mem_info[mem_type].phy_addr + ((uintptr_t)vir_addr - (uintptr_t)g_mem_info[mem_type].vir_addr);
}

td_s32 vau_mem_round_copy(vau_mem_type mem_type, td_uchar *dst, td_u32 dst_size, const td_uchar *src, td_u32 src_size)
{
    errno_t ret;
    td_uchar *start   = (td_uchar *)(uintptr_t)g_mem_info[mem_type].vir_addr;
    td_uchar *end     = (td_uchar *)((uintptr_t)g_mem_info[mem_type].vir_addr + g_mem_info[mem_type].total_size);
    td_uchar *act_dst = dst;

    if ((mem_type != VAU_MEM_TYPE_PATH_DATA) && (mem_type != VAU_MEM_TYPE_PATH_CMD)) {
        vau_err("only path support rewind mem be used by vg append new path");
        return EXT_FAILURE;
    }

    act_dst = (act_dst >= end) ? (start + (act_dst - start) % g_mem_info[mem_type].total_size) : (act_dst);

    if (act_dst + src_size > end) {
        td_u32 one_size = end - act_dst;
        ret = memcpy_s(act_dst, one_size, src, one_size);
        if (ret != EOK) {
            vau_err("call memcpy_s failed\n");
            return EXT_FAILURE;
        }
        ret = memcpy_s(start, dst_size - one_size, (td_uchar *)src + one_size, src_size - one_size);
    } else {
        ret = memcpy_s(act_dst, dst_size, src, src_size);
    }
    if (ret != EOK) {
        vau_err("call memcpy_s failed\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_u32 vau_mem_free_count(vau_mem_type mem_type)
{
    td_u32 node_num = 0;
    mem_node *tmp = TD_NULL;
    mem_node *node = TD_NULL;

    /* count mem free list has node num */
    osal_list_for_each_entry_safe(node, tmp, &g_mem_free_list_head[mem_type], list_head) {
        node_num++;
    }

    return node_num;
}

td_void vau_mem_ref_inc(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);
    mem_node *node = mem_node_search(mem_type, vir_addr);
    if (node != TD_NULL) {
        node->ref_cnt++;
    }
    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
    return;
}

td_void vau_mem_ref_dec(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    vau_mem_free(mem_type, vir_addr);
}

static mem_node *mem_node_search(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    mem_node *tmp  = TD_NULL;
    mem_node *node = TD_NULL;

    osal_list_for_each_entry_safe(node, tmp, &g_mem_busy_list_head[mem_type], list_head) {
        if ((node != TD_NULL) && (vir_addr == node->vir_addr)) {
            return node;
        }
    }

    vau_err("search mem node failure, mem_type[%d]", mem_type);
    return TD_NULL;
}

td_bool vau_mem_is_internal(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    /* this mem is from user or from mem pool */
    if (((uintptr_t)vir_addr < (uintptr_t)g_mem_info[mem_type].vir_addr) ||
        ((uintptr_t)vir_addr > ((uintptr_t)g_mem_info[mem_type].vir_addr + g_mem_info[mem_type].total_size))) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

td_void vau_mem_get_range(vau_mem_type mem_type, td_u32 *phy_start, td_u32 *phy_end)
{
    *phy_start = g_mem_info[mem_type].phy_addr;
    *phy_end = *phy_start + g_mem_info[mem_type].total_size;
}
