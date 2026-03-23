/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau list manage
 */

#include "vau_list.h"
#include "vau_dfx.h"
#include "vau_mem.h"
#include "vau_handle.h"
#include "vau_hal.h"
#include "vau_hal_apb.h"
#include "drv_vau.h"

static osal_wait g_vau_block_job_wq; /* wait queue used to block */
static vau_sw_job_mgr *g_vau_sw_job_list;

static td_bool g_vau_working_flag = 0;

static td_void vau_osi_list_safe_destroy_job(vau_sw_job *job);
static td_void vau_osi_list_release_handle(const vau_handle_mgr *handle_mgr);
static td_void vau_osi_list_add_job(vau_sw_job *job);
static td_void vau_osi_list_destroy_job(vau_sw_job *job);

static td_void vau_osi_list_safe_destroy_job(vau_sw_job *job)
{
    /* if user query this job, release job in query function */
    if (job->wait_for_done_count != 0) {
        job->notify_type = DRV_VAU_JOB_NOTIFY_MAX;
        osal_wait_wakeup(&job->queue);
    } else {
        vau_osi_list_destroy_job(job);
    }
    return;
}

static td_void vau_osi_list_release_handle(const vau_handle_mgr *handle_mgr)
{
    if (handle_mgr == TD_NULL) {
        return;
    }

    vau_handle_del(handle_mgr->handle);

    return;
}

td_s32 vau_list_init(td_void)
{
    osal_wait_init(&g_vau_block_job_wq);
    if (vau_handle_list_init() != EXT_SUCCESS) {
        vau_err("call vau_handle_list_init failed");
        return EXT_FAILURE;
    }

    g_vau_sw_job_list = (vau_sw_job_mgr *)osal_kmalloc(sizeof(vau_sw_job_mgr), OSAL_GFP_KERNEL);
    if (g_vau_sw_job_list == TD_NULL) {
        vau_handle_list_deinit();
        vau_err("call osal_kmalloc failed");
        return EXT_FAILURE;
    }
    (td_void)memset_s(g_vau_sw_job_list, sizeof(*g_vau_sw_job_list), 0x0, sizeof(vau_sw_job_mgr));

    OSAL_INIT_LIST_HEAD(&g_vau_sw_job_list->list_head);

    osal_spin_lock_init(&g_vau_sw_job_list->lock);

    return EXT_SUCCESS;
}

td_void vau_osi_list_free_node(vau_sw_node *node)
{
    vau_check_nullpointer_return_novalue(node);
    osal_list_del_init(&node->list_head);
    vau_hal_free_vector_mem(&node->hw_node);
    vau_mem_free(VAU_MEM_TYPE_NODE, (td_uchar *)node);
    return;
}

static td_void vau_list_free_serial_cmd(vau_sw_node *first_cmd, vau_sw_node *last_cmd)
{
    vau_sw_node *next_cmd = TD_NULL;
    vau_sw_node *cur_cmd = TD_NULL;

    if ((first_cmd == TD_NULL) || (last_cmd == TD_NULL)) {
        return;
    }

    cur_cmd = next_cmd = first_cmd;

    while (next_cmd != last_cmd) {
        next_cmd = osal_list_entry(cur_cmd->list_head.next, vau_sw_node, list_head);

        vau_osi_list_free_node(cur_cmd);

        if (next_cmd == TD_NULL) {
            break;
        }

        cur_cmd = next_cmd;
    }

    vau_osi_list_free_node(last_cmd);

    return;
}

td_void vau_list_deinit(td_void)
{
    vau_sw_job *job = TD_NULL;

    vau_check_nullpointer_return_novalue(g_vau_sw_job_list);

    while (osal_list_empty(&g_vau_sw_job_list->list_head) == 0) {
        job = osal_list_entry(g_vau_sw_job_list->list_head.next, vau_sw_job, list_head);
        vau_osi_list_destroy_job(job);
    }

    osal_spin_lock_destroy(&g_vau_sw_job_list->lock);
    osal_wait_destroy(&g_vau_block_job_wq);

    osal_kfree(g_vau_sw_job_list);
    g_vau_sw_job_list = TD_NULL;

    vau_handle_list_deinit();

    return;
}

