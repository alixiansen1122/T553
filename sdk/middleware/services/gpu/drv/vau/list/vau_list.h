/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: osilist manage
 */
#ifndef DRV_GRAPHIC_VAU_LIST_VAU_LIST_H
#define DRV_GRAPHIC_VAU_LIST_VAU_LIST_H

#include "soc_osal.h"
#include "drv_vau_type.h"
#include "vau_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_VAU_TIMEOUT_DBG_ADDR (0x56011000 + 0x4000)
#define DRV_VAU_TIMEOUT_REG_SIZE 20 /* 20: debug reg from 0x4000 ~ 0x404C */

/* notified mode  after current node's job end */
typedef enum {
    DRV_VAU_JOB_NONE_NOTIFY = 0,
    DRV_VAU_JOB_COMPL_NOTIFY,
    DRV_VAU_JOB_WAKE_NOTIFY,
    DRV_VAU_JOB_NOTIFY_MAX
} drv_vau_notify_mode;

typedef enum {
    VAU_JOB_TYPE_DISPLAY = 0,
    VAU_JOB_TYPE_RENDER,
    VAU_JOB_TYPE_MAX
} vau_job_type;

typedef enum {
    DRV_VAU_INT_NONE          = 0x0,
    DRV_VAU_INT_NODE_COMPLETE = 0x1,
    DRV_VAU_INT_TIMEOUT       = 0x2,
    DRV_VAU_INT_BUS_ERROR     = 0x4,
    DRV_VAU_INT_LIST_END      = 0x8,
    DRV_VAU_INT_DISPLAY_DONE  = 0x10,
    DRV_VAU_INT_TUNL_DONE     = 0x20,
    DRV_VAU_INT_CONFLICT      = 0x40,
    DRV_VAU_INT_DRAW_ERR      = 0x80,
    DRV_VAU_INT_ET_OVERSIZE   = 0x100,
    DRV_VAU_INT_DRAW_OVERSIZE = 0x200,
    DRV_VAU_INT_DCMP_CFG_ERR  = 0x400,
    DRV_VAU_INT_DCMP_BS_ERR   = 0x800,
    DRV_VAU_INT_ONE_DRAW_ERR      = 0x1000,
    DRV_VAU_INT_ONE_ET_OVERSIZE   = 0x2000,
    DRV_VAU_INT_ONE_DRAW_OVERSIZE = 0x4000,
    DRV_VAU_INT_ONE_DCMP_CFG_ERR  = 0x8000,
    DRV_VAU_INT_ONE_DCMP_BS_ERR   = 0x10000,

    DRV_VAU_INT_MAX               = 0x20000
} vau_int_type;

typedef struct {
    struct osal_list_head list_head;
    td_bool has_submitted;
    td_bool is_wait;
    td_u8 wait_for_done_count;
    td_u32 node_num;
    td_handle handle;
    td_void *private_data;
    const td_void *param;
    vau_job_type job_type;
    vau_int_type int_type;
    drv_vau_notify_mode notify_type; /* notice type after node completed */
    vau_sw_node *first_cmd;          /* software node of first instruct in job */
    vau_sw_node *last_cmd;           /* software node of last instruct in job  */
    vau_sw_node *tail_node;          /* last software node of job */
    osal_wait queue;
    drv_vau_finish_callback finish_callback;
    drv_vau_lowdelay_callback lowdelay_callback;
} vau_sw_job;

typedef struct {
    td_handle handle_finished;  /* job handle last completed */
    td_u32 job_wait_start;      /* job number in queue */
    td_u32 job_total_num;       /* job total number */
    td_u32 job_delay_submit;    /* vau busy, delay submit in interrupt */
    td_u32 job_resume_num;      /* resume num when timeout */
    td_u32 workqueen_start;
    td_u32 workqueen_end;
    td_u32 software_timeout;
    td_bool no_reset;
    td_u32 timeout_reg[DRV_VAU_TIMEOUT_REG_SIZE];
} vau_job_dbg;

typedef struct {
    struct osal_list_head list_head;
    td_u32 hw_node_addr_worked;
    td_handle last_handle;
    vau_sw_job *last_job[VAU_JOB_TYPE_MAX];
    vau_sw_job *to_commit_jobs[VAU_JOB_TYPE_MAX];
    vau_sw_job *committed_jobs;
    osal_spinlock lock;
    vau_job_dbg dbg;
} vau_sw_job_mgr;

td_s32 vau_list_init(td_void);
td_void vau_list_deinit(td_void);

td_s32 vau_list_beg_job(td_handle *handle, td_void *private_data);
td_s32 vau_list_cancel_job(td_handle handle);
td_s32 vau_list_set_node_finish(td_handle handle, vau_sw_node *cmd, td_bool is_display_node);

td_s32 vau_list_submit_job(td_handle handle, td_u32 time_out,
    const drv_vau_callback_func *callback_func, drv_vau_notify_mode notify_type);

td_s32 vau_list_wait_all_done(td_void);
td_s32 vau_list_wait_for_done(td_handle handle, td_u32 time_out);
td_void vau_list_reset(td_void);

td_bool vau_is_err_int(td_u32 int_status);
td_void vau_list_end_process(td_void);
td_void vau_list_node_end_process(td_u32 int_status);
td_void vau_list_node_lowdelay_ready(td_void);

td_void vau_list_free_pending_job(const td_void *private_data);
td_void vau_osi_list_free_node(vau_sw_node *node);

td_void vau_osi_list_proc(void);
vau_job_dbg *vau_list_get_dbg(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHIC_VAU_LIST_VAU_LIST_H */