static td_void vau_osi_list_flush_node(const vau_sw_node *hardware_node)
{
    uapi_unused(hardware_node);

    return;
}

static td_void vau_osi_list_flush_job(const vau_sw_job *job)
{
    vau_sw_node *next_cmd = TD_NULL;
    vau_sw_node *cur_cmd = TD_NULL;
    vau_sw_node *first_cmd = TD_NULL;
    vau_sw_node *last_cmd = TD_NULL;

    vau_check_nullpointer_return_novalue(job);
    first_cmd = job->first_cmd;
    last_cmd = job->tail_node;
    vau_check_nullpointer_return_novalue(first_cmd);
    vau_check_nullpointer_return_novalue(last_cmd);

    cur_cmd = next_cmd = first_cmd;
    while (next_cmd != last_cmd) {
        next_cmd = osal_list_entry(cur_cmd->list_head.next, vau_sw_node, list_head);
        vau_osi_list_flush_node(cur_cmd);
        cur_cmd = next_cmd;
    }

    vau_osi_list_flush_node(last_cmd);

    return;
}

static td_void vau_osi_list_add_job(vau_sw_job *job)
{
    osal_list_add_tail(&job->list_head, &g_vau_sw_job_list->list_head);
    g_vau_sw_job_list->dbg.job_wait_start++;
    g_vau_sw_job_list->dbg.job_total_num++;
    g_vau_sw_job_list->last_handle = job->handle;
    g_vau_sw_job_list->last_job[job->job_type] = job;
}

td_s32 vau_list_beg_job(td_handle *handle, td_void *private_data)
{
    vau_sw_job *job = TD_NULL;
    vau_handle_mgr *handle_mgr = TD_NULL;
    td_u32 size = (td_u32)(sizeof(vau_handle_mgr) + sizeof(vau_sw_job));
    vau_check_nullpointer_return_value(handle, EXT_FAILURE);

    handle_mgr = vau_handle_create(size);
    if (handle_mgr == TD_NULL) {
        vau_err("call vau_handle_create failed");
        return DRV_ERR_VAU_NO_MEM;
    }
    handle_mgr->res = (td_void *)((td_u8 *)handle_mgr + sizeof(vau_handle_mgr));
    *handle = handle_mgr->handle;

    job = (vau_sw_job *)handle_mgr->res;
    job->handle       = *handle;
    job->job_type     = VAU_JOB_TYPE_RENDER;
    job->private_data = private_data;

    osal_wait_init(&job->queue);

    OSAL_INIT_LIST_HEAD(&job->list_head);

    return EXT_SUCCESS;
}

static td_void vau_osi_list_destroy_job(vau_sw_job *job)
{
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = vau_handle_search(job->handle);
    if (handle_mgr == TD_NULL) {
        vau_err("vau_handle_search:%u failed", job->handle);
        return;
    }

    vau_list_free_serial_cmd(job->first_cmd, job->tail_node);
    osal_list_del_init(&job->list_head);
    osal_wait_destroy(&job->queue);
    vau_osi_list_release_handle(handle_mgr);

    return;
}

td_s32 vau_list_cancel_job(td_handle handle)
{
    vau_handle_mgr *handle_mgr = TD_NULL;
    vau_sw_job *job = TD_NULL;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return EXT_SUCCESS;
    }
    job = (vau_sw_job *)handle_mgr->res;

    vau_list_free_serial_cmd(job->first_cmd, job->tail_node);
    osal_list_del_init(&job->list_head);
    osal_wait_destroy(&job->queue);
    vau_osi_list_release_handle(handle_mgr);

    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);

    return EXT_SUCCESS;
}

static td_s32 vau_osal_wait_check(const td_void *data)
{
    if (*(td_s32 *)data == DRV_VAU_JOB_NOTIFY_MAX) {
        return 1;
    }
    return 0;
}

static td_void vau_list_set_job(vau_sw_job *job, const drv_vau_callback_func *pfunc, drv_vau_notify_mode notify_type)
{
    vau_hardware_node *hardware_node = &(job->tail_node->hw_node);

    vau_node_enable_complete_int(hardware_node);

    if (pfunc->is_lowdelay == TD_TRUE) {
        vau_node_enable_lowdelay_int(hardware_node);
        vau_node_set_tunl_enable(hardware_node);
    }

    job->has_submitted     = TD_TRUE;
    job->notify_type       = notify_type;
    job->param             = pfunc->param;
    job->finish_callback   = pfunc->finish_callback;
    job->lowdelay_callback = pfunc->lowdelay_callback;
}

static td_void vau_job_to_commit(vau_sw_job *job)
{
    if ((job->job_type < VAU_JOB_TYPE_DISPLAY) || (job->job_type >= VAU_JOB_TYPE_MAX)) {
        return;
    }
    /* if the job to commit is not null,join the current job to the tail node of the last job. */
    if (g_vau_sw_job_list->to_commit_jobs[job->job_type] != TD_NULL) {
        vau_sw_node *tail_node_in_job_list = g_vau_sw_job_list->last_job[job->job_type]->tail_node;

        vau_node_add_next_node(&tail_node_in_job_list->hw_node, job->first_cmd->phy_addr);

        /* 将job中的对应的所有 hw 节点flush到内存，保证硬件能正确访问 */
        vau_osi_list_flush_job(g_vau_sw_job_list->last_job[job->job_type]);
    } else {
        g_vau_sw_job_list->to_commit_jobs[job->job_type] = job;
    }
    /* 将job中的对应的所有 hw 节点flush到内存，保证硬件能正确访问 */
    vau_osi_list_flush_job(job);
    vau_osi_list_add_job(job);
    return;
}

static td_void vau_set_job_wake_notify(drv_vau_notify_mode *notify_type, vau_sw_job *job, td_u32 *time_out,
    td_bool *asynflag)
{
    if (*notify_type != DRV_VAU_JOB_WAKE_NOTIFY) {
        if ((!(td_bool)osal_in_interrupt()) && (vau_mem_free_count(VAU_MEM_TYPE_NODE) < 5)) { /* free 5 nodes */
            job->notify_type = DRV_VAU_JOB_WAKE_NOTIFY;
            *notify_type = DRV_VAU_JOB_WAKE_NOTIFY;
            *time_out = 1000; /* 1000ms */
            *asynflag = 1;    /* 由非阻塞方式转为阻塞方式标志位 */
            vau_dbg("notify type change to DRV_VAU_JOB_WAKE_NOTIFY");
        }
    }
    return;
}

static td_void vau_list_submit_first_cmd(vau_job_type type)
{
    td_s32 ret;
    vau_dfx_info *vau_info = vau_dfx_get_info();

    if (g_vau_working_flag == 0) {
        if ((g_vau_sw_job_list->to_commit_jobs[type] == TD_NULL) ||
            (g_vau_sw_job_list->to_commit_jobs[type]->first_cmd == TD_NULL)) {
            return;
        }

        ret = vau_hal_execute_node(g_vau_sw_job_list->to_commit_jobs[type]->first_cmd->phy_addr);
        if (ret == EXT_SUCCESS) {
            g_vau_sw_job_list->committed_jobs = g_vau_sw_job_list->to_commit_jobs[type];
            g_vau_sw_job_list->to_commit_jobs[type] = TD_NULL;
            g_vau_sw_job_list->hw_node_addr_worked = 0x0;
            g_vau_working_flag = 1;
            vau_info->func_info.func_cnt.execute_node_cnt++;
            vau_info->func_info.func_cnt.submit_from_user++;
        }
    } else {
        g_vau_sw_job_list->dbg.job_delay_submit++;
    }
    return;
}

static td_void vau_list_timeout_resume(td_void)
{
    vau_sw_job *job = TD_NULL;

    g_vau_sw_job_list->dbg.software_timeout++;
    if (memcpy_s((td_void *)g_vau_sw_job_list->dbg.timeout_reg, sizeof(g_vau_sw_job_list->dbg.timeout_reg),
                 (td_void *)DRV_VAU_TIMEOUT_DBG_ADDR, sizeof(g_vau_sw_job_list->dbg.timeout_reg)) != EOK) {
        vau_err("memcpy_s failed");
    }
    if (g_vau_sw_job_list->dbg.software_timeout < 4) { /* 4:index */
        for (td_u32 i = 0; i < DRV_VAU_TIMEOUT_REG_SIZE / 4; i++) {   /* 4:index */
            vau_print("%04x:  %08x %08x %08x %08x\n", (i * 16),       /* 16:index */
                      g_vau_sw_job_list->dbg.timeout_reg[i * 4 + 0],  /* 4:index */
                      g_vau_sw_job_list->dbg.timeout_reg[i * 4 + 1],  /* 4:index */
                      g_vau_sw_job_list->dbg.timeout_reg[i * 4 + 2],  /* 4,2:index */
                      g_vau_sw_job_list->dbg.timeout_reg[i * 4 + 3]); /* 4,3:index */
        }
        vau_mem_check_psram_addr();
        vau_dfx_proc_read(0, TD_NULL);
    }

    if (!g_vau_sw_job_list->dbg.no_reset) {
        vau_hal_init();
    } else {
        vau_err("do not reset when timeout\n");
    }

    g_vau_working_flag = 0;
    while (osal_list_empty(&g_vau_sw_job_list->list_head) == 0) {
        job = osal_list_entry(g_vau_sw_job_list->list_head.next, vau_sw_job, list_head);
        vau_osi_list_destroy_job(job);
        g_vau_sw_job_list->dbg.job_resume_num++;
    }
    return;
}

static td_s32 vau_list_wake_up_job(vau_sw_job *job, td_bool asynflag, const td_s32 *ret)
{
    if (job->notify_type == DRV_VAU_JOB_NOTIFY_MAX) {
        td_u32 int_status = job->int_type;
        td_handle handle = job->handle;
        if (job->wait_for_done_count == 0) {
            vau_osi_list_destroy_job(job);
        }
        if (vau_is_err_int(int_status)) {
            vau_err("handle %u error int : 0x%x", handle, int_status);
            return DRV_ERR_VAU_INT_ERR;
        }
        return EXT_SUCCESS;
    } else {
        job->notify_type = DRV_VAU_JOB_COMPL_NOTIFY;
        if (*ret == (-OSAL_ERESTARTSYS)) {
            vau_err("handle interrupt");
            return DRV_ERR_VAU_INTERRUPT;
        }
        if (asynflag == 1) {
            /* 如果由非阻塞方式转为阻塞方式则无超时信息 */
            return EXT_SUCCESS;
        }
        vau_err("handle %u timeout, running resume...", job->handle);
        vau_list_timeout_resume();
        return DRV_ERR_VAU_WAIT_TIMEOUT;
    }
}

static td_s32 vau_list_has_task(td_handle handle, vau_sw_job **return_job)
{
    vau_handle_mgr *handle_mgr = TD_NULL;
    vau_sw_job *job = TD_NULL;

    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        return EXT_FAILURE;
    }

    job = (vau_sw_job *)handle_mgr->res;
    if (job == NULL) {
        return EXT_FAILURE;
    }

    if (job->has_submitted == TD_TRUE) {
        return EXT_FAILURE;
    }

    if (job->first_cmd == TD_NULL) {
        return EXT_FAILURE;
    }

    if (job->tail_node == TD_NULL) {
        return EXT_FAILURE;
    }

    *return_job = job;

    return EXT_SUCCESS;
}

td_s32 vau_list_submit_job(td_handle handle, td_u32 time_out,
                           const drv_vau_callback_func *callback_func,
                           drv_vau_notify_mode notify_type)
{
    vau_sw_job *job = TD_NULL;
    td_s32 wait_ret, ret;
    td_size_t lockflags = 0;
    td_bool asynflag = 0;
    td_u32 tmp_time_out = time_out;

    if ((tmp_time_out == 0) || (tmp_time_out > 2000)) { /* 2000ms */
        tmp_time_out = 2000;                        /* max 2000ms */
    }
    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);

    ret = vau_list_has_task(handle, &job);
    if (ret != EXT_SUCCESS) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return EXT_SUCCESS;
    }

    vau_list_set_job(job, callback_func, notify_type);
    vau_job_to_commit(job);
    vau_set_job_wake_notify(&notify_type, job, &tmp_time_out, &asynflag);
    vau_list_submit_first_cmd(job->job_type);
    if (notify_type == DRV_VAU_JOB_WAKE_NOTIFY) {
        job->wait_for_done_count++;
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        wait_ret = osal_wait_timeout_interruptible(&g_vau_block_job_wq, vau_osal_wait_check,
                                                   (td_void *)&job->notify_type, tmp_time_out);
        osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
        job->wait_for_done_count--;
        ret = vau_list_wake_up_job(job, asynflag, &wait_ret);
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return ret;
    }
    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
    return EXT_SUCCESS;
}

td_s32 vau_list_wait_all_done(td_void)
{
    td_s32 ret;
    td_u32 wait_handle;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
    wait_handle = g_vau_sw_job_list->last_handle;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
    if (wait_handle == VAU_HANDLE_INVALID) {
        return EXT_SUCCESS;
    }

    ret = vau_list_wait_for_done(wait_handle, 1000); /* timeout 1000 */

    return ret;
}

td_s32 vau_list_wait_for_done(td_handle handle, td_u32 time_out)
{
    vau_sw_job *job = TD_NULL;
    vau_handle_mgr *handle_mgr = TD_NULL;
    td_s32 ret;
    td_size_t lockflags = 0;
    td_u32 tmp_time_out = time_out;

    if ((tmp_time_out == 0) || (tmp_time_out > 1000)) { /* 1000ms */
        tmp_time_out = 1000;                        /* max 1000ms */
    }

    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return EXT_SUCCESS;
    }
    job = (vau_sw_job *)handle_mgr->res;
    if (!job->has_submitted) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return DRV_ERR_VAU_INVALID_HANDLE;
    }
    job->is_wait = TD_TRUE;
    job->wait_for_done_count++;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);

    ret = osal_wait_timeout_interruptible(&job->queue, vau_osal_wait_check,
        (td_void *)&job->notify_type, tmp_time_out);
    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
    job->wait_for_done_count--;

    if (job->notify_type != DRV_VAU_JOB_NOTIFY_MAX) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        vau_err("handle[%u] timeout, ret[%d]", job->handle, ret);
        vau_list_timeout_resume();
        return DRV_ERR_VAU_QUERY_TIMEOUT;
    }

    /* complete */
    if (job->wait_for_done_count == 0) {
        vau_osi_list_destroy_job(job);
    }

    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);

    return EXT_SUCCESS;
}

td_void vau_list_end_process(td_void)
{
    td_s32 ret;
    td_u32 type;
    vau_dfx_info *vau_info = vau_dfx_get_info();

#ifdef CONFIG_VAU_DFX_DEBUG
    if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_END_REG)) {
        vau_hal_apb_print_all_reg();
    }
#endif
    for (type = (td_u32)VAU_JOB_TYPE_DISPLAY; type < (td_u32)VAU_JOB_TYPE_MAX; type++) {
        if ((g_vau_sw_job_list->to_commit_jobs[type] != TD_NULL) &&
            (g_vau_sw_job_list->to_commit_jobs[type]->first_cmd != TD_NULL)) {
            ret = vau_hal_execute_node(g_vau_sw_job_list->to_commit_jobs[type]->first_cmd->phy_addr);
            if (ret == EXT_SUCCESS) {
                g_vau_sw_job_list->committed_jobs = g_vau_sw_job_list->to_commit_jobs[type];
                g_vau_sw_job_list->to_commit_jobs[type] = TD_NULL;
                g_vau_sw_job_list->hw_node_addr_worked = 0x0;
                vau_info->func_info.func_cnt.execute_node_cnt++;
                vau_info->func_info.func_cnt.submit_from_int++;
            }
            break;
        }
    }

    return;
}

static td_s32 vau_list_comp_delete_job(td_bool is_work, td_handle finished_handle,
                                       td_size_t *lockflags, vau_int_type int_type)
{
    td_handle delhandle;
    vau_sw_job *del_job = TD_NULL;
    uapi_unused(is_work);
    while (osal_list_empty(&g_vau_sw_job_list->list_head) == 0) {
        del_job = osal_list_entry(g_vau_sw_job_list->list_head.next, vau_sw_job, list_head);
        delhandle = del_job->handle;
        g_vau_sw_job_list->dbg.job_wait_start--;
        if (delhandle == g_vau_sw_job_list->last_handle) {
            g_vau_sw_job_list->last_handle = VAU_HANDLE_INVALID;
        }
        osal_list_del_init(&del_job->list_head);
        del_job->int_type = int_type;
        if (del_job->notify_type == DRV_VAU_JOB_WAKE_NOTIFY) {
            del_job->notify_type = DRV_VAU_JOB_NOTIFY_MAX;
            if (del_job->finish_callback != TD_NULL) {
                osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, lockflags);
                del_job->finish_callback(del_job->param);
                osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, lockflags);
            }
            if (del_job->is_wait) {
                osal_wait_wakeup(&del_job->queue);
            }
            osal_wait_wakeup(&g_vau_block_job_wq);
        } else if (del_job->notify_type == DRV_VAU_JOB_COMPL_NOTIFY) {
            if (del_job->finish_callback != TD_NULL) {
                osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, lockflags);
                del_job->finish_callback(del_job->param);
                osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, lockflags);
            }
            vau_osi_list_safe_destroy_job(del_job);
        } else {
            vau_err("error status!!");
        }
        if (delhandle == finished_handle) {
            return EXT_SUCCESS;
        }
    }
    return EXT_FAILURE;
}

static td_s32 vau_list_comp_get_finished_handle(td_handle *finished_handle, const vau_handle_mgr *p_handle_mgr)
{
    vau_sw_job *job = TD_NULL;
    vau_sw_job *del_job = TD_NULL;
    job = (vau_sw_job *)p_handle_mgr->res;

    if (job->list_head.prev == &g_vau_sw_job_list->list_head) {
        vau_err("no pre job left, finished handle:%u", *finished_handle);
        return EXT_FAILURE;
    }

    del_job = osal_list_entry(job->list_head.prev, vau_sw_job, list_head);
    *finished_handle = del_job->handle;
    g_vau_sw_job_list->hw_node_addr_worked = 0x0;
    return EXT_SUCCESS;
}

td_bool vau_is_err_int(td_u32 int_status)
{
    td_u32 err_int = DRV_VAU_INT_TIMEOUT | DRV_VAU_INT_BUS_ERROR |
        DRV_VAU_INT_DRAW_ERR | DRV_VAU_INT_ET_OVERSIZE | DRV_VAU_INT_DRAW_OVERSIZE |
        DRV_VAU_INT_DCMP_CFG_ERR | DRV_VAU_INT_DCMP_BS_ERR |
        DRV_VAU_INT_ONE_DRAW_ERR | DRV_VAU_INT_ONE_ET_OVERSIZE | DRV_VAU_INT_ONE_DRAW_OVERSIZE |
        DRV_VAU_INT_ONE_DCMP_CFG_ERR | DRV_VAU_INT_ONE_DCMP_BS_ERR;
    if ((int_status & err_int) != 0) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

td_void vau_list_node_end_process(td_u32 int_status)
{
    vau_handle_mgr *handle_mgr = TD_NULL;
    td_handle finished_handle;
    td_size_t lockflags = 0;
    vau_sw_node *sw_node = TD_NULL;
    vau_hardware_node *hardware_node = TD_NULL;
    td_bool is_error_int = vau_is_err_int(int_status);

    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);
    g_vau_sw_job_list->dbg.workqueen_start++;
    td_bool is_work = (vau_hal_check_is_idle() == TD_TRUE) ? TD_FALSE : TD_TRUE;
    td_u32 running_hw_node_addr = vau_hal_get_current_node();

    if (is_error_int) {
#ifdef CONFIG_VAU_DFX_DEBUG
        if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_ERROR_REG)) {
            vau_hal_apb_print_all_reg();
        }
#endif
        if (!g_vau_sw_job_list->dbg.no_reset) {
            vau_hal_init();
        }
    }

    if ((running_hw_node_addr == 0) || (running_hw_node_addr == g_vau_sw_job_list->hw_node_addr_worked)) {
        vau_err("vau get cur node fail");
        goto unlock;
    }
    hardware_node = (vau_hardware_node *)vau_mem_map_to_viraddr(VAU_MEM_TYPE_NODE, running_hw_node_addr);
    sw_node = osal_container_of(hardware_node, vau_sw_node, hw_node);
    finished_handle = sw_node->handle;
    handle_mgr = vau_handle_search(finished_handle);
    if (handle_mgr == TD_NULL) {
        goto unlock;
    }

    if (!is_work || is_error_int) {
        g_vau_sw_job_list->hw_node_addr_worked = running_hw_node_addr;
    } else {
        if (vau_list_comp_get_finished_handle(&finished_handle, handle_mgr) < 0) {
            goto unlock;
        }
    }
    g_vau_sw_job_list->dbg.handle_finished = finished_handle;

    if (vau_list_comp_delete_job(is_work, finished_handle, &lockflags, int_status) == EXT_SUCCESS) {
        vau_list_end_process();
    }

unlock:
    if (vau_hal_check_is_idle()) {
        g_vau_working_flag = 0;
    }
    g_vau_sw_job_list->dbg.workqueen_end++;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
}

td_void vau_list_node_lowdelay_ready(td_void)
{
    vau_sw_job *lowdelay_job = TD_NULL;

    while (osal_list_empty(&g_vau_sw_job_list->list_head) == 0) {
        lowdelay_job = osal_list_entry(g_vau_sw_job_list->list_head.next, vau_sw_job, list_head);
        if (lowdelay_job->lowdelay_callback == TD_NULL) {
            continue;
        }

        lowdelay_job->lowdelay_callback(lowdelay_job->param);
        break;
    }

    return;
}

td_void vau_list_free_pending_job(const td_void *private_data)
{
    vau_handle_mgr *handle_mgr = TD_NULL;
    vau_handle_mgr *handle_mgr_save = TD_NULL;
    vau_sw_job *job = TD_NULL;
    td_size_t lockflags = 0;
    vau_handle_mgr *vau_handle_list = TD_NULL;

    vau_handle_list = vau_handle_list_get();
    vau_check_nullpointer_return_novalue(vau_handle_list);

    osal_spin_lock_irqsave(&g_vau_sw_job_list->lock, &lockflags);

    handle_mgr = osal_list_entry(vau_handle_list->list_head.next, vau_handle_mgr, list_head);
    if (handle_mgr == TD_NULL) {
        vau_err("no pending job!");
        osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
        return;
    }

    while (handle_mgr != vau_handle_list) {
        handle_mgr_save = handle_mgr;
        job = (vau_sw_job *)handle_mgr_save->res;
        if (job == TD_NULL) {
            vau_err("job null pointer!");
            osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);
            return;
        }

        handle_mgr = osal_list_entry(handle_mgr->list_head.next, vau_handle_mgr, list_head);

        /* free when it is not submitted */
        if ((!job->has_submitted) && (private_data == job->private_data)) {
            /* free handle resource */
            vau_list_free_serial_cmd(job->first_cmd, job->tail_node);
            osal_list_del_init(&handle_mgr_save->list_head);
            vau_mem_free(VAU_MEM_TYPE_HANDLE_MNG, (td_uchar *)handle_mgr_save);
        }

        if (handle_mgr == TD_NULL) {
            break;
        }
    }

    osal_spin_unlock_irqrestore(&g_vau_sw_job_list->lock, &lockflags);

    return;
}

static vau_sw_job *vau_osi_get_sw_job(td_handle handle)
{
    vau_handle_mgr *handle_mgr = TD_NULL;
    vau_sw_job *job = TD_NULL;

    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        vau_err("vau_handle_search %u failed", handle);
        return TD_NULL;
    }

    job = (vau_sw_job *)handle_mgr->res;
    if (job->has_submitted) {
        vau_err("handle[%u],jobhandle[%u] has_submitted[%u], int_type[%d]",
                handle, job->handle, job->has_submitted, job->int_type);
        return TD_NULL;
    }
    return job;
}

static td_void vau_osi_set_node_buf(const vau_sw_job *job, vau_sw_node *cmd)
{
    cmd->phy_addr = vau_mem_map_to_phyaddr(VAU_MEM_TYPE_NODE, (td_u8 *)&cmd->hw_node);
    if (job->tail_node != TD_NULL) {
        vau_node_add_next_node(&job->tail_node->hw_node, cmd->phy_addr);
    }
}

td_s32 vau_list_set_node_finish(td_handle handle, vau_sw_node *cmd, td_bool is_display_node)
{
    vau_sw_job *job = TD_NULL;

    job = vau_osi_get_sw_job(handle);
    if (job == TD_NULL) {
        vau_err("call vau_osi_get_sw_job failed");
        return DRV_ERR_VAU_INVALID_HANDLE;
    }

    job->node_num++;
    if (job->node_num == 1) {
        job->first_cmd = cmd;
        OSAL_INIT_LIST_HEAD(&cmd->list_head);
    }

    cmd->handle = job->handle;
    job->last_cmd = cmd;
    vau_osi_set_node_buf(job, cmd);

    osal_list_add_tail(&cmd->list_head, &job->first_cmd->list_head);
    job->tail_node = cmd;

    if (is_display_node) {
        job->job_type = VAU_JOB_TYPE_DISPLAY;
    }

    return EXT_SUCCESS;
}

#ifdef CONFIG_GFX_PROC_SUPPORT
td_void vau_osi_list_proc(void)
{
    if (g_vau_sw_job_list != TD_NULL) {
        vau_print("last finished handle     :%u\n", g_vau_sw_job_list->dbg.handle_finished);
        vau_print("job create num           :%u\n", g_vau_sw_job_list->dbg.job_total_num);
        vau_print("job submit(delay)        :%u\n", g_vau_sw_job_list->dbg.job_delay_submit);
        vau_print("job unfinish             :%u\n", g_vau_sw_job_list->dbg.job_wait_start);
        vau_print("job resume num           :%u\n", g_vau_sw_job_list->dbg.job_resume_num);
        vau_print("job timeout num          :%u\n", g_vau_sw_job_list->dbg.software_timeout);
        vau_print("workqueen start          :%u\n", g_vau_sw_job_list->dbg.workqueen_start);
        vau_print("workqueen end            :%u\n", g_vau_sw_job_list->dbg.workqueen_end);

        vau_print("timeout debug: ------------------------------------\n");
        if (g_vau_sw_job_list->dbg.software_timeout != 0) {
            td_u32 *addr = (td_u32 *)g_vau_sw_job_list->dbg.timeout_reg;
            td_u32 size = (td_u32)sizeof(g_vau_sw_job_list->dbg.timeout_reg) / 16; /* 16: 1 line 16bytes */
            for (td_u32 i = 0; i < size; i++) {
                vau_print("%04x:  %08x %08x %08x %08x\n", (DRV_VAU_TIMEOUT_DBG_ADDR + i * 16), /* 16:index */
                          addr[i * 4], addr[i * 4 + 1], addr[i * 4 + 2], addr[i * 4 + 3]); /* 2,3,4:index */
            }
        }
    }
    return;
}

vau_job_dbg *vau_list_get_dbg(td_void)
{
    return &g_vau_sw_job_list->dbg;
}
#endif

